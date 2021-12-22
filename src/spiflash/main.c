/*************************************************************************
Title:    Small Sound Player Module
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
    Copyright (C) 2019 Michael Petersen, Nathan Holmes, with portions from 
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

uint8_t inputs = 0;

void readInputs()
{
	static uint32_t lastRead = 0;
	static uint8_t counter = 0;
	
	if (millis > lastRead + 10)
	{
		spiCSRelease();
		lastRead = millis;
		
		if(PINB & _BV(PB3))
		{
			if (counter < 4)
				counter++;
		} else {
			if (counter > 0)
				counter--;
		}
		
		if (4 == counter)
			inputs = 0x00;
		else if (0 == counter)
			inputs = 0x01;
		
		spiCSAcquire();
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

	wdt_reset();                     // Reset the WDT, just in case it's still enabled over reset
	wdt_enable(WDTO_1S);             // Enable it at a 1S timeout.

	DDRB = _BV(PB4);
	PORTB |= _BV(PB3);                            // Turn on pullup for PB3 (enable input)

	audioInitialize();

	spiSetup();
	spiCSAcquire();
	spiflashReset();

	sei();

	wdt_reset();
	_delay_ms(100);  // Wait for PWM and PB3 to settle

	initializeRandomGenerator();
	isplInitialize(); // What should I do if this fails?

	while(1)
	{
		wdt_reset();

		isplVirtualMachineRun();
/*		if (!audioIsPlaying() && (inputs & 0x01))
		{
			isplAudioAssetLoad(0, &r);
			audioPlay(r.addr, r.size, r.sampleRate);
		}*/

		audioPump();
		readInputs();
	}
}



