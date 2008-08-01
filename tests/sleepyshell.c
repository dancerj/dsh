/*BINFMTC:
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

int main(int argc, char** argv)
{
  int i;

  srandom(getpid());		/* I want my time to differ based on PID */
  usleep((random()%10)*100000);	/* sleep! */
  for (i=1; i<argc; ++i)
    { 
      printf("%s ", argv[i]);
    }
  printf("\n");
  return 0;
}

