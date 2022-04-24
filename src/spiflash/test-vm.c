#include <stdio.h>
#include <string.h>
#include "lfsr.h"
#include "ispl.h"
#include <malloc.h>
#include <stdlib.h>

uint8_t* fileBuffer = NULL;

uint32_t millis = 0;

void spiSetup()
{
	return;
}

void spiCSAcquire()
{
	return;
}

void spiCSRelease()
{
	return;
}

inline void spiCSEnable()
{
	return;
}

inline void spiCSDisble()
{
	return;
}

uint32_t getMillis()
{
	return millis++;
}

void spiflashReset()
{
	printf("spiflashReset()\n");
}

void spiflashReadUUID(uint8_t* destPtr, uint8_t len)
{
	const uint8_t* uuid = "\x12\x34\x56\x78\x90\xAB\xCD\xEF";
	printf("spiflashReadUUID()\n");
	memcpy(destPtr, uuid, len);
}


void spiflashReadBlock(uint32_t addr, uint8_t len, uint8_t* destPtr)
{
	printf("spiflashReadBlock(addr=%d, len=%d)\n", addr, len);
	memcpy(destPtr, fileBuffer + addr, len);
}

uint8_t spiflashReadU8(uint32_t addr)
{
	printf("spiflashReadU8(addr=%d) = 0x%02X\n", addr, fileBuffer[addr]);
	return fileBuffer[addr];
}

uint16_t spiflashReadU16(uint32_t addr)
{
	uint16_t i = 0;
	i = (uint16_t)fileBuffer[addr]<<8 | fileBuffer[addr+1];
	printf("spiflashReadU16(addr=%d) = 0x%04X\n", addr, i);
	return i;
}

uint32_t spiflashReadU32(uint32_t addr)
{
	uint32_t i=0, j;
	for(j=0; j<4; j++)
	{
		i <<= 8;
		i |= fileBuffer[addr+j];
	}
	printf("spiflashReadU32(addr=%d) = 0x%08X\n", addr, i);
	return i;
}

uint8_t inputs = 1;

bool audioIsPlaying()
{
	return false;
}

void audioPlay(uint32_t addr, uint32_t len, uint16_t sampleRateHz)
{
	printf("audioPlay(addr=%d, len=%d, sampleRateSz=%d)\n", addr, len, sampleRateHz);
}

int main(int argc, char* argv[])
{
	FILE* f = fopen(argv[1], "rb+");
	uint32_t sz=0;
	int i;
	
	int end=atoi(argv[2]);
	
	fseek(f, 0L, SEEK_END);
	sz = ftell(f);
	rewind(f);
	fileBuffer = calloc(sz+1, sizeof(uint8_t));
	
	printf("Loading file of %d size\n", sz);

	fread(fileBuffer, sz, 1, f);

	isplInitialize();
	
	
	for(i=0; i<end; i++)
	{
		isplVirtualMachineRun();
	}
	
	
	free(fileBuffer);
}
