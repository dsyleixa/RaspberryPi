/*     Tx master  
 *     Raspberry Pi 
 *     ============
 *   
 *     ver 0007
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

char   * uart  =  "/dev/ttyAMA0";
int    Serial1;

 
//=====================================================================================
//  timer
// millis()
// micros()

//=====================================================================================
// conio.h

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

int _scrwidth_, _scrheight_;

#define vgHidewindow  HideWindow
#define vgShowwindow  End
#define vgcls()       ClearWindowRGB(_scrwidth_, _scrheight_, 0, 0, 0);	
#define vgStart()     Start(_scrwidth_, _scrheight_)
#define vgInit()      init(&_scrwidth_, &_scrheight_)
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
	vgInit();       // Graphics initialization
    vgStart();      // Start the picture
    vgcls();	
    
    vgSetcolor(255, 255, 255, 1);  // Set these at the start, no need to    
    vgFill(255,255,255, 1);        // keep calling them if colour hasn't changed
    vgSetfonttype(MonoTypeface);    
    vgSetfontsize(20);
}



//=====================================================================================
// debug monitor

void displayvalues(int line, char * caption, uint8_t array[]) {
  int cnt;
  char sbuf[128];
 
  sprintf(sbuf, "%s cks=%-4d", caption, array[1]);
  //lcdprintxy(0, line, sbuf); 
  printf(sbuf); printf("\n");
  for(cnt=0; cnt<8; ++cnt) {
    sprintf(sbuf, "%3d ", array[cnt]);      // print on TFT
    lcdprintxy(cnt*3*8, line+10, sbuf);
    printf(sbuf); 
  }   
  printf("\n");
 
} 

//=====================================================================================
//=====================================================================================
// serial com

const    uint32_t UARTclock=115200;
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
  sprintf(sbuf, "%4d %4d", sendbuf[4], sendbuf[6]);
  //lcdprintxy(0, 20, sbuf);
  printf(sbuf); printf("\n");

 
  //     Receive from Rx slave Arduino

  memset(cbuf, 0, sizeof(cbuf)); 
   
  resOK = receive ( cbuf, 10000,&cnt);
 
  if( resOK ) {                         // byte 0 == syncbyte ?
    cnt=0;

    //displayvalues(60, "Received...:", cbuf);
     chk=(byte)calcchecksum(cbuf);     
     memcpy(recvbuf, cbuf, sizeof(cbuf));
 
        // change values to send back!
        memcpy(sendbuf, recvbuf, sizeof(sendbuf));         // copy inbuf to outbuf
        sendbuf[4]+=1;                                     // change [4] to send back
       
       
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
     
    printf("initializing..."); printf("\n"); 
    
    // UART Serial com port 
    Serial1 = serialOpen (uart, UARTclock); // for Arduino code compatibility reasons 
    
    while(1) { loop(); }  
    
    serialClose( Serial1);
   
    exit(0);
}

//=====================================================================================
//=====================================================================================
