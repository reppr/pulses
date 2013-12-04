/* **************************************************************** */
/*
		    inputs_test.ino


            http://github.com/reppr/pulses/

Copyright © Robert Epprecht  www.RobertEpprecht.ch   GPLv2

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

#ifndef ARDUINO
  #error Arduino only
#endif

/* Keep ARDUINO GUI happy ;(		*/
#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include <Inputs.h>


/* BAUDRATE for Serial:	uncomment one of the following lines:	*/
#define BAUDRATE	115200		// works fine here
//#define BAUDRATE	57600
//#define BAUDRATE	38400
//#define BAUDRATE	19200
//#define BAUDRATE	9600		// failsafe
//#define BAUDRATE	31250		// MIDI


// Inputs(int inp_max);
Inputs INPUTS(4);
int inp;


int test_sample_method(int addr) {
  int value;

  Serial.print("TESTING test_sample_method\taddr=");
  Serial.print(addr);

  value=analogRead(addr);

  Serial.print("\tvalue=");
  Serial.print(value);

  Serial.println();

  return value;
}

void setup() {
  #ifdef BAUDRATE
    Serial.begin(BAUDRATE);
  #endif

  inp=0;
  INPUTS.setup_sample_method(inp, &test_sample_method, 4);
}


int samples=0;	// counters are private, so we double it here
void loop() {
  samples++;	// counters are private, so we double it here

  if (INPUTS.sample(inp)) {
    Serial.print("==> TRIGGER@");
    Serial.println(samples);
  }

  delay(1200);
}
