/*

 */


#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>


void *func(void *arg) {
	printf("Thread started\nBye\n");
	
	//while(1) 
		pthread_testcancel();
    sleep(1);
	
	return (void*)0;;
}

int main(void) {
	pthread_t id;
	
	void *ret;
    
    assert(sizeof(void *) == sizeof(int));
	
	pthread_create(&id, 0, func, 0);
	
	printf("PTHREAD_CANCELED=%d\n", (int)PTHREAD_CANCELED);
	
	printf("auto terminated\n");
	//pthread_cancel(id);
	
	printf("Join\n");
	pthread_join(id, &ret);
	
	printf("%p\t%d\n", ret, ret == PTHREAD_CANCELED);

	printf("%d\n", (int)ret);
	
	return 0;
}


/*

PTHREAD_CANCELED=-1
auto terminated
Join
Thread started
Bye
0x20b18	0
133912


------------------
(program exited with code: 0)
Press return to continue



*/
