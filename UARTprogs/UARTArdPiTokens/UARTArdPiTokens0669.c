
/*
   UART communication
   send/receive string of tokens
   *     
   Raspberry Pi  master
   ver 0669
 */
 
 /*
  * change log
  * 0669:
  * UART 115200 baud
  * 
  * ver 0667:
  * Arduino via USB = ACM0
  * 
  */
 
// (C) dsyleixa 2015
// freie Verwendung für private Zwecke
// für kommerzielle Zwecke nur nach Genehmigung durch den Autor.
// Programming language: gcc C/C++
// protected under the friendly Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License
// http://creativecommons.org/licenses/by-nc-sa/3.0/



#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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

char   * uart  =  "/dev/ttyACM0";
int    Serial;

//==================================================================

const uint32_t UARTclock = 115200;






//==================================================================
// debug monitor

void displayvalues(char * caption, uint8_t array[]) {
  int cnt;
  char sbuf[128];
 
  sprintf(sbuf, "%s ", caption);
  printf(sbuf); printf("\n");
  for(cnt=0; cnt<8; ++cnt) {
    sprintf(sbuf, "%3d ", array[cnt]);      // print on TFT
    printf(sbuf);
  }   
  printf("\n");
 
}

//==================================================================
//==================================================================
// serial TCP


const   uint8_t  MSGSIZE=64;
const   uint8_t  bsync=255;
uint8_t  sendbuf[MSGSIZE];
uint8_t  recvbuf[MSGSIZE];


uint8_t calcchecksum(uint8_t array[]) {
  int32_t  sum=0;
  for(int i=2; i<MSGSIZE; ++i) sum+=(array[i]);
  return (sum & 0x00ff);
}

bool checksumOK(uint8_t array[]){
   return (calcchecksum(array)==array[1]);
}


//==================================================================


void loop()
{
  char     sbuf[128], resOK;
  int8_t   c;   
  uint8_t  cbuf[MSGSIZE];
  int      ibuf;

 
  //   send to slave
 
  //Serial.println();
  sendbuf[0]=bsync;
  sendbuf[1]=calcchecksum(sendbuf);
 
  for(uint8_t i=0; i<MSGSIZE; i++) {                     //
     serialPutchar( Serial, sendbuf[i]);                // Send values to the slave       
  }       
  //serialFlush( Serial );                              // clear output buffer
 
  sprintf(sbuf, "send : %4d %4d     ", sendbuf[BYTE3], sendbuf[BYTE4]);
  printf(sbuf);

 
  //     Receive from slave

  memset(cbuf, 0, sizeof(cbuf));  

  c=0;
  
  while(serialDataAvail( Serial ) < MSGSIZE);
  for(int i=0; i<MSGSIZE; i++) {
      ibuf=serialGetchar( Serial );
      if(i==0 && ibuf!=bsync)
        { c=-1; serialFlush( Serial ); break; }
      else if(ibuf>=0) 
        { cbuf[i]=(uint8_t)ibuf; }
      else 
        { c=-1; serialFlush( Serial ); break; }
  }

   
  resOK =  (c>=0 && cbuf[0]==bsync && checksumOK(cbuf) ) ;
 
  if( resOK ) {                         //  receive ok?

     memcpy(recvbuf, cbuf, sizeof(cbuf));
     
     // debug
     sprintf(sbuf, "received: %4d(...+10) %4d(...+20)   ", recvbuf[BYTE3], recvbuf[BYTE4]);
     printf(sbuf);
 
      memset(sendbuf, 0, sizeof(sendbuf));    // clear send buf
     // debug: test values to send back!
      sendbuf[BYTE3]=recvbuf[BYTE3]+10;                      // change [BYTE3] to send back
      sendbuf[BYTE4]=recvbuf[BYTE4]+20;                      // change [BYTE4] to send back
       
  }
  else printf("receive error \n");
  printf("\n");
 
}


//==================================================================

 
int main() {
     
    printf("initializing..."); printf("\n");
   
    // UART Serial com port
    Serial = serialOpen (uart, UARTclock);  
    
    memset(sendbuf, 0, sizeof(sendbuf)); 
    while(1) { loop(); }
   
    serialClose( Serial);
   

    exit(0);
}


