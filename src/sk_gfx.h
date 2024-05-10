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
//----------------------------------------- http://jeanfrancoisdelnero.free.fr --//
///////////////////////////////////////////////////////////////////////////////////
//
// SK GFX support (*.MON,*.POJ,*.WPN,*.MSP,*.G* formats)
//
///////////////////////////////////////////////////////////////////////////////////

typedef struct _gfx
{
	int xsize;
	int ysize;
	unsigned char * buf;
	unsigned char * alpha;

	unsigned char pal[256*3];
}gfx;

unsigned int unpackLZ77(file_cache * f, unsigned char * dst, unsigned int max_out_size, int src_offset, int len);
int unpackRLE(unsigned char * dst, unsigned char * alpha, int dstlen, unsigned char * src, int srclen);

gfx * load_gfx( char * in_file, int index, int * nbimg );
int loadgfxpal(gfx *img, char * palfile);
int gfx2bmp(gfx *img, char *filename,char *palettefile1,char *palettefile2);
gfx * unload_gfx( gfx * img );

