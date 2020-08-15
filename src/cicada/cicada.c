/*************************************************************************
Title:    Cicada Sound Module
Authors:  Nathan D. Holmes <maverick@drgw.net>
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
#define MAX_VOL          (uint16_t)32
#define MIN_VOL         (uint16_t)512

uint16_t volume = 255;
volatile uint8_t newVolume = 0;
uint8_t mute = 1;

inline void disableAmplifier()
{
	PORTB &= ~_BV(PB4);
}

inline void enableAmplifier()
{
	PORTB |= _BV(PB4);
}

uint8_t ocr0a;

// 16kHz interrupt to load high speed PWMs
ISR(TIMER0_COMPA_vect) 
{
	static uint16_t wavIdx = 0;
	static uint8_t state = 0;
	static uint16_t myVolume = 255;
	static uint8_t myMute = 1;

	uint16_t delta;
	uint8_t data;

	if(newVolume)
	{
		myVolume = volume;
		newVolume = 0;
	}
	
	switch(state)
	{
		case 0:
			// Start
			if(!mute)
				myMute = 0;
			OCR0A = ocr0a;
			data = pgm_read_byte(&start_wav[wavIdx++]);
			if (wavIdx == start_wav_len)
			{
				wavIdx = 0;
				state++;
			}
			break;
		case 1:
		case 2:
		case 3:
		case 4:
			// Middle
			switch(state)
			{
				case 1:
					OCR0A = ocr0a;
					break;
				case 2:
					OCR0A = ocr0a + 1;
					break;
				case 3:
					OCR0A = ocr0a;
					break;
				case 4:
					OCR0A = ocr0a - 1;
					break;
			}
			data = pgm_read_byte(&mid_wav[wavIdx++]);
			if (wavIdx == mid_wav_len)
			{
				wavIdx = 0;
				state++;
			}
			break;
		default:
			// End
			if(mute)
				myMute = 1;
			OCR0A = ocr0a;
			data = pgm_read_byte(&end_wav[wavIdx++]);
			if (wavIdx == end_wav_len)
			{
				wavIdx = 0;
				state++;
			}
			break;
	}

	if(data > 0x80)
	{
		delta = (uint16_t)(data - 0x80) * (uint16_t)MAX_VOL / (uint16_t)myVolume;
		OCR1A = myMute ? 0x7F : 0x0080 + delta;
	}
	else
	{
		delta = (uint16_t)(0x0080 - data) * (uint16_t)MAX_VOL / (uint16_t)myVolume;
		OCR1A = myMute ? 0x7F : 0x0080 - delta;
	}

	if(state > 5)
	{
		state = 0;
	}
}

int main(void)
{
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

	DDRB |= _BV(PB4) | _BV(PB1);
	PORTB |= _BV(PB3);                            // Turn on pullup for PB3 (enable input)

	uint8_t dly = 1;

	sei();
	enableAmplifier();

	uint16_t counter;

	while(1)
	{
		wdt_reset();
		if(!(PINB & _BV(PB3)))
		{
			// Vary the pitch
			ocr0a = 60 + 3*((dly/8)%4);  // 63 is nominal (16kHz)
			OCR0A = ocr0a;
			
			counter = 65535;
			volume = MIN_VOL;
			mute = 1;
			TIMSK = _BV(OCIE0A);

			while(counter)
			{
				wdt_reset();
				if(counter > ATTACK_START)
				{
					// Initial pause
					mute = 1;
				}
				else if(counter > (ATTACK_START - ((MIN_VOL-MAX_VOL)*ATTACK_TIME) - 1))
				{
					// Ramp up
					mute = 0;
					if(!newVolume)
						volume = MIN_VOL - ((uint16_t)(ATTACK_START - counter) / ATTACK_TIME);
					newVolume = 1;
				}
				else if(counter > DECAY_START)
				{
					// Keep playing
				}
				else if(counter > (DECAY_START - ((MIN_VOL-MAX_VOL)*DECAY_TIME) - 1))
				{
					// Decay down
					if(!newVolume)
						volume = MAX_VOL + ((uint16_t)(DECAY_START - counter) / DECAY_TIME);
					newVolume = 1;
				}
				else
				{
					// Pause afterwards
					mute = 1;
					uint8_t i = dly;
					while(i)
					{
						i--;
						wdt_reset();
						_delay_ms(400);
					}
					break;  // Leave while(counter) loop
				}
				counter--;
				_delay_us(500);
			}
			// Really, really, really, crude Linear Congruential Method (a=5, c=0, m=256, X0 = 1), repeat period of 64, min = 1, max = 253
			dly *= 5;
		}
	}
}


