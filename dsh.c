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
 * Main program. 
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>
#include "dsh.h"
#include "linkedlist.h"
#include "parameter.h"

char * remoteshell_command="rsh";
int verbose_flag=0;		/* verbosity flag */
int wait_shell=1;		/* waiting for shell to execute (concurrence) */
int show_machine_names=0;	/* show machine names */
int num_topology=1;		/* number of topology to use as a block to execute rsh. 
				 1 = for-loop
				 2 = binary-tree
				 4 = quad-tree.
				*/
linkedlist* remoteshell_command_opt_r=NULL; /* reverse-ordered list of rsh options. */

/*
  A process that just echoes back
 */
static int
do_echoing_back(int fd_in, int fd_out, const char * prompt)
{
  char * buf = NULL;
  int bufsize = 0;
  FILE*f = fdopen (fd_in, "r");
  FILE*standard_output = fdopen (fd_out, "w");
  
  if (!f || !standard_output)
    {
      fprintf(stderr, "%s: Could not open descriptor [%i] or [%i]\n", PROGRAM_NAME, fd_in, fd_out);
      return -1; 
    }
  
  while (0 <  getline(&buf, &bufsize, f))
    {
      fprintf (standard_output, "%s: %s", prompt, buf);
    } 
  return 0;  
}

/** 
 * Actually execute the program, with maybe a pipe.
 */
static int
do_execute_with_optional_pipe (const char * remoteshell_command,
			       const linkedlist * commandline,
			       int pipe_option,
			       const char * machinename)
{
  if (pipe_option)
    {
      int capture_stdout[2];	/* the pipe fd's to use to capturing std-ins */
      int capture_stderr[2];
      int childid;      
            
      if ((-1 == pipe (capture_stdout)) || (-1 == pipe (capture_stderr)))
	{
	  fprintf(stderr, PROGRAM_NAME ": cannot create pipe\n");
	  return -1;
	}
      
      if (0 != (childid = fork()))
	{			/* do some stdin processing */
	  close (capture_stdout[1]);
	  close (capture_stderr[0]);
	  close (capture_stderr[1]);	  
	  if (do_echoing_back(capture_stdout[0], 1, machinename))
	    exit (EXIT_FAILURE);
	  exit(EXIT_SUCCESS);	/* actually, it should just get the
				   child process return code. */
	}
      else if (childid == -1)
	{			/* do some error processing */
	  fprintf (stderr, "%s: Cannot spawn process\n", PROGRAM_NAME);
	  exit (EXIT_FAILURE);	  
	}      
      else if (0 != (childid = fork ()))
	{			/* do some stderr processing */
	  
	  close (capture_stdout[0]);
	  close (capture_stdout[1]);
	  close (capture_stderr[1]);	  
	  if (do_echoing_back(capture_stderr[0], 2, machinename))
	    exit (EXIT_FAILURE);
	  exit (EXIT_SUCCESS);
	}
      else if (childid == -1)
	{			/* do some error processing */
	  fprintf (stderr, "%s: Cannot spawn process\n", PROGRAM_NAME);
	  exit (EXIT_FAILURE);	  
	}
      else
	{			/* execute the child with redirects */
	  if ((-1 == dup2 (capture_stdout[1], 1))||
	      (-1 == dup2 (capture_stderr[1], 2)))
	    {
	      fprintf(stderr, PROGRAM_NAME ": Failed playing with pipe\n");
	      exit (EXIT_FAILURE);
	    }
	  close (capture_stdout[0]);
	  close (capture_stdout[1]);
	  close (capture_stderr[0]);
	  close (capture_stderr[1]);
	  llexec (remoteshell_command, commandline);
	  fprintf(stderr, PROGRAM_NAME ": Failed executing %s with llexec call\n", remoteshell_command);
	  exit (EXIT_FAILURE);
	}
      
    }
  else
    return llexec ( remoteshell_command, commandline );
}

					   

/*
 * spawns rsh session on single machine
 */
static void
execute_rsh_single (const char * remoteshell_command, 
		    const linkedlist * remoteshell_command_opt_r, 
		    const char * param_machinename,
		    const linkedlist * rshcommandline_r,
		    int pipe_option)
{  
  int childpid;
  int childstatus;
   
  if (0==(childpid=fork()))
    {				/* child process */
      linkedlist * tmp = NULL;
      char * machinename = strdup (param_machinename); /* we will always exit() from here, so no need to free this myself. */
      char * username = machinename;
      linkedlist * local_remoteshell_command_opt_r = lldup(remoteshell_command_opt_r);      
      
				/* process to handle username@hostname */
      if (NULL != (machinename = strchr(machinename,'@')))
	{			/* username was specified */
	  *(machinename++) = 0 ; /* cut the string at @ and point to following
				  string (machine name) */
	  local_remoteshell_command_opt_r = lladd (local_remoteshell_command_opt_r, "-l");
	  local_remoteshell_command_opt_r = lladd (local_remoteshell_command_opt_r, username);
	}
      else
	{			/* no username was specified */
	  machinename=username;	      
	  username=NULL;
	}
      
      tmp = llcat (tmp, local_remoteshell_command_opt_r);
      tmp = lladd (tmp, machinename);
      tmp = llcat (tmp, lldup(rshcommandline_r));
      tmp = llreverse(tmp);
      
      if (verbose_flag) 
	{
	  printf( "DUMPing parameters passed to llexec\n");
	  lldump(tmp);
	}	  
      
      if (-1 == (do_execute_with_optional_pipe(remoteshell_command, 
					       tmp, pipe_option, 
					       param_machinename)))
	{
	  fprintf(stderr, PROGRAM_NAME 
		  ": Failed to execute remote shell command %s\n", 
		  remoteshell_command);
	  exit (EXIT_FAILURE);
	}
				/* what follows should never occur */
      fprintf (stderr, PROGRAM_NAME 
	       ": Unexpected error occurred, do_execute_with_optional_pipe failed, and returned an error code that is not -1\n");
      exit (EXIT_FAILURE);
    }
  else
    {
      if (wait_shell)
	  waitpid(childpid, &childstatus, 0);	/* wait for termination */
    }
}


				/* spawns dsh session on other machines */
static void
execute_rsh_multiple (const char * remoteshell_command, 
		      const linkedlist * remoteshell_command_opt_r, 
		      const linkedlist * machinelist, 
		      int nummachines, 
		      const linkedlist * rshcommandline_r)
{
  linkedlist* extraparam = NULL;
  linkedlist* tmp;  
  linkedlist* tmp_start ;  
  char * numstring;

  extraparam = lladd(extraparam, PROGRAM_NAME);

  tmp = tmp_start = lldup(machinelist);  
  while (tmp && (nummachines -- > 0))
    {
      extraparam=lladd (extraparam, "-m");
      extraparam=lladd (extraparam, tmp->string);
      tmp=tmp->next;
    }
  llfree (tmp_start);  
  tmp = tmp_start = llreverse(lldup(remoteshell_command_opt_r));
  while (tmp)
    {
      extraparam=lladd (extraparam, "-o");
      extraparam=lladd (extraparam, tmp->string);
      tmp=tmp->next;
    }
  llfree(tmp_start);  
				/* hand over any extra options here. */
  if (verbose_flag)
    extraparam=lladd (extraparam, "-v");
  if (wait_shell)
    extraparam=lladd (extraparam, "-w");
  else
    extraparam=lladd (extraparam, "-c");
  if (show_machine_names)
    extraparam=lladd (extraparam, "-M");

  extraparam=lladd (extraparam, "-n");
  asprintf(&numstring, "%i", num_topology);  
  extraparam=lladd (extraparam, numstring);
  free(numstring);  

				/* end of extra options. */
  extraparam=lladd (extraparam, "--");
  rshcommandline_r = llcat (extraparam, lldup(rshcommandline_r));

  execute_rsh_single (remoteshell_command, remoteshell_command_opt_r, machinelist->string, rshcommandline_r, 0);
}

/**
   execute remote shell 
   executes dsh on other machines, or
   executes rsh to execute command.
   depending on the parameter and 
   status.
*/
static void
execute_rsh ( const char * remoteshell_command, 
	      const linkedlist * remoteshell_command_opt_r,
	      const linkedlist * machinelist,
	      int nummachines,
	      const linkedlist * rshcommandline_r)
{				
  if (nummachines == 1)
    execute_rsh_single (remoteshell_command, remoteshell_command_opt_r, machinelist->string, rshcommandline_r, show_machine_names);
  else
    execute_rsh_multiple (remoteshell_command, remoteshell_command_opt_r, machinelist, nummachines, rshcommandline_r);
}


/**
   Called after the command-line parsing.
   do the shell execution without caring
   about what actually would happen
*/
int
do_shell (linkedlist* machinelist, linkedlist*rshcommandline_r)
{	
  int nummachines = llcount(machinelist) / num_topology ;  
  int i;

				/* topology 1 is special. */
  if ( nummachines == 0 || (num_topology == 1))
    {
      nummachines = 1;
    }    
  
  while (machinelist)
    {
      if (wait_shell && verbose_flag) 
	{
	  fprintf (stderr, "--- Executing on %s \n", machinelist->string);
	}
      execute_rsh (remoteshell_command, remoteshell_command_opt_r, machinelist, nummachines, rshcommandline_r);
      for (i=0; (i < nummachines) && machinelist; ++i)
	  machinelist = machinelist -> next;
    }
  if (!wait_shell)
    while(-1 != (waitpid(WAIT_ANY, NULL, 0)));	/* waiting for all. */
  
  if (verbose_flag)
    fprintf(stderr, "--- Terminated running\n");
  
  return 0;  
}

/* open /dev/null as stdin */
static void 
open_devnull(void)
{
  int in = open ("/dev/null", O_RDONLY);
  dup2 (in, 0);
}

int
main(int ac, char ** av)
{
  char *buf=NULL;
  
  load_configfile(DSH_CONF);
  if (asprintf (&buf, "%s/.dsh/dsh.conf", getenv("HOME")) < 0)
    {
      fprintf (stderr, "dsh: asprintf failed\n");
      exit (1);
    }  
  load_configfile(buf);
  free (buf);

  open_devnull();

  return parse_options(ac, av);  
}
