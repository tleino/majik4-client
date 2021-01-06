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

const static char rcsid[] = "$@asis{}Id$";

#include "sysdep.h"
#include "gridmap.h"
#include "keyboard.h"
#include "character.h"
#include "config.h"
#include "target.h"

int target_x = -1, target_y = -1;
char target_command[80];

void
APISelectTarget (int x, int y, char *command)
{
  coord *xy;
  int oid, i, c;
   
  target_x = x;
  target_y = y;
   
  strcpy (target_command, command);
   
  DisplayMap (target_x, target_y);
   
  while (1)
    {
      c = WGetKey (WINDOW_PROMPT);
		
      if (c == ERR)
	return;
				
      switch (c)
	{
	case '7':
	  target_y--;
	  target_x--;
	  break;
	case '8':
	  target_y--;
	  break;
	case '9':
	  target_y--;
	  target_x++;
	  break;
	case '4':
	  target_x--;
	  break;
	case '6':
	  target_x++;
	  break;
	case '1':
	  target_y++;
	  target_x--;
	  break;
	case '2':
	  target_y++;
	  break;
	case '3':
	  target_y++;
	  target_x++;
	  break;
	case 't':
	  oid = NextObject ();
	  xy = (coord *) QueryObjectCoord (oid);
			 
	  while (1)
	    {
	      if ((int) IsVisible(c_x, c_y, xy[0], xy[1], SIGHT_RADIUS, TRUE)
		  >= TWILIGHT_LIMIT)
		{
		  target_x = xy[0];
		  target_y = xy[1];
		  break;
		}
	      else
		{
		  oid = NextObject ();
		  xy = (coord *) QueryObjectCoord (oid);
		  continue;
		}
	    }
			 
	  break;
	case 's':
	  SocketWrite ("%s %d %d", target_command, target_x, target_y);
	  break;
	}
		
      if (tolower (c) == 'z' || tolower (c) == 's')
	break;
		
      ChangePrompt (FALSE, ECHO_QUIET, TRUE, NULL, NULL);
      DisplayMap (target_x, target_y);
		
      if ((int) IsVisible(c_x, c_y, target_x, target_y, SIGHT_RADIUS, TRUE)
	  >= TWILIGHT_LIMIT)
	{
	  if (QueryObjectId (target_x, target_y))
	    SocketWrite ("blook %d %d", target_x, target_y);
	  else
	    {
	      switch (TileAt (target_x, target_y))
		{
		case TILE_LADDER_UP:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A ladder leading up");
		  break;
		case TILE_LADDER_DOWN:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A ladder leading down");
		  break;
		case TILE_OPEN_DOOR:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A open door");
		  break;
		case TILE_CLOSED_DOOR:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A closed door");
		  break;
		case TILE_CRACK:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A crack");
		  break;
		case TILE_EARTH:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A piece of ground");
		  break;
		case TILE_UG_EARTH:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A piece of ground");
		  break;
		case TILE_ICE:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A piece of icy ground");
		  break;
		case TILE_GRASS:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"Grass");
		  break;
		case TILE_L_GRASS:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"Long grass");
		  break;
		case TILE_MARSH:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"Marsh");
		  break;
		case TILE_SAND:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A piece of sand covered ground");
		  break;
		case TILE_SNOW:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A piece of snow covered ground");
		  break;
		case TILE_BURNT:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop", 
				"A piece of burnt ground");
		  break;
		case TILE_FLOOR:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A piece of floor");
		  break;
		case TILE_STONE_FLOOR:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A piece of stone floor");
		  break;
		case TILE_WOOD_FLOOR:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A piece of wooden floor");
		  break;
		case TILE_CAVE_FLOOR:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A piece of cave floor");
		  break;
		case TILE_ACID:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A piece of acid covered ground");
		  break;
		case TILE_WATER:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"Water");
		  break;
		case TILE_D_WATER:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"Deep water");
		  break;
		case TILE_UG_WATER:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"Water");
		  break;
		case TILE_D_UG_WATER:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"Deep water");
		  break;
		case TILE_WALL:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A solid wall");
		  break;
		case TILE_EARTH_WALL:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"Earth wall!?");
		  break;
		case TILE_METAL_WALL:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A metal wall");
		  break;
		case TILE_STONE_WALL:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A stone wall");
		  break;
		case TILE_WOOD_WALL:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A wooden wall");
		  break;
		case TILE_ROCK:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"Solid rock");
		  break;
		case TILE_STALAGMITE:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A stalagmite");
		  break;
		case TILE_BUSH:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A bush");
		  break;
		case TILE_S_BUSH:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A small bush");
		  break;
		case TILE_L_BUSH:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A large bush");
		  break;
		case TILE_CON_TREE:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A conifer tree");
		  break;
		case TILE_S_CON_TREE:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A small conifer tree");
		  break;
		case TILE_L_CON_TREE:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A large conifer tree");
		  break;
		case TILE_DEC_TREE:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A deciduous tree");
		  break;
		case TILE_S_DEC_TREE:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A small deciduous tree");
		  break;
		case TILE_L_DEC_TREE:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A large deciduous tree");
		  break;
		case TILE_JUN_TREE:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A jun(?) tree");
		  break;
		case TILE_S_JUN_TREE:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A small jun(?) tree");
		  break;
		case TILE_L_JUN_TREE:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A large jun(?) tree");
		  break;
		}
	    }
	}
      else
	{
	  i = (int) IsVisible (c_x, c_y, target_x, target_y, SIGHT_RADIUS,
			       TRUE);
			 
	  if (i < TWILIGHT_LIMIT && i > 0)
	    ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
			  "There is too dark to see it properly.");
	  else
	    ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
			  "You can't see that far!");
	}
		
      return;
    }
   
  target_x = -1;
  target_y = -1;
   
  ChangePrompt (FALSE, ECHO_QUIET, TRUE, NULL, NULL);
  WPrtStr (WINDOW_TEXT, "You can move again.\n");
  WUpdate (WINDOW_TEXT);
  DisplayMap (c_x, c_y);
  
  return;
}
