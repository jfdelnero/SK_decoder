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
// SK Object / element ID definitions
// ... Work in progress ...
///////////////////////////////////////////////////////////////////////////////////

#include "sk_id.h"

const sk_obj_id_def sk_objects[]=
{
	{ 0x0060, "Stone"},
	{ 0x0061, "Skull"},
	{ 0x0014, "Clothes 2"},
	{ 0x000A, "Clothes"},
	{ 0x0056, "Broken sword"},
	{ 0x00D5, "Food"},
	{ 0x00D8, "Food 3"},
	{ 0x0008, "Food 2"},
	{ 0x003B, "Sharga sword"},
	{ 0x0031, "Axe"},
	{ 0x0052, "Sword"},
	{ 0x007E, "Coin"},
	{ 0x009A, "Gold key"},
	{ 0x00A6, "Silver key"},
	{ 0x009B, "Copper key"},
	{ 0x003E, "Brown Knife"},
	{ 0x00B7, "Afri's Orb"},
	//{ 0x00B6, "Orb"},
	{ 0x00C0, "Blue potion"},
	{ 0x00C6, "Green potion"},
	{ 0x00C7, "B.Blue potion"},
	{ 0x00C2, "potion 2"},
	{ 0x005D, "Arrow"},
	{ 0x0050, "baton"},
	{ 0x0045, "Hammer"},
	{ 0x0002, "Gold helmet"},
	{ 0x002C, "Gold shield"},
	{ 0x002B, "Wood shield"},
	{ 0x0035, "Molotov cocktail"},
	{ 0x00DC, "Parchment"},

	{0xFFFF, 0}
};

const sk_obj_id_def sk_fixed_objects[]=
{
	{ 0x0015, "Chest"},
	{ 0x0022, "Bones"},
	{ 0x0003, "Garbage"},
	{ 0x0005, "straw"},
	{ 0x0006, "broken glass"},
	{ 0x0002, "barrel"},
	{ 0x0009, "Broken chair"},
	{ 0x000A, "Broken table"},
	{ 0x000C, "Stair down"},
	{ 0x000D, "Stair up"},
	{ 0x0069, "Book table"},
	{ 0x0004, "wheat bag"},
	{ 0x0007, "Fire"},
	{ 0x0008, "Fire 2"},
	{0xFFFF, 0}
};

const sk_obj_id_def sk_monters[]=
{
	{ 0x0002, "Ant"},
	{ 0x0008, "Sharga"},
	{ 0x000B, "Snake"},
	{ 0x001D, "Dwarf"},
	{ 0x002D, "Thera"},
	{ 0x001E, "Khull Khumm"},

	{0xFFFF, 0}
};

const sk_obj_id_def sk_decorations[]=
{
	{ 0x000F, "Window"},
	{ 0x002B, "Text"},
	{ 0x002D, "Door"},
	{ 0x0003, "Wall chest"},
	{ 0x0004, "Lock"},
	{ 0x0001, "Switch"},
	{ 0x0000, "P.hole"},
	{0xFFFF, 0}
};

const char * get_obj_name(unsigned short id)
{
	int i;

	i = 0;
	while( ( sk_objects[i].id != 0xFFFF ) && ( sk_objects[i].id != id ) )
	{
		i++;
	}

	return sk_objects[i].name;
}

const char * get_fixed_obj_name(unsigned short id)
{
	int i;

	i = 0;
	while( ( sk_fixed_objects[i].id != 0xFFFF ) && ( sk_fixed_objects[i].id != id ) )
	{
		i++;
	}

	return sk_fixed_objects[i].name;
}

const char * get_monster_name(unsigned short id)
{
	int i;

	i = 0;
	while( ( sk_monters[i].id != 0xFFFF ) && ( sk_monters[i].id != id ) )
	{
		i++;
	}

	return sk_monters[i].name;
}

const char * get_decoration_name(unsigned short id)
{
	int i;

	i = 0;
	while( ( sk_decorations[i].id != 0xFFFF ) && ( sk_decorations[i].id != id ) )
	{
		i++;
	}

	return sk_decorations[i].name;
}
