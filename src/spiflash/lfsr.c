/*
 * lfsr.c - lfsr library
 *
 * Copyright (C) 2009 Robert C. Curtis (with a few modifications - NDH)
 *
 * lfsr.c is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * lfsr.c is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with lfsr.c. If not, see <http://www.gnu.org/licenses/>.
 */

/****************************************************************************/

#include <stdint.h>
#include "lfsr.h"

uint16_t random = 0xBEEF;

void randomSeedSet(uint16_t seed)
{
	// LFSRs react poorly to being seeded with zero
	// Thus, leave the default seed if we wind up this
	// extraordinarily unlikely scenario
	if (0 != seed)
		random = seed;
}

uint16_t randomGet()
{
	uint16_t tap = 0;
	int i = 0;
	const uint16_t const taps[4] = {(1 << 15), (1 << 14), (1 << 12), (1 << 3)};

	while(i < sizeof(taps) / sizeof(taps[0]))
		tap ^= !!(taps[i++] & random);
	random <<= 1;
	random |= tap;
	return random;
}

uint16_t randomRangedGet(uint16_t min, uint16_t max)
{
	uint16_t random = randomGet();
	return (random % (max-min)) + min;
}
