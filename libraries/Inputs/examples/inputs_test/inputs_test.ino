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

  INPUTS.setup_sample_method(0, &test_sample_method, 0, 4);
  INPUTS.setup_sample_method(1, &test_sample_method, 1, 4);
}


int cnt=0;
void loop() {
  cnt++;

  inp=0;
  if (INPUTS.sample(inp)) {
    Serial.print("\t==> TRIGGER@");
    Serial.println(INPUTS.get_counter(inp));
  }
  Serial.print("\tcounter=");
  Serial.println(INPUTS.get_counter(inp));

  if ((cnt % 3) == 0 ) {
    inp=1;
    if (INPUTS.sample(inp)) {
      Serial.print("\t==> TRIGGER@");
      Serial.println(INPUTS.get_counter(inp));
    }
    Serial.print("\tcounter=");
    Serial.println(INPUTS.get_counter(inp));
  }

  delay(1200);
}
