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

#ifndef __GRIDMAP_H__
#define __GRIDMAP_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
   
/* tile definitions */

#define TILE_PLAYER      '@'
#define TILE_LADDER_UP   '>'
#define TILE_LADDER_DOWN '<'
#define TILE_OPEN_DOOR   '/'
#define TILE_CLOSED_DOOR '+'

#define TILE_CRACK       'c'
#define TILE_BURNT		 'b'
#define TILE_EARTH       '.'
#define TILE_UG_EARTH    'e'
#define TILE_ICE         'i'
#define TILE_GRASS       'g'
#define TILE_L_GRASS     'G'
#define TILE_MARSH       'm'
#define TILE_SAND        's'
#define TILE_SNOW        'n'
#define TILE_FLOOR       'f'
#define TILE_STONE_FLOOR 't'
#define TILE_WOOD_FLOOR  'w'
#define TILE_CAVE_FLOOR  '0'

#define TILE_ACID        'a'
#define TILE_LAVA        'L'
#define TILE_WATER       '~'
#define TILE_D_WATER     'd'
#define TILE_UG_WATER    'u'
#define TILE_D_UG_WATER  'U'

#define TILE_WALL        '#'
#define TILE_EARTH_WALL  'E'
#define TILE_METAL_WALL  'M'
#define TILE_STONE_WALL  'S'
#define TILE_WOOD_WALL   'W'
#define TILE_ROCK        '^'
#define TILE_STALAGMITE  'T'

#define TILE_BUSH        'B'
#define TILE_S_BUSH      '1'
#define TILE_L_BUSH      '2'
#define TILE_CON_TREE    '%'
#define TILE_S_CON_TREE  '3'
#define TILE_L_CON_TREE  '4'
#define TILE_DEC_TREE    'D'
#define TILE_S_DEC_TREE  '5'
#define TILE_L_DEC_TREE  '6'
#define TILE_JUN_TREE    'J'
#define TILE_S_JUN_TREE  '7'
#define TILE_L_JUN_TREE  '8'


#define NUM_TERRAINS     7
#define NUM_OUTDOORS     3
#define NUM_TILES        36
#define SIGHT_RADIUS     (GRIDMAP_Y+1)
#define TWILIGHT_LIMIT   4

#define TOP              0
#define RIGTH            1 
#define BOTTOM           2
#define LEFT             3
 
/* structures */

extern BOOL maplock;
extern int light_level;
   
struct terrain
{
	char terrain;
	float visibility;
};

struct tile
{
	char terrain;
	char tile;
};

struct outdoor
{
   char terrain;
   int outdoorness;
};


/* function prototypes */

int IsVisible(coord, coord, coord, coord, float, BOOL);
void UpdateMap (char *);
void UpdateTile (coord, coord, char);
char TileAt (coord, coord);
void PrtTile (coord, coord, char, BOOL);
void SetTileColor (char);
void DisplayMap (coord, coord);
void UpdateLightMap();
float Range(coord, coord, coord, coord);
void SetLightLevel(int);
void PlaceCursor(coord, coord);
void UpdateMapPiece(int, char *);
   
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __GRIDMAP_H__ */
