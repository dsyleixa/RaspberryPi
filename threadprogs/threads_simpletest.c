#include <pthread.h>
#include <stdio.h>
#include <wiringPi.h>

/* volatile */ _Atomic bool ThreadsRun=true;

//...
void foo* (void*) {
  while(ThreadsRun) {
    ThreadsRun=digitalRead(26);
  }
  delay(1);
}

void bas* (void*) {
  static int t=0;
  while(ThreadsRun) {
     printf("%d \n", t++);
  }
  delay(100);
}

int main() {
  wiringPiSetupGpio();
  pthread_t thread0, thread1;
  //...
        
  pthread_create(&thread0, NULL, foo, NULL);
  pthread_create(&thread1, NULL, bas, NULL);
  
  pthread_join(thread0, NULL);
  pthread_join(thread1, NULL);
  
  return 0;
}