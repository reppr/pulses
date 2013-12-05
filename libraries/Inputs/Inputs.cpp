/* **************************************************************** */
/*
   Inputs.cpp

   Arduino library to process input values from different sources.

   Copyright © Robert Epprecht  www.RobertEpprecht.ch  GPLv2
   http://github.com/reppr/pulses

*/
/* **************************************************************** */


/* **************************************************************** */
// Preprocessor #includes:

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef ARDUINO
  #if ARDUINO >= 100
    #include "Arduino.h"
  #else
    #include "WProgram.h"
  #endif

  #include <Inputs.h>
#else
  #include <iostream>

  #include <Inputs/Inputs.h>
#endif


/* **************************************************************** */
// Constructor:
Inputs::Inputs(int inp_max):
  inp_max(inp_max)
{
  inputs = (input_t *) malloc(inp_max * sizeof(input_t));	// ERROR ################
  memset(inputs, 0, inp_max * sizeof(input_t));

  for (int inp=0; inp<inp_max; inp++) {
    inputs[inp].sample_method = NULL;
    inputs[inp].samples = NULL;
    // c++ did not like me :(
    // inputs[inp].in2o_method = NULL;
  }
}

// Destructor:
Inputs::~Inputs() {
  for (int inp=0; inp<inp_max; inp++)
    free(inputs[inp].samples);

  free(inputs);
}


/*
  bool setup_sample_method(int inp, int (*sample_method)(int addr), uint8_t addr, uint8_t oversampling)
  Setup an input sample method and reserve memory for oversampling:
*/
bool Inputs::setup_sample_method(int inp, int (*take_sample)(int addr), uint8_t addr, uint8_t oversampling)
{
  if ((inp < 0) or (inp >= inp_max))
    return false;	// inp out of range

  inputs[inp].sample_method = take_sample;
  inputs[inp].input_addr = addr;

  free(inputs[inp].samples);
  inputs[inp].samples = (int *) malloc(oversampling * sizeof(int));
  if (inputs[inp].samples == NULL)
    return false;	// not enough RAM

  inputs[inp].oversampling = oversampling;
  return true;	// everything ok
}


/*
  bool sample(int inp);
  Take a sample
  Return true every time a new oversampling set is ready
  as a trigger for possible reactions.
*/
bool Inputs::sample(int inp) {
  int value;
  unsigned int cyclic_index;

  if (inputs[inp].sample_method == NULL)	// not really needed
    return false;	// no error treatment, should not happen...

  value=(*(*inputs[inp].sample_method))((int) inputs[inp].input_addr);

  cyclic_index = (inputs[inp].counter++ % inputs[inp].oversampling);
  inputs[inp].samples[cyclic_index]=value;

  Serial.print("\tsample stored cyclic_index=");
  Serial.print(cyclic_index);
  Serial.print("\toversampling=");
  Serial.println(inputs[inp].oversampling);

  if (++cyclic_index == inputs[inp].oversampling)	// oversampling set ready?
    return true;		// oversampling set is ready, trigger possible reactions
  else
    return false;		// oversampling continues
}


/*
  ioV_t Inputs::in2o_calculation(int inp, ioV_t value)
  Calculate and return the output value from an input value
  according the roules for input inp.
*/
ioV_t Inputs::in2o_calculation(int inp, ioV_t value) {
  ioV_t outval;

  // getting these only *once*, hope that's faster:
  // (as all calculations are in ioV_t i declare all operands ioV_t)
  ioV_t input_offset = inputs[inp].input_offset;
  if (input_offset)
    value += input_offset;

  bool inverse = inputs[inp].flags & PROCESS_INVERSE;
  if (inverse)
    outval=1L;
  else
    outval=value;

  ioV_t mul = inputs[inp].mul;
  if (mul)
    outval *= mul;

  ioV_t div = inputs[inp].div;
  if ((div) && (div != 1))
    outval /= div;

  if (inverse)
    if (value)
      outval /= value;

  ioV_t output_offset = inputs[inp].output_offset;
  if (output_offset)
    outval += output_offset;

  return outval;
}


/*
  bool Inputs::setup_raw(int inp)
  Setup input inp to pass through raw values without further processing.
  Oversampling and friends might still be active though.
*/
bool Inputs::setup_raw(int inp) {
  if ((inp < 0) or (inp >= inp_max))
    return false;	// inp out of range

  inputs[inp].flags &= ~INPUT_PROCESSING;
}


/*
  bool Inputs::setup_linear(int inp,\
         ioP_t input_offset, ioP_t mul, ioP_t div, ioV_t output_offset, bool inverse)
  Configure a linear or reverse linear in2out function on input inp.
*/
bool Inputs::setup_linear(int inp, \
       ioP_t input_offset, ioP_t mul, ioP_t div, ioV_t output_offset, bool inverse)
{
  if ((inp < 0) or (inp >= inp_max))
    return false;	// inp out of range

  inputs[inp].flags |= INPUT_PROCESSING;
  inputs[inp].flags |= PROCESS_LINEAR;

  if(inverse)
    inputs[inp].flags |= PROCESS_INVERSE;
  else
    inputs[inp].flags &= ~PROCESS_INVERSE;

//	c++ did not like my first idea :(
//	  inputs[inp].in2o_method = &Inputs::in2o_linear;
//	  inputs[inp].in2o_method = &in2o_linear;

  inputs[inp].input_offset = input_offset;
  inputs[inp].mul = mul;
  inputs[inp].div = div;
  inputs[inp].output_offset = output_offset;
  return true;
}


/* **************************************************************** */
/* **************************************************************** */
/*    SOURCE ENDS HERE.						    */				 
/* **************************************************************** */
/* **************************************************************** */




// ################	FIXME: delete old source.
/* **************************************************************** */
// old source to check how i did it last time.
/* **************************************************************** */

#undef DO_COMPILE_OLD_SOURCE
#ifdef DO_COMPILE_OLD_SOURCE
/*
  old source from
  pulses/misc/src/arduino-git1/polyphonic_oscillators/polyphonic_oscillators.pde

  *WITH* RENAMES
*/

/* **************************************************************** */
// #define INPUTS_MAX	16	// maximal number of INPUTS
/* **************************************************************** */
#ifdef INPUTS_MAX

unsigned int inp=8;				// index of the input
unsigned int inputs_cyclic_index=0;	// cycle throug the inputs to return in time to the oscillators

unsigned char inputs_IN_state[INPUTS_MAX];
char inputs_IN_ADDR[INPUTS_MAX];	// addr, like PIN for analog inputs
short inputs_IN_last[INPUTS_MAX];


unsigned char inputs_in2out_method[INPUTS_MAX];
  // bitmask
/* ################ must be thought about more...... ################ */
/* ################ put them in the list display, too ################ */
  #define METHOD_add		1	// 0: set parameter 1: add to parameter
  #define METHOD_continuous	2	// act every time, not only on change
  #define METHOD_linear		4	// math method linear/multiply
  // valid bits, PLEASE ALWAYS DO UPDATE!
  unsigned char in2out_method_valid_mask=0x00000111;

// parameters for the in2out translation:
short inputs_offset[INPUTS_MAX];
long  inputs_output_offset[INPUTS_MAX];
double inputs_in2out_scaling[INPUTS_MAX];
double inputs_in2out_reminder[INPUTS_MAX];


// destination
unsigned char inputs_in2out_destination_type[INPUTS_MAX];	// i.e. osc, analog out
#define TYPE_no			0	//
#define TYPE_osc_period		1	// oscillator period
// #define TYPE_osc_phase  	2	// oscillator phase
#define TYPE_analog_out		3	// analog output

unsigned char inputs_in2out_destination_index[INPUTS_MAX];	// osc


// Do this once:
void inputs_initialize() {
  int inp;

  for (inp=0; inp<INPUTS_MAX; inp++) {
    inputs_IN_state[inp] = 0;
    inputs_IN_last[inp] = ILLEGALinputVALUE;
    inputs_in2out_method[inp] = 0;
    inputs_offset[inp] = 0;
    inputs_output_offset[inp] = 0;
    inputs_in2out_scaling[inp] = 1.0;
    inputs_in2out_reminder[inp] = 0.0;
    inputs_in2out_destination_type[inp] = TYPE_no;
    inputs_in2out_destination_index[inp] = 0;
  }
}


// Do this once for each input:
int input_setup(char pin, unsigned char state, unsigned char method,  \
		       short in_offset, long out_offset, double i2o_scaling, \
		       unsigned char destination_type, unsigned char index) {
  static unsigned char inputs=0;
  int inp= inputs;

  if (inp >= INPUTS_MAX) {	// ERROR

#ifdef USE_SERIAL
    Serial.println("ERROR: too many inputs.");
#endif

    return ILLEGAL;
  }

  inputs_IN_ADDR[inp]			= pin;
  inputs_IN_state[inp]			= state;

  inputs_in2out_method[inp]		= method;
  inputs_offset[inp]		= in_offset;
  inputs_output_offset[inp]		= out_offset;
  inputs_in2out_scaling[inp]		= i2o_scaling;
  inputs_in2out_destination_type[inp]	= destination_type;
  inputs_in2out_destination_index[inp]	= index;

  return inputs++;
}


short analog_IN(int inp) {
  return analogRead(analog_IN_ADDR[inp]);
}


long inputs_inval2out(int inp, int input_raw_value) {
  // Despite the long integer return type we do compute a long float value.
  // When adding to parameters we can accumulate cut-offs in inputs_in2out_reminder[inp]
  double value = input_raw_value;
  long out_value;

  if (inputs_in2out_method[inp] & METHOD_linear) {
    value += inputs_offset[inp];
    value *= inputs_in2out_scaling[inp];;
    value += inputs_output_offset[inp];
  } // else: ERROR

  if(inputs_in2out_method[inp] & METHOD_add) {
    value += inputs_in2out_reminder[inp];		// reminder from last time
    out_value = (long) value;				// integer part
    inputs_in2out_reminder[inp] = value - out_value;	// new reminder
  }

  return (long) value;
}


/* **************************************************************** */
// inputs_cyclic_poll(), main routine dealing with inputs.
// The top level loop calls it whenever there is time.
// The routine checks *one* active input and triggers action, if appropriate.
// Then it returns immediately not to block more realtime critical tasks.

void inputs_cyclic_poll() {
  int actual_index=inp;
  int value, osc;
  long now;

  {
    unsigned int i, inp;
    // find the next active input starting from inputs_cyclic_index:
    for (i=0; i<=INPUTS_MAX; i++) {
      inp = (inputs_cyclic_index++ % INPUTS_MAX); // try all inputs, starting
      if (inputs_IN_state[inp] & ACTIVE_undecided)	// found next active input
	break;
      if (i == INPUTS_MAX)				// there's no active input
	goto cyclic_done;
    }

    value = analog_IN(inp);
    // actions are triggered if the input has changed or if it's in continuous mode:
    if ((inputs_in2out_method[inp] & METHOD_continuous) || (value != inputs_IN_last[inp])) {
      inputs_IN_last[inp] = value;

      switch (inputs_in2out_destination_type[inp]) {

      case TYPE_no:	// nothing
	break;

      case TYPE_osc_period: // set oscillator period
	osc = inputs_in2out_destination_index[inp];

	// add or set?
	if (inputs_in2out_method[inp] & METHOD_add)	// add to parameter
  	  period[osc] += inputs_inval2out(inp, value);
	else						// set parameter
	  period[osc] = inputs_inval2out(inp, value);

	// check if next[osc] is not too far in the future:
	now = micros();
	if ((now + period[osc]) < next[osc]) { // we will not wait that long...
	  next[osc] = now + period[osc];
	  nextFlip = updateNextFlip();
	}
	break;

      default:	// ERROR

#ifdef USE_SERIAL
	Serial.print("inputs_cyclic_poll: unknown inputs_in2out_destination_type[inp].");
	Serial.print("\tinp "); Serial.println((int) inp);
#endif

	break;

      }
    }
  }

  cyclic_done:
  inp = actual_index;	// restore index
}


void display_inputs_info() {
  int global_inp=inp;
  int inp, value;

  Serial.println("  state\tpin\tvalue\tlast\tout val\tmethod\tin offs\toutoff\tscaling\tout\ttype");
  Serial.println("");
  for (inp=0; inp<INPUTS_MAX; inp++) {
    if (inp==global_inp)
      Serial.print(" * ");
    else
      Serial.print("   ");

    if (inputs_IN_state[inp] & ACTIVE_undecided)
      Serial.print("ON\t");
    else
      Serial.print("off\t");

    Serial.print((int) inputs_IN_ADDR[inp]); Serial.print("\t");

    Serial.print(value = analogRead(inputs_IN_ADDR[inp])); Serial.print("\t");
    Serial.print(inputs_IN_last[inp]); Serial.print("\t");
    Serial.print(inputs_inval2out(inp,value)); Serial.print("\t");

    Serial.print((int) inputs_in2out_method[inp]); Serial.print("\t");
    Serial.print(inputs_offset[inp]); Serial.print("\t");
    Serial.print(inputs_output_offset[inp]); Serial.print("\t");
    Serial.print(inputs_in2out_scaling[inp]); Serial.print("\t");

    Serial.print((int) inputs_in2out_destination_index[inp]); Serial.print("\t");
    switch(inputs_in2out_destination_type[inp]) {
    case TYPE_osc_period:
      Serial.print("oscperi");
      break;

    case TYPE_analog_out:
      Serial.print("analog");
      break;

    case TYPE_no:
      Serial.print("(none)");
      break;

    default:
      Serial.print("unknown");
    }

    Serial.println("");
  }
}

#endif	// INPUTS_MAX
/* **************************************************************** */

#endif  // DO_COMPILE_OLD_SOURCE
