#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

#include <errno.h>
#include <string.h>

#define MSGSIZE 30


unsigned char  calcchecksum( unsigned char array[]) {   
  int32_t  sum=0;
  for(int i=2; i<MSGSIZE; ++i) sum+=(array[i]);
  return (sum & 0x00ff);
}



int main (void)
{
	
  int fd, i ;
  unsigned char test=0; 
  unsigned char data [MSGSIZE] ;

  if ((fd = wiringPiI2CSetup (0x04) ) < 0)
  {
    fprintf (stderr, "Can't open RTC: %s\n", strerror (errno)) ;
    exit (EXIT_FAILURE) ;
  }


  for (;;)
  {
    read (fd, data, MSGSIZE) ;
    printf ("read:  ");
    for (i = 0 ; i < MSGSIZE ; ++i)
      printf ("  %3d", data [i]) ;
    printf ("\n") ;
    delay(10) ;
   
    memset(data, 0, sizeof(data) );
    data[5]=  test++;
    data[0]=  0xff;
    data[MSGSIZE-1]= 0x04;
    data[1] = calcchecksum( data );
    
    write(fd, data, MSGSIZE) ;
    printf ("write: ");
    for (i = 0 ; i < MSGSIZE ; ++i)
      printf ("  %3d", data [i]) ;
    printf ("\n\n") ;
    delay(10) ;
  }

  return 0 ;
}
