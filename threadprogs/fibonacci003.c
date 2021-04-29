/*
 *  Fibonacci numbers
 *  simulaneous competition:
 *  linear vs. recursive calculation
 * 
 */


#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <termio.h>
#include <limits.h>

#define msleep(ms)  usleep(1000*ms)

pthread_t threadID0, threadID1, threadID2;

long f;

volatile int running1 = 1;
volatile int running2 = 1;


// ********************************************************************


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

// ********************************************************************


inline unsigned long long fibolinear( long &n ) {
  volatile unsigned long long  fibarr[3]; 
  long i;

  fibarr[0]=0;
  fibarr[1]=1;
  fibarr[2]=0;

  if(n==0) return 0;

  for (i=1; i <= n; i++) {
     
      fibarr[0]=fibarr[1];
      fibarr[1]=fibarr[2];
      if( ULLONG_MAX - fibarr[1] < fibarr[0] ) // ==> Ueberlauf
      {
          n=i-1;
          printf("\n overflow for Fibonacci order (linear): %ld  \n" , i); 
          return fibarr[2];
      }
      fibarr[2]=fibarr[1]+fibarr[0];
      printf("\n order linear: %ld  %llu \n" , i, fibarr[2]);       
  }
  return fibarr[2];
 
}


volatile  long orderr = 0;

inline unsigned long long fiborecurs( long n ) {
  unsigned long long      llbuf, llbuf_1, llbuf_2;  


  if ( n==0 )    { llbuf = 0;  }
  else   
  if ( n==1 )    { llbuf = 1;  }
  else {
     llbuf_1 = fiborecurs(n-1);
     llbuf_2 = fiborecurs(n-2);  
      
     if( ULLONG_MAX - llbuf_1 < llbuf_2 ) // ==> Ueberlauf
     {  
          printf("\n overflow for Fibonacci order (recursive): %ld  \n" , orderr); 
          return llbuf_1;
     }      
     llbuf = llbuf_1 + llbuf_2;         
     if(n > orderr) {
         orderr = n;
         printf("\n order recursive: %ld  %llu \n" , orderr, llbuf); 
     }     
  }
  return (llbuf); 
}



// ********************************************************************




void* threadf2(void *) {
    unsigned long long res;
    int RETVAL = 0;
       
    res = fibolinear(f);
    
    printf("\n\nFibonacci (%ld) (linear) is = %llu \n", f, res);  
    printf("===============================================\n\n");      
    
    running2 = 0;
    msleep(100);
    
    return (void*)RETVAL; ;
    
}



void* threadf1(void *) {
    unsigned long long res;
    int RETVAL = 0;
 
    res = fiborecurs(f);
    
    printf("\n\nFibonacci (%ld) (recursive) is = %llu \n", orderr, res); 
    printf("===============================================\n\n");    
    
    running1 = 0;
    msleep(100);
    
    return (void*)RETVAL; ;
    
}




void* threadW0(void *) {
    int RETVAL = 0, i=0;
    
    while(running1 || running2) {
      ++i;
      
      if (kbhit())
      {
         int ch = getchar();
         if(ch==27) {       
            // DEBUG    
            printf("\nin pthread0: user kbd press ESC...  ");  
            printf("\npthread0 cancelling pthread 1... ");                         
            if(threadID1) pthread_cancel(threadID1);
            running1=0;
            // DEBUG 
            printf("\npthread0 cancelling pthread 2...");             
            if(threadID2) pthread_cancel(threadID2);
            running2=0;
            
            // DEBUG 
            printf("\nin pthread0: program interrupted by user  \n");
            
            RETVAL = 27;  // user break  
            return (void*)RETVAL; 
         }   
      }   
      msleep(10);
      if(i==100) {
         printf("."); 
         i=0;
      } 
      fflush(stdout);
    }
    return (void*)RETVAL; 
    
}

// ********************************************************************


int main() {	   
    void *retval0 = NULL, *retval1 = NULL, *retval2 = NULL;
    
    printf("enter Fibonacci order: ");
    scanf("%ld", &f);  
    
    pthread_create( &threadID2, 0, threadf2, 0 ); 
    pthread_create( &threadID1, 0, threadf1, 0 );  
    pthread_create( &threadID0, 0, threadW0, 0 ); 
    
         
    pthread_join( threadID0,  &retval0 );
    pthread_join( threadID1,  &retval1 );    
    pthread_join( threadID2,  &retval2 );
    
    // DEBUG
    printf("\nretval0 = %d \n", (int)retval0);
    printf("\nretval1 = %d \n", (int)retval1);
    printf("\nretval2 = %d \n", (int)retval2);
    printf("\nprogram finished \n");


	return 0;
    
}

