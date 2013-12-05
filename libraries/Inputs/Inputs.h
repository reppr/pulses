/* **************************************************************** */
/*
   Inputs.h

   Arduino library to process input values from different sources

   Copyright © Robert Epprecht  www.RobertEpprecht.ch  GPLv2
   http://github.com/reppr/pulses

*/
/* **************************************************************** */

#ifndef INPUTS_h
#define INPUTS_h


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#if defined(ARDUINO)
  #if ARDUINO >= 100
    #include "Arduino.h"
  #else
    #include "WProgram.h"
  #endif
#endif // ARDUINO


struct input_t {
  unsigned long counter;		// counts taken samples
  uint16_t flags;
  uint8_t input_addr;			// where to get data from, i.e: pin, i2c address
  uint8_t oversampling;			// previous samples kept in memory
  int (*sample_method)(int addr);	// method to take a sample, i.e. analogRead(pin)

  // c++ did not like me :(
  // long (*in2o_method)(int inp, const long value);	// method to get the output value from an input value

  // Parameters for processing input to output values:
  int input_offset;	// added to the input value before further processing
  int mul;		// factor for processing input to output values
  int div;		// divisor for processing input to output values
  long output_offset;	// added to the output value after processing

  int * samples;	// pointer to the stored samples for oversampling and friends
};

// flags bitmasks:
#define INPUT_ACTIVE		1
// *input processing* flags:
#define INPUT_PROCESSING	2	// any type
#define PROCESS_LINEAR		4	// linear
#define PROCESS_INVERSE		8	// reziprocal
// *output method* flags:
// #define SET_TO_VALUE
// #define ADD_TO_VALUE


class Inputs {
 public:
  Inputs(int inp_max);
  ~Inputs();

/*
  bool setup_sample_method(int inp, int (*sample_method)(int addr), uint8_t addr, uint8_t oversampling)
  Setup an input sample method and reserve memory for oversampling:
*/
  bool setup_sample_method(int inp, int (*sample_method)(int addr), uint8_t addr, uint8_t oversampling);

  // c++ did not like me :(
  // bool setup_in2o_method(int inp, long (*in2o_method)(int inp, long value));

/*
  bool sample(int inp);
  Take a sample
  Return true every time a new oversampling set is ready
  as a trigger for possible reactions.
*/
  bool sample(int inp);

  // long in2o_linear(int inp, long value);	// c++ did not like me :( FIXME ################

/*
  bool Inputs::setup_raw(int inp)
  Setup input inp to pass through raw values without further processing.
  Oversampling and friends might still be active though.
*/
  bool setup_raw(int inp);

/*
  bool Inputs::setup_linear(int inp,\
         int input_offset, int mul, int div, long output_offset, bool inverse)
  Configure a linear or reverse linear in2out function on input inp.
*/
  bool setup_linear(int inp, int input_offset, int mul, int div, long output_offset, bool inverse);

/*
  long Inputs::in2o_calculation(int inp, long value)
  Calculate and return the output value from an input value
  according the roules for input inp.
*/
  long in2o_calculation(int inp, long value);

  unsigned int get_inp_max() { return inp_max; }	// inlined

  unsigned long get_counter(int inp) {			// inlined
    return inputs[inp].counter;
  }

 private:
  int inp_max;			// max inputs possible
  input_t * inputs;		// data pointer for inputs
};


#endif	// ifndef INPUTS_h
