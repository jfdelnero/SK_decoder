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
// SK MAZ converter
//
///////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmp_file.h"
#include "cache.h"

#include "cmd_param.h"

int rawtobmp(unsigned char *rawdata,int xsize,int ysize,char *filename,char *palettefile1,char *palettefile2)
{
	int i;
	unsigned char palette[256*4];
	bitmap_data bmp;
	file_cache pal_file;
	int numcolor;

	bmp.palette = (unsigned char *)&palette;
	bmp.xsize = xsize;
	bmp.ysize = ysize;
	bmp.nb_color = 256;
	bmp.data = (uint32_t *)rawdata;

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
			printf("Warning : cannot open primary palette file %s\n",palettefile1);
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
			printf("Warning : cannot open secondary palette file %s\n",palettefile2);
		}
	}

	bmpRLE8b_write(filename,&bmp);

	return 0;
}

void drawbox(unsigned char * buf, int bufxsize, int bufysize, int xpos1, int ypos1,int xpos2, int ypos2, unsigned char wall)
{
	int startoffset;
	int x,y;

	startoffset = (ypos1 * bufxsize) + xpos1;

	//////////////////////////////
	// Walls
	//////////////////////////////
	if(wall & 0x8)
	{
		// upper bar
		for( x = 0; x < (xpos2 - xpos1) + 1; x++)
		{
			buf[startoffset + x] = 0;
		}
	}

	if(wall & 0x4)
	{
		// lower bar
		for( x = 0; x < (xpos2 - xpos1) + 1; x++)
		{
			buf[startoffset + x + ((ypos2 - ypos1)*bufxsize)] = 0;
		}
	}

	if(wall & 0x2)
	{
		// left bar
		for( y = 0; y < (ypos2 - ypos1) + 1; y++)
		{
			buf[startoffset + (y*bufxsize)] = 0;
		}
	}

	if(wall & 0x1)
	{
		// right bar
		for( y = 0; y < (ypos2 - ypos1) + 1; y++)
		{
			buf[startoffset + (xpos2 - xpos1) + (y*bufxsize)] = 0;
		}
	}

	//////////////////////////////
	// Doors
	//////////////////////////////

	wall = wall >> 4;

	if(wall & 0x8)
	{
		// upper bar
		for( x = 0; x < (xpos2 - xpos1) + 1; x++)
		{
			buf[startoffset + x] = 0xA0;
		}
	}

	if(wall & 0x4)
	{
		// lower bar
		for( x = 0; x < (xpos2 - xpos1) + 1; x++)
		{
			buf[startoffset + x + ((ypos2 - ypos1)*bufxsize)] = 0xA0;
		}
	}

	if(wall & 0x2)
	{
		// left bar
		for( y = 0; y < (ypos2 - ypos1) + 1; y++)
		{
			buf[startoffset + (y*bufxsize)] = 0xA0;
		}
	}

	if(wall & 0x1)
	{
		// right bar
		for( y = 0; y < (ypos2 - ypos1) + 1; y++)
		{
			buf[startoffset + (xpos2 - xpos1) + (y*bufxsize)] = 0xA0;
		}
	}
}


int export_maz2bmp( char * in_file)
{
	file_cache maz_file;
	int ofs;
	int x,y;
	unsigned char c;
	uint32_t header;
	int step_size;

	char nomfichier[256*2];
	char nomfichier2[256];

	unsigned char * bmp_buf;

	unsigned int xsize,ysize;

	if( open_file( &maz_file, in_file, -1, 0 ) < 0 )
	{
		printf("File access error : %s\n",in_file);
		return -1;
	}

	if(strrchr( in_file,'\\'))
	{
		sprintf(nomfichier2,"%s",strrchr(in_file,'\\')+1);
	}
	else
	{
		sprintf(nomfichier2,"%s",in_file);
	}

	if(strrchr(nomfichier2,'.'))
	{
		*strrchr(nomfichier2,'.')='_';
	}

	step_size = 16;

	ofs = 0;
	header = get_ushort(&maz_file, ofs, NULL);
	if( header != 0x4E4F )
	{
		printf("Bad MAZ header !\n");
		return -1;
	}

	ofs += 2;

	ysize = get_ushort(&maz_file, ofs, NULL);

	ofs += 2;

	xsize = get_ushort(&maz_file, ofs, NULL);

	ofs += 2;

	printf("offset: 0x%.8x res: %d by %d\n",ofs,xsize,ysize);
	sprintf(nomfichier,"%s.bmp",nomfichier2);

	bmp_buf = (unsigned char*)malloc((xsize*step_size)*(ysize*step_size));
	if( bmp_buf )
	{
		memset(bmp_buf,0xFF,(xsize*step_size)*(ysize*step_size));

		for(y=0;y<ysize;y++)
		{
			for(x=0;x<xsize;x++)
			{
				c = get_byte(&maz_file, ofs + ( y * xsize ) + x , NULL);

				drawbox(bmp_buf, (xsize*step_size), (ysize*step_size), x * step_size, y * step_size,((x + 1) * step_size) - 1, ((y + 1) * step_size) - 1, c);
			}
		}

		rawtobmp(bmp_buf,(xsize*step_size),(ysize*step_size),nomfichier,"","");

		free(bmp_buf);
	}
	else
	{
		printf("Fatal Error : cannot allocate %d bytes\n",xsize*ysize*step_size*step_size);
	}

	close_file( &maz_file );

	return 0;
}

#define BMP_MODE 0

int main (int argc, char ** argv)
{
	int i,mode;

	printf("SK MAZ converter V1.0 by HxC2001 (2024 http://hxc2001.free.fr)\n\n");

	if ( argc < 2 )
	{
		printf("Syntax : %s  [-bmp] [maz_file.maz files]\n",argv[0]);
		exit(1);
	}

	mode = BMP_MODE;
	if( isOption(argc, argv,"bmp",NULL, NULL) )
	{
		mode = BMP_MODE;
	}

	i = 1;
	while( i < argc )
	{
		if(argv[i][0] != '-')
		{
			switch(mode)
			{
				case BMP_MODE:
					export_maz2bmp( argv[i] );
				break;
			}
		}
		i++;
	}

	exit(0);
}
