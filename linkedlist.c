/*
 *  DSH / dancer's shell or the distributed shell
 *  Copyright (C) 2001 Junichi Uekawa
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
 * Linked list library routines.
 */


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "dsh.h"
#include "linkedlist.h"

static void * malloc_with_error(int size)
{
  void * u = malloc (size);
  if (!u)
    {
      fprintf (stderr, PROGRAM_NAME ": failed to allocate memory of %i bytes\n", size);
      exit(1);      
    }
  return u;  
}

void llfree(linkedlist* a)
{
  if (!a)
    return;
  llfree(a->next);
  free (a->string);  
  free (a);  
}

linkedlist* lladd(linkedlist*next, const char * b)
{
  linkedlist*tmp= malloc_with_error(sizeof(linkedlist));
  tmp->string = malloc_with_error(strlen(b)+1);
  strcpy(tmp->string, b);
  tmp->next=next;  
  return tmp;
}

linkedlist* llcat(linkedlist*a, linkedlist*b)
{
  linkedlist* orig=b;
  
  if (NULL==a) return b;
  if (NULL==b) return a;
  while (b->next)
    b=b->next;
  b->next = a;
  return orig;  
}

linkedlist* llreverse(linkedlist*a)
{
  linkedlist*prev=NULL;
  linkedlist*next;
  
  while (a)
    {
      next = a->next ;
      a->next=prev;
      prev=a;
      a=next;
    }
  return prev;  
}


linkedlist* lldup(linkedlist*a)
{
  if (a)
    return lladd (lldup(a->next), a->string);
  else 
    return NULL;  
}


int llcount (linkedlist*a)
{				/* count members */
  if (a)
    return llcount(a->next)+1 ;
  else
    return 0;  
}

				/* execute shell according to linked list */
int llexec (char * command, linkedlist*a)
{
  char ** av;
  int ac = llcount(a) + 1 ;
  int i=1;


  av = malloc_with_error (sizeof (char * ) * ac + 1);
  while (a)
    {
      av[i++]=a->string;
      a=a->next;
    }
  av[ac] = NULL;
  av[0] = command;
  execvp (command, av);  
  free (av);
  return 1;  
}

static void lldump_recursion(linkedlist*a)
{
  if (a)
    {
      printf ("[%s] ", a->string);
      lldump_recursion(a->next);
    }
}

void lldump(linkedlist*a)
{
  printf ("--- Dumping command-line\n");  
  lldump_recursion(a);
  printf ("\n");
}
