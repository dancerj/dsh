/*
 *  DSH / dancer's shell or the distributed shell
 *  Copyright (C) 2001, 2002 Junichi Uekawa
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * A library to read dsh config file style data files. Header file.
 */

#ifndef __LIBDSHCONFIG_H__
#define __LIBDSHCONFIG_H__

#include <stdio.h>

typedef struct dshconfig_internal
{
  char * title;
  char * data;
  struct dshconfig_internal * next;
} dshconfig_internal;

typedef struct dshconfig 
{
  dshconfig_internal * config;
} dshconfig ;

dshconfig *
open_dshconfig (FILE* file, char delimiter) ;

void
free_dshconfig(dshconfig* d);

dshconfig_internal *
dshconfig_splitline(const char * original, char delimiter);

const char * 
dshconfig_searchdata (const dshconfig * d, const char * index );

#endif


