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

#ifdef __GTK__
# include <gtk/gtk.h>
#endif /* __GTK__ */

#include <malloc.h>
#include <stdlib.h>

#include "main.h"
#include "character.h"
#include "majikrc.h"
#include "sighand.h"
#include "sysdep.h"

const static char rcsid[] = "$Id: main.c,v 1.21 1998/12/10 17:52:27 harum Exp $";
extern int LINES, COLS;

int
main (int argc, char *argv[])
{	
  int fd, i;
  char *majikrcfile = (char *)malloc(256);		/* Enough */
  FILE *fp;
  char buf[80];
  
  strcpy(majikrcfile, getenv("HOME"));
  strcat(majikrcfile, "/.majik4rc");
  
  if (LoadMajikRC(majikrcfile) == FALSE)
    if (LoadMajikRC("/etc/majik4rc") == FALSE)
      if (LoadMajikRC("majik4rc") == FALSE)
	{
	  /* Why to make having majikrc required? */
	  Die("Couldn't find ~/.majik4rc, /etc/majik4rc or majik4rc.");
	}
  
  free(majikrcfile); /* We don't need this anymore */
  
  signal (SIGINT, HandleSignal);
  signal (SIGSEGV, HandleSignal);   
  
#ifdef __GTK__
  gtk_init (&argc, &argv);
#endif /* __GTK__ */
  
  /* Some setup which should be performed when connecting w/ client */	
  
  c_prompt = (char *)malloc(8);
  
  InitScreen ();
  ClearScreen ();
  
  fd = SocketConnect (ADDR);
  
#ifdef __GTK__
  InitWindows ();
#endif /* __GTK__ */
  
  InitKeyboard();
  SetDefaultAliases();
  InitTerrains();
  
  fp = fopen("majik.ansi", "r");
   
  if (fp)
    {
      while (1)
	{
	  if (feof(fp))
	    break;
			 
	  fgets (buf, 80, fp);
	  WSetColor (WINDOW_DESCRIPTION, C_MAGENTA, 0);
	  WPrtStr (WINDOW_DESCRIPTION, buf);
	}
      
      fclose (fp);
    }
  else
    WPrtStr (WINDOW_DESCRIPTION, "Error: Could not find file \"majik.ansi\" " \
	     "in the current directory.\n");
  
  WSetColor (WINDOW_PROMPT, C_LIGHT_GRAY, 0);
  WUpdate (WINDOW_DESCRIPTION);
  
  while (1)
    MainLoop(fd); /* Called once in a second */
  
  SocketDisconnect ();
  DeinitKeyboard ();
  EndScreen ();
  return 0;
}
