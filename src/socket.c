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

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "config.h"
#include "error.h"
#include "socket.h"
#include "sysdep.h"
#include "mcp.h"
#include "character.h"

const static char rcsid[] = "$Id: socket.c,v 1.35 1998/12/14 10:30:50 namhas Exp $";

#define	SOCKET_BUFFER_ISIZE        512  /* Initial buffer size */
#define	SOCKET_BUFFER_INCREMENT   4096  /* Amount of buffer size increase */

int sockfd;
int server_port;
int protocol_debug;
char server_addr[16];

char *cmdbuf = NULL;
int cmdbufpos = 0, cmdbufsize = 0;

struct MCP_COMMAND mcpcmd = { 0, 0, NULL };
int mcppos = 0;

static int 
SearchStr(char *haystack, char needle)
{
  int i;
   
  i = 0;
  while ((haystack[i] != needle) && (haystack[i] != '\0'))
    i++;
  if (haystack[i] == '\0')
    return -1;
  else
    return i;
}

static int
CountChar(char *haystack, char needle, int length)
{
  int i, count = 0;
   
  for (i = 0; i < length; i++)
    if (haystack[i] == needle)
      count++;
  return count;
}
   
int
SocketConnect (char *addr)
{
  struct sockaddr_in servaddr;
   
  if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) < 0)
    Die ("socket: %s", strerror (errno));
   
  memset (&servaddr, 0, sizeof (servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons (PORT);
		   
  if (inet_aton (addr, &servaddr.sin_addr) <= 0)
    Die ("inet_aton: %s", strerror (errno));
		
  if (connect (sockfd, (struct sockaddr *) &servaddr, sizeof (servaddr)) < 0)
    Die ("connect: %s", strerror (errno));

  return sockfd;
}

void
SocketRead (void)
{
  char *buf;
  int bufsize;
  int i, n, s;
  FILE *fp = NULL;
   
  bufsize = SOCKET_BUFFER_ISIZE;
  buf = (char *)malloc(bufsize * sizeof(char));
  memset(buf, 0, bufsize);
   
  n = read(sockfd, buf, SOCKET_BUFFER_ISIZE);
  if (n == 0)
    Die("read: %s", strerror (errno));
   
  if (n == SOCKET_BUFFER_ISIZE)
    {
      /* We have more data waiting */
      do 
	{
	  buf = realloc(buf, bufsize + SOCKET_BUFFER_INCREMENT);
	  memset(&buf[bufsize], 0, SOCKET_BUFFER_INCREMENT);
	  n = read(sockfd, &buf[bufsize], SOCKET_BUFFER_INCREMENT);
	  bufsize += SOCKET_BUFFER_INCREMENT;
	} while (n == SOCKET_BUFFER_INCREMENT || n == 0);
      bufsize -= (SOCKET_BUFFER_INCREMENT - n);
    }
  else
    bufsize = n;
   

  /* Parse multiple commands from one packet */
  if (protocol_debug == 1)
    fp = fopen("protocol.debug", "a");
   
  if (cmdbuf != NULL)
    {
      /* If data exists from previous packet, increase buffer size */
      cmdbufsize += bufsize;
      cmdbuf = (char *)realloc(cmdbuf, cmdbufsize);
    }
   
  for (i = 0; i < bufsize; i++)
    {
      if (cmdbuf == NULL)
	{
	  /* New command */
	  cmdbufsize = bufsize;
	  cmdbuf = (char *)malloc(cmdbufsize * sizeof(char));
	}
      if (buf[i] == '\r')
	{
	  /* End of command */
	  cmdbuf[cmdbufpos++] = '\0';
			 
	  if (sscanf(cmdbuf, "%d:%d:%*s", &mcpcmd.command, &mcpcmd.datalen)
	      != 2)
	    {						       
	      /* Invalid command */
	      if (fp)
		fprintf(fp, "> (invalid): %s\n", cmdbuf);
	      free(cmdbuf);
	      cmdbuf = NULL;
	      cmdbufpos = 0;
	      continue;
	    }

	  /* Calculate the command start address */
	  s = SearchStr(cmdbuf, ':') + 1;
	  s += SearchStr(&cmdbuf[s], ':') + 1;
			 
	  mcpcmd.data = &cmdbuf[s];
	  if (fp && mcpcmd.data != NULL)
	    fprintf(fp, "> %d:%d:%s\n", (int)mcpcmd.command,
		    (int)mcpcmd.datalen, (char *)mcpcmd.data);
			 
	  ParseCommand(&mcpcmd);

	  free(cmdbuf);
	  cmdbuf = NULL;
	  cmdbufpos = 0;
	  cmdbufsize = 0;
	  continue;
	}
      cmdbuf[cmdbufpos++] = buf[i];
    }
		
  if (fp)
    fclose(fp);
  free(buf);
}

void 
SocketWriteRaw (char *data, int len)
{
  FILE *fp;
  char *cmd;
  int dataleft;
   
  cmd = (char *)malloc((len + 20) * sizeof(char));
  sprintf (cmd, "%d:%d:%s\n", current_command, len, data);
   
  if (protocol_debug == 1)
    {
      fp = fopen("protocol.debug", "a");
      if (fp)
	{
	  fprintf(fp, "< %s", cmd);
	  fclose(fp);
	}
    }
   
  dataleft = strlen(cmd);
  while (dataleft > 0)
    {
      if (dataleft > 2048)
	{
	  if (write (sockfd, cmd, 2048) == 0)
	    Die ("write: %s", strerror (errno));
	  dataleft -= 2048;
	}
      else
	{
	  if (write (sockfd, cmd, dataleft) == 0)
	    Die ("write: %s", strerror (errno));
	  dataleft -= dataleft;
	}
    }
   
  free(cmd);
}

void
SocketWrite (char *fmt, ...)
{
  va_list vl;
  char *cmddata;
  int cmdlen;

  /* FIXME: Remove this KLUDGE! */
#ifndef HAVE_VASPRINTF
  cmddata = malloc (strlen(fmt) + 10000 * sizeof(char));
   
  va_start (vl, fmt);
  vsprintf (cmddata, fmt, vl);
  va_end (vl);
#else
  va_start (vl, fmt);
  vasprintf (&cmddata, fmt, vl);
  va_end (vl);
#endif /* HAVE_VASPRINTF */

  cmdlen = strlen(cmddata);
   
  if (current_command == CMD_LOGIN_NAME)
    strcpy (c_name, cmddata);
   
  SocketWriteRaw(cmddata, cmdlen);
   
  current_command = CMD_COMMAND;
   
  free(cmddata);
}

void
SocketDisconnect (void)
{
  DEBUG ("disconnect");
  close (sockfd);
}
