/*
 * lfsr.h - lfsr library
 *
 * Copyright (C) 2009 Robert C. Curtis
 *
 * lfsr.h is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * lfsr.h is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with lfsr.h. If not, see <http://www.gnu.org/licenses/>.
 */

/****************************************************************************/

#include <stdint.h>

#ifndef _LFSR_H_
#define _LFSR_H_

void randomSeedSet(uint16_t seed);
uint16_t randomGet();
uint16_t randomRangedGet(uint16_t min, uint16_t max);

#endif
