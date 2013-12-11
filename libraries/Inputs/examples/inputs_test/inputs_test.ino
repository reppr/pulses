/* **************************************************************** */
/*
		    inputs_test.ino

     A couple of early test stubs to test Inputs library.
   (The different tests can be activated by pp #define's.)


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

// uncomment to get a lot of debugging output:
// #define INPUTS_DEBUGGING_ALL

#ifdef INPUTS_DEBUGGING_ALL
  #define INPUTS_DEBUGGING_SAMPLING
  #define INPUTS_DEBUGGING_IO_CALCULATING
  #define INPUTS_DEBUGGING_CUSTOM_FUNCTION
#endif


// uncomment to test reaction on samples
#define INPUTS_DEBUGGING_SAMPLE_REACTION


// uncomment for a simple example using a Sharp IR distance sensor on A0
// #define SHARP_IR_DISTANCE_TEST

// uncomment two lines to get a simple HC-SR04 ultrasonic distance sensor example
// edit to match the Arduino pins connected to the sensor
//#define HCSR04_TRIGGER_PIN	3	// triggers also compilation of HCSR04 code
//#define HCSR04_ECHO_PIN		2


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
Inputs INPUTS(8);
int inp;

/* **************************************************************** */
// test functions for debugging:
#ifdef INPUTS_DEBUGGING_SAMPLING
int test_sample_method(int addr) {	// a test sampling method for very first tests
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
/*
  void test_ioP_t();
  check configured parameter types of Inputs library
  quite an ugly hack, but ok here
*/
bool i2o_p_type_is_int8=false;
bool i2o_p_type_is_uint8=false;
bool i2o_p_type_is_int=false;
bool i2o_p_type_is_long_int=false;
bool i2o_p_type_is_float=false;
void test_ioP_t() {
  ioP_t testP=1.5f;
  if(testP == 1.5f) {
    Serial.println("ioP_t is floating point type");
    i2o_p_type_is_float=true;
  } else {
    testP=1000000000L;
    if((long) testP == 1000000000L) {
      Serial.println("ioP_t is type 'long int'");
      i2o_p_type_is_long_int=true;
    } else {
      testP=256;
      if((int) testP == 256) {
	Serial.println("ioP_t is type 'int'");
	i2o_p_type_is_int=true;
      } else {
	testP=-1;
	if(testP < 0) {
	  Serial.println("ioP_t is type 'int8_t'");
	  i2o_p_type_is_int8=true;
	} else {
	  Serial.println("ioP_t is type 'uint8_t'");
	  i2o_p_type_is_uint8=true;
	}
      }
    }
  }
  Serial.println();
}


/*
  void test_in2outval(int inp);
  test in2o_calculation for inputs 0 to 1023
*/
void test_in2outval(int inp) {
  Serial.println();
  Serial.print("test_in2outval\tinp=");
  Serial.println(inp);
  Serial.println();

  for (int i=0; i<1024; i++) {
    Serial.print("inval= ");
    Serial.print(i);
    Serial.print("\toutval=");
#ifdef I2O_PARAMETER_IS_FLOAT
      Serial.println(INPUTS.in2o_calculation(inp, i), 4);
#else
      Serial.println(INPUTS.in2o_calculation(inp, i));
#endif
  }
  Serial.println();
}
#endif


#ifdef INPUTS_DEBUGGING_CUSTOM_FUNCTION
// just a silly function for a quick test
ioV_t custom_in2o_method(int inp, ioV_t value) {
  return 7*value;
}
#endif


#ifdef INPUTS_DEBUGGING_SAMPLE_REACTION
// bar_graph_(inp, value) with a dummy inp
void bar_graph_(int dummy, ioV_t value) {
  bar_graph(value, 1023, '*');
}
#endif


#if (defined(SHARP_IR_DISTANCE_TEST) || defined(INPUTS_DEBUGGING_SAMPLE_REACTION))
int analogRead_(int pin) {	// horrible kludge, we need the type cast here...
  return analogRead(pin);
}
#endif


#ifdef HCSR04_TRIGGER_PIN
/*
  int read_HCSR04_ultrasonic(int dummy);
  with a dummy inp paramer
*/
#define HCSR04_TRIGGER_DURATION	10
int read_HCSR04_ultrasonic(int dummy) {
  long time;

  digitalWrite(HCSR04_TRIGGER_PIN, HIGH);			// trigger
  time = micros();

  while (micros() - time < HCSR04_TRIGGER_DURATION)	// >= 10 uS
    ;
  digitalWrite(HCSR04_TRIGGER_PIN, LOW);			// end trigger


  while (digitalRead(HCSR04_ECHO_PIN) == LOW)  	// wait for echo
    ;

  time = micros();				// echo started
  while (digitalRead(HCSR04_ECHO_PIN) == HIGH)	// echo still running
    ;
  return (int) (micros() - time);		// return echo duration
}
#endif


/* **************************************************************** */
/*
  bar_graph(value)
  print one value & bar graph line:
*/
void bar_graph(int value, int scale, char c) {
  const int length=64;
  int stars = ((long) value * (long) length) / (scale + 1) ;

  Serial.print("value==");
  Serial.print(value);
  Serial.print("\t");
  if (value >=0 && value <= scale) {
    for (int i=0; i<stars; i++) {
      Serial.print(c);
    }
  }
  Serial.println();
}

void bar_graph_signed(int value, int scale, char m, char p) {
  const int length=32;
  int stars = ((long) value * (long) length) / (scale + 1);

  Serial.print("value==");
  Serial.print(value);
  Serial.print("\t");

  for (int i=0; i < length; i++) {
    if (i < (length + stars))
      Serial.print(F(" "));
    else
      Serial.print(m);
  }

  if (stars==0)
    Serial.print(F("0"));
  else
    for (int i=0; i < stars; i++)
      Serial.print(m);

  Serial.println();
}


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
  #ifndef ILLEGAL
     #define ILLEGAL	~0
  #endif
  int get_free_RAM() { return ILLEGAL; }
#endif


/* **************************************************************** */
// Arduino setup():

void setup() {
#ifdef BAUDRATE
  Serial.begin(BAUDRATE);
  Serial.println();	// helps opening connection...
  Serial.println();	// helps opening connection...
  Serial.println();	// helps opening connection...

  Serial.println("running inputs_test.ino");
  Serial.print("Inputs=");
  Serial.print(INPUTS.get_inp_max());
  Serial.print("\tsizeof(input_t)=");
  Serial.print(sizeof(input_t));
  Serial.print("\ttotal=");
  Serial.println(sizeof(input_t) * INPUTS.get_inp_max());
  Serial.print("free RAM=");
  Serial.println(get_free_RAM());
  Serial.println();


  #ifdef INPUTS_DEBUGGING_IO_CALCULATING
    Serial.println("will TEST in2out calculation.");
  #endif

  #ifdef INPUTS_DEBUGGING_CUSTOM_FUNCTION
    Serial.println("will TEST in2out custom function.");
  #endif

  #ifdef INPUTS_DEBUGGING_SAMPLING
    Serial.println("will TEST sampling.");
  #endif

  #ifdef INPUTS_DEBUGGING_SAMPLE_REACTION
    Serial.println("will TEST reaction on sampling.");
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


#ifdef INPUTS_DEBUGGING_SAMPLE_REACTION
  // INPUTS.setup_sample_method(5, &analogRead_, 3, 8);		// A3, oversampling=8
  // INPUTS.setup_linear(5, 0, 255, 1023, 0, PROPORTIONAL);	// 255*x/1023

  inp=6;
  INPUTS.setup_sample_method(inp, &analogRead_, 2, 8);		// A2, oversampling=8
  INPUTS.setup_linear(inp, 0, 255, 1023, 0, PROPORTIONAL);	// 255*x/1023

  /*
  bool setup_io_reaction(int inp, void (*reaction)(int inp, ioV_t value));
  */
  INPUTS.setup_io_reaction(inp, &bar_graph_);
#endif

#ifdef SHARP_IR_DISTANCE_TEST
  INPUTS.setup_sample_method(3, &analogRead_, 0, 4);		// A0
  INPUTS.setup_linear(3, -20, 4800, 1, 0, INVERSE);		// 4800/(x-20)
#endif


#ifdef HCSR04_TRIGGER_PIN
  INPUTS.setup_sample_method(4, &read_HCSR04_ultrasonic, 99, 4);
  INPUTS.setup_linear(4, 0, 0, 58, 0, PROPORTIONAL);		// x/58 gives cm
#endif


#ifdef INPUTS_DEBUGGING_IO_CALCULATING	// testing in2o_calculation:
  test_ioP_t();

  // bool Inputs::setup_linear(int inp,\
            ioP_t input_offset, ioP_t mul, ioP_t div, ioV_t output_offset, bool inverse)

  if(i2o_p_type_is_int8) {
    INPUTS.setup_linear(0, 0, 1, -2, 0, PROPORTIONAL);			// linear, -128 to 127, -50%
    Serial.println("INPUTS.setup_linear(0, 0, 9, 10, 0, PROPORTIONAL);	// linear, <=255, 90%");
    test_in2outval(0);
  } else {
    if(i2o_p_type_is_uint8) {
    INPUTS.setup_linear(0, 0, 9, 10, 0, PROPORTIONAL);			// linear, <=255, 90%
    Serial.println("INPUTS.setup_linear(0, 0, 9, 10, 0, PROPORTIONAL);	// linear, <=255, 90%");
    test_in2outval(0);
    } else {
      INPUTS.setup_linear(0, 0, 1000, 100, 100000, PROPORTIONAL);	// linear, no offset
      Serial.println("INPUTS.setup_linear(0, 0, 1000, 100, 100000, PROPORTIONAL);	// linear, no offset");
      test_in2outval(0);

      INPUTS.setup_linear(0, -1, 1000, 100, 100000, PROPORTIONAL);	// linear, offset -1
      Serial.println("INPUTS.setup_linear(0, -1, 1000, 100, 100000, PROPORTIONAL);	// linear, offset -1");
      test_in2outval(0);

      INPUTS.setup_raw(1);						// raw
      Serial.println("INPUTS.setup_raw(1);	// raw");
      test_in2outval(1);

      INPUTS.setup_linear(0, 0, 10000, 0, 0, INVERSE);			// inverse
      Serial.println("INPUTS.setup_linear(0, 0, 10000, 0, 0, INVERSE);	// inverse");
      test_in2outval(0);
    }
  }

  if(i2o_p_type_is_long_int || i2o_p_type_is_float) {
    INPUTS.setup_linear(0, 0, 1000000000L, 0, 0, INVERSE);		// inverse, big number
    Serial.println("INPUTS.setup_linear(0, 0, 1000000000L, 0, 0, INVERSE);	// inverse, big number");
    test_in2outval(0);
  }

  #ifdef  I2O_PARAMETER_IS_FLOAT	// see: Inputs.h
    INPUTS.setup_linear(0, 0.0, 1.0, 1.0, 0.0, INVERSE);		// 1/x floating
    Serial.println("INPUTS.setup_linear(0, 0, 1.0, 1.0, 0, INVERSE);	// 1/x floating");
    test_in2outval(0);
  #endif

  Serial.println();
#endif	// INPUTS_DEBUGGING_IO_CALCULATING


#ifdef INPUTS_DEBUGGING_CUSTOM_FUNCTION
  // Inputs::setup_in2o_method(int inp, ioV_t (*method)(int inp, ioV_t value)) {
  INPUTS.setup_in2o_custom(2, &custom_in2o_method);
  Serial.println("INPUTS.setup_in2o_custom(2, &custom_in2o_method);	// *7");
  test_in2outval(2);
#endif


#ifdef HCSR04_TRIGGER_PIN
  pinMode(HCSR04_TRIGGER_PIN, OUTPUT);
  digitalWrite(HCSR04_TRIGGER_PIN, LOW);
  pinMode(HCSR04_ECHO_PIN, INPUT);
#endif
}


/* **************************************************************** */
// Arduino loop():

int cnt=0;
int loop_delay=1000;

void loop() {
  cnt++;

  ioV_t value;

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
#endif // INPUTS_DEBUGGING_SAMPLING

#ifdef INPUTS_DEBUGGING_SAMPLE_REACTION
  inp=6;
  INPUTS.sample_and_react(inp);
  loop_delay=125;
#endif

#ifdef SHARP_IR_DISTANCE_TEST
  inp=3;
  INPUTS.sample(inp);
  Serial.print("Sharp IR distance sensor\t");
  Serial.print(INPUTS.get_last_sampled(inp));

  Serial.print("\tcm=");
  value=INPUTS.in2o_calculation(inp, INPUTS.get_last_sampled(inp));
#ifdef I2O_PARAMETER_IS_FLOAT
      Serial.println(value, 4);
#else
      bar_graph(value, 120, 'I');
      //      Serial.println(INPUTS.in2o_calculation(inp, INPUTS.get_last_sampled(inp)));
#endif
#endif


#ifdef HCSR04_TRIGGER_PIN
  inp=4;
  INPUTS.sample(inp);
  Serial.print("Ultrasonic distance HC-SR04\t");
  Serial.print(INPUTS.get_last_sampled(inp));

  Serial.print("\tcm=");
  value=INPUTS.in2o_calculation(inp, INPUTS.get_last_sampled(inp));
#ifdef I2O_PARAMETER_IS_FLOAT
      Serial.println(value, 4);
#else
      bar_graph(value, 120, 'U');
#endif
#endif


  delay(loop_delay);
}

/* **************************************************************** */
