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
 * Command-line and parameter handling routines.
 */
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "dsh.h"

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif
#include "config.h"
#include "linkedlist.h"
#include "parameter.h"
#include "libdshconfig.h"

#include "gettext.h"
#define _(A) gettext(A)

/**
 * allocate memory with error
 *
 * @return allocated memory
 */
void * 
malloc_with_error(int size)
{
  void * u = malloc (size);
  if (!u)
    {
      fprintf (stderr, _("%s: failed to allocate memory of %i bytes\n"), PACKAGE, size);
      exit(1);      
    }
  return u;  
}

/* 
 * remove \n and other things from string
 * and return the new pointer.
 */
static const char *
stripwhitespace(char * buf)
{
  char * pointer = buf;
  char * revpointer;

  while (*pointer && isspace(*pointer))
    pointer ++;
  if (!*pointer)			/* if it is nothing, return NULL */
    return NULL;

  revpointer = pointer + strlen (pointer) - 1;

  /* strip whitespace from end. */
  while (revpointer > pointer && isspace(*revpointer))
    *(revpointer --) = 0;

  return pointer;
}


/**
 * Add hostname to machinelist if and only if 
 * the host does not already exist in the machine list.
 */
static linkedlist *
machinelist_lladd(linkedlist * machinelist,
		  const char * machinename)
{
  if (llmatch(machinelist, machinename)) /* if there is match, ignore */
    return machinelist;
  else
    return lladd(machinelist, machinename);
}


/*
 * Load machine list from netgroup (typically NIS)
 * Todo: Should perhaps remove duplicates.
 *
 * Code contributed from Petter Reinholdtsen on 22 Dec 2001.
 */
linkedlist*
read_machinenetgroup(linkedlist * machinelist,
                    const char * groupname)
{
  if (setnetgrent(groupname)) 
    {
      char *hostp=NULL, *userp=NULL, *domainp=NULL;
      
      while (getnetgrent(&hostp, &userp, &domainp))
	{
	  if (NULL != hostp)
	    machinelist = machinelist_lladd(machinelist, hostp);
	  
	}
      endnetgrent();
    } 
  else
    {
      fprintf(stderr, 
	      _("%s: Unknown netgroup %s.\n"),
	      PACKAGE, groupname);
    }
  return machinelist;
}

/**
 * read the machine list file from file.
 */
linkedlist* 
read_machinelist(linkedlist * machinelist, const char * listfile, const char*alternatelistfile)
{
  FILE * f;
  int bufferlen = 1024;  
  char * buf = malloc_with_error(bufferlen);
  
  if ((f = fopen (listfile, "r")) || ((NULL != alternatelistfile) && (f=fopen(alternatelistfile, "r"))))
    {
      while (-1 != getline (&buf, &bufferlen, f))
	{
	  const char * strippedstring = stripwhitespace(buf);
	  if (strippedstring)
	    machinelist=machinelist_lladd(machinelist,strippedstring); 
	}
      fclose(f);
    }
  else
    {
      if (alternatelistfile)
	fprintf (stderr, 
		 _("%s: File %s nor %s could not be opened for read\n"),
		 PACKAGE, listfile, alternatelistfile);
      else
	fprintf (stderr, 
		 _("%s: File %s could not be opened for read\n"), 
		 PACKAGE, listfile);
    }  
  free (buf);  
  return machinelist;  
}

int print_version(void)
{
  printf(_("Distributed Shell / Dancer\'s shell version %s \n"
	 "Copyright 2001,2002 Junichi Uekawa, \n"
	 "distributed under the terms and conditions of GPL version 2\n\n"),
	 VERSION);
  return 0;
}

int
print_help (void)
{
  print_version();
  printf(_(
	   "-v --verbose                   Verbose output\n"
	   "-q --quiet                     Quiet\n"	
	   "-M --show-machine-names        Prepend the host name on output\n"
	   "-i --duplicate-input           Duplicate input given to dsh\n"
	   "-b --bufsize                   Change buffer size used in input duplication\n"
	   "-m --machine [machinename]     Execute on machine\n"
	   "-n --num-topology              How to divide the machines\n"
	   "-a --all                       Execute on all machines\n"
	   "-g --group [groupname]         Execute on group member\n"
	   "-f --file [file]               Use the file as list of machines\n"
	   "-r --remoteshell [shellname]   Execute using shell (rsh/ssh)\n"
	   "-o --remoteshellopt [option]   Option to give to shell \n"
	   "-h --help                      Give out this message\n"
	   "-w --wait-shell                Sequentially execute shell\n"
	   "-c --concurrent-shell          Execute shell concurrently\n"
	   "-V --version                   Give out version information\n\n")
 );
  return 0;
}


				/* load the configuration file. */
int
load_configfile(const char * dsh_conf)
{
  dshconfig * t;
  dshconfig_internal * line;
  FILE*f = fopen (dsh_conf, "r");  
  char * buf_a=NULL;
  char * buf_b=NULL;
  
  if(verbose_flag) printf(_("Loading config file %s\n"), dsh_conf);

  if (f && (t = open_dshconfig(f, '=')))
    {
      for (line = t->config; line; line=line->next)
	{
	  buf_a = line -> title;
	  buf_b = line -> data;

	  if(verbose_flag) printf(_(" Parameter %s is %s\n"), buf_a, buf_b);
	  if (!strcmp(buf_a, "remoteshell"))
	    {
		  if (verbose_flag) printf(_("Using %s as the remote shell\n"), buf_b);
		  remoteshell_command = strdup (buf_b);
	    }	      
	  else if (!strcmp(buf_a, "remoteshellopt"))
	    {
	      if (verbose_flag) printf(_("Adding [%s] to shell options\n"), buf_b);
	      remoteshell_command_opt_r = lladd (remoteshell_command_opt_r, buf_b);
	    }	  
	  else if (!strcmp(buf_a, "waitshell"))
	    {
	      wait_shell = atoi ( buf_b );		  
	      if (verbose_flag) printf(_("Setting wait-shell to  [%i]\n"), wait_shell);
	    }	      
	  else if (!strcmp(buf_a, "showmachinenames"))
	    {
	      pipe_option |= (atoi ( buf_b ) != 0 );
	      if (verbose_flag) printf(_("Setting pipe option to  [%i]\n"), pipe_option);
	    }	      
	  else if (!strcmp(buf_a, "verbose"))
	    {
	      verbose_flag = atoi ( buf_b );
	      if (verbose_flag) printf(_("Setting verbose to  [%i]\n"), verbose_flag);
	    }	      
	  else
	    {
	      fprintf (stderr, 
		       _("%s: unparsed configuration file line %s found in %s\n"),
		       PACKAGE, buf_a, dsh_conf);
	    }
	}

      free_dshconfig(t);
      fclose(f);
    }  
  return 0;
}

/** 
 * open /dev/null as stdin
 */
void 
open_devnull(void)
{
  int in = open ("/dev/null", O_RDONLY);
  dup2 (in, 0);
}


/**
 * Split the option string with "," as delimiter, and 
 * add the hostnames to the linked list.
 *
 * @return linked list with items added.
 */
static linkedlist*
split_machines_list_and_add_machines(linkedlist* machinelist, const char * optarg)
{
  char * s = strdup (optarg);
  char * next ;
  char * current = s;
  
  while ((next = strchr(current,',')))
    {
      *next = 0;
      machinelist = machinelist_lladd(machinelist, current);
      current = next + 1;
    }
  machinelist = machinelist_lladd(machinelist, current);
  
  free(s);
  return machinelist;
}

/**
 * Option parsing routine.
 *
 * Remember to update other places, such as execute_rsh_multiple routine when changing here.
 * @return 1 on failure.
 */
int
parse_options ( int ac, char ** av)
{
  int index_point;  
  int c;			/* option */
  linkedlist * machinelist = NULL;
  linkedlist * rshcommandline_r = NULL; /* command line for rsh in reverse order*/

#ifdef HAVE_GETOPT_LONG
  static struct option long_options[]=
  {
    {"verbose", no_argument, 0, 'v'},
    {"quiet", no_argument, 0, 'q'},
    {"show-machine-names", no_argument, 0, 'M'},
    {"duplicate-input", no_argument, 0, 'i'},
    {"bufsize", required_argument, 0, 'b'},

				/* machine-specification */
    {"machine", required_argument, 0, 'm'},
    {"num-topology", required_argument, 0, 'n'},
    {"all", no_argument, 0, 'a' },
    {"group", required_argument, 0, 'g'},
    {"file", required_argument, 0, 'f'},

				/* rsh/ssh specification */
    {"remoteshell", required_argument, 0, 'r'},
    {"remoteshellopt", required_argument, 0, 'o'},

    {"help", no_argument, 0, 'h'},
    {"version", no_argument, 0, 'V'},
    {"wait-shell", no_argument, 0, 'w'},
    {"concurrent-shell", no_argument, 0, 'c'},
    {0,0,0,0}
  };
#else
#define getopt_long(a,b,c,d,e) getopt(a,b,c)
#endif  
  
  while((c = getopt_long (ac, av, "vqm:ar:f:g:hVcwo:Mn:ib:", long_options, &index_point)) != -1)
    {
      switch (c)
	{
	case 'a':
	  if (verbose_flag) printf (_("Adding all machines to the list\n"));
	  {
	    char * buf;
	    asprintf(&buf, "%s/.dsh/machines.list", getenv("HOME"));	    
	    machinelist = read_machinelist(machinelist, buf, DSHCONFDIR"/machines.list");
	    free (buf);	    
	  }	  
	  break;	  
	case 'g':
	  {
            if ('@' == *optarg)
              {			/* using libc call for using netgroup. */
                /* +1 to skip @ */
		if (verbose_flag) printf (_("Adding netgroup %s to the list\n"), optarg + 1);
                machinelist = read_machinenetgroup(machinelist, optarg+1);
              }
            else
              {			/* using dsh's own method. */
		char * buf1, *buf2;
		if (verbose_flag) printf (_("Adding group %s to the list\n"), optarg);
		asprintf(&buf1, DSHCONFDIR"/group/%s", optarg);
		asprintf(&buf2, "%s/.dsh/group/%s", getenv("HOME"), optarg);
		machinelist = read_machinelist (machinelist, buf2, buf1); 
		free(buf1);free(buf2);
	      }
	  }
	  break;	  
	case 'f':
	  if (verbose_flag) printf (_("Adding file %s to the list\n"), optarg);
	  machinelist = read_machinelist (machinelist, optarg, NULL); 
	  break;	  
	case 'v':
	  if (verbose_flag) printf (_("Verbose flag on\n"));
	  verbose_flag=1;	  
	  break;
	case 'q':
	  if (verbose_flag) printf (_("Verbose flag off\n"));
	  verbose_flag=0; 
	  break;
	case 'M':
	  if (verbose_flag) printf (_("Show machine names on output\n"));
	  pipe_option |= PIPE_OPTION_OUTPUT;
	  break;	  
	case 'i':
	  if (verbose_flag) printf (_("Duplicate input to all processes\n"));
	  pipe_option |= PIPE_OPTION_INPUT;
	  break;	  
	case 'b':
	  if (verbose_flag) printf (_("Buffer size used for dupliation\n"));
	  buffer_size = atoi(optarg);	  
	  break;	  
	case 'm':
	  if (verbose_flag) printf (_("Adding machine %s to list\n"), optarg);
	  machinelist = split_machines_list_and_add_machines (machinelist, optarg);
	  break;
	case 'n':
	  if (verbose_flag) printf (_("Topology number set to %s\n"), optarg);
	  num_topology = atoi (optarg);	  
	  break;	  
	case 'h':
	  print_help();
	  exit(1);
	  break;	  
	case 'V':
	  print_version();
	  exit(1);
	  break;	  
	case 'r':		/* specify the shell command */
	  if (verbose_flag) printf(_("Using %s as the remote shell\n"), optarg);
	  remoteshell_command = strdup (optarg);
	  break;	  
	case 'o':		/* specify the shell command options */
	  if (verbose_flag) printf(_("Adding [%s] to shell options\n"), optarg);
	  remoteshell_command_opt_r = lladd (remoteshell_command_opt_r, optarg);
	  break;	  
	case 'w':		/* wait shell */
	  if (verbose_flag) printf (_("Wait for shell to finish executing\n"));
	  wait_shell=1;
	  break;
	case 'c':		/* concurrent shell */
	  if (verbose_flag) printf (_("Do not wait for shell to finish\n"));
	  wait_shell=0;
	  break;
	default:
	  if (verbose_flag) printf (_("Unhandled option\n"));
	  break;	  
	}
    }

  if (!machinelist)
    {
      fprintf (stderr, _("%s: no machine specified\n"), PACKAGE);
      return 1;
    }

  {				/* generate the command line for remote. */
    int i;
    for (i=optind; i < ac; ++i)
      {
	rshcommandline_r = lladd(rshcommandline_r, av[i]);
      }
  }
  
  /* sanity checking */
  if ((pipe_option & PIPE_OPTION_INPUT) && wait_shell )
    {
      fprintf (stderr, _("%s: Input duplication and concurrent shell need to be specified together\n"), PACKAGE);
      return 1;
    }
  

  if (!(pipe_option & PIPE_OPTION_INPUT))
    open_devnull();		/* open /dev/null if no input pipe is required */

				/* actually execute the code. */
  return do_shell(machinelist, rshcommandline_r);
}
