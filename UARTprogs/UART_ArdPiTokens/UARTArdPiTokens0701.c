
/*
   UART communication
   send/receive string of tokens
   *     
   Raspberry Pi  master
   ver 0701
 */
 
 /*
  * change log
  * 0701:
  * Serial Read delimiter
  * 0700
  * first adjustments
  * 0669:
  * UART 115200 baud  * 
  * 0667:
  * Arduino via USB = ACM0
  * 
  */
 
// (C) dsyleixa 2015
// freie Verwendung für private Zwecke
// für kommerzielle Zwecke nur nach Genehmigung durch den Autor.
// Programming language: gcc C/C++
// protected under the friendly Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License
// http://creativecommons.org/licenses/by-nc-sa/3.0/



#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
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

char   uart[128]  =  "/dev/ttyACM0";
int    Serial;
const  uint32_t UARTclock = 115200;

//int32_t i0=0,i1=0,i2=0,i3=0,i4=0,i5=0,i6=0,i7=0,i8=0,i9=0;          // int (general)
int32_t i0,i1,i2,i3,i4,i5,i6,i7,i8,i9;     


#define TOKLEN 30
#define MSGLEN 1024
#define iINVALID -29999


std::string  inputString="";
char   cval[TOKLEN];  // number as cstring
char   mbuf[MSGLEN];  // cstring msg buffer






//==================================================================
// debug monitor
//==================================================================

void displayvalue(char c[TOKLEN]) {
  
  
}



//==================================================================
// token tools
//==================================================================




int16_t  strstrpos(const char * haystack, const char * needle)   // find 1st occurance of substr in str
{
   const char *p = strstr(haystack, needle);
   if (p) return static_cast<int16_t>(p-haystack);
   return -1;   // Not found = -1.
}


char * cstringarg( const char* haystack, const char* vname, char* sarg ) {
   int i=0, pos=-1;
   unsigned char  ch=0xff;
   const char*  kini = "&";       // start of varname: '&'
   const char*  kin2 = "?";       // start of varname: '?'
   const char*  kequ = "=";       // end of varname, start of argument: '='
   
   const int    NLEN=30;
   char  needle[NLEN] = "";       // complete pattern:  &varname=abc1234


   strcpy(sarg,"");
   strcpy(needle, kini);
   strcat(needle, vname);
   strcat(needle, kequ);
   pos = strstrpos(haystack, needle); 
   if(pos==-1) {
      needle[0]=kin2[0];
      pos = strstrpos(haystack, needle);
      if(pos==-1) return sarg;
   }
   pos=pos+strlen(vname)+2; // start of value = kini+vname+kequ   
   while( (ch!='&')&&(ch!='\0') ) {
      ch=haystack[pos+i];    
      if( (ch=='&')||(ch==';')||(ch=='\0') ||(ch=='\n')
        ||(i+pos>=(int)strlen(haystack))||(i>NLEN-1) ) {
           sarg[i]='\0';
           return sarg;
      }       
      if( (ch!='&') ) {
          sarg[i]=ch;          
          i++;       
      }      
   } 
   return sarg;
}



//==================================================================
// serial TCP
//==================================================================


void loop() {
    
  while(1) {  
      
     static bool stringComplete = false;
     
     
     //-------------------------------------------------------------
     // send    
     
     // debug
     i2++;  // change value to send back to Arduino
     
     char formatstr[MSGLEN];     

     // debug, cut-down:
     strcpy(formatstr, "§");
     strcat(formatstr, "&i0=%d;&i1=%d;&i2=%d;&i3=%d;\n");
     sprintf(mbuf, formatstr, i0,i1,i2,i3);
                    
     for(uint8_t i=0; i<strlen(mbuf); i++) {           //
        serialPutchar( Serial, mbuf[i]);               // Send values to the slave       
     }   
      
     
     //delay?
     delay(1); 
   
     
     
     
     //-------------------------------------------------------------
     // receive
     
     inputString="";
     char cstr[TOKLEN];
   
     int  n=0;
     char inChar;
     
     
     if (serialDataAvail(Serial)) { 
       while(!stringComplete && n<MSGLEN-1) {    
          if(n==MSGLEN-2) inChar='\n'; // emergency brake
          else  
            inChar = serialGetchar(Serial); 
       
          if(inChar=='\n' || inChar>=' ') inputString += inChar;       
          if (inChar == '\n')  {
            stringComplete = true;
          }
          n++;
       } 
     }
   
     
     if (stringComplete)  {
         
       //inputString.to_str(mbuf, len-1);
       strcpy (mbuf, inputString.c_str() );
   
       fprintf(stderr,"\n"); fprintf(stderr,mbuf); //fprintf(stderr,"\n"); 
   
       
       // cstringarg( char* haystack, char* vname, char* carg )
       // haystack pattern: &varname=1234abc;  delimiters &, \n, \0, SPACE, EOF 
   
       
       cstringarg(mbuf, "i0", cval); //    
       if(strlen(cval)>0) {          
          sprintf (cstr,  "i0=%d \n", atoi(cval));
          fprintf(stderr, cstr);
       }   
       cstringarg(mbuf, "i1", cval); //    
       if(strlen(cval)>0) {          
          sprintf (cstr,  "i1=%d \n", atoi(cval));
          fprintf(stderr, cstr);
       }
       cstringarg(mbuf, "i2", cval); //    
       if(strlen(cval)>0) {          
          sprintf (cstr,  "i2=%d \n", atoi(cval));
          fprintf(stderr, cstr);
       }
       cstringarg(mbuf, "i3", cval); //    
       if(strlen(cval)>0) {          
          sprintf (cstr,  "i3=%d \n", atoi(cval));
          fprintf(stderr, cstr);
       }  
       cstringarg(mbuf, "i4", cval); //    
       if(strlen(cval)>0) {          
          sprintf (cstr,  "i4=%d \n", atoi(cval));
          fprintf(stderr, cstr);
       }   
       
       inputString="";
       stringComplete = false;
   
       //delay?
       delay(1);
     }
   
   }     
}




//==================================================================

 
int main() {
     
    printf("initializing..."); printf("\n");
   
    // UART Serial com port
    Serial = serialOpen (uart, UARTclock);   
    printf("starting UART loop..."); printf("\n");   
    
    loop(); 
   
    serialClose( Serial);
    exit(0);
}



//
// eof

