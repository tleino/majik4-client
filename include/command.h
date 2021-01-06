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

#ifndef __COMMAND_H__
#define __COMMAND_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
	 
#define COMMAND_SAYPROMPT    0
#define COMMAND_DOWN         1
#define COMMAND_UP           2
#define COMMAND_OPEN         3
#define COMMAND_CLOSE        4
#define COMMAND_QUIT         5
#define COMMAND_NW           6
#define COMMAND_NE           7
#define COMMAND_SW           8
#define COMMAND_SE           9
#define COMMAND_N            10
#define COMMAND_W            11
#define COMMAND_S            12
#define COMMAND_E            13
#define COMMAND_LOOK         14
#define COMMAND_DUMP         15
#define COMMAND_SCROLLDOWN   16
#define COMMAND_SCROLLUP     17
#define COMMAND_SCROLLTOGGLE 18
#define COMMAND_GET          19
#define COMMAND_DROP         20
#define COMMAND_INVENTORY    21
#define COMMAND_GIVE         22
#define COMMAND_HELP         23
#define COMMAND_NOOP         24
#define COMMAND_WIELD		 25
#define COMMAND_WEAR		 26
#define COMMAND_REMOVE		 27
#define COMMAND_CHATPROMPT	 28
#define COMMAND_SKILLS		 29
#define COMMAND_THROW		 30   
#define COMMAND_SHOOT		 31
#define COMMAND_APPLY        32
#define COMMAND_N_NW         33
#define COMMAND_N_NE         34
#define COMMAND_N_SW         35
#define COMMAND_N_SE         36
#define COMMAND_N_N          37
#define COMMAND_N_W          38
#define COMMAND_N_S          39
#define COMMAND_N_E          40
#define COMMAND_NAME         41
   
#define NUM_COMMANDS         42

void SetCommandAlias(int,char);
void SetDefaultAliases();

extern char command_buffer[NUM_COMMANDS];
   
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __COMMAND_H__ */
