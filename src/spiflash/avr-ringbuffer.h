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

typedef struct
{
	uint8_t headIdx;
	uint8_t tailIdx;
	bool full;
	uint8_t* buffer;
	uint8_t bufferSz;
} RingBuffer;


void ringBufferInitialize(RingBuffer* r, uint8_t* buffer, uint8_t bufferSz);
void ringBufferReinitialize(RingBuffer* r);
uint8_t ringBufferDepth(RingBuffer* r);
void ringBufferPush(RingBuffer* r, uint8_t data);
uint8_t ringBufferPop(RingBuffer* r);
uint8_t ringBufferSize(RingBuffer* r);

#endif

