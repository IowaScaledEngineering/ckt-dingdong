/*************************************************************************
Title:    ISE Sound Programming Language for attiny85
Authors:  Nathan Holmes <maverick@drgw.net>, Colorado, USA
          Michael Petersen <railfan@drgw.net>, Colorado, USA
File:     ispl.h
License:  GNU General Public License v3

LICENSE:
    Copyright (C) 2022 Nathan Holmes & Michael Petersen

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
#include <stdint.h>
#include <stdbool.h>
#include "ispl.h"
#include "audio.h"
#include "spiflash.h"

ISPLTable isplTable;
ISPLTable audioTable;

void isplTableLoad(ISPLTable* t, uint8_t tableNum)
{
	uint32_t n;
	uint32_t s;

	n = spiflashReadU32(ISPL_MANIFEST_BASE_ADDR + ISPL_MANIFEST_REC_N_OFFSET);
	s = spiflashReadU16(ISPL_MANIFEST_BASE_ADDR + ISPL_MANIFEST_REC_S_OFFSET);

	t->baseAddr = 0;
	t->n = 0;
	t->s = 0;

	if (tableNum >= n)
		return;
	
	t->baseAddr = spiflashReadU32(ISPL_MANIFEST_BASE_ADDR + tableNum * s + ISPL_MANIFEST_REC_ADDR_OFFSET);
	t->n = spiflashReadU32(ISPL_MANIFEST_BASE_ADDR + tableNum * s + ISPL_MANIFEST_REC_N_OFFSET);
	t->s = spiflashReadU16(ISPL_MANIFEST_BASE_ADDR + tableNum * s + ISPL_MANIFEST_REC_S_OFFSET);
}

bool isplAudioAssetLoad(uint16_t assetNum, AudioAssetRecord* r)
{
	uint32_t recordAddr = audioTable.baseAddr;

	memset(r, 0, sizeof(AudioAssetRecord));
	if (assetNum >= audioTable.n)
		return false;

	recordAddr += assetNum * audioTable.s;

	r->type = spiflashReadU8(recordAddr + ISPL_AUDIOREC_TYPE_OFFSET);
	r->addr = spiflashReadU32(recordAddr + ISPL_AUDIOREC_ADDR_OFFSET);
	r->size = spiflashReadU32(recordAddr + ISPL_AUDIOREC_SIZE_OFFSET);
	r->sampleRate = spiflashReadU16(recordAddr + ISPL_AUDIOREC_RATE_OFFSET);
	r->flags = spiflashReadU32(recordAddr + ISPL_AUDIOREC_FLAGS_OFFSET);
	return true;
}

bool isplInitialize()
{
	uint8_t buffer[8];
	spiflashReadBlock(ISPL_HEADER_IDENTIFIER_ADDR, ISPL_HEADER_IDENTIFIER_LEN, buffer);
	if (0 != memcmp(buffer, ISPL_HEADER_IDENTIFIER, ISPL_HEADER_IDENTIFIER_LEN))
		return false;  // Header doesn't contain correct starting record

	isplTableLoad(&isplTable, ISPL_TABLE_PROGRAM);
	isplTableLoad(&audioTable, ISPL_TABLE_AUDIO);
	return true;
}





