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

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>
#define PROGRAM_NAME "dsh"
#define DSH_VERSION "0.0.1"
#define DSH_CONF "/etc/dsh/dsh.conf"

typedef struct machine_id_tag
{
  char * machinename;
  struct machine_id_tag * next;
} machine_id;

char * remoteshell_command="rsh-client";
int verbose_flag=0;		/* verbosity flag */
int wait_shell=1;		/* waiting for shell to execute (concurrence) */

void * malloc_with_error(int size)
{
  void * u = malloc (size);
  if (!u)
    {
      fprintf (stderr, PROGRAM_NAME ": failed to allocate memory of %i bytes\n", size);
      exit(1);      
    }
  return u;  
}

machine_id* create_machineelement (char * buf, machine_id* next)
{
  machine_id*machinetmp= malloc_with_error(sizeof(machine_id));
  machinetmp->machinename = malloc_with_error(strlen(buf)+1);
  strcpy(machinetmp->machinename, buf);
  machinetmp->next=next;  
  return machinetmp;
}

int print_version(void)
{
  printf("Distributed Shell / Dancer\'s shell version %s \n"
	 "Copyright 2001 Junichi Uekawa, \n"
	 "distributed under the terms and conditions of GPL version 2\n\n",
	 DSH_VERSION);
  return 0;
}

int print_help (void)
{
  print_version();
  printf("-v --verbose                  Verbose output\n"
	 "-q --quiet                    Quiet\n"	
	 "-m --machine [machinename]    Execute on machine\n"
	 "-a --all                      Execute on all machines\n"
	 "-g --group [groupname]        Execute on group member\n"
	 "-r --remoteshell [shellname]  Execute using shell (rsh/ssh)\n"
	 "-h --help                     Give out this message\n"
	 "-w --wait-shell               Sequentially execute shell\n"
	 "-c --concurrent-shell         Execute shell concurrently\n"
	 "-V --version                  Give out version information\n\n"
 );
  return 0;
}


int do_shell(machine_id * machinelist, int index_point, int ac, char **av);

int load_configfile(const char * dsh_conf)
{
				/* load /etc/dsh/dsh.conf */
  FILE * f = fopen (dsh_conf, "r");  
  if (f)
    {
				/* not implemented. */
      fclose(f);
    }  
  return 0;  
}

static void stripn(char * buf)
{
  while(*buf)
    {
      if (*buf == '\n')
	{
	  *buf = 0;
	  return;
	}
      buf ++;
    }  
}

machine_id* read_machinelist(machine_id * machinelist, const char * listfile, const char*alternatelistfile)
{
  FILE * f;
  int bufferlen = 1024;  
  char * buf = malloc_with_error(bufferlen);
  
  if ((f = fopen (listfile, "r")) || (f=fopen(alternatelistfile, "r")))
    {
      while (-1 != getline (&buf, &bufferlen, f))
	{
	  stripn(buf);	  
	  machinelist=create_machineelement(buf, machinelist);
	}      
      fclose(f);
    }
  else
    {
      fprintf (stderr, "DSH: File %s nor %s could not be opened for read\n", listfile, alternatelistfile);
    }  
  free (buf);  
  return machinelist;  
}

int parse_options ( int ac, char ** av)
{
  int index_point;  
  int c;			/* option */
  machine_id * machinelist = NULL;
  
  static struct option long_options[]=
  {
    {"verbose", no_argument, 0, 'v'},
    {"quiet", no_argument, 0, 'q'},

				/* machine-specification */
    {"machine", required_argument, 0, 'm'},
    {"all", no_argument, 0, 'a' },
    {"group", required_argument, 0, 'g'},

				/* rsh/ssh specification */
    {"remoteshell", required_argument, 0, 'r'},
    {"help", no_argument, 0, 'h'},
    {"version", no_argument, 0, 'V'},
    {"wait-shell", no_argument, 0, 'w'},
    {"concurrent-shell", no_argument, 0, 'c'},
    {0,0,0,0}
  };
  
  
  while((c = getopt_long (ac, av, "vqm:ar:g:hVcw", long_options, &index_point)) != -1)
    {
      switch (c)
	{
	case 'a':
	  if (verbose_flag) printf ("Adding all machines to the list\n");
	  {
	    char * buf;
	    asprintf(&buf, "%s/.dsh/machines.list", getenv("HOME"));	    
	    machinelist = read_machinelist(machinelist, buf, "/etc/dsh/machines.list");	  
	    free (buf);	    
	  }
	  
	  break;	  
	case 'g':
	  if (verbose_flag) printf ("Adding group %s to the list\n", optarg);
	  {
	    char * buf1, *buf2;
	    asprintf(&buf1, "/etc/dsh/group/%s", optarg);
	    asprintf(&buf2, "%s/.dsh/group/%s", getenv("HOME"), optarg);
	    machinelist = read_machinelist (machinelist, buf2, buf1); 
	    free(buf1);free(buf2);	    
	  }
	case 'v':
	  if (verbose_flag) printf ("Verbose flag on\n");
	  verbose_flag=1;	  
	  break;
	case 'q':
	  if (verbose_flag) printf ("Verbose flag off\n");
	  verbose_flag=0; 
	  break;
	case 'm':
	  if (verbose_flag) printf ("Adding machine %s to list\n", optarg);
	  machinelist = create_machineelement(optarg, machinelist);
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
	  if (verbose_flag) printf("Using %s as the remote shell\n", optarg);
	  remoteshell_command = strdup (optarg);
	  break;	  
	case 'w':		/* wait shell */
	  if (verbose_flag) printf ("Wait for shell to finish executing\n");
	  wait_shell=1;
	  break;
	case 'c':		/* concurrent shell */
	  if (verbose_flag) printf ("Do not wait for shell to finish\n");
	  wait_shell=0;
	  break;
	default:
	  if (verbose_flag) printf ("Unhandled option\n");
	  break;	  
	}
    }

  if (!machinelist)
    {
      fprintf (stderr, "%s: no machine specified\n", av[0]);
      return 1;
    }
  return do_shell(machinelist, index_point, ac - optind, &(av[optind]));
}

int do_shell(machine_id* machinelist, int index_point, int ac, char **av)
{
  				/* optind to ac has the remaining options. */
  while (machinelist)
    {
      int childpid;
      int childstatus;      
      
      if (wait_shell) 
	{
	  fprintf (stderr, "--- Executing on %s \n", machinelist->machinename);
	}
      
      if (0==(childpid=fork()))
	{
	  char **uav = malloc_with_error (sizeof (char *) * (ac + 1 + 2));
	  int i;
	  
	  uav[0]=remoteshell_command;
	  uav[1]=machinelist->machinename;
	  for (i=0; i<ac; ++i)
	    uav[i + 2]=av[i];
	  av[ac-optind + 2]= 0;
	  
	  if (-1 == (execvp(remoteshell_command, uav)))
	    {
	      fprintf(stderr, "Failed to execute remote shell command %s\n", remoteshell_command);
	      _exit (EXIT_FAILURE);
	    }
	}
      else
	{
	  if (wait_shell)
	    waitpid(childpid, &childstatus, 0);	/* wait for termination */
	}
      
      machinelist = machinelist -> next;
    }
  if (!wait_shell)
    while(-1 != (waitpid(WAIT_ANY, NULL, 0)));	/* waiting for all. */
  
  if (verbose_flag)
    fprintf(stderr, "--- Terminated running\n");
  
  return 0;  
}
    
int main(int ac, char ** av)
{
  load_configfile(DSH_CONF);
  
  return parse_options(ac, av);  
}
