/* Majik 4 client 
 * Copyright (C) 1998, 1999  Majik Development Team <majik@majik.netti.fi> 
 * 
 * This program is free software; you can redistribute it and/or modify 
 * it under the terms of the GNU General Public License as published by 
 * the Free Software Foundation; either version 2 of the License, or 
 * (at your option) any later version. 
 * 
 * This program is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU General Public License for more details. 
 *  
 * You should have received a copy of the GNU General Public License 
 * along with this program; if not, write to the Free Software 
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 */ 

const static char rcsid[] = "$Id: sound.c,v 1.2 1998/10/31 19:50:46 namhas Exp $";

#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "sysdep.h"
#include "sound.h"

#define PLAY_CMD "/usr/bin/play 2>/dev/null"

void
PlaySound (int sfx)
{
#ifdef ENABLE_SOUND
  char cmd[256];
   
  switch (sfx)
    {
    case SFX_DIE:
      strcpy (cmd, "" PLAY_CMD " " DATA_DIR "sound/agony.8svx &");
      break;
    case SFX_CLOSE_DOOR:
      strcpy (cmd, "" PLAY_CMD " " DATA_DIR "sound/closedoor.8svx &");
      break;
    case SFX_OPEN_DOOR:
      strcpy (cmd, "" PLAY_CMD " " DATA_DIR "sound/opendoor.8svx &");
      break;
    case SFX_HIT:
      strcpy (cmd, "" PLAY_CMD " " DATA_DIR "sound/hit.8svx &");
      break;
    case SFX_HIT2:
      strcpy (cmd, "" PLAY_CMD " " DATA_DIR "sound/hit2.8svx &");
      break;
    case SFX_HIT3:
      strcpy (cmd, "" PLAY_CMD " " DATA_DIR "sound/hit3.8svx &");
      break;
    case SFX_KILL:
      strcpy (cmd, "" PLAY_CMD " " DATA_DIR "sound/kill.8svx &");
      break;
    case SFX_LOGOFF:
      strcpy (cmd, "" PLAY_CMD " " DATA_DIR "sound/logoff.8svx &");
      break;
    case SFX_MISS:
      strcpy (cmd, "" PLAY_CMD " " DATA_DIR "sound/miss.8svx &");
      break;
    case SFX_MISS2:
      strcpy (cmd, "" PLAY_CMD " " DATA_DIR "sound/miss2.8svx &");
      break;
    case SFX_WALL:
      strcpy (cmd, "" PLAY_CMD " " DATA_DIR "sound/wall.8svx &");
      break;
    }
   
  system (cmd);
#endif /* ENABLE_SOUND */
}
