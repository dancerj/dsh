/*
 *  DSH / dancer's shell or the distributed shell
 *  Copyright (C) 2001-2008 Junichi Uekawa
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
 * compatibility code
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dsh.h"
#include "compat.h"


#ifdef __TEST_COMPAT_C__
// for testing the code.
#undef HAVE_GETLINE
#undef HAVE_ASPRINTF
#endif

/* function defining "getline" */
#ifndef HAVE_GETLINE
/* an imcomplete, and wrong implementation of getline */
ssize_t getline (char **LINEPTR, size_t *N, FILE *STREAM)
{
  const int GETLINESIZE = 256;

  if (!*LINEPTR)
    *LINEPTR= malloc (GETLINESIZE);
  if (*N != GETLINESIZE)
    *LINEPTR = realloc (*LINEPTR, GETLINESIZE);
  if (!*LINEPTR)
    {
      return -1;
    }
  if (!fgets (*LINEPTR, GETLINESIZE - 1, STREAM))
    return -1;

  *N = strlen (*LINEPTR);
  return GETLINESIZE;

}
#endif

/* define asprintf if it doesn't exist */
#ifndef HAVE_ASPRINTF
#include <stdarg.h>
int asprintf(char **strp, const char *fmt, ...)
{
  ssize_t buflen = 50 * strlen(fmt); /* pick a number, any number */
  *strp = malloc(buflen);

  if (*strp)
  {
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(*strp, buflen, fmt, ap);
    va_end(ap);
    return buflen;
  }
  return -1;
}
#endif /* HAVE_ASPRINTF  */

