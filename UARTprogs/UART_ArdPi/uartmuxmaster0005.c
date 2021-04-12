
/* 
   UART communication
   send/receive byte array (64 bytes)
        
   Raspberry Pi  master
   
*/
  
#define vers "uartmuxmaster0005"
   
 
 

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

#define  byte     uint8_t
#define  INVALID  -1001

char   * uart  =  "/dev/ttyAMA0";
int    Serial1;


double  fLat=-9000.0, fLng=-9000.0;
double  fcourse, fspeed;
int16_t dcoursex32, dspeedx32;
int8_t  nsat;

 
 

//******************************************************
// uart comm array
//******************************************************

#define SYNCSLOT      0  // start sync signal of this Msg: bsync  (e.g. 0xff/255)
#define CKSSLOT       1  // chksum this Msg
#define BYTE0         2  // byte 0      // byte: 8-bit => 8 digital bits 0-7
#define BYTE1         3  // byte 1      // byte: 8-bit => 8 digital bits 8-15
#define LONG0         4  // int32_t 0      // 3x int32_t: 32-bit/4byte
#define LONG1         8  // int32_t 1
#define LONG2        12  // int32_t 2
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



//******************************************************************************
// bit and byte and pin operations
//******************************************************************************
// convert byte arrays to int

inline int16_t  ByteArrayToInt16(uint8_t  barray[], uint8_t  slot) {
  return ((barray[slot + 1] << 8) + (barray[slot]));
}

inline int32_t  ByteArrayToInt32(uint8_t  barray[], uint8_t  slot) {
  return ( (barray[slot+3]<<24) + (barray[slot+2]<<16) + (barray[slot+1]<<8) + barray[slot] );
}

//-----------------------------------------------------------------------------
// convert int to byte arrays

inline void Int16ToByteArray(int16_t vint16,  uint8_t barray[],  uint8_t slot) {
  memcpy(barray+slot*sizeof(char), &vint16, sizeof(int16_t));    // copy int16 to array
}

inline void Int32ToByteArray(int32_t vint32,  uint8_t barray[],  uint8_t slot) {
  memcpy(barray+slot*sizeof(char), &vint32, sizeof(int32_t));    // copy int32 to array
}


//-----------------------------------------------------------------------------
// convert float/double to byte arrays



void DoubleToByteArray(double fvar, uint8_t * barray,  uint8_t slot) {
   union {
     double  f;
     uint8_t b8[sizeof(double)];
   } u;
   u.f = fvar;
   memcpy(barray+slot*sizeof(char), u.b8, sizeof(double));  
}


double ByteArrayToDouble(uint8_t * barray, uint8_t  slot) {
   union {
     double  f;
     uint8_t b8[sizeof(double)];
   } u;
   memcpy( u.b8, barray+slot*sizeof(char), sizeof(double)); 
   return u.f;
}




void FloatToByteArray(float fvar, uint8_t * barray,  uint8_t slot) {
    union {
     double  f;
     uint8_t b4[sizeof(float)];
   } u;
   u.f = fvar;
   memcpy(barray+slot*sizeof(char), u.b4, sizeof(float));   
}



double ByteArrayToFloat(uint8_t * barray, uint8_t  slot) {
   union {
     double  f;
     uint8_t b4[sizeof(float)];
   } u;
   memcpy( u.b4, barray+slot*sizeof(char), sizeof(float)); 
   return u.f;
}


//===============================================================================
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

//===============================================================================
//===============================================================================
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
unsigned int32_t start=millis();
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


//===============================================================================

bool receive(uint8_t * buf, uint16_t timeout, uint8_t *cnt){ // by passing cnt in and out,
// i can timeout and still save a partial buffer, so a resync costs less (less data lost)

bool inSync=false;
unsigned int32_t start=millis();
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

//===============================================================================


void UARTMUXloop()
{
  char     sbuf[128],  resOK;   
  static   uint8_t cnt=0;
  uint8_t  cbuf[MSGSIZE], chk;

  while(1) {
     //   send to Rx slave Arduino
    
     //Serial.println();
     sendbuf[0]=bsync;
     sendbuf[1]=calcchecksum(sendbuf);
    
     for(uint8_t i=0; i<MSGSIZE; i++) {                     // better use write() ?
        serialPutchar( Serial1, sendbuf[i]);                // Send values to the Rx Arduino       
     }       
     //serialFlush( Serial1 );                              // clear output buffer
    
     sprintf(sbuf, "send : %4d %4d     ", sendbuf[BYTE0], sendbuf[BYTE1]);
     printf(sbuf); 
   
    
     //     Receive from Rx slave Arduino
   
     memset(cbuf, 0, sizeof(cbuf));
      
     resOK = receive ( cbuf, 10000,&cnt);
    
     if( resOK ) {                         // byte 0 == syncbyte ?
        cnt=0;
               
        chk=(byte)calcchecksum(cbuf);     
        memcpy(recvbuf, cbuf, sizeof(cbuf));
        
        fLat = ByteArrayToDouble(recvbuf,DOUBL0);
        fLng = ByteArrayToDouble(recvbuf,DOUBL1);
        dcoursex32 = ByteArrayToInt16(recvbuf, ANA7);
        fcourse = dcoursex32/32.0;
        dspeedx32 = ByteArrayToInt16(recvbuf, ANA8);
        fspeed = dspeedx32/32.0;
        nsat = recvbuf[BYTE2];
        
        
        // debug
        sprintf(sbuf, "received: %4d %4d  %13.9f %13.9f  %6.1f  %4.1f  %2d \n ", 
               recvbuf[BYTE0], recvbuf[BYTE1], fLat, fLng, fcourse, fspeed, nsat);
        printf(sbuf);
    
           // change values to send back!
           memset(sendbuf, 0, sizeof(sendbuf));         // clear send buf
           sendbuf[BYTE0]=recvbuf[BYTE0]+10;                    // change [BYTE0]+[BYTE1] to send back
           sendbuf[BYTE1]=recvbuf[BYTE1]+20;  
                  
     }
  } 
}
   
   
   //============================================================================

 
int main() {
    unsigned long timesav;
    char  sbuf[128];

     
    printf("initializing..."); printf("\n");
   
    // UART Serial com port
    Serial1 = serialOpen (uart, 115200); // for Arduino code compatibility reasons
   
    UARTMUXloop(); 
   
    serialClose( Serial1);
   

    exit(0);
}


