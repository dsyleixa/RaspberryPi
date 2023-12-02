

#include <stdio.h>
#include <pthread.h>
//#include <unistd.h>
#include <wiringPi.h>


void *func0(void *arg) {
     fprintf(stderr, "func0 started by thread0\n");	
     fprintf(stderr, "func0 finished\n");		
     return NULL;
}

void *func1(void *arg) {
    fprintf(stderr, "func1 started by thread1\n");
    fprintf(stderr, "func1 finished\n");		
    return NULL;
}


int main(void) {
    pthread_t thread0, thread1;
    
    unsigned long t=millis(), now=millis();
	
	pthread_create( &thread0, 0, func0, 0);
    fprintf(stderr, "\nthread0 started, time ms=%lu\n", millis()-now);
    now=millis();    
    pthread_create( &thread1, 0, func1, 0);
    fprintf(stderr, "\nthread1 started, time ms=%lu\n", millis()-now);  
    
    	
	pthread_join(thread0, NULL);
    pthread_join(thread1, NULL);
	
	fprintf(stderr, "\nthreads thread0, thread1 joined, program finished\n");
    fprintf(stderr, "\nprogram runtime ms=%lu\n", millis()-t); 
	
	return 0;
}


/*


*/
