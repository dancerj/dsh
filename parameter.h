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
 */


int load_configfile(const char * dsh_conf);
int parse_options ( int ac, char ** av);
extern char * remoteshell_command;

extern int  verbose_flag;		/* verbosity flag */
extern int wait_shell;		/* waiting for shell to execute (concurrence) */
extern int  show_machine_names;	/* show machine names */
extern int  num_topology;		/* number of topology to use as a block to execute rsh. 
				 1 = for-loop
				 2 = binary-tree
				 4 = quad-tree.
				*/
extern linkedlist* remoteshell_command_opt_r; /* reverse-ordered list of rsh options. */


