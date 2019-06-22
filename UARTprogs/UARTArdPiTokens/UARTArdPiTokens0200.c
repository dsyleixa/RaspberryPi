#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>

volatile bool TASKS_ACTIVE = false;
volatile bool HEARTBEAT = false;

// This is the cleanup function for UART_thr, it will be called if the
// thread is cancelled or exits via pthread_exit(). Use it to release any
// resources the thread has.
void UART_thr_cleanup(void* data)
{
    printf("UART_thr cleanup.\n");
}

void* UART_thr(void* data)          // UART comm (now just stalling)
{
    int old;
    if (pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &old)) {
        printf("UART: Error setting async cancel state.\n");
        pthread_exit(NULL);
    }
    pthread_cleanup_push(UART_thr_cleanup, NULL);
    HEARTBEAT = true;
    for(int i = 0; i < 5; i++) {
        printf("UART: i = %d\n", i);
        sleep(1);
    }
    printf("UART: simulating stall\n");
    HEARTBEAT = false;
    while(1) ;  // mimics stalling forever
    pthread_cleanup_pop(true);
    return NULL;
}


// We pass in a pointer to the thread id which we may need to cancel
void* WATCHER_thr(void* heartbeat_id)           // thread watcher
{
    pthread_t heartbeat_thread_id = *(pthread_t*)heartbeat_id;
    bool THREAD_ACTIVE = true;
    printf("WATCHER: Checking heartbeat.\n");
    while(THREAD_ACTIVE)  {  //
        //  cancel UART thread if no heart beat detected
        if(!HEARTBEAT) {
            //  cancel UART thread  // <~~~~~~~~~ ????  pthread_kill ? pthread_cancel ?
            printf("WATCHER: Lost heartbeat, cancelling UART thread.\n");
            pthread_cancel(heartbeat_thread_id);
            THREAD_ACTIVE = false;
        }
    }
    TASKS_ACTIVE = false;
    return NULL;  //
}


//

int main() {

    // threads
    pthread_t       thread0, thread1, thread2;
    pthread_attr_t  thread_attr;
    struct sched_param   param;

    // start multithreading
    pthread_attr_init(&thread_attr);  // Initialise the attributes
    pthread_attr_setschedpolicy(&thread_attr, SCHED_RR);  // Set attributes to RR policy

    param.sched_priority = 40;
    pthread_attr_setschedparam(&thread_attr, &param); // Set attributes to priority 40 (policy is already RR)
    pthread_create(&thread2, &thread_attr, UART_thr, NULL);    // medium  priority: UART

    param.sched_priority = 80;
    pthread_attr_setschedparam(&thread_attr, &param); // Set attributes to priority 80
    pthread_create(&thread0, &thread_attr, WATCHER_thr, &thread2);     // high priority: heartbeat monitor

    pthread_attr_destroy(&thread_attr); // We've done with the attributes

    TASKS_ACTIVE = true;
    while(TASKS_ACTIVE) {
        printf("MAIN: tasks active, waiting\n");
        sleep(1);
    }
    printf("MAIN: threads ended, goodbye.\n");

    // wait for threads to join before exiting
    pthread_join(thread0, NULL);
    //pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    exit(0);
}
