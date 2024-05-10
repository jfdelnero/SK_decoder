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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmp_file.h"
#include "cache.h"

#include "sk_gfx.h"

unsigned int unpackLZ77(file_cache * f, unsigned char * dst, unsigned int max_out_size, int src_offset, int len)
{
	unsigned char *P;
	unsigned char * start_dst_p;
	unsigned char * end_dst_p;
	unsigned char bit;
	unsigned char code;
	unsigned short ref;
	int end_src;
	unsigned int unpacked_size;

	unpacked_size = 0;
	end_src = src_offset + len;
	start_dst_p = dst;
	end_dst_p = dst + max_out_size;

	while (src_offset < end_src)
	{
		code = get_byte( f, src_offset++, NULL);

		for( bit = 0;  bit < 8; bit++)
		{
			if ( code & (0x1 << bit) )
			{
				// backreference :
				// 12-bit distance and 4-bit length
				ref = get_ushort( f, src_offset, NULL);
				src_offset += 2;
				P = dst - ( ref >> 4 );
				ref = ( ref & 0xF ) + 3;
				unpacked_size += ref;
				while(ref--)
				{
					if((dst < end_dst_p) && (P >= start_dst_p) &&  (P < end_dst_p) )
					{
						*dst++ = *P++;
					}
				}
			}
			else
			{
				if(dst < end_dst_p)
				{
					*dst++ = get_byte( f, src_offset++, NULL);
				}
				unpacked_size++;
			}
		}
	}

	return unpacked_size;
}

int unpackRLE(unsigned char * dst, unsigned char * alpha, int dstlen, unsigned char * src, int srclen)
{
	#define JUMPPIX 0
	#define COPYPIX 1
	int statemachine;
	unsigned char c;
	int m,l;

	////////////////////////////////////////////////////////////
	// Sprite to bitmap conversion state machine
	//
	l=0;
	m=0;

	c = src[m];

	if( c & 0x80 )
		statemachine=JUMPPIX;
	else
		statemachine=COPYPIX;

	while( l < dstlen && m < srclen - 1 )
	{
		switch(statemachine)
		{
			case JUMPPIX:
				c = c & 0x7f;
				l += c;
				m++;
				c = src[m];
				if( c & 0x80 )
					statemachine = JUMPPIX;
				else
					statemachine = COPYPIX;
			break;

			case COPYPIX:
				m++;
				do
				{
					if( l < dstlen && m < srclen )
					{
						dst[l] = src[m];

						if(alpha)
							alpha[l] = 0xFF;
					}

					l++;
					m++;
					c--;
				}while(c!=0);

				if( m >= srclen )
					return 1;

				c = src[m];
				if( !( c & 0x80 ) )
					statemachine = COPYPIX;
				else
					statemachine = JUMPPIX;

			break;

			default:
			break;
		}
	}

	return l;
}

gfx * load_gfx( char * in_file, int index, int * nbimg )
{
	int i;
	file_cache gfx_file;
	uint32_t ptr1;

	unsigned char * bufferout;
	unsigned char * bufferout2;

	int nbrimage;
	int blocksize;
	int unpackedsize;

	unsigned int xsize,ysize;
	gfx * img;

	img = NULL;

	if( open_file( &gfx_file, in_file, -1, 0 ) < 0 )
	{
		printf("File access error : %s\n",in_file);
		return NULL;
	}

	nbrimage = get_byte( &gfx_file, 0, NULL);

	if( nbimg )
	{
		*nbimg = nbrimage;

		close_file( &gfx_file );

		return NULL;
	}

	if( index >= nbrimage )
	{
		close_file( &gfx_file );

		return NULL;
	}

	ptr1 = get_ulong(&gfx_file, (index*6)+2, NULL);
	blocksize = get_ushort(&gfx_file, (index*6) + 4 + 2, NULL);

	if( blocksize &&  ptr1 )
	{
		xsize = get_ushort(&gfx_file, ptr1 + 2, NULL);
		ysize = get_ushort(&gfx_file, ptr1 + 2 + 2, NULL);

		//printf("%d : offset: 0x%.8x blocksize: 0x%.8x res: %d by %d\n",index,ptr1,blocksize,xsize,ysize);

		bufferout = (unsigned char*)malloc(xsize*ysize*2);
		if( bufferout )
		{
			memset(bufferout,0x00,xsize*ysize*2);

			unpackedsize = unpackLZ77( &gfx_file, (unsigned char *)bufferout, xsize*ysize*2, ptr1+20, blocksize-20);

			if( unpackedsize > xsize*ysize*2 )
			{
				printf("Warning : unpack error !\n");
			}

			// xsize*ysize*2 : First part for the image, the second for the alpha channel.
			bufferout2=(unsigned char*)malloc(xsize*ysize*2);
			if(bufferout2)
			{
				memset(bufferout2,0x00,xsize*ysize*2);

				unpackRLE( bufferout2, &bufferout2[xsize*ysize], xsize*ysize, bufferout, unpackedsize);

				/////////////////////////////////////////////////
				// Write bitmap to a bmp file
				//
				//

				img = malloc( sizeof(gfx) );
				if( img )
				{
					memset( img, 0, sizeof(gfx) );

					for(i=0;i<256;i++)
					{
						img->pal[(i*3) + 0] = i;
						img->pal[(i*3) + 1] = i;
						img->pal[(i*3) + 2] = i;
					}

					img->xsize = xsize;
					img->ysize = ysize;

					img->buf = bufferout2;
					img->alpha = &bufferout2[xsize * ysize];
				}
				else
				{
					free(bufferout2);
				}
			}
			else
			{
				printf("Fatal Error : cannot allocate %d bytes\n",xsize*ysize*2);
			}

			free(bufferout);
		}
		else
		{
			printf("Fatal Error : cannot allocate %d bytes\n",xsize*ysize*2);
		}
	}

	close_file( &gfx_file );

	return img;
}

gfx * unload_gfx( gfx * img )
{
	if( img )
	{
		free(img->buf);
		free(img);
	}

	return NULL;
}

int loadgfxpal(gfx *img, char * palfile)
{
	file_cache pal_file;
	int numcolor,i;

	if( open_file( &pal_file, palfile, -1, 0 ) >= 0 )
	{
		numcolor = get_ushort( &pal_file, 0, NULL);
		for(i=0;i<numcolor;i++)
		{
			img->pal[(i*3) + 0] = get_byte( &pal_file, 2 + (i*3)+2, NULL) * 4;
			img->pal[(i*3) + 1] = get_byte( &pal_file, 2 + (i*3)+1, NULL) * 4;
			img->pal[(i*3) + 2] = get_byte( &pal_file, 2 + (i*3)+0, NULL) * 4;
		}

		close_file( &pal_file );

		return 1;
	}
	else
	{
		//printf("Warning : cannot open primary palette file %s\n",palfile);
	}

	return -1;
}

int gfx2bmp(gfx *img, char *filename,char *palettefile1,char *palettefile2)
{
	int i;
	unsigned char palette[256*4];
	bitmap_data bmp;
	file_cache pal_file;
	int numcolor;

	bmp.palette = (unsigned char *)&palette;
	bmp.xsize = img->xsize;
	bmp.ysize = img->ysize;
	bmp.nb_color = 256;
	bmp.data = (uint32_t *)img->buf;

	//init palette

	if(!strlen(palettefile1) && !strlen(palettefile2) )
	{
		for(i=0;i<256;i++)
		{
			bmp.palette[(i*4) + 0] = i;
			bmp.palette[(i*4) + 1] = i;
			bmp.palette[(i*4) + 2] = i;
		}
	}
	else
	{
		for(i=0;i<256;i++)
		{
			bmp.palette[i*4] = 0;
			bmp.palette[i*4+1] = 0;
			bmp.palette[i*4+2] = 0;
		}
	}

	if(strlen(palettefile1))
	{
		if( open_file( &pal_file, palettefile1, -1, 0 ) >= 0 )
		{
			numcolor = get_ushort( &pal_file, 0, NULL);
			for(i=0;i<numcolor;i++)
			{
				bmp.palette[(i*4)]   = get_byte( &pal_file, (i*3)+2, NULL) * 4;
				bmp.palette[(i*4)+1] = get_byte( &pal_file, (i*3)+1, NULL) * 4;
				bmp.palette[(i*4)+2] = get_byte( &pal_file, (i*3)+0, NULL) * 4;
			}

			close_file( &pal_file );
		}
		else
		{
			//printf("Warning : cannot open primary palette file %s\n",palettefile1);
		}
	}

	if(strlen(palettefile2))
	{
		if( open_file( &pal_file, palettefile2, -1, 0 ) >= 0 )
		{
			numcolor = get_ushort( &pal_file, 0, NULL);
			for(i=0;i<numcolor;i++)
			{
				bmp.palette[(i*4)]   = get_byte( &pal_file, (i*3)+2, NULL) * 4;
				bmp.palette[(i*4)+1] = get_byte( &pal_file, (i*3)+1, NULL) * 4;
				bmp.palette[(i*4)+2] = get_byte( &pal_file, (i*3)+0, NULL) * 4;
			}

			close_file( &pal_file );
		}
		else
		{
			//printf("Warning : cannot open secondary palette file %s\n",palettefile2);
		}
	}

	bmpRLE8b_write(filename,&bmp);

	return 0;
}
