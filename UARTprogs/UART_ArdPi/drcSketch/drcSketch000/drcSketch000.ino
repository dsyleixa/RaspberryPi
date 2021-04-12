/*
 * drcAduino:
 *	Arduino code to implement DRC - The Drogon Remote Control.
 *	Allow another device talking to us over the serial port to control the IO pins.
 *
 *	Copyright (c) 2012 Gordon Henderson
 *
 * Full details at:
 *	http://projects.drogon.net/drogon-remote-control/drc-protocol-arduino/
 *
 * Commands:
 *	@: 0x40	Ping					Send back #: 0x23
 *	0: 0x30 0xNN	Set Pin NN OFF
 *	1: 0x31 0xNN	Set Pin NN ON
 *	i: 0x69 0xNN	Set Pin NN as Input
 *	o: 0x6F 0xNN	Set Pin NN as Output
 *	p: 0x6F 0xNN	Set Pin NN as PWM
 *	v: 0x6F 0xNN	Set PWM value on Pin NN 
 *	r: 0x72 0xNN	Read back digital Pin NN	Send back 0: 0x30 or 1: 0x31
 *	a: 0x61 0xNN	Read back analogue pin NN	Send back binary 2 bytes, Hi first.
 *	
 *********************************************************************************
 * This file is part of drcAduino:
 *	Drogon Remote Control for Arduino
 *	http://projects.drogon.net/drogon-remote-control/
 *
 *    drcAduino is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    drcAduino is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with drcAduino.  If not, see <http://www.gnu.org/licenses/>.
 *********************************************************************************
 */


// Serial commands

#define CMD_PING         '@'
#define CMD_PIN_0        '0'
#define CMD_PIN_1        '1'

#define CMD_PIN_I        'i'
#define CMD_PIN_O        'o'
#define CMD_PWM_PIN      'p'

#define CMD_RD_PIN       'r'
#define CMD_RA_PIN       'a'
#define CMD_PWM_VAL_PIN  'v'

#define CMD_DEBUG_DIGITAL       'D'
#define CMD_DEBUG_ANALOGUE      'A'

// Arduino with a 168 or 328 chip
//      ie. Arduino Classic, Demi, Uno.

#define MIN_APIN         0
#define MAX_APIN         5 // 11

#define MIN_DPIN         2
#define MAX_DPIN        13 // 52 

void setup ()
{
  int pin ;
  
  Serial.begin (115200) ;
  Serial.println ("DRC Arduino 1.0") ;
  pinMode (13, OUTPUT) ;
  digitalWrite (13, HIGH) ;
  for (pin = 2 ; pin < 13 ; ++pin)
  {
    digitalWrite (pin, LOW) ;
    pinMode (pin, INPUT) ;
  }
  analogReference (DEFAULT) ;
}

int myGetchar ()
{
  int x ;
  while ((x = Serial.read ()) == -1)
    ;
  return x ;
}

void loop ()
{
  unsigned int pin ;
  unsigned int aVal, dVal ;
  
  for (;;)
  {
    if (Serial.available () > 0)
    {
      switch (myGetchar ())
      {
        case CMD_PING:
          Serial.write (CMD_PING) ;
          continue ;

        case CMD_PIN_O:
          pin = Serial.read () ;
          if ((pin >= MIN_DPIN) && (pin <= MAX_DPIN))
            pinMode (pin, OUTPUT) ;
          continue ;

        case CMD_PIN_I:
          pin = myGetchar () ;
          if ((pin >= MIN_DPIN) && (pin <= MAX_DPIN))
            pinMode (pin, INPUT) ;
          continue ;

        case CMD_PIN_0:
          pin = myGetchar () ;
          if ((pin >= MIN_DPIN) && (pin <= MAX_DPIN))
            digitalWrite (pin, LOW) ;
          continue ;

        case CMD_PIN_1:
          pin = myGetchar () ;
          if ((pin >= MIN_DPIN) && (pin <= MAX_DPIN))
            digitalWrite (pin, HIGH) ;
          continue ;

        case CMD_RD_PIN:
          pin = myGetchar () ;
          if ((pin >= MIN_DPIN) && (pin <= MAX_DPIN))
            dVal = digitalRead (pin) ;
          else
            dVal = LOW ;
          Serial.write ((dVal == HIGH) ? '1' : '0') ;
          continue ;

        case CMD_RA_PIN:
          pin = myGetchar () ;
          if ((pin >= MIN_APIN) && (pin <= MAX_APIN))
            aVal = analogRead (pin) ;
          else
            aVal = 0 ;
          Serial.write ((aVal & 0xFF00) >> 8) ;        // High byte first
          Serial.write ( aVal & 0x00FF      ) ;
          continue ;

        case CMD_PWM_PIN:       
          pin = myGetchar () ;
          if ((pin >= MIN_DPIN) && (pin <= MAX_DPIN))
            pinMode (pin, OUTPUT) ;
          continue ;

        case CMD_PWM_VAL_PIN:
          pin  = myGetchar () ;
          dVal = myGetchar () ;
          if ((pin >= MIN_DPIN) && (pin <= MAX_DPIN))
            analogWrite (pin, dVal) ;
          continue ;
      }
    }
  }
}
