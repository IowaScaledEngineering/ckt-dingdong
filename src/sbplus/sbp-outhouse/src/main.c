/*************************************************************************
Title:    SoundBytes Plus
Authors:  Michael Petersen <railfan@drgw.net>
          Nathan D. Holmes <maverick@drgw.net>
          Based on the work of David Johnson-Davies - www.technoblogy.com - 23rd October 2017
           and used under his Creative Commons Attribution 4.0 International license
File:     $Id: $
License:  GNU General Public License v3

CREDIT:
    The basic idea behind this playback design came from David Johson-Davies, who
    provided the basic framework and the place where I started.

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
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <stdbool.h>
#include <stdint.h>
#include "spiflash.h"
#include "audio.h"
#include "ispl.h"
#include "lfsr.h"
#include "debouncer.h"

uint8_t inputs = 0;
DebounceState8_t inputDebouncer;


void readInputs()
{
	static uint32_t lastRead = 0;
	uint8_t currentInputState = 0;

	if (millis > lastRead + 10)
	{
		lastRead = millis;

		// Invert the two inputs, since they're active low
		// Shift PA4/PA5 down to bits 0, 1, mask off
		currentInputState = ((PINA ^ 0xFF)>>4) & 0x03;
		inputs = debounce8(currentInputState, &inputDebouncer);
	} 
}

// Grab the 8-byte UUID out of the SPI flash and use it to seed the
// LFSR random number generator

void initializeRandomGenerator()
{
	uint8_t buffer[UUID_LEN_BYTES];
	uint8_t i;
	spiflashReadUUID(buffer, sizeof(buffer));
	for(i=2; i<UUID_LEN_BYTES; i+=2)
	{
		buffer[0] ^= buffer[i];
		buffer[1] ^= buffer[i+1];
	}

	randomSeedSet(((uint16_t)buffer[1]<<8) + buffer[0]);
}

int main(void)
{
	// Deal with watchdog first thing
	MCUSR = 0;								// Clear reset status
	bool notYetActivated = true;
	wdt_reset();                     // Reset the WDT, just in case it's still enabled over reset
	wdt_enable(WDTO_1S);             // Enable it at a 1S timeout.

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

	initDebounceState8(&inputDebouncer, 0x00);

	audioInitialize();

	spiSetup();
	spiflashReset();

	sei();

	wdt_reset();

	initializeRandomGenerator();
	isplInitialize(); // What should I do if this fails?

	while(1)
	{
		wdt_reset();

		if (notYetActivated)
			randomGet(); // Just keep pulling random numbers for a while

		if (!audioIsPlaying() && (inputs & 0x01))
		{
			notYetActivated = false;
			AudioAssetRecord r;
			isplAudioAssetLoad(0, &r);
			audioPlay(r.addr, r.size, r.sampleRate);
		}
		audioPump();
		readInputs();
	}
}



