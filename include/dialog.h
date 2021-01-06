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

#ifndef __DIALOG_H__
#define __DIALOG_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
   
#define DIALOG_YESNO     0 /* <text>                                    */
#define DIALOG_MSGBOX    1 /* <text>                                    */
#define DIALOG_INPUTBOX  2 /* <text> [init]                             */
#define DIALOG_TEXTBOX   3 /* <text>                                    */
#define DIALOG_MENU      4 /* <text> ({ ({ tag, item }), ... })         */
#define DIALOG_CHECKLIST 5 /* <text> ({ ({ tag, item, status }), ... }) */
#define DIALOG_RADIOLIST 6 /* <text> ({ ({ tag, item, status }), ... }) */
   
struct dialog
{
   int type, scrollpos, width, height;
   char callback[80], title[80], text[80], args[80];
   WINDOW *window;
};
   
typedef struct dialog DIALOG;
   
void APIOpenDialog (DIALOG *);
   
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __DIALOG_H__ */
