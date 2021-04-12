

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <pthread.h>

#include <wiringPi.h>
#include <wiringSerial.h>


#define TOKLEN 30

int16_t  strstrpos(char * haystack,  char * needle)   // find 1st occurance of substr in str
{
   char *p = strstr(haystack, needle);
   if (p) return p - haystack;
   return -1;   // Not found = -1.
}


char * cstringarg( char* haystack, char* vname, char* sarg ) {
   int i=0, pos=-1;
   unsigned char  ch=0xff;
   const char*  kini = "&";       // start of varname: '&'
   const char*  kin2 = "?";       // start of varname: '?'
   const char*  kequ = "=";       // end of varname, start of argument: '='
   char  needle[TOKLEN] = "";     // complete pattern:  &varname=abc1234


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
        ||(i+pos>=strlen(haystack))||(i>TOKLEN-1) ) {
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


//
int main() {
   char cbuf[256]="&xval=10;&GPIO13=1;&yval=200;\n ";
   char cval[30];
   int x, y, GPIO13;
  
   cstringarg(cbuf, "GPIO13", cval);     
   if(strlen(cval)>0) {          
      GPIO13=atoi(cval);     
   }  
   cstringarg("&xval=10;&GPIO13=1;&yval=200;\n ", "xval", cval);     
   if(strlen(cval)>0) {          
      x=atoi(cval) ;     
   }  

   char test[20];
   strcpy(test, "yval");
   cstringarg(cbuf, test, cval);     
   if(strlen(cval)>0) {          
      y=atoi(cval) ;     
   }  
   
   printf("x=%d, y=%d, GPIO=%d \n", x,y,GPIO13);
   return 0;


   //
}
