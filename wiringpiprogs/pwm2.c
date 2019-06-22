/*
 * pwm.c:
 *	Test of the software PWM driver. Needs 12 LEDs connected
 *	to the Pi.
 *
 * Copyright (c) 2012-2013 Gordon Henderson. <projects@drogon.net>
 ***********************************************************************
 * This file is part of wiringPi:
 *	https://projects.drogon.net/raspberry-pi/wiringpi/
 *
 *    wiringPi is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    wiringPi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public License
 *    along with wiringPi.  If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <wiringPi.h>
#include <softPwm.h>

#define RANGE		100
#define	NUM_LEDS	  2             // 2 LEDs


int ledMap [NUM_LEDS] = { 0, 1 } ;  // 0=GPIO_17,  1=GPIO_18



int main ()
{
  short  i, j ;

  if (wiringPiSetup () == -1)
  {
    fprintf (stdout, "oops: %s\n", strerror (errno)) ;
    return 1 ;
  }
  
  for (i = 0 ; i < NUM_LEDS ; ++i) {         // init softPWM
    softPwmCreate (ledMap [i], 0, RANGE) ;      
  }
    
  
  while(1) {                                 // loop:

    for (j=0; j<100; ++j)  {                 // all LEDs 0...100
	   for (i = 0 ; i < NUM_LEDS ; ++i) {
         softPwmWrite (ledMap [i], j) ;
         
         printf ("%3d, %3d, %3d\n", i, ledMap [i], j) ;
         delay(20);
       }
    }
    
    for (j=100; j>0; --j)  {                 // all LEDs 100...0
	   for (i = 0 ; i < NUM_LEDS ; ++i) {
         softPwmWrite (ledMap [i], j) ;
         printf ("%3d, %3d, %3d\n", i, ledMap [i], j) ;
         delay(20);
       }
    }
  }
  
}
  
  
  
