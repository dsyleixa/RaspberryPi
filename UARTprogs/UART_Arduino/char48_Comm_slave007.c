/*     Rx slave     
 *     Arduino Due
 *     ===========
 *      ver 0007
 *      IDE 1.6.5
 */
 
// (C) Helmut Wunder (HaWe) 2015
// freie Verwendung für private Zwecke
// für kommerzielle Zwecke nur nach Genehmigung durch den Autor.
// Programming language: Arduino Sketch C/C++ (IDE 1.6.1 - 1.6.5)
// protected under the friendly Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License
// http://creativecommons.org/licenses/by-nc-sa/3.0/

 
#include <SPI.h>
#include <SD.h>
#include <DueTimer.h>


// SPI header                      |--|
// SPI pins Due         -----------|  |------------
//#define MISO     74   |  RES   76_SCK   74_MISO |
//#define MOSI     75   | -GND   75_MOSI   Vc+5V  |
//#define SCK      76   --------------------------

// SPI pins Mega
//#define MISO     50
//#define MOSI     51
//#define SCK      52


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



// set LCD TFT type
int16_t  LCDTYPE   =   -1;

#define  _LCD1602_    1  // LCD1602  Hitachi HD44780 driver <LiquidCrystal.h> 
                           // http://www.arduino.cc/en/Tutorial/LiquidCrystal   //
#define  _UTFT_       4  // Henning Karlsen UTFT 2.2-2.4" 220x176 - 320x240 lib
                           // http://henningkarlsen.com/electronics/library.php?id=51   //
#define _ILI9341_     8  // https://github.com/adafruit/Adafruit_ILI9340
                           // https://github.com/adafruit/Adafruit-GFX-Library //
#define _ILI9341due_  9  // ILI9341_due NEW lib by Marek Buriak
                           // http://marekburiak.github.io/ILI9341_due/ //
                           
//--------------------------------------------------------------------------------------------------
#define    tft_dc     49
#define    tft_cs     50
#define    tft_rst     0                        
                          
//=====================================================================================
// UTFT Henning Karlsen
//=====================================================================================
#include <UTFTQD.h>

//UTFT   qdUTFT(Model,  SDA=MOSI,    SCL,       CS,     RESET,  RS)    // Due: 3 exposed SS pins: 4,10,52
  UTFT   qdUTFT(QD220A,   A2,        A1,        A5,     A4,     A3);   // adjust model parameter and pins!
//UTFT   qdUTFT(QD220A, _MEGAMOSI_, _MEGASCK_, tft_cs , 0,  tft_dc);   // A0->Vc (LED), A4->BoardReset
 extern  uint8_t SmallFont[];
 
//=====================================================================================
// TFT Adafruit LIL9340/ILI9341
//=====================================================================================
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9340.h>

Adafruit_ILI9340  tft = Adafruit_ILI9340(tft_cs, tft_dc, tft_rst);


//=====================================================================================
// TFT ILI9341_due // http://marekburiak.github.io/ILI9341_due/ //
//=====================================================================================
#include <ILI9341_due_config.h>
#include <ILI9341_due.h>
#include <SystemFont5x7.h>

ILI9341_due      dtft = ILI9341_due(tft_cs, tft_dc);

// Color set
#define BLACK           0x0000
#define RED             0xF800
#define GREEN           0x07E0
#define BLUE            0x102E
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0 
#define ORANGE          0xFD20
#define GREENYELLOW     0xAFE5 
#define DARKGREEN       0x03E0
#define WHITE           0xFFFF

uint16_t  color;

//--------------------------------------------------------------------------------------------------

#define  lcdWhiteBlack()  {                                                                 \
   if(LCDTYPE==_UTFT_)      { qdUTFT.setColor(255,255,255); qdUTFT.setBackColor(  0,  0,  0);} \
   else if(LCDTYPE==_ILI9341_)   { tft.setTextColor(ILI9340_WHITE, ILI9340_BLACK) ;} \
   else if(LCDTYPE==_ILI9341due_)   { dtft.setTextColor(WHITE, BLACK) ;} \
}

#define  lcdNormal()      {                                                                 \
   if(LCDTYPE==_UTFT_)      { qdUTFT.setColor(255,255,255); qdUTFT.setBackColor(  0,  0,  0);} \
   else if(LCDTYPE==_ILI9341_)   { tft.setTextColor(ILI9340_WHITE, ILI9340_BLACK) ;} \
   else if(LCDTYPE==_ILI9341due_)   { dtft.setTextColor(WHITE, BLACK) ;} \
}

#define  lcdInvers()      {                                                                 \
   if(LCDTYPE==_UTFT_)      { qdUTFT.setColor(  0,  0,  0); qdUTFT.setBackColor(255,255,255);} \
   else if(LCDTYPE==_ILI9341_)   { tft.setTextColor(ILI9340_BLACK, ILI9340_WHITE) ;} \
   else if(LCDTYPE==_ILI9341due_)   { dtft.setTextColor(BLACK, WHITE) ;} \
}

#define  lcdWhiteRed()    {                                                                 \
   if(LCDTYPE==_UTFT_)      { qdUTFT.setColor(255,255,255); qdUTFT.setBackColor(255,  0,  0);} \
   else if(LCDTYPE==_ILI9341_)   { tft.setTextColor(ILI9340_WHITE, ILI9340_RED) ;} \
   else if(LCDTYPE==_ILI9341due_)   { dtft.setTextColor(WHITE, RED) ;} \
}

#define  lcdRedBlack()    {                                                                 \   
   if(LCDTYPE==_UTFT_)      { qdUTFT.setColor(255,  0,  0); qdUTFT.setBackColor(  0,  0,  0);} \
   else if(LCDTYPE==_ILI9341_)   { tft.setTextColor(ILI9340_RED, ILI9340_BLACK) ;} \
   else if(LCDTYPE==_ILI9341due_)   { dtft.setTextColor(RED, BLACK) ;} \
}

#define  lcdYellowBlue()  {                                                                 \     
   if(LCDTYPE==_UTFT_)      { qdUTFT.setColor(255,255,  0); qdUTFT.setBackColor( 64, 64, 64);} \
   else if(LCDTYPE==_ILI9341_)   { tft.setTextColor(ILI9340_YELLOW, ILI9340_BLUE);} \
   else if(LCDTYPE==_ILI9341due_)   { dtft.setTextColor(YELLOW, BLUE);} \
}

int16_t  fontwi= 8;  // default
int16_t  fonthi=10;  // default


void putfonttype(uint8_t fsize) {
  if(LCDTYPE==_UTFT_)  { 
    fontwi= qdUTFT.getFontXsize(); 
    fonthi= qdUTFT.getFontYsize(); 
  }
  else
  if(LCDTYPE==_ILI9341_) {
     if(fsize==_SmallFont_)     { fontwi= 6; fonthi=9; }  // 5x7 + overhead 
     else
     if(fsize==_MediumFont_)    { fontwi=12; fonthi=16; } // ?
     else
     if(fsize==_BigFont_)       { fontwi=18; fonthi=23; } // ?
  }
  else
  if(LCDTYPE==_ILI9341due_) {
     if(fsize==_SmallFont_)     { fontwi= 6; fonthi=9; }  // 5x7 + overhead 
  }
  _maxx_ = LCDmaxX / fontwi;    // max number of letters x>>
  _maxy_ = LCDmaxY / fonthi;    // max number of letters y^^ 
}


void setlcdorient(int16_t orient) {
  
  if(LCDTYPE==_ILI9341_) {
      tft.setRotation(orient);
      LCDmaxX=tft.width();
      LCDmaxY=tft.height();        
   }
   else
   if(LCDTYPE==_ILI9341due_) {
      dtft.setRotation((iliRotation)orient); 
      LCDmaxX=dtft.width();
      LCDmaxY=dtft.height();   
   }      
}


void lcdcls()  {                                                         
   if(LCDTYPE==_UTFT_)       { qdUTFT.clrScr();                }  
   else
   if(LCDTYPE==_ILI9341_)    { tft.fillScreen(ILI9340_BLACK);  }
   else
   if(LCDTYPE==_ILI9341due_) { dtft.fillScreen(BLACK);  }
   _curx_ =0;  _cury_ =0;
}


void curlf()   {                                                        
   _curx_=0; 
   if( _cury_ <=(LCDmaxY-10) ) _cury_+=fonthi; 
   else _cury_=0; 
     
   if(LCDTYPE==_ILI9341_)    { tft.setCursor(0, _cury_); }  
   else
   if(LCDTYPE==_ILI9341due_) { dtft.cursorToXY(0, _cury_); }  
}



void curxy(int16_t  x,  int16_t  y) {
   _curx_ = x;
   _cury_ = y; 
   if(LCDTYPE==_ILI9341_)      {tft.setCursor(x, y); }
   else
   if(LCDTYPE==_ILI9341due_)   {dtft.cursorToXY(x, y); }
}



void lcdprintxy(int16_t x, int16_t y, char * str) {
   if(LCDTYPE==_UTFT_)          { 
     qdUTFT.print(str,x,y); 
     _curx_=x+strlen(str)*fontwi; 
     _cury_=y; 
   }
   else if(LCDTYPE==_ILI9341_)  { 
      tft.setCursor(x,y);     
      tft.print(str); 
      _curx_=tft.getCursorX(); 
      _cury_=tft.getCursorY(); 
   }
   else if(LCDTYPE==_ILI9341due_)  { 
      dtft.cursorToXY(x,y);     
      dtft.printAt(str, x, y); 
      _curx_=x+strlen(str)*fontwi; 
      _cury_=y; 
   }
}


void lcdprint(char * str) {
    if(LCDTYPE==_UTFT_)     { 
      qdUTFT.print(str, _curx_, _cury_); 
      _curx_=_curx_+strlen(str)*fontwi; 
    }
    else if(LCDTYPE==_ILI9341_)  { 
       tft.setCursor(_curx_, _cury_); 
       tft.print(str); 
       _curx_=tft.getCursorX(); 
       _cury_=tft.getCursorY(); 
    }
    else if(LCDTYPE==_ILI9341due_)  { 
       dtft.cursorToXY(_curx_, _cury_); 
       dtft.printAt(str, _curx_, _cury_ ); 
      _curx_=_curx_+strlen(str)*fontwi;
    }
}



void initLCD(uint8_t orientation) { // 0,2==Portrait  1,3==Landscape
   if(LCDTYPE==_UTFT_) {
      qdUTFT.InitLCD(orientation%2);
      LCDmaxX=qdUTFT.getDisplayXSize();
      LCDmaxY=qdUTFT.getDisplayYSize();
      qdUTFT.setFont(SmallFont);
      putfonttype(UTFT_SmallFont);
   }
   else
   if(LCDTYPE==_ILI9341_) {
      tft.begin();
      setlcdorient(orientation);       
      tft.setTextSize(_SmallFont_);
      putfonttype(_SmallFont_);
   } 
   else
   if(LCDTYPE==_ILI9341due_) {
      dtft.begin();
      setlcdorient(orientation);  
      dtft.fillScreen(BLACK);
      dtft.setFont(SystemFont5x7); 
      dtft.setTextColor(WHITE);
      dtft.print("ILI9341due"); 
      putfonttype(_SmallFont_);
   }  
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
volatile int32_t motenc[MAXMOTORS],  oldenc[MAXMOTORS] ;

// buffer for motor commands
uint8_t   _motorstate_[7];      // 0: MOTORSLOT: MotNr
                                // 1: mot_runstate
                                // 2: pwm
                                // 3+4: mot enc_int16
                                // 5+6: mot enc_int32
                                



//*************************************************************************************
//*************************************************************************************
// ARDUINO GPIO PIN SETUP
//*************************************************************************************
//*************************************************************************************


//=====================================================================================
// MOTORS
//=====================================================================================
// 6 motors: pins 5-10 + 22-48

// motor 0
#define pinenc0A   22  // enc0A yellow
#define pinenc0B   23  // enc0B blue
#define pinmot0d1  24  // dir0-1   <<
#define pinmot0d2  25  // dir0-2
#define pinmot0pwm 10  // pwm enable0   

// motor 1
#define pinenc1A   26  // enc1A yellow
#define pinenc1B   27  // enc1B blue
#define pinmot1d1  28  // dir1-1   <<
#define pinmot1d2  29  // dir1-2
#define pinmot1pwm  9  // pwm enable1   


// motor 2
#define pinenc2A   30  // enc2A yellow
#define pinenc2B   31  // enc2B blue
#define pinmot2d1  32  // dir2-1   <<
#define pinmot2d2  33  // dir2-2
#define pinmot2pwm  8  // pwm enable2   

// motor 3
#define pinenc3A   34  // enc3A yellow
#define pinenc3B   35  // enc3B blue
#define pinmot3d1  36  // dir3-1   <<
#define pinmot3d2  37  // dir3-2
#define pinmot3pwm  7  // pwm enable3   

// motor 4
#define pinenc4A   38  // enc4A yellow
#define pinenc4B   39  // enc4B blue
#define pinmot4d1  40  // dir4-1   <<
#define pinmot4d2  41  // dir4-2
#define pinmot4pwm  6  // pwm enable4   

// motor 5
#define pinenc5A   42  // enc5A yellow
#define pinenc5B   43  // enc5B blue
#define pinmot5d1  47  // dir5-1   <<
#define pinmot5d2  48  // dir5-2
#define pinmot5pwm  5  // pwm enable5   



// optional: 6+2 = 8 motors
/*
// motor 6
#define pinenc6A   49  // enc6A yellow
#define pinenc6B   13  // enc6B blue
#define pinmot6d1  12  // dir6-1   
#define pinmot6d2  11  // dir6-2
#define pinmot6pwm 44  // only for Mega! pwm enable6  <<  %timer 5

// motor 7
#define pinenc7A    4  // enc7A yellow
#define pinenc7B    3  // enc7B blue
#define pinmot7d1   2  // dir7-1   
#define pinmot7d2  45  // dir7-2
#define pinmot7pwm 46  // only for Mega! pwm enable7  <<  %timer 5
*/

//=====================================================================================
// ANALOG PINS:   A0-A11
// DIGITAL PINS:  2,3,4,11,12,13, 51,52
//                49,50: TFT (dc, cs)
//=====================================================================================

               
// motor direction pin array               
byte pinmotdir[MAXMOTORS][ 2] = 
{
  {pinmot0d1, pinmot0d2},       
  {pinmot1d1, pinmot1d2},
  {pinmot2d1, pinmot2d2},
  {pinmot3d1, pinmot3d2},
  {pinmot4d1, pinmot4d2},
  {pinmot5d1, pinmot5d2},
};

int  pinmotpwm[MAXMOTORS] =      {pinmot0pwm, pinmot1pwm, pinmot2pwm,  // motor pwm pin array
                                  pinmot3pwm, pinmot4pwm, pinmot5pwm,
                                 };




//************************************************************************************
// Encoder functions courtesy of / entnommen aus: http: //www.meinDUINO.de //
//************************************************************************************

// Schritt-Tabellen für 1/1, 1/2 oder 1/4-Auflösung/resolution

// 1/1 Auflösung/resolution
//int8_t schrittTab[16] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};

// 1/2 Auflösung/resolution
int8_t  schrittTab[16] = {0, 0, 0, 0, 1, 0, 0, -1, 0, 0, 0, 1, 0, 0, -1, 0};

// 1/4 Auflösung/resolution
//int8_t schrittTab[16] = {0,0,0,0,0,0,0,-1,0,0,0,0,0,1,0,0};



//------------------------------------------------------------------------------------
void setupPins() {

  // motor pin settings 
  // setup for L293D motor driver type

  // motor 0
     pinMode(pinenc0A, INPUT_PULLUP);  // enc0A    yellow
     pinMode(pinenc0B, INPUT_PULLUP);  // enc0B    blue
     pinMode(pinmot0d1, OUTPUT);        // dir0-1   
     pinMode(pinmot0d2, OUTPUT);        // dir0-2   
     pinMode(pinmot0pwm ,OUTPUT);       // enable0 
       
     // motor 1
     pinMode(pinenc1A, INPUT_PULLUP);  // enc1A    yellow
     pinMode(pinenc1B, INPUT_PULLUP);  // enc1B    blue
     pinMode(pinmot1d1, OUTPUT);        // dir1-1   
     pinMode(pinmot1d2, OUTPUT);        // dir1-2 
     pinMode(pinmot1pwm, OUTPUT);       // enable1
       
     // motor 2
     pinMode(pinenc2A, INPUT_PULLUP);  // enc2A    yellow
     pinMode(pinenc2B, INPUT_PULLUP);  // enc2B    blue
     pinMode(pinmot2d1, OUTPUT);        // dir2-1 
     pinMode(pinmot2d2, OUTPUT);        // dir2-2   
     pinMode(pinmot2pwm, OUTPUT);       // enable2 
       
     // motor 3
     pinMode(pinenc3A, INPUT_PULLUP);  // enc3A     yellow
     pinMode(pinenc3B, INPUT_PULLUP);  // enc3B     blue
     pinMode(pinmot3d1, OUTPUT);        // dir3-1   
     pinMode(pinmot3d2, OUTPUT);        // dir3-2 
     pinMode(pinmot3pwm, OUTPUT);       // enable3 
       
     // motor 4
     pinMode(pinenc4A, INPUT_PULLUP);  // enc4A     yellow
     pinMode(pinenc4B, INPUT_PULLUP);  // enc4B     blue
     pinMode(pinmot4d1, OUTPUT);        // dir4-1   
     pinMode(pinmot4d2, OUTPUT);        // dir4-2 
     pinMode(pinmot4pwm, OUTPUT);       // enable4 
       
     // motor 5
     pinMode(pinenc5A, INPUT_PULLUP);  // encA5     yellow
     pinMode(pinenc5B, INPUT_PULLUP);  // encB5     blue
     pinMode(pinmot5d1, OUTPUT);        // dir5-1   
     pinMode(pinmot5d2, OUTPUT);        // dir5-2   
     pinMode(pinmot5pwm, OUTPUT);       // enable5   
  
}




//------------------------------------------------------------------------------------
// Interrupt Service Routine: encoder readings at Interrupt
//------------------------------------------------------------------------------------
volatile int8_t ISRab[MAXMOTORS]     = {0, 0, 0, 0, 0, 0};

void EcoderHandler() {
 
  ISRab [ 0] <<= 2;
  ISRab [ 0] &= B00001100;
  ISRab [ 0] |= (digitalRead(pinenc0A) << 1) | digitalRead(pinenc0B);
  motenc[ 0] += schrittTab[ISRab[0]];           //

  ISRab [ 1] <<= 2;
  ISRab [ 1] &= B00001100;
  ISRab [ 1] |= (digitalRead(pinenc1A) << 1) | digitalRead(pinenc1B);
  motenc[ 1] += schrittTab[ISRab[1]];           //

  ISRab [ 2] <<= 2;
  ISRab [ 2] &= B00001100;
  ISRab [ 2] |= (digitalRead(pinenc2A) << 1) | digitalRead(pinenc2B);
  motenc[ 2] += schrittTab[ISRab[2]];           //

  ISRab [ 3] <<= 2;
  ISRab [ 3] &= B00001100;
  ISRab [ 3] |= (digitalRead(pinenc3A) << 1) | digitalRead(pinenc3B);
  motenc[ 3] += schrittTab[ISRab[3]];           //

  ISRab [ 4] <<= 2;
  ISRab [ 4] &= B00001100;
  ISRab [ 4] |= (digitalRead(pinenc4A) << 1) | digitalRead(pinenc4B);
  motenc[ 4] += schrittTab[ISRab[4]];           //

  ISRab [ 5] <<= 2;
  ISRab [ 5] &= B00001100;
  ISRab [ 5] |= (digitalRead(pinenc5A) << 1) | digitalRead(pinenc5B);
  motenc[ 5] += schrittTab[ISRab[5]];           // 
}




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

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
// read+write bits in numbers
/* 
#define bitRead(source, bit) ( ((source) >> (bit)) & 0x01 )
#define bitSet (source, bit) ( (source) |= (1UL << (bit)) )
#define bitClear(source, bit) ( (source) &= ~(1UL << (bit)) )
#define bitWrite(source, bit, bitvalue) ( bitvalue ? bitSet(source, bit) : bitClear(source, bit) )
*/


//------------------------------------------------------------------------------------

int16_t  toggleup(int16_t  nr,  int16_t  max) {
  if ( nr < (max - 1) ) ++nr;
  else nr = 0;
  return nr;
}


//------------------------------------------------------------------------------------

#define sensortouch(pinHIGH) !digitalRead(pinHIGH)

//------------------------------------------------------------------------------------







//*************************************************************************************
//*************************************************************************************
// UART TRANSMISSION CONTROL 
//*************************************************************************************
//*************************************************************************************


const    uint8_t  MSGSIZE = 48;
uint8_t  bsync=255;
uint8_t  sendbuf[MSGSIZE];
uint8_t  recvbuf[MSGSIZE];


//=====================================================================================
// CHECKSUM
//=====================================================================================

uint8_t calcchecksum(uint8_t array[]) {
  int32_t  sum=0;
  for(int i=2; i<MSGSIZE; ++i) sum+=(array[i]);
  return (sum & 0x00ff);
}

bool checksumOK(uint8_t array[]){
return (calcchecksum(array)==array[1]);
}

//=====================================================================================
// UART SEND/RECEIVE BUFFER
//=====================================================================================
// addToBuffer and receive function courtesy of chucktodd

bool addToBuffer( uint8_t buf[], uint8_t *cnt, uint16_t timeout){
bool inSync = *cnt>0;
unsigned long start=millis();
while((*cnt<MSGSIZE)&&(millis()-start<timeout)){
  if(Serial1.available()){ // grab new char, test for sync char, if so start adding to buffer
    buf[*cnt] = (uint8_t)Serial1.read();
    if(inSync) *cnt += 1;  // my origional *cnt++ was updating the pointer address, not
                           // the pointed to sendbufue
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





//*************************************************************************************
//*************************************************************************************
// setup
//*************************************************************************************
//*************************************************************************************


const uint32_t UARTclock = 115200;

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
   initLCD(1);   
   Serial.println(" done.");   lcdcls();
   sprintf(sbuf, "LCD=%d wi%d x hi%d",LCDTYPE,LCDmaxX,LCDmaxY);
   Serial.println(sbuf);
   Serial.println();
   lcdcls(); lcdprint(sbuf);

   setupPins();
   
   sprintf(sbuf, "setup(): done.");
   Serial.println(); Serial.println(sbuf);   
   curlf(); curlf(); lcdprint(sbuf);

   lcdcls();
   
   sprintf(sbuf, "Rx slave, BAUD= %ld", UARTclock );;
   lcdprintxy(0, 0, sbuf);

}



//=====================================================================================
// DISPLAY VALUES
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
    //Serial.print(sbuf);                      // Print sendbufue to the Serial Monitor
  }   
  //Serial.println(); 
 
} 



//=====================================================================================
// L O O P
//=====================================================================================

void loop()
{ 
  char     sbuf[128],  resOK;   
  static   uint8_t cnt=0; 
  uint8_t  cbuf[MSGSIZE], chk;
  uint32_t xtime;
 

  //     Receive fromTx master Arduino

  memset(cbuf, 0, sizeof(cbuf)); 
   
  resOK = receive ( cbuf, 10000,&cnt);
 
  if( resOK ) {                                      // byte 0 == syncbyte ?
    cnt=0;

    //displayvalues(60, "Received...:", cbuf);
     chk=(byte)calcchecksum(cbuf);     
     memcpy(recvbuf, cbuf, sizeof(cbuf));
 
        // change values to send back!
        memcpy(sendbuf, recvbuf, sizeof(sendbuf));   // copy inbuf to outbuf
        sendbuf[4]+=1;                               // change [6] to send back 
        sendbuf[6]+=1;                               // change [6] to send back       
       
  }


 
  //   send to Tx master Arduino
 
  //Serial.println(); 
  sendbuf[0]=bsync;
  sendbuf[1]=calcchecksum(sendbuf);
  for(uint8_t i=0; i<MSGSIZE; i++) {       
     Serial1.write(sendbuf[i]);                      // Send value to the Rx Arduino       
  }       
  //Serial1.flush();                                 // clear output buffer
  //displayvalues(20, "Transmitted...: ", sendbuf);
  sprintf(sbuf, "%4d %4d", sendbuf[4], sendbuf[6]);
  lcdprintxy(0, 20, sbuf);
 
 
}


//=====================================================================================
//=====================================================================================

