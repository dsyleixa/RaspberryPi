
/*
 
 ***********************************************************************
 *    This file is part of wiringPi:
 *    https://projects.drogon.net/raspberry-pi/wiringpi/
 *
 *    Encoder Readind >>>>  !! CAUTION !!  NOT TESTED !!  <<<< 
 *
 *    You should have received a copy of the GNU Lesser General Public License
 *    along with wiringPi.  If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <wiringPi.h>  // -I/usr/local/include -L/usr/local/lib -lwiringPi


int encoderPin1 = 2;
int encoderPin2 = 3;

volatile int  lastEncoded  = 0;
volatile long encoderValue = 0;

volatile long lastencoderValue = 0;

int lastMSB = 0;
int lastLSB = 0;


/*
 * ISR updateEncoder:
 *********************************************************************************
 */

void updateEncoder (void)
{
  int MSB = digitalRead(encoderPin1);       //MSB = most significant bit
  int LSB = digitalRead(encoderPin2);       //LSB = least significant bit
 
  int encoded = (MSB << 1) |LSB;            //converting the 2 pin value to single number
  int sum  = (lastEncoded << 2) | encoded;  //adding it to the previous encoded value

  if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderValue ++;
  if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderValue --;

  lastEncoded = encoded;                    //store this value for next time
}


/*
 *********************************************************************************
 * main
 *********************************************************************************
 */

int main (void)
{

  // setup 
  if (wiringPiSetup () < 0)  {
    fprintf (stderr, "Unable to setup wiringPi: %s\n", strerror (errno)) ;
    return 1 ;
  }
 
  pinMode(encoderPin1, INPUT);  // attach pullup resistor !
  pinMode(encoderPin2, INPUT);  // attach pullup resistor !

  pullUpDnControl(encoderPin1, PUD_UP); // Enable pull-up resistor on encoder pin
  pullUpDnControl(encoderPin2, PUD_UP); // Enable pull-up resistor on encoder pin
 

  // ISR init
  if (wiringPiISR (encoderPin1, INT_EDGE_FALLING, &updateEncoder) < 0)  {
    fprintf (stderr, "Unable to setup ISR: %s\n", strerror (errno)) ;
    return 1 ;
  }
  if (wiringPiISR (encoderPin2, INT_EDGE_FALLING, &updateEncoder) < 0)  {
    fprintf (stderr, "Unable to setup ISR: %s\n", strerror (errno)) ;
    return 1 ;
  }

 

  // loop
 
  for (;;)  {   
    printf ("%3d: %6ld\n",  1, encoderValue);
    delay(100);
  }

  return 0 ;
}


