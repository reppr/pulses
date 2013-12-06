/* **************************************************************** */
/*
   Inputs.h

   Arduino library to process input values from different sources

   Copyright © Robert Epprecht  www.RobertEpprecht.ch  GPLv2
   http://github.com/reppr/pulses

*/
/* **************************************************************** */


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


/* **************************************************************** */
/*
  parameter types for input 2 output value calculation
  a trade off situation between RAM usage, range and precision...
*/
// UNCOMMENT *ONE* OF THE FOLLOWING TRIPLETS:

// *small* unsigned integer range 0 to 255, save RAM:
//#define I2O_PARAMETER_TYPE	uint8_t
//#define I2O_VALUE_TYPE	unsigned int
//#undef I2O_PARAMETER_IS_FLOAT		// see: inputs_test.ino

// *small* signed integer range -128 to 127, save RAM:
//#define I2O_PARAMETER_TYPE	int8_t
//#define I2O_VALUE_TYPE	int
//#undef I2O_PARAMETER_IS_FLOAT		// see: inputs_test.ino

// normal signed integer range, save some RAM:
//#define I2O_PARAMETER_TYPE	int
//#define I2O_VALUE_TYPE		long
//#undef I2O_PARAMETER_IS_FLOAT		// see: inputs_test.ino

// integer, bigger range:
#define I2O_PARAMETER_TYPE	long
#define I2O_VALUE_TYPE		long
#undef  I2O_PARAMETER_IS_FLOAT	// see: inputs_test.ino

// floating point:
//#define I2O_PARAMETER_TYPE	double
//#define I2O_VALUE_TYPE		double
//#define I2O_PARAMETER_IS_FLOAT		// see: inputs_test.ino


#ifndef I2O_PARAMETER_TYPE
  #error "#define I2O_PARAMETER_TYPE"
#endif
#ifndef I2O_VALUE_TYPE
  #error "#define I2O_VALUE_TYPE"
#endif
typedef I2O_PARAMETER_TYPE ioP_t;	// ioP_t is 'input 2 output parameter type'
typedef I2O_VALUE_TYPE ioV_t;		// ioV_t is 'input 2 output value type', possibly longer


/* **************************************************************** */
struct input_t {
  unsigned long counter;		// counts taken samples
  uint16_t flags;
  uint8_t input_addr;			// where to get data from, i.e: pin, i2c address
  uint8_t oversampling;			// previous samples kept in memory
  int (*sample_method)(int addr);	// method to take a sample, i.e. analogRead(pin)

  // c++ did not like me :(
  // method to get the output value from an input value
  ioV_t (*in2o_method)(int inp, ioV_t value);

  // Parameters for processing input to output values:
  ioP_t input_offset;	// added to the input value before further processing
  ioP_t mul;		// factor for processing input to output values
  ioP_t div;		// divisor for processing input to output values

  ioV_t output_offset;	// added to the output value after processing

  int * samples;	// pointer to the stored samples for oversampling and friends
};

// flags bitmasks:
#define INPUT_ACTIVE		1
// *input processing* flags:
#define INPUT_PROCESSING	2	// any type
#define PROCESS_LINEAR		4	// linear
#define PROCESS_INVERSE		8	// reziprocal
#define PROCESS_CUSTOM		128	// custom processing function
// *output method* flags:
//#define SET_TO_VALUE		256
//#define ADD_TO_VALUE		512
//#define MUL_VALUE	        1024


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
  bool setup_in2o_custom(int inp, ioV_t (*method)(int inp, ioV_t value));

/*
  bool sample(int inp);
  Take a sample
  Return true every time a new oversampling set is ready
  as a trigger for possible reactions.
*/
  bool sample(int inp);

  // ioV_t in2o_linear(int inp, ioV_t value);	// c++ did not like me :( FIXME ################

/*
  bool Inputs::setup_raw(int inp)
  Setup input inp to pass through raw values without further processing.
  Oversampling and friends might still be active though.
*/
  bool setup_raw(int inp);

/*
  bool Inputs::setup_linear(int inp,\
         ioP_t input_offset, ioP_t mul, ioP_t div, ioV_t output_offset, bool inverse)
  Configure a linear or reverse linear in2out function on input inp.
*/
  bool setup_linear(int inp, ioP_t input_offset, ioP_t mul, ioP_t div, ioV_t output_offset, bool inverse);

/*
  ioV_t Inputs::in2o_calculation(int inp, ioV_t value)
  Calculate and return the output value from an input value
  according the roules for input inp.
*/
  ioV_t in2o_calculation(int inp, ioV_t value);

  unsigned int get_inp_max() { return inp_max; }	// inlined

  unsigned long get_counter(int inp) {			// inlined
    return inputs[inp].counter;
  }

 private:
  int inp_max;			// max inputs possible
  input_t * inputs;		// data pointer for inputs
};


#endif	// ifndef INPUTS_h
