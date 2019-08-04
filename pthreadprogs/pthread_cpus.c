// run program on reserved cpus 2+3
//
// add in /boot/cmdline.txt :
// isolcpus= 2,3
// https://www.raspberrypi.org/forums/viewtopic.php?f=33&t=247493&p=1512885#p1512885
// You can check by looking at /proc/<PID of your program>/status 
// and looking at the Cpus_allowed_list line.


#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <sched.h>

int main()
{
  cpu_set_t cpus;

  CPU_ZERO(&cpus);

  CPU_SET(2, &cpus);
  CPU_SET(3, &cpus);

  if (sched_setaffinity(0, sizeof(cpus), &cpus)) {
    perror("sched_setaffinity");
    exit(1);
  }
  
  /* Do stuff */
}
