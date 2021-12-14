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
#include "avr-ringbuffer.h"

// 8MHz / 8 / 42 = 23.81kHz
#define OCR0A_DEFAULT 63

// playBell acts as an indicator between the main loop and 
// the ISR to tell it to shut down playback once it hits the end
// of the bell sound sample

volatile uint8_t playBell = 0;
RingBuffer audioData;
uint8_t audioDataBuffer[128];
uint32_t audioStartIdx = 0;
uint32_t audioReadIdx = 0;
uint32_t audioDataLen = 0;
/*
inline void disableAmplifier()
{
	PORTB &= ~_BV(PB4);
}

inline void enableAmplifier()
{
	PORTB |= _BV(PB4);
}
*/

// 16kHz interrupt to load high speed PWMs
ISR(TIMER0_COMPA_vect) 
{
	if (ringBufferDepth(&audioData))
	{
		OCR1B = ringBufferPop(&audioData);
	} else {
		OCR1B = 0x7F;
	}
}

#define min(a,b) ((a)<(b)?(a):(b))

void audioInitialize()
{
	audioReadIdx = audioStartIdx = audioDataLen = 0;
	ringBufferReinitialize(&audioData);
}

bool audioPlaying()
{
	if (ringBufferDepth(&audioData) > 0 || audioReadIdx < audioStartIdx + audioDataLen)
		return true;
	return false;
}

void audioPlay(uint32_t addr, uint32_t len, uint16_t sampleRateHz)
{
	// Load ring buffer with initial data
	uint8_t loadBytes = (audioData.bufferSz - 1) - ringBufferDepth(&audioData);
	audioStartIdx = audioReadIdx = addr;
	audioDataLen = len;
	
	// 8MHz / prescaler 8:1 / sample frequency
	OCR0A = (uint8_t)(((8000000UL / 8UL) + ((uint32_t)sampleRateHz - 1)) / sampleRateHz);

	spiflashReadToRingBuffer(audioReadIdx, loadBytes, &audioData);
	audioReadIdx += loadBytes;
}

void audioPump()
{
	uint8_t bufferFree = audioData.bufferSz - ringBufferDepth(&audioData);
	if ((audioReadIdx < audioDataLen) &&  bufferFree > 32)
	{
		// Load more data
		uint8_t bytesToRead = min(bufferFree-1, audioDataLen - audioReadIdx);
		spiflashReadToRingBuffer(audioReadIdx, bytesToRead, &audioData);
		audioReadIdx += bytesToRead;
	}
}


int main(void)
{
	// Deal with watchdog first thing
	MCUSR = 0;								// Clear reset status

	wdt_reset();                     // Reset the WDT, just in case it's still enabled over reset
	wdt_enable(WDTO_1S);             // Enable it at a 1S timeout.

	// Enable 64 MHz PLL and use as source for Timer1
	PLLCSR = _BV(PCKE) | _BV(PLLE);

	// Set up Timer/Counter1 for PWM output on PB1 (OCR1A)
	TIMSK = 0;                                    // Timer interrupts OFF
	TCCR1 = _BV(CS10); // PWM A, clear on match, 1:1 prescale
	GTCCR = _BV(PWM1B) | _BV(COM1B1);
	OCR1B = 0x7F;                                 // 50% duty at start

	// Set up Timer/Counter0 for interrupts to output samples.
	TCCR0A = _BV(WGM00) | _BV(WGM01);             // Fast PWM (also needs WGM02 in TCCR0B)
	TCCR0B = _BV(WGM02) | _BV(CS01);              // 1/8 prescale
	OCR0A = OCR0A_DEFAULT;

	DDRB = _BV(PB4);
	PORTB |= _BV(PB3);                            // Turn on pullup for PB3 (enable input)
	
	ringBufferInitialize(&audioData, audioDataBuffer, sizeof(audioDataBuffer));
	
	spiSetup();
	spiCSAcquire();
	spiflashReset();
	sei();

	wdt_reset();
	_delay_ms(400);  // Wait for PWM and PB3 to settle

	audioInitialize();

	TIMSK = _BV(OCIE0A);

	while(1)
	{
		wdt_reset();

		if (!audioPlaying())
		{
			audioPlay(0, 729088, 16000);
		}
		audioPump();
	}
}

