/*************************************************************************
Title:    Audio Functions
Authors:  Nathan Holmes <maverick@drgw.net>, Colorado, USA
          Michael Petersen <railfan@drgw.net>, Colorado, USA
File:     audio.c
License:  GNU General Public License v3

LICENSE:
    Copyright (C) 2024 Nathan Holmes, Michael Petersen

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

AudioRingBuffer audioBuffer;
uint32_t audioStartIdx = 0;
uint32_t audioEndIdx = 0;
uint32_t audioReadIdx = 0;
uint32_t audioDataLen = 0;
bool audioRun = true;
bool audioLoop = false;

// Audio playing ISR

uint32_t millis = 0;

void stopAudio()
{
	audioRun = false;
}

void stopAudioRepeat()
{
	audioLoop = false;
}

ISR(TIMER0_COMPA_vect) 
{
	static uint16_t micros = 0;
	static uint8_t next = 0x7F;
	OCR1B = next;
	next = audioBufferPop();
	
	micros += OCR0A;
	if(micros >= 1000)
	{
		millis++;
		micros -= 1000;
	}
}

uint32_t getMillis()
{
	return millis;
}

void audioAmplifierEnable(uint8_t enable)
{
	if (enable)
		PORTA |= _BV(PA7);
	else
		PORTA &= ~(_BV(PA7));
}
	
void audioInitialize()
{
	audioReadIdx = audioStartIdx = audioDataLen = 0;
	audioBufferInitialize();

	// Enable 64 MHz PLL and use as source for Timer1
	PLLCSR = _BV(PLLE);
	
	// Wait for PLL lock per datasheet
	do
	{
		_delay_us(100);
	}
	while (!(PLLCSR & _BV(PLOCK)));

	// Enable PCK once PLL lock established
	PLLCSR = _BV(PCKE) | _BV(PLLE);

	TIMSK = 0;                                    // Timer interrupts OFF

	// Set up Timer/Counter1 for PWM output on PB3 (OC1B)
	TCCR1A = 0b00100001; // PWM B, clear on match
	TCCR1B = 0b00000001; // No dead time, 1:1 prescale for counter 1
	OCR1B = 0x7F;        // 50% duty at start

	// Set up Timer/Counter0 for 1MHz clock, interrupts to output samples.
	//  OCR0A will be loaded as the number of us between samples
	TCCR0A = 0b00000001;  // CTC Mode
	TCCR0B = 0b00000010;  // Just CS01 - 1/8 prescale
	OCR0A = 42;
	TIMSK = _BV(OCIE0A);
	
	audioAmplifierEnable(1);
}

bool audioIsPlaying()
{
	if (audioBufferDepth() > 0 || audioReadIdx < audioStartIdx + audioDataLen)
		return true;
	return false;
}

void audioPlay(uint32_t addr, uint32_t len, uint16_t sampleRateHz, bool loop)
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

	audioRun = true;
	audioLoop = loop;
}

void audioPump()
{
	uint8_t bufferFree = sizeof(audioBuffer.buffer) - audioBufferDepth();
	uint32_t bytesRemaining = audioEndIdx - audioReadIdx;

	if (audioLoop && 0 == bytesRemaining)
	{
		audioReadIdx = audioStartIdx;
		bytesRemaining = audioEndIdx - audioReadIdx;
	}

	if ((bytesRemaining) && bufferFree > 32 && audioRun)
	{
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

