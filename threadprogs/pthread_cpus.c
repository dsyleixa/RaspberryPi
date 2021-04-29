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

/*

In main() after any initialisation put

pthread_t thread0, thread1, thread2, thread3;
const char *message0 = "Starting 0";
const char *message1 = "Starting 1";
const char *message2 = "Starting 2";
const char *message3 = "Starting 3";
int iret0, iret1, iret2, iret3;

// Create independent threads each of which will execute function

iret0 = pthread_create(&thread0, NULL, function0, (void*) message0);
if (iret0)
{
fprintf(stderr,"Thread 0 return : %d\n",iret0);
exit(EXIT_FAILURE);
}

Same for iret1, 2 and 3
Then use :

// All threads should never finish
pthread_join(thread0, NULL);
pthread_join(thread1, NULL);
pthread_join(thread2, NULL);
pthread_join(thread3, NULL);



Then for each thread you want

void *function0(void *ptr)
{
char *message = (char*) ptr;
printf("%s \n", message);
cpu_set_t cpuset;
CPU_ZERO(&cpuset);
CPU_SET(1,&cpuset);
sched_setaffinity(0, sizeof(cpu_set_t), &cpuset);
while (1)
DoStuff();
}

Finally you use isolcpu = 1,2,3 or whatever you need to isolate the time critical threads. Put all your non-time critcal stuff in one of the threads you don't isolate and allow Linux to also use that CPU or of course you will have real problems.


*/
