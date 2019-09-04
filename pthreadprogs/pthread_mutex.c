#include <pthread.h>      // POSIX pthread multithreading, link flag: -pthread

pthread_mutex_t  mutexBP; // Mutex variable

int main() {
    // *SNIP*   
    pthread_mutex_init (&mutexBP, NULL);   // init a mutex 
    // *SNIP*
}


// Und dann im Quellcode vor/hinter jeweils allen zu schützenden statements:
   pthread_mutex_lock (&mutexBP);       // lock the following variable operations by the mutex
   // (statements)
   pthread_mutex_unlock (&mutexBP);     // release the mutex to write/read by different threads

//