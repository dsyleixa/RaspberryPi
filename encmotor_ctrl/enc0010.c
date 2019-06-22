// Raspberry Pi Encoder Motor Control
//
// High Priority phread task for Encoder Timer 
// H-Bridge control: direction + pwm (L293 type)
//
// ver 0010

// protected under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License
// http://creativecommons.org/licenses/by-nc-sa/3.0/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <errno.h>
#include <pthread.h>
#include <termios.h>

#include "VG/openvg.h"
#include "VG/vgu.h"
#include "fontinfo.h"
#include "shapes.h"

#include <wiringPi.h>
#include <wiringSerial.h>

#include <stdbool.h>
#include <termio.h>



#define  byte  uint8_t;

#define  MAXMOTORS    2   // max number of encoder motors 

typedef struct  {	
	            // electrical motor pins
	   uint8_t  pind1, pind2, pinpwm;    // dir + pwm L293 H-Bridge type
	   uint8_t  pinQa, pinQb;            // rotary enc pins Qa,Qb
	   
	            // pwm and encoder values
	   int32_t  dirpwm;     
	   int32_t  motenc, oldenc;          // rotary encoder values 
} tEncMotor;

tEncMotor motor[MAXMOTORS];



/*************************************************************
* motor functions
*************************************************************/


inline void motoron(uint8_t nr, int dirpwm) {
	int dir, pwm;
	
	if(dirpwm>= 0) dir= +1;
	else dir= -1;
	pwm = abs(dirpwm);
	
	if(dir>=0) {
		digitalWrite(motor[nr].pind1, LOW);
		digitalWrite(motor[nr].pind2, HIGH);
	}
	else {
		digitalWrite(motor[nr].pind1, HIGH);
		digitalWrite(motor[nr].pind2, LOW);
	}
	motor[nr].dirpwm = dirpwm;
	pwmWrite(motor[nr].pinpwm, pwm);
	
	
}




/*************************************************************
* rpi_conio
*************************************************************/

bool kbhit(void)
{
    struct termios original;
    tcgetattr(STDIN_FILENO, &original);

    struct termios term;
    memcpy(&term, &original, sizeof(term));

    term.c_lflag &= ~ICANON;
    tcsetattr(STDIN_FILENO, TCSANOW, &term);

    int characters_buffered = 0;
    ioctl(STDIN_FILENO, FIONREAD, &characters_buffered);

    tcsetattr(STDIN_FILENO, TCSANOW, &original);

    bool pressed = (characters_buffered != 0);

    return pressed;
}

void echoOff(void)
{
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);

    term.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

void echoOn(void)
{
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);

    term.c_lflag |= ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}



                               
/*************************************************************
* Encoder Handler Routine
*************************************************************/

volatile int8_t ISRab[MAXMOTORS];

// 1/2 resolution
int8_t enctab[16] = {0, 0,0,0,1,0,0,-1, 0,0,0,1,0,0,-1,0};

void updateEncoders() {   
  int i; 	
  for( i=0; i<MAXMOTORS; ++i ) {	
     ISRab [ i] <<= 2;
     ISRab [ i] &=  0b00001100;
     ISRab [ i] |= (digitalRead( motor[ i].pinQa ) << 1) | digitalRead( motor[ i].pinQb );
     motor[ i].motenc += enctab[ISRab[ i] ];    
  }     

}

/*************************************************************
* pthread tasks
*************************************************************/
volatile static int8_t threadrun = 1;                             

void* thread3Go(void *)   //  high priority encoder task
{
   while(threadrun) {
      updateEncoders();
      usleep(100);
   }         
   return NULL;
}

void* thread2Go(void *)  // higher priority motor control task
{
   while(threadrun) {
	   
	  for(int pwm = -1023; pwm < 1023; ++pwm ) {
		motoron(0, pwm);  
        delay(10);
	  }
	  for(int pwm = 1023; pwm > -1023; --pwm ) {
		motoron(0, pwm);  
        delay(10);
	  }
   }         
   return NULL;
}

void* thread1Go(void *)   // medium priority task
{
   char  sbuf[128];
   int   c; 
   
   while(threadrun) {   	   
      c=0;
      if (kbhit())    {
          c = getchar();
          if(c==27) threadrun=0;  // semaphore to stop all tasks
      } 
      delay(100);
   }         
   return NULL;
}

void* thread0Go(void *)  // low priority display task
{
   char  sbuf[128];
   
   while(threadrun) {
      sprintf(sbuf, " 0=%6ld 1=%6ld     0=%6ld 1=%6ld \n ",  
         motor[0].motenc, motor[0].dirpwm,   motor[1].motenc, motor[1].dirpwm );
      printf(sbuf);
      delay(100);
   }         
   return NULL;
}




/*************************************************************
* setup
*************************************************************/

void setup() {
  int i;   
   
  // motor pin settings
  // encoder pin settings
  // setup for L293D motor driver
  
  // motor pins, wiringPi numbering (in parenthesis: BCM numbering)
  
     motor[0].pinQa = 5;   // (BCM 24) change for rotation direction
     motor[0].pinQb = 4;   // (BCM 23) change for rotation direction
     motor[0].pind1 =24;   // (BCM 19)
     motor[0].pind2 =25;   // (BCM 26)
     motor[0].pinpwm= 1;   // (BCM 18) hardware pwm
     
     motor[1].pinQa = 0;   // (BCM 17) change for rotation direction
     motor[1].pinQb = 2;   // (BCM 27) change for rotation direction
     motor[1].pind1 =21;   // (BCM 5)
     motor[1].pind2 =22;   // (BCM 6)
     motor[1].pinpwm=23;   // (BCM 13) hardware pwm   
     
     
     for( i=0; i< MAXMOTORS; ++i)  {       
        pinMode(motor[i].pinQa, INPUT);        // encA    
        pinMode(motor[i].pinQb, INPUT);        // encB    
        pinMode(motor[i].pind1, OUTPUT);       // dir-1   
        pinMode(motor[i].pind2, OUTPUT);       // dir-2   
        pinMode(motor[i].pinpwm ,PWM_OUTPUT);  // pwm
       
        motor[i].motenc = 0;
        motor[i].oldenc = 0;
        ISRab[i] = 0; 
	}
}



/*************************************************************
* main
*************************************************************/


int main() {
    char  sbuf[128];
    pthread_t thread0, thread1, thread2, thread3;
   
    wiringPiSetup();   
    if(wiringPiSetup() == -1) return 1;   
   
    setup();
   
    struct sched_param param;
   
   
    pthread_create(&thread0, NULL, thread0Go, NULL);
    param.sched_priority = 10;
    pthread_setschedparam(thread0, SCHED_RR, &param);
   
    pthread_create(&thread1, NULL, thread1Go, NULL);
    param.sched_priority = 25;
    pthread_setschedparam(thread1, SCHED_RR, &param);
   
    pthread_create(&thread2, NULL, thread2Go, NULL);
    param.sched_priority = 50;
    pthread_setschedparam(thread2, SCHED_RR, &param);
   
    pthread_create(&thread3, NULL, thread3Go, NULL);
    param.sched_priority = 90;
    pthread_setschedparam(thread3, SCHED_RR, &param);
   
   
    pthread_join(thread0, NULL);
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);
   
    exit(0);
   
}

/*

planned enhancements

              // PID custom target values
       int32_t  target;                  // set target
       int16_t  tarpwm;                  // motor target speed
                // PID custom regulation parameters
       double   kP;                      // P: basic propotional to error
       double   kI;                      // I: integral: avoid perish
       double   kD;                      // D: derivative: avoid oscillating
       double   precis;                  // error precision to target
       int32_t  regtime;                 // PID loop time
       double   damp;                    // damp the integral memory
       int8_t   cont;                    // target: continue or hit once
                // PID internal control variables
       int8_t   runstate;                // monitors runstate
       int16_t  outp;                    // PID control output value
       int16_t  maxout;                  // max output (max motor pwr)
       int32_t  read;                    // current sensor reading
       double   err;                     // current error
       double   integr;                  // integral of errors
       double   speed;                   // current speed


*/
