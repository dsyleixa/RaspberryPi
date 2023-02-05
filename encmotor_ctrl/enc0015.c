// Raspberry Pi Encoder Motor Control
//
// High Priority phread task for Encoder Timer
// H-Bridge control: direction + pwm (L293 type)
//
// ver 0013

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


#define MAXMOTORS          10 // maximum number of encoder motors 
#define MAXANALOG           9 // maximum number of analog sensors 
#define MAXDIGITAL         16 // maximum number of digital pins 

typedef struct  {   
               // electrical motor pins
      uint8_t  pind1, pind2, pinpwm;    // dir + pwm L293 H-Bridge type
      uint8_t  pinQa, pinQb;            // rotary enc pins Qa,Qb
      
               // pwm and encoder values
      int32_t  dirpwm;                  // applied pwm  
      int32_t  motenc, oldenc;          // rotary encoder values
} tEncMotor;


tEncMotor motor[MAXMOTORS];



/**************************************************************
  
/**************************************************************
  
 * NEW pin setup
 *  
 1 (  )   +3.3                              2 (  )   +5
 3 ( 2)   i2c-1 SDA-1                       4 (  )   +5
 5 ( 3)   i2c-1 SCL-1                       6 (  )   GND
 7 ( 4)   <<< 1-Wire  <<<                   8 (14)   UART TX ArdMEGA
 9 (  )   GND                              10 (15)   UART RX ArdMEGA
11 (17)   motQa-1                          12 (18)   < Bumper FLeft  >
13 (27)   motQb-1                          14 (  )   GND
15 (22)   motd1-m0 (en)                    16 (23)   motd1-m1 (en)
17 (  )   +3.3                             18 (24)   motd2-m0 (dir)      
19 (10)   <<< SPI MOSI  <<<                20 (  )   GND
21 ( 9)   <<< SPI MISO  <<<                22 (25)   motd2-m1 (dir) //SPIdisab
23 (11)   <<< SPI SCK   <<<                24 ( 8)   <<< CE0 <<<
25 (  )   GND                              26 ( 7)   <<< CE1 <<<
27 ( 0)   i2c-0 SDA-0                      28 ( 1)   i2c-0 SCL-0 
29 ( 5)   motSF-m0   <<<                   30 (  )   GND
31 ( 6)   motSF-m1   <<<                   32 (12)   motpwm-m0
33 (13)   motpwm-m1                        34 (  )   GND
35 (19)   motQa-m0                         36 (16)   < Bumper FRight >  
37 (26)   motQb-m0                         38 (20)   < Bumper rear >  
39 (  )   GND                              40 (21)   < EMERGENCY STOP >
  
*/

/*  
 * OLD pin setup
  
 1 (  )   +3.3                              2 (  )   +5
 3 ( 2)   i2c-1 SDA-1                       4 (  )   +5
 5 ( 3)   i2c-1 SCL-1                       6 (  )   GND
 7 ( 4)   <<< 1-Wire  <<<                   8 (14)   UART TX ArdMEGA 
 9 (  )   GND                              10 (15)   UART RX ArdMEGA 
11 (17)   motQa-m1                         12 (18)   < Bumper FLeft  >
13 (27)   motQb-m1                         14 (  )   GND
15 (22)   <           >                    16 (23)   motQa-m0
17 (  )   +3.3                             18 (24)   motQb-m0
19 (10)   <<< SPI MOSI  <<<                20 (  )   GND
21 ( 9)   <<< SPI MISO  <<<                22 (25)   <<< SPI CS/SS <<<
23 (11)   <<< SPI SCK   <<<                24 ( 8)   <<< CE0 <<<
25 (  )   GND                              26 ( 7)   <<< CE1 <<<
27 ( 0)   i2c-0 SDA-0                      28 ( 1)   i2c-0 SCL-0  
29 ( 5)   motd1-m1                         30 (  )   GND
31 ( 6)   motd2-m1                         32 (12)   motpwm-m0
33 (13)   motpwm-1                         34 (  )   GND
35 (19)   motd1-m0                         36 (16)   < Bumper FRight >
37 (26)   motd2-m0                         38 (20)   < Bumper Rear   >
39 (  )   GND                              40 (21)   < EMERGENCY STOP >
  
*/



//*************************************************************
// motor ON / OFF / DIR  functions
//*************************************************************

//*************************************************************
// outcomment/uncomment motor type patterns
//*************************************************************


// #define  motorBrake(nr,p)  motorL293Brake(nr,p)
#define  motorBrake(nr,p)  motorMC33Brake(nr,p)


// #define motorOn(nr,p)  motorL293On(nr,p)
#define motorOn(nr,p)  motorMC33On(nr,p)


// #define motorCoast(nr,p)  motorL293Coast(nr)
#define motorCoast(nr)  motorMC33Coast(nr) 




//*************************************************************
// motor type L293
//*************************************************************



inline void motorL293Brake(uint nr, int dirpwm) {      // brake by pwm power
   int pwm;
   
   pwm = abs(dirpwm);
   
   digitalWrite(motor[nr].pind1, HIGH);
   digitalWrite(motor[nr].pind2, HIGH);     
   
   motor[nr].dirpwm = pwm;
   softPwmWrite(motor[nr].pinpwm, pwm);    // brake power always > 0   
   
}



inline void motorL293On(uint nr, int dirpwm) { // motor On (nr, dir_pwm)
   int dir, pwm;                             // signed pwm:
   
   if(dirpwm > 0) dir = +1;                   // pwm > 0: forward
   else if(dirpwm < 0) dir = -1;              // pwm < 0: reverse
   else dir = 0;                              // pwm = 0: coast
   
   //if(! _REMOTE_OK_) dirpwm=0;
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



inline void motorL293Coast(uint nr) {
	motorL293On(nr, 0);
}	




//*************************************************************
// motor type MC33926
//*************************************************************

/*
 * This table shows how the drivers’ control inputs affect the motor outputs:
 ---------------------------------------------------------------------------------------
 Inputs                           Outputs
 ---------------------------------------------------------------------------------------
 EN      DIR        PWM        MxA       MxB       operating mode
 ---------------------------------------------------------------------------------------
 1        0         PWM      PWM(H/L)     L        forward/brake at speed PWM %
 1        1         PWM         L      PWM(H/L)    reverse/brake at speed PWM %
 1        X          0          L         L        brake low (outputs shorted to ground)
 0        X          X          Z         Z        coast (outputs off)
 ---------------------------------------------------------------------------------------
 *
 */



inline void motorMC33Brake(uint nr, int dirpwm) {      // brake by pwm power
   int pwm;
   
   pwm = abs(dirpwm);
   
   digitalWrite(motor[nr].pind1, HIGH);
   digitalWrite(motor[nr].pind2, HIGH);     
   
   motor[nr].dirpwm = pwm;
   softPwmWrite(motor[nr].pinpwm, pwm);    // brake power always > 0   
   
}



inline void motorMC33On(uint nr, int dirpwm) { // motor On (nr, dir_pwm)
   int dir, pwm;                             // signed pwm:
   
   if(dirpwm > 0) dir = +1;                   // pwm > 0: forward
   else if(dirpwm < 0) dir = -1;              // pwm < 0: reverse
   else dir = 0;                              // pwm = 0: coast
   
   //if(! _REMOTE_OK_) dirpwm=0;
   pwm = abs(dirpwm);
   softPwmWrite( motor[nr].pinpwm, pwm);
      
   if(dir> 0) {
      digitalWrite( motor[nr].pind1, HIGH);  // forward/brake at speed PWM %
      digitalWrite( motor[nr].pind2, LOW);     
   }
   else
   if(dir==0) {
      digitalWrite( motor[nr].pind1, HIGH);  // brake low (outputs shorted to ground)
      digitalWrite( motor[nr].pind2, LOW);
   }
   else {
      digitalWrite( motor[nr].pind1, HIGH);  // reverse/brake at speed PWM %
      digitalWrite( motor[nr].pind2, HIGH);
   }
    
   motor[nr].dirpwm = dirpwm;
   //softPwmWrite( motor[nr].pinpwm, pwm);
      
}


inline void motorMC33Coast(uint nr) {
	  softPwmWrite( motor[nr].pinpwm, 0);
      digitalWrite( motor[nr].pind1, LOW);  // coast (outputs off)
      digitalWrite( motor[nr].pind2, LOW);
      motor[nr].dirpwm = 0;
      //softPwmWrite( motor[nr].pinpwm, pwm);
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

// 1/4 resolution
// int8_t enctab[16] = {0,0,0,0,0,0,0,-1,0,0,0,0,0,1,0,0}; 

// 1/2 resolution
int8_t enctab[16] = {0, 0,0,0,1,0,0,-1, 0,0,0,1,0,0,-1,0};

// 1/1 resolution
//int8_t enctab[16] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0}; 


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
             

void* thread3Go(void *)   //  high priority encoder task
{
   while(threadrun) {
       updateEncoders();
       usleep(100);
   }         
   return NULL;
}

//*************************************************************

void* thread2Go(void *)  // higher priority motor control task
{
   while(threadrun) {
      
     for(int pwm = -255; pwm < 255; ++pwm ) {
                motorOn(0, pwm); 
                motorOn(1, pwm); 
                if (!threadrun) return NULL;  
                delay(20);
     }
     for(int pwm = 255; pwm > -255; --pwm ) {
                motorOn(0, pwm); 
                motorOn(1, pwm); 
                if (!threadrun) return NULL;
                delay(20);
     }
   }         
   return NULL;
}

//*************************************************************

void* thread1Go(void *)   // medium priority task for keyboard monitoring
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

void* thread0Go(void *)  // low priority display task
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
 
  // motor pins, BCM numbering    
 
 /*  // vOLD: L293
     motor[0].pinQa = 23;   // (BCM 23)  change for rotation direction
     motor[0].pinQb = 24;   // (BCM 24) 
     motor[0].pind1 = 19;   // (BCM 19)
     motor[0].pind2 = 26;   // (BCM 26)
     motor[0].pinpwm= 12;   // (BCM 12)  pwm
     
     motor[1].pinQa = 17;   // (BCM 17)  change for rotation direction
     motor[1].pinQb = 27;   // (BCM 27) 
     motor[1].pind1 =  5;   // (BCM  5)
     motor[1].pind2 =  6;   // (BCM  6)
     motor[1].pinpwm= 13;   // (BCM 13)  pwm   
*/     
     // NEW: MC33296
     motor[0].pinQa = 19;   //  enc Qa
     motor[0].pinQb = 26;   //  enc Qb 
     motor[0].pind1 = 22;   //  en
     motor[0].pind2 = 24;   //  dir
     motor[0].pinpwm= 12;   //  pwm
     
     motor[1].pinQa = 17;   //  enc Qa
     motor[1].pinQb = 27;   //  enc Qb
     motor[1].pind1 = 23;   //  en
     motor[1].pind2 = 25;   //  dir
     motor[1].pinpwm= 13;   //  pwm  
     
     printf("GPIO pin setup: \n");
     for( i=0; i< 2; ++i)  {       
        pinMode( motor[i].pinQa, INPUT);        // encA   
        pinMode( motor[i].pinQb, INPUT);        // encB   
        pinMode( motor[i].pind1, OUTPUT);       // dir-1   
        pinMode( motor[i].pind2, OUTPUT);       // dir-2   
        pinMode( motor[i].pinpwm, PWM_OUTPUT);  // pwm
        
        err= softPwmCreate( motor[i].pinpwm, 0, 255);
        
        printf("mot% -2d (err=%-3d)  qa %-4d qb %-4d d1 %-4d d2 %-4d pwm %-4d \n", 
           i, err, motor[i].pinQa, motor[i].pinQb, motor[i].pind1, motor[i].pind2, motor[i].pinpwm);
       
        motor[i].motenc = 0;
        motor[i].oldenc = 0;
        ISRab[i] = 0;            
     }
   
   
     // Touch Buttons: pressed=LOW, up=HIGH !
     pinMode( 18, INPUT); pullUpDnControl( 18, PUD_UP);
     pinMode( 16, INPUT); pullUpDnControl( 16, PUD_UP);
     pinMode( 20, INPUT); pullUpDnControl( 20, PUD_UP);
     pinMode( 21, INPUT); pullUpDnControl( 21, PUD_UP);
     
     
   printf("\ncheck \nand then press ENTER to start");
   getchar();
}



/*************************************************************
* main
*************************************************************/


int main() {
    char  sbuf[128];
    int iores;
    pthread_t thread0, thread1, thread2, thread3;
    struct  sched_param  param;
   
    putenv ("WIRINGPI_GPIOMEM=1") ;                     // no sudo for gpios required
    iores = wiringPiSetupGpio();                        // init by BCM pin numbering
    if( iores == -1 ) return 1;     
   
    setup();   
   
    pthread_create(&thread0, NULL, thread0Go, NULL);     // lowest priority task: screen output
    param.sched_priority = 10;
    pthread_setschedparam(thread0, SCHED_RR, &param);
   
    pthread_create(&thread1, NULL, thread1Go, NULL);     // medium priority task: keyboard monitoring (stop program)
    param.sched_priority = 25;
    pthread_setschedparam(thread1, SCHED_RR, &param);
   
    pthread_create(&thread2, NULL, thread2Go, NULL);     // higher priority task: !!! motor control program !!!
    param.sched_priority = 50;
    pthread_setschedparam(thread2, SCHED_RR, &param);
   
    pthread_create(&thread3, NULL, thread3Go, NULL);     // highest priority task: encoder reading     
    param.sched_priority = 90;
    pthread_setschedparam(thread3, SCHED_RR, &param);
   
   
    pthread_join(thread0, NULL);
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);
   
    for(int i=0; i< MAXMOTORS; ++i)  {
       motorCoast(i);
    }
   
    delay(1000);
   
    exit(0);
   
}

//*************************************************************

/*

planned enhancements

                // PID custom target values
       int32_t  target;                  // set target
       int16_t  tarspeed;                // motor target speed
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
