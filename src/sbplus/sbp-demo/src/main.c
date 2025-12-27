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
    Copyright (C) 2024 Michael Petersen, Nathan Holmes, with portions from 
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
#include <stdlib.h>
#include "spiflash.h"
#include "audio.h"
#include "ispl.h"
#include "i2c.h"

#define BELL       0
#define SAFEMECH   1
#define SAFEHYB    2
#define USSTEAR    3
#define WCH333     4
#define WCH777     5
#define WRS222     6
#define CTCBELL    7
#define CICADA1    8
#define CICADA2    9
#define CICADA3   10
#define CICADA4   11
#define CICADA5   12
#define CICADA6   13
#define CICADA7   14

#define VOLUME_COEF          10
volatile uint8_t currentVolume = 255;
uint8_t targetVolume = 255;

uint8_t i2cAddr = 0x20;
uint16_t ioState = 0x00;
uint16_t ioStateSaved = 0x00;

void readInputs()
{
	static uint32_t lastRead = 0;
	uint8_t i2cData;

	uint32_t currentMillis = getMillis();
	
	if (currentMillis > lastRead + 20)
	{
		lastRead = currentMillis;

		// Read inputs
		readByte(i2cAddr, 0x12, &i2cData);
		ioState = i2cData;
		readByte(i2cAddr, 0x13, &i2cData);
		ioState += i2cData << 8;
	} 

	static uint32_t lastVolume = 0;
	uint8_t deltaVolume;
	if (currentMillis > lastVolume + 10)
	{
		lastVolume = currentMillis;
		if(currentVolume < targetVolume)
		{
			deltaVolume = (targetVolume - currentVolume);
			if((deltaVolume > 0) && (deltaVolume < VOLUME_COEF))
				deltaVolume = VOLUME_COEF;  // Make sure it goes all the way to min or max
			currentVolume += deltaVolume / VOLUME_COEF;
		}
		else if(currentVolume > targetVolume)
		{
			deltaVolume = (currentVolume - targetVolume);
			if((deltaVolume > 0) && (deltaVolume < VOLUME_COEF))
				deltaVolume = VOLUME_COEF;  // Make sure it goes all the way to min or max
			currentVolume -= deltaVolume / VOLUME_COEF;
		}
	setVolume(currentVolume);
	}
}

typedef enum 
{
	PLAYBACK_OFF,
	PLAYBACK_START,
	PLAYBACK_WAIT,
	VOLUME_WAIT,
}
PlayBackState;

#define LOCKOUT_TIME_MILLISECS  5000UL

int main(void)
{
	// Deal with watchdog first thing
	MCUSR = 0;								// Clear reset status
	wdt_reset();                     // Reset the WDT, just in case it's still enabled over reset
	wdt_enable(WDTO_1S);             // Enable it at a 1S timeout.

	// PORT A
	//  PA7 - Output - /SHUTDOWN to amplifier
	//  PA6 - Output - No Connection
	//  PA5 - Input  - SCL
	//  PA4 - Input  - SDA
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
	DDRA  = 0b11101110;

	PORTB = 0b00000000; 	// Just make everything low
	DDRB  = 0b11111111;     // And set it as an output

	uint8_t i;

	audioInitialize();
	spiSetup();
	spiflashReset();

	PlayBackState playState = PLAYBACK_OFF;
	AudioAssetRecord r;

	sei();
	wdt_reset();

	isplInitialize(); // What should I do if this fails?

	// Set up MCP23017, BANK = 0 by default
	writeByte(i2cAddr, 0x0C, 0xFF);   // Enable pull-ups
	writeByte(i2cAddr, 0x0D, 0xFF);
	writeByte(i2cAddr, 0x02, 0xFF);   // Invert polarity
	writeByte(i2cAddr, 0x03, 0xFF);

	uint8_t asset = 0;

	while(1)
	{
		wdt_reset();

		switch(playState)
		{
			case PLAYBACK_OFF:
				asset = 0xFF;   // Set to an unused value
				if(ioState)
				{
					switch(ioState)
					{
						case 0x4000:
							asset = BELL;
							break;
						case 0x2000:
							asset = SAFEMECH;
							break;
						case 0x1000:
							asset = SAFEHYB;
							break;
						case 0x0800:
							asset = USSTEAR;
							break;
						case 0x0400:
							asset = WCH333;
							break;
						case 0x0200:
							asset = WCH777;
							break;
						case 0x0100:
							asset = WRS222;
							break;
						case 0x0008:
							asset = CTCBELL;
							break;
						case 0x0040:
							i = (rand() % 7);  // Random number between 0 and 6
							asset = CICADA1 + i;
							break;
						default:
							break;
					}
				}

				if(0xFF != asset)
				{
					playState = PLAYBACK_START;
					ioStateSaved = ioState;
				}
				break;

			case PLAYBACK_START:
				targetVolume = 255;
				isplAudioAssetLoad(asset, &r);
				if( (CTCBELL == asset) || (asset >= CICADA1) )
					audioPlay(r.addr, r.size, r.sampleRate, false);
				else
					audioPlay(r.addr, r.size, r.sampleRate, true);
				playState = PLAYBACK_WAIT;
				break;

			case PLAYBACK_WAIT:
				if (!(ioState & ioStateSaved))  // Check for the button that was activated to be deactivated
				{
					//targetVolume = 0;
					playState = VOLUME_WAIT;
				}
				break;

			case VOLUME_WAIT:
//				if (0 == currentVolume)
//				{
					stopAudioRepeat();
					playState = PLAYBACK_OFF;
//				}
				break;

			default:
				playState = PLAYBACK_OFF;
				break;

		}

		audioPump();
		readInputs();
	}
}




