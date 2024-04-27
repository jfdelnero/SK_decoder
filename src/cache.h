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

#define FILE_CACHE_SIZE (64*1024)

typedef struct file_cache_
{
	FILE * f;
	int  current_offset;
	int  cur_page_size;
	int  file_size;
	unsigned char cache_buffer[FILE_CACHE_SIZE];
	int dirty;
}file_cache;

int open_file(file_cache * fc, char* path,unsigned char fill);
unsigned char get_byte(file_cache * fc,int offset, int * success);
int16_t get_short(file_cache * fc,int offset, int * success);
uint16_t get_ushort(file_cache * fc,int offset, int * success);
int32_t get_long(file_cache * fc,int offset, int * success);
uint32_t get_ulong(file_cache * fc,int offset, int * success);
float get_float( file_cache * fc,int offset, int * success);
double get_double( file_cache * fc,int offset, int * success);
void close_file(file_cache * fc);
