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

#ifndef __CHARACTER_H__
#define __CHARACTER_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
   
typedef struct
{
	int current,
		maximum;
} ATTRIBUTE;

/* Attributes */

#define A_STR 0
#define A_DEX 1
#define A_STU 2
#define A_CON 3
#define A_AGI 4
#define A_INT 5
#define A_WIS 6
#define A_POW 7
#define A_CHA 8
#define A_MAX 9

/* Character info, located in character.h */

extern ATTRIBUTE c_stats[A_MAX];
extern char 	 c_name[12];
extern uint32 	 c_money;
extern int 	 	 c_align_x, c_align_y;
extern int 		 c_hp, c_ep, c_sp;   /* % */
extern char      c_status[12][20];
extern char *	 c_prompt;
extern coord     c_x, c_y;
extern int       c_oid;
extern int       c_speed;
extern int       c_freeze;
extern int       c_online;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CHARACTER_H__ */
