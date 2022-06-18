/*
 *  DSH / dancer's shell or the distributed shell
 *  Copyright (C) 2001, 2002, 2003 Junichi Uekawa
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
#include <string.h>
#include <unistd.h>
#include "dsh.h"
#include "linkedlist.h"
#include "parameter.h"

#include "gettext.h"
#define _(A) gettext(A)

/**
   free a linked list.
 */
void 
llfree(linkedlist* a)
{				/* add paranoia checks */
  if (!a)
    return;
  llfree(a->next);
  a->next = NULL;
  free (a->string);  
  a->string = NULL;
  free (a);  
}

/** 
 * add a char to linked list
 *
 * @newly added list, or exit(1) on error
 */
linkedlist* 
lladd(linkedlist*next, const char * b)
{
  linkedlist*tmp= malloc_with_error(sizeof(linkedlist));
  if (!(tmp->string = strdup(b)))
    {
      fprintf (stderr, _("Out of memory in lladd\n"));
      exit (1);
    }
  tmp->next=next;  
  return tmp;
}

/**
 * concatenate linked list.  
 * a comes after b.
 *
 * @return concatenated list, never errors.
 */
linkedlist* 
llcat(linkedlist*a, linkedlist*b)
{
  linkedlist* orig=b;
  
  if (NULL==a) return b;
  if (NULL==b) return a;
  while (b->next)
    b=b->next;
  b->next = a;
  return orig;  
}

/**
 * Reverse the order of the linked list.
 *
 * @return the reversed list, never NULL.
 */
linkedlist* 
llreverse(linkedlist*a)
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

/**
 * duplicate the list 
 *
 * @return the new list member, or NULL in failure
 */
linkedlist* 
lldup(const linkedlist*a)
{
  if (a)
    return lladd (lldup(a->next), a->string);
  else 
    return NULL;  
}

/**
 * Count the number of members in the list
 *
 * @return number of members.
 */
int 
llcount (const linkedlist*a)
{				/* count members */
  if (a)
    return llcount(a->next)+1 ;
  else
    return 0;  
}

/**
   execute shell according to linked list

   @returns 1 on failure, does not return on success.
 */
int
llexec (const char * command, const linkedlist * a)
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
  av[0] = strdup(command);  
  execvp (command, av);  
  free (av[0]);  
  free (av);
  return 1;			/* when this function returns,
				   there was an error executing the program */
}

static void 
lldump_recursion(const linkedlist*a)
{
  if (a)
    {
      printf ("[%s] ", a->string);
      lldump_recursion(a->next);
    }
}

/**  
 * Dump contents of the linked list for debug
 */
void 
lldump(const linkedlist*a)
{
  lldump_recursion(a);
  printf ("\n");
}

/**
 * Find matching member from the linked list.
 *
 * @return pointer to matching member item, or NULL
 */
const linkedlist * 
llmatch(const linkedlist * a, const char * search)
{
  for ( ; a; a=a->next)
    {
      if (!strcasecmp (a->string, search))
	return a;
    }
  return NULL;
}
