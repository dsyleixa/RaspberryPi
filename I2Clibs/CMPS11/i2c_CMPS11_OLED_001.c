/*
 * 
 * CMPS11 IMU
 * 3D gyro + 3D compass + 3D acceleromter
 * 
 * test demo
 * ver 0003
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

#include <stdbool.h>
#include <string.h>
#include <termio.h>

#include <wiringPi.h>
#include <wiringPiI2C.h>

#include <ArduiPi_OLED_lib.h>
#include <Adafruit_GFX.h>
#include <ArduiPi_OLED.h>

#define  byte  uint8_t


//----------------------------------------------------------------------
// Instantiate the OLED display

ArduiPi_OLED oledtft;


void oled_cls() {
     oledtft.clearDisplay(); 
     oledtft.display(); 
}


void oled_printxy(int x, int y, char * str) {
     oledtft.setCursor(x,y);
     oledtft.print(str);
     oledtft.display(); 
}


//----------------------------------------------------------------------
// mimic conio.h kbhit

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


//----------------------------------------------------------------------
 
// CMPS11 IMU
#define CMPS11_ADDR 0x60
int    fcmps11;

//----------------------------------------------------------------------


int main() {
   uint8_t  ver, high_byte, low_byte;
   int8_t   pitch, roll;
   int16_t  angle16;
   
   char sbuf[100];
      
   // Oled supported display in ArduiPi_SSD1306.h
   // change parameters to fit to your LCD
   if ( !oledtft.init(OLED_I2C_RESET, 6) )
      exit(EXIT_FAILURE);

   oledtft.begin();
   oled_cls();
   oledtft.setTextSize(1);
   oledtft.setTextColor(WHITE, BLACK);  

   // no sudo for gpios required
   putenv ("WIRINGPI_GPIOMEM=1") ;
   
   // cmps11 IMU i2c setup
   fcmps11 = wiringPiI2CSetupInterface( "/dev/i2c-1", CMPS11_ADDR );	 
   ver     = wiringPiI2CReadReg8 (fcmps11, 0);
   
   sprintf(sbuf, "CMPS11 fw ver:%3d \n\n", ver);     
   printf(sbuf);
   oled_printxy( 0, 0, sbuf);
   

   while(1) {
				
     //angle8    = wiringPiI2CReadReg8 (fcmps11, 1) ;    // 8 bit angle
     high_byte = wiringPiI2CReadReg8 (fcmps11, 2) ;
     low_byte  = wiringPiI2CReadReg8 (fcmps11, 3) ;
     pitch     = wiringPiI2CReadReg8 (fcmps11, 4) ;
     roll      = wiringPiI2CReadReg8 (fcmps11, 5) ;
     
     angle16 = (high_byte<<8) + low_byte;     // Calculate 16 bit angle
       
     sprintf(sbuf, "roll : %3d   ", roll);       // Display roll data
     printf(sbuf);
    
     sprintf(sbuf, "pitch: %3d   ", pitch);      // Display pitch data
     printf(sbuf);
    
     sprintf(sbuf, "angle: %6.1f ", (float)angle16/10.0);     // Display 16 bit angle with decimal place
     printf(sbuf); 
     oled_printxy( 0,24, sbuf);
     
     printf("\n\n");
     
     if (kbhit())
     {         
         int c = getchar();
         if(c==27) break;
     }   
     
     delay(100);                           // Short delay before next loop
	    	 
  }
  
  oled_cls();
  oledtft.close();
   
  return (0);
}
