#include <stdio.h>
#include <pigpio.h>

int main() {
   double start;
   int dc;
    
   if(gpioInitialise()<0) {
        fprintf(stderr, "pigpio init failed\n");
   }
   gpioSetMode(18, PI_OUTPUT);
   
   start =time_time();

   dc = 25;    

   while((time_time()-start) < 20.0)
   {
      gpioPWM(18, dc);
      time_sleep(0.5);
      gpioWrite(18, 0);
      time_sleep(0.5);   
      // debug:
      gpioPWM(18, 255);
      time_sleep(0.5);
      gpioWrite(18, 0);
      time_sleep(0.5);    
   }
   
   return 0;
   
}
