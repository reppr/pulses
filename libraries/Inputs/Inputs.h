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

typedef int8_t gpio_pin_t;		// negative values might be used for pin extensions


/* **************************************************************** */
/*
  Parameter types for input 2 output value calculation
  a trade off situation between RAM usage, range and precision...

  Undecided which parameter types to use i implement pp switches
  for different formats to test:
*/
// UNCOMMENT *ONE* OF THE FOLLOWING TRIPLETS:

// *small* unsigned integer range 0 to 255, save RAM:
//#define I2O_parameter_t	uint8_t
//#define I2O_value_t	unsigned int
//#undef I2O_PARAMETER_IS_FLOAT		// see: inputs_test.ino

// *small* signed integer range -128 to 127, save RAM:
//#define I2O_parameter_t	int8_t
//#define I2O_value_t	int
//#undef I2O_PARAMETER_IS_FLOAT		// see: inputs_test.ino

// normal signed integer range, save some RAM:
//#define I2O_parameter_t	int
//#define I2O_value_t		long
//#undef I2O_PARAMETER_IS_FLOAT		// see: inputs_test.ino

// integer, bigger range:
#define I2O_parameter_t		long
#define I2O_value_t		long
#undef  I2O_PARAMETER_IS_FLOAT	// see: inputs_test.ino

// floating point:
//#define I2O_parameter_t	double
//#define I2O_value_t		double
//#define I2O_PARAMETER_IS_FLOAT	// see: inputs_test.ino


#ifndef I2O_parameter_t
  #error "#define I2O_parameter_t"
#endif
#ifndef I2O_value_t
  #error "#define I2O_value_t"
#endif
typedef I2O_parameter_t ioP_t;	// ioP_t is 'input 2 output parameter type'
typedef I2O_value_t ioV_t;	// ioV_t is 'input 2 output value type', possibly longer


/* **************************************************************** */
struct input_t {
  unsigned long counter;		// counts taken samples
  uint16_t flags;
  gpio_pin_t inp_A;			// address where to get data from, i.e: pin, i2c address
  gpio_pin_t inp_B;			// second input parameter, second pin, i2c register, etc.
  unsigned int oversample;		// total samples kept in memory
  int (*sample_method)(int addr);	// method to take a sample, i.e. analogRead(pin)

  // method to get the output value from an input value
  ioV_t (*in2o_method)(int inp, ioV_t value);

  // Parameters for processing input to output values:
  ioP_t in_offset;	// added to the input value before further processing
  ioP_t mul;		// factor for processing input to output values
  ioP_t div;		// divisor for processing input to output values

  ioV_t out_offset;	// added to the output value after processing

  int * samples;	// pointer to the stored samples for oversampling and friends

  // function pointer to void out_reaction(int inp, ioV_t value)
  void (*out_reaction)(int inp, ioV_t output_value);
  gpio_pin_t out_A;	// output address like pin, pulse, etc.
  gpio_pin_t out_B;	// second output parameter, like second pin, index, etc.
  unsigned int last_output;	// for display
};


// inputs[inp].flags bitmasks:

// special input types flags:
#define INPUT_ANALOG_internal	1	//				A

// *input oversampling* flags
#define OVERSAMPLE_AVERAGE	2	//				O

// *input processing* flags:
#define INPUT_PROCESSING	4	// any type, not really needed	:
#define PROCESS_LINEAR		8	// linear			L
#define PROCESS_INVERSE		16	// reziprocal			R
// 32
// 64
#define PROCESS_CUSTOM		128	// custom processing function	C

// *output method* flags:
#define INPUT_OUTPUT_REACTION	256	//				:
#define SET_TO_VALUE		512	//				=
#define ADD_TO_VALUE		1024	//				+
#define MUL_VALUE		2048	//				*
//  4096

// *inbuilt reaction flags:
#define SET_PWM			8192	// output PWM internal		W
//  16384

// general flags:

// ==> if you change INPUT_ACTIVE you *must* call fix_active_inputs() <==
#define INPUT_ACTIVE		32768	// leading bit in the menu	!


// aliases for io_calculation
#define PROPORTIONAL	false
#define INVERSE		true



class Inputs {
 public:
  Inputs(int inputs_to_allocate);

  ~Inputs();

  /*
  bool do_next_input(void);
  Main function to be called from Arduino loop().
    Take a sample on next active input,
    react adequately,
      like: on completing a new oversampling set
	    compute average, do in2o_calculation on that
	    influence the period of a pulse, or whatever...
    Return a flag if an action was triggered.
  */
  bool do_next_input(void);

  /*
  bool sample_and_react(int inp);
  Take a sample on inp,
    react adequately,
      like: on completing a new oversampling set
	    compute average, do in2o_calculation on that
	    influence the period of a pulse, or whatever...
  Return true every time an action was triggered.
  Can be called directly from the sketch or by a pulse.
  */
  bool sample_and_react(int inp);

  /*
    bool malloc_samples(int inp, unsigned int oversample);
    (free old sample memory, and)
    malloc memory for 'oversample' bytes:
  */
  bool malloc_samples(int inp, unsigned int oversample);

  /*
    bool sample(int inp);
    Take a sample on inp.
    Return true every time a new oversampling set is ready
    as a trigger for possible reactions.
  */
  bool sample(int inp);

  /*
    int get_last_sampled(int inp)
    mainly for debugging
    get the last sample that was taken before on inp
    do *not* call this if there was no sample taken before...
  */
  int get_last_sampled(int inp) const {			// inlined
    return inputs[inp].samples[((inputs[inp].counter - 1) % inputs[inp].oversample)];
  }

  /*
    int get_last_output(int inp)
    for display
  */
  int get_last_output(int inp) const {			// inlined
    return inputs[inp].last_output ;
  }

  /*
    bool setup_analog_read(int inp, gpio_pin_t addr, unsigned int oversample);
    Setup internal analogRead(pin), and reserve memory for oversampling:
  */
  bool setup_analog_read(int inp, gpio_pin_t addr, unsigned int oversample);

  /*
    bool setup_sample_method(int inp, int (*sample_method)(int addr), \
					   gpio_pin_t addr, unsigned int oversample)
    Setup a custom input sample method and reserve memory for oversampling:
  */
  bool setup_sample_method(int inp, int (*sample_method)(int addr), \
			   gpio_pin_t addr, unsigned int oversample);


  /*
    int oversamples_average(int inp);
    return average of the saved samples
  */
  int oversamples_average(int inp) const;

  /*
    unsigned int oversamples_deviation(int inp);
    Get average of absolute deviation against average of all samples
  */
  unsigned int oversamples_deviation(int inp) const;



  /*
    ioV_t Inputs::in2o_calculation(int inp, ioV_t value)
    Calculate and return the output value from an input value
    according the roules for input inp.
  */
  ioV_t in2o_calculation(int inp, ioV_t value) const;

  /*
    bool Inputs::setup_raw(int inp)
    Setup input inp to pass through raw values without further processing.
    Oversampling and friends might still be active though.
  */
  bool setup_raw(int inp);

  /*
    bool setup_in2o_custom(int inp, ioV_t (*method)(int inp, ioV_t value));
    setup a custom method to calculate output value from sample value.
  */
  bool setup_in2o_custom(int inp, ioV_t (*method)(int inp, ioV_t value));

  // ioV_t in2o_linear(int inp, ioV_t value);	// c++ did not like me :( FIXME ################

  /*
    bool Inputs::setup_linear(int inp,	\
	   ioP_t in_offset, ioP_t mul, ioP_t div, ioV_t out_offset, bool inverse)
    Configure a linear or reverse linear in2out function on input inp.
  */
  bool setup_linear(int inp, ioP_t in_offset, ioP_t mul, ioP_t div, ioV_t out_offset, bool inverse);


  /*
    bool setup_io_reaction(int inp, void (*reaction)(int inp, ioV_t value));
    setup a reaction 'void reaction(int inp, ioV_t value)' to be done when
    reaching a full oversampling set
  */
  bool setup_io_reaction(int inp, void (*reaction)(int inp, ioV_t value));


  /*
    bool setup_PWM(int inp, gpio_pin_t pin);
    Setup PWM output on input inp, using PWM pin (no checks...).
    On ARDUINO initialize pin.
  */
  bool setup_PWM(int inp, gpio_pin_t pin);


  /*
    some inlined get_xxx() functions:
  */
  int get_sample(int inp, int index) const {		// inlined
    return inputs[inp].samples[index];
  }

  unsigned int get_inputs_allocated() const {		// inlined
    return inputs_allocated; }

  unsigned long get_counter(int inp) const {		// inlined
    return inputs[inp].counter;
  }

  uint16_t get_flags(int inp) const {			// inlined
    return inputs[inp].flags;
  }

  gpio_pin_t get_inp_A(int inp) const {			// inlined
    return inputs[inp].inp_A;
  }

  gpio_pin_t get_inp_B(int inp) const {			// inlined
    return inputs[inp].inp_B;
  }

  unsigned int get_oversample(int inp) const {		// inlined
    return inputs[inp].oversample;
  }

  ioP_t get_in_offset(int inp) const {			// inlined
    return inputs[inp].in_offset;
  }

  ioP_t get_mul(int inp) const {			// inlined
    return inputs[inp].mul;
  }

  ioP_t get_div(int inp) const {			// inlined
    return inputs[inp].div;
  }

  ioV_t get_out_offset(int inp) const {			// inlined
    return inputs[inp].out_offset;
  }

// MISSING get functions:
//   samples(int inp)
//   out_reaction(int inp, ioV_t value);
//   sample_method(int inp)
//   in2o_method(int inp)

  gpio_pin_t get_out_A(int inp) const {			// inlined
    return inputs[inp].out_A;
  }

  gpio_pin_t get_out_B(int inp) const {			// inlined
    return inputs[inp].out_B;
  }


  /*
    some inlined set_xxx() functions:
  */
  void set_counter(int inp, unsigned long counter) {	// inlined
    inputs[inp].counter = counter;
  }

  void set_flags(int inp, uint16_t flags) {		// inlined
    inputs[inp].flags = flags;
    fix_active_inputs();
  }

  void set_inp_A(int inp, gpio_pin_t addr) {		// inlined
    inputs[inp].inp_A = addr;
    inputs[inp].flags |= INPUT_ANALOG_internal;
  }

  void set_inp_B(int inp, gpio_pin_t i2) {			// inlined
    inputs[inp].inp_B = i2;
  }

  /*
  // do not do this...
  void set_oversample(int inp, unsigned int oversample) {
    inputs[inp].oversample = oversample;
  }
  */

  void set_in_offset(int inp, ioP_t offset) {		// inlined
    inputs[inp].in_offset = offset;
  }

  void set_mul(int inp, ioP_t mul) {			// inlined
    inputs[inp].mul=mul;
  }

  void set_div(int inp, ioP_t div) {			// inlined
    inputs[inp].div = div;
  }

  void set_out_offset(int inp, ioV_t out_offset) {	// inlined
    inputs[inp].out_offset = out_offset;
  }

  void set_out_A(int inp, gpio_pin_t out_A) {		// inlined
    inputs[inp].out_A = out_A;
  }

  void set_out_B(int inp, gpio_pin_t o2) {		// inlined
    inputs[inp].out_B = o2;
  }

  /*
  not implemented set_xxx() functions:
    samples(int inp)
    out_reaction(int inp, ioV_t value);
    sample_method(int inp)
    in2o_method(int inp)
  */

  unsigned long selected_inputs;
  unsigned long active_inputs;			// as a check from outside
  void fix_active_inputs();

  const char * flag_mnemonics_on  = "AO:LR..C:=+*.W.!";
  const char * flag_mnemonics_off = "________________";

 private:
  int inputs_allocated;		// allocated inputs
  int next_input;		// next input to take a sample
  input_t * inputs;		// data pointer for inputs
};


#ifndef PWMRANGE

  #ifdef ESP32	// ESP32 does not know PWMRANGE
    #define PWMRANGE	(2^13 - 1)
  #else
//    #error Inputs.h 'PWMRANGE' is not defined
    #define PWMRANGE	255
  #endif

#endif	//  ! PWMRANGE

#endif	// ifndef INPUTS_h
