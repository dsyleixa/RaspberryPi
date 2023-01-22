#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <pigpio.h>

// local defines
typedef struct
{
	uint16_t		frequency;
	uint16_t		duration;
}TONE_DEF;

#define	TONE_PIN 18		// use GPIO 18

// local variables

static TONE_DEF		playTune[] =	
{
		{ 587, 625 },		// D5
		{ 659, 625 },		// E5
		{ 523, 625 },		// C5
		{ 262, 625 },		// C4
		{ 392, 625 },		// G4
		{ 0 , 0 }			// end marker
};

// local function prototypes

static void tone( uint16_t freq, uint16_t duration );

// main program entry

int main(int argc, char *argv[])
{
	int		iCount = 0;

	if ( gpioInitialise() == PI_INIT_FAILED )
	{
		printf("gpioInitialise() failed\n");
		return -1;
	}

	do
	{
		tone( playTune[iCount].frequency, playTune[iCount].duration );	// play a tone
		usleep ( 20000 );			// short ( 20 mS ) delay between tones
		iCount++;
	} while ( playTune[iCount].frequency != 0 );

	return 0;
}

// tone function.... frequency in Hz, duration in milliseconds

static void tone( uint16_t freq, uint16_t duration )
{
	gpioHardwarePWM( TONE_PIN, freq, 500000 );
	usleep(duration * 1000 );
	gpioHardwarePWM( TONE_PIN, 0, 0 ); 
}