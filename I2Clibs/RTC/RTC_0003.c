/*
 * 
 * RTC DS3231
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

#include <wiringPi.h>
#include <wiringPiI2C.h>

#define  byte  uint8_t
 
// RTC DS3231
#define ADDR_RTCDS3231 0x68
int     fRTCds3231;



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

   // fRTCds3231 = wiringPiI2CSetupInterface( "/dev/i2c-0",  ADDR_RTCDS3231 );	
   fRTCds3231 = wiringPiI2CSetupInterface( "/dev/i2c-1",  ADDR_RTCDS3231 );   
   
   printf("RTC DS3231 \n\n");
   getDS3231time( year, month, day, hour, minute, second, dow);
   sprintf(sbuf, "20%02d/%02d/%02d  %02d:%02d:%02d", year, month, day, hour, minute, second); 
   printf(sbuf);     
   printf(" Day of week %1d: ", dow);     
   printf(sdow(dow) );
   printf("\n\n");	
   
   printf("***  Set Date/Time: enter 1      *** \n");
   printf("***  else:          display time *** \n\n");

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
         printf(" Day of week %1d: ", dow);     
         printf(sdow(dow) );
         printf("\n");
         setDS3231time( year, month, day, hour, minute, second, dow );
      }             
       
      printf("RTC DS3231 \n");
      printf("***  Set Date/Time: enter 1      *** \n");
      printf("***  else:          display time *** \n\n");
      i=0;
      i = getchar();         
            
   }
   

 
   while(1) {
     getDS3231time( year, month, day, hour, minute, second, dow);
     sprintf(sbuf, "20%02d/%02d/%02d  %02d:%02d:%02d", year, month, day, hour, minute, second); 
     printf(sbuf);     
     printf(" Day of week %1d: ", dow);     
     printf(sdow(dow) );
     printf("\n");		
     
     delay (1000);	  	    	 
   }

   return (0);
}
//=====================================================================================
//=====================================================================================
