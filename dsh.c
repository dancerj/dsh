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
#include <assert.h>
#include <errno.h>

#include "dsh.h"
#include "compat.h"

#ifdef HAVE_SIGNAL_H
#include <signal.h>
#endif

#include "linkedlist.h"
#include "parameter.h"

#include "gettext.h"
#define _(A) gettext(A)

char * remoteshell_command="rsh";
int verbose_flag=0;		/* verbosity flag */
int wait_shell=1;		/* waiting for shell to execute (concurrence) */
int pipe_option=0;	        /* show machine names -- piping option. */
int num_topology=1;		/* number of topology to use as a block to execute rsh.
				 1 = for-loop
				 2 = binary-tree
				 4 = quad-tree.
				*/
linkedlist* remoteshell_command_opt_r=NULL; /* reverse-ordered list of rsh options. */
int buffer_size = 1024;		/* buffer size. */
int forklimit = 0; 		/* do not limit number of forks */

/** current fork count; required for forklimit processing */
static int currentforkcount = 0;


/**
 *  A process that just echoes back, reading fd_in using getline, and
 *  writing to fd_out. When this process returns, it expects process
 *  exit.
 *
 * @returns -1 on error, 0 on success.
 */
static int
do_echoing_back_process(int fd_in, int fd_out, const char * prompt)
{
  char*buf = NULL;
  size_t bufsize = 0;
  FILE*f = fdopen (fd_in, "r");
  FILE*standard_output = fdopen (fd_out, "w");
  sigset_t newmask, omask;

  setlinebuf(standard_output);

  /* do not die through the SIGHUP or SIGCHLD signal, yet */
  sigemptyset(&newmask);
  sigaddset(&newmask, SIGHUP);
  sigaddset(&newmask, SIGCHLD);
  sigaddset(&newmask, SIGPIPE);

  if (sigprocmask(SIG_BLOCK, &newmask, &omask)<0)
    {
      perror("sigprocmask");
      return -1;
    }

  if (!f || !standard_output)
    {
      if(f) fclose(f);
      if(standard_output) fclose(standard_output);
      fprintf(stderr, _("%s: Could not open descriptor [%i] or [%i]\n"), PACKAGE, fd_in, fd_out);
      return -1;
    }

  while (0<getline(&buf, &bufsize, f))
    {
      fprintf(standard_output, "%s: %s", prompt, buf);
    }

  fflush(standard_output);	/* make sure I flush everything out */
  assert(feof(f));		/* make sure I am at the end of input file */
  fclose(standard_output);
  fclose(f);

  if (buf)
    free(buf);

  /* unmask signal, probably not really required, but do it
     anyway */
  sigprocmask(SIG_SETMASK, &omask, (sigset_t *) NULL);

  return 0;
}

/**
 * Forks the output pipe routine for fd[x]
 *
 * This is the remote-shell-invoking process, used for execing the
 * remote shell. child process is forked from here, and that child
 * process does the echoing back.
 *
 * @returns -1 on error, 0 on success
 */
static int
fork_and_pipe_echoing_routine (
			       /** File descriptor ID */ int fdid,
			       const char * machinename)
{
  int childid;
  int capture_fd[2];
  if (-1==pipe(capture_fd))
    {
      perror("pipe");
      fprintf(stderr, _("%s: cannot create pipe\n"), PACKAGE);
      fflush(stderr);
      return -1;
    }
  fflush(stderr);
  fflush(stdout);
  if (0!=(childid=fork()))
    {
      /* The parent process */

      int status;
      close (capture_fd[1]);
      if (do_echoing_back_process(capture_fd[0], fdid, machinename))
	exit (EXIT_FAILURE);
      if (-1 == waitpid(WAIT_ANY, &status, 0))
	{
	  assert(0);
	}

      assert(WIFEXITED(status));
      exit(WEXITSTATUS(status));
    }
  else if (childid==-1)
    {			/* do some error processing */
      perror("fork");	/* I want to know the errno */
      fprintf (stderr, _("%s: Cannot spawn process\n"), PACKAGE);
      fflush(stderr);
      return -1;
    }
  else
    {
      /* The child process -- this is the main trunc
	 I don't want dsh to wait for the termination of this process.
       */
      if (-1==dup2(capture_fd[1], fdid))
	{
	  fprintf(stderr, PACKAGE ": %s\n", _("Failed playing with pipe"));
	  return -1;
	}
      close (capture_fd[0]);
      close (capture_fd[1]);
    }
  return 0;
}

/**
 * Actually execute the program, with maybe a pipe process being forked.
 * the actual program execution is done by llexec.
 * This part does the optional pipe construction or direct llexec
 * depending on the flags.
 *
 * @returns -1 on failure
 */
static int
do_execute_with_optional_pipe (const char * remoteshell_command,
			       const linkedlist * commandline,
			       int pipe_option , /** 1=pipe output and put
						  pretty flags*/
			       const char * machinename)
{
  if (pipe_option & PIPE_OPTION_OUTPUT)		/* pipe the outputs */
    {
      if (fork_and_pipe_echoing_routine(1, machinename) ||
	  fork_and_pipe_echoing_routine(2, machinename))
	{
	  fprintf(stderr, _("%s: Failed on constructing a pipe and forking\n"), PACKAGE);
	  return -1;
	}
    }

  llexec(remoteshell_command, commandline);
  fprintf(stderr, _("%s: Failed executing %s with llexec call\n"), PACKAGE, remoteshell_command);
  fflush(stderr);
  exit (EXIT_FAILURE);
}

/**
   The exit code to give after successful termination of the dsh program.
 */
int dsh_exit_code = 0;

/**
   Updating the exit code of the child.

   dsh will return the exit code of a random child process which did
   not return success, if dsh itself was ran successfully.
 */
void
dsh_update_exit_code(int exit_code_of_child /** The exit code of the child process*/)
{
  /* default behavior was to always ignore the child exit code. */
  if ((exit_code_of_child!=EXIT_SUCCESS) &&
      (dsh_exit_code==EXIT_SUCCESS))
    {
      dsh_exit_code=exit_code_of_child;
    }
}

static int * fd_output_array = NULL; /** array of fd to duplicate input to */
static int fd_output_array_len = 0;

/**
 * adds an fd to the list of fds to be processed by the input-duplication daemon.
 */
static void
add_fd_to_output_array(int fd)
{
  fd_output_array_len++;
  fd_output_array=realloc(fd_output_array, fd_output_array_len*sizeof(int));
  fd_output_array[fd_output_array_len-1]=fd;
}

/**
 * spawns rsh/ssh session on single machine
 * I am the main process still, and I fork the child processes.
 *
 * @returns -1 on failure, 0 on success
 */
static int
execute_rsh_single (const char * remoteshell_command,
		    const linkedlist * remoteshell_command_opt_r,
		    const char * param_machinename,
		    const linkedlist * rshcommandline_r,
		    int pipe_option /** The pipe option */)
{
  int childpid;
  int childstatus = 0;
  int input_pipe [2];

  if (pipe_option & PIPE_OPTION_INPUT)
    {
      pipe(input_pipe);
    }

  fflush(stderr);
  fflush(stdout);
  /* Execute the rsh process */
  if (0==(childpid=fork()))
    {				/* child process */
      /* Must NOT fork again from this point until I say otherwise */
      linkedlist * tmp = NULL;
      char * machinename =
	strdup (param_machinename); /* we will always exit() from here, so
				       no need to free this myself. */
      char * username = machinename;
      linkedlist * local_remoteshell_command_opt_r = lldup(remoteshell_command_opt_r);

      /* input piping */
      if (pipe_option & PIPE_OPTION_INPUT)
	{
	  if (-1==dup2 (input_pipe[0],0)) perror("dup2-ers");
	  close (input_pipe[1]);
	  close (input_pipe[0]);
	  /* Must NOT fork again before this point. */
	}
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

      if (verbose_flag) 	/* debugging */
	{
	  printf(_( "DUMPing parameters passed to llexec\n"));
	  lldump(tmp);
	}

      if (-1 == (do_execute_with_optional_pipe(remoteshell_command,
					       tmp, pipe_option,
					       param_machinename)))
	{
	  fprintf(stderr,
		  _("%s: Failed to execute remote shell command %s\n"),
		  PACKAGE, remoteshell_command);
	  exit (EXIT_FAILURE);
	}
				/* what follows should never occur */
      fprintf (stderr,
	       _("%s: Unexpected error occurred, do_execute_with_optional_pipe failed, and returned an error code that is not -1\n"), PACKAGE);
      exit (EXIT_FAILURE);
    }
  else if (childpid==-1)
    {
      /* fork failed */
      fprintf (stderr,
	       _("%s: fork failed, in execute_rsh_single\n"), PACKAGE);
      return -1;
    }
  else				/* parent process */
    {
      if (pipe_option & PIPE_OPTION_INPUT)
	{
	  close (input_pipe[0]);
	  /* add input_pipe[1] to the array of outputs one
	     must handle here... */
	  add_fd_to_output_array(input_pipe[1]);
	}

      currentforkcount ++;

      if ((wait_shell)||	/* if wait_shell is specified */
	  ((forklimit > 0) && (currentforkcount > forklimit)) /* or fork limit is exceeded */
	  )
	{
	    /* wait for termination,
	       if it was required */
	  if (verbose_flag)
	    printf ("%s\n", _("... Waiting for process to end with waitpid"));

	  if (-1 != waitpid(wait_shell ? /* wait for childpid; or ANY if forklimit.
					    Setting this to ANY means it will go ahead when
					    any process ends, while waiting for childpid
					    means to wait for last process, which is less efficient.
					  */
			    childpid : /* wait_shell */
			    WAIT_ANY /* forklimit */
			    , &childstatus, 0))
	    {
	      assert(WIFEXITED(childstatus));
	      dsh_update_exit_code(WEXITSTATUS(childstatus));
	    }
	  else
	    {
	      assert(0);	/* weird error condition. */
	    }
	}

      return 0;
    }
  assert(0); /* nobody reaches here. */
}

/**
 * spawns dsh session on other machines. The invoked dsh session in
 * turn will invoke remote shell sessions.
 *
 * TODO: quote properly.
 * @return -1 on failure.
 */
static int
execute_rsh_multiple (const char * remoteshell_command,
		      const linkedlist * remoteshell_command_opt_r,
		      const linkedlist * machinelist,
		      int nummachines,
		      const linkedlist * rshcommandline_r)
{
  linkedlist* extraparam = NULL;
  linkedlist* tmp;
  linkedlist* tmp_start ;
  char * buffer;

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
  if (pipe_option)
    extraparam=lladd (extraparam, "-M");

  if (asprintf(&buffer, "-n%i", num_topology)<0)
    {
      fprintf (stderr, _("%s: asprintf failed\n"), PACKAGE);
      return -1;
    }

  extraparam=lladd (extraparam, buffer);
  free(buffer);

  /* add remoteshell command */
  if (asprintf(&buffer, "-r%s", remoteshell_command)<0)
    {
      fprintf (stderr, _("%s: asprintf failed\n"), PACKAGE);
      return -1;
    }

  extraparam=lladd (extraparam, buffer);
  free(buffer);

				/* end of extra options. */
  extraparam=lladd (extraparam, "--");
  rshcommandline_r = llcat (extraparam, lldup(rshcommandline_r));

  return execute_rsh_single (remoteshell_command, remoteshell_command_opt_r, machinelist->string, rshcommandline_r, 0);
}

/**
   execute remote shell
   executes dsh on other machines, or
   executes rsh to execute command.
   depending on the parameter and
   status.

   @return 0 on success, -1 on failure.
*/
static int
execute_rsh ( const char * remoteshell_command,
	      const linkedlist * remoteshell_command_opt_r,
	      const linkedlist * machinelist,
	      int nummachines /** The number of machines to invoke rsh at the same time */,
	      const linkedlist * rshcommandline_r)
{
  if (nummachines == 1)
    return execute_rsh_single (remoteshell_command, remoteshell_command_opt_r, machinelist->string, rshcommandline_r, pipe_option);
  else
    return execute_rsh_multiple (remoteshell_command, remoteshell_command_opt_r, machinelist, nummachines, rshcommandline_r);
}

/**
 * Forks off to do read from input of dsh, and duplicate it to all
 * child processes so that they can receive the same input.
 *
 * Used for processing the -i option
 *
 * By the time this routine is called, the child rsh processes are
 * up and running.
 */
void
run_input_forking_child_processes_process()
{
  char * buf;
  int count ;
  int i;

  fflush(stderr);
  fflush(stdout);
  switch (fork())
    {
    case 0:
      /* the child process */
      buf = malloc_with_error ( buffer_size );
      signal(SIGPIPE, SIG_IGN);	/* I'll handle SIGPIPE with EPIPE */

      /* This routine blocks even if all processes associated with
	 fd_output_array is terminated */
      while ((count = read(0, buf, buffer_size)) != -1 )
	{
	  if (count == 0)	/* if there is zero-byte read, it is an end-of-file */
	    break;
	  for (i=0; i<fd_output_array_len; ++i)
	    {
	      if (write(fd_output_array[i], buf, count) == -1)
		{
		  /* handle errors */
		  if (errno == EPIPE)
		    {
		      fprintf(stderr, _("%s: Process terminated (before write).\n"), PACKAGE);
		      /* pipe ended */
		      fflush(stderr);
		      goto out_of_while;
		      /* This is going to terminate ALL processes
		       even if only one server terminated early. */
		    }
		  else
		    perror("dsh: write");
		}
	    }
	}
    out_of_while:
      for (i=0; i<fd_output_array_len; ++i)
	{
	  close(fd_output_array[i]);
	}
      exit (EXIT_SUCCESS);
    case -1:
      /* fork failed */
      perror("fork");      	/* I want to know the errno */
      fprintf(stderr,_("%s: fork failed trying to dupilcate input\n"), PACKAGE);
      fflush(stderr);
      break;
    default:
      /* parent process closes the output array. */
      for (i=0; i < fd_output_array_len; ++i)
	{
	  close (fd_output_array [i]);
	}
      if (verbose_flag)
	printf (_("%s: forked off input forking process\n"), PACKAGE);
      /* do not read from stdin. */
      open_devnull();
    }
}

/**
   Shell dispatcher code. Called after the command-line parsing.

   This code is the starting point for the shell execution on multiple
   hosts.

   @return -1 on failure
   @return dsh_exit_code on success

   return value is used as the main dsh exit code.
*/
int
do_shell (linkedlist* machinelist, linkedlist*rshcommandline_r)
{
  int nummachines;
  int i;

  assert (num_topology > 0);
  assert (num_topology <= llcount(machinelist));

  nummachines = llcount(machinelist) / num_topology ;

  /* topology 1 is special. */
  if (nummachines == 0 || num_topology == 1)
    {
      nummachines = 1;
    }

  while (machinelist)
    {
      if (wait_shell && verbose_flag)
	{
	  fprintf (stderr, _("--- Executing on %s \n"), machinelist->string);
	}
      if (execute_rsh (remoteshell_command, remoteshell_command_opt_r, machinelist, nummachines, rshcommandline_r))
	{
	  fprintf(stderr, _("%s: execute_rsh failed, rsh invocation failure.\n"), PACKAGE);
	  return -1;
	}

      /* skip the machines that program was executed on, and repeat */
      for (i=0; (i < nummachines) && machinelist; ++i)
	  machinelist = machinelist -> next;
    }
  /* I have executed on all processes, I can now start cleaning up process...  */

  /* Try forking the input using fork, for -i option. */
  if (pipe_option &= PIPE_OPTION_INPUT)
    run_input_forking_child_processes_process();

  if (!wait_shell)
    {
      int childstatus = 0;
      int childpid;

      /* waiting for all. */
      while (-1!=(childpid=waitpid(WAIT_ANY, &childstatus, 0)))
	{
	  /* Is child dead of signal ? i.e. segv, etc. */
	  if (WIFSIGNALED(childstatus))
	    {
	      fprintf (stderr, _("%s: Child process %i exited with signal %i\n"), PACKAGE, childpid,
		       WTERMSIG(childstatus));
	      fflush (stderr);
	      dsh_update_exit_code(2);
	    }
	  else			/* get exit code and update */
	    {
	      assert(WIFEXITED(childstatus));
	      dsh_update_exit_code(WEXITSTATUS(childstatus));
	    }
	}
    }

  if (verbose_flag)
    fprintf(stderr, _("--- Terminated running\n"));

  return dsh_exit_code;
}

/**
 * The entry point to dsh. Sets up internationalization, and then
 * passes on control to other functions.
 */
int
main(int ac, char ** av)
{
  char *buf=NULL;

  setlocale (LC_ALL, "");
  if (!textdomain(PACKAGE_NAME))
    {
      if (!bindtextdomain(PACKAGE_NAME, LOCALEDIR))
	fprintf (stderr, "%s: failed to call bindtextdomain\n", PACKAGE);
    }

  /* load configuration files. */
  load_configfile(DSH_CONF);
  if (0>asprintf(&buf, "%s/.dsh/dsh.conf", getenv("HOME")))
    {
      fprintf (stderr, _("%s: asprintf failed\n"), PACKAGE);
      exit (1);
    }
  load_configfile(buf);
  free (buf);

  return parse_options(ac, av);
}
