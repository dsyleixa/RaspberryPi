#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <wiringPi.h>


void ISR(uint32_t tick) {
   // do sth specific
}


uint32_t timerloop() {
   volatile static uint32_t  TimerTick=0;
   while(true) {
      if(TimerTick>=100000)TimerTick=0;
      ISR (TimerTick);
      TimerTick++;
      delayMicroseconds(10);
   }
}


int main() {
   const int policy = SCHED_RR;
   const int minPrio = sched_get_priority_min(policy);
   const int maxPrio = sched_get_priority_max(policy);
   sched_param param;
   
   pthread thread0;
   pthread_create(&thread0, NULL, timerloop, NULL);
   param.sched_priority = maxPrio - 1;
   pthread_setschedparam(thread0, policy, &param);

   //...
  
}