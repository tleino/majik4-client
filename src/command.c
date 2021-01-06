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

#include "command.h"

const static char rcsid[] = "$Id: command.c,v 1.15 1999/01/05 09:30:34 namhas Exp $";

char command_buffer[NUM_COMMANDS];

const char *command_list[] = 
{   
  "SAYPROMPT",
  "DOWN",
  "UP",
  "OPEN",
  "CLOSE",
  "QUIT",
  "NORTHWEST",
  "NORTHEAST",
  "SOUTHWEST",
  "SOUTHEAST",
  "NORTH",
  "WEST",
  "SOUTH",
  "EAST",
  "LOOK",
  "DUMP",
  "SCROLLDOWN",
  "SCROLLUP",
  "SCROLLTOGGLE",
  "GET",
  "DROP",
  "INVENTORY",
  "GIVE",
  "HELP",
  "NOOP",
  "WIELD",
  "WEAR",
  "REMOVE",
  "CHATPROMPT",
  "SKILLS",
  "THROW",
  "SHOOT",
  "APPLY",
  "NUMPAD NORTHWEST",
  "NUMPAD NORTHEAST",
  "NUMPAD SOUTHWEST",
  "NUMPAD SOUTHEAST",
  "NUMPAD NORTH",
  "NUMPAD WEST",
  "NUMPAD SOUTH",
  "NUMPAD EAST",
  "NAME",
  "\n",
};

void
SetCommandAlias(int commnum, char alias)
{
  command_buffer[commnum] = alias;
}

void
SetDefaultAliases()
{
  command_buffer[COMMAND_SAYPROMPT] = '!';
  command_buffer[COMMAND_DOWN] = '>';
  command_buffer[COMMAND_UP] = '<';
  command_buffer[COMMAND_OPEN] = 'o';
  command_buffer[COMMAND_CLOSE] = 'c';
  command_buffer[COMMAND_QUIT] = 'q';
  command_buffer[COMMAND_N_NW] = '7';
  command_buffer[COMMAND_N_NE] = '9';
  command_buffer[COMMAND_N_SW] = '1';
  command_buffer[COMMAND_N_SE] = '3';
  command_buffer[COMMAND_N_N] = '8';
  command_buffer[COMMAND_N_W] = '4';
  command_buffer[COMMAND_N_S] = '2';
  command_buffer[COMMAND_N_E] = '6';
  command_buffer[COMMAND_NW] = 'y';
  command_buffer[COMMAND_N] = 'k';
  command_buffer[COMMAND_NE] = 'u';
  command_buffer[COMMAND_W] = 'h';
  command_buffer[COMMAND_E] = 'l';
  command_buffer[COMMAND_SW] = 'b';
  command_buffer[COMMAND_S] = 'j';
  command_buffer[COMMAND_SE] = 'n';
  command_buffer[COMMAND_LOOK] = 'L';
  command_buffer[COMMAND_DUMP] = 'D';
  command_buffer[COMMAND_SCROLLDOWN] = '+';
  command_buffer[COMMAND_SCROLLUP] = '-';
  command_buffer[COMMAND_SCROLLTOGGLE] = '*';
  command_buffer[COMMAND_DROP] = 'd';
  command_buffer[COMMAND_GET] = ',';
  command_buffer[COMMAND_INVENTORY] = 'i';
  command_buffer[COMMAND_GIVE] = 'g';
  command_buffer[COMMAND_HELP] = '?';
  command_buffer[COMMAND_NOOP] = '5';
  command_buffer[COMMAND_WIELD] = 'w';
  command_buffer[COMMAND_WEAR] = 'W';
  command_buffer[COMMAND_REMOVE] = 'r';
  command_buffer[COMMAND_CHATPROMPT] = 'C';
  command_buffer[COMMAND_SKILLS] = 'S';
  command_buffer[COMMAND_THROW] = 't';
  command_buffer[COMMAND_SHOOT] = 's';
  command_buffer[COMMAND_APPLY] = 'a';
  command_buffer[COMMAND_NAME] = 'N';
}
