/*     Tx master  
 *     Raspberry Pi   
       ver 0006
       
 */
 


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


#include <wiringPi.h>
#include <wiringSerial.h>

#define  byte  uint8_t

char   * uart;
int    Serial1;


const    uint8_t  MSGSIZE=64;
uint8_t  bsync=255;
uint8_t  sendbuf[MSGSIZE];
uint8_t  recvbuf[MSGSIZE];



//=====================================================================================
// debug monitor



//=====================================================================================
//=====================================================================================
// serial com
   

uint8_t calcchecksum(uint8_t array[]) {
  int32_t  sum=0;
  for(int i=2; i<MSGSIZE; ++i) sum+=(array[i]);
  return (sum & 0x00ff);
}

#define  checksumOK(array) (calcchecksum(array)==array[1]) 



//=====================================================================================

//=====================================================================================






//==============================================================
void loop()
{ 
  char     sbuf[128],  resOK;   
  static   uint8_t cnt=0; 
  uint8_t  cbuf[MSGSIZE], chk;
  uint8_t  bbuf;
  int i;

 
  //   send to slave
  printf("sending...\n");
 
  sendbuf[0]=bsync;
  sendbuf[1]=calcchecksum(sendbuf);
  
  for(uint8_t i=0; i<MSGSIZE; i++) {                     // better use write() ?
     serialPutchar( Serial1, sendbuf[i]);                // Send values to the Rx Arduino       
  }       
   
  sprintf(sbuf, "%4d %4d", sendbuf[4], sendbuf[6]);
  
  printf(sbuf); printf("\n");

 
  
  //-------------------------------------------------------
  //     Receive from slave 

  memset(cbuf, 0, sizeof(cbuf)); 
   
  printf("receiving...\n");
  if (serialDataAvail(Serial1) ) {
      i=0;
      while(serialDataAvail(Serial1) && i<MSGSIZE) {
          cbuf[i] = serialGetchar(Serial1);
          printf("%3d ", cbuf[i]);
          i++;
      }
      //while(serialDataAvail(uart) {bbuf= serialGetchar(uart);} // clear buf
  }
  
  resOK=checksumOK(cbuf);
  printf("nrec=%d  resOK=%d \n", i, resOK);
      
 
  if( resOK ) {                         // byte 0 == syncbyte ?
    cnt=0;

 
     chk=(byte)calcchecksum(cbuf);     
     memcpy(recvbuf, cbuf, sizeof(cbuf));
 
        // change values to send back!
        memcpy(sendbuf, recvbuf, sizeof(sendbuf));         // copy inbuf to outbuf
        sendbuf[4]+=1;                                     // change [4] to send back
       
       
  } 
}


//=====================================================================================

 
int main() {
    unsigned long timesav;
    char  sbuf[128];
    char  instr[128], bbuf;
     
    printf("initializing..."); printf("\n"); 
    
    // UART Serial com port 
    uart= "/dev/ttyAMA0";
    Serial1 = serialOpen (uart, 115200); // for Arduino code compatibility reasons 
     while(serialDataAvail(Serial1)) {bbuf= serialGetchar(Serial1);} // clear buf
    
    while(1) { loop(); } 
   
    exit(0);
}

//=====================================================================================
//=====================================================================================
