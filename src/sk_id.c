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
// SK Objects / elements ID definitions
// ... Work in progress ...
///////////////////////////////////////////////////////////////////////////////////

#include "sk_id.h"

#define NULL 0x00000000

// Items list dynamically generated from OBJTYPE.DEF and BOOKINT.ENC
sk_obj_id_def sk_objects[MAX_OBJ];

const sk_obj_id_def sk_fixed_objects[]=
{
	{0x0000, "FORGE",	NULL},
	{0x0001, "FORGEFIR",NULL},
	{0x0002, "BARREL",	NULL},
	{0x0003, "RUBBLE1",	NULL},
	{0x0004, "SACKOGRN","wheat bag"},
	{0x0005, "BED",		NULL},
	{0x0006, "PLATE",	"broken glass"},
	{0x0007, "FIREPIT",	NULL},
	{0x0008, "CAMPFIRE",NULL},
	{0x0009, "SMCHAIR",	"Broken chair"},
	{0x000A, "SMTABLE",	"Broken table"},
	{0x000B, "CEILHOLE",NULL},
	{0x000C, "D1STDN",	"Stair down"},
	{0x000D, "D1STUP",	"Stair up"},
	{0x000E, "H1PITFLO",NULL},
	{0x000F, "FOUNTAIN",NULL},
	{0x0010, "D1LDRDN",	NULL},
	{0x0011, "D1LDRUP",	NULL},
	{0x0012, "WDLADD",	NULL},
	{0x0013, "WDLADU",	NULL},
	{0x0014, "DIAS",	NULL},
	{0x0015, "CHEST1",	NULL},
	{0x0016, "D1CPUP",	NULL},
	{0x0017, "D1CPDOWN",NULL},
	{0x0018, "ORBOFSTR",NULL},
	{0x0019, "CHEST2",	NULL},
	{0x001A, "CHEST3",	NULL},
	{0x001B, "CHEST4",	NULL},
	{0x001C, "CHEST5",	NULL},
	{0x001D, "ORBSHDOW",NULL},
	{0x001E, "BOULDER",	NULL},
	{0x001F, "WASPNEST",NULL},
	{0x0020, "CAGEBK",	NULL},
	{0x0021, "CAGEFT",	NULL},
	{0x0022, "BONES",	NULL},
	{0x0023, "PORTD1A",	NULL},
	{0x0024, "PORTD1B",	NULL},
	{0x0025, "PORTD1C",	NULL},
	{0x0026, "PORTD1D",	NULL},
	{0x0027, "PORTD1E",	NULL},
	{0x0028, "PORTD2A",	NULL},
	{0x0029, "PORTD2B",	NULL},
	{0x002A, "PORTD2C",	NULL},
	{0x002B, "PORTD2D",	NULL},
	{0x002C, "PORTD2E",	NULL},
	{0x002D, "PORTD3A",	NULL},
	{0x002E, "PORTD3B",	NULL},
	{0x002F, "PORTD3C",	NULL},
	{0x0030, "PORTD3D",	NULL},
	{0x0031, "PORTD3E",	NULL},
	{0x0032, "PORTD4B",	NULL},
	{0x0033, "PORTD4C",	NULL},
	{0x0034, "PORTD4D",	NULL},
	{0x0035, "RUBBLE3",	NULL},
	{0x0036, "HEAD",	NULL},
	{0x0037, "BUCKET",	NULL},
	{0x0038, "WELL",	NULL},
	{0x0039, "DRAGSTAT",NULL},
	{0x003A, "BUDDA1",	NULL},
	{0x003B, "MINEPIT",	NULL},
	{0x003C, "MINECEIL",NULL},
	{0x003D, "DEDETTIN",NULL},
	{0x003E, "DEDICE",	NULL},
	{0x003F, "DEDELF",	NULL},
	{0x0040, "DEDSHAM",	NULL},
	{0x0041, "DEDSLIME",NULL},
	{0x0042, "DEDGUARD",NULL},
	{0x0043, "DEDWAHOO",NULL},
	{0x0044, "DEDLETH",	NULL},
	{0x0045, "DEDCHAIN",NULL},
	{0x0046, "DEDPLATE",NULL},
	{0x0047, "DEDGOB",	NULL},
	{0x0048, "DEDTENT",	NULL},
	{0x0049, "DEDTARM",	NULL},
	{0x004A, "DEDTROLL",NULL},
	{0x004B, "DEDWASP",	NULL},
	{0x004C, "DEDSPIKE",NULL},
	{0x004D, "DEDANT",	NULL},
	{0x004E, "DEDSERP",	NULL},
	{0x004F, "DEDZOMB",	NULL},
	{0x0050, "D3CPUP",	NULL},
	{0x0051, "D3CPDOWN",NULL},
	{0x0052, "COLDFIRE",NULL},
	{0x0053, "SNOWPILE",NULL},
	{0x0054, "GAS_A",	NULL},
	{0x0055, "SW_BASE",	NULL},
	{0x0056, "ORCGATE",	NULL},
	{0x0057, "POST",	NULL},
	{0x0058, "DUCK",	NULL},
	{0x0059, "D2STDN",	NULL},
	{0x005A, "D2STUP",	NULL},
	{0x005B, "D3STDN",	NULL},
	{0x005C, "D3STUP",	NULL},
	{0x005D, "GAS_B",	NULL},
	{0x005E, "DPASS_D1",NULL},
	{0x005F, "DPASS_D2",NULL},
	{0x0060, "DPASS_D3",NULL},
	{0x0061, "DPASS_D4",NULL},
	{0x0062, "SW_ANIM",	NULL},
	{0x0063, "COT",		NULL},
	{0x0064, "GSTONE",	NULL},
	{0x0065, "STONEA",	NULL},
	{0x0066, "STONEB",	NULL},
	{0x0067, "STONEC",	NULL},
	{0x0068, "GSPARK",	NULL},
	{0x0069, "DTBL",	"Book table"},
	{0x006A, "SCRLCASE",NULL},
	{0x006B, "CIRCLE",	NULL},
	{0x006C, "CIRCLEW",	NULL},
	{0x006D, "DEDFUNGI",NULL},
	{0x006E, "GLYPH",	NULL},
	{0x006F, "CIRCLEB",	NULL},
	{0x0070, "DEDTOADS",NULL},
	{0x0071, "FLINT",	NULL},
	{0x0072, "BLCKPASS",NULL},
	{0x0073, "CAVEIN",	NULL},
	{0x0074, "CAVEIN_P",NULL},
	{0x0075, "COLDPIT",	NULL},
	{0x0076, "QUEEN",	NULL},
	{0x0077, "QUEENB",	NULL},
	{0x0078, "DRAGON3",	NULL},
	{0x0079, "DRAGON4",	NULL},
	{0x007A, "DRAGON5",	NULL},
	{0x007B, "DRAGON6",	NULL},
	{0x007C, "SARCABK",	NULL},
	{0x007D, "SARCAFT",	NULL},
	{0x007E, "GARGOUT",	NULL},
	{0x007F, "BUDDA2",	NULL},
	{0x0080, "BUDDA3",	NULL},
	{0x0081, "BUDDA4",	NULL},
	{0x0082, "STOVE",	NULL},
	{0x0083, "DWRFGATE",NULL},
	{0x0084, "DRAGON7",	NULL},
	{0x0085, "DRAGON8",	NULL},
	{0x0086, "YES_OUT",	NULL},
	{0x0087, "YES_PUSH",NULL},
	{0x0088, "NO_OUT",	NULL},
	{0x0089, "NO_PUSH",	NULL},
	{0x008A, "SHRINE",	NULL},
	{0x008B, "SB",		NULL},
	{0x008C, "ITHRONE",	NULL},
	{0x008D, "SANKH",	NULL},
	{0x008E, "SCIRCLE",	NULL},
	{0x008F, "SCRESNT",	NULL},
	{0x0090, "SCROSS",	NULL},
	{0x0091, "PILLAR",	NULL},
	{0x0092, "ANCIDOOR",NULL},
	{0x0093, "PUDDLE",	NULL},
	{0x0094, "RUBBLE2",	NULL},
	{0x0095, "RUBBLE3",	NULL},
	{0x0096, "RUBBLE4",	NULL},
	{0x0097, "RUBBLE8",	NULL},
	{0x0098, "DRAIN",	NULL},
	{0x0099, "ICAVEIN",	NULL},
	{0x009A, "IPILE",	NULL},
	{0x009B, "ENDGAME",	NULL},
	{0x009C, "BIGMUSH",	NULL},
	{0x009D, "DRAGON9",	NULL},
	{0x009E, "DEDGRAK",	NULL},
	{0x009F, "PILLARBS",NULL},
	{0x00A0, "PANKH",	NULL},
	{0x00A1, "PCIRCLE",	NULL},
	{0x00A2, "PCRESENT",NULL},
	{0x00A3, "PCROSS",	NULL},

	{0xFFFF, 0, 0}
};

const sk_obj_id_def sk_monters[]=
{
	{0x0000, "SKELETON",	NULL},
	{0x0001, "SLIME",		NULL},
	{0x0002, "ANT",			NULL},
	{0x0003, "SKULL",		NULL},
	{0x0004, "SPIKER",		NULL},
	{0x0005, "ELEMENTL",	NULL},
	{0x0006, "ZOMBIE",		NULL},
	{0x0007, "LTHDWARF",	NULL},
	{0x0008, "SHARGA",		NULL},
	{0x0009, "ICEWITCH",	NULL},
	{0x000A, "GUARD",		NULL},
	{0x000B, "LANDSNAK",	NULL},
	{0x000C, "Drakey",		NULL},
	{0x000D, "FARLI",		NULL},
	{0x000E, "KARZAK",		NULL},
	{0x000F, "DOMBUR",		NULL},
	{0x0010, "SKUZ",		NULL},
	{0x0011, "ELF",			NULL},
	{0x0012, "SPARKLE",		NULL},
	{0x0013, "WAHO_NPC",	NULL},
	{0x0014, "SZOMBIE",		NULL},
	{0x0015, "ET_GETUP",	NULL},
	{0x0016, "DRAKE",		NULL},
	{0x0017, "SHAMAN",		NULL},
	{0x0018, "TCBODY",		NULL},
	{0x0019, "T0",			NULL},
	{0x001A, "T1",			NULL},
	{0x001B, "T2",			NULL},
	{0x001C, "TROLL",		NULL},
	{0x001D, "CHNDWARF",	NULL},
	{0x001E, "KING",		NULL},
	{0x001F, "WASP",		NULL},
	{0x0020, "SSKEL",		NULL},
	{0x0021, "PLTDWARF",	NULL},
	{0x0022, "SSKULL",		NULL},
	{0x0023, "BOULDR",		NULL},
	{0x0024, "TOADSTOL",	NULL},
	{0x0025, "FAEGIRL",		NULL},
	{0x0026, "FAEGLOW",		NULL},
	{0x0027, "FAEMALE",		NULL},
	{0x0028, "FAEGLOW",		NULL},
	{0x0029, "FUNGI_M",		NULL},
	{0x002A, "L3SNAKE",		NULL},
	{0x002B, "WAHOOKA",		NULL},
	{0x002C, "ICESHARG",	NULL},
	{0x002D, "THERA",		NULL},
	{0x002E, "ICEQUEEN",	NULL},
	{0x002F, "DRAGON",		NULL},
	{0x0030, "ET_MAIN",		NULL},
	{0x0031, "KING_SPC",	NULL},

	{0xFFFF, NULL, NULL}
};

const sk_obj_id_def sk_decorations[]=
{
	/*
	{0x0000, "PIPE",		NULL},
	{0x0001, "LEVER_UP",	NULL},
	{0x0002, "LEVER_DN",	NULL},
	{0x0003, "HIDBRICK",	NULL},
	{0x0004, "KEYHOLE",		NULL},
	{0x0005, "PUSHBUTN",	NULL},
	{0x0006, "CSW_FU",		NULL},
	{0x0007, "CSW_FD",		NULL},
	{0x0008, "CSW_EU",		NULL},
	{0x0009, "CSW_ED",		NULL},
	{0x000A, "RBWINDOW",	NULL},
	{0x000B, "HDBRKBUT",	NULL},
	{0x000C, "DARTHOLE",	NULL},
	{0x000D, "MIRROR",		NULL},
	{0x000E, "ORB",			NULL},
	{0x000F, "SIGN",		NULL},
	{0x0010, "THBUT2O",		NULL},
	{0x0011, "THBUT2I",		NULL},
	{0x0012, "MBUTTOUT",	NULL},
	{0x0013, "MBUTTIN",		NULL},
	{0x0014, "SMHOLE_E",	NULL},
	{0x0015, "SMHOLE_F",	NULL},
	{0x0016, "THBUTOUT",	NULL},
	{0x0017, "THBUTIN",		NULL},
	{0x0018, "NYN_NYNG",	NULL},
	{0x0019, "YN_NYNG",		NULL},
	{0x001A, "NYN_YNG",		NULL},
	{0x001B, "YN_YNG",		NULL},
	{0x001C, "PUSHBUTI",	NULL},
	{0x001D, "CRACK",		NULL},
	{0x001E, "ORC_HEAD",	NULL},
	{0x001F, "CMIRROR",		NULL},
	{0x0020, "CAVEBTNO",	NULL},
	{0x0021, "CAVEBTNI",	NULL},
	{0x0022, "THPANEL",		NULL},
	{0x0023, "ICESYM",		NULL},
	{0x0024, "TAPE_I",		NULL},
	{0x0025, "TAPE_II",		NULL},
	{0x0026, "D1SIGN",		NULL},
	{0x0027, "SKEYHOLE",	NULL},
	{0x0028, "DOOR_D1",		NULL},
	{0x0029, "DOOR_D2",		NULL},
	{0x002A, "DOOR_D3",		NULL},
	{0x002B, "DOOR_DWF",	NULL},
*/

	{ 0x000F, "Window",		NULL},
	{ 0x002B, "Text",		NULL},
	{ 0x002D, "Door",		NULL},
	{ 0x0003, "Wall chest",	NULL},
	{ 0x0004, "Lock",		NULL},
	{ 0x0001, "Switch",		NULL},
	{ 0x0000, "P.hole",		NULL},
	{0xFFFF,  NULL, 		NULL}
};

char * get_obj_name(unsigned short id)
{
	int i;

	i = 0;
	while( ( sk_objects[i].id != 0xFFFF ) && ( sk_objects[i].id != id ) )
	{
		i++;
	}

	return sk_objects[i].name;
}

char * get_obj_desc(unsigned short id)
{
	int i;

	i = 0;
	while( ( sk_objects[i].id != 0xFFFF ) && ( sk_objects[i].id != id ) )
	{
		i++;
	}

	return sk_objects[i].desc;
}

char * get_fixed_obj_name(unsigned short id)
{
	int i;

	i = 0;
	while( ( sk_fixed_objects[i].id != 0xFFFF ) && ( sk_fixed_objects[i].id != id ) )
	{
		i++;
	}

	return sk_fixed_objects[i].name;
}

char * get_monster_name(unsigned short id)
{
	int i;

	i = 0;
	while( ( sk_monters[i].id != 0xFFFF ) && ( sk_monters[i].id != id ) )
	{
		i++;
	}

	return sk_monters[i].name;
}

char * get_decoration_name(unsigned short id)
{
	int i;

	i = 0;
	while( ( sk_decorations[i].id != 0xFFFF ) && ( sk_decorations[i].id != id ) )
	{
		i++;
	}

	return sk_decorations[i].name;
}
