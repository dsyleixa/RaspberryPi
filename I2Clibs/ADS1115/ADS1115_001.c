// ads1115 example wiringPi
 

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>

#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <ads1115.h>


#define  PINBASE  120    // choose  PINBASE  value arbitrarily

#define  ADS_ADDR 0x48



int main() {
   int analog0,analog1,analog2,analog3; 
  
   int ioerr = wiringPiSetupGpio();   
     if( ioerr == -1 ) return 1;  
    
   ads1115Setup (PINBASE, ADS_ADDR);    
    
   while(true) {        
        analog0 = analogRead(PINBASE+0);      // connected to ads1115 A0
          printf("analog0 = %d \n", analog0);
	    analog1 = analogRead(PINBASE+1);    // connected to ads1115 A1
          printf("analog1 = %d \n", analog1);
        analog2 = analogRead(PINBASE+2);    // connected to ads1115 A2
          printf("analog2 = %d \n", analog2);
        analog3 = analogRead(PINBASE+3);    // connected to ads1115 A3
          printf("analog3 = %d \n", analog3);
       
       delay(250);  // sample rate 250ms
   }
       
   
     
  
  return 0;
  }
