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
#include <socket.h>
#include <netdb.h>

#include "majikrc.h"

const static char rcsid[] = "$Id: majikrc.c,v 1.7 1998/12/08 17:00:37 namhas Exp $";

static void ParseOption(char *option, char *value)
{
  int port;
  struct hostent *h;
   
  if (strcasecmp(option, "server") == 0)
    {
      if (sscanf(value, "%*d.%*d.%*d.%*d") == 4)
	strcpy(&server_addr[0], &value[0]);
      else
	{
	  h = gethostbyname(value);
	  if (h == NULL)
	    Die("Invalid host in majikrc.");
	  sprintf(&server_addr[0], "%d.%d.%d.%d",
		  (ubyte)h->h_addr_list[0][0],
		  (ubyte)h->h_addr_list[0][1],
		  (ubyte)h->h_addr_list[0][2],
		  (ubyte)h->h_addr_list[0][3]);
	}
      return;
    }
  if (strcasecmp(option, "port") == 0)
    {
      sscanf(value, "%d", &port);
      server_port = port;
      return;
    }
  if (strcasecmp(option, "protocol_debug") == 0) /* remove before release */
    {
      if (!strcmp(value, "yes"))
	protocol_debug = 1;
      else
	protocol_debug = 0;
      return;
    }
  if (strcasecmp(option, "gridmap_width") == 0)
    {
      sscanf(value, "%d", &port);
      gridmap_width = port;
      return;
    }
  if (strcasecmp(option, "gridmap_height") == 0)
    {
      sscanf(value, "%d", &port);
      gridmap_height = port;
      return;
    }
}

static void ParseLine(char *strbuf, int line)
{
  char option[32];
  char value[128];
  int i, j;
  int ostart, oend, vstart, vend;
   
  memset(&option, 0, 32);
  memset(&value, 0, 128);
   
  /* We could parse this more easily with sscanf() but I want to be more
   * compatible to accept '    option = value' lines, for example.
   * */
   
  ostart = 0;
  while (strbuf[ostart] == ' ')
    ostart++;
   
  for (i = ostart; i < ostart+32; i++)
    {
      if (strbuf[i] == '=')
	break;
      if (strbuf[i] == '\n')
	return;
    }
  oend = i;
   
  if (i == 32)
    Die("majikrc: parse error at line #%d\n", line);
   
  for (j = i; j < strlen(strbuf); j++)
    if (strbuf[j] == '\n')
      break;
   
  for (i = ostart; i < oend; i++)
    if (strbuf[i] != ' ')
      option[i - ostart] = strbuf[i];
   
  vstart = oend;
  vstart++;
   
  while (strbuf[vstart] == ' ')
    vstart++;
  vend = vstart;
  while (strbuf[vend] != ' ' && strbuf[vend] != '\n')
    vend++;

  memcpy(&value[0], &strbuf[vstart], vend - vstart);
   
  fprintf(stdout, "option: '%s' = '%s'\n", option, value);
  ParseOption(&option[0], &value[0]);
}




/*
 * BOOL LoadMajikRC(char *filename)
 * 
 * Return true on successful load, false otherwise
 * */

BOOL LoadMajikRC(char *filename)
{
  FILE *fd;
  char *strbuf;
  char *homedir;
  char file[80];
  int line;
   
  if (filename[0] == '~' && filename[1] == '/')
    {
      homedir = getenv ("HOME");
      sprintf (file, "%s/%s", homedir, filename);
    }
   	 
  sprintf (file, "%s", filename);
		
  fd = fopen(file, "r");
  if (fd == NULL)
    return FALSE;
   
  strbuf = (char *)malloc((128+32) * sizeof(char));
   
  line = 0;
  while (!feof(fd))
    {
      if (fgets(strbuf, 128+32, fd) == NULL)
	break;
      line++;
      if (strbuf[0] == '#')
	continue; /* Skip comments */
      ParseLine(strbuf, line);
    }
   
  return TRUE;
}



