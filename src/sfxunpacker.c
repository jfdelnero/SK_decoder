/*
//
// Copyright (C) 2002-2024 Jean-François DEL NERO
//
// This file is part of SK_decoder.
//
// SK_decoder may be used and distributed without restriction provided
// that this copyright statement is not removed from the file and that any
// derivative work contains the original copyright notice and the associated
// disclaimer.
//
// SK_decoder is free software; you can redistribute it
// and/or modify  it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// SK_decoder is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//   See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with SK_decoder; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//
*/

///////////////////////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------//
//-----------H----H--X----X-----CCCCC----22222----0000-----0000------11----------//
//----------H----H----X-X-----C--------------2---0----0---0----0--1--1-----------//
//---------HHHHHH-----X------C----------22222---0----0---0----0-----1------------//
//--------H----H----X--X----C----------2-------0----0---0----0-----1-------------//
//-------H----H---X-----X---CCCCC-----222222----0000-----0000----1111------------//
//-------------------------------------------------------------------------------//
//----------------------------------------------------- http://hxc2001.free.fr --//
///////////////////////////////////////////////////////////////////////////////////
//
// SK sound FX converter *.CPR -> *.WAV
//
///////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <stdio.h>
#include "cache.h"

int unpacksfx(file_cache * in_file, int in_offset, unsigned char *bufferout, unsigned int sizebuffer)
{
	uint32_t shift_in;
	int shift_bit_count;
	int prev_word_size;
	int word_size;
	int word_max_val;
	unsigned char *bufferout_end;
	int diff,diff2,diff3,diff4;
	int signed_prev_sample;
	int sample;
	int l,g,nbgrp;

	bufferout_end = bufferout + sizebuffer;

	nbgrp = sizebuffer / 8;

	diff2 = 0;
	diff3 = 0;
	diff4 = 0;

	signed_prev_sample = 0;

	shift_in = 0x00000000;
	shift_bit_count = 1;

	prev_word_size = 1;
	word_max_val = 2;

	while( nbgrp )
	{
		word_size = prev_word_size;

		shift_bit_count--;
		if( !shift_bit_count )
		{
			// shift in next byte...
			shift_in = get_byte(in_file, in_offset++, NULL);
			shift_bit_count = 8;
		}

		shift_in <<= 1;

		// New word size code ?
		if( shift_in & 0x100 )
		{
			shift_in &= 0xFF;

			// Get the 4 bits word size code
			for(l = 0; l < 4; l++)
			{
				shift_bit_count--;
				if( !shift_bit_count )
				{
					// shift in next byte...
					shift_in |= get_byte(in_file, in_offset++, NULL);
					shift_bit_count = 8;
				}

				shift_in <<= 1;
			}

			word_max_val = ((0x1<<((shift_in >> 8) & 0xF)));
			word_size = ((shift_in >> 8) & 0xF) - 1;
		}

		for(g=0;g<8;g++)
		{
			if( word_size != 0xA )
			{
				// Get the next word ...

				shift_in &= 0xFF;

				for(l = 0; l < word_size + 1; l++)
				{
					shift_bit_count--;
					if( !shift_bit_count)
					{
						// shift in next byte...
						shift_in |= get_byte(in_file, in_offset++, NULL);
						shift_bit_count = 8;
					}

					shift_in <<= 1;
				}

				diff = (shift_in >> 8) & 0xFFF;
				if( diff & (word_max_val >> 1) )
				{
					diff = (diff - word_max_val);
				}

				diff = -diff;
				diff = diff * 4;
			}
			else
			{
				diff = 0;
			};

			// Calc the new sample
			sample = ( ( ( diff + (diff2 * 2) ) - ( diff4 + diff3 ) + ( signed_prev_sample * 4 ) ) + ( 0x80 * 4 ) ) / 4;

			// Prepare next step...
			diff4 = diff3;
			diff3 = diff2;
			diff2 = (sample - 0x80 ) - signed_prev_sample;
			signed_prev_sample = (sample - 0x80);

			if( bufferout < bufferout_end )
			{
				*bufferout++ = (unsigned char)(sample & 0xFF);
			}
			else
			{
				return -1;
			}

		}

		prev_word_size = word_size;

		nbgrp--;
	}

	return sizebuffer;
 }
