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

#include <errno.h>
#include <malloc.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <stdarg.h>

#include "main.h"
#include "gridmap.h"
#include "mcp.h"
#include "socket.h"
#include "object.h"
#include "sysdep.h"
#include "command.h"
#include "list.h"
#include "error.h"
#include "target.h"
#include "sysdep.h"

const static char rcsid[] = "$Id: keyboard.c,v 1.84 1999/01/05 09:30:35 namhas Exp $";

#define KEYBUF_SIZE		4096

char key_buffer[10][KEYBUF_SIZE];
int key_cnt, k_echo = ECHO_NORMAL, k_cline = TRUE, k_continuous = FALSE,
  key_x, key_y, k_no_delay = 0;
extern int grid_w, grid_h;
char k_command[80];
extern WINDOW *list;
char last_command[4096];
int lastc, looking = FALSE, look_tx, look_ty;
BOOL browse_text;

#define MAXBUF 4096

struct hist {
  char *line;
  struct hist *prev;
  struct hist *next;
};

static struct hist *history = NULL;
static struct hist *current = NULL;

static char line[MAXBUF];
static int cur_pos = 0;
static int max_pos = 0;
static int real_pos = 0;
static int max_real = 0;

static void FixLine (void);
static void ClearLine (void);
static void ClearEoline (void);
static void CopyLine (char *);
static void AddHistory (char *);

static void
FixLine (void)
{
  int i, j;
   
  if (max_pos > max_real)
    j = max_real;
  else
    j = max_pos;
   
  for (i=real_pos;i<j;i++)
    WPrtChar (WINDOW_PROMPT, line[i]);
   
  WPrtChar (WINDOW_PROMPT, ' ');
   
  for (i=j+1;i>real_pos;i--)
    WPrtChar (WINDOW_PROMPT, 0x08);
   
  WUpdate (WINDOW_PROMPT);
}

static void
ClearLine (void)
{
  int i;
   
  for (i=0;i<max_pos;i++)
    line[i] = '\0';
   
  WCursor (WINDOW_PROMPT, 0, 0);
  WDelLine (WINDOW_PROMPT);
  WPrtStr (WINDOW_PROMPT, c_prompt);
  WCursor (WINDOW_PROMPT, strlen (c_prompt), 0);

  cur_pos = 0;
  max_pos = 0;
  real_pos = 0;
  max_real = (COLS - strlen (c_prompt) - 2);
   
  WUpdate (WINDOW_PROMPT);
}

static void
ClearEoline (void)
{
  int i;
   
  for (i=cur_pos;i<max_pos;i++)
    line[i] = '\0';
   
  for (i=real_pos;i<max_real;i++)
    WPrtChar (WINDOW_PROMPT, ' ');
  for (i=real_pos; i<max_real;i++)
    WPrtChar (WINDOW_PROMPT, 0x08);
   
  WUpdate (WINDOW_PROMPT);
}

static void
CopyLine (char *copy)
{
  strcpy (line, copy);
  WPrtStr (WINDOW_PROMPT, line);
  cur_pos = max_pos = strlen (line);
   
  WUpdate (WINDOW_PROMPT);
}

static void
AddHistory (char *copy)
{
  struct hist *entry;
   
  entry = (struct hist *) malloc (sizeof (struct hist));
  entry->line = malloc ((unsigned int) strlen (copy)+1);
  strcpy (entry->line, copy);
   
  entry->prev = history;
  entry->next = NULL;
   
  if (history != NULL)
    history->next = entry;
   
  history = entry;
}

void
InitKeyboard()
{
  /* key_buffer = (char *) malloc(KEYBUF_SIZE);
     if (!key_buffer)
     Die("malloc: %s\n", strerror(errno)); */
   
  max_real = (COLS - strlen (c_prompt) - 2);
  real_pos = 0;
  cur_pos = 0;
  max_pos = 0;
   
  memset(key_buffer[key_y], 0, KEYBUF_SIZE);
}

void
DeinitKeyboard()
{
  /* free(key_buffer); */
}

static void
DoCommand(char *kbuf, int kbuf_len, int sockfd)
{	
  SocketWrite (kbuf);
}

/* Majik Line-Editing commands (EMACS-bindings):
 * 
 * ^A            moves to the beginning of the line
 * ^B            moves back a single character
 * ^E            moves to the end of the line 
 * ^F            moves forward a single character
 * ^K            kills from current position to the end of line
 * ^P            moves back through history
 * ^N            moves forward through history
 * ^H and DEL    delete the previous character
 * ^D            deletes the current character
 * ^L/^R         redraw the line
 * ^U            kills the entire line
 * ^W            kills last word
 * LF and or CR  return the entire line
 */

void
DoKeyboard(int sockfd)
{
  char buf[8192];
  int c, i;
  char *new_line;
   
  if (target_x != -1 && target_y != -1)
    APISelectTarget(target_x, target_y, target_command);
   
  c = WGetKey(WINDOW_PROMPT);
   
  if (c == ERR) /* continue the loop immediately */
    return;
   
  if (list != NULL)
    {
      if (toupper(c) >= 'A' || toupper(c) <= 'Z')
	{
	  if (APIMatchSelect (c))
	    return;
	}
		
      if (toupper(c) == 'Z')
	{
	  APICloseWindow();
	  return;
	}
		
      if (c == '+' || c == '-')
	{
	  APIScrollWindow (c == '+' ? 1 : 0);
	  return;
	}
    }
   
  if (c_freeze > 0)
    {
      WPrtStr (WINDOW_TEXT, "You are unable to do anything for %d turns.\n",
	       c_freeze);
      WUpdate (WINDOW_TEXT);		
      return;
    }
  else if (c_freeze == -1)
    {
      WPrtStr (WINDOW_TEXT, "You are unable to do anything.\n");
      WUpdate (WINDOW_TEXT);
      return;
    }
   
  /* pre-pre parse */
   
  switch (c)
    {
    case 0x03: /* CTRL+C */
      Die ("CTRL+C received.");
      break;
    case 0x1b: /* ESC or CIO */
    case 0x9b:
      if (c == 0x1b) /* ESC, read [ */
	c = WGetKey (WINDOW_PROMPT);
		
      if (c == 0x9b || c == 0x5b) /* cursor movement: CIO or ESC[ */
	{
	  c = WGetKey (WINDOW_PROMPT);
			 
	  switch (c)
	    {
	    case 0x46:
	      if (k_cline == FALSE)
		c = '1';
	      else
		c = 005;
	      break;
	    case 0x42:
	      if (k_cline == FALSE)
		c = '2';
	      else
		c = 016;
	      break;
	    case 0x36:
	      c = WGetKey (WINDOW_PROMPT); /* read rest */
	      c = '3';
	      break;
	    case 0x44:
	      if (k_cline == FALSE)
		c = '4';
	      else
		c = 002;
	      break;
	    case 0x45:
	      c = '5';
	      break;
	    case 0x43:
	      if (k_cline == FALSE)
		c = '6';
	      else
		c = 006;
	      break;
	    case 0x48:
	      if (k_cline == FALSE)
		c = '7';
	      else
		c = 001;
	      break;
	    case 0x41:
	      if (k_cline == FALSE)
		c = '8';
	      else
		c = 020;
	      break;
	    case 0x35:
	      c = WGetKey (WINDOW_PROMPT); /* read rest */
	      c = '9';
	      break;
	    case 0x53:
	      c = 004; 
	      break;
	    default:
	      c = 0;
	      break;
	    }
	}
      break;
    default:
      break;
    }
   
  /* command line mode */
   
  if (k_cline == TRUE)
    {
      if (isprint (c))
	{
	  for (i=max_pos;i>cur_pos;i--)
	    line[i] = line[i-1];
			 			 
	  if (real_pos > max_real-1)
	    {				  
	      WCursor (WINDOW_PROMPT, strlen (c_prompt), 0);
				  
	      for(i=cur_pos-real_pos;i<max_real+(cur_pos-real_pos);i++)
		WPrtChar (WINDOW_PROMPT, line[i]);
					   
	      real_pos--;
				  
	      WUpdate (WINDOW_PROMPT);
	    }
				  
	  if (k_echo == ECHO_PASSWORD)
	    WPrtChar (WINDOW_PROMPT, '*');
	  else
	    WPrtChar (WINDOW_PROMPT, c);
			 
	  line[cur_pos] = c;
	  cur_pos++;
	  max_pos++;
			 
	  if (real_pos <= (COLS - strlen (c_prompt)) - 2)
	    real_pos++;
			 
	  if (cur_pos < max_pos)
	    FixLine ();
	  else
	    WUpdate (WINDOW_PROMPT);
			 
	  line[max_pos] = '\0';
	}
      else
	{
	  switch (c)
	    {
	    case 001: /* ^A */
	      if (real_pos > max_real-1)
		{
		  WCursor (WINDOW_PROMPT, strlen (c_prompt), 0);
					   
		  for(i=0;i<max_real;i++)
		    WPrtChar (WINDOW_PROMPT, line[i]);
					   
		  WCursor (WINDOW_PROMPT, strlen (c_prompt), 0);
					   
		  real_pos = 0;
		  cur_pos = 0;
		}
	      else
		{
		  while (cur_pos > 0)
		    {
		      cur_pos -= 1;
		      real_pos -= 1;
		      WPrtChar (WINDOW_PROMPT, 0x08);
		    }
		}
				  
	      WUpdate (WINDOW_PROMPT);
	      break;
	    case 002: /* ^B */
	      if (real_pos > max_real-1)
		{
		  WCursor (WINDOW_PROMPT, strlen (c_prompt), 0);
					   
		  real_pos -= 1;
		  cur_pos -= 1;
					   
		  for(i=cur_pos-real_pos;i<max_real+(cur_pos-real_pos);i++)
		    WPrtChar (WINDOW_PROMPT, line[i]);
		}
	      else if (cur_pos > 0)
		{
		  cur_pos -= 1;
		  real_pos -= 1;
		  WPrtChar (WINDOW_PROMPT, 0x08);
		}
	      WUpdate (WINDOW_PROMPT);
	      break;
	    case 005: /* ^E */
	      if (max_pos > max_real-1)
		{
		  WCursor (WINDOW_PROMPT, strlen (c_prompt), 0);
					   
		  for(i=max_pos-max_real;i<max_real;i++)
		    WPrtChar (WINDOW_PROMPT, line[i]);
					   
		  WCursor (WINDOW_PROMPT, strlen (c_prompt), max_real);
					   
		  real_pos = max_real;
		  cur_pos = max_pos;
		}
	      else
		while (cur_pos < max_pos)
		  {
		    WPrtChar (WINDOW_PROMPT, line[cur_pos]);
		    WUpdate (WINDOW_PROMPT);
		    cur_pos += 1;
		  }
	      break;  
	    case 006: /* ^F */
	      if (cur_pos < max_pos)
		{
		  WPrtChar (WINDOW_PROMPT, line[cur_pos]);
		  WUpdate (WINDOW_PROMPT);
		  cur_pos += 1;
		  real_pos += 1;
		}
	      break;
	    case 013: /* ^K */
	      ClearEoline();
	      max_pos = cur_pos;
	      break;
	    case 020: /* ^P */
	      if (history != NULL)
		{
		  if (current == NULL)
		    {
		      current = history;
		      ClearLine();
		      CopyLine(current->line);
		    }
		  else if (current->prev != NULL)
		    {
		      current = current->prev;
		      ClearLine();
		      CopyLine(current->line);
		    }
		}
	      break;
	    case 016: /* ^N */
	      if (current != NULL)
		{
		  current = current->next;
		  ClearLine();
					   
		  if (current != NULL)
		    CopyLine(current->line);
		  else
		    cur_pos = max_pos = 0;
		}
	      break;
	    case 014: /* ^L */
	    case 022: /* ^R */
	      ClearScreen();
				  
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
	      break;
	    case 0177: /* DEL */
	    case 010: /* ^H */
	      if (real_pos > max_real-1)
		{
		  WDelLine (WINDOW_PROMPT);
		  WCursor (WINDOW_PROMPT, 0, 0);
		  WPrtStr (WINDOW_PROMPT, c_prompt);
					   
		  real_pos--;
		  max_pos--;
		  cur_pos--;
					   
		  for (i=cur_pos;i<max_pos;i++)
		    line[i] = line[i+1];
					  
		  for(i=cur_pos-real_pos;i<max_real+(cur_pos-real_pos);i++)
		    WPrtChar (WINDOW_PROMPT, line[i]);
					   
		  WPrtChar (WINDOW_PROMPT, 0x08);
		  FixLine();
		}
	      else if (cur_pos > 0)
		{
		  cur_pos -= 1;
		  WPrtChar (WINDOW_PROMPT, 0x08);
		  for (i=cur_pos;i<max_pos;i++)
		    line[i] = line[i+1];
		  max_pos -= 1;
		  real_pos -= 1;
					   
		  FixLine();
		}
	      break;
	    case 004: /* ^D */
	      if (cur_pos < max_pos)
		{
		  for(i=cur_pos;i<max_pos;i++)
		    line[i] = line[i+1];
		  max_pos -= 1;
		  FixLine();
		}
	      break;
	    case 025: /* ^U */
	      ClearLine();
	      break;
	    case 027: /* ^W */
	      while((cur_pos > 0) && (line[cur_pos-1] == ' '))
		{
		  cur_pos -= 1;
		  real_pos -= 1;
		  WPrtChar (WINDOW_PROMPT, 0x08);
		}
	      while ((cur_pos > 0) && (line[cur_pos-1] != ' '))
		{
		  cur_pos -= 1;
		  real_pos -= 1;
		  WPrtChar (WINDOW_PROMPT, 0x08);
		}
	      ClearEoline(); 
	      max_pos = cur_pos;
	      break;
	    case '\t':
	      k_cline = k_cline ? FALSE : TRUE;
				  
	      if (k_cline == TRUE)
		ChangePrompt (TRUE, ECHO_NORMAL, TRUE, NULL, "> ");
	      else
		ChangePrompt (FALSE, ECHO_QUIET, TRUE, NULL, NULL);
				  
	      break;
	    case '\n': /* ^J */
	    case '\r': /* ^M */
	      line[max_pos+1] = '\0';
	      new_line = malloc(strlen(line)+1);
	      strcpy (new_line, line);
				  
	      AddHistory (line);
				  
	      if (strlen(k_command) > 0)
		sprintf (buf, "%s %s\n", k_command, line);
	      else
		sprintf (buf, "%s\n", line);
				  
	      if (!k_continuous)
		ChangePrompt (FALSE, ECHO_QUIET, TRUE, NULL, NULL);
				  
	      DoCommand(buf, max_pos + 1, sockfd);
				  
	      cur_pos = 0;
	      max_pos = 0;
	      real_pos = 0;
				  
	      if (k_continuous)
		{
		  WDelLine (WINDOW_PROMPT);
		  WCursor (WINDOW_PROMPT, 0, 0);
		  WPrtStr (WINDOW_PROMPT, c_prompt);
		  WUpdate (WINDOW_PROMPT);
		}
	      else
		ChangePrompt (FALSE, ECHO_QUIET, TRUE, NULL, NULL);
				  
	      break;
	    default:
	      break;
	    }
	}
      return;
    }

  switch (c)
    {
    case 0x03: /* CTRL+C */
      Die ("CTRL+C received.");
      break;
    case 0x1b: /* ESC or CIO */
    case 0x9b:
      if (c == 0x1b) /* ESC, read [ */
	c = WGetKey (WINDOW_PROMPT);
		
      if (c == 0x9b || c == 0x5b) /* cursor movement: CIO or ESC[ */
	{
	  c = WGetKey (WINDOW_PROMPT);
			 
	  if (k_cline == FALSE)
	    {
	      switch (c) /* parse numpad without numlock, also arrow
			  * keys should work after this */
		{
		case 0x46: /* 1/END   */
		  c = '1';
		  break;
		case 0x42: /* 2/DOWN  */
		  c = '2';
		  break;
		case 0x36: /* 3/PGDN  */
		  c = WGetKey (WINDOW_PROMPT); /* read rest */
		  c = '3';
		  break;
		case 0x44: /* 4/LEFT  */
		  c = '4';
		  break;
		case 0x45: /* 5/NOOP  */
		  c = '5';
		  break;
		case 0x43: /* 6/RIGHT */
		  c = '6';
		  break;
		case 0x48: /* 7/HOME  */
		  c = '7';
		  break;
		case 0x41: /* 8/UP    */
		  c = '8';
		  break;
		case 0x35: /* 9/PGUP  */
		  c = WGetKey (WINDOW_PROMPT); /* read rest */
		  c = '9';
		  break;
		}
				  
	      Command (c, c_x, c_y);
	      memset(key_buffer[key_y], 0, KEYBUF_SIZE);
	    }
	  else
	    {
	      switch (c)
		{
		case 'A': /* cursor up */				  
		  if (key_y-1 < 0)
		    break;
					   
		  key_y--;
					   
		  WCursor (WINDOW_PROMPT, 0, 0);
		  WDelLine (WINDOW_PROMPT);
		  WPrtStr (WINDOW_PROMPT, c_prompt);
		  WCursor (WINDOW_PROMPT, strlen (c_prompt), 0);
					   
		  key_x = 0;
		  key_cnt = strlen (key_buffer[key_y]);
		  break;
		case 'B': /* cursor down */				  
		  if (key_y+1 > 9)
		    break;
					   
		  key_y++;
					   
		  WCursor (WINDOW_PROMPT, 0, 0);
		  WDelLine (WINDOW_PROMPT);
		  WPrtStr (WINDOW_PROMPT, c_prompt);
		  WCursor (WINDOW_PROMPT, strlen (c_prompt), 0);
					   
		  key_x = 0;
		  key_cnt = strlen (key_buffer[key_y]);
		  break;
		case 'C': /* cursor right */
		  key_x++;
		  WCursor (WINDOW_PROMPT, strlen(c_prompt) + key_x, 0);
		  WUpdate (WINDOW_PROMPT);
		  break;
		case 'D': /* cursor left */
		  key_x--;
		  WCursor (WINDOW_PROMPT, strlen(c_prompt) + key_x, 0);
		  WUpdate (WINDOW_PROMPT);
		  break;
		}
	    }
	}
      break;
    case '\t':
      k_cline = k_cline ? FALSE : TRUE;
		
      if (k_cline == TRUE)
	ChangePrompt (TRUE, ECHO_NORMAL, TRUE, NULL, "> ");
      else
	ChangePrompt (FALSE, ECHO_QUIET, TRUE, NULL, NULL);
		
      break;
    case 0x12: /* CTRL+R, full refresh */
      ClearScreen();
		
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
      break;
    case 0x08:
    case 0x04:
    case 0x7f:
      if (key_x > 0 && key_cnt > 0)
	{
	  key_cnt--;
	  key_x--;
			 
	  if (key_x < key_cnt)
	    {
	      for (i = key_x; i < (key_cnt+1); i++)
		key_buffer[key_y][i] = key_buffer[key_y][i+1];
	    }
	  else
	    key_buffer[key_y][key_x] = 0;
			 
	  WCursor (WINDOW_PROMPT, strlen(c_prompt) + key_x, 0);
	  WDelCh (WINDOW_PROMPT);
	  WCursor (WINDOW_PROMPT, strlen(c_prompt) + key_x, 0);
	  WUpdate (WINDOW_PROMPT);
	}
      break;
    case '\r':
    case '\n':
      key_buffer[key_y][key_cnt] = '\n';
		
      if (strlen(k_command) > 0)
	sprintf (buf, "%s %s", k_command, key_buffer[key_y]);
      else
	sprintf (buf, "%s", key_buffer[key_y]);
		
      if (!k_continuous)
	ChangePrompt (FALSE, ECHO_QUIET, TRUE, NULL, NULL);

      DoCommand(buf, key_cnt + 1, sockfd);
		
      if (k_continuous)
	{
	  WDelLine (WINDOW_PROMPT);
	  WCursor (WINDOW_PROMPT, 0, 0);
	  WPrtStr (WINDOW_PROMPT, c_prompt);
	  WUpdate (WINDOW_PROMPT);
	}
      else
	ChangePrompt (FALSE, ECHO_QUIET, TRUE, NULL, NULL);
		
      cur_pos = 0;
      max_pos = 0;
      real_pos = 0;
		
      if (key_y > 9)
	{
	  for (i = 0; i < 10; i++)
	    strcpy (key_buffer[i], key_buffer[i+1]);
	}
		
      memset (key_buffer[key_y], 0, KEYBUF_SIZE);
		
      break;
    default:
      if (k_cline == FALSE)
	k_echo = ECHO_QUIET;
		
      if (k_cline == FALSE && strlen(k_command) > 0)
	{
	  k_echo = ECHO_QUIET;
	  SocketWrite (k_command);
			 
	  if (!k_continuous)
	    ChangePrompt (FALSE, ECHO_QUIET, TRUE, NULL, NULL);
			 
	  return;
	}
		
      if (k_cline == TRUE)
	{
	  if (key_x < key_cnt) /* INSERT */
	    {				  
	      for (i = key_x; i < key_cnt; i++)
		key_buffer[key_y][i+1] = key_buffer[key_y][i];
				  
	      key_buffer[key_y][key_x] = c;
	    }
	  else
	    key_buffer[key_y][key_x] = c;
			 
	  key_cnt++;
	  key_x++;
			 
	  if (key_x > key_cnt)
	    key_cnt = key_x;
	}
		
      if (k_echo == ECHO_NORMAL)
	{
	  if (key_x < key_cnt) /* INSERT */
	    {
	      WDelLine (WINDOW_PROMPT);
	      WCursor (WINDOW_PROMPT, 0, 0);
	      WPrtStr (WINDOW_PROMPT, c_prompt);
	      WSetColor (WINDOW_PROMPT, C_WHITE, 0);
	      WPrtStr (WINDOW_PROMPT, key_buffer[key_y]);
	      WCursor (WINDOW_PROMPT, strlen(c_prompt) + key_x, 0);
	      WSetColor (WINDOW_PROMPT, C_LIGHT_GRAY, 0);
	    }
	  else
	    {
	      WSetColor (WINDOW_PROMPT, C_WHITE, 0);
	      WPrtChar (WINDOW_PROMPT, c);
	      WSetColor (WINDOW_PROMPT, C_LIGHT_GRAY, 0);
	    }
	}
		
      else if (k_echo == ECHO_PASSWORD)
	WPrtChar (WINDOW_PROMPT, '*');
		
      WUpdate (WINDOW_PROMPT);
			 
      if (k_cline == FALSE)
	{
	  memset (c_prompt, 0, sizeof (c_prompt));
	  strcpy (c_prompt, "");
			 
	  Command (c, c_x, c_y);			 			 
	  memset(key_buffer[key_y], 0, KEYBUF_SIZE);
	}
      break;
    }
      
  lastc = c;
  return;
}

void
Command (int c, int x, int y)
{
  int found, i, oid;
   
  /* handle aliases */
   
  found = 0;
   
  for(i=0;i<NUM_COMMANDS;i++)
    {
      if(command_buffer[i] == c)
	{		 
	  found = 1;
	  break;
	}
    }
   
  if (i != COMMAND_SAYPROMPT && i != COMMAND_INVENTORY &&
      i != COMMAND_LOOK && i != COMMAND_DUMP &&
      i != COMMAND_SCROLLDOWN && i != COMMAND_SCROLLUP &&
      i != COMMAND_SCROLLTOGGLE && i != COMMAND_HELP && found)
    {
      if (k_no_delay)
	k_no_delay = 0;
      /* else
	 usleep (c_speed); */
    }
   
  switch (i)
    {
    case COMMAND_SAYPROMPT: /* prompt for say */
      ChangePrompt (TRUE, ECHO_NORMAL, TRUE, "say", "say> ");
      break;
    case COMMAND_CHATPROMPT:
      SocketWrite ("chatto\n");
      ChangePrompt (TRUE, ECHO_NORMAL, TRUE, "chat", "chat> ");
      break;
    case COMMAND_DOWN:
      SocketWrite ("down\n");
      break;
    case COMMAND_UP:
      SocketWrite ("up\n");
      break;
    case COMMAND_OPEN:
      SocketWrite ("open\n");
      break;
    case COMMAND_CLOSE:
      SocketWrite ("close\n");
      break;
    case COMMAND_QUIT:
      SocketWrite ("quit\n");
      break;
    case COMMAND_GET:
      SocketWrite ("get\n");
      break;
    case COMMAND_DROP:
      SocketWrite ("drop\n");
      break;
    case COMMAND_INVENTORY:
      SocketWrite ("i\n");
      break;
    case COMMAND_GIVE:
      SocketWrite ("give\n");
      break;
    case COMMAND_HELP:
      SocketWrite ("help\n");
      break;
    case COMMAND_WIELD:
      SocketWrite ("wield\n");
      break;
    case COMMAND_WEAR:
      SocketWrite ("wear\n");
      break;
    case COMMAND_REMOVE:
      SocketWrite ("remove\n");
      break;
    case COMMAND_SKILLS:
      SocketWrite ("skills\n");
      break;
    case COMMAND_THROW:
      SocketWrite ("throw\n");
      break;
    case COMMAND_SHOOT:
      SocketWrite ("shoot\n");
      break;
    case COMMAND_APPLY:
      SocketWrite ("apply\n");
      break;
    case COMMAND_NAME:
      SocketWrite ("name\n");
      break;
    case COMMAND_N_NW:
    case COMMAND_NW:
      MovePlayer(-1,-1);
      break;
    case COMMAND_N_NE:
    case COMMAND_NE:
      MovePlayer(-1,1);
      break;
    case COMMAND_N_SW:
    case COMMAND_SW:
      MovePlayer(1,-1);
      break;
    case COMMAND_N_SE:
    case COMMAND_SE:
      MovePlayer(1,1);
      break;
    case COMMAND_N_N:
    case COMMAND_N:
      MovePlayer(-1,0);
      break;
    case COMMAND_N_W:
    case COMMAND_W:
      MovePlayer(0,-1);
      break;
    case COMMAND_N_E:
    case COMMAND_E:
      MovePlayer(0,1);
      break;
    case COMMAND_N_S:
    case COMMAND_S:
      MovePlayer(1,0);
      break;
    case COMMAND_NOOP:
      MovePlayer(0,0);
      break;
    case COMMAND_LOOK:
      WPrtStr (WINDOW_TEXT, "(^c1s^c0=select target ^c1t^c0=toggle target " \
	       "^c1z^c0=exit)\n");
      WUpdate (WINDOW_TEXT);
      APISelectTarget(c_x, c_y, "vlook");
      break;
    case COMMAND_DUMP:
      DumpObjects();
      break;
    case COMMAND_SCROLLDOWN:
      WScrollDown (WINDOW_TEXT, 2);
      break;
    case COMMAND_SCROLLUP:
      WScrollUp (WINDOW_TEXT, 2);
      break;
    case COMMAND_SCROLLTOGGLE:
      browse_text = browse_text ? FALSE : TRUE;
      break;
    default:
      WPrtStr (WINDOW_TEXT, "There is no such command available.\n");
      WUpdate (WINDOW_TEXT);
      break;
    }   
}

void
ChangePrompt (int cline, int echo, int continuous, char *command, char *fmt,
	      ...)
{
  va_list vl;
  static char buf[8192];
   
  cur_pos = 0;
  max_pos = 0;
  real_pos = 0;
  max_real = (COLS - strlen (c_prompt) - 2);
   
  history = NULL;
   
  WDelLine (WINDOW_PROMPT);
  WCursor (WINDOW_PROMPT, 0, 0);
   
  if (fmt != NULL)
    {
      va_start (vl, fmt);
      vsprintf (buf, fmt, vl);
      va_end (vl);
		
      memset (c_prompt, 0, sizeof (c_prompt));
      strcpy (c_prompt, buf);
      WPrtStr (WINDOW_PROMPT, c_prompt);
    }
  else
    memset (c_prompt, 0, sizeof (c_prompt));
   
  k_cline = cline;
  k_echo = echo;
  k_continuous = continuous;
   
  if (command != NULL)
    strcpy (k_command, command);
  else
    memset (k_command, 0, sizeof (k_command));
   
  WUpdate (WINDOW_PROMPT);
  memset (key_buffer[key_y], 0, sizeof (key_buffer));
  key_cnt = 0;
  key_x = 0;
      
  if (k_cline == TRUE)
    {
      line[0] = '\0';
      cur_pos = 0;
      max_pos = 0;
      current = NULL;
      max_real = (COLS - strlen (c_prompt) - 2);
    }
		
  if (target_x != -1 && target_y != -1)
    DisplayMap (target_x, target_y);  
}

void
LookTarget ()
{
  int c, i;
   
  DisplayMap (look_tx, look_ty);
         
  while (1)
    {
      c = WGetKey (WINDOW_PROMPT);
		
      if (c == ERR)
	return;
				
      switch (c)
	{
	case '7':
	  look_ty--;
	  look_tx--;
	  break;
	case '8':
	  look_ty--;
	  break;
	case '9':
	  look_ty--;
	  look_tx++;
	  break;
	case '4':
	  look_tx--;
	  break;
	case '6':
	  look_tx++;
	  break;
	case '1':
	  look_ty++;
	  look_tx--;
	  break;
	case '2':
	  look_ty++;
	  break;
	case '3':
	  look_ty++;
	  look_tx++;
	  break;
	case 'm':
	case 'M':
	  SocketWrite ("%s", k_command);
	  break;
	}
		
      if (list != NULL && tolower(c) == 'z')
	APICloseWindow();
      else if (tolower(c) == 'z')
	break;
		
      ChangePrompt (FALSE, ECHO_QUIET, TRUE, NULL, NULL);
		
      DisplayMap (look_tx, look_ty);
		
      if ((int)IsVisible(c_x, c_y, look_tx, look_ty, SIGHT_RADIUS, TRUE)
	  >= TWILIGHT_LIMIT)
	{
	  if (QueryObjectId (look_tx, look_ty))
	    SocketWrite ("blook %d %d", look_tx, look_ty);
	  else
	    {
	      switch (TileAt(look_tx, look_ty))
		{
		case TILE_LADDER_UP:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A ladder leading up");
		  break;
		case TILE_LADDER_DOWN:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A ladder leading down");
		  break;
		case TILE_OPEN_DOOR:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A open door");
		  break;
		case TILE_CLOSED_DOOR:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A closed door");
		  break;
		case TILE_CRACK:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A crack");
		  break;
		case TILE_EARTH:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A piece of ground");
		  break;
		case TILE_UG_EARTH:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A piece of ground");
		  break;
		case TILE_ICE:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A piece of icy ground");
		  break;
		case TILE_GRASS:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"Grass");
		  break;
		case TILE_L_GRASS:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"Long grass");
		  break;
		case TILE_MARSH:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"Marsh");
		  break;
		case TILE_SAND:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A piece of sand covered ground");
		  break;
		case TILE_SNOW:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A piece of snow covered ground");
		  break;
		case TILE_FLOOR:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A piece of floor");
		  break;
		case TILE_STONE_FLOOR:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A piece of stone floor");
		  break;
		case TILE_WOOD_FLOOR:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A piece of wooden floor");
		  break;
		case TILE_CAVE_FLOOR:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A piece of cave floor");
		  break;
		case TILE_ACID:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A piece of acid covered ground");
		  break;
		case TILE_WATER:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"Water");
		  break;
		case TILE_D_WATER:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"Deep water");
		  break;
		case TILE_UG_WATER:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"Water");
		  break;
		case TILE_D_UG_WATER:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"Deep water");
		  break;
		case TILE_WALL:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A solid wall");
		  break;
		case TILE_EARTH_WALL:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"Earth wall!?");
		  break;
		case TILE_METAL_WALL:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A metal wall");
		  break;
		case TILE_STONE_WALL:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A stone wall");
		  break;
		case TILE_WOOD_WALL:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A wooden wall");
		  break;
		case TILE_ROCK:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"Solid rock");
		  break;
		case TILE_STALAGMITE:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A stalagmite");
		  break;
		case TILE_BUSH:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A bush");
		  break;
		case TILE_S_BUSH:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A small bush");
		  break;
		case TILE_L_BUSH:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A large bush");
		  break;
		case TILE_CON_TREE:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A conifer tree");
		  break;
		case TILE_S_CON_TREE:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A small conifer tree");
		  break;
		case TILE_L_CON_TREE:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A large conifer tree");
		  break;
		case TILE_DEC_TREE:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A deciduous tree");
		  break;
		case TILE_S_DEC_TREE:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A small deciduous tree");
		  break;
		case TILE_L_DEC_TREE:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A large deciduous tree");
		  break;
		case TILE_JUN_TREE:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A jun(?) tree");
		  break;
		case TILE_S_JUN_TREE:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A small jun(?) tree");
		  break;
		case TILE_L_JUN_TREE:
		  ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
				"A large jun(?) tree");
		  break;
		}
	    }
	}
      else
	{
	  i = (int)IsVisible(c_x, c_y, look_tx, look_ty, SIGHT_RADIUS, TRUE);
	  
	  if (i < TWILIGHT_LIMIT && i > 0)
	    ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
			  "There is too dark to see it properly.");
	  else
	    ChangePrompt (FALSE, ECHO_QUIET, TRUE, "noop",
			  "You can't see that far!");
	}
		
      return;
    }
      
  looking = FALSE;
  ChangePrompt (FALSE, ECHO_QUIET, TRUE, NULL, NULL);
  WPrtStr (WINDOW_TEXT, "You can move again.\n");
  WUpdate (WINDOW_TEXT);
  DisplayMap (c_x, c_y);
   
  return;
}		

int
SelectTarget ()
{
  coord *xy;
  int c, oid;
   
#ifdef __CURSES__
  nodelay (GetWinHandle(WINDOW_PROMPT), FALSE);
#endif /* __CURSES __ */
   
  WPrtStr (WINDOW_TEXT, "Select target with 't' and press 'z' when ready!\n");
  WUpdate (WINDOW_TEXT);
   
  while (c != 'z')
    {
      c = WGetKey (WINDOW_PROMPT);
      if (c == 't')
	{
	  oid = NextObject();
	  xy = QueryObjectCoord (oid);
			 
	  WCursor (WINDOW_MAP, (xy[0] - (c_x - (GRIDMAP_X-1)/2) )
		   , (xy[1] - (c_y - (GRIDMAP_Y-1)/2) ));
	  WUpdate (WINDOW_MAP);
	}
    }
   
#ifdef __CURSES__
  nodelay (GetWinHandle(WINDOW_PROMPT), TRUE);
#endif /* __CURSES__ */   
   
  return oid;
}

int
QueryTarget ()
{
  coord tx = c_x, ty = c_y, *ret = malloc (sizeof(coord) * 2);
  int c;
   
  if (tx > (GRIDMAP_X-1)/2)
    tx = (GRIDMAP_X-1)/2;
   
  if (ty > (GRIDMAP_Y-1)/2)
    ty = (GRIDMAP_Y-1)/2;
   
  WCursor (WINDOW_MAP, tx, ty);
  WUpdate (WINDOW_MAP);
   
  WPrtStr (WINDOW_TEXT, "Move cursor and press 'z' when ready!\n");
  WUpdate (WINDOW_TEXT);

#ifdef __CURSES__
  nodelay (GetWinHandle(WINDOW_PROMPT), FALSE);
#endif /* __CURSES__ */
      
  while (c != 'z')
    {
      c = WGetKey (WINDOW_PROMPT);
		
      switch (c)
	{
	case '7':
	  ty--;
	  tx--;
	  break;
	case '8':
	  ty--;
	  break;
	case '9':
	  ty--;
	  tx++;
	  break;
	case '4':
	  tx--;
	  break;
	case '6':
	  tx++;
	  break;
	case '1':
	  ty++;
	  tx--;
	  break;
	case '2':
	  ty++;
	  break;
	case '3':
	  ty++;
	  tx++;
	  break;
	}
		
      WCursor (WINDOW_MAP, tx, ty);
      WUpdate (WINDOW_MAP);		
    }
   
#ifdef __CURSES__
  nodelay (GetWinHandle(WINDOW_PROMPT), TRUE);
#endif /* __CURSES__ */
   
  ret[0] = abs (c_x - (GRIDMAP_X-1)/2 + tx);
  ret[1] = abs (c_y - (GRIDMAP_Y-1)/2 + ty);
   
  return QueryObjectId (ret[0], ret[1]);
}

void			
MovePlayer (int y, int x)
{			
  /*   if (y < 0)
       y = 0;
   
       if (x < 0)
       x = 0;
   
       if (y >= grid_h -1)
       y = grid_h - 1;
   
       if (x > grid_w - 1)
       x = grid_w - 1;
  */ 
  current_command = CMD_XYPOS;
  SocketWrite ("%d#%d", x, y);
}
