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

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "sysdep.h"
#include "list.h"
#include "config.h"
#include "socket.h"

extern WINDOW *list;
char api_command[80], api_data[8192];
int api_type, api_scrollpos = 0;
extern int COLS, LINES;

void
APIScrollWindow (int forward)
{
  char buf[256];
   
  /* KLUDGE */
   
  if (forward)
    WPrtStr (WINDOW_TEXT, "APIScrollWindow: under construction\n");
  else
    WPrtStr (WINDOW_TEXT, "APIScrollWindow: under construction\n");
   
  WUpdate (WINDOW_TEXT);
}

int
APIMatchSelect (int c)
{
  int i, count = 0;
  char buf[256];
   
  if (list == NULL || api_type == LIST_TEXT)
    return 0;
   
  memset (buf, 0, sizeof (buf));
  strcpy (buf, "");
   
  for (i = 0; i < strlen(api_data); i++)
    {
      if (api_data[i] == ';')
	{			 
	  if (toupper(c) == 'A'+count)
	    {				  
	      SocketWrite ("%s %s", api_command, buf);
				  
	      APICloseWindow();
	      return 1;
	    }
	  else
	    {
	      count++;
	      memset (buf, 0, sizeof (buf));
	      strcpy (buf, "");
	      continue;
	    }
	}
      sprintf (buf, "%s%c", buf, api_data[i]);
    }
   
  return 0;
}

void
APIOpenWindow (int width, int height, int start_x, int start_y, int type,
	       char *title, char *command, char *data)
{
  int i, max_height = 0, max_width = 0, w = 0, h = 0, count = 0;
  char *buf;
      
  memset (api_data, 0, sizeof (api_data));
  strncpy (api_data, data, strlen (data));
   
  memset (api_command, 0, sizeof (api_command));
  strncpy (api_command, command, strlen (command));
      
  api_type = type;
   
  if (list != NULL)
    APICloseWindow();
   
  for (i = 0; i < strlen(data); i++)
    {
      if (data[i] == ';')
	{
	  if (w > max_width)
	    max_width = w;
			 
	  w = 0;
	  h++;
	}
		
      w++;
    }
   
  max_height = h;
      
  if (title != NULL)
    {
      if (max_width < strlen (title))
	max_width = strlen (title);
		
      max_height += 2;
    }
  
  if (max_width < strlen (" (z): close window (+): page up (-): page down"))
    max_width = strlen (" (z): close window (+): page up (-): page down");
   
  if (type == LIST_SELECT)
    max_width += 3;
   
  /* Borders, footer etc. */
   
  max_height += 4;
  max_width += 4;
   
  if (width == 0 || width < max_width)
    width = max_width;
  if (height == 0 || height < max_height)
    height = max_height;
   
  if (width >= COLS)
    width = COLS-1;
  if (height >= LINES)
    height = LINES-1;
   
  if (start_x == 0)
    start_x = GRIDMAP_X+2;   
  if (start_y == 0)
    start_y = 0;
   
  list = CreateWindow (height, width, start_y, start_x);
  WSetColor (WINDOW_LIST, C_BLUE, 0);
  WBox (WINDOW_LIST, 0, 0);
  WSetColor (WINDOW_LIST, C_LIGHT_GRAY, 0);
  WCursor (WINDOW_LIST, 1, 1);
   
  buf = malloc (max_width);
  memset (buf, 0, sizeof (buf));
   
  if (title != NULL)
    {
      WSetColor (WINDOW_LIST, C_YELLOW, 0);
      WPrtStr (WINDOW_LIST, "%s", title);
      WCursor (WINDOW_LIST, 1, 3);
      WSetColor (WINDOW_LIST, C_LIGHT_GRAY, 0);
      count += 2;
    }
   
  strcpy (buf, " ");
   
  for (i = 0; i < strlen(data); i++)
    {
      if (data[i] == ';')
	{
	  if (type == LIST_TEXT)
	    {
	      WPrtStr (WINDOW_LIST, "%s", buf);
	      WCursor (WINDOW_LIST, 1, 2+count);
	    }
	  else
	    {
	      WPrtStr (WINDOW_LIST, " %c -%s", 'A'+(title ? (count-2) : count),
		       buf);
	      WCursor (WINDOW_LIST, 1, 2+count);
	    }
			 
	  count++;
	  i++;
			 
	  memset (buf, 0, sizeof (buf));
	  strcpy (buf, " ");
	}
      sprintf (buf, "%s%c", buf, data[i]);
    }
   
  WCursor (WINDOW_LIST, 1, max_height-2);
  WSetColor (WINDOW_LIST, C_BROWN, 0);
  WPrtStr (WINDOW_LIST, " (z): close window (+): page up (-): page down");
  WCursor (WINDOW_LIST, 1, max_height-3);
  WSetColor (WINDOW_LIST, C_LIGHT_GRAY, 0);
  WUpdate (WINDOW_LIST);
}

void
APICloseWindow (void)
{
  WClear (WINDOW_LIST);
  WUpdate (WINDOW_LIST);
  // FIXME: Crashes for some strange reason with new ncurses?
  // DestroyWindow (WINDOW_LIST);
   
  list = NULL;
   
  touchwin (GetWinHandle (WINDOW_MAP));
  touchwin (GetWinHandle (WINDOW_DESCRIPTION));
  touchwin (GetWinHandle (WINDOW_TEXT));
  touchwin (GetWinHandle (WINDOW_SCORE));
  touchwin (GetWinHandle (WINDOW_PROMPT));
   
  WUpdate (WINDOW_MAP);
  WUpdate (WINDOW_DESCRIPTION);
  WUpdate (WINDOW_TEXT);
  WUpdate (WINDOW_SCORE);
  WUpdate (WINDOW_PROMPT);
}
