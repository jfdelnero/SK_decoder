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
#include "sk_gfx.h"
#include "font4x6.h"

#include "cmd_param.h"

#include "sk_id.h"
#include "utils.h"

int rawto24bbmp(uint32_t *rawdata,int xsize,int ysize,char *filename)
{
	bitmap_data bmp;

	bmp.xsize = xsize;
	bmp.ysize = ysize;
	bmp.data = (uint32_t *)rawdata;

	bmp24b_write(filename, &bmp);

	return 0;
}

void printchar(uint32_t * buf, int bufxsize, int bufysize, int xpos, int ypos, uint32_t flags, unsigned char c)
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
							buf[pix_ofs] = 0x101010;
						}
					}
				}
			}
		}
	}
}

int bmp_printf(uint32_t * buf, int xsize, int ysize, int xpos, int ypos, uint32_t flags, char * string, ... )
{
	va_list marker;
	va_start( marker, string );
	char strbuf[512];
	int i;

	strbuf[sizeof(strbuf) - 1] = '\0';
	vsnprintf(strbuf,sizeof(strbuf) - 1, string,marker);

	if( flags & (0x1<<1) )
	{
		i = 0;
		// center
		while(strbuf[i])
		{
			i++;
		}
		ypos -= (6/2);
		xpos -= (i*5/2);
	}

	i = 0;
	while(strbuf[i])
	{
		printchar(buf, xsize, ysize, xpos + (i * 5) , ypos, flags, strbuf[i]);
		i++;
	}

	va_end( marker );

	return 0;
}

void fillbox(uint32_t * buf, int bufxsize, int bufysize, int xpos1, int ypos1,int xpos2, int ypos2, unsigned char wall)
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
			buf[startoffset + (y*bufxsize) + x] = ((uint32_t)((float)((pix>>0)&0xFF) * level)<<0) |
												  ((uint32_t)((float)((pix>>8)&0xFF) * level)<<8) |
												  ((uint32_t)((float)((pix>>16)&0xFF) * level)<<16);
		}
	}
}

void drawbox(uint32_t * buf, int bufxsize, int bufysize, int xpos1, int ypos1,int xpos2, int ypos2, unsigned char wall)
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

int calc_tile_offset(int off_code, int stepsize)
{
	if(off_code > 4)
		off_code = 4;

	if(off_code < -4)
		off_code = -4;

	return (stepsize / 2) + ((stepsize / 9) * off_code);
}

static uint32_t rgb(uint8_t r, uint8_t g, uint8_t b)
{
	return ((((uint32_t)b)<<16) | (((uint32_t)g)<<8) | (((uint32_t)r)<<0));
}

void disp_sprite( uint32_t * buf, int bufxsize, int bufysize, gfx * img, int xpos, int ypos)
{
	int dstoffset,srcoffset;
	int x,y;
	uint8_t pix;

	xpos = xpos - (img->xsize/2);
	ypos = ypos - (img->ysize/2);

	srcoffset = 0;
	dstoffset = (ypos * bufxsize) + xpos;

	for(y=0;y<img->ysize;y++)
	{
		for(x=0;x<img->xsize;x++)
		{
			if(
				( xpos + x >= 0 && xpos + x < bufxsize) &&
				( ypos + y >= 0 && ypos + y < bufysize)
			)
			{
				if( img->alpha[srcoffset] )
				{
					dstoffset = ( (ypos + y) * bufxsize ) + ( xpos + x );
					pix = img->buf[srcoffset];
					buf[dstoffset] = rgb(img->pal[(pix*3) + 2],img->pal[(pix*3) + 1],img->pal[(pix*3) + 0]);
				}
				srcoffset++;
			}
		}
	}
}

void get_gpr_folder(char * fullmazpath, char * grp_folder, char * name, char * ext, char * outpath)
{
	int l;

	hxc_getpathfolder(fullmazpath, outpath, SYS_PATH_TYPE);

	l = strlen(outpath);

	outpath[l - 1] = 0;
	while(l && outpath[l] != '/')
	{
		l--;
	}
	outpath[l] = 0;

	strcat(outpath,"/");
	strcat(outpath,grp_folder);
	strcat(outpath,"/");
	if(name)
	{
		strcat(outpath, name );
		strcat(outpath, ext);
	}
}

void getpalfolder(char * fullmazpath, char * palfolder, char * name)
{
	int l;

	hxc_getpathfolder(fullmazpath, palfolder, SYS_PATH_TYPE);

	l = strlen(palfolder);

	palfolder[l - 1] = 0;
	while(l && palfolder[l] != '/')
	{
		l--;
	}
	palfolder[l] = 0;

	strcat(palfolder,"/GROUP24_ungrouped/");
	if(name)
	{
		strcat(palfolder, name );
		strcat(palfolder, ".PAL");
	}
}

void getencfolder(char * fullmazpath, char * palfolder, char * name)
{
	getpalfolder( fullmazpath, palfolder, name);
}

void getspritefolder(char * fullmazpath, char * spritefolder, char * name, char * ext)
{
	hxc_getpathfolder(fullmazpath, spritefolder, SYS_PATH_TYPE);

	strcat(spritefolder,"/");
	if(name)
	{
		strcat(spritefolder, name );
		if(ext)
			strcat(spritefolder, ext );
	}
}

int gen_obj_def(char * in_def_file, char * in_enc_file)
{
	file_cache def_file;
	file_cache enc_file;
	int i, j, l;
	char * enc_buf;
	uint8_t d;
	char tmp_str[128];
	char * ptr;

	for(i=0;i<MAX_OBJ;i++)
	{
		sk_objects[i].id = 0xFFFF;
		sk_objects[i].name = NULL;
		sk_objects[i].desc = NULL;
	}

	// Get the objects id

	if( open_file( &def_file, in_def_file, -1, 0 ) < 0 )
	{
		printf("obj def file access error : %s\n",in_def_file);
		return -1;
	}

	printf("Loading objects definitions ...\n");

	i = 0;
	while( i < def_file.file_size / 56 && i < MAX_OBJ - 1 )
	{
		sk_objects[i].id = i;

		sk_objects[i].name = calloc(1,10);

		j = 0;
		while( get_byte(&def_file, (i * 56) + 6 + j , NULL )  && j < 8 )
		{
			sk_objects[i].name[j] = get_byte(&def_file, (i * 56) + 6 + j , NULL );
			j++;
		}

		sk_objects[i].desc = NULL;

		i++;
	}

	close_file( &def_file );

	// Get the objects descriptions
	if( open_file( &enc_file, in_enc_file, -1, 0 ) < 0 )
	{
		printf("enc script file access error : %s\n",in_def_file);
		return -1;
	}

	enc_buf = malloc( enc_file.file_size + 1 );
	if( enc_buf )
	{
		memset( enc_buf, 0, enc_file.file_size  + 1 );

		d = 0xFF;
		for(i=0;i<enc_file.file_size;i++)
		{
			enc_buf[i] = get_byte(&enc_file, i, NULL ) ^ d;
			d--;
		}


		i = 0;
		while( sk_objects[i].id != 0xFFFF )
		{
			strcpy(tmp_str, "item_");
			strcat(tmp_str, sk_objects[i].name);

			if(strlen(tmp_str) < 9)
				strcat(tmp_str, "  { \"");
			else
				strcat(tmp_str, " { \"");

			j = 0;
			while( tmp_str[j] )
			{
				if( tmp_str[j] >= 'A' && tmp_str[j] <= 'Z' )
				{
					tmp_str[j] = tmp_str[j] + ('a' - 'A');
				}
				j++;
			}

			sk_objects[i].desc = calloc(1,128);

			ptr = strstr(enc_buf, tmp_str);
			if( ptr )
			{
				l = strlen(tmp_str);

				j = 0;
				while( j < 128 - 1 && ptr[l + j] != '"' )
				{
					sk_objects[i].desc[j] = ptr[l + j];
					j++;
				}
			}
			i++;
		}
		free(enc_buf);
	}

	close_file( &enc_file );

#if 0
	i = 0;
	while( sk_objects[i].id != 0xFFFF )
	{
		if( sk_objects[i].name )
		{
			if( !sk_objects[i].desc )
				printf("Object 0x%.4X : %s  \t|\t???\n", sk_objects[i].id, sk_objects[i].name);
			else
				printf("Object 0x%.4X : %s  \t|\t%s\n", sk_objects[i].id, sk_objects[i].name,sk_objects[i].desc);
		}
		else
		{
			printf("Object 0x%.4X : ???\n");
		}
		i++;
	}
#endif
	return 0;
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
	gfx * img;

	char nomfichier[256*2];
	char nomfichier2[256];
	char tmp_str[256];
	char tmp_str2[256*4];

	char enc_file[256*4];
	char def_file[256*4];

	uint32_t * bmp_buf;

	unsigned int xsize,ysize;

	// #### MAZ formats
	// - Little endian
	// 0x4F 0x4E [uint16_t : line_map_size - 1 (steps)] [uint16_t : col_map_size (steps)] [uint8_t: wall_data (line_map_size*col_map_size][uint8_t: event_data ? (line_map_size*col_map_size]
	// [number_of_objects ?][uint16_t:code, uint16_t:xpos, uint16_t:ypos, 12 uint8_t obj data  ]

	printf("MAZ file : %s\n", in_file);

	get_gpr_folder(in_file, "GROUP24_ungrouped", "BOOKINT", ".ENC", enc_file);
	get_gpr_folder(in_file, "GROUP24_ungrouped", "OBJTYPE", ".DEF", def_file);

	gen_obj_def(def_file,enc_file);

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

	step_size = 128;

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

	bmp_buf = (uint32_t*)malloc((xsize*step_size)*(ysize*step_size) * sizeof(uint32_t));
	if( bmp_buf )
	{
		memset(bmp_buf, 0xFF, (xsize*step_size) * (ysize*step_size) * sizeof(uint32_t));

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
			int centerx = (get_ushort(&maz_file, ofs + 4, NULL ) * step_size) + calc_tile_offset( get_short(&maz_file, ofs + 8, NULL ), step_size);
			int centery = (get_ushort(&maz_file, ofs + 2, NULL ) * step_size) + calc_tile_offset(-get_short(&maz_file, ofs + 6, NULL ), step_size);

			if( get_obj_name( get_ushort(&maz_file, ofs, NULL ) ) )
			{
				sprintf(tmp_str,"%s", get_obj_name( get_ushort(&maz_file, ofs, NULL ) ) );

				// Try to load the sprite
				get_gpr_folder( in_file, "GROUP24_ungrouped", get_obj_name( get_ushort(&maz_file, ofs, NULL ) ) ,".POJ", tmp_str2);

				img = load_gfx( tmp_str2, 0, NULL );
				if( img )
				{
					getpalfolder(in_file, (char*)&tmp_str2, "DUNGEO00" );
					loadgfxpal( img, tmp_str2);

					getpalfolder(in_file, (char*)&tmp_str2, get_obj_name( get_ushort(&maz_file, ofs, NULL )) );
					loadgfxpal( img, tmp_str2);

					disp_sprite( bmp_buf, (xsize*step_size), (ysize*step_size), img, centerx, centery);

					img = unload_gfx( img );

					tmp_str[0] = 0;
				}
			}
			else
			{
				sprintf(tmp_str,"item %.2X", get_ushort(&maz_file, ofs, NULL ));
			}

			if(strlen(tmp_str))
			{
				sprintf(tmp_str,"item %.2X", get_ushort(&maz_file, ofs, NULL ));

				bmp_printf(bmp_buf,  (xsize*step_size), (ysize*step_size),
								centerx,
								centery,
								0x0002, tmp_str );
			}


			printf("Item %.3d (%.2d,%.2d) (off:0x%.4X) : ",i,get_ushort(&maz_file, ofs + 4, NULL ), (get_ushort(&maz_file, ofs + 2, NULL ) ), ofs);
			for(j=0;j<18;j++)
			{
				printf("%.2X ",get_byte(&maz_file, ofs + j, NULL ));
			}

			if(get_obj_name(get_ushort(&maz_file, ofs , NULL )))
				printf(" (%s - %s)",get_obj_name(get_ushort(&maz_file, ofs , NULL )), get_obj_desc(get_ushort(&maz_file, ofs , NULL )));

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

			if( get_decoration_name( get_ushort(&maz_file, ofs, NULL ) ) )
			{
				sprintf(tmp_str,"%s", get_decoration_name( get_ushort(&maz_file, ofs, NULL ) ) );
			}
			else
			{
				sprintf(tmp_str,"W%.2X", get_ushort(&maz_file, ofs, NULL ));
			}

			bmp_printf(bmp_buf,  (xsize*step_size), (ysize*step_size),  get_ushort(&maz_file, ofs + 4, NULL ) * step_size + x_shift, (get_ushort(&maz_file, ofs + 2, NULL ) * step_size) + y_shift, 0x0000, tmp_str );

			printf("Decoration %.3d (%.2d,%.2d) (off:0x%.4X) : ",i,get_ushort(&maz_file, ofs + 4, NULL ), (get_ushort(&maz_file, ofs + 2, NULL ) ), ofs );
			if( get_ushort(&maz_file, ofs + 8, NULL ) != 0xD499 )
			{
				for(j=0;j<18;j++)
				{
					printf("%.2X ",get_byte(&maz_file, ofs + j, NULL ));
				}
			}
			else
			{
				for(j=0;j<10;j++)
				{
					printf("%.2X ",get_byte(&maz_file, ofs + j, NULL ));
				}
			}

			if(get_decoration_name(get_ushort(&maz_file, ofs , NULL )))
				printf(" (%s)",get_decoration_name(get_ushort(&maz_file, ofs , NULL )));

			printf("\n");

			if( get_ushort(&maz_file, ofs + 4, NULL ) >= xsize)
				printf(" bad x position ! (%d)\n", get_ushort(&maz_file, ofs + 4, NULL ));

			if( get_ushort(&maz_file, ofs + 2, NULL ) >= ysize)
				printf(" bad y position ! (%d)\n", get_ushort(&maz_file, ofs + 2, NULL ));

			if( get_ushort(&maz_file, ofs + 8, NULL ) != 0xD499 )
			{
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

			printf("Object %.3d (%.2d,%.2d) (off:0x%.4X) : ",i,get_ushort(&maz_file, ofs + 4, NULL ), (get_ushort(&maz_file, ofs + 2, NULL )), ofs  );
			for(j=0;j<item_size;j++)
			{
				printf("%.2X ",get_byte(&maz_file, ofs + j, NULL ));
			}

			if(get_fixed_obj_name(get_ushort(&maz_file, ofs , NULL )))
				printf(" (%s)",get_fixed_obj_name(get_ushort(&maz_file, ofs , NULL )));

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
					printf("\tObject_sub_item %.3d (off:0x%.4X) : ",j, ofs + item_size - 2 );
					for(k=0;k< 16; k++)
					{
						printf("%.2X ", get_byte(&maz_file, ofs + item_size - 2 + k, NULL ) );
					}

					if(get_obj_name(get_ushort(&maz_file, ofs + item_size - 2 , NULL )))
						printf(" (%s - %s)",get_obj_name(get_ushort(&maz_file, ofs + item_size - 2 , NULL )), get_obj_desc(get_ushort(&maz_file, ofs + item_size - 2 , NULL )));

					printf("\n");
					item_size += 16;
				}
			}

			int sub_items = get_ushort(&maz_file, ofs + item_size - 2, NULL );
			if(sub_items)
			{
				for(j = 0; j<sub_items;j++)
				{
					printf("\tObject_sub_item2 %.3d (off:0x%.4X) : ", j, ofs + item_size);
					for(k=0;k< 16; k++)
					{
						printf("%.2X ", get_byte(&maz_file, ofs + item_size + k, NULL ) );
					}

					if(get_obj_name(get_ushort(&maz_file, ofs + item_size , NULL )))
						printf(" (%s)",get_obj_name(get_ushort(&maz_file, ofs + item_size , NULL )));

					printf("\n");
					item_size += 16;
				}

				int sub_items2 = get_ushort(&maz_file, ofs + item_size - 2, NULL );
				if(sub_items2)
				{
					for(j = 0; j<sub_items2;j++)
					{
						printf("\t\tL2_sub_item %.3d (off:0x%.4X) : ", j, ofs + item_size);
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

			int centerx = (get_ushort(&maz_file, ofs + 4, NULL ) * step_size) + calc_tile_offset( get_short(&maz_file, ofs + 8, NULL ), step_size);
			int centery = (get_ushort(&maz_file, ofs + 2, NULL ) * step_size) + calc_tile_offset(-get_short(&maz_file, ofs + 6, NULL ), step_size);

			if( get_fixed_obj_name( get_ushort(&maz_file, ofs, NULL ) ) )
			{
				sprintf(tmp_str,"%s", get_fixed_obj_name( get_ushort(&maz_file, ofs, NULL ) ) );

				// Try to load the sprite
				getspritefolder( in_file, tmp_str2, get_fixed_obj_name( get_ushort(&maz_file, ofs, NULL )), ".MSP");

				img = load_gfx( tmp_str2, get_ushort(&maz_file, ofs + 16, NULL ), NULL );
				if( img )
				{
					getpalfolder(in_file, (char*)&tmp_str2, "DUNGEO00" );
					loadgfxpal( img, tmp_str2);

					getpalfolder(in_file, (char*)&tmp_str2, get_fixed_obj_name( get_ushort(&maz_file, ofs, NULL )) );
					loadgfxpal( img, tmp_str2);

					disp_sprite( bmp_buf, (xsize*step_size), (ysize*step_size), img, centerx, centery);

					img = unload_gfx( img );
					tmp_str[0] = '\0';
				}
			}
			else
			{
				sprintf(tmp_str,"obj %.2X", get_ushort(&maz_file, ofs, NULL ));
			}

			if(strlen(tmp_str))
			{
				bmp_printf(bmp_buf,  (xsize*step_size), (ysize*step_size),
							centerx,
							centery,
							0x0002, tmp_str );
			}

			ofs += item_size;
		}

		//////////////////////////////////////////////////////////////////////
		nb_obj = get_ushort(&maz_file, ofs, NULL);

		printf("\n-----------------------------------------------------------\n");
		printf("Monsters_list : %d items (offset 0x%X) ...\n\n",nb_obj,ofs);

		ofs += 2;

		for(i=0;i<nb_obj;i++)
		{
			item_size = 0x12;

			printf("Monster %.3d (%.2d,%.2d) (off:0x%.4X) : ",i,get_ushort(&maz_file, ofs + 4, NULL ), (get_ushort(&maz_file, ofs + 2, NULL ) ), ofs);
			for(j=0;j<item_size;j++)
			{
				printf("%.2X ",get_byte(&maz_file, ofs + j, NULL ));
			}

			if(get_monster_name(get_ushort(&maz_file, ofs , NULL )))
				printf(" (%s)",get_monster_name(get_ushort(&maz_file, ofs , NULL )));

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
					printf("\tMonster_item %.3d (off:0x%.4X) : ",j, ofs + item_size);
					for(k=0;k< 14; k++)
					{
						printf("%.2X ", get_byte(&maz_file, ofs + item_size + k, NULL ) );
					}

					if(get_obj_name(get_ushort(&maz_file, ofs + item_size  , NULL )))
						printf(" (%s - %s)",get_obj_name(get_ushort(&maz_file, ofs + item_size  , NULL )), get_obj_desc(get_ushort(&maz_file, ofs + item_size, NULL )));

					printf("\n");
					item_size += 14;
				}

				printf("\n");
			}

			int centerx = (get_ushort(&maz_file, ofs + 4, NULL ) * step_size) + calc_tile_offset( get_short(&maz_file, ofs + 8, NULL ), step_size);
			int centery = (get_ushort(&maz_file, ofs + 2, NULL ) * step_size) + calc_tile_offset(-get_short(&maz_file, ofs + 6, NULL ), step_size);

			if( get_monster_name( get_ushort(&maz_file, ofs, NULL ) ) )
			{
				sprintf(tmp_str,"%s", get_monster_name( get_ushort(&maz_file, ofs, NULL ) ) );

				// Try to load the sprite
				getspritefolder( in_file, tmp_str2, get_monster_name( get_ushort(&maz_file, ofs, NULL )), ".MSP");

				img = load_gfx( tmp_str2, get_ushort(&maz_file, ofs + 16, NULL ), NULL );
				if( img )
				{
					getpalfolder(in_file, (char*)&tmp_str2, "DUNGEO00" );
					loadgfxpal( img, tmp_str2);

					getpalfolder(in_file, (char*)&tmp_str2, get_monster_name( get_ushort(&maz_file, ofs, NULL )) );
					loadgfxpal( img, tmp_str2);

					disp_sprite( bmp_buf, (xsize*step_size), (ysize*step_size), img, centerx, centery);

					img = unload_gfx( img );
				}

			}
			else
			{
				sprintf(tmp_str,"M%.2X", get_ushort(&maz_file, ofs, NULL ));
			}

			bmp_printf(bmp_buf,  (xsize*step_size), (ysize*step_size),
							(get_ushort(&maz_file, ofs + 4, NULL ) * step_size) + calc_tile_offset( get_short(&maz_file, ofs + 8, NULL ), step_size),
							(get_ushort(&maz_file, ofs + 2, NULL ) * step_size) + calc_tile_offset(-get_short(&maz_file, ofs + 6, NULL ), step_size),
							0x0002, tmp_str );

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

		rawto24bbmp(bmp_buf,(xsize*step_size),(ysize*step_size),nomfichier );

		i = 0;
		while( sk_objects[i].id != 0xFFFF )
		{
			if( sk_objects[i].name )
				free(sk_objects[i].name);

			if( sk_objects[i].desc )
				free(sk_objects[i].desc);

			i++;
		}

		free(bmp_buf);
	}
	else
	{
		printf("Fatal Error : cannot allocate %d bytes\n",xsize*ysize*step_size*step_size);
	}

	close_file( &maz_file );

	return 0;
}

int export_enc2txt( char * in_file, char * out_file )
{
	file_cache enc_file;
	int i;
	uint8_t d;
	FILE *f;

	if( open_file( &enc_file, in_file, -1, 0 ) < 0 )
	{
		printf("File access error : %s\n",in_file);
		return -1;
	}

	f = fopen(out_file,"wb");
	if(f)
	{
		d = 0xFF;
		for(i=0;i<enc_file.file_size;i++)
		{
			fprintf( f, "%c", get_byte(&enc_file, i, NULL ) ^ d );
			d--;
		}
		fclose(f);
	}

	close_file( &enc_file );

	return 0;
}

#define BMP_MODE 0
#define ENC_MODE 1

int main (int argc, char ** argv)
{
	int i,mode;
	char out_file[512];

	printf("SK MAZ converter V1.0 by HxC2001 (2024 http://hxc2001.free.fr)\n\n");

	if ( argc < 2 )
	{
		printf("Syntax : %s  [-bmp/-enc] [maz_file.maz/encoded.enc files]\n",argv[0]);
		exit(1);
	}

	mode = BMP_MODE;
	if( isOption(argc, argv,"bmp",NULL, NULL) )
	{
		mode = BMP_MODE;
	}

	if( isOption(argc, argv,"enc",NULL, NULL) )
	{
		sprintf(out_file,"out.txt");
		mode = ENC_MODE;
	}

	if( isOption(argc, argv,"out",(char*)&out_file, NULL) )
	{
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
				case ENC_MODE:
					export_enc2txt( argv[i], out_file );
				break;
			}
		}
		i++;
	}

	exit(0);
}
