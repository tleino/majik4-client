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

#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
	 
void InitKeyboard();
void DeinitKeyboard();
void DoKeyboard(int sockfd);
void MovePlayer(int y, int x);
int QueryTarget ();
int SelectTarget ();
void LookTarget ();
void ChangePrompt (int, int, int, char *, char *, ...);
void Command(int, int, int);

#define ECHO_QUIET    0
#define ECHO_NORMAL   1
#define ECHO_PASSWORD 2

extern int k_echo, k_cline, k_no_delay;
extern char k_command[80];

extern BOOL browse_text;
   
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __KEYBOARD_H__ */
