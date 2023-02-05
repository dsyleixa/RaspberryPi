// encoder test
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <errno.h>

#include <termios.h>

#include "VG/openvg.h"
#include "VG/vgu.h"
#include "fontinfo.h"
#include "shapes.h"

#include <wiringPi.h>
#include <wiringSerial.h>

#define  byte  uint8_t;

#define  MAXMOTORS           2 // max number of encoder motors at Arduino Uno=2 // Due=6 // Mega=8


// motor 0
#define pinenc0A   4  // enc0A yellow
#define pinenc0B   5  // enc0B blue
#define pinmot0d1  24  // dir0-1   <<
#define pinmot0d2  25  // dir0-2
#define pinmot0pwm 10  // pwm enable0   

// motor 1
#define pinenc1A   6  // enc1A yellow
#define pinenc1B   7  // enc1B blue
#define pinmot1d1  28  // dir1-1   <<
#define pinmot1d2  29  // dir1-2
#define pinmot1pwm  9  // pwm enable1   


volatile long   motenc[MAXMOTORS], 
                oldenc[MAXMOTORS] ;
               
int pinmotdir[MAXMOTORS][ 2] = {
  {pinmot0d1, pinmot0d2} ,  // motor direction pin array
  {pinmot1d1, pinmot1d2}
};

int  pinmotpwm[MAXMOTORS] =      {pinmot0pwm, pinmot1pwm };

volatile int8_t ISRab[MAXMOTORS];

// 1/4 resolution
// int8_t enctab[16] = {0,0,0,0,0,0,0,-1,0,0,0,0,0,1,0,0}; 

// 1/2 resolution
int8_t enctab[16] = {0, 0,0,0,1,0,0,-1, 0,0,0,1,0,0,-1,0};

// 1/1 resolution
//int8_t enctab[16] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0}; 

                              
                                 
/*************************************************************
* Interrupt Handler Routine
*************************************************************/


void setup() {
  int i;	
	
  // motor pin settings
  // setup for L293D motor driver 
  // pullUpDnControl (butpin, PUD_UP);
  
  
  // motor 0
     pinMode(pinenc0A, INPUT);          // enc0A    yellow
     pullUpDnControl(pinenc0A, PUD_UP);    
     pinMode(pinenc0B, INPUT);          // enc0B    blue
     pullUpDnControl(pinenc0B, PUD_UP); 
     pinMode(pinmot0d1, OUTPUT);        // dir0-1   
     pinMode(pinmot0d2, OUTPUT);        // dir0-2   
     pinMode(pinmot0pwm ,OUTPUT);       // enable0 
       
  // motor 1
     pinMode(pinenc1A, INPUT);          // enc0A    yellow
     pullUpDnControl(pinenc1A, PUD_UP);    
     pinMode(pinenc1B, INPUT);          // enc0B    blue
     pullUpDnControl(pinenc1B, PUD_UP); 
     pinMode(pinmot1d1, OUTPUT);        // dir1-1   
     pinMode(pinmot1d2, OUTPUT);        // dir1-2 
     pinMode(pinmot1pwm, OUTPUT);       // enable1   

     for( i=0; i< MAXMOTORS; ++i) motenc[i] = 0;
     for( i=0; i< MAXMOTORS; ++i) ISRab[i] = 0;

}


void loop() {
  char sbuf[128];	 
	 
  while(true) {  
	 
    ISRab [ 0] <<= 2;
    ISRab [ 0] &=  0b00001100;
    ISRab [ 0] |= (digitalRead(pinenc0A) << 1) | digitalRead(pinenc0B);
    motenc[ 0] += enctab[ISRab[0]];          

    ISRab [ 1] <<= 2;
    ISRab [ 1] &= 0b00001100;
    ISRab [ 1] |= (digitalRead(pinenc1A) << 1) | digitalRead(pinenc1B);
    motenc[ 1] += enctab[ISRab[1]];           

	  
    sprintf(sbuf, " 0=%6ld, \n ",  motenc[ 0]);
    printf(sbuf);
    delay(1);
  }
}


int main() {
    char  sbuf[128];
    
    wiringPiSetup();
    if(wiringPiSetup() == -1) return 1;
     
    setup();
    loop();
    
}
     


