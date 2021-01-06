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

const static char rcsid[] = "$Id: mcp.c,v 1.69 1998/12/14 21:18:39 yorkaturr Exp $";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "sysdep.h"
#include "mcp.h"
#include "error.h"
#include "keyboard.h"
#include "gridmap.h"
#include "socket.h"
#include "character.h"
#include "object.h"
#include "config.h"
#include "list.h"
#include "sound.h"
#include "mapgen.h"
#include "dialog.h"
#include "target.h"

int current_command;
extern int look_tx, look_ty, looking; /* for look kludge */
extern coord map_x, map_y;

void
ParseCommand (struct MCP_COMMAND *input)
{
  struct object o;
  int i = 0, j = 0, count = 1, oid;
  int32 x, y, radius;
  char *buf; /* Temporary buffer for parsing */
  char tile;
  char *map;
  DIALOG *win;
   
  /* for CMD_LIST */
  int width, height, start_x, start_y, type;
  char title[80], command[80];
   
  /* for CMD_ANIMATE */
  int x1 = 0, y1 = 0, x2 = 0, y2 = 0, animate_color = 0, animate_char = 0;
  float bonus = 0.0, ax = 0.0, ay = 0.0;
  int dx = 0, dy = 0;
  int right = 0, down = 0;
  int divi = 0;
   
  buf = (char *)malloc(input->datalen * sizeof(char));
   
  memset (buf, 0, input->datalen);

  switch (input->command)
    {
    case CMD_START:
      c_online = TRUE;
		
      ClearScreen ();
      InitWindows ();
		
      k_cline = FALSE;
      current_command = CMD_START;
      SocketWrite(VERSION);
      current_command = CMD_COMMAND;
      break;
    case CMD_ASK_NAME:
      Cursor (1, LINES-2);
      k_echo = ECHO_NORMAL;
      WSetColor (WINDOW_PROMPT, C_LIGHT_GRAY, 0);
      ChangePrompt (TRUE, ECHO_NORMAL, FALSE, NULL, "Your character's name: ");
      current_command = CMD_LOGIN_NAME;
      break;
    case CMD_ASK_PASS:
      Cursor (1, LINES-2);
      k_echo = ECHO_PASSWORD;
      WSetColor (WINDOW_PROMPT, C_LIGHT_GRAY, 0);
      ChangePrompt (TRUE, ECHO_PASSWORD, FALSE, NULL, "Password: ");
      current_command = CMD_LOGIN_PASS;
      break;
    case CMD_QUIT:
      Die (input->data);
		
      break;
    case CMD_SEE:
    case CMD_HEAR:
    case CMD_SMELL:
    case CMD_FEEL:
    case CMD_TASTE:
    case CMD_SENSE:
    case CMD_OOC:
    case CMD_INFO:
    case CMD_MSG:
		
      sscanf (input->data, "%s", buf);
      i = strlen(buf);
      sscanf (input->data, "%*s %s", buf);
      i += strlen(buf);
      sscanf (input->data, "%*s %*s %s", buf);
      i += strlen(buf)+3;
		
      for (count=0;i<strlen(input->data);i++, count++)
	buf[count] = input->data[i];
      buf[count] = '\0';
		 
      if (sscanf (input->data, "%d %d %d", &x, &y, &radius) != 3)
	radius = 0;
		
      if ((radius && (int)IsVisible(x, y, c_x, c_y, radius,1)) || !radius
	  || input->command != CMD_SEE)
	{
	  if (c_online == FALSE)
	    {
	      Cursor (1, LINES);
	      SetColor (C_LIGHT_RED);
				  
	      PrtStr ("%s\n", buf);
	      Update ();
	    }
	  else
	    {
	      WPrtStr (WINDOW_TEXT, "%s\n", buf);
	      WUpdate (WINDOW_TEXT);
	      PlaceCursor(x, y);
	    }
	}

      break;
    case CMD_MAP:
      UpdateMap (input->data);
      //		UpdateLightMap ();
      //		DisplayMap(c_x, c_y);
      break;
    case CMD_MAP_SLICE:
      if (sscanf (input->data, "%d %s", &x, buf) == 2)
	{
	  UpdateMapPiece(x, buf);
	}
      break;
    case CMD_LOCK_MAP:
      maplock = TRUE;
      break;
    case CMD_RELEASE_MAP:
      maplock = FALSE;
      break;
    case CMD_UPDATE_LIGHT:
      UpdateLightMap();
      DisplayMap(c_x, c_y);
      break;
    case CMD_GENERATE_MAP:
      map = (char *)malloc((100 * (100+1)+1) * sizeof(char));
      map[100*(100+1)+1] = '\0';
      GenerateMap(map, 100, 100, input->data);
      current_command = CMD_MAP;
      SocketWriteRaw(map, strlen(map));
      UpdateMap(map);
      free(map);
      break;
    case CMD_MAP_COORD:
      if (sscanf (input->data, "%d#%d", &x, &y) == 2)
	{
	  map_x = x;
	  map_y = y;
	}
      break;
    case CMD_TILE:
      if (sscanf (input->data, "%d %d %c", &x, &y, &tile) == 3)
	{
	  UpdateTile (x, y, tile);
	}
      break;
    case CMD_XYPOS:
      if (sscanf (input->data, "%d#%d", &x, &y) == 2)
	{
	  c_x = x;
	  c_y = y;
	  UpdateLightMap();
			 
	  if (!looking)
	    DisplayMap(c_x, c_y);
	  else
	    DisplayMap(look_tx, look_ty);
	}
      break;
    case CMD_SET_LIGHT:
      if (sscanf (input->data, "%d", &i) == 1)
	{
	  SetLightLevel( i );
	  UpdateLightMap();
	  DisplayMap(c_x, c_y);
	}
      break;
    case CMD_DESCRIPTION:
      WCursor (WINDOW_DESCRIPTION, 0, 0);
		
      /* This kludge is softer than WClear() since if we use WClear() the
       * whole shit is updated and that is not nice!
       */
		
      for (i=0;i<GRIDMAP_Y;i++)
	{
	  WDelLine (WINDOW_DESCRIPTION);
	  WCursor (WINDOW_DESCRIPTION, i, 0);
	}
		
      WCursor (WINDOW_DESCRIPTION, 0, 0);
		
      WPrtStr (WINDOW_DESCRIPTION, WrapStr (input->data, COLS-(GRIDMAP_X+10)));
      WUpdate (WINDOW_DESCRIPTION);
      UpdateScore();
      break;
    case CMD_DIALOG:
      if (sscanf (input->data, "%d %s %s %s %s", &win->type, win->callback, win->title, win->text, win->args) == 5)
	APIOpenDialog (win);
      else
	APIOpenWindow (0, 0, 0, 0, 0, "API Error", "noop",
		       "Error: The list contains no data.;");
      break;
    case CMD_TARGET:
      if (sscanf (input->data, "%s", command) == 1)
	{
	  for (i = 0; i < strlen(command); i++)
	    if (command[i] == '_')
	      command[i] = ' ';
			 
	  WPrtStr (WINDOW_TEXT, "(^c1s^c0=select target ^c1t^c0=toggle " \
		   "target ^c1z^c0=exit)\n");
	  WUpdate (WINDOW_TEXT);
			 
	  APISelectTarget (c_x, c_y, command);
	}
      else
	{
	  WPrtStr (WINDOW_TEXT, "Error: Not enough information for " \
		   "CMD_TARGET.\n");
	  WUpdate (WINDOW_TEXT);
	}
      break;
    case CMD_ANIMATE:
      if (sscanf (input->data, "%d %d %d %d %d %d", &animate_char,
		  &animate_color, &x1, &y1, &x2, &y2) == 6)
	{
	  dx = x2 - x1;
	  dy = y2 - y1;
			 			 
	  if (!dx && !dy)
	    break;
			 
	  if (x2 > x1)
	    right = 1;
			 
	  if (y2 > y1)
	    down = 1;
			 
	  if (abs(dx) >= abs(dy))
	    {
	      if (dx)
		bonus += (float) dy / abs(dx);
				  
	      ay = (float) y1 + bonus;
				  
	      if (right)
		{
		  divi = abs (x1+1-x2);
					   
		  if (divi == 0)
		    divi = 1;
					   
		  for (i=x1+1,j=x2;i<j;i++)
		    {
		      usleep (DEFAULT_SPEED / divi);
		      PrtGrid(i, (int)(ay + 0.5), animate_char, animate_color);
		      ay += bonus;
		    }
		}
	      else
		{
		  divi = abs(x1-1-x2);
					   
		  if (divi == 0)
		    divi = 1;
					   
		  for (i=x1-1,j=x2;i>j;i--)
		    {
		      usleep (DEFAULT_SPEED / divi);
		      PrtGrid(i, (int)(ay + 0.5), animate_char, animate_color);
		      ay += bonus;
		    }
		}
	    }
	  else
	    {
	      if (dy)
		bonus += (float) dx / abs(dy);
				  
	      ax = (float) x1 + bonus;
				  
	      if (down)
		{
		  divi = abs (y1+1-y2);
					   
		  if (divi == 0)
		    divi = 1;
					   
		  for (i=y1+1,j=y2;i<j;i++)
		    {
		      usleep (DEFAULT_SPEED / divi);
		      PrtGrid((int)(ax + 0.5), i, animate_char, animate_color);
		      ax += bonus;
		    }
		}
	      else
		{
		  divi = abs (y1-1-y2);
					   
		  if (divi == 0)
		    divi = 1;
					   
		  for (i=y1-1,j=y2;i>j;i--)
		    {
		      usleep (DEFAULT_SPEED / divi);
		      PrtGrid((int)(ax + 0.5), i, animate_char, animate_color);
		      ax += bonus;
		    }
		}
	      if (target_x != -1 && target_y != -1)
		DisplayMap (target_x, target_y);
	      else
		DisplayMap (c_x, c_y);
				  
	      k_no_delay = 1;
	    }
	}
      break;
    case CMD_LIST:		
      if (sscanf (input->data, "%d %d %d %d %d %s %s %s", &width, &height,
		  &start_x, &start_y, &type, title, command, buf) == 8)
	{
	  for (i = 0; i < strlen(title); i++)
	    if (title[i] == '_')
	      title[i] = ' ';
			 
	  for (i = 0; i < strlen(command); i++)
	    if (command[i] == '_')
	      command[i] = ' ';
			 
	  for (i = 0; i < strlen(buf); i++)
	    if (buf[i] == '_')
	      buf[i] = ' ';
			 			 
	  APIOpenWindow (width, height, start_x, start_y, type, title,
			 command, buf);
	}
      else
	APIOpenWindow (0, 0, 0, 0, 0, "API Error", "noop", "Error: The list " \
		       "contains no data.;");
		
      break;
    case CMD_SPEED:
      c_speed = atoi (input->data);
      break;
    case CMD_FREEZE:
      c_freeze = atoi (input->data);
      break;
    case CMD_FLUSH:
      FlushInput();
      break;
    case CMD_PROMPT:
      if (sscanf (input->data, "%d %s %s", &type, command, buf) == 3)
	{
	  for (i = 0; i < strlen(command); i++)
	    if (command[i] == '_')
	      command[i] = ' ';
			 
	  for (i = 0; i < strlen(buf); i++)
	    if (buf[i] == '_')
	      buf[i] = ' ';
			 
	  if (type == 0)
	    ChangePrompt (TRUE, ECHO_NORMAL, FALSE, command, buf);
	  else
	    ChangePrompt (FALSE, ECHO_QUIET, FALSE, command, buf);
	}
      break;
    case CMD_SOUND:
      PlaySound (atoi(input->data));
      break;
      /* OBJECT HANDLING */
    case CMD_ADD_OBJECT:
      if (sscanf (input->data, "%d", &o.oid) == 1)
	{
	  o.character = 0;
	  o.x = 0; 
	  o.y = 0;
	  o.light = 0;
	  o.invis = 0;
	  o.next = (struct object *)NULL;
	  AddObject(o);
	}
      break;
    case CMD_DEL_OBJECT:  
      if (sscanf (input->data, "%d", &oid) == 1)
	{
	  RemoveObject(oid);
	  UpdateLightMap();
			 
	  if (target_x == -1 && target_y == -1)
	    DisplayMap(c_x, c_y);
	  else
	    DisplayMap(target_x, target_y);
	}
      break;
    case CMD_MOVE_OBJECT:
      if (sscanf (input->data, "%d %d %d", &oid, &x, &y) == 3)
	{
	  MoveObject(oid, x, y);
	}
      break;
    case CMD_INFO_OBJECT:
      if (sscanf (input->data, "%d %d %d %d %d %d %s", 
		  &o.oid, &o.character, &o.color,
		  &o.light, &o.physical, &o.invis, o.name) == 7)
	{
	  o.x = -1;
	  o.y = -1;
	  UpdateObject(o);
	}
      break;
    case CMD_DEL_OBJECTS:
      DeleteObjects();
      break;
    case CMD_OID_OWN: 
      if (sscanf(input->data, "%d", &i) == 1)
	c_oid = i;
      break;
    case CMD_PLR_HP:
      c_hp = atoi (input->data);
      UpdateScore();
      break;
    case CMD_PLR_SP:
      c_sp = atoi (input->data);
      UpdateScore();
      break;
    case CMD_PLR_EP:
      c_ep = atoi (input->data);
      UpdateScore();
      break;
    case CMD_PLR_STAT:
      sscanf (input->data, "%d %d %d %d %d %d %d %d %d",
	      &c_stats[A_STR].current, &c_stats[A_DEX].current,
	      &c_stats[A_STU].current, &c_stats[A_CON].current,
	      &c_stats[A_AGI].current, &c_stats[A_INT].current,
	      &c_stats[A_WIS].current, &c_stats[A_POW].current,
	      &c_stats[A_CHA].current);
		
      UpdateScore();
      break;
    case 0: 
      break;
    default:
      WPrtStr (WINDOW_TEXT, "Unknown protocol command: %d\n",
	       input->command);
      WUpdate (WINDOW_TEXT);
      break;
    }	
  free(buf);
}

int
PrtGrid (int x, int y, int character, int color)
{
  int i = 0, tx = 0, ty = 0;
   
  i = IsVisible (c_x, c_y, x, y, SIGHT_RADIUS, TRUE);
   
  if (i >= -1 && abs(x - x) < (GRIDMAP_X-1)/2+1 && abs(y - y)
      < (GRIDMAP_Y-1)/2+1)
    {	
      tx = (x - (GRIDMAP_X-1)/2);
      ty = (y - (GRIDMAP_Y-1)/2);
		
      if (tx < 0)
	tx = 0;
      if (ty < 0)
	ty = 0;
		
      DisplayMap (x, y);
		
      WCursor (WINDOW_MAP, x - tx, y - ty);
      if (i >= TWILIGHT_LIMIT)
	WSetColor (WINDOW_MAP, color, 0);
      else
	WSetColor (WINDOW_MAP, C_DARK_GRAY, 0);
		
      WPrtChar (WINDOW_MAP, character);
      WUpdate (WINDOW_MAP);
    }
}
