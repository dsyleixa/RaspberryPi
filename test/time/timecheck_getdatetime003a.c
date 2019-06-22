
#include <time.h>
#include <stdio.h>

void getdatetime(int &year, int &mon, int &day, 
                 int &hour, int &min, int &sec, int &msec, int &dow) 
{
   struct timespec now;
   clock_gettime( CLOCK_REALTIME, &now );
   

   const struct tm *tms = localtime( &now.tv_sec );
 
   year = tms->tm_year+1900; 
   mon  = tms->tm_mon+1;
   day  = tms->tm_mday;
   hour = tms->tm_hour;
   min  = tms->tm_min;
   sec  = tms->tm_sec;
   dow  = tms->tm_wday+1;

   msec = (int)now.tv_nsec / 1000000;
}


int main()
{
   char sbuf[100];	   
   int year, mon, day, hour, min, sec, ms, dow;

   getdatetime(year, mon, day, hour, min, sec, ms, dow);

   sprintf( sbuf, "y%d m%d d%d h%d m%d s%d ms%d dw%d", 
            year, mon, day, hour, min, sec, ms, dow ); 
   printf(sbuf);
   
   getchar();
   return 0;
}

 
