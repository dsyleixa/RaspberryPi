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

#include "VG/openvg.h"
#include "VG/vgu.h"
#include "fontinfo.h"
#include "shapes.h"
#include "shapes_plus.h"

#include <wiringPi.h>
#include <wiringSerial.h>

#define  byte  uint8_t

char   * uart;
int    Serial1;

//=====================================================================================
// millis() timer

uint32_t millisec()
{
   struct timeval now;
   uint32_t ticks;
   gettimeofday(&now, NULL);
   ticks=now.tv_sec*1000+now.tv_usec/1000;
   return(ticks);
}

#define  millis  millisec


//=====================================================================================
// openvg + shapes_plus

int _scrwidth_, _scrheight_;

#define vgHidewindow  HideWindow
#define vgShowwindow  End
#define vgcls()       ClearWindowRGB(_scrwidth_, _scrheight_, 0, 0, 0);	
#define vgStart       Start
#define vgInit        init 
#define vgCleanup     finish

int      _fontsize_ = 10;
Fontinfo _font_     = MonoTypeface;


inline void setfontsize(int size) { _fontsize_ = size; }

inline void setfonttype(Fontinfo myfont) { _font_ = myfont; }

inline void lcdprintxytop(float x, float y, char * buf) {
	Text(x, _scrheight_-y, buf, _font_ , _fontsize_); 
}

inline void lcdprintxy(float x, float y, char * buf) {
	Text(x, y, buf, _font_ , _fontsize_); 
}


void initgraph() {
	vgInit(& _scrwidth_, & _scrheight_);       // Graphics initialization

    vgStart(_scrwidth_, _scrheight_);   // Start the picture
    vgcls();	
    
    Stroke(255, 255, 255, 1);  // Set these at the start, no need to    
    Fill(255,255,255, 1);      // keep calling them if colour hasn't changed
    setfonttype(MonoTypeface);    
    setfontsize(20);
}



//=====================================================================================
// debug monitor

void displayvalues(int line, char * caption, uint8_t array[]) {
  int cnt;
  char sbuf[128];
 
  sprintf(sbuf, "%s cks=%-4d", caption, array[1]);
  lcdprintxy(0, line, sbuf); 
  //Serial.println(sbuf);
  for(cnt=0; cnt<8; ++cnt) {
    sprintf(sbuf, "%3d ", array[cnt]);      // print on TFT
    lcdprintxy(cnt*3*8, line+10, sbuf);
    //Serial.print(sbuf);                      // Print sendbuffer to the Serial Monitor
  }   
  //Serial.println(); 
 
} 

//=====================================================================================
//=====================================================================================
// serial com


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


const    uint8_t  MSGSIZE=32;
uint8_t  bsync=255;
uint8_t  sendbuf[MSGSIZE];
uint8_t  recvbuf[MSGSIZE];

//=====================================================================================
//=====================================================================================

void loop()
{  
  char     sbuf[128],  resOK, bbuf;    
  uint8_t  cbuf[MSGSIZE], chk;
  int      i;


  
  //----------------------------------------------------------
  //    send to slave
  //----------------------------------------------------------
  
  // prepare send buffer for testing
  // if receive succeeded: change received values to send back!
  if( resOK ) {     
        
        memcpy(sendbuf, recvbuf, sizeof(sendbuf));   // copy inbuf to outbuf
        sendbuf[4]+=10;                              // change [4] to send back  
        sendbuf[6]+=20;                              // change [6] to send back 
  }      
  Serial.println("sending....");  
  sendbuf[0]=bsync;
  sendbuf[1]=calcchecksum(sendbuf);
  for(uint8_t i=0; i<MSGSIZE; i++) {        
     Serial1.write(sendbuf[i]);                      // Send value to the Rx Arduino        
  }        
  
  displayvalues(60, "Transmitted...: ", sendbuf, 8);



 
  
  //----------------------------------------------------------
  //     Receive from slave
  //----------------------------------------------------------
  
  memset(cbuf, 0, sizeof(cbuf));  

  //lcdcls(); 
  Serial.println("receiving....");
  if (Serial1.available()) {       
    i=0;
    while(Serial1.available() && i<MSGSIZE) {    
       cbuf[i] = Serial1.read();
       //if(i==0 && cbuf[i]!=bsync) continue;
       Serial.print(cbuf[i]);
       i++;
    }
    while(Serial1.available() ) bbuf=Serial1.read(); // clear Serial input buf
  } 
  // TCP check
  resOK=checksumOK(cbuf) ;  // && cbuf[0]==bsync
  // debug
  Serial.print("nrecv: ");  Serial.print(i); Serial.print("  "); Serial.println(resOK);

  if( resOK ) {                                      // byte 0 == syncbyte ?
     displayvalues(0, "Received...:", cbuf, 8);
     chk=(byte)calcchecksum(cbuf);      
     memcpy(recvbuf, cbuf, sizeof(cbuf));
  }
  else {
     sprintf(sbuf, "Rec. bsync?:%3d  resOK:%d \n", cbuf[0], resOK);
     Serial.println(sbuf);
  }


}


//=====================================================================================

 
int main() {
    unsigned long timesav;
    char  sbuf[128];
    char  instr[128];
     
    printf("initializing..."); printf("\n"); 
    
    // UART Serial com port 
    uart[] = "/dev/ttyAMA0";
    Serial1 = serialOpen (uart, 115200); // for Arduino code compatibility reasons 
    
    while(1) { loop() };  
   
    exit(0);
}

//=====================================================================================
//=====================================================================================
