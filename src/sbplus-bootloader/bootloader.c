/*************************************************************************
Title:    SoundBytes Plus Bootloader
Authors:  Michael Petersen <railfan@drgw.net>
          Nathan D. Holmes <maverick@drgw.net>
File:     $Id: $
License:  GNU General Public License v3

LICENSE:
    Copyright (C) 2023 Michael Petersen, Nathan Holmes, with portions from 
     David Johson-Davies under a Creative Commons Attribution 4.0 license

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

*************************************************************************/

#include <avr/io.h>
#include <avr/boot.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <stdbool.h>
#include <stdint.h>
#include "string.h"
#include "spiflash.h"

#define PAGE_SZ_BYTES    64

#define ISPL_HEADER_IDENTIFIER_ADDR   0x00000000
#define ISPL_HEADER_IDENTIFIER_LEN    4
#define ISPL_HEADER_IDENTIFIER        "ISPL"

#define ISPL_MANIFEST_BASE_ADDR       0x00000008

#define ISPL_MANIFEST_REC_ADDR_OFFSET 0
#define ISPL_MANIFEST_REC_N_OFFSET    4
#define ISPL_MANIFEST_REC_S_OFFSET    8
#define ISPL_MANIFEST_REC_CRC_OFFSET  10

#define ISPL_TABLE_MANIFEST           0
#define ISPL_TABLE_PROGRAM            1
#define ISPL_TABLE_AUDIO              2

#define EEPROM_PROG_CRC32_BASE      508

typedef struct
{
	uint32_t baseAddr;
	uint32_t n;
	uint16_t s;
} ISPLTable;

bool isplTableLoad(ISPLTable* t, uint8_t tableNum)
{
	uint32_t n;
	uint32_t s;

	n = spiflashReadU32(ISPL_MANIFEST_BASE_ADDR + ISPL_MANIFEST_REC_N_OFFSET);
	s = spiflashReadU16(ISPL_MANIFEST_BASE_ADDR + ISPL_MANIFEST_REC_S_OFFSET);

	t->baseAddr = 0;
	t->n = 0;
	t->s = 0;

	if (tableNum >= n)
		return false;
	
	t->baseAddr = spiflashReadU32(ISPL_MANIFEST_BASE_ADDR + tableNum * s + ISPL_MANIFEST_REC_ADDR_OFFSET);
	t->n = spiflashReadU32(ISPL_MANIFEST_BASE_ADDR + tableNum * s + ISPL_MANIFEST_REC_N_OFFSET);
	t->s = spiflashReadU16(ISPL_MANIFEST_BASE_ADDR + tableNum * s + ISPL_MANIFEST_REC_S_OFFSET);

	return true;
}


bool isplInitialize(ISPLTable* isplProgramTable)
{
	uint8_t buffer[8];
	spiflashReadBlock(ISPL_HEADER_IDENTIFIER_ADDR, ISPL_HEADER_IDENTIFIER_LEN, buffer);
	if (0 != memcmp(buffer, ISPL_HEADER_IDENTIFIER, ISPL_HEADER_IDENTIFIER_LEN))
		return false;  // Header doesn't contain correct starting record

	return isplTableLoad(isplProgramTable, ISPL_TABLE_PROGRAM);
}

int main ( void )
{
	// Deal with watchdog first thing
	MCUSR = 0;	// Clear reset status
	wdt_disable();
	cli();

	// Let's just get the outputs in a sane state, because we need to talk to the SPI flash
	//  and probably keep the amplifier dead so it doesn't make weird noises

	// PORT A
	//  PA7 - Output - /SHUTDOWN to amplifier
	//  PA6 - Output - No Connection
	//  PA5 - Input  - /EN2x (enable pullup)
	//  PA4 - Input  - /EN1x (enable pullup)	
	//  PA3 - Output - /CS to flash
	//  PA2 - Output - CLK to flash
	//  PA1 - Output - MOSI to flash
	//  PA0 - Input - MISO to flash (enable pullup)

	// PORT B
	//  PB7 - n/a    - /RESET (not I/O pin)
	//  PB6 - Output - No Connection
	//  PB5 - Output - No Connection
	//  PB4 - Output - No Connection
	//  PB3 - Output - Audio PWM Output
	//  PB2 - Output - (AVR programming SPI)
	//  PB1 - Output - (AVR programming SPI)
	//  PB0 - Output - (AVR programming SPI)
	

	PORTA = 0b00111001;
	DDRA  = 0b11001110;

	PORTB = 0b00000000; 	// Just make everything low
	DDRB  = 0b11111111;     // And set it as an output

	spiSetup();
	spiflashReset();

	ISPLTable isplProgramTable;

	if (true == isplInitialize(&isplProgramTable))
	{
		// At this point, we have a valid ISPL program image

		// Check if the CRC is the same as the one we have in eeprom
		uint32_t crc32_eeprom = eeprom_read_dword((const uint32_t*)EEPROM_PROG_CRC32_BASE);
		uint32_t crc32_spiflash = spiflashReadU32(isplProgramTable.baseAddr + isplProgramTable.n - 4);


		// If the program CRC isn't the same as what we have in flash, reflash the part
		if (crc32_eeprom != crc32_spiflash)
		{
			// The actual program size is 4 bytes less than reported because of the CRC on the end
			isplProgramTable.n -= 4; 

			eeprom_write_dword((uint32_t*)EEPROM_PROG_CRC32_BASE, 0xFFFFFFFF);
			eeprom_busy_wait();

			// Full program space erase (up to bootloader)
			uint16_t progmemAddr = 0;
			for (progmemAddr = PAGE_SZ_BYTES; progmemAddr < BOOTLOADER_ADDRESS; progmemAddr += PAGE_SZ_BYTES)
				boot_page_erase(progmemAddr);

			uint8_t pageBuffer[PAGE_SZ_BYTES];
			uint16_t bytesLeftToWrite = isplProgramTable.n;
			progmemAddr = 0;
			while (bytesLeftToWrite > 0)
			{
				uint8_t thisPageSz = min(PAGE_SZ_BYTES, bytesLeftToWrite); 

				spiflashReadBlock(isplProgramTable.baseAddr + progmemAddr, thisPageSz, pageBuffer);

				// If we're not getting a full page, fill with 0xFF
				if (thisPageSz < PAGE_SZ_BYTES)
					memset(pageBuffer + thisPageSz, 0xFF, PAGE_SZ_BYTES - thisPageSz);

				if (0 == progmemAddr)
				{
					//boot_page_fill(BOOTLOADER_ADDRESS - 2, 0xCFFF & (((fake_page[0] & 0x0FFF) - (BOOTLOADER_ADDRESS / 2) + 1) | 0xC000)); // jump to start
					// Preserve the current bootloader jump instruction
	//				tmp = pgm_read_word(BOOTLOADER_ADDRESS + CUR_ADDR); // get the original instruction inside bootloader
				}

				for(uint8_t i=0; i<PAGE_SZ_BYTES; i+=2)
				{
					uint16_t w = (uint16_t)pageBuffer[i] | ((uint16_t)pageBuffer[i+1]<<8);
					boot_page_fill_safe(progmemAddr + i, w);
				}

				boot_page_write_safe(progmemAddr);
				bytesLeftToWrite -= thisPageSz;
			}

			boot_spm_busy_wait();

			// Did we succeed?  Write eeprom crc
			eeprom_write_dword((uint32_t*)EEPROM_PROG_CRC32_BASE, crc32_spiflash);
			eeprom_busy_wait();
		}
	}

	// this will jump to user app
	asm volatile("rjmp (__vectors - 2)");
}
