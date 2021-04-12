/*     Tx master 
 *     Raspberry Pi 
 * 
 *     ver 0010      
 */
 
// (C) Helmut Wunder (HaWe) 2015
// freie Verwendung für private Zwecke
// für kommerzielle Zwecke nur nach Genehmigung durch den Autor.
// Programming language: gcc  C/C++, Geany IDE
// protected under the friendly Creative Commons 
// Attribution-NonCommercial-ShareAlike 3.0 Unported License
// http://creativecommons.org/licenses/by-nc-sa/3.0/
 


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <sys/time.h>
#include <errno.h>
#include <pthread.h>

#include "VG/openvg.h"   
#include "VG/vgu.h"
#include "fontinfo.h"
#include "shapes.h"

#include <wiringPi.h>
#include <wiringSerial.h>

#define  byte  uint8_t

char   * uart  =  "/dev/ttyAMA0";
int    Serial1;

#define RED   255,  0,    0
#define WHITE 255, 255, 255
 

//======================================================================
// openvg 

int _scrwidth_, 
    _scrheight_;

#define cls()   WindowClear()   

int    _fontsize_ = 10;

inline void lcdprintxy(float x, float y, char * buf) {
   Text( (x+_fontsize_), (y+_fontsize_*1.2), buf, MonoTypeface , _fontsize_);   
}


void Initgraph() {
	 initWindowSize(20,20,600,480);
     init(&_scrwidth_, &_scrheight_);  

     Start(_scrwidth_, _scrheight_);   // Start the picture
     Background(0, 0, 0);  
     Stroke(WHITE, 1);  // Set these at the start, no need to   
     Fill(WHITE, 1);      // keep calling them if colour hasn't changed
     cls();
     
     End();
}

//======================================================================
// msg array structure


const    uint8_t  MSGSIZE=64;
uint8_t  bsync=255;
uint8_t  sendbuf[MSGSIZE];
uint8_t  recvbuf[MSGSIZE];

#define OUT_REGSTATE_NUL            0
#define OUT_REGSTATE_COAST          2
#define OUT_REGSTATE_BRAKE          3
#define OUT_REGSTATE_EMERG_STOP     5
#define OUT_REGSTATE_ON             8

#define OUT_REGSTATE_PIDIDLE       15

#define OUT_REGSTATE_PIDACTIVE     16
#define OUT_REGSTATE_PIDRELATIVE   17
#define OUT_REGSTATE_PIDABSOLUTE   18
#define OUT_REGSTATE_PIDHOLD       19
#define OUT_REGSTATE_PIDHOLDCLOSE  20


#define SYNCSLOT      0  // start of this Msg 
#define CKSSLOT       1  // chksum this Msg 
#define DIGSLOT       2  // 2,3: digital Data bitpattern
#define ENCSLOT0      4  // motor 0 -7: 4,8,12,14,16,18,20,22
#define ENCSLOT1      8  // motor 1
#define ENCSLOT2     12  // motor 2
#define ENCSLOT3     14  // motor 3
#define ENCSLOT4     16  // motor 4
#define ENCSLOT5     18  // motor 5
#define ENCSLOT6     20  // motor 6 / optional: float 0
#define FLOATSLOT0   20  // optional: float2
#define ANASLOT0     24  // 24,26,28,30,32,34,36,38,40,42,44,46
#define ANASLOT10    44  // analog 10+11 / optional: float 1
#define FLOATSLOT1   44  // optional: float3

#define PWMSLOT0     24  // 24-39: pwm Dpin: D0-D16
#define MCMDSLOT0    40  // 40-47: 8 motor cmd sent by master pwm -100..0..+100, 
                         //        brake -128 emergStop -127
                         //        PID rotate: abs./float 120 /stop 121 rel./float 122 /stop 123 /hold 124
#define MRUNSLOT0    48  // 48-55: bytes returned to master: 8 current motor runstates
#define LAST8SLOT    55  // custom: float0 or int32
#define LAST4SLOT    59  // custom: float1 or int32
#define TERMINUS     63  // end of array block

#define MAXMOTORS           8 // maximum number of encoder motors 
#define MAXANALOG          12 // maximum number of analog sensors 
#define MAXDIGPINS         16 // maximum number of digital pins 

int8_t   digval[MAXDIGPINS];
uint16_t digvalraw=0;
int16_t  anaval[MAXANALOG];
int32_t  motenc[MAXMOTORS],  oldenc[MAXMOTORS] ;


//----------------------------------------------------------------------
// read+write bits in numbers
#define bitRead(source, bit) ( ((source) >> (bit)) & 0x01 )
#define bitSet (source, bit) ( (source) |= (1UL << (bit)) )
#define bitClear(source, bit) ( (source) &= ~(1UL << (bit)) )
#define bitWrite(source, bit, bitvalue) ( bitvalue ? bitSet(source, bit) : bitClear(source, bit) )




//======================================================================

void displayvalues(int line, uint8_t array[]) {
  int cnt;
  char sbuf[128];
  
  //Stroke(WHITE, 1);
  Fill(WHITE, 1);
  
  lcdprintxy(0,_scrheight_-20, "sendbuf");
  lcdprintxy(0,_scrheight_-240, "recvbuf");
 
  for(cnt=0; cnt<16; ++cnt) {
    sprintf(sbuf, "%3d ", array[cnt ]);      // print on TFT raw
    lcdprintxy(cnt*3*_fontsize_, line, sbuf);
  }  
  for(cnt=0; cnt<16; ++cnt) {
    sprintf(sbuf, "%3d ", array[cnt+16]);      // print on TFT raw
    lcdprintxy(cnt*3*_fontsize_, line-20, sbuf);
  }  
 
  for(cnt=0; cnt<16; ++cnt) {
    sprintf(sbuf, "%3d ", array[cnt+32]);      // print on TFT raw
    lcdprintxy(cnt*3*_fontsize_, line-40, sbuf);
  }  
  for(cnt=0; cnt<16; ++cnt) {
    sprintf(sbuf, "%3d ", array[cnt+48]);      // print on TFT raw
    lcdprintxy(cnt*3*_fontsize_, line-60, sbuf);
  } 
  
  
  for(cnt=0; cnt<16; ++cnt) {
    sprintf(sbuf, "%3d ", digval[cnt]);   // print on TFT dig val
    lcdprintxy(cnt*3*_fontsize_, line-110, sbuf);
  }  
  
  //Stroke(RED, 1);
  Fill(RED, 1);  

  for(cnt=0; cnt<16; ++cnt) {
    sprintf(sbuf, "%3d ", cnt);      // print on TFT
    lcdprintxy(cnt*3*_fontsize_, line-85, sbuf);
  }   
  
  End();
 
}

//======================================================================
//======================================================================
// serial com



void decyphdval(uint8_t array[]){
  memcpy( &digvalraw, array+DIGSLOT*sizeof(char), sizeof(uint16_t) );	
  for (int i=0; i<MAXDIGPINS; ++i) {
	  digval[i] = bitRead(digvalraw, i);
  }	
}

//======================================================================
// TCP

uint8_t calcchecksum(uint8_t array[]) {
  int32_t  sum=0;
  for(int i=2; i<MSGSIZE; ++i) sum+=(array[i]);
  return (sum & 0x00ff);
}

bool checksumOK(uint8_t array[]){
return (calcchecksum(array)==array[1]);
}

// =====================================================================
// addToBuffer and receive function courtesy of chucktodd

bool addToBuffer( uint8_t buf[], uint8_t *cnt, uint16_t timeout){
bool inSync = *cnt>0;
unsigned long start=millis();
while((*cnt<MSGSIZE)&&(millis()-start<timeout)){
  if( serialDataAvail( Serial1 ) ) { // grab new char, test for sync char, if so start adding to buffer
    buf[*cnt] = (uint8_t)serialGetchar( Serial1 );
    if(inSync) *cnt += 1;  // my origional *cnt++ was updating the pointer address, not
                           // the pointed to sendbuffer
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


//======================================================================

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
       p = (uint8_t*)memchr((buf+1),0xff,(MSGSIZE-1)); //forgot to skip the current sync at 0
       
       
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

//======================================================================
// tasks
//======================================================================




//======================================================================
// loop
//======================================================================
void loop()
{
  char     sbuf[128],  resOK;   
  static   uint8_t cnt=0;
  uint8_t  cbuf[MSGSIZE], chk;

 
  //   send to Rx slave Arduino
 
  sendbuf[0]=bsync;
  sendbuf[1]=calcchecksum(sendbuf);  
 
  for(uint8_t i=0; i<MSGSIZE; i++) {                     // better use write() ?
     serialPutchar( Serial1, sendbuf[i]);                // Send values to the Rx Arduino       
  }   
  
  cls(); 
  displayvalues(_scrheight_-40, sendbuf);
 
  //     Receive from Rx slave Arduino

  memset(cbuf, 0, sizeof(cbuf));
   
  resOK = receive ( cbuf, 10000, &cnt);
 
  if( resOK ) {                         // byte 0 == syncbyte ?
     cnt=0;
    
     displayvalues(_scrheight_-260, cbuf);
     
     decyphdval(cbuf);
     
     chk=(byte)calcchecksum(cbuf);    
     memcpy(recvbuf, cbuf, sizeof(cbuf));
 
        // change values to send back!
        memcpy(sendbuf, recvbuf, sizeof(sendbuf));         // copy inbuf to outbuf
        sendbuf[4]+=1;                                     // change [4] to send back    
  }
  
}


//======================================================================
// main
//====================================================================== 
int main() {
    unsigned long timesav;
    char  sbuf[128];
     
    printf("initializing..."); printf("\n");
    Initgraph();
    
    // UART Serial com port
    Serial1 = serialOpen (uart, 115200); // for Arduino code compatibility reasons
   
    while(1) { loop(); } 
   
    serialClose( Serial1);
    
    End();
    finish();   
    
    exit(0);
}

//======================================================================
//======================================================================
