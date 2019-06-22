/*
 *  LED on/off by keyb press
 *  v. 002
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termio.h>
#include <wiringPi.h>


#define   PIN_LED   17 // BCM Pin number


#define  msleep(ms)  usleep(1000*ms)



// ********************************************************************


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



// ********************************************************************


int main() {	
    
    int ch;   
    
    setenv("WIRINGPI_GPIOMEM", "1", 1);   // no sudo for gpios required
    wiringPiSetupGpio() ;
    
    pinMode (PIN_LED, OUTPUT) ;
    printf("\npress    0: LED OFF     1: LED ON     [ESC]: quit \n\n");
    
    while(1) {
 
      
      if (kbhit())
      {
         ch = getchar();
         if(ch==27) {                                
            printf("  [ESC]: user break \n");
            return 0; 
         }   
         else
         if(ch=='1') {
            digitalWrite (PIN_LED, HIGH) ;
            printf("   LED ON \n");
         }
         else
         if(ch=='0') {
            digitalWrite (PIN_LED, LOW) ;
            printf("   LED OFF \n");
         }
         else
         printf("\n");
     
      }   
      msleep(10);     
    }
 

	return 0;
    
}

