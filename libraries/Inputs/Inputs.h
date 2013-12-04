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
  unsigned long counter;	// counts taken samples
  uint16_t flags;
  //  uint8_t input_method;		// how do we get the data?     i.e: analogRead(pin)
  uint8_t input_addr;		// where to get data from      i.e: pin, i2c address
  uint8_t oversampling;		// previous samples kept in memory
  // uint8_t input_processing;	// how to process input data?  i.e: linear mapping
  // int input_offset;
  //  scaling;
  // output_offset;
  // uint8_t output_method;		// what to do with the result?
  int (*sample_method)(int addr);	// method to take a sample
  // long (*react_on_input)(int inp);
  int * samples;				// pointer to the samples
};
// flag masks:
#define INPUT_ACTIVE		1
#define INPUT_PROCESSING	2	// any type
// *input processing* flags:
// #define PROCESS_none		0	// just pass input value
#define PROCESS_LINEAR		4	// linear
//
// *output method* flags:
#define SET_TO_VALUE
#define ADD_TO_VALUE


class Inputs {
 public:
  Inputs(int inp_max);
  ~Inputs();

  // setup an input sample method and reserve memory for samples:
  bool setup_sample_method(int inp, int (*sample_method)(int addr), uint8_t addr, uint8_t oversampling);

  bool sample(int inp);		// take a sample
				//   return true every time a new oversampling set is ready

  unsigned int get_inp_max() { return inp_max; }	// inlined

  unsigned long get_counter(int inp) {			// inlined
    return inputs[inp].counter;
  }

 private:
  int inp_max;			// max inputs possible
  input_t * inputs;		// data pointer for inputs
};


#endif	// ifndef INPUTS_h
