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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sysdep.h"
#include "gridmap.h"
#include "character.h"
#include "object.h"
#include "list.h"
#include "config.h"
#include "socket.h"
#include "target.h"

const static char rcsid[] = "$Id: object.c,v 1.19 1998/12/02 13:53:45 namhas Exp $";
struct object obj_list = { 0, 0, 0, 0, 0, 0, 0, 0, "", NULL };

void
AddObject(struct object obj)
{   
  struct object *o;
   
  o = &obj_list;
  while (o->next)
    o = o->next;
   
  o->next = (struct object *)malloc(sizeof(struct object));
  o = o->next;
   
  memcpy(o, &obj, sizeof(struct object));
}

void
RemoveObject(int oid)
{  
  struct object *o, *p;
   
  if (!oid)
    return; /* We can't remove root node */
   
  o = &obj_list;
  while (o)
    {
      if (o->oid == oid)
	break;
		
      p = o;
      o = o->next;
    }
   
  if (!o) /* not found, handle bug */
    return;
   
  p->next = o->next;
  free(o);
   
}

void
MoveObject (int oid, coord x, coord y)
{
  struct object *o;
   
  o = &obj_list;
   
  while (o)
    {
      if (o->oid == oid)
	break;
		
      o = o->next;
    }
   
  if (!o) /* insert bughandler here */
    return;
   
  o->x = x;
  o->y = y;
   
  if (maplock == TRUE)
    return;
   
  if (o->oid != c_oid &&
      (abs(o->x - c_x) < GRIDMAP_X/2+1 || abs(o->y - c_y) < GRIDMAP_Y/2+1))
    {
      if (o->light)
	{
	  UpdateLightMap();
			 
	  if (target_x == -1 && target_y == -1)
	    DisplayMap(c_x, c_y);
	  else
	    DisplayMap(target_x, target_y);
	}
      else 
	{
	  if (target_x == -1 && target_y == -1)
	    DisplayMap(c_x, c_y);
	  else
	    DisplayMap(target_x, target_y);
	}
    }
}

void
UpdateObject (struct object obj)
{
  struct object *o;
   
  o = &obj_list;
   
  while (o)
    {
      if (o->oid == obj.oid)
	break;
		
      o = o->next;
    }
  
  if (!o) /* bug, again */
    return;
   
  if (obj.x != -1)
    o->x = obj.x;
   
  if (obj.y != -1)
    o->y = obj.y;
   
  if (obj.character != -1)
    o->character = obj.character;
   
  if (obj.color != -1)
    o->color = obj.color;
   
  if (obj.light != -1)
    o->light = obj.light;
   
  if (obj.physical != -1)
    o->physical = obj.physical;
   
  if (obj.invis != -1)
    o->invis = obj.invis;
   
  strcpy(o->name, obj.name);

  if (maplock == TRUE)
    return;
   
  if ((abs(o->x - c_x) < GRIDMAP_X/2+1 || abs(o->y - c_y) < GRIDMAP_Y/2+1))
    {
      if (o->light)
	{
	  UpdateLightMap();
			 
	  if (target_x == -1 && target_y == -1)
	    DisplayMap(c_x, c_y);
	  else
	    DisplayMap(target_x, target_y);
	}
      else
	{
	  if (target_x == -1 && target_y == -1)
	    DisplayMap(c_x, c_y);
	  else
	    DisplayMap(target_x, target_y);
	}
    }
}

void
DeleteObjects (void)
{
  struct object *o;
  int oid;
   
  o = &obj_list;
   
  while (o)
    {		
      oid = o->oid;
      o = o->next;
		
      if (oid != 0)
	RemoveObject(oid);
    }
   
}

void
QueryObjectName (int oid)
{
  struct object *o;
   
  o = &obj_list;
      
  while (o)
    {
      if (oid == o->oid && strlen(o->name) > 3)
	SocketWrite ("blook %d %d", o->x, o->y);
		
      o = o->next;
    }   
}

void
DumpObjects (void)
{
  struct object *o;
  char buf[8192];
   
  strcpy (buf, "");
  o = &obj_list;
   
  while (o)
    {	
      sprintf(buf,  "%s%12s #%4d '%c': (%2d, %2d) %2d %s;",
	      buf, o->name,
	      o->oid,
	      (char)(o->character == 0 ? ' ' : o->character),
	      o->x,
	      o->y,
	      o->light,
	      o->invis ? "[I]" : ""
	      );
		
      o = o->next;
    }
   
  APIOpenWindow (0, 0, 0, 0, LIST_TEXT, "client: object list", "", buf);
}

int 
NextObject()
{
  static struct object *o = &obj_list;
  int oid;
   
  if (!o->oid)
    o = o->next;

  oid = o->oid;
   
  if (o->next)
    o = o->next;
  else
    o = &obj_list;
   
   
  return oid;
}

coord *
QueryObjectCoord(int oid)
{
  struct object *o;
  coord *ret = malloc (sizeof(coord) * 2);
   
  o = &obj_list;

  while (o)
    {
      if (o->oid == oid)
	{
	  ret[0] = o->x;
	  ret[1] = o->y;
	}
		
      o = o->next;
    }

  return ret;

}

int
QueryObjectId (coord x, coord y)
{
  struct object *o;
   
  o = &obj_list;
   
  while (o)
    {
      if (o->x == x && o->y == y)
	{
	  return o->oid;
	}
		
      o = o->next;
    }
   
  return 0;
}
