/*
 * 
 * RTC DS3231
 * plus OLED oledtft
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

#include <wiringPi.h>
#include <wiringPiI2C.h>

#include <ArduiPi_OLED_lib.h>
#include <Adafruit_GFX.h>
#include <ArduiPi_OLED.h>

#include <getopt.h>


#define  byte  uint8_t
 
// RTC DS3231
#define ADDR_RTCDS3231 0x68
int     fRTCds3231;

// Instantiate the OLED oledtft
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


void oled_clrlnxy(int x, int y) {
     oledtft.setTextColor(BLACK, BLACK); 
     oled_printxy( 0, 8, "                     ");
     oledtft.setTextColor(WHITE, BLACK);  
}


//=====================================================================================
// Convert normal decimal numbers to binary coded decimal
//=====================================================================================
byte decToBcd(byte val) {  return( (val/10*16) + (val%10) );  }


//=====================================================================================
// Convert binary coded decimal to normal decimal numbers
//=====================================================================================
byte bcdToDec(byte val)  {  return( (val/16*10) + (val%16) ); }


//=====================================================================================




void setDS3231time( byte year, byte month, byte day, 
     byte hour, byte minute, byte second, byte dow)
{
     // sets time and date data to DS3231  
   
     wiringPiI2CWriteReg8(fRTCds3231, 0, decToBcd(second));      // set seconds
     wiringPiI2CWriteReg8(fRTCds3231, 1, decToBcd(minute));      // set minutes
     wiringPiI2CWriteReg8(fRTCds3231, 2, decToBcd(hour));        // set hours
     wiringPiI2CWriteReg8(fRTCds3231, 3, decToBcd(dow));         // ( 1=Sunday, 7=Saturday)
     wiringPiI2CWriteReg8(fRTCds3231, 4, decToBcd(day));         // set dayOfMonth (1 to 31)
     wiringPiI2CWriteReg8(fRTCds3231, 5, decToBcd(month));       // set month
     wiringPiI2CWriteReg8(fRTCds3231, 6, decToBcd(year));        // set year (0 to 99)
   
}


void getDS3231time( int &year, int &month, int &day, 
     int &hour, int &minute, int &second, int &dow)
{
     second =     bcdToDec(wiringPiI2CReadReg8 (fRTCds3231, 0) & 0x7f );
     minute =     bcdToDec(wiringPiI2CReadReg8 (fRTCds3231, 1) );
     hour =       bcdToDec(wiringPiI2CReadReg8 (fRTCds3231, 2) & 0x3f );
     dow =        bcdToDec(wiringPiI2CReadReg8 (fRTCds3231, 3) );
     day =        bcdToDec(wiringPiI2CReadReg8 (fRTCds3231, 4) );
     month =      bcdToDec(wiringPiI2CReadReg8 (fRTCds3231, 5) );
     year =       bcdToDec(wiringPiI2CReadReg8 (fRTCds3231, 6) );   
}


  char * sdow (int dow) {	 
	 switch(dow){     
     case 1:
       return("Sunday");
       break;
     case 2:
       return("Monday");
       break;
     case 3:
       return("Tuesday");
       break;
     case 4:
       return("Wednesday");
       break;
     case 5:
       return("Thursday");
       break;
     case 6:
       return("Friday");
       break;
     case 7:
       return("Saturday");
       break;
     } 
     return ("error");
	  
  }

//=====================================================================================



int main() {
   static int  year, month, day, hour, minute, second, dow;   
   int i=0, check;
   char sbuf[100];   
   
   // Oled supported oledtft in ArduiPi_SSD1306.h

   // I2C change parameters to fit to your LCD
   if ( !oledtft.init(OLED_I2C_RESET, 6) )
     exit(EXIT_FAILURE);

   oledtft.begin();
	
   // init done
  
   oledtft.clearDisplay();   // clears the screen  buffer
   oledtft.display();   		// oledtft it (clear oledtft) 
 
   oledtft.clearDisplay();

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
   //oledtft.setTextColor(BLACK, WHITE); // 'inverted' text
   oledtft.display();
   sleep(1);
   oled_cls();


   // fRTCds3231 = wiringPiI2CSetupInterface( "/dev/i2c-0",  ADDR_RTCDS3231 );	
   fRTCds3231 = wiringPiI2CSetupInterface( "/dev/i2c-1",  ADDR_RTCDS3231 );   
   
   strcpy(sbuf, "RTC DS3231 \n\n");
   printf(sbuf); 
   oled_printxy( 0, 0, sbuf);
   getDS3231time( year, month, day, hour, minute, second, dow); 
   sprintf(sbuf, "20%02d/%02d/%02d  %02d:%02d:%02d", year, month, day, hour, minute, second); 
   printf(sbuf); 
   oled_printxy( 0, 8, sbuf);    
   sprintf(sbuf, " Day of week %1d: ", dow); 
   printf(sbuf);  
   printf(sdow(dow) );
   printf("\n\n");	
   
   printf("***  Set Date/Time: enter 1      *** \n");
   printf("***  else:          oledtft time *** \n\n");
   oled_printxy( 0,24, "Set DateTime enter 1");
   oled_printxy( 0,32, "else:   oledtft time");

   i = getchar();
   
   //debug
   //printf("%d \n", i);
   
   while (i=='1') {
      // get string yy mm dd hh mm ss dw : gets() ?
      printf("yy mm dd hh mm ss dw (DayOfWeek) \n");
      check=scanf("%d %d %d %d %d %d %d", &year, &month, &day,  &hour, &minute, &second, &dow);
      
      getchar();
      printf("check=%d\n", check);
      
      if(check==7) {     
	     sprintf(sbuf, "20%02d/%02d/%02d  %02d:%02d:%02d", year, month, day, hour, minute, second);  
         printf(sbuf);     
         oled_printxy( 0, 8, sbuf); 
         printf(" Day of week %1d: ", dow);     
         printf(sdow(dow) );
         printf("\n");
         setDS3231time( year, month, day, hour, minute, second, dow );
      }             
       
      oled_cls();
       
      printf("RTC DS3231 \n");
      oled_printxy( 0, 0, sbuf); 
      printf("***  Set Date/Time: enter 1      *** \n");
      printf("***  else:          oledtft time *** \n\n");
      oled_printxy( 0,24, "Set DateTime enter 1");
      oled_printxy( 0,32, "else:   oledtft time");
      i=0;
      i = getchar();         
            
   }
   
   oled_cls();
   oled_printxy( 0, 0, "RTC DS3231 \n"); 
   
   while(1) {
     getDS3231time( year, month, day, hour, minute, second, dow);
     sprintf(sbuf, "20%02d/%02d/%02d  %02d:%02d:%02d", year, month, day, hour, minute, second);        
     printf(sbuf);     
     oled_clrlnxy( 0, 8); 
     oled_printxy( 0, 8, sbuf);
     printf(" Day of week %1d: ", dow);     
     printf(sdow(dow) );
     printf("\n");		
     
     delay (1000);	  	    	 
   }
   
   // Free PI GPIO ports
   display.close();

   return (0);
}
//=====================================================================================
//=====================================================================================
