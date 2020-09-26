//  Raspberry Pi Master code to send/receive byte arrays
//  to an Arduino as an I2C slave
//  
//  ver. 0.002a
//
//  protected under the friendly Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License
//  http://creativecommons.org/licenses/by-nc-sa/3.0/   //   

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

#include <errno.h>
#include <string.h>

#define MSGSIZE 32


uint8_t  calcchecksum( uint8_t  array[]) {   
  int32_t  sum=0;
  for(int i=2; i<MSGSIZE; ++i) sum+=(array[i]);
  return (sum & 0x00ff);
}



int main (void)
{
	
  int fd, i ;
  uint8_t test=0; 
  uint8_t data [MSGSIZE] ;

  if ((fd = wiringPiI2CSetup (0x04) ) < 0)
  {
    fprintf (stderr, "Can't open RTC: %s\n", strerror (errno)) ;
    exit (EXIT_FAILURE) ;
  }


  for (;;)
  {
	memset(data, 0, sizeof(data) );
	data[0]=  0xff;   // init for transmission error check
    read (fd, data, MSGSIZE) ;
    if( data[1] != calcchecksum( data ) && (data[2] != 127) ) {
         // handle transmission error !
    }   
    else {
       printf ("  read:  ");
       for (i = 0 ; i < 7 ; ++i)
          printf ("  %3d", data [i]) ;
       printf ("  %3d", data [MSGSIZE-1]) ;
       //printf ("\n") ;
       delay(10) ;
      
       memset(data, 0, sizeof(data) );
       data[5]=  test++;
       data[0]=  0xff;
       data[MSGSIZE-1]= 0x04;
       data[1] = calcchecksum( data );
       
       write(fd, data, MSGSIZE) ;
       printf ("  write: ");
       for (i = 0 ; i < 7 ; ++i)
         printf ("  %3d", data [i]) ;
       printf ("  %3d", data [MSGSIZE-1]) ;
       printf ("\n\n") ;
       delay(10) ;     
    }
  }

  return 0 ;
}
