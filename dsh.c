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
#include <sys/stat.h>
#include <fcntl.h>
#include <locale.h>

#include "dsh.h"
#include "linkedlist.h"
#include "parameter.h"

#include "gettext.h"
#define _(A) gettext(A)

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

/* function defining "getline" */
#ifndef HAVE_GETLINE
/* an imcomplete, and wrong implementation of getline */
ssize_t getline (char **LINEPTR, size_t *N, FILE *STREAM)
{
  const int GETLINESIZE = 256;
  int fgl;
  
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

#if !defined(WAIT_ANY)
#define WAIT_ANY ((pid_t)-1)
#endif /* WAIT_ANY */

/**
 *  A process that just echoes back, reading fd_in using getline, 
 *  and writing to fd_out
 *
 * @returns -1 on error, 0 on success.
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
      fprintf(stderr, "%s: Could not open descriptor [%i] or [%i]\n", PACKAGE, fd_in, fd_out);
      return -1; 
    }
  
  while (0 <  getline(&buf, &bufsize, f))
    {
      fprintf (standard_output, "%s: %s", prompt, buf);
    } 
  if (buf)
    free (buf);
  
  return 0;  
}

/** 
 * Actually execute the program, with maybe a pipe process being forked.
 * the actual program execution is done by llexec.
 * This part does the optional pipe construction or direct llexec
 * depending on the flags.
 *
 * @returns -1 on failure, exit (1) on exec failure.
 */
static int
do_execute_with_optional_pipe (const char * remoteshell_command,
			       const linkedlist * commandline,
			       int pipe_option , /** 1=pipe output and put
						  pretty flags*/
			       const char * machinename)
{
  if (pipe_option & 1)		/* pipe the outputs */
    {
      int capture_stdout[2];	/* the pipe fd's to use to capturing std-ins */
      int capture_stderr[2];
      int childid;      
            
      if ((-1 == pipe (capture_stdout)) || (-1 == pipe (capture_stderr)))
	{
	  fprintf(stderr, _("%s: cannot create pipe\n"), PACKAGE);
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
	  fprintf (stderr, _("%s: Cannot spawn process\n"), PACKAGE);
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
	  fprintf (stderr, _("%s: Cannot spawn process\n"), PACKAGE);
	  exit (EXIT_FAILURE);	  
	}
      else
	{			/* execute the child with redirects */
	  if ((-1 == dup2 (capture_stdout[1], 1))||
	      (-1 == dup2 (capture_stderr[1], 2)))
	    {
	      fprintf(stderr, PACKAGE ": %s\n", _("Failed playing with pipe"));
	      exit (EXIT_FAILURE);
	    }
	  close (capture_stdout[0]);
	  close (capture_stdout[1]);
	  close (capture_stderr[0]);
	  close (capture_stderr[1]);
	}
    }

  if (pipe_option &2)
    {
      /* NO-OP right now. */
    }

  llexec ( remoteshell_command, commandline );
  fprintf(stderr, _("%s: Failed executing %s with llexec call\n"), PACKAGE, remoteshell_command);
  exit (EXIT_FAILURE);
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
	  fprintf(stderr, PACKAGE 
		  ": Failed to execute remote shell command %s\n", 
		  remoteshell_command);
	  exit (EXIT_FAILURE);
	}
				/* what follows should never occur */
      fprintf (stderr, PACKAGE
	       ": Unexpected error occurred, do_execute_with_optional_pipe failed, and returned an error code that is not -1\n");
      exit (EXIT_FAILURE);
    }
  else
    {
      if (wait_shell)
	  waitpid(childpid, &childstatus, 0);	/* wait for termination */
    }
}


/**
 * spawns dsh session on other machines
 *
 * TODO: quote properly.
 */
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

  extraparam = lladd(extraparam, DSH_COMMANDLINE);

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

  asprintf(&numstring, "-n%i", num_topology);  
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
	      int nummachines /** The number of machines to invoke rsh at the same time */,
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

   @returns 0.
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
	  fprintf (stderr, _("--- Executing on %s \n"), machinelist->string);
	}
      execute_rsh (remoteshell_command, remoteshell_command_opt_r, machinelist, nummachines, rshcommandline_r);
      
      /* skip the machines that program was executed on, and repeat */
      for (i=0; (i < nummachines) && machinelist; ++i)
	  machinelist = machinelist -> next;
    }
  if (!wait_shell)
    while(-1 != (waitpid(WAIT_ANY, NULL, 0)));	/* waiting for all. */
  
  if (verbose_flag)
    fprintf(stderr, _("--- Terminated running\n"));
  
  return 0;  
}

/** open /dev/null as stdin */
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
  
  setlocale (LC_ALL, "");
  bindtextdomain(PACKAGE_NAME, LOCALEDIR);
  textdomain(PACKAGE_NAME);

  
  load_configfile(DSH_CONF);
  if (asprintf (&buf, "%s/.dsh/dsh.conf", getenv("HOME")) < 0)
    {
      fprintf (stderr, _("%s: asprintf failed\n"), PACKAGE);
      exit (1);
    }  
  load_configfile(buf);
  free (buf);

  open_devnull();		/* this should be deprecated when
				   show_machine_names &2 is implemented
				 */

  return parse_options(ac, av);  
}
