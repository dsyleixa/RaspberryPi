/*     Program GPIO speed test
 *     Raspberry Pi 2
 *     ver   001b     
 *
 */




#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <sys/time.h>
#include <errno.h>
#include <pthread.h>

#include <wiringPi.h>
#include <wiringSerial.h>
#include <wiringPiI2C.h>
#include <softPwm.h>





// tasks


unsigned long itime0, itime1, xtime0, xtime1;
int s0=1, s1=1;
int counts = 25000;

void* thread0 (void* ) {       // 
   volatile int ival;
   
   itime0=millis();
    for (int i=0; i<counts; i++) {
       ival=digitalRead(5);
       digitalWrite(20, HIGH);
       ival=digitalRead(5);
       digitalWrite(20, LOW);
    }   
    xtime0=millis();
    s0=0;
    return NULL;
}


void* thread1 (void* ) {       // 
   volatile int ival;
   
   itime1=millis();
   /*                               // optionally outcomment or uncomment!
    for (int i=0; i<counts; i++) {
       ival=digitalRead(6);
       digitalWrite(21, HIGH);
       ival=digitalRead(6);
       digitalWrite(21, LOW);
    }   
    */
    xtime1=millis();
    s1=0;
    return NULL;
}





//
void setupwiringPi() {
 
    int iores;
    putenv ("WIRINGPI_GPIOMEM=1");                 // no sudo for gpios required
    iores = wiringPiSetupGpio();                   // init by BCM pin numbering
    if( iores == -1 ) exit(1);     
   
    pinMode(  5, INPUT); pullUpDnControl( 5, PUD_UP);
    pinMode(  6, INPUT); pullUpDnControl( 6, PUD_UP);
     
    pinMode( 20, OUTPUT); 
    pinMode( 21, OUTPUT); 
   
   
   
}





// main


int main() {
    int key;
    unsigned long t=0, t0, t1;
    double ns0, ns1;
   
    sleep(1);
   
    setupwiringPi();     
   
    pthread_t tid0, tid1;
    struct  sched_param  param;

    pthread_create(&tid0, NULL, thread0, NULL);    // 
    param.sched_priority = 80;
    pthread_setschedparam(tid0, SCHED_RR, &param);
   
    pthread_create(&tid1, NULL, thread1, NULL);    // 
    param.sched_priority = 20;
    pthread_setschedparam(tid1, SCHED_RR, &param); // edited
   
    while(s0 || s1) {
      printf("%ld\n", t);
      delay(100);
      t+=100;      
   };
   
     
    // wait for threads to join before exiting
    pthread_join( tid0, NULL);
    pthread_join( tid1, NULL);
   
    printf("\n\n");
    t0=xtime0-itime0;
    t1=xtime1-itime1;
   
    ns0=t0*1000000/(float)(4*counts);
    ns1=t1*1000000/(float)(4*counts);
   
    printf("time thread0: %ld ms,   time thread1: %ld ms\n", t0, t1);
    printf("time delta0:  %.1f ns,  time delta1:  %.1f ns\n", ns0, ns1);
   
   
    key=getchar();

   
    exit(0);
}
