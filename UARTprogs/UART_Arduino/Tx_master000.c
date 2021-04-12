/*     Tx master     
       ver 0006
       IDE 1.6.5
 */
 
#include <SPI.h>
#include <SD.h>
#include <UTFTQD.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9340.h>


#define  clock()  millis()


//=====================================================================================
// TFT LCD
//=====================================================================================
#define   UTFT_SmallFont     8 // UTFT 8x10
#define   UTFT_MediumFont   12 // UTFT ++
#define   UTFT_BigFont      18 // UTFT +++
#define   _SmallFont_        1 // 9341 6x9
#define   _MediumFont_       2 // 9341 12x16
#define   _BigFont_          3 // 9341 18x23

int16_t  LCDmaxX , LCDmaxY ;                // display size
int16_t  _curx_, _cury_,                    // last x,y cursor pos on TFT screen
         _maxx_, _maxy_;                    // max. x,y cursor pos on TFT screen       
char     wspace[128];                       // line of white space


// set LCD TFT type
int16_t  LCDTYPE   =   -1;

#define  _LCD1602_    1  // LCD1602  Hitachi HD44780 driver <LiquidCrystal.h>
                           // http://www.arduino.cc/en/Tutorial/LiquidCrystal   //
#define  _SERLCD_     2  // Sparkfun serLCD 16x2 
                           // http://playground.arduino.cc/Code/SerLCD   //
#define  _UTFT_       4  // Henning Karlsen UTFT 2.2-2.4" 220x176 - 320x240 lib
                           // http://henningkarlsen.com/electronics/library.php?id=51   //
#define _ILI9341_     8  // https://github.com/adafruit/Adafruit_ILI9340
                           // https://github.com/adafruit/Adafruit-GFX-Library //
                           
                           
//--------------------------------------------------------------------------------------------------
#define  UTFT_cs      52    // <<<<<<<< adjust!

//UTFT   qdUTFT(Model, SDA=MOSI,  SCL, CS,         RESET,  RS)    // Due: 3 exposed SS pins: 4,10,52
  UTFT   qdUTFT(QD220A,   A2,     A1,  A5,         A4,     A3);   // adjust model parameter and pins!
//UTFT   qdUTFT(QD220A,   50,     49,  UTFT_cs,  0,     51);   // A0->Vc (LED), A4->BoardReset
 extern  uint8_t SmallFont[];
//--------------------------------------------------------------------------------------------------
#define    tft_cs     50
#define    tft_dc     49
#define    tft_rst     0
Adafruit_ILI9340   tft = Adafruit_ILI9340(tft_cs, tft_dc, tft_rst);

//--------------------------------------------------------------------------------------------------

int16_t  fontwi= 8;  // default
int16_t  fonthi=10;  // default


void putfonttype(uint8_t fsize) {
  if(LCDTYPE==_UTFT_)  { fontwi= qdUTFT.getFontXsize(); fonthi=qdUTFT.getFontYsize(); }
  else
  if(fsize==_SmallFont_)     { fontwi= 6; fonthi=9; }  // 5x7 + overhead ?
  else
  if(fsize==_MediumFont_)    { fontwi=12; fonthi=16; } // ?
  else
  if(fsize==_BigFont_)       { fontwi=18; fonthi=23; } // ?
 
  _maxx_ = LCDmaxX / fontwi;    // max number of letters x>>
  _maxy_ = LCDmaxY / fonthi;    // max number of letters y^^
  memset(wspace, ' ', _maxx_);  // line of white space
  wspace[_maxx_]='\0';
}



void setlcdorient(int8_t orient) {
  if(LCDTYPE==_ILI9341_) {
      tft.setRotation(orient);
      LCDmaxX=tft.width();
      LCDmaxY=tft.height();       
   }
}

void lcdcls()  {                                                         
   if(LCDTYPE==_UTFT_)      { qdUTFT.clrScr();                } 
   if(LCDTYPE==_ILI9341_)   { tft.fillScreen(ILI9340_BLACK);  }
   _curx_ =0;  _cury_ =0;
}

void curlf()   {                                                       
   _curx_=0;
   if( _cury_ <=(LCDmaxY-10) ) _cury_+=fonthi;
   else _cury_=0;   
   if(LCDTYPE==_ILI9341_)   {tft.setCursor(0, _cury_); } 
}



void curxy(int16_t  x,  int16_t  y) {
   _curx_ = x;
   _cury_ = y;
   if(LCDTYPE==_ILI9341_)   {tft.setCursor(x, y); }
}


void lcdprintxy(int16_t x, int16_t y, char * str) {
   if(LCDTYPE==_UTFT_)     { qdUTFT.print(str,x,y); _curx_=x+strlen(str)*fontwi; _cury_=y; }
   else if(LCDTYPE==_ILI9341_)  {
      tft.setCursor(x,y);  tft.print(str);
      _curx_=tft.getCursorX(); _cury_=tft.getCursorY();
   }
}


void lcdprint(char * str) {
    if(LCDTYPE==_UTFT_)     { qdUTFT.print(str, _curx_, _cury_); _curx_=_curx_+strlen(str)*fontwi; }
    else if(LCDTYPE==_ILI9341_)  {
       tft.setCursor(_curx_, _cury_); tft.print(str);
       _curx_=tft.getCursorX(); _cury_=tft.getCursorY();
    }
}


void initlcd(uint8_t orient) { // 0,2==Portrait  1,3==Landscape
   if(LCDTYPE==_UTFT_) {
      qdUTFT.InitLCD();
      LCDmaxX=qdUTFT.getDisplayXSize();
      LCDmaxY=qdUTFT.getDisplayYSize();
      qdUTFT.setFont(SmallFont);
      putfonttype(UTFT_SmallFont);
      fontwi=qdUTFT.getFontXsize();
      fonthi=qdUTFT.getFontYsize();
   }
   else
   if(LCDTYPE==_ILI9341_) {
      tft.begin();
      setlcdorient(orient);       
      tft.setTextSize(_SmallFont_);
      putfonttype(_SmallFont_);
   }   
} 



//=====================================================================================
//=====================================================================================

const    uint8_t  MSGSIZE=32;
uint8_t  bsync=255;
uint8_t  sendbuf[MSGSIZE];
uint8_t  recvbuf[MSGSIZE];


//=====================================================================================
const uint32_t UARTclock=9600;  // altern. 115200

void setup() {
   char sbuf[128];   
   int32_t  i=0;
         
   // Serial
   Serial.begin(115200);   // USB terminal
 
   Serial1.begin(UARTclock);                    // RX-TX UART
   while(Serial1.available())  Serial1.read();  // clear output buffer

     
   // TFT LCD
   Serial.println();
   LCDTYPE = _UTFT_;
   Serial.print("init LCD...");
   initlcd(1);   
   Serial.println(" done.");   lcdcls();
   sprintf(sbuf, "LCD=%d wi%d x hi%d",LCDTYPE,LCDmaxX,LCDmaxY);
   Serial.println(sbuf);
   Serial.println();
   lcdcls(); lcdprint(sbuf);
   
   sprintf(sbuf, "setup(): done.");
   Serial.println(); Serial.println(sbuf);   
   curlf(); curlf(); lcdprint(sbuf);

   lcdcls();
   
   sprintf(sbuf, "Tx master, BAUD= %ld", UARTclock );;
   lcdprintxy(0, 0, sbuf);


}



//=====================================================================================
//=====================================================================================

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

// ================================================================

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
  if(Serial1.available()){ // grab new char, test for sync char, if so start adding to buffer
    buf[*cnt] = (uint8_t)Serial1.read();
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
  if(done){ // do checksumOK test of buffer;
    done=checksumOK(buf);
    if(!done){// checksumOK failed, scan buffer for next sync char
       p = (uint8_t*)memchr((buf+1),0xff,(MSGSIZE-1)); //forgot to skip the current sync at 0
       
       
       if(p){ // found next sync char, shift buffer content, refill buffer
         *cnt = MSGSIZE -(p-buf); // count of characters to salvage from this failure
         memcpy(buf,p,*cnt); //cnt is now where the next character from Serial is stored!
         }
       else *cnt=0; // whole buffer is garbage
       }
    }
   
  }while(!done&&(millis()-start<timeout));

return done; // if done then buf[] contains a sendbufid buffer, else a timeout occurred
}

//=====================================================================================
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
  for(uint8_t i=0; i<MSGSIZE; i++) {       
     Serial1.write(sendbuf[i]);                          // Send values to the Rx Arduino       
  }       
  //Serial1.flush();                                     // clear output buffer
  //displayvalues(20, "Transmitted...: ", sendbuf);
  sprintf(sbuf, "%4d %4d", sendbuf[4], sendbuf[6]);
  lcdprintxy(0, 20, sbuf);

 
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
 
}


//=====================================================================================
//=====================================================================================
