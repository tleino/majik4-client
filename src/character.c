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

const static char rcsid[] = "$Id: character.c,v 1.7 1998/12/08 19:02:08 namhas Exp $";

#include "main.h"
#include "character.h"

/* Character properties */

char		c_name[12];
ATTRIBUTE 	c_stats[A_MAX];
uint32		c_money; /* 2^32 should be enough for everyone  */ 
int             c_align_x, c_align_y;
int             c_hp, c_ep, c_sp; /* % */
char		c_status[12][20];
coord           c_x, c_y;
int             c_oid;
int             c_online = 0;

int  c_speed = DEFAULT_SPEED; /* delay for usleep() in keyboard loop */
int  c_freeze = 0;            /* turns frozen                        */
char c_freezestr[80];         /* message to be displayed when frozen */

char		*c_prompt;

/*
  Namhas      Str: XX Dex: XX Con: XX Agi: XX Int: XX Wis: XX Pow: XX Cha: XX
  Hp: XXX% Sp: XXX% Ep: XXX% Money: XXXXXX                            Chaotic
  BLIND DEAF MUTE FAMISHED DEHYDRATED SATIATED ... ... ...
  >mur namhas
  >kick namhas
*/
