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
#include "spiflash.h"
#include "audio.h"
#include "ispl.h"
#include "lfsr.h"
#include "debouncer.h"

DebounceState8_t inputDebouncer;

#define WHITE_INPUT_ACTIVE   0x01
#define BLUE_INPUT_ACTIVE    0x02
#define BOTH_INPUTS_ACTIVE   0x04

#define VOLUME_COEF          10
volatile uint8_t currentVolume = 0;
uint8_t targetVolume = 0;

void readInputs()
{
	static uint32_t lastRead = 0;
	uint8_t currentInputState = 0;

	uint32_t currentMillis = getMillis();
	
	if (currentMillis > lastRead + 20)
	{
		lastRead = currentMillis;

		// Invert the two inputs, since they're active low, mask off
		if((~PINA & (_BV(PA4) | _BV(PA5))) == (_BV(PA4) | _BV(PA5)))
			currentInputState = BOTH_INPUTS_ACTIVE;
		else if((~PINA & (_BV(PA4) | _BV(PA5))) == (_BV(PA4)))
			currentInputState = WHITE_INPUT_ACTIVE;
		else if((~PINA & (_BV(PA4) | _BV(PA5))) == (_BV(PA5)))
			currentInputState = BLUE_INPUT_ACTIVE;
		debounce8(currentInputState, &inputDebouncer);
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
			setVolume(currentVolume);
		}
		else if(currentVolume > targetVolume)
		{
			deltaVolume = (currentVolume - targetVolume);
			if((deltaVolume > 0) && (deltaVolume < VOLUME_COEF))
				deltaVolume = VOLUME_COEF;  // Make sure it goes all the way to min or max
			currentVolume -= deltaVolume / VOLUME_COEF;
			setVolume(currentVolume);
		}
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

	PlayBackState playState = PLAYBACK_OFF;
	AudioAssetRecord r;


	sei();
	wdt_reset();

//	initializeRandomGenerator();
	isplInitialize(); // What should I do if this fails?

	uint8_t asset = 0;

	while(1)
	{
		wdt_reset();

		switch(playState)
		{
			case PLAYBACK_OFF:
				if (getDebouncedState(&inputDebouncer) & WHITE_INPUT_ACTIVE)
				{
					asset = 0;
					playState = PLAYBACK_START;
				}
				else if (getDebouncedState(&inputDebouncer) & BLUE_INPUT_ACTIVE)
				{
					asset = 1;
					playState = PLAYBACK_START;
				}
				else if (getDebouncedState(&inputDebouncer) & BOTH_INPUTS_ACTIVE)
				{
					asset = 2;
					playState = PLAYBACK_START;
				}
				break;

			case PLAYBACK_START:
				targetVolume = 255;
				isplAudioAssetLoad(asset, &r);
				audioPlay(r.addr, r.size, r.sampleRate, true);
				playState = PLAYBACK_WAIT;
				break;

			case PLAYBACK_WAIT:
				if (!getDebouncedState(&inputDebouncer))
				{
					targetVolume = 0;
					playState = VOLUME_WAIT;
				}
				break;

			case VOLUME_WAIT:
				if (0 == currentVolume)
				{
					stopAudio();
					playState = PLAYBACK_OFF;
				}
				break;

			default:
				playState = PLAYBACK_OFF;
				break;

		}

		audioPump();
		readInputs();
	}
}




