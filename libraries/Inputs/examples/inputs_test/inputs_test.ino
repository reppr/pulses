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

#define INPUTS_DEBUGGING_ALL

#ifdef INPUTS_DEBUGGING_ALL
  #define INPUTS_DEBUGGING_SAMPLING
  #define INPUTS_DEBUGGING_IO_CALCULATING
#endif


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

#ifdef INPUTS_DEBUGGING_SAMPLING
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
#endif


#ifdef INPUTS_DEBUGGING_IO_CALCULATING
bool i2o_p_type_is_int=false;
bool i2o_p_type_is_long_int=false;
bool i2o_p_type_is_float=false;
void test_ioP_t() {
  ioP_t testP=1.5f;
  if(testP == 1.5f) {
    Serial.println("ioP_t is floating point type");
    i2o_p_type_is_float=true;
  }
  else {
    testP=1000000000L;
    if((long) testP == 1000000000L) {
      Serial.println("ioP_t is type 'long int'");
      i2o_p_type_is_long_int=true;
    } else {
      Serial.println("ioP_t is type 'int'");
      i2o_p_type_is_int=true;
    }
  }
  Serial.println();
}


void test_in2outval(int inp) {
  Serial.println();
  Serial.print("test_in2outval\tinp=");
  Serial.println(inp);
  Serial.println();

  for (int i=0; i<1024; i++) {
    Serial.print("inval= ");
    Serial.print(i);
    Serial.print("\toutval=");
    if(! i2o_p_type_is_float)
      Serial.println(INPUTS.in2o_calculation(inp, i));
    else
      Serial.println(INPUTS.in2o_calculation(inp, i), 4);
  }
  Serial.println();
}
#endif


void setup() {
#ifdef BAUDRATE
  Serial.begin(BAUDRATE);
  Serial.println();	// helps opening connection...
  Serial.println();	// helps opening connection...
  Serial.println();	// helps opening connection...

  #ifdef INPUTS_DEBUGGING_SAMPLING
    Serial.println("will TEST sampling.");
  #endif

  #ifdef INPUTS_DEBUGGING_IO_CALCULATING
    Serial.println("will TEST in2out calculation.");
  #endif

  Serial.println();
#endif

#ifdef INPUTS_DEBUGGING_SAMPLING
  /*
  bool Inputs::setup_sample_method(int inp,\
         int (*take_sample)(int addr), uint8_t addr, uint8_t oversampling)
  */
  INPUTS.setup_sample_method(0, &test_sample_method, 0, 4);	// A0
  INPUTS.setup_sample_method(1, &test_sample_method, 1, 4);	// A1
#endif	// INPUTS_DEBUGGING_SAMPLING

#ifdef INPUTS_DEBUGGING_IO_CALCULATING	// testing in2o_calculation:
  test_ioP_t();

  // bool Inputs::setup_linear(int inp,\
            ioP_t input_offset, ioP_t mul, ioP_t div, ioV_t output_offset, bool inverse)

  INPUTS.setup_linear(0, 0, 1000, 100, 100000, false);	// linear, no offset
  Serial.println("INPUTS.setup_linear(0, 0, 1000, 100, 100000, false);	// linear, no offset");
  test_in2outval(0);

  INPUTS.setup_linear(0, -1, 1000, 100, 100000, false);	// linear, offset -1
  Serial.println("INPUTS.setup_linear(0, -1, 1000, 100, 100000, false);	// linear, offset -1");
  test_in2outval(0);

  INPUTS.setup_raw(1);					// raw
  Serial.println("INPUTS.setup_raw(1);	// raw");
  test_in2outval(1);

  INPUTS.setup_linear(0, 0, 10000, 0, 0, true);		// inverse
  Serial.println("INPUTS.setup_linear(0, 0, 10000, 0, 0, true);	// inverse");
  test_in2outval(0);

  #ifdef I2O_P_LONG_V_LONG		// see: Inputs.h
    INPUTS.setup_linear(0, 0, 1000000000L, 0, 0, true);	// inverse, big number
    Serial.println("INPUTS.setup_linear(0, 0, 1000000000L, 0, 0, true);	// inverse, big number");
    test_in2outval(0);
  #endif

  #ifdef I2O_P_DOUBLE_V_LONG_DOUBLE	// see: Inputs.h
    INPUTS.setup_linear(0, 0.0, 0.0, 0.0, 0.0, true);	// 1/x floating
//    INPUTS.setup_linear(0, 12.4, 1.5437, 0.6543, 17.7354, true);	// 1/x floating
    Serial.println("INPUTS.setup_linear(0, 0, 0, 0, 0, true);	// 1/x floating");
    test_in2outval(0);
  #endif

  Serial.println();
#endif	// INPUTS_DEBUGGING_IO_CALCULATING
}


int cnt=0;
void loop() {
  cnt++;

#ifdef INPUTS_DEBUGGING_SAMPLING
  if (cnt == 1)
    Serial.println("\nTesting analog sampling:");

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
#endif // INPUTS_DEBUGGING_SAMPLING

}
