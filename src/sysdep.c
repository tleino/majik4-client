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

#ifdef HAVE_SYS_TIME_H
# include <sys/time.h>
# include <sys/types.h>
#endif /* HAVE_SYS_TIME_H */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#include "error.h"
#include "sysdep.h"
#include "character.h"
#include "socket.h"
#include "keyboard.h"
#include "gridmap.h"
#include "config.h"
#include "string.h"

const static char rcsid[] = "$Id: sysdep.c,v 1.39 1998/12/14 10:30:49 namhas Exp $";
BOOL initialized, windows = FALSE;

int scroll_pos;
int last_ustime = 0;

#ifdef HAVE_LIBCURSES
WINDOW *score, *map, *description, *text, *prompt, *text_status, *list;
#elif defined(HAVE_LIBCURSES)
WINDOW *score, *map, *description, *text, *prompt, *text_status, *list;
#else
# ifdef LIBGTK
GtkWidget *window, *score, *map, *description, *text, *prompt, *text_status, *vbox, *hbox, *list;
#endif /* LIBGTK */
#endif /* HAVE_LIBCURSES */

#ifndef HAVE_LIBCURSES
int LINES, COLS;
#endif /* HAVE_LIBCURSES */

int gridmap_width, gridmap_height;

void
InitScreen (void)
{
#ifdef HAVE_LIBCURSES
  if (initscr() == NULL)
    Die ("initscr: Could not open the screen");
   
  if (LINES < 24 || COLS < 80)
    Die ("majik: Requires at least 24x80 screen");
   
  noecho ();
  cbreak ();	
#else
# ifdef HAVE_LIBGTK
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
   
  gtk_signal_connect (GTK_OBJECT (window), "destroy",
		      GTK_SIGNAL_FUNC (Die), NULL);
   
  gtk_window_set_title (GTK_WINDOW (window),
			"Majik4 - the forth incarnation of Majik");
  gtk_widget_set_usize (GTK_WIDGET (window), 640, 480);
   
  gtk_widget_show (window);
#endif /* HAVE_LIBCURSES */
#endif /* HAVE_LIBGTK */	
   
  InitColors ();
  initialized = TRUE;
}

void
ClearScreen (void)
{
#ifdef HAVE_LIBCURSES
  clear ();
#endif /* HAVE_LIBCURSES */
  Update();
}

void
EndScreen (void)
{
  if (initialized)
    {
      ClearScreen ();
#ifdef HAVE_LIBCURSES
      endwin ();
#endif /* HAVE_LIBCURSES */
    }
}

WINDOW *
GetWinHandle(byte wnum)
{
#ifdef HAVE_LIBCURSES
  switch (wnum)
    {
    case WINDOW_SCORE:
      return score;
    case WINDOW_MAP:
      return map;
    case WINDOW_DESCRIPTION:
      return description;
    case WINDOW_TEXT:
      return text;
    case WINDOW_PROMPT:
      return prompt;
    case WINDOW_LIST:
      return list;
    }
  Die("getwinhandle(): Unknown window");
#else
# ifdef HAVE_LIBGTK
  switch (wnum)
    {
    case WINDOW_SCORE:
      return score;
    case WINDOW_MAP:
      return map;
    case WINDOW_DESCRIPTION:
      return description;
    case WINDOW_TEXT:
      return text;
    case WINDOW_PROMPT:
      return prompt;
    }
  Die("getwinhandle(): Unknown window");
# endif /* HAVE_LIBGTK */
#endif /* HAVE_LIBCURSES */
  return NULL;
}
		
void
InitWindows (void)
{
  windows = TRUE;
  browse_text = FALSE;
  scroll_pos = 0;
   
  if (GRIDMAP_Y < 3 || GRIDMAP_X < 3)
    Die ("The minimal size of the gridmap's width and/or height is 3.");
   
  if (GRIDMAP_Y / 2.0 == GRIDMAP_Y / 2 || GRIDMAP_X / 2.0 == GRIDMAP_X / 2)
    Die ("Gridmap's width and/or height must be indivisible.");
   
  if (GRIDMAP_Y > LINES-7 || GRIDMAP_X > COLS-3)
    Die ("The maximum size of the gridmap for this window size is %dx%d.",
	 LINES-7, COLS-3);
	      
#ifdef HAVE_LIBCURSES
  score = newwin (3, COLS, LINES-4, 0);
  prompt = newwin (1, COLS, LINES-1, 0);
  map = newwin (GRIDMAP_Y, GRIDMAP_X, 0, 0);
  description = newwin (GRIDMAP_Y, COLS-(GRIDMAP_X+2), 0, GRIDMAP_X+2);
  text = newpad (TEXTBUFFER, COLS);
   
  prefresh (text, 0, 0, GRIDMAP_Y+1, 0, GRIDMAP_Y+(LINES-GRIDMAP_Y-6), COLS);
	 
  WSetColor (WINDOW_DESCRIPTION, C_GREEN, 0);
  WPrtStr (WINDOW_PROMPT, c_prompt);
      
  //	UpdateMap (NULL);
  //	DisplayMap (0, 0);
   
  UpdateScore();
   
  WUpdate (WINDOW_SCORE);
  WUpdate (WINDOW_MAP);
  WUpdate (WINDOW_DESCRIPTION);
  WUpdate (WINDOW_TEXT); 
  WUpdate (WINDOW_PROMPT);
   
  nodelay (GetWinHandle(WINDOW_PROMPT), TRUE);
  /*   clearok (curscr, FALSE); */
  scrollok (text, TRUE);
  //	wsetscrreg (text, 0, TEXTBUFFER);
   
#else
# ifdef HAVE_LIBGTK
  vbox = gtk_vbox_new (FALSE, 0);
  hbox = gtk_hbox_new (FALSE, 0);
   
  gtk_container_add (GTK_CONTAINER(window), vbox);   
   
  map = gtk_text_new (NULL, NULL);
  description = gtk_text_new (NULL, NULL);
  text = gtk_text_new (NULL, NULL);
  score = gtk_text_new  (NULL, NULL);
  prompt = gtk_text_new (NULL, NULL);
   
  gtk_widget_realize (text);
   
  /* WPrtStr (WINDOW_MAP, "[map]\n");
     WPrtStr (WINDOW_DESCRIPTION, "[description]\n");
     WPrtStr (WINDOW_TEXT, "[text]\n");
     WPrtStr (WINDOW_SCORE, "[score]\n");
     WPrtStr (WINDOW_PROMPT, "[prompt]\n"); */
      
  gtk_widget_set_usize (map, 5, 5);
   
  gtk_box_pack_start (GTK_BOX (hbox), map, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (hbox), description, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (vbox), text, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (vbox), score, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (vbox), prompt, TRUE, TRUE, 0);
               
  gtk_widget_show (vbox);
  gtk_widget_show (hbox);
  gtk_widget_show (map);
  gtk_widget_show (description);
  gtk_widget_show (text);
  gtk_widget_show (score);
  gtk_widget_show (prompt);
   
  gtk_main();
# endif /* HAVE_LIBGTK */
#endif /* HAVE_LIBCURSES */
}

WINDOW *
CreateWindow (int nlines, int ncols, int begin_y, int begin_x)
{
#ifdef HAVE_LIBCURSES
  return newwin (nlines, ncols, begin_y, begin_x);
#endif /* HAVE_LIBCURSES */
}

int
DestroyWindow (byte window)
{
#ifdef HAVE_LIBCURSES
  return delwin (GetWinHandle (window));
#endif /* __CURSES __ */
}

int
WBox (byte window, chtype verch, chtype horch)
{
#ifdef HAVE_LIBCURSES
  return box (GetWinHandle (window), verch, horch);
#endif /* HAVE_LIBCURSES */
}

void
FlushInput ()
{
#ifdef HAVE_LIBCURSES
  flushinp();
#endif /* HAVE_LIBCURSES */
}

void
WUpdate (byte window)
{
  if (!windows)
    return Update();
   
#ifdef HAVE_LIBCURSES
  switch (window)
    {
    case WINDOW_TEXT : 
      prefresh (text, 0+scroll_pos, 0, GRIDMAP_Y+1, 0,
		GRIDMAP_Y+(LINES-GRIDMAP_Y-6), COLS);
      break;
    default :	wrefresh(GetWinHandle(window));
      break;
    }
   
  if (list != NULL)
    {
      touchwin (GetWinHandle (WINDOW_LIST));
      wrefresh (GetWinHandle (WINDOW_LIST));
    }
#endif
}

void
DelLine ()
{
#ifdef HAVE_LIBCURSES
  deleteln ();
#endif /* HAVE_LIBCURSES */
}

void
WDelLine (byte window)
{
  if (!windows)
    return DelLine ();
	
#ifdef HAVE_LIBCURSES
  wdeleteln (GetWinHandle (window));
#endif /* HAVE_LIBCURSES */
}

void
WPrtStr (byte window, char *fmt, ...)
{
  int y, x, i, inverse;
  va_list vl;
  static char buf[8192];
   
  va_start (vl, fmt);
  vsprintf (buf, fmt, vl);
  va_end (vl);
   
  if (!windows)
    return PrtStr (buf);
   
#ifdef HAVE_LIBCURSES
  for (i=0;i<strlen(buf);i++)
    {
      if (i < strlen(buf) - 3)
	{
	  if (buf[i] == '^' && (buf[i+1] == 'c' || buf[i+1] == 'b'))
	    {
	      if (i > 0 && buf[i-1] == '/')
		{
		  i += 3;
		  continue;
		}
				  
	      inverse = FALSE;
				  
	      if (buf[i+1] == 'b')
		inverse = TRUE;
				  
	      switch (buf[i+2])
		{
		case '0':
		  wattrset (GetWinHandle (window), A_NORMAL);
		  break;
		case '1':
		  wattrset (GetWinHandle (window), A_BOLD);
		  break;
		case '4':
		  wattrset (GetWinHandle (window), A_REVERSE);
		  break;
		case 'l':
		  WSetColor (window, C_BLACK, inverse);
		  break;
		case 'r':
		  WSetColor (window, C_RED, inverse);
		  break;
		case 'g':
		  WSetColor (window, C_GREEN, inverse);
		  break;
		case 'y':
		  WSetColor (window, C_BROWN, inverse);
		  break;
		case 'b':
		  WSetColor (window, C_BLUE, inverse);
		  break;
		case 'p':
		  WSetColor (window, C_MAGENTA, inverse);
		  break;
		case 'c':
		  WSetColor (window, C_CYAN, inverse);
		  break;
		case 'w':
		  WSetColor (window, C_LIGHT_GRAY, inverse);
		  break;
		case 'L':
		  WSetColor (window, C_DARK_GRAY, inverse);
		  break;
		case 'R':
		  WSetColor (window, C_LIGHT_RED, inverse);
		  break;
		case 'G':
		  WSetColor (window, C_LIGHT_GREEN, inverse);
		  break;
		case 'Y':
		  WSetColor (window, C_YELLOW, inverse);
		  break;
		case 'B':
		  WSetColor (window, C_LIGHT_BLUE, inverse);
		  break;
		case 'P':
		  WSetColor (window, C_LIGHT_MAGENTA, inverse);
		  break;
		case 'C':
		  WSetColor (window, C_LIGHT_CYAN, inverse);
		  break;
		case 'W':
		  WSetColor (window, C_WHITE, inverse);
		  break;
		}
				  
	      i += 3;
	    }
	}
      waddch(GetWinHandle(window), buf[i]);
    }
   
  WSetColor (window, C_LIGHT_GRAY, 0);
   
  if (window == WINDOW_TEXT && browse_text == FALSE)
    {
      getyx(GetWinHandle(WINDOW_TEXT), y, x);
		   		   
      if (y > scroll_pos + (LINES - (15+(GRIDMAP_Y-9))))
	{
	  scroll_pos = y - (LINES - (15+(GRIDMAP_Y-9)));
	  WUpdate (WINDOW_TEXT);
	}
    }
   
#else
# ifdef HAVE_LIBGTK
  gtk_text_insert (GTK_TEXT (GetWinHandle (window)), NULL, NULL, NULL, buf,
		   -1);
# endif /* HAVE_LIBGTK */
#endif /* HAVE_LIBCURSES */
}

void
UpdateScore (void)
{
  char tmp[80];
   
  if (!windows)
    return;
   
  strcpy (tmp, c_name);
   
  tmp[0] = toupper (tmp[0]);
   
  WCursor (WINDOW_SCORE, 0, 0);
  WPrtStr (WINDOW_SCORE, "%-12s Str: %-2d Dex: %-2d Stu: %-2d Con: %-2d " \
	   "Agi: %-2d Int: %-2d Wis: %-2d Pow: %-2d Cha: %-2d\n Hp: %3d " \
	   "Sp: %3d Ep: %3d Money: %-6d X: %-3d Y: %-3d", tmp,
	   c_stats[A_STR].current,
	   c_stats[A_DEX].current,
	   c_stats[A_STU].current,
	   c_stats[A_CON].current,
	   c_stats[A_AGI].current,
	   c_stats[A_INT].current,
	   c_stats[A_WIS].current,
	   c_stats[A_POW].current,
	   c_stats[A_CHA].current,
	   c_hp, c_sp, c_ep, c_money, c_x, c_y);
         
  WUpdate (WINDOW_SCORE);
}

void
Update (void)
{
#ifdef HAVE_LIBCURSES
  refresh ();
#endif /* HAVE_LIBCURSES */
}

void
Cursor (byte x, byte y)
{
#ifdef HAVE_LIBCURSES	
  move (y, x);
  Update ();
#endif /* HAVE_LIBCURSES */
}

void
WCursor (byte window, byte x, byte y)
{
  if (!windows)
    return;
	
#ifdef HAVE_LIBCURSES
  wmove(GetWinHandle(window), y, x);
#endif /* HAVE_LIBCURSES */
}

void
WDelCh (byte window)
{
  if (!windows)
    return;
	
#ifdef HAVE_LIBCURSES
  wdelch(GetWinHandle(window));
#endif
}

void
WPrtChar (byte window, byte c)
{
  if (!windows)
    return PrtChar (c);
		
#ifdef HAVE_LIBCURSES
  waddch (GetWinHandle (window), (char) c);
#endif /* HAVE_LIBCURSES */
}
	
void
PrtChar (byte c)
{
#ifdef HAVE_LIBCURSES
  addch ((char) c);
#endif /* HAVE_LIBCURSES */
}

char
GetKey (void)
{
#ifdef HAVE_LIBCURSES
  return (char) getch ();
#endif /* HAVE_LIBCURSES */
}

int
WGetKey (byte window)
{
  if (!windows)
    return (int) GetKey();
   
#ifdef HAVE_LIBCURSES
  return (int) wgetch (GetWinHandle (window));
#endif /* HAVE_LIBCURSES */
}	
void
PrtStr (char *fmt, ...)
{
  va_list vl;
  static char buf[1000];
	
  va_start (vl, fmt);
  vsprintf (buf, fmt, vl);
  va_end (vl);
	
#ifdef HAVE_LIBCURSES
  addstr (buf);
#endif /* HAVE_LIBCURSES */
	
  Update ();
}

void
WClear (byte window)
{
  if (!windows)
    return;
	
#ifdef HAVE_LIBCURSES
  wclear(GetWinHandle(window));
#endif /* HAVE_LIBCURSES */
}

#ifdef HAVE_LIBCURSES
static byte ct[MAX_COLOR] =
{
  COLOR_BLACK, COLOR_BLUE, COLOR_GREEN, COLOR_CYAN, COLOR_RED,
  COLOR_MAGENTA, COLOR_YELLOW, COLOR_WHITE, COLOR_BLACK, COLOR_BLUE,
  COLOR_GREEN, COLOR_CYAN, COLOR_RED, COLOR_MAGENTA, COLOR_YELLOW,
  COLOR_WHITE
};
#endif /* HAVE_LIBCURSES */

void
InitColors (void)
{
#ifdef HAVE_LIBCURSES
  byte i;
	
  start_color ();
	
  for (i = 0; i < MAX_COLOR; i++)
    init_pair (i, ct[i], COLOR_BLACK);
#endif /* HAVE_LIBCURSES */
}

void
WSetColor (byte window, byte c, byte inverse)
{
  if (!windows)
    return SetColor (c);
	
#ifdef HAVE_LIBCURSES
  if (c >= 8)
    wattrset (GetWinHandle (window), A_BOLD | (COLOR_PAIR (c)));
  else
    wattrset (GetWinHandle (window), COLOR_PAIR (c));
      
  if (inverse)
    wattron (GetWinHandle (window), A_REVERSE);
#endif /* HAVE_LIBCURSES */
}

void
SetColor (byte c)
{
#ifdef HAVE_LIBCURSES
  if (c >= 8)
    attrset (A_BOLD | (COLOR_PAIR (c)));
  else
    attrset (COLOR_PAIR (c));
#endif /* HAVE_LIBCURSES */
}

void
WScrollUp (byte window, int n)
{
  if ((scroll_pos -= n) < 0)
    scroll_pos = 0;
  WUpdate(window);
}

void
WScrollDown (byte window, int n)
{
  int y, x;
   
#ifdef HAVE_LIBCURSES
  getyx(text, y, x);
	
  if ((scroll_pos += n) > TEXTBUFFER-(LINES-14))
    scroll_pos = TEXTBUFFER-(LINES-14);
  else if (scroll_pos > y)
    scroll_pos = y;
  WUpdate(window);
#endif /* HAVE_LIBCURSES */
}

void MainLoop(int sockfd)
{
#ifdef HAVE_SELECT
  fd_set readfds;
  struct timeval tv;
	
  FD_ZERO(&readfds);
  FD_SET(0, &readfds);
  FD_SET(sockfd, &readfds);
	
  tv.tv_sec = 1; /* One second timeout */
  tv.tv_usec = 0;
	
  select(sockfd + 1, &readfds, NULL, NULL, &tv);
		
  if (FD_ISSET(sockfd, &readfds))
    SocketRead();
  if (FD_ISSET(0, &readfds))
    DoKeyboard(sockfd);
#endif /* HAVE_SELECT */
}
