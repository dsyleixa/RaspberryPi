#include <iostream>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>


void getdatetime(int &year, int &mon, int &day, int &hour, int &min, int &sec, int &dow) {
   time_t t = time(NULL);
   struct tm  tms = * localtime(&t);	
   
   year = tms.tm_year+1900; 
   mon  = tms.tm_mon+1;
   day  = tms.tm_mday;
   hour = tms.tm_hour;
   min  = tms.tm_min;
   sec  = tms.tm_sec;
   dow  = tms.tm_wday+1;
   
   //printf("\n%d\n", year);

     	
}


int main()
{

   char sbuf[100];	
   
   int year, mon, day, hour, min, sec, dow;

   
   if (system( "ping -c1 -s1 www.google.de") ) {
	  printf("internet connx failed \n");
   }
   else {
	  printf("internet connx OK ! :) \n");
   }   
   
   getdatetime(year, mon, day, hour, min, sec, dow) ;
   sprintf( sbuf, "y%d m%d d%d h%d m%d s%d dw%d", year, mon, day, hour, min, sec, dow ); 
   printf(sbuf);
   
   getchar();
   return 0;
}




