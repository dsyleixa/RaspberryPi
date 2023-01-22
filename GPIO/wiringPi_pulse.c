#include <stdio.h>
#include <inttypes.h>
#include <wiringPi.h>
#include <softPwm.h>

int main() {
   uint32_t start;
   int dc;
   int iores = wiringPiSetupGpio();// init by BCM pin numbering
   if( iores == -1 )
        return 1;
        
   pinMode(18, OUTPUT);
   softPwmCreate(18, 0, 255);
   
   start = millis();

   dc = 25;    

   while((millis()-start) < 20000)
   {
      softPwmWrite(18, dc);  //  
      delay(500);
      softPwmWrite(18, 0);
      delay(500);  
       
      // debug:
      softPwmWrite(18, 255); 
      delay(500);
      softPwmWrite(18, 0);  // digitalWrite don't work then here
      delay(500);   
       
   }
   
   return 0;
   
}
