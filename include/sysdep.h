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

#ifndef __SYSDEP_H__
#define __SYSDEP_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
	 
#if defined(HAVE_LIBCURSES)
# include <curses.h>
#elif defined(HAVE_LIBGTK)
# include <gtk/gtk.h>
#else
# error No frontend library specified.
#endif /* HAVE_LIBCURSES */

/* Definitions */

#define WINDOW_SCORE       0
#define WINDOW_MAP         1
#define WINDOW_DESCRIPTION 2
#define WINDOW_TEXT        3
#define WINDOW_PROMPT      4
#define WINDOW_LIST        5
#define WINDOW_MAX         6

/* Type definitions */

typedef signed char byte;
typedef unsigned char ubyte;
typedef short int16;
typedef unsigned short uint16;
typedef long int32;
typedef unsigned long uint32;
typedef byte BOOL;
typedef int32 coord;

#ifndef TRUE
# define TRUE	1
#endif /* TRUE */

#ifndef FALSE
# define FALSE	0
#endif /* FALSE */

/* Global prototypes for sysdep.c */

void InitScreen (void);
void ClearScreen (void);
void EndScreen (void);
void InitWindows (void);
void Update (void);
void InitColors (void);
void WSetColor (byte, byte, byte);
void SetColor (byte);
void Cursor (byte, byte);
void WCursor (byte, byte, byte);
void WUpdate (byte);
void WPrtChar (byte, byte);
void PrtChar (byte);
char GetKey (void);
int WGetKey (byte);
void PrtStr (char *, ...);
void WPrtStr (byte, char *, ...);
void WClear (byte);
void WDelCh (byte);
void DelLine (void);
void WDelLine (byte);
void WScrollUp (byte, int);
void WScrollDown (byte, int);
WINDOW *GetWinHandle (byte);
WINDOW *CreateWindow (int, int, int, int);
int DestroyWindow (byte);
int WBox (byte, chtype, chtype);
void FlushInput (void);

void MainLoop (int);

/* Print output functions */

void UpdateScore (void);

/* Color definitions */
   
enum color_types
{
   C_BLACK, C_BLUE, C_GREEN, C_CYAN, C_RED, C_MAGENTA,
   C_BROWN, C_LIGHT_GRAY, C_DARK_GRAY, C_LIGHT_BLUE,
   C_LIGHT_GREEN, C_LIGHT_CYAN, C_LIGHT_RED, C_LIGHT_MAGENTA,
   C_YELLOW, C_WHITE, MAX_COLOR
};
 
extern int gridmap_width, gridmap_height;
   
#ifdef __cplusplus
}
#endif /* __cplusplus */
		
#endif /* __SYSDEP_H__ */
