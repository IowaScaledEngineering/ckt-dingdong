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
#include "avr-ringbuffer.h"

void ringBufferInitialize(RingBuffer* r, uint8_t* buffer, uint8_t bufferSz)
{
	r->buffer = buffer;
	r->bufferSz = bufferSz;
	r->headIdx = r->tailIdx = 0;
	r->full = false;
	memset(r->buffer, 0, r->bufferSz);
}

void ringBufferReinitialize(RingBuffer* r)
{
	r->headIdx = r->tailIdx = 0;
	r->full = false;
}

uint8_t ringBufferSize(RingBuffer* r)
{
	return r->bufferSz;
}

uint8_t ringBufferDepth(RingBuffer* r)
{
	if(r->full)
		return(r->bufferSz);

	return (uint8_t)(r->headIdx - r->tailIdx) % 128;//r->bufferSz;
}

void ringBufferPush(RingBuffer* r, uint8_t data)
{
	r->buffer[r->headIdx] = data;

	if( ++r->headIdx >= r->bufferSz )
		r->headIdx = 0;

	if (r->headIdx == r->tailIdx)
		r->full = true;
}

uint8_t ringBufferPop(RingBuffer* r)
{
	uint8_t retval = 0;
	if (0 == ringBufferDepth(r))
		return(0);

	retval = r->buffer[r->tailIdx];

	if( ++r->tailIdx >= r->bufferSz )
		r->tailIdx = 0;
	r->full = false;

	return(retval);
}


