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

#ifndef _ISPL_H_
#define _ISPL_H_

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "audio.h"

typedef struct
{
	uint32_t baseAddr;
	uint32_t n;
	uint16_t s;
} ISPLTable;

extern ISPLTable isplTable;
extern ISPLTable audioTable;

#define ISPL_HEADER_IDENTIFIER_ADDR   0x00000000
#define ISPL_HEADER_IDENTIFIER_LEN    4
#define ISPL_HEADER_IDENTIFIER        "ISPL"

#define ISPL_MANIFEST_BASE_ADDR       0x00000008

#define ISPL_MANIFEST_REC_ADDR_OFFSET 0
#define ISPL_MANIFEST_REC_N_OFFSET    4
#define ISPL_MANIFEST_REC_S_OFFSET    8

#define ISPL_TABLE_MANIFEST           0
#define ISPL_TABLE_PROGRAM            1
#define ISPL_TABLE_AUDIO              2

#define ISPL_AUDIOREC_TYPE_OFFSET     0
#define ISPL_AUDIOREC_ADDR_OFFSET     1
#define ISPL_AUDIOREC_SIZE_OFFSET     5
#define ISPL_AUDIOREC_RATE_OFFSET     9
#define ISPL_AUDIOREC_FLAGS_OFFSET   13

bool isplAudioAssetLoad(uint16_t assetNum, AudioAssetRecord* r);
void isplTableLoad(ISPLTable* t, uint8_t tableNum);
bool isplInitialize();

/*  Start of Flash:
 *  IDENT:  ISPL
 *  VER:    1000
 *  
 *  Manifest Table Record:
 *  [Addr:32] [Recs-N:32] [Recs-S:16]
 *  
 *  Manifest positions:
 *  0 - Manifest table
 *  1 - Audio assets table
 *  2 - Program
 * 
 *  Audio Asset Record:
 *  [Type:8] [Addr:32] [Size:32] [SampleRate:16] [Flags:32]
 * 
 */

#endif
