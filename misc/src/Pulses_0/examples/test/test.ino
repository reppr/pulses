/* **************************************************************** */
/*
			test.ino

            http://github.com/reppr/pulses/
        Given as an example for the Menu library.


Copyright © Robert Epprecht  www.RobertEpprecht.ch   GPLv2



    Please do read README_test at the end of this file
*/

/* **************************************************************** */


/*  GNU General Public License GPLv2

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation version 2.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/


/* **************************************************************** */
// SOURCE CODE STARTS HERE:
/* **************************************************************** */

#include <stdio.h>
#include <stdlib.h>

#ifdef ARDUINO
  #if ARDUINO >= 100
    #include "Arduino.h"
  #else
    #include "WProgram.h"
  #endif
#else
  #include <iostream>
  #include <Pulses.cpp>		// why that?
#endif

#include <Pulses.h>

const int pl_max=16;

Pulses PULSES(pl_max);

/* BAUDRATE for Serial:	uncomment one of the following lines:	*/
#define BAUDRATE	115200		// works fine here
//#define BAUDRATE	57600
//#define BAUDRATE	38400
//#define BAUDRATE	19200
//#define BAUDRATE	9600		// failsafe
//#define BAUDRATE	31250		// MIDI



/* **************************************************************** */
#ifdef ARDUINO
/* Arduino setup() and loop():					*/

int get_free_RAM();
void setup() {
  Serial.begin(BAUDRATE);	// Start serial communication.
  Serial.println(F("\nTesting test.ino\n"));

  Serial.print(F("free RAM :\t"));
  Serial.println(get_free_RAM());

  Serial.println(F("\nPULSES.pulses_init()"));
  PULSES.pulses_init();

  Serial.print(F("free RAM :\t"));
  Serial.println(get_free_RAM());

  Serial.print(F("\nsizeof(pulse) "));
  Serial.print(sizeof(pulse));
  Serial.print(F(" * "));
  Serial.print(pl_max);
  Serial.print(F(" pulses = total "));
  Serial.println(sizeof(pulse)*pl_max);

Serial.println(F("\n(done)"));
}

void loop() {	// ARDUINO
}

#else		// c++ Linux PC test version

int main() {
  printf("\nTesting test.ino\n");

  printf("\nPULSES.pulses_init();\n\n");
  PULSES.pulses_init();
  printf("\n");

  printf("\nsizeof(pulse) %d * pl_max %d = total %d\n", sizeof(pulse), pl_max, sizeof(pulse)*pl_max );

}

#endif

/* **************************************************************** */
/*
  Determine RAM usage:
  int get_free_RAM() {
*/

#ifdef ARDUINO
  extern int __bss_end;
  extern void *__brkval;

  int get_free_RAM() {
    int free;

    if ((int) __brkval == 0)
      return ((int) &free) - ((int) &__bss_end);
    else
      return ((int) &free) - ((int) __brkval);
  }
#else			// not used yet on PC ;(	################
  int get_free_RAM() { return ILLEGAL; }
#endif



/* **************************************************************** */
/* README_test

README_test

Testing Pulses library in a very early stage
*not usable* for anything else...


   Copyright © Robert Epprecht  www.RobertEpprecht.ch   GPLv2

   http://github.com/reppr/pulses

*/
/* **************************************************************** */
