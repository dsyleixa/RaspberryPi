
/*
   UART communication
   send/receive byte array (64 bytes)
   *     
   Raspberry Pi  master
   ver 0668
 */
 
 /*
  * change log
  * 0668:
  * UART 500000 baud
  * 
  * ver 0667:
  * Arduino via USB = ACM0
  * 
  */
 
// (C) Helmut Wunder (HaWe) 2015
// freie Verwendung f�r private Zwecke
// f�r kommerzielle Zwecke nur nach Genehmigung durch den Autor.
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

const uint32_t UARTclock = 500000;


//==================================================================
// comm msg array
//==================================================================

#define SYNCSLOT      0  // start sync signal of this Msg: bsync=0xff (255)
#define CKSSLOT       1  // chksum this Msg
#define BYTE0         2  // byte 0     // byte: 8-bit => 8 digital bits 0-7
#define BYTE1         3  // byte 1     // byte: 8-bit => 8 digital bits 8-15
#define ENC0          4  // motorenc 0 // 10 encoders: 32-bit
#define ENC1          8  // motorenc 1
#define ENC2         12  // motorenc 2
#define ENC3         16  // motorenc 3
#define ENC4         20  // motorenc 4
#define ENC5         24  // motorenc 5
#define ENC6         28  // motorenc 6
#define ENC7         32  // motorenc 7
#define ENC8         36  // motorenc 8
#define ENC9         40  // motorenc 9
#define ANA0         44  // analog 0   // 9 analog: 16-bit
#define ANA1         46  // analog 1   // analog 0+1 = joystick for drive
#define ANA2         48  // analog 2
#define ANA3         50  // analog 3
#define ANA4         52  // analog 4
#define ANA5         54  // analog 5
#define ANA6         56  // analog 6   // opt byte 9+8
#define ANA7         58  // analog 7   // opt byte 7+6
#define ANA8         60  // analog 8   // opt byte 5+4
#define BYTE2        62  // byte 2     // byte: 8-bit => 8 digital bits 16-23
#define TERM         63  // byte 3, terminating, heart beat signal 


// optional:
#define LONG0         4  // long 0      // 3x long: 32-bit/4byte
#define LONG1         8  // long 1
#define LONG2        12  // long 2
#define DOUBL0       16  // double0     // 2x double 64bit/8byte
#define DOUBL1       24  // double1
#define FLOAT0       32  // float0      // 3x float 32bit/4byte
#define FLOAT1       36  // float1
#define FLOAT2       40  // float2


// alias 
#define BYTE3        63    // byte 3    
#define BYTE4        61    // byte 4  
#define BYTE5        60    // byte 5
#define BYTE6        59    // byte 6 
#define BYTE7        58    // byte 7 
#define BYTE8        57    // byte 6 
#define BYTE9        56    // byte 7 



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
      if(ibuf>=0) 
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


