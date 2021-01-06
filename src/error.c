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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>

#include "sysdep.h"
#include "error.h"

const static char rcsid[] = "$Id: error.c,v 1.6 1998/08/10 22:25:18 namhas Exp $";

void
Die (char *fmt, ...)
{
  va_list vl;
  static char buf[1024];
	
  va_start (vl, fmt);
  vsprintf (buf, fmt, vl);
  va_end (vl);
	
  EndScreen ();
	
  if (errno != 0)
    {
      fprintf (stderr, "** Majik has caught an error number %d:\n %s\n",
	       errno, buf);
    }
  else
    {
      fprintf (stderr, "** Majik has exited with reason:\n %s \n",
	       buf);
    }
	
  exit (1);
}
