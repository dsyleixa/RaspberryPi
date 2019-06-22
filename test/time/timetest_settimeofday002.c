/*
 * settimetest.001c
 * set time test
 * 
 */


#include <sys/time.h>
#include <stdio.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    struct timeval now;
    int result;

    now.tv_sec = 1499878000;
    now.tv_usec = 0;

    result = settimeofday(&now, NULL);

    if(result < 0)
         fprintf(stderr,"Error setting the Time.\n");   
}
