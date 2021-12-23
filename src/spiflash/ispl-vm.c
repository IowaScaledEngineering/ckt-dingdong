#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef VM_DEBUG
#include <stdio.h>
#endif

#include "ispl.h"
#include "spiflash.h"
#include "lfsr.h"

#define ISPL_STACK_SZ 32
#define ISPL_CACHE_SZ 32

uint8_t isplvm_codeCache[ISPL_CACHE_SZ];
uint16_t isplvm_codeCacheStart;
uint16_t isplvm_stack[ISPL_STACK_SZ];
uint16_t isplvm_cip;
uint8_t isplvm_sp;


bool isplInitialize()
{
	uint8_t buffer[8];
	spiflashReadBlock(ISPL_HEADER_IDENTIFIER_ADDR, ISPL_HEADER_IDENTIFIER_LEN, buffer);
	if (0 != memcmp(buffer, ISPL_HEADER_IDENTIFIER, ISPL_HEADER_IDENTIFIER_LEN))
		return false;  // Header doesn't contain correct starting record

	isplTableLoad(&isplTable, ISPL_TABLE_PROGRAM);
	
#ifdef VM_DEBUG
	printf("isplTable.baseAddr = %d, n=%d, s=%d\n", isplTable.baseAddr, isplTable.n, isplTable.s);
#endif
	
	isplTableLoad(&audioTable, ISPL_TABLE_AUDIO);

#ifdef VM_DEBUG
	printf("audioTable.baseAddr = %d, n=%d, s=%d\n", audioTable.baseAddr, audioTable.n, audioTable.s);
#endif


	isplvm_cip = 0;
	isplvm_sp = 0;
	memset(isplvm_stack, 0, sizeof(isplvm_stack));
	memset(isplvm_codeCache, 0, sizeof(isplvm_codeCache));
	isplvm_codeCacheStart = 0xFFFF;
	return true;
}

uint8_t isplFetchCIP()
{
	uint8_t retval = 0;
	//isplTable->baseAddr
	if (isplvm_cip < isplvm_codeCacheStart || isplvm_cip >= isplvm_codeCacheStart + ISPL_CACHE_SZ)
	{
		// Load new cache from flash
		if (isplvm_cip < ISPL_CACHE_SZ/4)
			isplvm_codeCacheStart = isplvm_cip;
		else 
			isplvm_codeCacheStart = isplvm_cip - (ISPL_CACHE_SZ/4);

#ifdef VM_DEBUG
	printf("isplTable.baseAddr = %d\n", isplTable.baseAddr);
#endif


		spiflashReadBlock(isplTable.baseAddr + isplvm_codeCacheStart, ISPL_CACHE_SZ, isplvm_codeCache);
	}

#ifdef VM_DEBUG
	//printf("isplvm_cip = %d\n", isplvm_cip);
#endif

	retval = isplvm_codeCache[isplvm_cip - isplvm_codeCacheStart];
	isplvm_cip++;
	return retval;
}

uint16_t isplFetchU16CIP()
{
	uint16_t i = ((uint16_t)isplFetchCIP()<<8) | isplFetchCIP();
#ifdef VM_DEBUG
	printf("U16 constant = %u\n", i);
#endif
	return i;
}

int16_t isplFetchS16CIP()
{
	int16_t i = isplFetchU16CIP() - 65536;
#ifdef VM_DEBUG
	printf("S16 constant = %d\n", i);
#endif
	return i;
}

#ifndef ISPLVMOpcodes

#define ISPLVMOpcodesEnum \
	ENUM(ISPL_LDST   , 0x00) \
	ENUM(ISPL_LDIN   , 0x01) \
	ENUM(ISPL_RAND   , 0x02) \
	ENUM(ISPL_LDCIP  , 0x03) \
	ENUM(ISPL_LDSP   , 0x04) \
	ENUM(ISPL_LDTIME , 0x05) \
	ENUM(ISPL_PLAY   , 0x0A) \
	ENUM(ISPL_STOP   , 0x0B) \
	ENUM(ISPL_DROP   , 0x20) \
	ENUM(ISPL_DUP    , 0x21) \
	ENUM(ISPL_PUSH   , 0x22) \
	ENUM(ISPL_SWAP   , 0x23) \
	ENUM(ISPL_ROT    , 0x24) \
	ENUM(ISPL_OVER   , 0x25) \
	ENUM(ISPL_LOAD   , 0x26) \
	ENUM(ISPL_LOADn  , 0x27) \
	ENUM(ISPL_STOR   , 0x28) \
	ENUM(ISPL_STORn  , 0x29) \
	ENUM(ISPL_ADD    , 0x30) \
	ENUM(ISPL_SUB    , 0x31) \
	ENUM(ISPL_MUL    , 0x32) \
	ENUM(ISPL_DIV    , 0x33) \
	ENUM(ISPL_MOD    , 0x34) \
	ENUM(ISPL_AND    , 0x35) \
	ENUM(ISPL_OR     , 0x36) \
	ENUM(ISPL_XOR    , 0x37) \
	ENUM(ISPL_NOT    , 0x38) \
	ENUM(ISPL_NOTL   , 0x3A) \
	ENUM(ISPL_ANDL   , 0x3B) \
	ENUM(ISPL_CALL   , 0x70) \
	ENUM(ISPL_RET    , 0x71) \
	ENUM(ISPL_JMP    , 0x80) \
	ENUM(ISPL_JEQ    , 0x81) \
	ENUM(ISPL_JNE    , 0x82) \
	ENUM(ISPL_JLT    , 0x83) \
	ENUM(ISPL_JLTE   , 0x84) \
	ENUM(ISPL_JGT    , 0x85) \
	ENUM(ISPL_JGTE   , 0x86) \
	ENUM(ISPL_RJMP   , 0x90) \
	ENUM(ISPL_RJEQ   , 0x91) \
	ENUM(ISPL_RJNE   , 0x92) \
	ENUM(ISPL_RJLT   , 0x93) \
	ENUM(ISPL_RJLTE  , 0x94) \
	ENUM(ISPL_RJGT   , 0x95) \
	ENUM(ISPL_RJGTE  , 0x96) \
	ENUM(ISPL_NOP    , 0xFF)
#define ENUM(x,y) x=y,
typedef enum { ISPLVMOpcodesEnum } ISPLVMOpcodes;
#undef ENUM

#endif

#ifdef VM_DEBUG
typedef struct { uint8_t val; const char* const name; } ISPLVMOpcodeMapElement;
#define ENUM(x,y) { (y), #x },
        const ISPLVMOpcodeMapElement ISPLOpcodeNames[] = { ISPLVMOpcodesEnum };
#undef ENUM


const char* isplOpcodeName(uint8_t op)
{
	uint16_t i;
	for(i=0; i<256; i++)
	{
		if (ISPLOpcodeNames[i].val == op)
			return ISPLOpcodeNames[i].name;
	}
	return("UNK");
}
#endif


extern uint8_t inputs;

bool isplEvaluateConditional(uint8_t comparisonType, uint16_t a, uint16_t b)
{
	switch(comparisonType)
	{
		case 0x01:
			return a == b;
		case 0x02:
			return a != b;
		case 0x03:
			return a < b;
		case 0x04:
			return a <= b;
		case 0x05:
			return a > b;
		case 0x06:
			return a >= b;
	}
	return false;
}

// Program
// while()
//  if (!audioIsPlaying() && inputEnableLine)
//    play(0)

// ldst                 0x00
// push 0x0001          0x22 0x00 0x01
// and                  0x35
// lnot                 0x3A
// ldin                 0x01
// push 0x0001          0x22 0x00 0x01
// and                  0x35
// land                 0x3B
// push 0               0x22 0x00 0x00
// rje 4                0x91 0x00 0x04
// push 0               0x22 0x00 0x00
// play                 0x0A
// jmp 0                0x80 0x00 0x00

void isplVirtualMachineRun()
{
	uint16_t a, b;
#ifdef VM_DEBUG
	uint16_t cip = isplvm_cip;
#endif
	uint8_t op = isplFetchCIP();
	int16_t c;
#ifdef VM_DEBUG
printf("CIP=0x%04X  SP=%d OP=[%s] (0x%02x)\n", cip, isplvm_sp, isplOpcodeName(op), op);
#endif
	
	switch(op)
	{

		case ISPL_CALL:
			isplvm_stack[isplvm_sp++] = isplvm_cip;
			// fall through
		case ISPL_JMP:
			isplvm_cip = isplFetchU16CIP();
			break;

		case ISPL_RET:
			isplvm_cip = isplvm_stack[--isplvm_sp];
			break;

		case ISPL_JEQ:
		case ISPL_JNE:
		case ISPL_JLT:
		case ISPL_JLTE:
		case ISPL_JGT:
		case ISPL_JGTE:
			a = isplFetchU16CIP(); // Fetch jump address
			if (isplEvaluateConditional(op & 0x07, isplvm_stack[isplvm_sp-1], isplvm_stack[isplvm_sp-2]))
				isplvm_cip = a;
			isplvm_sp -= 2;
			break;

		case ISPL_RJMP:
			isplvm_cip += isplFetchS16CIP();
			break;

		case ISPL_RJEQ:
		case ISPL_RJNE:
		case ISPL_RJLT:
		case ISPL_RJLTE:
		case ISPL_RJGT:
		case ISPL_RJGTE:
			c = isplFetchS16CIP(); // Fetch jump address
			if (isplEvaluateConditional(op & 0x07, isplvm_stack[isplvm_sp-1], isplvm_stack[isplvm_sp-2]))
				isplvm_cip += c;
			isplvm_sp -= 2;
			break;
		
		case ISPL_LDST:
			a = 0;
			if (audioIsPlaying())
				a |= 0x0001;
			isplvm_stack[isplvm_sp++] = a;
			break;

		case ISPL_LDIN:
			isplvm_stack[isplvm_sp++] = inputs;
			break;

		case ISPL_LDTIME:
			isplvm_stack[isplvm_sp++] = (uint16_t)getMillis();
			break;

		case ISPL_RAND:
			a = isplFetchU16CIP();
			b = isplFetchU16CIP();
			isplvm_stack[isplvm_sp++] = randomRangedGet(a, b);
			break;

		case ISPL_PLAY:
			{
			AudioAssetRecord r;
			isplAudioAssetLoad(isplvm_stack[--isplvm_sp], &r);
			audioPlay(r.addr, r.size, r.sampleRate);
			}
			break;
			
		case ISPL_DUP:
			isplvm_stack[isplvm_sp] = isplvm_stack[isplvm_sp-1];
			isplvm_sp++;
			break;

		case ISPL_LOADn:  // DUP of stack position n (global var)
			isplvm_stack[isplvm_sp++] = isplvm_stack[isplFetchCIP()];
			break;

		case ISPL_LOAD:  // DUP of stack position (stacktop)
			isplvm_stack[isplvm_sp] = isplvm_stack[isplvm_stack[isplvm_sp]];
			break;

		case ISPL_STORn:  // Store value from top of stack into constant position (global var)
			isplvm_stack[isplFetchCIP()] = isplvm_stack[--isplvm_sp];
			break;

		case ISPL_STOR:  // Store value 1 down on stack into position from stop of stack
			a = isplvm_stack[--isplvm_sp];
			b = isplvm_stack[--isplvm_sp];
			isplvm_stack[b] = a;
			break;


		case ISPL_DROP:
			isplvm_sp--;
			break;
			
		case ISPL_SWAP:
			a = isplvm_stack[isplvm_sp-1];
			isplvm_stack[isplvm_sp-1] = isplvm_stack[isplvm_sp-2];
			isplvm_stack[isplvm_sp-2] = a;
			break;

		case ISPL_ROT:
			a = isplvm_stack[isplvm_sp-1];
			b = isplvm_stack[isplvm_sp-2];
			isplvm_stack[isplvm_sp-2] = isplvm_stack[isplvm_sp-3];
			isplvm_stack[isplvm_sp-3] = a;
			isplvm_stack[isplvm_sp-1] = b;
			break;
			
		case ISPL_OVER:
			isplvm_stack[isplvm_sp] = isplvm_stack[isplvm_sp-2];
			isplvm_sp++;
			break;

		case ISPL_PUSH:
			isplvm_stack[isplvm_sp++] = isplFetchU16CIP();
			break;

		case ISPL_ADD:
			isplvm_stack[isplvm_sp-2] += isplvm_stack[isplvm_sp-1];
			isplvm_sp--;
			break;

		case ISPL_SUB:
			isplvm_stack[isplvm_sp-2] -= isplvm_stack[isplvm_sp-1];
			isplvm_sp--;
			break;

		case ISPL_MUL:
			isplvm_stack[isplvm_sp-2] *= isplvm_stack[isplvm_sp-1];
			isplvm_sp--;
			break;

		case ISPL_DIV:
			isplvm_stack[isplvm_sp-2] /= isplvm_stack[isplvm_sp-1];
			isplvm_sp--;
			break;

		case ISPL_AND:
			isplvm_stack[isplvm_sp-2] &= isplvm_stack[isplvm_sp-1];
			isplvm_sp--;
			break;

		case ISPL_OR:
			isplvm_stack[isplvm_sp-2] |= isplvm_stack[isplvm_sp-1];
			isplvm_sp--;
			break;

		case ISPL_XOR:
			isplvm_stack[isplvm_sp-2] ^= isplvm_stack[isplvm_sp-1];
			isplvm_sp--;
			break;

		case ISPL_NOT:
			isplvm_stack[isplvm_sp-1] = ~isplvm_stack[isplvm_sp-1];
			break;

		case ISPL_NOTL:
			isplvm_stack[isplvm_sp-1] = !isplvm_stack[isplvm_sp-1];
			break;

		case ISPL_ANDL:
			isplvm_stack[isplvm_sp-2] = isplvm_stack[isplvm_sp-2] && isplvm_stack[isplvm_sp-1];
			isplvm_sp--;
			break;

		default:
			break;
	}
}

