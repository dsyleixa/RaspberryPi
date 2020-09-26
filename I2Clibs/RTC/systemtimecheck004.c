#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>


void getdatetime(int &year, int &mon, int &day, int &hour, int &min, int &sec, int &dow) {
   time_t t = time(NULL);
   struct tm  tm = * localtime(&t);	
   
   year = (int)tm.tm_year+1900-2000, 
   mon  = (int)tm.tm_mon+1;
   day  = (int)tm.tm_mday;
   hour = (int)tm.tm_hour;
   min  = (int)tm.tm_min;
   sec  = (int)tm.tm_sec;
   dow  = (int)tm.tm_wday+1;

     	
}


int main()
{

   char sbuf[100];	
   
   int year, mon, day, hour, min, sec, dow;
   
   sleep(1);
   system("wmctrl -r LXTerminal -e 0,200,200,512,640");

   
   if (system( "ping -c1 -s1 www.google.de") ) {
	  printf("internet connx failed \n");
   }
   else {
	  printf("internet connx OK ! :) \n");
   }   
   
   getdatetime(year, mon, day, hour, min, sec, dow) ;
   sprintf( sbuf, "%d %d %d %d %d %d %d", year, mon, day, hour, min, sec, dow ); 
   printf(sbuf);
   
   getchar();
   return 0;
}




