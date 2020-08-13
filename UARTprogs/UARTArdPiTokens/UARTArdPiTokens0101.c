
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
    
  while(1) {  
      
     static bool stringComplete = false;
     
     
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
     delay(10); 
   
     
     
     
     //-------------------------------------------------------------
     // receive
     
     inputString="";
     char cstr[TOKLEN];
   
     int  n=0;
     char inChar;     
     
     
     while (serialDataAvail(Serial) && n<MSGLEN-1 ) { 
       if(n==MSGLEN-2) inChar='\n'; // emergency brake
       else  
         inChar = serialGetchar(Serial); 
    
       if(inChar=='\n' || inChar>=' ') inputString += inChar;       
       if (inChar == '\n')  {
         stringComplete = true;
       }
       n++;
     }
        
     if (stringComplete)  {         

       strcpy (mbuf, inputString.c_str() );

       fprintf(stderr,mbuf); fprintf(stderr,"\n"); 
       
       // process mbuf !
   
       
       inputString="";
       stringComplete = false;
   
       //delay?
       delay(10);
     }
   
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

