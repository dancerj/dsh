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

#ifdef __DSH_H__
#error dsh.h included more than once.
#endif
#define __DSH_H__

#include "config.h"

#define DSH_CONF DSHCONFDIR "/dsh.conf"

/** pipe the output, and add machine names */
#define PIPE_OPTION_OUTPUT 1
/** pipe the input, and add input duplication */
#define PIPE_OPTION_INPUT 2

#ifndef HAVE_GETLINE
/* an imcomplete, and wrong implementation of getline */
ssize_t getline (char **LINEPTR, size_t *N, FILE *STREAM);
#endif

#ifndef HAVE_ASPRINTF
int asprintf(char **strp, const char *fmt, ...);
#endif

