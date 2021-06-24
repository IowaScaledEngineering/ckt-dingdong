/*************************************************************************
Title:    Cicada Sound Module
Authors:  Michael Petersen <railfan@drgw.net>
          Nathan D. Holmes <maverick@drgw.net>
          Based on the work of David Johnson-Davies - www.technoblogy.com - 23rd October 2017
           and used under his Creative Commons Attribution 4.0 International license
License:  GNU General Public License v3

CREDIT:
    The basic idea behind this playback design came from David Johson-Davies, who
    provided the basic framework and the place where I started.

LICENSE:
    Copyright (C) 2020 Michael Petersen, Nathan Holmes, with portions from 
     David Johson-Davies under a Creative Commons Attribution 4.0 license

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

*************************************************************************/

#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>

// 16kHz 8 bit unsigned PCM data
#include "start.h"
#include "mid.h"
#include "end.h"

#define ATTACK_START  (uint16_t)62000
#define ATTACK_TIME       (uint16_t)4
#define DECAY_START   (uint16_t)45000
#define DECAY_TIME        (uint16_t)4
#define MAX_VOL         (uint16_t)256
#define MIN_VOL        (uint16_t)4096

uint16_t attenuation = MIN_VOL;
volatile uint8_t newAttenuation = 0;
uint8_t mute = 1;
uint8_t repeat = 0;

inline void disableAmplifier()
{
	PORTB &= ~_BV(PB4);
}

inline void enableAmplifier()
{
	PORTB |= _BV(PB4);
}

uint8_t pitch;

// 16kHz interrupt to load high speed PWMs
ISR(TIMER0_COMPA_vect) 
{
	static uint16_t wavIdx = 0;
	static uint8_t state = 8;
	static uint16_t myAttenuation = MIN_VOL;
	static uint8_t myMute = 1;

	uint8_t delta;
	uint8_t data = 0x7F;

	if(newAttenuation)
	{
		myAttenuation = attenuation;
		newAttenuation = 0;
	}
	
	if(8 == state)
	{
		// Start
		if(!mute)
			myMute = 0;
		data = pgm_read_byte(&start_wav[wavIdx++]);
		if (wavIdx == start_wav_len)
		{
			wavIdx = 0;
			state = 4 + repeat;
		}
	}
	else if(state)
	{
		// Middle
		data = pgm_read_byte(&mid_wav[wavIdx++]);
		if (wavIdx == mid_wav_len)
		{
			wavIdx = 0;
			state--;
			OCR0A = pitch - (state & 0x03);  // Wobble the pitch
		}
	}
	else
	{
		// End
		if(mute)
			myMute = 1;
		data = pgm_read_byte(&end_wav[wavIdx++]);
		if (wavIdx == end_wav_len)
		{
			wavIdx = 0;
			state = 8;
		}
	}

	if(data > 0x80)
	{
		delta = (uint16_t)(data - 0x80) * (uint16_t)256 / (uint16_t)myAttenuation;
		OCR1A = myMute ? 0x7F : 0x0080 + delta;
	}
	else
	{
		delta = (uint16_t)(0x0080 - data) * (uint16_t)256 / (uint16_t)myAttenuation;
		OCR1A = myMute ? 0x7F : 0x0080 - delta;
	}
}

int main(void)
{
	uint8_t i;

	// Deal with watchdog first thing
	MCUSR = 0;								// Clear reset status

	wdt_reset();                     // Reset the WDT, just in case it's still enabled over reset
	wdt_enable(WDTO_1S);             // Enable it at a 1S timeout.

	// Enable 64 MHz PLL and use as source for Timer1
	PLLCSR = _BV(PCKE) | _BV(PLLE);

	// Set up Timer/Counter1 for PWM output on PB1 (OCR1A)
	TIMSK = 0;                                    // Timer interrupts OFF
	TCCR1 = _BV(PWM1A) | _BV(COM1A1) | _BV(CS10); // PWM A, clear on match, 1:1 prescale
	OCR1A = 0x7F;                                 // 50% duty at start

	// Set up Timer/Counter0 for 16kHz interrupt to output samples.
	TCCR0A = _BV(WGM00) | _BV(WGM01);             // Fast PWM (also needs WGM02 in TCCR0B)
	TCCR0B = _BV(WGM02) | _BV(CS01);              // 1/8 prescale
//	OCR0A = 63;                                   // Divide by ~500 (16kHz)
	pitch = 63;

	DDRB |= _BV(PB4) | _BV(PB1);
	PORTB |= _BV(PB3);                            // Turn on pullup for PB3 (enable input)

	sei();
	enableAmplifier();
	mute = 1;
	TIMSK = _BV(OCIE0A);

	_delay_ms(100);  // Wait for PB3 to settle

	uint8_t *eePtr = 0;

	while(1)
	{
		wdt_reset();
		
		if(!(PINB & _BV(PB3)))
		{
			// First Byte: aaabbccc
			// aaa = Duration
			// bb  = Repeats
			// ccc = Pitch
			uint8_t eeVal = eeprom_read_byte(eePtr++);
		
			// Vary the pitch
			pitch = 60 + (eeVal & 0x07);  // 63 is nominal (16kHz)
			OCR0A = pitch;
			
			attenuation = MIN_VOL;
			repeat = (eeVal >> 3) & 0x03;
			uint8_t duration = (eeVal >> 4) & 0x0E;   // 2 to 14, in steps of 2

			// Second Byte: pre-chirp delay
			eeVal = eeprom_read_byte(eePtr++);
			
			while(eeVal--)
			{
				// Pre-chirp delay
				wdt_reset();
				_delay_ms(100);
			}
			
			mute = 0;

			while(attenuation > MAX_VOL)
			{
				// Ramp-up
				wdt_reset();
				if(!newAttenuation)
					attenuation -= 8;
				newAttenuation = 1;
				_delay_ms(2);
			}
			
			// attenuation now equals MAX_VOL
			
			// Wait minimum 16 seconds + duration
			for(i=0; i<((16+duration)*8); i++)   // (16+14)*8 = 30 * 8 = 240 (max)
			{
				wdt_reset();
				_delay_ms(60);  // Should be 125ms, but ISR slows things down
			}

			while(attenuation < MIN_VOL)
			{
				// Ramp-down
				wdt_reset();
				if(!newAttenuation)
					attenuation += 8;
				newAttenuation = 1;
				_delay_ms(2);
			}
			
			mute = 1;
		}
		if(eePtr > (uint8_t *)511)
			eePtr = 0;
	}
}


