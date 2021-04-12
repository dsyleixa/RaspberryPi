
/*
   UART communication
   send/receive string of tokens
   *     
   Raspberry Pi  master
   ver 0702
 */
 
 /*
  * change log
  * 0702: debug value (i2) from Raspi
  * 0701: Serial Read delimiter, debug value (i0) from Arduino
  * 0700: first adjustments
  * 0669: UART 115200 baud  * 
  * 0667: Arduino via USB = ACM0
  * 
  */


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

int32_t i0=0,i1=0,i2=0,i3=0,i4=0,i5=0,i6=0,i7=0,i8=0,i9=0;          // int (general)
//int32_t i0,i1,i2,i3,i4,i5,i6,i7,i8,i9;     


#define TOKLEN 30
#define MSGLEN 1024
#define iINVALID -29999


std::string  inputString="";
char   cval[TOKLEN];  // number as cstring
char   mbuf[MSGLEN];  // cstring msg buffer

volatile uint8_t   _TASKS_ACTIVE_= 1;






//==================================================================
// tools
//==================================================================


// conio.h - mimics

bool kbhit(void)
{
    struct termios original;
    tcgetattr(STDIN_FILENO, &original);

    struct termios term;
    memcpy(&term, &original, sizeof(term));

    term.c_lflag &= ~ICANON;
    tcsetattr(STDIN_FILENO, TCSANOW, &term);

    int characters_buffered = 0;
    ioctl(STDIN_FILENO, FIONREAD, &characters_buffered);

    tcsetattr(STDIN_FILENO, TCSANOW, &original);

    bool pressed = (characters_buffered != 0);

    return pressed;
}



// string tokens

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
        ||(i+pos>=(int)strlen(haystack))||(i>NLEN-2) ) {
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


// ============================================================ 
// addToBuffer and receive function courtesy of chucktodd

bool addToBuffer( uint8_t buf[], uint8_t *cnt, uint16_t timeout){
   bool inSync = *cnt>0;
   unsigned long start=millis();
   
   while((*cnt<MSGSIZE)&&(millis()-start<timeout)) {
     if( serialDataAvail( Serial ) ) { // grab new char, test for sync char, if so start adding to buffer
        buf[*cnt] = (uint8_t)serialGetchar( Serial );
        
        if(inSync) *cnt += 1;   
                                
        else {
           if(buf[*cnt]==0xFF) {
             inSync = true;
             *cnt +=1;
           }
        }
      }
   }
   return (*cnt==MSGSIZE);
}


void* thread0Name (void* ) {          // low priority: display values               // low priority: display values
    
    //printf("init graph...: ");
    setupGraphics();
    // DEBUG
    //printf(" done. \n\n");
           
    while(_TASKS_ACTIVE_) {
      cls();
      displayvalues();
      delay(40);   
    }   
    End();

    FinishShapes();  
    
    return NULL;   

}



void* UARTloop() {
    
  while(_TASKS_ACTIVE_) {  
      
     static bool stringComplete = false;
     
     //serialFlush(Serial);
     
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
     
     stringComplete=false;
     
     if (serialDataAvail(Serial)) 
     { 
       while(n<MSGLEN-1) {    
          if(n==MSGLEN-2) inChar='\n'; // emergency brake
          else  
            inChar = serialGetchar(Serial); 
       
          if(inChar=='\n' || inChar>=' ') inputString += inChar;       
          if (inChar == '\n')  {
            stringComplete = true;
            break;
          }          
          n++;
       } 
     }
   
     
     if (stringComplete)  {
         
       //inputString.to_str(mbuf, len-1);
       strcpy (mbuf, inputString.c_str() );
   
       fprintf(stderr,"\n"); fprintf(stderr,mbuf); //fprintf(stderr,"\n"); 
   
       
       // cstringarg( char* haystack, char* vname, char* carg )
       // haystack pattern: &varname=1234abc;  delimiters &, \n, \0, EOF 
   
       
       cstringarg(mbuf, "i0", cval); //    
       if(strlen(cval)>0) {          
          sprintf (cstr,  "i0=%d \n", (int32_t)atol(cval));
          fprintf(stderr, cstr);
       }   
       cstringarg(mbuf, "i1", cval); //    
       if(strlen(cval)>0) {          
          sprintf (cstr,  "i1=%d \n", (int32_t)atol(cval));
          fprintf(stderr, cstr);
       }
       cstringarg(mbuf, "i2", cval); //    
       if(strlen(cval)>0) {          
          sprintf (cstr,  "i2=%d \n", (int32_t)atol(cval));
          fprintf(stderr, cstr);
       }       
       
       
       inputString="";
       stringComplete = false;
   
       //delay?
       delay(1);
     }          
   
   }     
}

void* KBDloop(void *)           // low priority:  keyboard monitoring          // low priority:  keyboard monitoring
{
   int   c, ch;  // keyboard scancode, letter char
   
   while(_TASKS_ACTIVE_) {               
    
      c=0; ch=0;      
      echoOff();      
      
      if (kbhit())    {
          cursxy_lxt(0,11); printf(cleol_lxt);
          ch=getchar();      
          if(ch>=32 && ch<127) {
              printf("%c", ch);
              fflush(stdout);
          }   
          else if (ch<32) {
              if (kbhit()) ch=getchar();     
          }
          if(ch==27) _TASKS_ACTIVE_ = false;
     }     
     delay(50);
   }         
   return NULL;
}



//==================================================================

 
int main() {
    
    
    void  *retval0=NULL, *retval1=NULL, *retval2=NULL, *retval3=NULL;
    
    // UART Serial com port
    Serial = serialOpen (uart, UARTclock);   
    printf("starting UART..."); printf("\n");   
    
    
    pthread_t thread0, thread1, thread2, thread3, thread4, thread5, thread6;
    struct  sched_param  param;

    pthread_create(&thread0, NULL, thr0Name, NULL);    // medium priority task: console output
    param.sched_priority = 40;
    pthread_setschedparam(thread0, SCHED_RR, &param);
   
    pthread_create(&thread1, NULL, KBDloop, NULL);     // low priority: keyboard monitoring (stop program)
    param.sched_priority = 30;
    pthread_setschedparam(thread1, SCHED_RR, &param);
   
    pthread_create(&thread2, NULL, UARTloop, NULL);    // medium  priority: UART
    param.sched_priority = 40;
    pthread_setschedparam(thread2, SCHED_RR, &param);
    
     
    printf("initializing..."); printf("\n");
   
    
    
    
    while(_TASKS_ACTIVE_) { delay(1); }
     
    // wait for threads to join before exiting
    pthread_join(thread0, &retval0);
    pthread_join(thread1, &retval1);
    pthread_join(thread2, &retval2);
   
    serialClose( Serial);
    exit(0);
}


