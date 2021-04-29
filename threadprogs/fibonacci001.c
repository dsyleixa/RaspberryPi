/*
 *
 * 
 */


#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include <unistd.h>

#define msleep(ms)  usleep(1000*ms)


long f;

unsigned long long fibrec( long n ) {
  unsigned long long   llbuf;  
  volatile static long order = 0;

  if ( n==0 )    { llbuf = 0;  }
  else   
  if ( n==1 )    { llbuf = 1;  }
  else {
     llbuf = fibrec(n-1) + fibrec(n-2);     
     if(n > order) {
         order = n;
         printf("\n order recursive: %ld  %lld \n" , order, llbuf); 
     }     
  }
  return (llbuf); 
}



volatile int running = 1;



void* thread1Go(void *) {
    unsigned long long res;
       
    res = fibrec(f);
    
    printf("\n\nFibonacci (%ld) is = %lld \n", f, res);     
    
    running = 0;
    msleep(100);
    return NULL;
    
}


void* thread0Go(void *) {
    
    while(running) {
       sleep(1);
       printf(".");
       fflush(stdout);
    }
    return NULL;
    
}



int main(int argc, char **argv)
{
	
    pthread_t thread0, thread1;


    printf("enter Fibonacci order: ");
    scanf("%ld", &f);  
    
    pthread_create(&thread0, NULL, thread0Go, NULL); 
    pthread_create(&thread1, NULL, thread1Go, NULL); 
    
     
    pthread_join(thread0, NULL);
    pthread_join(thread1, NULL);

    printf("program finished \n");

    
	return 0;
}

