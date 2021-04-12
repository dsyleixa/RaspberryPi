// Arduino Serial to Raspi USB
// (C) 2018 by dsyleixa


// history:
// 0703  simple msg str 
// 0702
// 0701   Serial.read delimiters
// 0700   
// 0101   ported for RPi USB
//
// 0009  
// 0008  analog aX=analogRead(AX) (X=0...11), syntax "&AX=%ld",aX
// 0007  pin-loop, 6x out-pins OUTn(I/O/pwm), &_ALLPINS_=0 when BCB-close
// 0006  output pins DPINn
// 0003  send/receive strings
// 0002  receiving strings, pattern: &varname1=value1;
// 0001  receiving simple Serial char 

// BCB-Arduino: Arduino to RPi
// ver 0701


#define _DUE_MEGA_    // <<<<<<<<<<< outcomment/uncomment acc. to board type for available GPIO pins!


const uint32_t UARTclock = 115200;

int32_t i0=0,i1=0,i2=0,i3=0,i4=0,i5=0,i6=0,i7=0,i8=0,i9=0;          // int (general)


#define TOKLEN 30
#define MSGLEN 512
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
      if( (ch=='&')||(ch==';')||(ch=='\0') ||(ch=='\n')
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


  #endif
}


//==================================================================
// loop
//==================================================================

void loop() {

  //-------------------------------------------------------------
  // receive
  
  int  n=0;
  char inChar;     

  if(Serial.available()) {  
       inputString=Serial.readStringUntil('\n');    
       inputString.toCharArray(mbuf, min( (int)inputString.length(), MSGLEN-1) );     
  }
    
  //----------------------
  // process mbuf!

    

  //----------------------    
  inputString="";

  //delay 
  delay(1);
  

  
  //-------------------------------------------------------------
  // send  
  
  Serial.flush();
  
  char formatstr[MSGLEN];
  strcpy(formatstr, "§");
  strcat(formatstr, "&i0=%ld;&i1=%ld;&i2=%ld;&i3=%ld;&i4=%ld;&i5=%ld;&i6=%ld;\n");  
  sprintf(mbuf, formatstr, i0,i1,i2,i3,i4,i5,i6 );      
             
  //for (int i=0; i<strlen(mbuf); i++ ) Serial.print(mbuf[i]); 
  Serial.print(mbuf); 
   
   //delay?
  delay(1);
    
  i0++;

  
}




// end of file
