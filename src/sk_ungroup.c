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

#include "utils.h"

#include "linux/limits.h"

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

int extract_file( file_cache * grp_file, char * dstfolder, files_table * entry )
{
	FILE * f_out;
	int i;
	unsigned char page[8192];
	char path[1024];

	if(!strcmp(entry->file_name,"!SENTINEL!"))
	{
		return -1;
	}

	printf("File export : %-14s\tOffset : 0x%.8X\tSize : %u byte(s)\n",
								entry->file_name,
								entry->offset,
								entry->size);

	if(dstfolder)
	{
		sprintf(path,"%s/%s",dstfolder,entry->file_name);
	}
	else
	{
		strcpy(path,entry->file_name);
	}

	f_out = fopen( path,"w+b");
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

int ungroup( char * group_file, int dry, int subfolder )
{
	files_table * bufferf;
	int nbfichier;
	uint32_t taillet=0;
	int i;
	int file_table_offset;
	file_cache grp_file;
	char str_tmp[PATH_MAX];

	if( open_file( &grp_file, group_file, -1, 0 ) < 0 )
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
		str_tmp[0] = 0;
		if( subfolder )
		{
			hxc_getfilenamebase( group_file, (char*)&str_tmp, SYS_PATH_TYPE);
			strcat(str_tmp,"_ungrouped");
			hxc_mkdir(str_tmp);

			for( i=0; i < nbfichier; i++ )
			{
				extract_file( &grp_file, (char*)&str_tmp, &bufferf[i] );
			}
		}
		else
		{
			for( i=0; i < nbfichier; i++ )
			{
				extract_file( &grp_file, NULL, &bufferf[i] );
			}
		}
	}

	for(i=0;i<nbfichier;i++)
		taillet += bufferf[i].size;

	printf("\nTotal : %u file(s) \t%u byte(s) \n",nbfichier,taillet);

	close_file( &grp_file );

	free(bufferf);

	return 0;
}

int group( char * group_file, int dry, char * outputfile )
{
	filefoundinfo finfo;
	void * handle;
	int ret;
	uint16_t count,cnt2;
	uint32_t checksum;
	int i, totalsize, ofs, file_data_offset;
	char data_file_path[PATH_MAX];
	char tmp_str[PATH_MAX];
	file_cache data_file;
	file_cache grp_file;
	unsigned char c;
	const char * sentinel="!SENTINEL!";

	totalsize = 0;
	count = 0;
	handle = hxc_find_first_file( group_file, "*.*", &finfo);
	if(handle)
	{
		do
		{
			if( !finfo.isdirectory )
			{
				printf("%s\n",finfo.filename);
				totalsize += finfo.size;
				count++;
			}
			ret = hxc_find_next_file( handle, group_file, "*.*", &finfo);
		}while (ret);

		hxc_find_close( handle );
	}

	cnt2 = 0;
	if( count )
	{
		printf("\n%d file(s)\n",count);

		if( open_file( &grp_file, outputfile, 2 + (22 * (count + 1)) + totalsize + 4, 0 ) < 0 )
		{
			printf("Can't create : %s\n", outputfile);
			return -1;
		}

		ofs = 0;
		set_ushort( &grp_file, ofs, count);
		ofs += 2;
		file_data_offset = ofs + (22 * (count + 1));

		checksum = 0;
		handle = hxc_find_first_file( group_file, "*.*", &finfo);
		if(handle)
		{
			do
			{
				if( !finfo.isdirectory )
				{
					sprintf(data_file_path,"%s/%s",group_file,finfo.filename);
					printf("Input file %s ...\n",data_file_path);

					if( open_file( &data_file, data_file_path, -1, 0 ) < 0 )
					{
						printf("File access error : %s\n", group_file);
						hxc_find_close( handle );
						return -1;
					}

					hxc_getfilenamebase((char*)data_file_path,(char*)tmp_str, SYS_PATH_TYPE);

					for(i=0;i<8+1+3;i++)
					{
						c = tmp_str[i];
						if(!c)
							break;
						set_byte( &grp_file, 2 + (22 * cnt2) + i, c);
					}

					set_ulong( &grp_file, 2 + (22 * cnt2) + 14, file_data_offset);

					for(i=0;i<data_file.file_size;i++)
					{
						c = get_byte( &data_file, i, NULL );
						set_byte( &grp_file, file_data_offset + i, c);
						checksum += c;
					}

					file_data_offset += data_file.file_size;

					close_file( &data_file );

					cnt2++;
				}
				ret = hxc_find_next_file( handle, group_file, "*.*", &finfo);
			}while (ret && cnt2 < count);

			for(i=0;i<8+1+3;i++)
			{
				c = sentinel[i];
				if(!c)
					break;
				set_byte( &grp_file, 2 + (22 * cnt2) + i, c);
			}

			set_ulong( &grp_file, 2 + (22 * cnt2) + 14, file_data_offset);

			set_ulong( &grp_file, grp_file.file_size - 4, checksum);

			hxc_find_close( handle );
			
			printf("Group file checksum : 0x%.8X\n",checksum);
			
		}

		close_file(&grp_file);
	}
	return 0;
}

int main (int argc, char ** argv)
{
	int i, j, dry, createsubfolder;
	char str_tmp[PATH_MAX];
	char output_file[PATH_MAX];

	printf("SK UNGROUP V1.01 by HxC2001 (2002-2024 http://hxc2001.free.fr)\n\n");

	if ( argc < 2 )
	{
		printf("Syntax :\n");
		printf("To unpack : %s group_files [-L] [-folder]\n",argv[0]);
		printf("To pack : %s -pack in_folders_paths\n",argv[0]);
		exit(1);
	}

	dry = 0;
	if( isOption(argc, argv,"L",NULL, NULL) )
	{
		dry = 1;
	}

	createsubfolder = 0;
	if( isOption(argc, argv,"folder",NULL, NULL) )
	{
		createsubfolder = 1;
	}

	output_file[0]=0;
	if( isOption(argc, argv,"out",(char*)&output_file, NULL) )
	{
	}

	if( isOption(argc, argv,"pack",(char*)&str_tmp, NULL) )
	{
		if( !strlen(output_file) )
		{
			i = 1;
			while( i < argc )
			{
				if(argv[i][0] != '-')
				{
					for(j=0;j<100;j++)
					{
						sprintf(output_file,"GROUP%.2d_ungrouped",j);
						if( strstr(argv[i],output_file) )
						{
							sprintf(output_file,"GROUP%.2d",j);
							break;
						}
					}
					group( (char*)argv[i], dry, (char*)output_file );
				}
				i++;
			}
			
			exit(0);
		}
		else
		{
			group( (char*)str_tmp, dry, (char*)output_file );
			exit(0);			
		}
	}

	i = 1;
	while( i < argc )
	{
		if(argv[i][0] != '-')
		{
			ungroup( argv[i], dry, createsubfolder );
		}
		i++;
	}

	exit(0);
}
