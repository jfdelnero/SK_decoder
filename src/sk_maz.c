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
#include <stdarg.h>
#include "bmp_file.h"
#include "cache.h"

#include "font4x6.h"

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

void printchar(unsigned char * buf, int bufxsize, int bufysize, int xpos, int ypos, uint32_t flags, unsigned char c)
{
	int startoffset;
	int charoffset;
	int x,y;
	float level;
	int pix,pix_ofs;

	level = 0.7;
	if(c >= ' ' && c < 128)
	{
		startoffset = (ypos * bufxsize) + xpos;
		charoffset = (c - ' ') * 6;

		for(y=0;y<6;y++)
		{
			for(x=0;x<4;x++)
			{
				pix_ofs = startoffset + (y*bufxsize) + x;
				if(pix_ofs >= 0 && pix_ofs < bufxsize*bufysize )
				{
					if( font4x6[charoffset + y] & (0x80 >> x) )
					{
							pix = buf[pix_ofs];
							buf[pix_ofs] = (uint8_t)((float)pix * level);
					}
					else
					{
						if( flags & 0x1)
						{
							buf[pix_ofs] = 0x10;
						}
					}
				}
			}
		}
	}
}

int bmp_printf(unsigned char * buf, int xsize, int ysize, int xpos, int ypos, uint32_t flags, char * string, ... )
{
	va_list marker;
	va_start( marker, string );
	char strbuf[512];
	int i;

	strbuf[sizeof(strbuf) - 1] = '\0';
	vsnprintf(strbuf,sizeof(strbuf) - 1, string,marker);

	i = 0;
	while(strbuf[i])
	{
		printchar(buf, xsize, ysize, xpos + (i * 5) , ypos, flags, strbuf[i]);
		i++;
	}

	va_end( marker );

	return 0;
}

void fillbox(unsigned char * buf, int bufxsize, int bufysize, int xpos1, int ypos1,int xpos2, int ypos2, unsigned char wall)
{
	int startoffset;
	int x,y;
	int pix;
	float level;

	startoffset = (ypos1 * bufxsize) + xpos1;
	level = (float)(255 - wall) / (float)255;

	for( y = 0; y < (ypos2 - ypos1) + 1; y++)
	{
		for( x = 0; x < (xpos2 - xpos1) + 1; x++)
		{
			pix = buf[startoffset + (y*bufxsize) + x];
			buf[startoffset + (y*bufxsize) + x] = (uint8_t)((float)pix * level);
		}
	}
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


int export_maz2bmp( char * in_file )
{
	file_cache maz_file;
	int ofs;
	int x,y,i,j,k;
	int x_shift, y_shift;
	unsigned char c;
	uint32_t header;
	int step_size;
	int nb_obj,item_size;

	char nomfichier[256*2];
	char nomfichier2[256];

	unsigned char * bmp_buf;

	unsigned int xsize,ysize;

	// #### MAZ formats
	// - Little endian
	// 0x4F 0x4E [uint16_t : line_map_size - 1 (steps)] [uint16_t : col_map_size (steps)] [uint8_t: wall_data (line_map_size*col_map_size][uint8_t: event_data ? (line_map_size*col_map_size]
	// [number_of_objects ?][uint16_t:code, uint16_t:xpos, uint16_t:ypos, 12 uint8_t obj data  ]

	printf("MAZ file : %s\n", in_file);

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

	step_size = 32;

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

	printf("offset: 0x%.8x map size: %d by %d\n",ofs,xsize,ysize);
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

		ofs += (ysize * xsize);

		for(y=0;y<ysize;y++)
		{
			for(x=0;x<xsize;x++)
			{
				c = get_byte(&maz_file, ofs + ( y * xsize ) + x , NULL);

				fillbox(bmp_buf, (xsize*step_size), (ysize*step_size), x * step_size, y * step_size,((x + 1) * step_size) - 1, ((y + 1) * step_size) - 1, c);
			}
		}

		ofs += (ysize * xsize);

		//////////////////////////////////////////////////////////////////////
		nb_obj = get_ushort(&maz_file, ofs, NULL);

		printf("\n-----------------------------------------------------------\n");
		printf("List 1 : %d items (offset 0%X) ...\n\n",nb_obj,ofs);
		ofs += 2;

		for(i=0;i<nb_obj;i++)
		{
			bmp_printf(bmp_buf,  (xsize*step_size), (ysize*step_size),  get_ushort(&maz_file, ofs + 4, NULL ) * step_size + 8, (get_ushort(&maz_file, ofs + 2, NULL ) * step_size) + 8, 0x0001, "<%.2X>", get_ushort(&maz_file, ofs, NULL ) );

			printf("Item %.3d (%.2d,%.2d) (off:0x%.4X) : ",i,get_ushort(&maz_file, ofs + 4, NULL ), (get_ushort(&maz_file, ofs + 2, NULL ) ), ofs);
			for(j=0;j<18;j++)
			{
				printf("%.2X ",get_byte(&maz_file, ofs + j, NULL ));
			}
			printf("\n");

			if( get_ushort(&maz_file, ofs + 4, NULL ) >= xsize)
				printf(" bad x position ! (%d)\n", get_ushort(&maz_file, ofs + 4, NULL ));

			if( get_ushort(&maz_file, ofs + 2, NULL ) >= ysize)
				printf(" bad y position ! (%d)\n", get_ushort(&maz_file, ofs + 2, NULL ));

			ofs += 18;
		}

		//////////////////////////////////////////////////////////////////////
		nb_obj = get_ushort(&maz_file, ofs, NULL);
		printf("\n-----------------------------------------------------------\n");
		printf("Wall decoration / properties list : %d items (offset 0x%X) ...\n\n",nb_obj,ofs);

		ofs += 2;

		for(i=0;i<nb_obj;i++)
		{
			x_shift = (step_size / 2) - ((4+4+1)/2);
			y_shift = (step_size / 2) - ((6)/2);

			switch(get_byte(&maz_file, ofs + 6, NULL ) & 0xF)
			{
				case 0: // Face nord
					y_shift -= (step_size / 3);
				break;
				case 1: // Face ouest
					x_shift -= (step_size / 3);
				break;
				case 2: // Face sud
					y_shift += (step_size / 3);
				break;
				case 3: // Face est
					x_shift += (step_size / 3);
				break;
			}

			bmp_printf(bmp_buf,  (xsize*step_size), (ysize*step_size),  get_ushort(&maz_file, ofs + 4, NULL ) * step_size + x_shift, (get_ushort(&maz_file, ofs + 2, NULL ) * step_size) + y_shift, 0x0000, "W%.2X", get_ushort(&maz_file, ofs, NULL ) );

			printf("Item %.3d (%.2d,%.2d) (off:0x%.4X) : ",i,get_ushort(&maz_file, ofs + 4, NULL ), (get_ushort(&maz_file, ofs + 2, NULL ) ), ofs );
			if( get_ushort(&maz_file, ofs + 8, NULL ) != 0xD499 )
			{
				for(j=0;j<18;j++)
				{
					printf("%.2X ",get_byte(&maz_file, ofs + j, NULL ));
				}
				printf("\n");
			}
			else
			{
				for(j=0;j<10;j++)
				{
					printf("%.2X ",get_byte(&maz_file, ofs + j, NULL ));
				}
				printf("\n");
			}

			if( get_ushort(&maz_file, ofs + 4, NULL ) >= xsize)
				printf(" bad x position ! (%d)\n", get_ushort(&maz_file, ofs + 4, NULL ));

			if( get_ushort(&maz_file, ofs + 2, NULL ) >= ysize)
				printf(" bad y position ! (%d)\n", get_ushort(&maz_file, ofs + 2, NULL ));

			if( get_ushort(&maz_file, ofs + 8, NULL ) != 0xD499 )
			{
				//printf("-- 0x%.4X\n",get_ushort(&maz_file, ofs + 8, NULL ));
				ofs += 8;
			}

			ofs += 10;
		}

		//////////////////////////////////////////////////////////////////////
		nb_obj = get_ushort(&maz_file, ofs, NULL);

		printf("\n-----------------------------------------------------------\n");
		printf("Object list : %d items (offset 0x%X) ...\n\n",nb_obj,ofs);

		ofs += 2;

		for(i=0;i<nb_obj;i++)
		{
			item_size = 0x1C;

			printf("Item %.3d (%.2d,%.2d) (off:0x%.4X) : ",i,get_ushort(&maz_file, ofs + 4, NULL ), (get_ushort(&maz_file, ofs + 2, NULL )), ofs  );
			for(j=0;j<item_size;j++)
			{
				printf("%.2X ",get_byte(&maz_file, ofs + j, NULL ));
			}
			printf("\n");

			if( get_ushort(&maz_file, ofs + 4, NULL ) >= xsize)
				printf(" bad x position ! (%d)\n", get_ushort(&maz_file, ofs + 4, NULL ));

			if( get_ushort(&maz_file, ofs + 2, NULL ) >= ysize)
				printf(" bad y position ! (%d)\n", get_ushort(&maz_file, ofs + 2, NULL ));

			int items = get_ushort(&maz_file, ofs + item_size - 4, NULL );
			if(items)
			{
				for(j = 0; j<items;j++)
				{
					printf("\tL1 sub item %.3d (off:0x%.4X) : ",j, ofs + item_size - 2 );
					for(k=0;k< 16; k++)
					{
						printf("%.2X ", get_byte(&maz_file, ofs + item_size - 2 + k, NULL ) );
					}
					printf("\n");
					item_size += 16;
				}
			}

			int sub_items = get_ushort(&maz_file, ofs + item_size - 2, NULL );
			if(sub_items)
			{
				for(j = 0; j<sub_items;j++)
				{
					printf("\tL2 sub item %.3d (off:0x%.4X) : ", j, ofs + item_size);
					for(k=0;k< 16; k++)
					{
						printf("%.2X ", get_byte(&maz_file, ofs + item_size + k, NULL ) );
					}
					printf("\n");
					item_size += 16;
				}

				int sub_items2 = get_ushort(&maz_file, ofs + item_size - 2, NULL );
				if(sub_items2)
				{
					for(j = 0; j<sub_items2;j++)
					{
						printf("\t\tL3 sub item %.3d (off:0x%.4X) : ", j, ofs + item_size);
						for(k=0;k< 14; k++)
						{
							printf("%.2X ", get_byte(&maz_file, ofs + item_size + k, NULL ) );
						}
						printf("\n");
						item_size += 14;
					}
				}
			}

			if( items || sub_items)
				printf("\n");

			bmp_printf(bmp_buf,  (xsize*step_size), (ysize*step_size),  get_ushort(&maz_file, ofs + 4, NULL ) * step_size + 16, (get_ushort(&maz_file, ofs + 2, NULL ) * step_size) + 16, 0x0000, "o%.2X", get_ushort(&maz_file, ofs, NULL ) );

			ofs += item_size;
		}

		//////////////////////////////////////////////////////////////////////
		nb_obj = get_ushort(&maz_file, ofs, NULL);

		printf("\n-----------------------------------------------------------\n");
		printf("List 4 : %d items (offset 0x%X) ...\n\n",nb_obj,ofs);

		ofs += 2;

		for(i=0;i<nb_obj;i++)
		{
			item_size = 0x12;

			printf("Item %.3d (%.2d,%.2d) (off:0x%.4X) : ",i,get_ushort(&maz_file, ofs + 4, NULL ), (get_ushort(&maz_file, ofs + 2, NULL ) ), ofs);
			for(j=0;j<item_size;j++)
			{
				printf("%.2X ",get_byte(&maz_file, ofs + j, NULL ));
			}
			printf("\n");

			if( get_ushort(&maz_file, ofs + 4, NULL ) >= xsize)
				printf(" bad x position ! (%d)\n", get_ushort(&maz_file, ofs + 4, NULL ));

			if( get_ushort(&maz_file, ofs + 2, NULL ) >= ysize)
				printf(" bad y position ! (%d)\n", get_ushort(&maz_file, ofs + 2, NULL ));

			int items = get_ushort(&maz_file, ofs + item_size - 2, NULL );
			if(items)
			{
				for(j = 0; j<items;j++)
				{
					printf("\tL1 sub item %.3d (off:0x%.4X) : ",j, ofs + item_size);
					for(k=0;k< 14; k++)
					{
						printf("%.2X ", get_byte(&maz_file, ofs + item_size + k, NULL ) );
					}
					printf("\n");
					item_size += 14;
				}

				printf("\n");
			}

			bmp_printf(bmp_buf,  (xsize*step_size), (ysize*step_size),  get_ushort(&maz_file, ofs + 4, NULL ) * step_size + 8, (get_ushort(&maz_file, ofs + 2, NULL ) * step_size) + 16, 0x0000, "M%.2X", get_ushort(&maz_file, ofs, NULL ) );

			ofs += item_size;
		}

		//0x1C  uint16_t type? uint16_t:ypos, uint16_t:xpos
		for(x=0;x<xsize;x++)
		{
			bmp_printf(bmp_buf, (xsize*step_size), (ysize*step_size), (step_size * x) + (step_size / 4), (step_size / 4), 0x0, "%d", x );
			bmp_printf(bmp_buf, (xsize*step_size), (ysize*step_size), (step_size * x) + (step_size / 4), ((ysize-1) * step_size ) + (step_size / 4), 0x0, "%d", x );
		}

		for(y=0;y<ysize;y++)
		{
			bmp_printf(bmp_buf, (xsize*step_size), (ysize*step_size), (step_size / 4) , (step_size * y) + (step_size / 4), 0x0, "%d", y );
			bmp_printf(bmp_buf, (xsize*step_size), (ysize*step_size), ((xsize-1) * step_size ) + (step_size / 4), (step_size * y) + (step_size / 4), 0x0, "%d", y );
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
