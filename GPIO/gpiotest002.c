//=====================================================================================
/*     Program GPIO test
 *     Raspberry Pi 2
 *     ver 001       
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




//*************************************************************
// tasks
//*************************************************************

unsigned long itime0, xtime0, itime1, xtime1, itime2, xtime2, itime3, xtime3; 
int s0=1, s1=1, s2=1, s3=1;
int counts = 100000;

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
	 
    for (int i=0; i<counts; i++) {
       ival=digitalRead(6);
       digitalWrite(21, HIGH);
       ival=digitalRead(6);
       digitalWrite(21, LOW);
    }    
     
    xtime1=millis();
    s1=0;
    return NULL;
}

void* thread2 (void* ) {       //  
	volatile int ival;
	
	itime2=millis();
    for (int i=0; i<counts; i++) {
       ival=digitalRead(17);
       digitalWrite(22, HIGH);
       ival=digitalRead(17);
       digitalWrite(22, LOW);
    }    
    xtime2=millis();
    s2=0;
    return NULL;
}


void* thread3 (void* ) {       //  
	volatile int ival;
	
	itime3=millis();
    for (int i=0; i<counts; i++) {
       ival=digitalRead(18);
       digitalWrite(23, HIGH);
       ival=digitalRead(18);
       digitalWrite(23, LOW);
    }    
    xtime3=millis();
    s3=0;
    return NULL;
}





//
void setupwiringPi() {
  
    int iores;
    putenv ("WIRINGPI_GPIOMEM=1");                 // no sudo for gpios required
    iores = wiringPiSetupGpio();                   // init by GPIO pin numbering
    if( iores == -1 ) exit(1);     
    
    pinMode(  5, INPUT); pullUpDnControl( 5, PUD_UP);
    pinMode(  6, INPUT); pullUpDnControl( 6, PUD_UP);
    pinMode( 17, INPUT); pullUpDnControl(17, PUD_UP);
    pinMode( 18, INPUT); pullUpDnControl(18, PUD_UP);
     
    pinMode( 20, OUTPUT);  
    pinMode( 21, OUTPUT);  
    pinMode( 22, OUTPUT);  
    pinMode( 23, OUTPUT);  
    
    
	
}




//*************************************************************
// main
//*************************************************************

int main() {
    int key;
    unsigned long t=0, t0, t1, t2, t3;
    double ns0, ns1, ns2, ns3;
    
    sleep(1);
    
    setupwiringPi();     
   
    pthread_t tid0, tid1, tid2, tid3;
    struct  sched_param  param;

    pthread_create(&tid0, NULL, thread0, NULL);    //  
    param.sched_priority = 80;
    pthread_setschedparam(tid0, SCHED_RR, &param);
    
    pthread_create(&tid1, NULL, thread1, NULL);    //  
    param.sched_priority = 60;
    pthread_setschedparam(tid1, SCHED_RR, &param);
    
    pthread_create(&tid2, NULL, thread2, NULL);    //  
    param.sched_priority = 40;
    pthread_setschedparam(tid2, SCHED_RR, &param);
    
    pthread_create(&tid3, NULL, thread3, NULL);    //  
    param.sched_priority = 20;
    pthread_setschedparam(tid3, SCHED_RR, &param);
    
    while( s0 || s1 || s2 || s3 ) { 
		printf("%ld\n", t);
		delay(100);
		t+=100;		
	};
   
     
    // wait for threads to join before exiting
    pthread_join( tid0, NULL);
    pthread_join( tid1, NULL);
    pthread_join( tid2, NULL);
    pthread_join( tid3, NULL);
    
    printf("\n\n");
    t0=xtime0-itime0;
    t1=xtime1-itime1;
    t2=xtime2-itime2;
    t3=xtime3-itime3;
    
    ns0=t0*1000000/(float)(4*counts); 
    ns1=t1*1000000/(float)(4*counts); 
    ns2=t2*1000000/(float)(4*counts); 
    ns3=t3*1000000/(float)(4*counts); 
    
    printf("time thread0: %ld ms,   time thread1: %ld ms\n", t0, t1); 
    printf("time thread2: %ld ms,   time thread3: %ld ms\n\”", t2, t3);
     
    printf("time delta0:  %.1f ns,  time delta1:  %.1f ns\n", ns0, ns1); 
    printf("time delta2:  %.1f ns,  time delta3:  %.1f ns\n\n", ns2, ns3); 
    
    
    key=getchar();

    
    exit(0);
}

//=====================================================================================
//=====================================================================================
