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

const static char rcsid[] = "$Id: dialog.c,v 1.1 1998/12/08 19:25:47 namhas Exp $";

#include <sysdep.h>

#include "config.h"
#include "dialog.h"

void
APIOpenDialog (DIALOG *win)
{
  int i, j;
   
  win->scrollpos = 0;
  win->width = 0;
  win->height = 0;
   
  WPrtStr (WINDOW_TEXT, "APIOpenDialog\n");
  WUpdate (WINDOW_TEXT);
  WPrtStr (WINDOW_TEXT, "dialog created: %d %s %s %s %s %d\n",
	   win->type, win->callback, win->title, win->text, win->args,
	   win->scrollpos);
  WUpdate (WINDOW_TEXT);
   
  strcpy (win->text, (char *) WrapStr (win->text, COLS-GRIDMAP_X+10));
   
  /* Calculate width and height */
   
  if (win->width < strlen(win->title))
    win->width = strlen(win->title);
   
  win->height++;
   
  for (i=0;i<strlen(win->text);i++)
    {
      if (win->text[i] == '\n')
	{
	  win->height++;
	  j=0;
	}
      else
	j++;
		
      if (j > win->width)
	win->width = j;
    }
   
  win->height += 4;
  win->width += 4;
   
  if (win->width >= COLS-GRIDMAP_X+2)
    win->width = COLS-GRIDMAP_X+2;
  if (win->height >= LINES-1)
    win->height = LINES-1;
   
  WPrtStr (WINDOW_TEXT, "win->width: %d win->height: %d\n", win->width,
	   win->height);
  WUpdate (WINDOW_TEXT);
   
  switch (win->type)
    {
    case DIALOG_YESNO:
    case DIALOG_MSGBOX:
    case DIALOG_INPUTBOX:
    case DIALOG_TEXTBOX:
    case DIALOG_MENU:
    case DIALOG_CHECKLIST:
    case DIALOG_RADIOLIST:
      break;
    }
}
