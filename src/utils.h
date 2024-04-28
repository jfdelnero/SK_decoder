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

#ifdef WIN32
#define DIR_SEPARATOR "\\"
#define DIR_SEPARATOR_CHAR '\\'
#else
#define DIR_SEPARATOR "/"
#define DIR_SEPARATOR_CHAR '/'
#endif

#define FILEFOUND_NAMESIZE 256

typedef struct filefoundinfo_
{
	int isdirectory;
	char filename[FILEFOUND_NAMESIZE];
	int size;
}filefoundinfo;

char * hxc_strupper(char * str);
char * hxc_strlower(char * str);

char * hxc_dyn_strcat(char * deststr,char * srcstr);
char * hxc_dyn_sprintfcat(char * deststr,char * srcstr, ...);
char * hxc_getfilenamebase(char * fullpath,char * filenamebase, int type);
char * hxc_getfilenameext(char * fullpath,char * filenameext, int type );
int hxc_getfilenamewext(char * fullpath,char * filenamewext, int type);
int hxc_getpathfolder(char * fullpath,char * folder,int type);
int hxc_checkfileext(char * path,char *ext,int type);
int hxc_getfilesize(char * path);

int hxc_open (const char *filename, int flags, ...);
FILE *hxc_fopen (const char *filename, const char *mode);
int hxc_fread(void * ptr, size_t size, FILE *f);
char * hxc_fgets(char * str, int num, FILE *f);
int	hxc_fclose(FILE * f);
//int hxc_stat( const char *filename, struct stat *buf);

void * hxc_find_first_file(char *folder, char *file, filefoundinfo* fileinfo);
int hxc_find_next_file(void* handleff, char *folder, char *file, filefoundinfo* fileinfo);
int hxc_find_close(void* handle);

char * hxc_getcurrentdirectory(char *currentdirectory,int buffersize);
int hxc_mkdir(char * folder);

char * hxc_strupper(char * str);
char * hxc_strlower(char * str);

int hxc_fgetsize( FILE * f );
enum
{
	SYS_PATH_TYPE = 0,
	UNIX_PATH_TYPE,
	WINDOWS_PATH_TYPE,
};


