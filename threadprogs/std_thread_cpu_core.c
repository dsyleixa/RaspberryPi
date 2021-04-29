
#include <pthread.h>
#include <thread>
#include <sched.h>
#include <unistd.h>


cpu_set_t cpus;
unsigned int core = 0;

using namespace std;


void foo() { 
    // core affinity was set from outside...
    //...
}

void bas() {
   // calling core affinity from inside of a thread:	 
   CPU_ZERO(&cpus);  // Clear the set of cpus   
   core=2;           // set the cpu core number
   CPU_SET(core, &cpus);  
   
   sched_setaffinity( 0, sizeof(cpu_set_t), &cpus ); 
   ... 
}



int main() {   
   //...   
   std::thread t1 (foo);
   CPU_ZERO(&cpus);  // Clear the set of cpus   
   core=1;           // correct way to set the value?
   CPU_SET(core, &cpus);     
   pthread_setaffinity_np( t1.native_handle(), sizeof(cpu_set_t), &cpus );
   //...
   
   // calling core affinity from inside of a thread:
   std::thread t2 (bas);
}