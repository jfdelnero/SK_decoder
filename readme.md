# SK_decoder toolkit

### sk_ungroup - Unpack group files

Support:

 - "GROUP" Files (Files package)

Commmand:

./sk_ungroup group_files [-L]

### sk_sfx2wav - Convert sound files to wav files.

Support:

 - ".CPR" Files (Sound FX)

 - "VOICES" Files (Voices / dialogue audio files - extract the subtitles files too)

Command:

./sk_sfx2wav [-sfx/-voices] [SFX_FILE.CPR/VOICES] files

Note: use -sfx for .CPR files, -voices for "VOICES" file.

### sk_gfx2bmp - Convert graphics files to bmp files

Support:

 - ".MON" Files (Characters sprites animations)

 - ".Gxx" Files (More Characters sprites animations)

 - ".MSP" Files (General sprites)

 - ".POJ" Files (Objects sprites)

 - ".WPN" Files (Weapons animations)

 - ".MMP" Files (Book font/sprites)

 - ".UBG" Files (Maze animations)

 - ".PAL" Files (Palette files)

Command:

./sk_gfx2bmp  [-mon/-mmp/-ubg] [-pal1:palettefile1.pal] [-pal2:palettefile2.pal] [gfx_file.MON/.POJ/.WPN/.MSP/.G*/.MMP/.UBG files]

Note: use -mon for .MON/.Gxx/.POJ/.WPN files, -mmp for ".MMP" files and -ubg for ".UBG" files.

### No "yet?" supported formats :

 - ".BGF"
 - ".MAZ" (Maze map)
 - ".IXY" (Sprite location information ?)
 - ".Sxx" (Script ?)
 - ".MMS" (Script ?)
 - ".SCX" (Font)
 - ".STR" (Strings for command line parser ?)
 - ".TBJ" (Data structure definition text files).
 - ".DAT" (Animation related arrays)
 - ".DEF" (Object types definition ?)
 - ".ENC"
 - ".BNK" (Midi instrument patch)
 - ".HMP" (Human Machine Interfaces MIDI P (HMP) Music files)
 - ".MVE" (Movie files)

-------------------------------------------------------------------------------

SK_decoder

Copyright (C) 2002-2024  Jean-François DEL NERO / HxC2001

-------------------------------------------------------------------------------
