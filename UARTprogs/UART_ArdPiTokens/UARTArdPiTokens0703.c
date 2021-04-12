
/*
   UART communication
   send/receive string of tokens
   *     
   Raspberry Pi  master
   ver 0703
 */
 
 /*
  * change log
  * 0703: pthread MT
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
#include <stdint.h>

#include <math.h>
#include <fcntl.h>
#include <string.h>
#include <string>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <errno.h>
#include <pthread.h>

#include <linux/input.h>
#include <termios.h>
#include <signal.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/select.h>

#include <wiringPi.h>
#include <wiringSerial.h>

#define  byte  uint8_t

char   uart[128]  =  "/dev/ttyACM0";
int    Serial;
const  uint32_t UARTclock = 115200;

int32_t i0=0,i1=0,i2=0,i3=0,i4=0,i5=0,i6=0,i7=0,i8=0,i9=0;          // int (general)
//int32_t i0,i1,i2,i3,i4,i5,i6,i7,i8,i9;     


#define TOKLEN      30
#define MSGLEN    1024
#define iINVALID -29999


std::string  inputString="";
char   cval[TOKLEN];  // number as cstring
char   mbuf[MSGLEN];  // cstring msg buffer


volatile uint8_t   _TASKS_ACTIVE_= 1;
pthread_mutex_t    _MutexUART_;








//==================================================================
// tools
//==================================================================

// ============================================================ 
// conio  

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

// ============================================================ 

void echoOff(void)
{
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);

    term.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

void echoOn(void)
{
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);

    term.c_lflag |= ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}




// ============================================================ 
// string tokens

int16_t  strstrpos(const char * haystack, const char * needle)   // find 1st occurance of substr in str
{
   const char *p = strstr(haystack, needle);
   if (p) return static_cast<int16_t>(p-haystack);
   return -1;   // Not found = -1.
}

// ============================================================ 

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
// threads
//==================================================================



void* THR0_thr (void* ) {          //   reserve
    
    while(_TASKS_ACTIVE_) {  
            
    }
    return NULL;   

}

//==================================================================


void* UART_thr( void* ) {
    
  while(_TASKS_ACTIVE_) {  
      
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
       
       pthread_mutex_lock( & _MutexUART_);        
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
       pthread_mutex_unlock( & _MutexUART_); 
          
       inputString="";
       stringComplete = false;
   
       //delay?
       delay(1);
     }             
   }     
   return NULL;
}

//==================================================================


void* KBD_thr(void*)           // low priority:  keyboard monitoring          // low priority:  keyboard monitoring
{
   int   ch;  // keyboard scancode, letter char
   
   while(_TASKS_ACTIVE_) {               
    
      ch=0;      
      echoOff();      
      
      if (kbhit())    {          
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
//==================================================================
 
int main() {
    
    // threads    
    pthread_t    /*thread0,*/ thread1, thread2;
    struct       sched_param  param;
    pthread_mutex_init ( & _MutexUART_, NULL);
    
    printf("starting ..."); printf("\n");   
    
    // open UART Serial com port    
    Serial = serialOpen (uart, UARTclock);       
        

    // start multithreading  
    
    pthread_create(&thread1, NULL, KBD_thr, NULL);     // medium priority: keyboard monitor 
    param.sched_priority = 60;
    pthread_setschedparam(thread1, SCHED_RR, &param);
   
    pthread_create(&thread2, NULL, UART_thr, NULL);    // medium priority: UART
    param.sched_priority = 60;
    pthread_setschedparam(thread2, SCHED_RR, &param);    
     
    
    while(_TASKS_ACTIVE_) { delay(10); }
     
    // wait for threads to join before exiting
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
   
    serialClose( Serial);
    exit(0);
}

//==================================================================

