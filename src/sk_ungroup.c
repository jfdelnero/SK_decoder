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

/*///////////////////////////////////////////////////////////////////////////////////////
//  SK UNGROUP V1.01
//
//  hxc2001.free.fr
//  Permet d'extraire les fichiers contenus dans un fichier GROUP
//  (fichiers presents dans le jeu SK entre autres...)
//
//  Modifications:
//  25/3/2002    0.9          Release initiale
//  12/1/2003    (0.9->0.91)  Mise en binaire de l'ecriture (oups! "w" -> "w+b")
//  20/4/2024    (0.91->1.01) Major code refactoring.
///////////////////////////////////////////////////////////////////////////////////////*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cache.h"
#include "cmd_param.h"

typedef struct files_table_
{
	char file_name[64];
	uint32_t offset;
	uint32_t size;
	uint32_t flags;
}files_table;

int get_file_entry( file_cache * grp_file, files_table * entry, int offset )
{
	int i;

	i = 0;
	while( get_byte( grp_file, offset + i, NULL)  && i < 32)
	{
		entry->file_name[i] = get_byte( grp_file, offset + i, NULL);
		i++;
	}

	entry->file_name[i] = 0;

	entry->offset = get_ulong( grp_file, offset + 14, NULL);
	entry->flags = get_ulong( grp_file, offset + 18, NULL);

	entry->size = 0;

	return offset + 22;
}

int update_entries_size(files_table * entry, int nb)
{
	int i;

	for(i=0;i<nb;i++)
	{
		if( strcmp(entry[i].file_name,"!SENTINEL!") )
		{
			entry[i].size = (entry[i+1].offset)-(entry[i].offset);
		}
		else
			nb--;
	}

	return nb;
}

int extract_file( file_cache * grp_file, files_table * entry )
{
	FILE * f_out;
	int i;
	unsigned char page[8192];

	if(!strcmp(entry->file_name,"!SENTINEL!"))
	{
		return -1;
	}

	printf("File export : %-14s\tOffset : 0x%.8X\tSize : %u byte(s)\n",
								entry->file_name,
								entry->offset,
								entry->size);

	f_out = fopen( entry->file_name,"w+b");
	if( f_out != NULL )
	{
		for( i = 0; i < entry->size; i++ )
		{
			page[ i & (sizeof(page) - 1) ] = get_byte( grp_file, entry->offset + i, NULL );
			if( ( i & (sizeof(page) - 1) ) == ( sizeof(page) - 1 ) )
			{
				fwrite(page,sizeof(page),1,f_out);
			}
		}

		if( i & ( sizeof(page) - 1 ) )
		{
			fwrite(page,i & ( sizeof(page) - 1 ),1,f_out);
		}

		fclose(f_out);
	}
	else
	{
		printf("Error : Can't create file ! (%s)\n", entry->file_name);
		return -1;
	}

	return 0;
}

int ungroup( char * group_file, int dry )
{
	files_table * bufferf;
	int nbfichier;
	uint32_t taillet=0;
	int i;
	int file_table_offset;
	file_cache grp_file;

	if( open_file( &grp_file, group_file, 0 ) < 0 )
	{
		printf("File access error : %s\n", group_file);
		return -1;
	}

	printf("Group file %s ...\n",group_file);

	// GROUP Format
	//
	// Head : Number of files : 16 bits
	//
	// File entry (22 bytes):
	// File name (Max : 8 + . + 3 + Null char) : 14 bytes
	// Offset : 4 Bytes
	// ?????? : 4 Bytes

	nbfichier = get_ushort( &grp_file, 0, NULL );

	printf("%d Entries\n",nbfichier);

	bufferf = (files_table*)malloc( nbfichier * sizeof(files_table) );
	if( !bufferf )
	{
		printf("Allocation error !\n");
		close_file( &grp_file );
		exit(2);
	}

	file_table_offset = 2;
	for(i=0;i<nbfichier;i++)
	{
		file_table_offset = get_file_entry( &grp_file, &bufferf[i], file_table_offset );
	}

	nbfichier = update_entries_size( bufferf, nbfichier);

	if( dry )
	{
		for( i=0; i < nbfichier; i++ )
		{
			printf("File : %-14s\tOffset : 0x%.8X\tFlags : 0x%.8X\tSize : %u byte(s)\n",bufferf[i].file_name,bufferf[i].offset,bufferf[i].flags,bufferf[i].size);
		}
	}
	else
	{
		for( i=0; i < nbfichier; i++ )
		{
			extract_file( &grp_file, &bufferf[i] );
		}
	}

	for(i=0;i<nbfichier;i++)
		taillet += bufferf[i].size;

	printf("\nTotal : %u file(s) \t%u byte(s) \n",nbfichier,taillet);

	close_file( &grp_file );

	free(bufferf);

	return 0;
}

int main (int argc, char ** argv)
{
	int i, dry;

	printf("SK UNGROUP V1.01 by HxC2001 (2002-2024 http://hxc2001.free.fr)\n\n");

	if ( argc < 2 )
	{
		printf("Syntax : %s group_files [-L]\n",argv[0]);
		exit(1);
	}

	dry = 0;
	if( isOption(argc, argv,"L",NULL, NULL) )
	{
		dry = 1;
	}

	i = 1;
	while( i < argc )
	{
		if(argv[i][0] != '-')
		{
			ungroup( argv[i], dry );
		}
		i++;
	}

	exit(0);
}
