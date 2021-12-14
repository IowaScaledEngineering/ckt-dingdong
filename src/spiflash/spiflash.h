/*************************************************************************
Title:    Small Sound Player Module - SPI Flash Interface
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
    Copyright (C) 2021 Michael Petersen, Nathan Holmes

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

*************************************************************************/
#ifndef _SPIFLASH_H_
#define _SPIFLASH_H_


#include <stdint.h>
#include "audio.h"

#define SPI_DDR_PORT DDRB
#define SPI_PORT     PORTB
#define USI_DI_PIN   PB0
#define USI_DO_PIN   PB1
#define USI_SCK_PIN  PB2
#define USI_CS_PIN   PB3

#define SPI_FLASH_READ_CMD       0x03
#define SPI_FLASH_POWER_UP_CMD   0xAB


void spiSetup();
void spiCSAcquire();
void spiCSRelease();
void spiCSEnable();
void spiCSDisble();
uint8_t spiTransferByte(uint8_t txData);
void spiflashReset();
void spiflashReadBlock(uint32_t addr, uint8_t len, uint8_t* destPtr);
void spiflashReadToRingBuffer(uint32_t addr, uint8_t len);
#endif
