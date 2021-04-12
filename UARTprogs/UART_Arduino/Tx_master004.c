/*     Tx master  
 *     Raspberry Pi 
 *     ============
 *   
 *     ver 0007.4
 *     Rasbian 8.0
       
 */
 
 
// (C) Helmut Wunder (HaWe) 2015
// freie Verwendung für private Zwecke
// für kommerzielle Zwecke nur nach Genehmigung durch den Autor.
// Programming language: gcc C/C++, Geany IDE
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
#include <errno.h>

#include <termios.h>

#include "VG/openvg.h"
#include "VG/vgu.h"
#include "fontinfo.h"
#include "shapes.h"
#include "shapes_plus.h"

#include <wiringPi.h>
#include <wiringSerial.h>

#define  byte  uint8_t

const    uint32_t UARTclock=115200;
char     * uart  =  "/dev/ttyAMA0";
int      Serial1;

 


//=====================================================================================
// C TOOLS
//=====================================================================================

// timer
// millis()
// micros()

//=====================================================================================

int kbhit(void)
{
  struct termios oldt, newt;
  int ch;
  int oldf;
 
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
 
  ch = getchar();
 
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);
 
  if(ch != EOF)
  {
    ungetc(ch, stdin);
    return 1;
  }
 
  return 0;
}

//=====================================================================================
// openvg + shapes_plus
//=====================================================================================

int _scrwidth_, _scrheight_;

#define vgHidewindow  HideWindow
#define vgShowwindow  End
#define vgcls()       ClearWindowRGB(_scrwidth_, _scrheight_, 0, 0, 0);	
#define vgInit()      init(&_scrwidth_, &_scrheight_)
#define vgStart()     Start(_scrwidth_, _scrheight_)
#define vgCleanup     finish
#define vgSetcolor    Stroke
#define vgFill        Fill

int      _fontsize_ = 10;
Fontinfo _font_     = MonoTypeface;


inline void vgSetfontsize(int size) { _fontsize_ = size; }

inline void vgSetfonttype(Fontinfo myfont) { _font_ = myfont; }

inline void lcdprintxytop(float x, float y, char * buf) {
	Text(x, _scrheight_-y, buf, _font_ , _fontsize_); 
}

inline void lcdprintxy(float x, float y, char * buf) {
	Text(x, y, buf, _font_ , _fontsize_); 
}


void vgInitgraph() {
	vgInit();
    vgStart();
	
    vgcls();	
    
    vgSetcolor(255, 255, 255, 1);  // Set these at the start, no need to    
    vgFill(255,255,255, 1);        // keep calling them if colour hasn't changed
    vgSetfonttype(MonoTypeface);    
    vgSetfontsize(10);
}

//=====================================================================================
// DATA PACKAGES
//=====================================================================================

#define MAXMOTORS           6 // maximum number of encoder motors 
#define MAXANALOG          12 // maximum number of analog sensors 
#define MAXDIGPINS         16 // maximum number of digital pins 


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
#define FLOATSLOT0   20  // optional: float 0
#define ANASLOT0     24  // 24,26,28,30,32,34,36,38,40,42,44,46
#define ANASLOT10    44  // analog 10+11 / optional: float 1
#define FLOATSLOT1   44  // optional: float 1

#define PWMSLOT0     24  // 24-39, pwm Dpin: D0-D16
#define MCMDSLOT0    40  // 40-47, motor cmd pwm -100..0..+100, 
                         //        brake -128 emergStop -127
                         //        PID rotate: abs./float 120 /stop 121 rel./float 122 /stop 123 /hold 124


volatile int8_t  digval[MAXDIGPINS];
volatile int16_t anaval[MAXANALOG];
volatile int32_t motenc[MAXMOTORS]; 

//=====================================================================================
// PARSER
//=====================================================================================

//************************************************************************************
// bit and byte and pin operations
//************************************************************************************
// convert byte arrays to int

inline int16_t  ByteArrayToInt16(uint8_t  array[], uint8_t  slot) {
  return ((array[slot + 1] << 8) + (array[slot]));
}

inline long  ByteArrayToInt32(uint8_t  array[], uint8_t  slot) {
  return ( (array[slot+3]<<24) + (array[slot+2]<<16) + (array[slot+1]<<8) + array[slot] );
}

//************************************************************************************
// convert int to byte arrays

inline void Int16ToByteArray(int16_t  integer, uint8_t  *array, uint8_t  slot) {
  array[slot]   = (integer & 0xFF); // loByte
  array[slot + 1] = (integer >> 8); // hiByte
}

inline void Int32ToByteArray(int32_t  int32, uint8_t  *array, uint8_t  slot) {
  array[slot]     = (int32 & 0xFF);         // loByte 8
  array[slot + 1] = ((int32 >>  8) & 0xFF); // intByte 16
  array[slot + 2] = ((int32 >> 16) & 0xFF); // longloByte 24
  array[slot + 2] = ((int32 >> 24) & 0xFF); // hiByte 32
}

//************************************************************************************
// read+write bits in numbers

#define bitRead(source, bit) ( ((source) >> (bit)) & 0x01 )

#define bitSet (source, bit) ( (source) |= (1UL << (bit)) )

#define bitClear(source, bit) ( (source) &= ~(1UL << (bit)) )

#define bitWrite(source, bit, bitvalue) ( bitvalue ? bitSet(source, bit) : bitClear(source, bit) )


//=====================================================================================

void parsercvbuf(byte * arr) {
	int i;
	
	for( i=0; i<8; ++i) {
		digval[i] = bitRead( arr[DIGSLOT], i);
	}
	for( i=0; i<8; ++i) {
		digval[i+8] = bitRead( arr[DIGSLOT+1], i);
	}
	
	motenc[0] = ByteArrayToInt32(arr, ENCSLOT0);
	motenc[1] = ByteArrayToInt32(arr, ENCSLOT1);
	for( i=2; i<MAXMOTORS; ++i) {
		motenc[i] = ByteArrayToInt16(arr, ENCSLOT3+i-2);
	}
	
	for( i=0; i<MAXANALOG; ++i) {
		anaval[i] = ByteArrayToInt16(arr, ANASLOT0+i);
	}


	
	
}

//=====================================================================================
// DISPLAY
//=====================================================================================

void displayvalues(int line, uint8_t array[]) {
  int cnt;
  char sbuf[128];
  
  vgcls(); 
 
  sprintf(sbuf, "Received:");
  lcdprintxytop(60, line+20, sbuf);     
  //printf(sbuf); printf("\n");
  
  
  for(cnt=0; cnt<16; ++cnt) {
    sprintf(sbuf, "%3d ", cnt);      // print cnt on TFT
    lcdprintxytop( (cnt+1)*60, line+40, sbuf);
    //printf(sbuf); 
  } 
  
  lcdprintxytop(60, line+80, "sync chksum TC bytes");
  for(cnt=0; cnt<2; ++cnt) {
    sprintf(sbuf, "%3d ", array[cnt]);      // print dig bitbytes on TFT
    lcdprintxytop( (cnt+1)*60, line+100, sbuf);
    //printf(sbuf); 
  }   
  
  lcdprintxytop(60, line+140, "digital bits/bytes");
  for(cnt=2; cnt<4; ++cnt) {
    sprintf(sbuf, "%3d ", array[cnt]);        // print dig 0-7 on TFT
    lcdprintxytop( (cnt+1-2)*60, line+160, sbuf);
    //printf(sbuf); 
  }  
  
  for(cnt=0; cnt<16; ++cnt) {
    sprintf(sbuf, "%3d ",  digval[cnt] );     // print dig 8-15 on TFT
    lcdprintxytop( (cnt+1)*60, line+175, sbuf);
    //printf(sbuf); 
  }  
  
  
  
  lcdprintxytop(60, line+200, "encoder bytes #0-2 2x long 1x short // #3-7 5x short");
  for(cnt=4; cnt<14; ++cnt) {
    sprintf(sbuf, "%3d ", array[cnt]);      // print enc bytes  
    lcdprintxytop( (cnt+1-4)*60, line+220, sbuf);
    //printf(sbuf); 
  }     

  for(cnt=14; cnt<24; ++cnt) {
    sprintf(sbuf, "%3d ", array[cnt]);      // print enc bytes  
    lcdprintxytop( (cnt+1-14)*60, line+235, sbuf);
    //printf(sbuf); 
  } 
  
  for(cnt=0; cnt<MAXMOTORS; ++cnt) {
	sprintf(sbuf, "%+010ld ", (long)motenc[cnt]);  // print enc  
    lcdprintxytop( 3+(cnt+1)*120, line+260, sbuf);
    //printf(sbuf);  
  }
  
  lcdprintxytop(60, line+280, "analog bytes #0-5x int // #6-11x int");
  for(cnt=24; cnt<36; ++cnt) {
    sprintf(sbuf, "%3d ", array[cnt]);      // print analog bytes 0-5
    lcdprintxytop( (cnt+1-24)*60, line+300, sbuf);
    //printf(sbuf); 
  } 

  for(cnt=36; cnt<48; ++cnt) {
    sprintf(sbuf, "%3d ", array[cnt]);      // print analog bytes 6-11
    lcdprintxytop( (cnt+1-36)*60, line+315, sbuf);
    //printf(sbuf);     
  } 
  
  for(cnt=0; cnt<MAXANALOG; ++cnt) {
	sprintf(sbuf, "%+06d ", anaval[cnt]);     // print analog values 0-11  
    lcdprintxytop( (cnt+1)*60, line+340, sbuf);
  }

  vgShowwindow();         // Show the graphic screen
  //printf("\n");
 
} 

//=====================================================================================
// SERIAL COMM
//=====================================================================================

const    uint8_t  MSGSIZE = 48;
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

//=====================================================================================
// LOOP
//=====================================================================================

void loop()
{ 
  char     sbuf[128], ch, resOK;   
  static   uint8_t cnt=0; 
  uint8_t  cbuf[MSGSIZE], chk;

 
  //   send to Rx slave Arduino
 
  //Serial.println(); 
  sendbuf[0]=bsync;
  sendbuf[1]=calcchecksum(sendbuf);
  
  write( Serial1, sendbuf, MSGSIZE );            // Send values to the Rx Arduino   
     
  //serialFlush( Serial1 );                              // clear output buffer
  
  //displayvalues(20, "Transmitted...: ", sendbuf);  
  //sprintf(sbuf, "%4d %4d", sendbuf[4], sendbuf[6]);
  //printf(sbuf); printf("\n");

 
  //     Receive from Rx slave Arduino

  memset(cbuf, 0, sizeof(cbuf)); 
   
  resOK = receive ( cbuf, 10000, &cnt);
 
  if( resOK ) {                         // byte 0 == syncbyte ?
    cnt=0;

    memcpy(recvbuf, cbuf, sizeof(cbuf));
    parsercvbuf(cbuf);
     
    displayvalues(10, cbuf);
    
 
    // debug: change values to send back!
    memcpy(sendbuf, recvbuf, sizeof(sendbuf));     // copy inbuf to outbuf
    //sendbuf[4]+=1;                                 // change [4] to send back
       
       
  }
  // debug...: careful!  effing slowing down!
  if( kbhit() ) {ch=0; while(ch!='\n') ch=getchar();  }
  //
  
  
}


//=====================================================================================

 
int main() {
    unsigned long timesav;
    char  sbuf[128];
    char  instr[128];
     
    //printf("initializing..."); printf("\n"); 
    
    // UART Serial com port 
    Serial1 = serialOpen (uart, UARTclock); // for Arduino code compatibility reasons 
    
    // graphic screen
    vgInitgraph();    
       
    vgHidewindow();         // Hide the graphic screen     
    // debug
    // lcdprintxytop(50,  50, "press key to proceed");       
    // vgShowwindow();         // Show the graphic screen        
    // getchar();
     
    vgcls(); 
    while(1) { loop(); }
    
   
    serialClose( Serial1);
   
    exit(0);
}

//=====================================================================================
//=====================================================================================
