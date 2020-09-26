/*
 * 
 * 
 * OLED Display
 * plus wringPi  millis + delay
 *
 * http://hallard.me/adafruit-oled-display-driver-for-pi/
 * https://github.com/hallard/ArduiPi_OLED
 *  
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

#include <stdbool.h>
#include <string.h>
#include <termio.h>

#include <wiringPi.h>
#include <wiringPiI2C.h>

#include <ArduiPi_OLED_lib.h>
#include <Adafruit_GFX.h>
#include <ArduiPi_OLED.h>


#define  byte  uint8_t
 

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


//=====================================================================================



int main() {

   int check;
   char sbuf[100];   
   uint32_t msec, millisav;
   
   // wiringPi   
   setenv("WIRINGPI_GPIOMEM", "1", 1);       // no sudo for gpios required
   check = wiringPiSetup();                  // init by wiringPi pin numbering
   if( check == -1 ) return 1;     
   
   // Oled supported display in ArduiPi_SSD1306.h
   // change parameters to fit to your LCD
   if ( !oledtft.init(OLED_I2C_RESET, 6) )
     exit(EXIT_FAILURE);

   oledtft.begin();
	
   // init done
  
   oled_cls();

   // test
 
   oledtft.setTextSize(1);
   oledtft.setTextColor(WHITE);
   oledtft.setTextSize(1); 
   
   oledtft.setCursor(0, 0);  oledtft.print(" 0 Hello, world!\n");
   oledtft.setCursor(0, 8);  oledtft.print(" 8 i2c OLED");  
   oledtft.setCursor(0,16);  oledtft.print("16 128x64 addr 0x3c");
   oledtft.setCursor(0,24);  oledtft.print("24 8x21 char size=1");
   oledtft.setCursor(0,32);  oledtft.print("32 ");
   oledtft.setCursor(0,40);  oledtft.print("40 ");
   oledtft.setCursor(0,48);  oledtft.print("48 456789112345678921");
   oledtft.setCursor(0,56);  oledtft.print("56 ");
   oledtft.display();
   sleep(1);
   oled_cls();


   millisav=millis();
   while(1) {
     msec = millis() - millisav;
     
     sprintf(sbuf, " millisec = %ld \n", msec);        
     printf(sbuf);     
     oledtft.setTextColor(WHITE, BLACK);
     oled_printxy( 0,16, sbuf);
     
     if (kbhit())
     {
         int c = getchar();
         if(c==27) break;
     }   
     delay(500);   
   }
   
   oled_cls();
   return (0);
}
//=====================================================================================
//=====================================================================================
