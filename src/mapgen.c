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

#include <stdlib.h>

#include "mapgen.h"
#include "sysdep.h"

#define MAX_TERRAINS 32 /* Maximum amount of parameters */

/* Terrain parameter flags */
#define TP_NOTHING   0x00
#define TP_NORANDOM  0x01

/* Parameter order */
#define NW           0
#define NE           1
#define SW           2
#define SE           3


const static char rcsid[] = "$Id: mapgen.c,v 1.4 1998/11/19 19:10:36 harum Exp $";

typedef struct
{
  char t;
  int flags;
  float (*BF)(float, float, float);
  int fparams[2];
   
  float p[4];
} TERRAINPARAM;

unsigned int seed = 0x42C574FA;

float
Random()
{
  seed *= 0x9C72A295;
  return ((seed & 0xFFFF) / 65536.0);
}

int
highest(float *f, int a)
{
  int i, j;
  float max = 0.0;
   
  j = 0;
  for (i = 0; i < a; i++)
    {
      if (f[i] > max)
	{
	  max = f[i];
	  j = i;
	}
    }
  return j;
}

float
BlendLinear(float x, float s, float e)
{
  if (x < s)
    return 0.0;
  if (x >= s && x < e)
    return (x - s) / (e - s);
  return 1.0;	/* x >= e */
}

float
BlendSaw(float x, float s, float w)
{
  if (x < s)
    return 0.0;
  if (x >= s && x < w)
    return (x - s) / (2*s);		// BUG?
  if (x >= w && x < (s + w))
    return 1.0 - (x - s) / (2*s);
  return 1.0; /* x >= (s + 0.5) */
}

float
B(float x, int a)
{
  if (a != 3)
    return x;
  else
    {
      if (x < 0.3)
	return 0;
      if (x >= 0.3 && x < 0.6)
	return (x-0.3)*(1.0/0.3);
      return 1.0;
    }
}

void
InterpolateMap(char *map, int params, TERRAINPARAM *t, int width, int height)
{
  int x, y, i, j, pos;
  float f;
  float dx, ddx, dy;
  float ymult[4][4];
  float *prob = (float *)malloc(width * height * params * sizeof(float));
   
  for (y = 0; y < height; y++)
    {
      dy = (float)y / (height-1);
      ddx = (1.0 / (width - 1));
      dx = 0;
      for (i = 0; i < params; i++)
	{
	  f = t[i].BF(dy, t[i].fparams[0], t[i].fparams[1]);
	  ymult[i][NW] = t[i].p[NW] * (1-f);
	  ymult[i][SW] = t[i].p[SW] * f;
	  ymult[i][NE] = t[i].p[NE] * (1-f);
	  ymult[i][SE] = t[i].p[SE] * f;
	}
		
      for (x = 0; x < width; x++)
	{
	  for (i = 0; i < params; i++)
	    {
	      f = t[i].BF(dx, t[i].fparams[0], t[i].fparams[1]);
	      prob[y*(width*params) + x*params + i] = ((1-f)*ymult[i][NW] + 
						       (1-f)*ymult[i][SW] +
						       (  f)*ymult[i][NE] +
						       (  f)*ymult[i][SE]);
	      if (!(t[i].flags & TP_NORANDOM))
		prob[y*(width*params) + x*params + i] *= Random();
	    }
			 
	  dx += ddx;
	}
    }
  for (y = 0; y < height; y++)
    {
      for (x = 0; x < width; x++)
	{
	  pos = y * width * params + x * params;
	  j = highest(&prob[pos], params);
	  map[y*(width+1)+x] = t[j].t;
	}
      map[y*(width+1)+width] = ';';
    }
}

int
SkipSpaces(char *str, int num)
{
  int pos = 0;
  while (num > 0)
    {
      while (str[pos] != ' ' && str[pos] != 0)
	pos++;
      num--;
      pos++;
    }
  return pos;
}

char
*GenerateMap(char *map, int width, int height, char *options)
{
  /* 'options' string format: '
   * <number of terrains (NOT)> <terrain data x NOT>
   * <terrain data> = <char> <flags> <probabilities> <function def>
   * <function def> = <function type> <function parameters>
   */

  int num_terrains, i, func, pos;
  int nw, ne, sw, se;
  TERRAINPARAM *tparams;
   
  // Parse the initial parameters
  sscanf(options, "%d %d", &num_terrains, &seed);
   
  pos = SkipSpaces(options, 2);
   
  if (num_terrains > MAX_TERRAINS)
    num_terrains = MAX_TERRAINS; // Prevent some bugs
   
  tparams = (TERRAINPARAM *)malloc(num_terrains * sizeof(TERRAINPARAM));
   
  for (i = 0; i < num_terrains; i++)
    {
      if (sscanf(&options[pos], "%c %d %d %d %d %d %d %d %d",
		 &tparams[i].t,
		 &tparams[i].flags, 
		 &nw, &ne, &sw, &se,
		 &func, 
		 &tparams[i].fparams[0], &tparams[i].fparams[1]) != 9)
	WPrtStr(WINDOW_TEXT, "mapgen: parameter parsing bug\n");

      tparams[i].p[0] = nw / 100.0;	   
      tparams[i].p[1] = ne / 100.0;	   
      tparams[i].p[2] = sw / 100.0;	   
      tparams[i].p[3] = se / 100.0;	   
      switch (func)
	{
	case 0: 
	  tparams[i].BF = BlendLinear;	
	  break;
	case 1: 
	  tparams[i].BF = BlendSaw; 	
	  break;
	default: tparams[i].BF = BlendLinear; break;
	}
      pos += SkipSpaces(&options[pos], 9);
    }
   
  InterpolateMap(map, num_terrains, tparams, width, height);
  WUpdate(WINDOW_TEXT);
  free(tparams);
   
  return map;
}
