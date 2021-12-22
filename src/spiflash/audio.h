/*************************************************************************
Title:    AVR Ringbuffer
Authors:  Mark Finn <mark@mfinn.net>, Green Bay, WI, USA
          Nathan Holmes <maverick@drgw.net>, Colorado, USA
File:     avr-ringbuffer.h
License:  GNU General Public License v3

LICENSE:
    Copyright (C) 2021 Mark Finn, Nathan Holmes

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

#ifndef _AVR_RINGBUFFER_H_
#define _AVR_RINGBUFFER_H_

#include <stdint.h>
#include <stdbool.h>

#ifndef VM_DEBUG
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>
#endif

#define AUDIO_BUFFER_SZ  128

#define min(a,b) ((a)<(b)?(a):(b))

extern uint32_t millis;

typedef struct
{
	uint8_t headIdx;
	uint8_t tailIdx;
	bool full;
	uint8_t buffer[AUDIO_BUFFER_SZ];
} AudioRingBuffer;

typedef enum
{
	AUDIO_UNKNOWN   = 0,
	AUDIO_8BIT_UPCM = 1,
	AUDIO_TONE      = 100,
} AudioRecordType;

typedef struct
{
	uint8_t type;
	uint32_t addr;
	uint32_t size;
	uint16_t sampleRate;
	uint32_t flags;
} AudioAssetRecord;


void audioInitialize();
bool audioIsPlaying();
void audioPlay(uint32_t addr, uint32_t len, uint16_t sampleRateHz);
void audioPump();

// Functions related to the audio buffer - probably shouldn't call directly

void audioBufferInitialize();
uint8_t audioBufferSize();
uint8_t audioBufferDepth();
void audioBufferPush(uint8_t data);
uint8_t audioBufferPop();
uint32_t getMillis();
#endif

