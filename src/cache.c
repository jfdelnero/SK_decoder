/*
//
// Copyright (C) 2002-2024 Jean-Fran�ois DEL NERO
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "cache.h"

int open_file(file_cache * fc, char* path, int filesize,unsigned char fill)
{
	memset(fc,0,sizeof(file_cache));

	if( filesize < 0 )
	{   // Read mode
		fc->f = fopen(path,"rb");
		if(fc->f)
		{
			if(fseek(fc->f,0,SEEK_END))
				goto error;

			fc->file_size = ftell(fc->f);

			if(fseek(fc->f,fc->current_offset,SEEK_SET))
				goto error;

			if(fc->current_offset + FILE_CACHE_SIZE > fc->file_size)
				fc->cur_page_size = ( fc->file_size - fc->current_offset);
			else
				fc->cur_page_size = FILE_CACHE_SIZE;

			memset(&fc->cache_buffer,fill,FILE_CACHE_SIZE);

			if(fc->file_size)
			{
				if( fread(&fc->cache_buffer,fc->cur_page_size,1,fc->f) != 1 )
					goto error;
			}

			return 0;
		}
	}
	else
	{   // write mode
		fc->f = fopen(path,"wb");
		if(fc->f)
		{
			memset(&fc->cache_buffer,fill,FILE_CACHE_SIZE);

			while( fc->file_size < filesize)
			{

				if( fc->file_size + FILE_CACHE_SIZE > filesize )
				{
					if( fwrite( &fc->cache_buffer, filesize - fc->file_size, 1, fc->f ) != 1 )
						goto error;

					fc->file_size += (filesize - fc->file_size);
				}
				else
				{
					if( fwrite( &fc->cache_buffer, FILE_CACHE_SIZE, 1, fc->f ) != 1 )
						goto error;

					fc->file_size += FILE_CACHE_SIZE;
				}
			}

			fclose(fc->f);

			fc->current_offset = 0;

			fc->f = fopen(path,"r+b");
			if(fc->f)
			{
				if(fseek(fc->f,fc->current_offset,SEEK_END))
					goto error;

				fc->file_size = ftell(fc->f);

				if(fseek(fc->f,fc->current_offset,SEEK_SET))
					goto error;

				if(fc->current_offset + FILE_CACHE_SIZE > fc->file_size)
					fc->cur_page_size = ( fc->file_size - fc->current_offset);
				else
					fc->cur_page_size = FILE_CACHE_SIZE;

				if( fread(&fc->cache_buffer,fc->cur_page_size,1,fc->f) != 1 )
					goto error;

				if(fseek(fc->f,fc->current_offset,SEEK_SET))
					goto error;
			}
			else
			{
				goto error;
			}

			return 0;
		}
	}

	return -1;

error:
	if(fc->f)
		fclose(fc->f);

	fc->f = 0;

	return -1;
}

unsigned char get_byte(file_cache * fc,int offset, int * success)
{
	unsigned char byte;
	int ret;

	byte = 0xFF;
	ret = 1;

	if(fc)
	{
		if(offset < fc->file_size)
		{
			if( ( offset >= fc->current_offset ) &&
				( offset <  (fc->current_offset + FILE_CACHE_SIZE) ) )
			{
				byte = fc->cache_buffer[ offset - fc->current_offset ];
			}
			else
			{
				fc->current_offset = (offset & ~(FILE_CACHE_SIZE-1));
				fseek(fc->f, fc->current_offset,SEEK_SET);

				memset(&fc->cache_buffer,0xFF,FILE_CACHE_SIZE);

				if(fc->current_offset + FILE_CACHE_SIZE < fc->file_size)
					ret = fread(&fc->cache_buffer,FILE_CACHE_SIZE,1,fc->f);
				else
					ret = fread(&fc->cache_buffer,fc->file_size - fc->current_offset,1,fc->f);

				byte = fc->cache_buffer[ offset - fc->current_offset ];
			}
		}
	}

	if(success)
	{
		*success = ret;
	}

	return byte;
}

int set_byte(file_cache * fc,unsigned int offset, unsigned char byte)
{
	if(fc)
	{
		if(offset < fc->file_size)
		{
			if( ( offset >= fc->current_offset ) &&
				( offset <  (fc->current_offset + FILE_CACHE_SIZE) ) )
			{
				fc->cache_buffer[ offset - fc->current_offset ] = byte;
				fc->dirty = 1;
			}
			else
			{
				if( fc->dirty )
				{
					if(fseek(fc->f, fc->current_offset, SEEK_SET))
						goto error;

					if( fwrite( &fc->cache_buffer, fc->cur_page_size, 1, fc->f ) != 1 )
						goto error;

					fc->dirty = 0;
				}

				fc->current_offset = (offset & ~(FILE_CACHE_SIZE-1));
				if(fc->current_offset + FILE_CACHE_SIZE > fc->file_size)
					fc->cur_page_size = ( fc->file_size - fc->current_offset );
				else
					fc->cur_page_size = FILE_CACHE_SIZE;

				if(fseek(fc->f, fc->current_offset,SEEK_SET))
					goto error;

				memset(&fc->cache_buffer,0xFF,FILE_CACHE_SIZE);

				if( fread(&fc->cache_buffer,fc->cur_page_size,1,fc->f) != 1 )
					goto error;

				fc->cache_buffer[ offset - fc->current_offset ] = byte;

				fc->dirty = 1;
			}
		}
	}

	return 1;

error:
	return 0;
}


int16_t get_short(file_cache * fc,int offset, int * success)
{
	uint16_t val;

	val = 0;

	val = get_byte(fc, offset++, success);
	val |= (((unsigned short)get_byte(fc, offset, success))<<8);

	return (int16_t)(val);
}

int set_ushort(file_cache * fc,unsigned int offset, uint16_t data)
{
	set_byte(fc,offset, data & 0xFF);
	return set_byte(fc,offset+1, (data>>8) & 0xFF);
}

int set_ulong(file_cache * fc,unsigned int offset, uint32_t data)
{
	set_ushort(fc,offset, data & 0xFFFF);
	return set_ushort(fc,offset+2, (data>>16) & 0xFFFF);
}

uint16_t get_ushort(file_cache * fc,int offset, int * success)
{
	uint16_t val;

	val = 0;

	val = get_byte(fc, offset++, success);
	val |= (((unsigned short)get_byte(fc, offset, success))<<8);

	return val;
}

uint32_t get_ulong(file_cache * fc,int offset, int * success)
{
	uint32_t val;

	val = 0;

	val = get_byte(fc, offset++, success);
	val |= (((unsigned short)get_byte(fc, offset++, success))<<8);
	val |= (((unsigned short)get_byte(fc, offset++, success))<<16);
	val |= (((unsigned short)get_byte(fc, offset++, success))<<24);

	return val;
}

int32_t get_long(file_cache * fc,int offset, int * success)
{
	uint32_t val;

	val = get_ulong(fc, offset, success);

	return (int32_t)(val);
}

float get_float( file_cache * fc,int offset, int * success)
{
	uint32_t val;
	float fval;

	val = get_ulong(fc, offset, success);
	memcpy((void*)&fval,(void*)&val,4);

	return fval;
}

double get_double( file_cache * fc,int offset, int * success)
{
	int i;
	uint8_t val[8];
	double dval;

	for(i=0;i<8;i++)
		val[i] = get_byte(fc, offset + i, success);


	memcpy((void*)&dval,(void*)&val,8);

	return dval;
}

void close_file(file_cache * fc)
{
	if(fc)
	{
		if(fc->f)
		{
			if( fc->dirty )
			{
				if(fseek(fc->f, fc->current_offset, SEEK_SET))
					goto error;

				if( fwrite( &fc->cache_buffer, fc->cur_page_size, 1, fc->f ) != 1 )
					goto error;

				fc->dirty = 0;
			}

			fclose(fc->f);

			fc->f = NULL;
		}
	}

	return;

error:
	fclose(fc->f);

	return;
}
