
/*
   UART communication
   send/receive byte array (64 bytes)
   *     
   Raspberry Pi  master
   ver 0667
 */
 
 /*
  * change log
  * 
  * ver 0667:
  * Arduino via USB = ACM0
  * 
  */
 
// (C) Helmut Wunder (HaWe) 2015
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
int    Serial1;

 




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

 
  //   send to slave
 
  //Serial.println();
  sendbuf[0]=bsync;
  sendbuf[1]=calcchecksum(sendbuf);
 
  for(uint8_t i=0; i<MSGSIZE; i++) {                     //
     serialPutchar( Serial1, sendbuf[i]);                // Send values to the slave       
  }       
  //serialFlush( Serial1 );                              // clear output buffer
 
  sprintf(sbuf, "send : %4d %4d     ", sendbuf[4], sendbuf[6]);
  printf(sbuf);

 
  //     Receive from slave

  memset(cbuf, 0, sizeof(cbuf));
   
  resOK = receive ( cbuf, 10000,&cnt);
 
  if( resOK ) {                         //  receive ok?
     cnt=0;

     memcpy(recvbuf, cbuf, sizeof(cbuf));
     
     // debug
     sprintf(sbuf, "received: %4d %4d    \n ", recvbuf[4], recvbuf[6]);
     printf(sbuf);
 
      memset(sendbuf, 0, sizeof(sendbuf));    // clear send buf
     // debug: test values to send back!
      sendbuf[4]=recvbuf[4]+10;                      // change [4] to send back
      sendbuf[6]=recvbuf[6]+20;                      // change [6] to send back
       
       
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


