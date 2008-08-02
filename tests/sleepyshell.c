/*
  A shell command which does randomly sleep.
  Do not use its output directly in log diffs, since it is different every time.
  Counting the number of lines would be good.
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

int main(int argc, char** argv)
{
  int i;
  pid_t p=getpid();
  int sleeptime; 
  srandom(p);		/* I want my time to differ based on PID */
  sleeptime=(random()%10)*100000;
  usleep(sleeptime);	/* sleep! */

  printf("PID:%i time:%i ", 
	 p, sleeptime);		/* get pid for debugging. */
  for (i=1; i<argc; ++i)
    { 
      printf("%s ", argv[i]);
    }
  printf("\n");
  return 0;
}

