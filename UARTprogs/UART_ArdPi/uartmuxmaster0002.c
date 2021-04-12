
/* 
   UART communication
   send/receive byte array (64 bytes)
   *     
   Raspberry Pi  master
   ver 666nrm
 
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
#include <pthread.h>


#include <wiringPi.h>
#include <wiringSerial.h>

#define  byte  uint8_t

char   * uart  =  "/dev/ttyAMA0";
int    Serial1;

 
 

//******************************************************
// uart comm array
//******************************************************

#define SYNCSLOT      0  // start sync signal of this Msg: bsync  (e.g. 0xff/255)
#define CKSSLOT       1  // chksum this Msg
#define BYTE0         2  // byte 0      // byte: 8-bit => 8 digital bits 0-7
#define BYTE1         3  // byte 1      // byte: 8-bit => 8 digital bits 8-15
#define INT0          4  // int0        // 3x long: 32-bit/4byte
#define INT1          8  // int1
#define INT2         12  // int2
#define DOUBL0       16  // double0     // 2x double 64bit/8byte
#define DOUBL1       24  // double1
#define FLOAT0       32  // float0      // 3x float 32bit/4byte
#define FLOAT1       36  // float1
#define FLOAT2       40  // float2
#define ANA0         44  // analog 0    // 9x ananog (ADC) 16-bit/2byte
#define ANA1         46  // analog 1    
#define ANA2         48  // analog 2
#define ANA3         50  // analog 3
#define ANA4         52  // analog 4
#define ANA5         54  // analog 5
#define ANA6         56  // analog 6
#define ANA7         58  // analog 7
#define ANA8         60  // analog 8
#define BYTE2        62  // byte 2      //  1 byte custom
#define TERM         63  // terminating //  1 byte custom 

 

//******************************************************
// bit and byte and pin operations
//******************************************************

void double2bytebuf(double fvar, uint8_t * bytearr) {
    
   union {
      double  f;
      uint8_t b8[8];
   }  u;
   
   u.f = fvar;
   memcpy(bytearr, u.b8, 8);
   
}

double  bytebuf2double(uint8_t * bytearr) {    
   double f;
   int i;
   union {
      double  f;
      uint8_t b8[8];
   }  u;
   
   memcpy(u.b8, bytearr, 8); 
   return u.f;   
}




//=====================================================================================
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

//=====================================================================================
//=====================================================================================
// serial TCP


const    uint8_t  MSGSIZE=64;
uint8_t  bsync=255;
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

// ================================================================
// addToBuffer and receive function courtesy of chucktodd

bool addToBuffer( uint8_t buf[], uint8_t *cnt, uint16_t timeout){
bool inSync = *cnt>0;
unsigned long start=millis();
while((*cnt<MSGSIZE)&&(millis()-start<timeout)){
  if( serialDataAvail( Serial1 ) ) { // grab new char, test for sync char, if so start adding to buffer
    buf[*cnt] = (uint8_t)serialGetchar( Serial1 );
    if(inSync) *cnt += 1;  
    else{
     if(buf[*cnt]==0xFF){
       inSync = true;
       *cnt +=1;
       }
     }
    }
  }
return (*cnt==MSGSIZE);
}


//=====================================================================================

bool receive(uint8_t * buf, uint16_t timeout, uint8_t *cnt){ // by passing cnt in and out,
// i can timeout and still save a partial buffer, so a resync costs less (less data lost)

bool inSync=false;
unsigned long start=millis();
uint8_t * p;  // pointer into buf for reSync operation
bool done=false;

do{
  done = addToBuffer(buf,cnt,timeout); // if this return false, a timeout has occured, and the while will exit.
  if(done){                         // do checksumOK test of buffer;
    done=checksumOK(buf);
    if(!done){                      // checksumOK failed, scan buffer for next sync char
       p = (uint8_t*)memchr((buf+1),0xff,(MSGSIZE-1)); 
       
       
       if(p){ // found next sync char, shift buffer content, refill buffer
         *cnt = MSGSIZE -(p-buf); // count of characters to salvage from this failure
         memcpy(buf,p,*cnt); //cnt is now where the next character from Serial is stored!
         }
       else *cnt=0; // whole buffer is garbage
       }
    }
   
  } while(!done&&(millis()-start<timeout));

return done; // if done then buf[] contains a sendbufid buffer, else a timeout occurred
}

//=====================================================================================


void loop()
{
  char     sbuf[128],  resOK;   
  static   uint8_t cnt=0;
  uint8_t  cbuf[MSGSIZE], chk;

 
  //   send to Rx slave Arduino
 
  //Serial.println();
  sendbuf[0]=bsync;
  sendbuf[1]=calcchecksum(sendbuf);
 
  for(uint8_t i=0; i<MSGSIZE; i++) {                     // better use write() ?
     serialPutchar( Serial1, sendbuf[i]);                // Send values to the Rx Arduino       
  }       
  //serialFlush( Serial1 );                              // clear output buffer
 
  sprintf(sbuf, "send : %4d %4d     ", sendbuf[4], sendbuf[6]);
  printf(sbuf); 

 
  //     Receive from Rx slave Arduino

  memset(cbuf, 0, sizeof(cbuf));
   
  resOK = receive ( cbuf, 10000,&cnt);
 
  if( resOK ) {                         // byte 0 == syncbyte ?
     cnt=0;
            
     chk=(byte)calcchecksum(cbuf);     
     memcpy(recvbuf, cbuf, sizeof(cbuf));
     
     // debug
     sprintf(sbuf, "received: %4d %4d    \n ", recvbuf[4], recvbuf[6]);
     printf(sbuf);
 
        // change values to send back!
        memcpy(sendbuf, recvbuf, sizeof(sendbuf));         // copy inbuf to outbuf
        sendbuf[4]+=10;                                     // change [4] to send back
        sendbuf[6]+=20;  
        
       
  }
 
}


//=====================================================================================

 
int main() {
    unsigned long timesav;
    char  sbuf[128];

     
    printf("initializing..."); printf("\n");
   
    // UART Serial com port
    Serial1 = serialOpen (uart, 115200); // for Arduino code compatibility reasons
   
    while(1) { loop(); }
   
    serialClose( Serial1);
   

    exit(0);
}


