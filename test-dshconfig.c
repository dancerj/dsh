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
 * tester for libdshconfig.
 */

#include <stdio.h>
#include "libdshconfig.h"

/* parse and dump the input to output */
int main()
{
  dshconfig * t = open_dshconfig(stdin, ':');
  dshconfig_internal * line;
  if (!t)
    return 1;
  
  for (line = t->config; line; line = line -> next)
    {
      printf ("[%s]=[%s]\n", line->title, line->data);
    }  
  return 0;
}

