//----------------------------------------------------------------------
// Raspberry Pi Encoder Motor Control
//
// High Priority phread task for Encoder Timer
// H-Bridge control: direction + pwm (L293 type)
//
// ver 003

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
#include <softPwm.h>

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



//*************************************************************
// motor functions
//*************************************************************


#define motorCoast(nr) motorOn(nr, 0)          // alias for motor coast


void motorBrake(int nr, int dirpwm) {  // brake by pwm power
   int pwm;
   
   pwm = abs(dirpwm);
   
   digitalWrite(motor[nr].pind1, HIGH);
   digitalWrite(motor[nr].pind2, HIGH);      
   
   motor[nr].dirpwm = pwm;
   softPwmWrite(motor[nr].pinpwm, pwm);  // brake power always > 0   
   
}



void motorOn(int nr, int dirpwm) { // motor On (nr, dir_pwm)
   int dir, pwm;                             // signed pwm:
   
   if(dirpwm > 0) dir = +1;                   // pwm > 0: forward
   else if(dirpwm < 0) dir = -1;              // pwm < 0: reverse
   else dir = 0;                              // pwm = 0: coast
   pwm = abs(dirpwm);
   
   if(dir> 0) {
      digitalWrite( motor[nr].pind1, HIGH);
      digitalWrite( motor[nr].pind2, LOW);      
   }
   else
   if(dir==0) {
      digitalWrite( motor[nr].pind1, LOW);
      digitalWrite( motor[nr].pind2, LOW);
   }
   else {
      digitalWrite( motor[nr].pind1, LOW);
      digitalWrite( motor[nr].pind2, HIGH);
   }
   motor[nr].dirpwm = dirpwm;
   softPwmWrite( motor[nr].pinpwm, pwm);
   
   
}




//*************************************************************
// rpi_conio
//*************************************************************

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

//*************************************************************

void echoOff(void)
{
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);

    term.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

//*************************************************************

void echoOn(void)
{
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);

    term.c_lflag |= ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

                             
//*************************************************************
// Encoder Handler Routine
//*************************************************************

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

//*************************************************************
// pthread tasks
//*************************************************************

volatile static int8_t threadrun = 1;               
             

void* encoder_thread(void *)   //  high priority encoder task
{
   while(threadrun) {
       updateEncoders();
       usleep(100);
   }         
   return NULL;
}

//*************************************************************

void* motorctrl_thread(void *)  // higher priority motor control task
{
   while(threadrun) {
      
     for(int pwm = -1023; pwm < 1023; ++pwm ) {
                motorOn(0, pwm); 
                motorOn(1, pwm); 
                if (!threadrun) return NULL;  
                delay(10);
     }
     for(int pwm = 1023; pwm > -1023; --pwm ) {
                motorOn(0, pwm); 
                motorOn(1, pwm); 
                if (!threadrun) return NULL;
                delay(10);
     }
   }         
   return NULL;
}

//*************************************************************

void* kbd_thread_thread(void *)   // medium priority task for keyb read 
{
   char  sbuf[128];
   int   c;
   
   while(threadrun) {         
      c=0;
      if (kbhit())    {
          c = getchar();          // ESC to quit program 
          if( c==27 ) {
               threadrun=0;  // semaphore to stop all tasks
               printf("\n\n ESC pressed - program terminated by user \n\n");
               return NULL;
          }
      }
      delay(50);
   }         
   return NULL;
}

//*************************************************************

void* display_thread(void *)  // low priority display task
{
   char  sbuf[128];
   
   while(threadrun) {
      sprintf(sbuf, " m0=%8ld  pwm=%6d     m1=%8ld  pwm=%6d \n ", 
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
  int i, err;   
   
  // motor pin settings
  // encoder pin settings
  // setup for L293D motor driver
 
  // motor pins, wiringPi numbering (in parenthesis: BCM numbering)
 
     motor[0].pinQa = 5;   // (BCM 24) change for rotation direction
     motor[0].pinQb = 4;   // (BCM 23) change for rotation direction
     motor[0].pind1 =24;   // (BCM 19)
     motor[0].pind2 =25;   // (BCM 26)
     motor[0].pinpwm=26;   // (BCM ..)  pwm
     
     motor[1].pinQa = 0;   // (BCM 17) change for rotation direction
     motor[1].pinQb = 2;   // (BCM 27) change for rotation direction
     motor[1].pind1 =21;   // (BCM  5)
     motor[1].pind2 =22;   // (BCM  6)
     motor[1].pinpwm=23;   // (BCM ..)  pwm   
     
     
     for( i=0; i< MAXMOTORS; ++i)  {       
        pinMode( motor[i].pinQa, INPUT);        // encA   
        pinMode( motor[i].pinQb, INPUT);        // encB   
        pinMode( motor[i].pind1, OUTPUT);       // dir-1   
        pinMode( motor[i].pind2, OUTPUT);       // dir-2   
        pinMode( motor[i].pinpwm, PWM_OUTPUT);  // pwm
        
        err= softPwmCreate( motor[i].pinpwm, 0, 1024);
        
        printf("err %-4d  qa %-4d qb %-4d d1 %-4d d2 %-4d pwm %-4d \n", 
          err, motor[i].pinQa, motor[i].pinQb, motor[i].pind1, motor[i].pind2, motor[i].pinpwm);
       
        motor[i].motenc = 0;
        motor[i].oldenc = 0;
        ISRab[i] = 0;
   }
   printf("press ENTER");
   getchar();
}



/*************************************************************
* main
*************************************************************/


int main() {
    char  sbuf[128];
    int ioerr;
    pthread_t thread0, thread1, thread2, thread3;
    struct  sched_param  param;
   
    ioerr = wiringPiSetup();   
    if( ioerr == -1 ) return 1;   
   
    setup();   
   
    pthread_create(&thread0, NULL, display_thread, NULL);   // lowest priority task: screen output
    param.sched_priority = 10;
    pthread_setschedparam(thread0, SCHED_RR, &param);
   
    pthread_create(&thread1, NULL, kbd_thread_thread, NULL); // medium priority task: keyboard monitoring (stop program)
    param.sched_priority = 25;
    pthread_setschedparam(thread1, SCHED_RR, &param);
   
    pthread_create(&thread2, NULL, motorctrl_thread, NULL); // higher priority task:  motor control program  
    param.sched_priority = 50;
    pthread_setschedparam(thread2, SCHED_RR, &param);
   
    pthread_create(&thread3, NULL, encoder_thread, NULL);   // highest priority task: encoder reading     
    param.sched_priority = 90;
    pthread_setschedparam(thread3, SCHED_RR, &param);
   
   
    pthread_join(thread0, NULL);
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);
   
    for(int i=0; i< MAXMOTORS; ++i)  {
       motorOn(i, 0);
    }
   
    delay(1000);
   
    exit(0);
   
}