/*
 * 
 * CMPS11 IMU
 * 3D gyro + 3D compass + 3D acceleromter
 * 
 * test demo
 * ver 0001
 * 
 */

#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include <wiringPi.h>
#include <wiringPiI2C.h>
 
// CMPS11 IMU
#define CMPS11_ADDR 0x60
int    cmps11;


int main() {
   uint8_t  ver, high_byte, low_byte, angle8;
   int8_t   pitch, roll;
   int16_t  angle16;
   
   char sbuf[100];
	
   cmps11 = wiringPiI2CSetupInterface( "/dev/i2c-1",  CMPS11_ADDR );	 
   ver       = wiringPiI2CReadReg8 (cmps11, 0) ;
   


   sprintf(sbuf, "\n  CMPS11 - fw version: %3d \n\n", ver);    //
   printf(sbuf);
 
   while(1) {
				
     angle8    = wiringPiI2CReadReg8 (cmps11, 1) ;
     high_byte = wiringPiI2CReadReg8 (cmps11, 2) ;
     low_byte  = wiringPiI2CReadReg8 (cmps11, 3) ;
     pitch     = wiringPiI2CReadReg8 (cmps11, 4) ;
     roll      = wiringPiI2CReadReg8 (cmps11, 5 ) ;
     
     angle16 = high_byte;                 // Calculate 16 bit angle
     angle16 <<= 8;
     angle16 += low_byte;
       
     sprintf(sbuf, "roll: %3d ", roll);    // Display roll data
     printf(sbuf);
    
     sprintf(sbuf, "    pitch: %3d ", pitch);      // Display pitch data
     printf(sbuf);
    
     sprintf(sbuf, "    angle full: %d.%d ", angle16/10, angle16%10);     // Display 16 bit angle with decimal place
     printf(sbuf);
 
     sprintf(sbuf, "    angle 8: %3d ", angle8);      // Display 8bit angle
     printf(sbuf);
     printf("\n");
     printf("\n");
     
     delay(100);                           // Short delay before next loop
	    	 
  }

  return (0);
}
