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
 */

typedef struct linkedlist_tag
{
  char * string;
  struct linkedlist_tag * next;
} linkedlist;

void llfree(linkedlist* a);
linkedlist* lladd(linkedlist*next, const char * b);
linkedlist* llcat(linkedlist*a, linkedlist*b);
linkedlist* llreverse(linkedlist*a);
linkedlist* lldup_r(const linkedlist*a);
linkedlist* lldup(const linkedlist*a);
int llcount (const linkedlist*a);
int llexec (const char * command, const linkedlist*a);
void lldump(const linkedlist*a);
const linkedlist * llmatch(const linkedlist * a, const char * search);

