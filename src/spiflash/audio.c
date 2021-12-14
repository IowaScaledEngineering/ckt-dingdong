/*************************************************************************
Title:    AVR Ringbuffer
Authors:  Mark Finn <mark@mfinn.net>, Green Bay, WI, USA
          Nathan Holmes <maverick@drgw.net>, Colorado, USA
File:     avr-ringbuffer.h
License:  GNU General Public License v3

LICENSE:
    Copyright (C) 2014 Mark Finn, Nathan Holmes

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License along 
    with this program. If not, see http://www.gnu.org/licenses/
    
*************************************************************************/
#include <string.h>
#include "audio.h"
#include "spiflash.h"

#define min(a,b) ((a)<(b)?(a):(b))

AudioRingBuffer audioBuffer;
uint32_t audioStartIdx = 0;
uint32_t audioEndIdx = 0;
uint32_t audioReadIdx = 0;
uint32_t audioDataLen = 0;

// Audio playing ISR

ISR(TIMER0_COMPA_vect) 
{
	OCR1B = audioBufferPop();
}


void audioInitialize()
{
	audioReadIdx = audioStartIdx = audioDataLen = 0;
	audioBufferInitialize();

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
	OCR0A = 42;
	
	TIMSK = _BV(OCIE0A);
}

bool audioIsPlaying()
{
	if (audioBufferDepth() > 0 || audioReadIdx < audioStartIdx + audioDataLen)
		return true;
	return false;
}

void audioPlay(uint32_t addr, uint32_t len, uint16_t sampleRateHz)
{
	// Load ring buffer with initial data
	uint8_t loadBytes = (sizeof(audioBuffer.buffer) - 1) - audioBufferDepth();
	audioStartIdx = audioReadIdx = addr;
	audioDataLen = len;
	
	// 8MHz / prescaler 8:1 / sample frequency
	OCR0A = (uint8_t)(((8000000UL / 8UL) + ((uint32_t)sampleRateHz - 1)) / sampleRateHz);

	spiflashReadToRingBuffer(audioReadIdx, loadBytes);
	audioReadIdx += loadBytes;
	audioEndIdx = audioStartIdx + audioDataLen;
}

void audioPump()
{
	uint8_t bufferFree = sizeof(audioBuffer.buffer) - audioBufferDepth();
	uint32_t bytesRemaining = audioEndIdx - audioReadIdx;
	if ((bytesRemaining) &&  bufferFree > 32)
	{
		// Load more data
		uint8_t bytesToRead = min(bufferFree-1, bytesRemaining);
		spiflashReadToRingBuffer(audioReadIdx, bytesToRead);
		audioReadIdx += bytesToRead;
	}
}

void audioBufferInitialize()
{
	audioBuffer.headIdx = audioBuffer.tailIdx = 0;
	audioBuffer.full = false;
}

uint8_t audioBufferSize()
{
	return sizeof(audioBuffer.buffer);
}

uint8_t audioBufferDepth()
{
	if(audioBuffer.full)
		return(audioBufferSize());

	return (uint8_t)(audioBuffer.headIdx - audioBuffer.tailIdx) % sizeof(audioBuffer.buffer);
}

void audioBufferPush(uint8_t data)
{
	audioBuffer.buffer[audioBuffer.headIdx] = data;

	if( ++audioBuffer.headIdx >= sizeof(audioBuffer.buffer) )
		audioBuffer.headIdx = 0;

	if (audioBuffer.headIdx == audioBuffer.tailIdx)
		audioBuffer.full = true;
}

uint8_t audioBufferPop()
{
	uint8_t retval = 0;
	if (0 == audioBufferDepth())
		return(0x7F);

	retval = audioBuffer.buffer[audioBuffer.tailIdx];

	if( ++audioBuffer.tailIdx >= sizeof(audioBuffer.buffer) )
		audioBuffer.tailIdx = 0;
	audioBuffer.full = false;

	return(retval);
}


