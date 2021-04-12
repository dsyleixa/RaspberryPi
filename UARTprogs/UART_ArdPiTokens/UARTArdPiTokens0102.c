
/*
   UART communication
   send/receive string of tokens
   *     
   Raspberry Pi  master
   ver 0101
 */



#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <math.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include <pthread.h>


#include <wiringPi.h>
#include <wiringSerial.h>

#define  byte  uint8_t

char   uart[256]  =  "/dev/ttyACM0";
int    Serial;


//==================================================================

const uint32_t UARTclock = 115200;



int i0;

#define TOKLEN 30
#define MSGLEN 1024
#define iINVALID -29999


std::string  inputString="";
char   cval[TOKLEN];  // number as cstring
char   mbuf[MSGLEN];  // cstring msg buffer





//==================================================================
// serial TCP
//==================================================================


void loop() {
  static int32_t count=0;
    
  while(1) 
  {  
        
     //-------------------------------------------------------------
     // send    
  
     char formatstr[MSGLEN];     
     // debug, cut-down:
     strcpy(formatstr, "§");
     strcat(formatstr, "message from Raspi: %d;\n");
     sprintf(mbuf, formatstr, i0);
     for(uint8_t i=0; i<strlen(mbuf); i++) {           //
        serialPutchar( Serial, mbuf[i]);               // Send values to the slave       
     }   
     
     //delay?
     //delay(1);
     // Suspect you had to have the delay is because it dropped straight
     // thru the next section because your loop had serialDataAvail(Serial)
     // If the other end hasn't sent the response it would drop straight thru 
      
     //-------------------------------------------------------------
     // receive
     
     inputString="";
     char cstr[TOKLEN];
   
     int  n=0;
     char inChar = 0;
     
     unsigned long timeout = millis() + 1000;   /* 1 second in the future */
     while (inChar != '\n' && n < MSGLEN-2
	        && millis() < timeout) 
     { 
	   inChar = serialGetchar( Serial ); 
           inputString += inChar;   
           n++;
     }
   
     if (inChar == '\n')  // terminated correctly  
     {
        strcpy (mbuf, inputString.c_str() );
        if(count % 1000 == 0) {
            fprintf(stderr,mbuf); fprintf(stderr,"\n");    
        }
        // process mbuf !
        inputString="";
        
     } else {
	     if (n == MSGLEN-1)
		 fprintf(stderr,"Buffer error %i characters sent\n", n);
                else fprintf(stderr, "Timeout error no data for 1 sec\n", n);
	   }
       
       count++;
    }     
     
   
}




//==================================================================

 
int main() {
     
    printf("initializing..."); printf("\n");
   
    // UART Serial com port
    Serial = serialOpen (uart, UARTclock);   
    printf("starting UART loop..."); printf("\n");   
    
    loop(); 
   
    serialClose( Serial);
    exit(0);
}



//
// eof

