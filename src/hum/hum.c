/*************************************************************************
Title:    Railroad Crossing Bell Module
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
    Copyright (C) 2019 Michael Petersen, Nathan Holmes, with portions from 
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

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>

//#include "hum-16k.h"
#include "hum-18k.h"

// 8MHz / 8 / 63 = 15.873kHz
//#define OCR0A_DEFAULT 63

// 8MHz / 8 / 56 = 17.857kHz
#define OCR0A_DEFAULT 56

// playBell acts as an indicator between the main loop and 
// the ISR to tell it to shut down playback once it hits the end
// of the bell sound sample

volatile uint8_t playBell = 0;

inline void disableAmplifier()
{
	PORTB &= ~_BV(PB4);
}

inline void enableAmplifier()
{
	PORTB |= _BV(PB4);
}


// 16kHz interrupt to load high speed PWMs
ISR(TIMER0_COMPA_vect) 
{
	static uint16_t wavIdx = 0;
	uint8_t sample = pgm_read_byte(&wav[wavIdx++]);
	OCR1A = sample;

	if (wavIdx == wav_len)
	{
		wavIdx = 0;
		// Make sure we only stop at the end of the bell sound, otherwise it may cut off
		// mid-cycle and sound very strange
		if (!playBell)
		{
			// Stop timer0 ISR
			TIMSK = 0;
			// Stop speaker output, center output
			OCR1A = 0x7F;
			disableAmplifier();
		}
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

	// Set up Timer/Counter0 for interrupts to output samples.
	TCCR0A = _BV(WGM00) | _BV(WGM01);             // Fast PWM (also needs WGM02 in TCCR0B)
	TCCR0B = _BV(WGM02) | _BV(CS01);              // 1/8 prescale
	OCR0A = OCR0A_DEFAULT;

	DDRB = _BV(PB4) | _BV(PB1);
	PORTB |= _BV(PB3);                            // Turn on pullup for PB3 (enable input)
	PORTB |= _BV(PB0); // FIXME
	disableAmplifier();
	sei();

	wdt_reset();
	_delay_ms(400);  // Wait for PWM and PB3 to settle

	uint8_t state = 0;

	while(1)
	{
		wdt_reset();

		switch(state)
		{
			case 0:
			default:
				// Idle & off
				playBell = 0;
				if(!(PINB & _BV(PB3)))
					state = 1;  // Go to playing
				break;
			case 1:
				// Playing
				enableAmplifier();
				playBell = 1;
				TIMSK = _BV(OCIE0A);
				if(PINB & _BV(PB3))
					state = 2;  // Go to off delay
				break;
			case 2:
			{
				// Off delay
#ifdef OFFDELAY
				uint8_t offTicks = 155;  // Should be 200, but ISR wastes some time, so adjusted empirically
				while(offTicks--)
				{
					// Delay 5s
					wdt_reset();
					_delay_ms(25);
					if(!(PINB & _BV(PB3)))
						state = 1;  // Go back to playing if enable pin is low
				}
#endif
				state = 0;  // Go to idle
				break;
			}
		}

	}
	
}

