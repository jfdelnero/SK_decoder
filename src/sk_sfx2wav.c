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
// SK sound FX (*.CPR) and voices to *.wav converter.
//
///////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <stdint.h>
#include "cache.h"
#include "wave.h"
#include "sfxunpacker.h"

#include "cmd_param.h"

int export_sfx(char * in_file)
{
	file_cache sfx_file;
	wave_io * wave;

	int ofs,ret;
	uint16_t header;
	uint32_t outsize;

	char file_name[256*2];
	char file_name2[256];

	int unpackedsize;
	unsigned char * bufferout;

	ret = -1;

	if( open_file( &sfx_file, in_file, 0 ) < 0 )
	{
		printf("File access error : %s\n",in_file);
		return -1;
	}

	if(strrchr( in_file,'\\'))
	{
		sprintf(file_name2,"%s",strrchr(in_file,'\\')+1);
	}
	else
	{
		sprintf(file_name2,"%s",in_file);
	}

	if(strrchr(file_name2,'.'))
	{
		*strrchr(file_name2,'.')='_';
	}

	ofs = 0;
	header = get_ushort(&sfx_file, ofs, NULL);

	ofs += 2;

	if( header != 0x0002 )
	{
		printf("Error : bad header code : 0x%.4X\n",header);

		close_file( &sfx_file );
		return -1;
	}

	outsize = get_ulong(&sfx_file, ofs, NULL);
	if( outsize > (16*1024*1024) || !outsize )
	{
		close_file( &sfx_file );
		return -1;
	}

	ofs += 4;

	bufferout = (unsigned char*) malloc( outsize );
	if(!bufferout)
	{
		printf("Fatal Error : cannot allocate %d bytes\n",outsize);

		close_file( &sfx_file );
		return -1;
	}

	memset(bufferout, 0x80, outsize);

	sprintf(file_name,"%s.wav",file_name2);

	printf("Unpack %s, %d -> %d bytes...\n",in_file, sfx_file.file_size - ofs , outsize);

	unpackedsize = unpacksfx(&sfx_file, ofs, bufferout, outsize);

	if( unpackedsize != outsize)
	{
		printf("Warning : Unpack error ? unpackedsize (%d) != outsize (%d)\n",unpackedsize, outsize);
	}

	printf("Write %s, %d samples...\n",file_name, outsize);

	wave = create_wave(file_name, 22050, WAVE_FILE_FORMAT_WAV_8BITS_MONO);
	if(wave)
	{
		write_wave(wave, bufferout, outsize);
		close_wave(wave);
		ret = 1;
	}
	else
	{
		printf("Error : Can't 'Write %s !\n",file_name);
	}

	free(bufferout);

	close_file( &sfx_file );

	return ret;
}

int export_voices(char * in_file)
{
	file_cache voices_file;
	wave_io * wave;
	unsigned char * sub_text;
	unsigned char * audio_buffer;

	int ofs;
	uint32_t nb_of_voices,names_list_offset,addresses_list_offset;
	uint32_t block_address, audio_address, subtitles_size, audio_size;
	uint32_t outsize;

	char file_name[256*2];
	char file_name2[256];
	char voice_name[8 + 1];

	int i, j;

	FILE * subtitles_file;

	printf("Extract voices file : %s ...\n",in_file);
	if( open_file( &voices_file, in_file, 0 ) < 0 )
	{
		printf("File access error : %s\n",in_file);
		return -1;
	}

	if(strrchr( in_file,'\\'))
	{
		sprintf(file_name2,"%s",strrchr(in_file,'\\')+1);
	}
	else
	{
		sprintf(file_name2,"%s",in_file);
	}

	if(strrchr(file_name2,'.'))
	{
		*strrchr(file_name2,'.')='_';
	}

	ofs = 0;
	nb_of_voices = get_ulong(&voices_file, ofs, NULL);
	ofs += 4;
	names_list_offset = get_ulong(&voices_file, ofs, NULL);
	ofs += 4;
	addresses_list_offset = get_ulong(&voices_file, ofs, NULL);
	ofs += 4;

	for(i = 0; i < nb_of_voices; i++ )
	{
		for(j=0;j<8;j++)
		{
			voice_name[j] = get_byte(&voices_file, names_list_offset + (i*8) + j, NULL);
		}
		voice_name[8] = 0;

		sprintf(file_name,"%s.wav",voice_name);

		block_address = get_ulong(&voices_file, addresses_list_offset + (i * 4), NULL);
		audio_address = get_ulong(&voices_file, block_address, NULL);
		audio_size = get_ulong(&voices_file, block_address + 0x4, NULL);
		subtitles_size = get_ulong(&voices_file, block_address + 0x10, NULL);

		audio_buffer = NULL;
		if( audio_size && audio_size < 32*1024*1024 )
		{
			audio_buffer = (unsigned char*) malloc( audio_size );

			if( audio_buffer )
			{
				memset( audio_buffer, 0, audio_size );
				for(j=0;j<audio_size;j++)
				{
					audio_buffer[j] = get_byte(&voices_file, audio_address + j, NULL);
				}
			}
		}

		printf("Voice File: %s offset: 0x%.8x size: %d bytes\n",voice_name,block_address,audio_size);

		printf("Write %s, %d samples...\n",file_name, outsize);

		wave = create_wave(file_name, 22050, WAVE_FILE_FORMAT_WAV_8BITS_STEREO);
		if(wave)
		{
			write_wave(wave, audio_buffer, audio_size / 2);
			close_wave(wave);
		}
		else
		{
			printf("Error : Can't 'Write %s !\n",file_name);
		}

		if( subtitles_size && subtitles_size < (16*1024*1024) )
		{
			sprintf(file_name,"%s.txt",voice_name);

			printf("Create subtitles file : %s\n",file_name);
			subtitles_file = fopen( file_name,"w" );
			if(subtitles_file)
			{
				sub_text = malloc( subtitles_size + 1 );
				if( sub_text )
				{
					memset(sub_text,0, subtitles_size + 1);
					for(j=0;j<subtitles_size;j++)
					{
						sub_text[j] = get_byte(&voices_file, block_address + 0x18 + j, NULL);
						if( sub_text[j]==0 )
							sub_text[j] = ' ';
					}
					sub_text[subtitles_size] = 0;

					fprintf( subtitles_file, "%s\n\n", sub_text );


					free( sub_text );
				}
				fclose( subtitles_file );
			}
		}

		free(audio_buffer);
	}

	close_file( &voices_file );

	return 0;
}

#define SFX_MODE 0
#define VOICES_MODE 1

int main (int argc, char ** argv)
{
	int i,mode;

	printf("SK SFX2WAV converter V1.1 by HxC2001 (2002-2024 http://hxc2001.free.fr)\n\n");

	if ( argc < 2 )
	{
		printf("Syntax : %s [-voices/-sfx] sfx_file.cpr/voices files\n",argv[0]);
		exit(1);
	}

	mode = SFX_MODE;
	if( isOption(argc, argv,"voices",NULL, NULL) )
	{
		mode = VOICES_MODE;
	}

	if( isOption(argc, argv,"sfx",NULL, NULL) )
	{
		mode = SFX_MODE;
	}

	i = 1;
	while( i < argc )
	{
		if(argv[i][0] != '-')
		{
			switch(mode)
			{
				case SFX_MODE:
					export_sfx( argv[i] );
				break;
				case VOICES_MODE:
					export_voices( argv[i] );
				break;
			}
		}
		i++;
	}

	exit(0);
}
