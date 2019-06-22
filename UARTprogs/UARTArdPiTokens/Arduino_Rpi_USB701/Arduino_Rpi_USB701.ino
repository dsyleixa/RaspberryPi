// Arduino Serial to Raspi USB
// (C) 2018 by dsyleixa


// history:
// 0701   Serial.read delimiters
// 0700   ported to RPi comm
//
// 0.0.9  
// 0.0.8  analog aX=analogRead(AX) (X=0...11), syntax "&AX=%d",aX
// 0.0.7  pin-loop, 6x out-pins OUTn(I/O/pwm), &_ALLPINS_=0 when BCB-close
// 0.0.6  output pins DPINn
// 0.0.3  send/receive strings
// 0.0.2  receiving strings, pattern: &varname1=value1;
// 0.0.1  receiving simple Serial char 

// BCB-Arduino: Arduino to RPi
// ver 0701


// #define _DUE_MEGA_    // <<<<<<<<<<< outcomment/uncomment acc. to board type for available GPIO pins!


// i2c
#include <Wire.h>   // Incl I2C comm, but needed for not getting compile error

const uint32_t UARTclock = 115200;

int    i0=0,i1=0,i2=0,i3=0,i4=0,i5=0,i6=0,i7=0,i8=0,i9=0;          // int (general)
int    a0=0,a1=0,a2=0,a3=0,a4=0,a5=0,a6=0,a7=0,a8=0,a9=0,a10=0,a11=0;  // int for analogRead(pin): a0=analogRead(A0)=0, a1=analogRead(A1),... 
double f0=0,f1=0,f2=0,f3=0,f4=0,f5=0,f6=0,f7=0,f8=0,f9=0;          // double (general)

#define TOKLEN 30
#define MSGLEN 1024
#define iINVALID -29999

String  inputString="";
char    cval[TOKLEN];  // number as cstring
char    mbuf[MSGLEN];  // cstring msg buffer


//==================================================================
// tools
//==================================================================

int16_t  strstrpos(char * haystack,  char * needle)   // find 1st occurance of substr in str
{
   char *p = strstr(haystack, needle);
   if (p) return (int16_t)(p - haystack);
   return -1;   // Not found = -1.
}


//------------------------------------------------------------
char * cstringarg( char* haystack, char* vname, char* carg ) {
   int i=0, pos=-1;
   unsigned char  ch=0xff;
   const char*  kini = "&";       // start of varname: '&'
   const char*  kin2 = "?";       // start of varname: '?'
   const char*  kequ = "=";       // end of varname, start of argument: '='
   char  needle[TOKLEN] = "";     // complete pattern:  &varname=abc1234

   strcpy(carg,"");
   strcpy(needle, kini);
   strcat(needle, vname);
   strcat(needle, kequ);
   pos = strstrpos(haystack, needle); 
   if(pos==-1) {
      needle[0]=kin2[0];
      pos = strstrpos(haystack, needle);
      if(pos==-1) return carg;
   }
   pos=pos+strlen(vname)+2; // start of value = kini+vname+kequ   
   while( (ch!='&')&&(ch!='\0') ) {
      ch=haystack[pos+i];    
      if( (ch=='&')||(ch==';')||(ch==' ')||(ch=='\0') ||(ch=='\n')
        ||(i+pos>=(int)strlen(haystack))||(i>TOKLEN-1) ) {
           carg[i]='\0';
           return carg;
      }       
      if( (ch!='&') ) {
          carg[i]=ch;          
          i++;       
      }      
   } 
   return carg;
}

//------------------------------------------------------------

void writeDPin(int pin, int ival) {
  pinMode(pin, OUTPUT);  // safety
  if(ival==0)   { digitalWrite(pin, LOW); }
  else
  if(ival==255) { digitalWrite(pin, HIGH); }
  else {       
     analogWrite(pin, ival); 
  }   
}




//==================================================================
// setup
//==================================================================
void setup() {
  Serial.begin(UARTclock);
  
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  #define _DEBUG_      // <<<<<<<<<<< outcomment/uncomment for testing !!
  
  #ifdef _DEBUG_
  
  i0=0;
  i1=1;
  i2=-22;
  i3=33;
  i4=-444;
  i5=5555;
  i6= B10101010*256;
  i6+=B10101010;

  f0=0;
  f1=10.1; 
  f2=-20.22;
  f3=300.333;
  f4=-4000.4444;
  f5=50000.55555;

  a0=0;
  a1=1;
  a2=2;
  a3=3;
  a4=4;
  a5=5;
  a6=6;
  a7=7;
  a8=8;
  a9=9;
  a10=10;
  a11=11;

  #endif
}


//==================================================================
// loop
//==================================================================

void loop() {
  static bool stringComplete = false;

  //-------------------------------------------------------------
  // receive
  
  int  n=0;
  char inChar;
     
  while (Serial.available() && n<MSGLEN-1) { 
       if(n==MSGLEN-2) inChar='\n';
       else  
       inChar = inChar = (char)Serial.read();
    
       inputString += inChar;
       
       if (inChar == '\n')  {
         stringComplete = true;
       }
       n++;
  }

  if (stringComplete)  {
    inputString.toCharArray(mbuf, MSGLEN-1);
    
    // cstringarg( char* haystack, char* vname, char* carg )
    // haystack pattern: &varname=1234abc;  delimiters &, \n, \0, SPACE, EOF 
    
    cstringarg(mbuf, "LEDBI", cval); // LEDBI: LED_BUILTIN    
    if(strlen(cval)>0) {          
      writeDPin(LED_BUILTIN, atoi(cval) );     
    }  


    
    //----------------------
    // adjust output pins for DUE & MEGA!!
    
    #if defined ( __AVR_ATmega2560__ ) || defined  ( __SAM3X8E__ )
    
    for(int o=22; o<54; o++) {        
       char keyw[20];
       sprintf(keyw, "OUT%d", o);     // OUT2, OUT3, ...
       cstringarg(mbuf, keyw, cval); 
       if(strlen(cval)>0) {          
          writeDPin(o, atoi(cval));      
       } 
       cstringarg(mbuf, "_ALLPINS_", cval); 
       if(strlen(cval)>0) {        
           writeDPin(o, atoi(cval));      
       } 
    }
    #endif
    //----------------------
    
    inputString="";
    stringComplete = false;

    //delay?
    delay(10);
  }
  //-------------------------------------------------------------
  // send  
  

  //----------------------
  
  char formatstr[MSGLEN];
  strcpy(formatstr, "§");
  strcat(formatstr, "&i0=%d;&i1=%d;&i2=%d;&i3=%d;&i4=%d;&i5=%d;&i6=%d;");
  strcat(formatstr, "&f0=%f;&f1=%f;&f2=%f;&f3=%f;&f4=%f;&f5=%f;");   
  strcat(formatstr, "&A0=%d;&A1=%d;&A2=%d;&A3=%d;&A4=%d;&A5=%d;&A6=%d;&A7=%d;&A8=%d;&A9=%d;&A10=%d;&A11=%d;\n");

  sprintf(mbuf, formatstr, i0,i1,i2,i3,i4,i5,i6, f0,f1,f2,f3,f4,f5, a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11 );                 
  for (int i=0; i<strlen(mbuf); i++ ) Serial.print(mbuf[i]);
  
    
  i0++;

  //delay?
  delay(10); 
}




// end of file
