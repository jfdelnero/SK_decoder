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

#pragma pack(1)

#ifndef BI_BITFIELDS
#define BI_BITFIELDS 3
#endif

typedef struct bitmap_data_
{
	int16_t xsize;
	int16_t ysize;
	int16_t nb_color;
	uint8_t * palette;
	uint32_t  * data;
}bitmap_data;

#ifndef WIN32

typedef struct tagBITMAPFILEHEADER {
	uint16_t   bfType;
	uint32_t   bfSize;
	uint16_t   bfReserved1;
	uint16_t   bfReserved2;
	uint32_t   bfOffBits;
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER {
	uint32_t    biSize;
	uint32_t    biWidth;
	uint32_t    biHeight;
	uint16_t    biPlanes;
	uint16_t    biBitCount;
	uint32_t    biCompression;
	uint32_t    biSizeImage;
	uint32_t    biXPelsPerMeter;
	uint32_t    biYPelsPerMeter;
	uint32_t    biClrUsed;
	uint32_t    biClrImportant;
} BITMAPINFOHEADER;

typedef struct tagRGBQUAD {
	uint8_t rgbBlue;
	uint8_t rgbGreen;
	uint8_t rgbRed;
	uint8_t rgbReserved;
} RGBQUAD;

#endif

#pragma pack()

int bmp_load(char * file,bitmap_data * bdata);
int bmpRLE8b_write(char * file,bitmap_data * bdata);
int bmp24b_write(char * file,bitmap_data * bdata);
int bmp16b_write(char * file,bitmap_data * bdata);
