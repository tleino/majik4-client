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

#ifndef __CONFIG_H__
#define __CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "sysdep.h"
   
#ifdef __DEBUG__
# define DEBUG(x) fprintf (stderr, "DEBUG: " x "\n");
#else
# define DEBUG(x)
#endif

#define PORT server_port
#define ADDR server_addr

#define DATA_DIR "/var/lib/games/majik/"

/* gridmap */

#define GRIDMAX_X 100
#define GRIDMAX_Y 100
#define GRIDMAP_X gridmap_width
#define GRIDMAP_Y gridmap_height

/* window setup */

#define TEXTBUFFER 200

/* character defaults */

#define DEFAULT_SPEED 500000 /* 0.5s */
   
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CONFIG_H__ */
