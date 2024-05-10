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
// SK GFX converter *.MON,*.POJ,*.WPN,*.MSP,*.G* -> *.BMP
//
///////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmp_file.h"
#include "cache.h"

#include "cmd_param.h"

#include "sk_gfx.h"

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

int export_gfx( char * in_file, char * pal1_path, char * pal2_path )
{
	char nomfichier[256*2];
	char nomfichier2[256];

	int j;
	int nbrimage;
	gfx * img;

	nbrimage = 0;

	load_gfx( in_file, -1, &nbrimage );

	printf("found %d images in this file:\n",nbrimage);
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

	j=0;
	while( j < nbrimage )
	{
		img = load_gfx( in_file, j, NULL );
		if(img)
		{
			if( pal1_path )
				loadgfxpal( img, pal1_path );

			if( pal2_path )
				loadgfxpal( img, pal2_path );

			sprintf(nomfichier,"%s_%.3d.bmp",nomfichier2,j);

			gfx2bmp( img, nomfichier, pal1_path, pal2_path );

			img = unload_gfx( img );
		}

		j++;
	}

	return 0;
}

int export_mmp( char * in_file, char * pal1_path, char * pal2_path )
{
	file_cache mmp_file;
	int ofs;
	int end_index_offset;

	char nomfichier[256*2];
	char nomfichier2[256];

	int i,j;
	unsigned char * bufferout;

	unsigned int xsize,ysize;

	if( open_file( &mmp_file, in_file, -1, 0 ) < 0 )
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

	end_index_offset = get_ulong(&mmp_file, 0, NULL);

	i=0;
	while( i*4 < end_index_offset )
	{
		ofs = get_ulong(&mmp_file, i * 4, NULL);

		xsize = get_ushort(&mmp_file, ofs + 0, NULL);
		ysize = get_ushort(&mmp_file, ofs + 2, NULL);

		if ( (xsize*ysize)+ofs <= mmp_file.file_size)
		{
			printf("%d : offset: 0x%.8x res: %d by %d\n",i,ofs,xsize,ysize);
			sprintf(nomfichier,"%s_%.3d.bmp",nomfichier2,i);

			bufferout = (unsigned char*)malloc(xsize*ysize);
			if( bufferout )
			{
				memset(bufferout,0x00,xsize*ysize);

				for(j=0;j<xsize*ysize;j++)
				{
					bufferout[j] = get_byte(&mmp_file, ofs + 4 + j, NULL);
				}

				rawtobmp(bufferout,xsize,ysize,nomfichier,pal1_path,pal2_path);

				free(bufferout);
			}
			else
			{
				printf("Fatal Error : cannot allocate %d bytes\n",xsize*ysize);
			}
		}

		i++;
	}

	close_file( &mmp_file );

	return 0;
}

int export_ubg( char * in_file, char * pal1_path, char * pal2_path )
{
	file_cache ubg_file;
	int ofs;
	int nb_images;
	uint32_t partsize;

	char nomfichier[256*2];
	char nomfichier2[256];

	int i,j;
	unsigned char * bufferout;

	unsigned int xsize,ysize;

	if( open_file( &ubg_file, in_file, -1, 0 ) < 0 )
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

	nb_images = get_ushort(&ubg_file, 0, NULL);

	i = 0;
	ofs = 2;
	while( i < nb_images )
	{
		partsize = get_ushort( &ubg_file, ofs, NULL);

		xsize = get_ushort(&ubg_file, ofs + 2, NULL);
		ysize = get_ushort(&ubg_file, ofs + 4, NULL);

		printf("%d : offset: 0x%.8x res: %d by %d\n",i,ofs,xsize,ysize);
		sprintf(nomfichier,"%s_%.3d.bmp",nomfichier2,i);

		bufferout = (unsigned char*)malloc(xsize*ysize);
		if( bufferout )
		{
			memset(bufferout,0x00,xsize*ysize);

			for(j=0;j<xsize*ysize;j++)
			{
				bufferout[j] = get_byte(&ubg_file, ofs + 8 + j, NULL);
			}

			rawtobmp(bufferout,xsize,ysize,nomfichier,pal1_path,pal2_path);

			free(bufferout);
		}
		else
		{
			printf("Fatal Error : cannot allocate %d bytes\n",xsize*ysize);
		}

		ofs += partsize + 8;
		i++;
	}

	close_file( &ubg_file );

	return 0;
}

#define MON_MODE 0
#define MMP_MODE 1
#define UBG_MODE 2

int main (int argc, char ** argv)
{
	int i,mode;
	char pal1_path[512];
	char pal2_path[512];

	printf("SK GFX2BMP converter V1.0 by HxC2001 (2002-2024 http://hxc2001.free.fr)\n\n");

	if ( argc < 2 )
	{
		printf("Syntax : %s  [-mon/-mmp/-ubg] [-pal1:palettefile1.pal] [-pal2:palettefile2.pal] [gfx_file.MON/.POJ/.WPN/.MSP/.G*/.MMP/.UBG files]\n",argv[0]);
		exit(1);
	}

	pal1_path[0] = 0;
	if( isOption(argc, argv,"pal1",(char*)&pal1_path, NULL) )
	{
	}

	pal2_path[0] = 0;
	if( isOption(argc, argv,"pal2",(char*)&pal2_path, NULL) )
	{
	}

	mode = MON_MODE;
	if( isOption(argc, argv,"mmp",NULL, NULL) )
	{
		mode = MMP_MODE;
	}

	if( isOption(argc, argv,"ubg",NULL, NULL) )
	{
		mode = UBG_MODE;
	}

	if( isOption(argc, argv,"mon",NULL, NULL) )
	{
		mode = MON_MODE;
	}


	i = 1;
	while( i < argc )
	{
		if(argv[i][0] != '-')
		{
			switch(mode)
			{
				case MON_MODE:
					export_gfx( argv[i], pal1_path, pal2_path );
				break;
				case MMP_MODE:
					export_mmp( argv[i], pal1_path, pal2_path );
				break;
				case UBG_MODE:
					export_ubg( argv[i], pal1_path, pal2_path );
				break;
			}
		}
		i++;
	}

	exit(0);
}
