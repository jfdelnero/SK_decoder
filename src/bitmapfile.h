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

#pragma pack(2)

#define WORD uint16_t
#define DWORD uint32_t
#define LONG uint32_t

typedef struct tagBITMAPFILEHEADER
{
    WORD bfType;    // Spécifie le type de fichier. Doit être BM (0x4D42)
    DWORD bfSize;    // Spécifie la taille en byte du fichier bitmap
    WORD bfReserved1;    // Réservé. Doit être égal à 0
    WORD bfReserved2;    // Réservé. Doit être égal à 0
    DWORD bOffBits;    // Spécifie le décalage en byte depuis  l'en-tête de fichier du bitmap jusqu'au données du bitmap
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{
    DWORD biSize;    // Spécifie le nombre de byte requis pour la structure
    LONG biWidth;    // Spécifie la largeur en pixel de l'image
    LONG biHeight;    // Spécifie la hauteur en pixel de l'image
    WORD biPlanes;    // Spécifie le nombre de plan de couleur. Doit être égal à 0
    WORD biBitCount;    // Spécifie le nombre de bits en pixel : doit être égal à 1, 4, 8, 16, 24 ou 32
    DWORD biCompression;    // Spécifie le type de compression
    DWORD biSizeImage;    // Taille de l'image en byte
    LONG biXPelsPerMeter;    // Spécifie le nombre de pixels par mètre sur l'axe des abscisses
    LONG biYPelsPerMeter;    // Spécifie le nombre de pixels par mètre sur l'axe des ordonnées
    DWORD biClrUsed;    // Spécifie le nombre de couleurs utilisé par le bitmap
    DWORD biClrImportant;    // Spécifie le nombre de couleurs qui sont importantes
} BITMAPINFOHEADER; 

#pragma pack()
