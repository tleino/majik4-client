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

const static char rcsid[] = "$Id: gridmap.c,v 1.51 1998/12/26 13:17:34 namhas Exp $";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "config.h"
#include "sysdep.h"
#include "gridmap.h"
#include "character.h"
#include "object.h"
#include "target.h"

/* List of terrains and their effects to visibility. This table should be 
 * reprogrammable by the server. */

struct outdoor outdoorness[NUM_OUTDOORS] = {
  { TILE_CON_TREE   , 4 },
  { TILE_WOOD_FLOOR , 3 },
  { TILE_CAVE_FLOOR , 1 }
}; 
		
   

struct tile tiles[NUM_TILES] = {
  { TILE_CRACK      , '.' },
  { TILE_UG_EARTH   , '.' }, 
  { TILE_ICE        , '.' },
  { TILE_GRASS      , '.' },
  { TILE_L_GRASS    , '.' },
  { TILE_MARSH      , '.' },       
  { TILE_SAND       , '.' },
  { TILE_SNOW       , '.' },
  { TILE_FLOOR      , '.' },
  { TILE_STONE_FLOOR, '.' },
  { TILE_WOOD_FLOOR , '.' }, 
  { TILE_CAVE_FLOOR , '.' },
  { TILE_BURNT	   , '.' },
	 
  { TILE_ACID       , '~' },
  { TILE_LAVA       , '~' },
  { TILE_D_WATER    , '~' },
  { TILE_UG_WATER   , '~' },
  { TILE_D_UG_WATER , '~' },
	 
  { TILE_EARTH_WALL , '#' },
  { TILE_METAL_WALL , '#' },
  { TILE_STONE_WALL , '#' },
  { TILE_WOOD_WALL  , '#' },

  { TILE_ROCK       , '^' },
  { TILE_STALAGMITE , '^' },
   
  { TILE_BUSH       , '%' },
  { TILE_S_BUSH     , '%' },
  { TILE_L_BUSH     , '%' },
  { TILE_CON_TREE   , '%' },
  { TILE_S_CON_TREE , '%' },
  { TILE_L_CON_TREE , '%' },
  { TILE_DEC_TREE   , '%' },
  { TILE_S_DEC_TREE , '%' },
  { TILE_L_DEC_TREE , '%' },
  { TILE_JUN_TREE   , '%' },
  { TILE_S_JUN_TREE , '%' },
  { TILE_L_JUN_TREE , '%' }

};
			

coord map_x, map_y;
int grid_w, grid_h; /* Gridmap height & width */
static char *gridmap; 
static int *lightmap;
char terrains[256];

BOOL maplock;
int light_level;

#define GRIDMAP(x, y) gridmap[(y) * grid_w + (x)]
#define LIGHTMAP(x, y) lightmap[(y) * grid_w + (x)]

void
InitTerrains()
{
  memset(terrains, 0, sizeof(terrains));
  terrains[TILE_L_CON_TREE] = 75;
  terrains[TILE_L_DEC_TREE] = 75;
  terrains[TILE_L_JUN_TREE] = 75;
  terrains[TILE_S_CON_TREE] = 25;
  terrains[TILE_S_DEC_TREE] = 25;
  terrains[TILE_S_JUN_TREE] = 25;
  terrains[TILE_CON_TREE] = 50;
  terrains[TILE_DEC_TREE] = 50;
  terrains[TILE_JUN_TREE] = 50;
  terrains[TILE_L_BUSH] = 25;
  terrains[TILE_S_BUSH] = 5;
  terrains[TILE_BUSH] = 10;
  terrains[TILE_ROCK] = 95;
  terrains[TILE_STALAGMITE] = 75;
  terrains[TILE_EARTH_WALL] = 100;
  terrains[TILE_METAL_WALL] = 100;
  terrains[TILE_STONE_WALL] = 100;
  terrains[TILE_WOOD_WALL] = 100;
  terrains[TILE_CLOSED_DOOR] = 100;
  terrains[TILE_L_GRASS] = 15;
  terrains[TILE_WALL] = 100;
}

/* BOOL IsVisible()
 *
 * Returns whether gridmap location (p1x, p1y) is visible from (p0x, p0y) or 
 * not. Adom-style algorithm is used, where a point is determined to be 
 * visible if the path between (p0 -> p1) can be described with one horizontal 
 * or vertical and one diagonal segment. This would be a lot easier to do 
 * 'backwards' (p1 -> p0), but we cannot implement some of the required 
 * features when doing so. Thus, a more complex approach is used.
 * Check_light argument is to enable/disable light value checking.
 *
 */
float
IsVisible2(coord p0x, coord p0y, coord p1x, coord p1y, float r,
	   BOOL check_light)
{
  int x, y, k;
  BOOL xc, yc;
  float visibility = 100, modi = 1;
  float radius = r, rad = 0;
   
  if (check_light == TRUE)
    radius = LIGHTMAP(p1x, p1y);  
  else
    modi = 2;

  x = p0x; y = p0y;
  do
    {
      if (xc == TRUE && yc == FALSE)
	rad = 1.0;
      if (xc == FALSE && yc == TRUE)
	rad = 1.0;
      if (xc == TRUE && yc == TRUE)
	rad = 1.41;
      if ((xc == TRUE || yc == TRUE) && (k = terrains[GRIDMAP(x,y)]))
	{
	  visibility *= (100-k)/(100*rad);
	}
      radius -= rad*modi;
      xc = FALSE; yc = FALSE;
      if (p1x > x) {  x++; xc = TRUE; }
      if (p1x < x) {  x--; xc = TRUE; }
      if (p1y > y) {  y++; yc = TRUE; }
      if (p1y < y) {  y--; yc = TRUE; }
		  
    } while (x != p1x || y != p1y);

  return radius*(visibility/100);
}

float
IsVisible1(coord p0x, coord p0y, coord p1x, coord p1y, float r,
	   BOOL check_light)
{
  int xdir, ydir, k, i, j;
  float visibility = 100, dx, dy, modi = 1;
  float radius = r;

  dx = p1x - p0x;
  dy = p1y - p0y;

  if (dx != 0)
    xdir = dx / abs(dx);
  else
    xdir = 0;
  if (dy != 0)
    ydir = dy / abs(dy);
  else
    ydir = 0;
   
  if (check_light == TRUE)
    radius = LIGHTMAP(p1x, p1y);
  else 
    modi = 2;
   
  if (abs(dx) > abs(dy))
    {
      /* HORIZONTAL */
      for (i = 0; i != abs(dx) - abs(dy); i++)
	{
	  visibility *= ((100-terrains[GRIDMAP(p0x+i*xdir, p0y)])/100);
	  radius -= modi;
	}
		
      for (j = 0; j != abs(dy); j++ , i++)
	{
	  if (k = terrains[GRIDMAP(p0x+i*xdir, p0y+j*ydir)])
	    visibility *= ((100-k)/141);
	  radius -= 1.41*modi;
	}
    }
  else
    {
      /* VERTICAL */
      for (i = 0; i != abs(dy) - abs(dx); i++)
	{
	  if (k = terrains[GRIDMAP(p0x, p0y+i*ydir)])
	    visibility *= ((100-k)/100);
	  radius -= modi;
	}
		
      for (j = 0; j != abs(dx); j++, i++)
	{
	  if (k = terrains[GRIDMAP(p0x+j*xdir, p0y+i*ydir)])
	    visibility *= ((100-k)/141);
	  radius -= 1.41*modi;
	}
    }
	
  return (visibility/100)*radius;
}

int
IsVisible(coord p0x, coord p0y, coord p1x, coord p1y, float radius,
	  BOOL check_light)
{
  int a, b;

  a = (int)IsVisible1(p0x-map_x, p0y-map_y, p1x-map_x, p1y-map_y, radius,
		      check_light);
  b = (int)IsVisible2(p0x-map_x, p0y-map_y, p1x-map_x, p1y-map_y, radius,
		      check_light);
  if (a > b)	
    return a;
  return b;
}


void
UpdateMap (char *map)
{
  int x, y;
  if (!map)
    return;
	
  /* Calculate map width & height */	
  grid_w = 0;
  while (map[grid_w] != ';')
    grid_w++;
	
  grid_h = strlen(map)/(grid_w+1);
	
  if (gridmap != NULL)
    free(gridmap);
  gridmap = (char *)malloc(grid_w * grid_h);

  if (lightmap != NULL)
    free(lightmap);
  lightmap = malloc(sizeof(int) * grid_w * grid_h);

  for (y = 0; y < grid_h; y++)
    for (x = 0; x < grid_w; x++)
      GRIDMAP(x, y) = map[y*(grid_w+1)+x];
}

void
UpdateMapPiece (int side, char *piece)
{
  int x, y;
   
  switch (side)
    {
    case TOP:
      for (y = 50; y > 0; y--)
	for (x = 0; x < 51; x++)
	  GRIDMAP(x, y) = GRIDMAP(x, y-1);
      for (x = 0; x < 51; x++)
	GRIDMAP(x, 0) = piece[x];
      break;
	
    case BOTTOM:
      for (y = 0; y < 50; y++)
	for (x = 0; x < 51; x++)
	  GRIDMAP(x, y) = GRIDMAP(x, y+1);
      for (x = 0; x < 51; x++)
	GRIDMAP(x, 50) = piece[x];
      break;

    case RIGTH:
      for (x = 0; x < 50; x++)
	for (y = 0; y < 51; y++)
	  GRIDMAP(x, y) = GRIDMAP(x+1, y);
		
      for (y = 0; y < 51; y++)
	GRIDMAP(50, y) = piece[y];
      break;
	  
    case LEFT:
      for (x = 50; x > 0; x--)
	for (y = 0; y < 51; y++)
	  GRIDMAP(x, y) = GRIDMAP(x-1, y);
      for (y = 0; y < 51; y++)
	GRIDMAP(0, y) = piece[y];
      break;
	  
    default:
      break;
    }
   
}
   
void
UpdateLightMap()
{
  coord x1,y1,x2,y2;
  int i,j,vis;
  struct object *o;
	
  if (maplock == TRUE)
    return;
   
  //   if (lightmap != NULL)
  //		free(lightmap);
  //	lightmap = malloc(sizeof(int)*grid_w * grid_h);
  for (i=0; i < grid_w * grid_h ; i++)
    {
      lightmap[i] = (light_level+1)*(light_level+1);
      for (j=0; j < NUM_OUTDOORS ; j++)
	{
	  if (gridmap[i] == outdoorness[j].terrain)
	    { 
	      lightmap[i] = ((light_level - (5 -
					     outdoorness[j].outdoorness))+1)*
		((light_level - (5 - outdoorness[j].outdoorness))+1);
	      if (lightmap[i] < 0)
		lightmap[i] = 0;
	      break; 
	    }
	}
    }

  o = &obj_list;
	
  while (o)
    {
      if (o->light)
	{
	  x1 = o->x - (o->light+1)*(o->light+1)/2;
	  x2 = o->x + (o->light+1)*(o->light+1)/2+1;
	  y1 = o->y - (o->light+1)*(o->light+1)/2;
	  y2 = o->y + (o->light+1)*(o->light+1)/2+1;
					
	  if (x1-map_x < 0)
	    x1 = map_x;
	  if (x2-map_x > grid_w)
	    x2 = grid_w+map_x;
	  if (y1-map_y < 0)
	    y1 = map_y;
	  if (y2-map_y > grid_h)
	    y2 = grid_h+map_y;
				   
	  for (i = y1;i < y2;i++)
	    for (j = x1;j < x2;j++)
	      {
		vis = (int)(IsVisible(o->x,o->y,j,i,
				      (o->light*6+1)-o->light,0));
		lightmap[(i-map_y) * grid_w + (j-map_x)] = 
		  vis > LIGHTMAP(j-map_x, i-map_y) ?
		  vis : LIGHTMAP(j-map_x, i-map_y);
	      }
				   
	}
      o = o->next;
    }
}

float
Range(coord x0, coord y0, coord x1, coord y1)
{
  float dx, dy;
  float range=0;
	
  dx = abs(x0-x1);
  dy = 1.5*abs(y0-y1);
	
  range = abs(dx-dy);
	
  if (dx < dy)
    range +=1.803*dx;
  else range +=1.803*dy;
  return range;
	
}

	

void
UpdateTile (coord x, coord y, char tile)
{
  if (gridmap == NULL)
    return;
  GRIDMAP (x-map_x, y-map_y) = tile;
  DisplayMap(c_x, c_y);
}

char
TileAt (coord x, coord y)
{
  x -= map_x;
  y -= map_y;
  if (x > grid_w - 1 || y > grid_h - 1)
    return ' ';
  else
    return GRIDMAP (x, y) == (int) NULL ? ' ' : GRIDMAP (x, y);
}

void
PrtTile (coord x, coord y, char tile, BOOL lit)
{
  int k;
  if (!lit)
    WSetColor (WINDOW_MAP, C_DARK_GRAY, 0);
  else
    SetTileColor (tile);
  for (k=0;k<NUM_TILES+1;k++)
    if (tiles[k].terrain == tile)
      {
	WPrtChar (WINDOW_MAP, tiles[k].tile);
	return;
      }
  WPrtChar(WINDOW_MAP, tile);
}	

void
DisplayMap (coord x, coord y)
{
  int i, j, toggle = 0;
  coord ti, tj, tx, ty, kx, ky;
  struct object *o;
    
  if (maplock == TRUE)
    return;
  kx = x;
  ky = y;

  if (!gridmap)
    return;

  WCursor (WINDOW_MAP, 0, 0);
		
  for (i = 0; i < GRIDMAP_Y; i++)
    {
      for (j = 0; j < GRIDMAP_X; j++)
	{
	  ti = x - (GRIDMAP_X-1)/2;
	  tj = y - (GRIDMAP_Y-1)/2;
				
	  if (ti < 0)
	    ti = 0;
				
	  if (tj < 0)
	    tj = 0;
	  if ((int)IsVisible(c_x, c_y, j+ti, i+tj, SIGHT_RADIUS, TRUE) 
	      >= TWILIGHT_LIMIT)
	    PrtTile (j+ti, i+tj, TileAt(j+ti, i+tj), TRUE);
	  else if (IsVisible(c_x, c_y, j+ti, i+tj, SIGHT_RADIUS, TRUE) >= 1)
	    PrtTile (j+ti, i+tj, TileAt(j+ti, i+tj), FALSE);
	  else
	    PrtTile (j+ti, i+tj, ' ', FALSE);
	}
    }
	
  o = &obj_list;
  while (o)
    {	   
      if (o->oid && o->character && o->physical == toggle && o->color)
	{
	  i = (int)IsVisible(c_x, c_y, o->x, o->y, SIGHT_RADIUS, TRUE);
	  if (i >= 1 && abs(o->x  - kx) < (GRIDMAP_X-1)/2+1 
	      && abs(o->y  - ky) < (GRIDMAP_Y-1)/2+1) 
	    {
	      tx = (kx - (GRIDMAP_X-1)/2);
	      ty = (ky - (GRIDMAP_Y-1)/2);
					
	      if (tx < 0)
		tx = 0;
	      if (ty < 0)
		ty = 0;
							
	      if (o->invis == 0)
		{
		  WCursor (WINDOW_MAP, o->x - tx, o->y - ty);
		  if (i >= TWILIGHT_LIMIT)
		    WSetColor (WINDOW_MAP, o->color, 0);
		  else
		    WSetColor (WINDOW_MAP, C_DARK_GRAY, 0);
						 
		  WPrtChar (WINDOW_MAP, o->character);
		}
	    }
	}
      o = o->next;
	   
      if (!o)
	{
	  o = &obj_list;
	  toggle++;
	}
	   
      if (toggle > 1)
	break;
    }
	
  tx = c_x-map_x;
  ty = c_y-map_y;
   
  if (tx > (GRIDMAP_X-1)/2)
    tx = (GRIDMAP_X-1)/2;
  if (ty > (GRIDMAP_Y-1)/2)
    ty = (GRIDMAP_Y-1)/2;
   
  if (x == c_x && y == c_y)
    {
      WCursor (WINDOW_MAP, tx, ty);
      SetTileColor (TILE_PLAYER);
      WPrtChar (WINDOW_MAP, '@');
    }
   	
  WCursor (WINDOW_MAP, tx, ty);
  WUpdate (WINDOW_MAP);
}

void
SetTileColor (char tile)
{
  switch (tile)
    {
    case TILE_CRACK:
    case TILE_BURNT:
      WSetColor (WINDOW_MAP, C_BLACK, 0);
      break;
    case TILE_D_WATER:
    case TILE_D_UG_WATER:
      WSetColor (WINDOW_MAP, C_BLUE, 0);
      break;
    case TILE_LAVA:
    case TILE_MARSH:
    case TILE_METAL_WALL:
    case TILE_BUSH:
    case TILE_S_BUSH:
    case TILE_L_BUSH:
      WSetColor (WINDOW_MAP, C_RED, 0);
      break;
    case TILE_WATER:
    case TILE_UG_WATER:
    case TILE_ICE:
      WSetColor (WINDOW_MAP, C_CYAN, 0);
      break;
    case TILE_GRASS:
    case TILE_JUN_TREE:
    case TILE_S_JUN_TREE:
    case TILE_L_JUN_TREE:
      WSetColor (WINDOW_MAP, C_LIGHT_GREEN, 0);
      break;
    case TILE_SAND:
    case TILE_DEC_TREE:
    case TILE_S_DEC_TREE:
    case TILE_L_DEC_TREE:
    case TILE_WOOD_FLOOR:
      WSetColor (WINDOW_MAP, C_YELLOW, 0);
      break;
    case TILE_PLAYER:
    case TILE_SNOW:
    case TILE_STALAGMITE:
      WSetColor (WINDOW_MAP, C_WHITE, 0);
      break;
    case TILE_CAVE_FLOOR:
    case TILE_STONE_FLOOR:
    case TILE_LADDER_UP:
    case TILE_LADDER_DOWN:
    case TILE_WALL:
    case TILE_STONE_WALL:
    case TILE_ROCK:
      WSetColor (WINDOW_MAP, C_LIGHT_GRAY, 0);
      break;
    case TILE_ACID:
    case TILE_L_GRASS:
    case TILE_CON_TREE:
    case TILE_S_CON_TREE:
    case TILE_L_CON_TREE:
      WSetColor (WINDOW_MAP, C_GREEN, 0);
      break;
    case TILE_CLOSED_DOOR:
    case TILE_OPEN_DOOR:
    case TILE_EARTH:
    case TILE_UG_EARTH:
    case TILE_FLOOR:
    case TILE_EARTH_WALL:
    case TILE_WOOD_WALL:
      WSetColor (WINDOW_MAP, C_BROWN, 0);
      break;
      //      WSetColor (WINDOW_MAP, C_CYAN);
      //      WSetColor (WINDOW_MAP, C_LIGHT_CYAN);
      //      WSetColor (WINDOW_MAP, C_LIGHT_RED);
      //      WSetColor (WINDOW_MAP, C_LIGHT_MAGENTA);
      //      WSetColor (WINDOW_MAP, C_MAGENTA);
		
    }
}

void
SetLightLevel(int i)
{
  light_level = i;
}

void
PlaceCursor(coord x, coord y)
{
  coord tx, ty;
   
  if (target_x != -1 && target_y != -1)
    return; /* not while targetng */
	 
  tx = (c_x - (GRIDMAP_X-1)/2);
  ty = (c_y  - (GRIDMAP_Y-1)/2);
   
  if (tx < 0)
    tx = 0;
  if (ty < 0)
    ty = 0;
   
  WCursor (WINDOW_MAP, x - tx, y - ty);
  WUpdate (WINDOW_MAP);
}
