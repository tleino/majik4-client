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
#include <string.h>
#include <stdarg.h>

#include "string.h"

const static char rcsid[] = "$Id: string.c,v 1.1 1998/08/13 09:23:36 namhas Exp $";
static char outbuf[4096];

char *
WrapStr(char *text, int width)
{   
  char wbuf[256];
  char *p;
  char *o;
  int l,c,x;
   
  p = text;
  o = outbuf;
  l = 1;
  x = 0;
   
  while (*p && x < 3072)
    {
      c = 0;
		
      while ((*p) && (*p != ' ') && (c < width) && (c < 256))
	{
	  wbuf[c] = *p;
	  p++; c++;
	}
		
		
      wbuf[c] = 0;
		
      if (*p == ' ') p++;
		
      if (l > width)
	{	 
	  *o++ = '\n';
	  l = 1; x++;
	}
		
      strncpy(o,wbuf,c); o += strlen(wbuf);
		
      *o++ = ' ';
		
      x += (strlen(wbuf) + 1);
      l += (strlen(wbuf) + 1);
    }
   
   
  *o++ = '\n';
  *o++ = 0;
   
  return outbuf;
}

char *String(char *fmt, ...)
{
  static char buf[8192];
  va_list vl;
   
  va_start(vl, fmt);
  vsprintf(buf, fmt, vl);
  va_end(vl);
   
  return buf;
}
