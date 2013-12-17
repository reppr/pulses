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
Inputs::Inputs(int inputs_allocated):
  inputs_allocated(inputs_allocated)
{
  inputs = (input_t *) malloc(inputs_allocated * sizeof(input_t));
  if (inputs == NULL)
    inputs_allocated=0;
  else {
    memset(inputs, 0, inputs_allocated * sizeof(input_t));

    for (int inp=0; inp<inputs_allocated; inp++) {
      inputs[inp].sample_method = NULL;
      inputs[inp].samples = NULL;
      // c++ did not like me :(
      inputs[inp].in2o_method = NULL;
    }
  }
}

// Destructor:
Inputs::~Inputs() {
  for (int inp=0; inp<inputs_allocated; inp++)
    free(inputs[inp].samples);

  free(inputs);
}


/*
  bool setup_sample_method(int inp, int (*sample_method)(int addr), uint8_t addr, uint8_t oversample)
  Setup an input sample method and reserve memory for oversampling:
*/
bool Inputs::setup_sample_method(int inp, int (*take_sample)(int addr), uint8_t addr, uint8_t oversample)
{
  if ((inp < 0) or (inp >= inputs_allocated))
    return false;	// inp out of range

  inputs[inp].sample_method = take_sample;
  inputs[inp].inp_A = addr;

  free(inputs[inp].samples);
  inputs[inp].samples = (int *) malloc(oversample * sizeof(int));
  if (inputs[inp].samples == NULL)
    return false;	// not enough RAM

  inputs[inp].oversample = oversample;
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

  value=(*(*inputs[inp].sample_method))((int) inputs[inp].inp_A);

  cyclic_index = (inputs[inp].counter++ % inputs[inp].oversample);
  inputs[inp].samples[cyclic_index]=value;

  if (++cyclic_index == inputs[inp].oversample)	// oversampling set ready?
    return true;		// oversampling set is ready, trigger possible reactions
  else
    return false;		// oversampling continues
}


/*
  int oversamples_average(int inp);
  return average of the saved samples
*/
int Inputs::oversamples_average(int inp){
  int oversample=inputs[inp].oversample;
  long sum=0;
  for(int i=0; i < oversample; i++)
    sum += inputs[inp].samples[i];
  sum  += oversample / 2;	// integers rounding, no float samples...
  return sum / oversample;
}


/*
  unsigned int oversamples_deviation(int inp);
  get average of absolute deviation against average of all samples
*/
unsigned int Inputs::oversamples_deviation(int inp) {
  int oversample=inputs[inp].oversample;
  int average=oversamples_average(inp);

  int deviation=0;
  for(int i=0; i < oversample; i++)
    deviation += abs(average - inputs[inp].samples[i]);

  deviation  += oversample / 2;	// integers rounding, no float samples...
  return deviation / oversample;
}


/*
  ioV_t Inputs::in2o_calculation(int inp, ioV_t value)
  Calculate and return the output value from an input value
  according the roules for input inp.
*/
ioV_t Inputs::in2o_calculation(int inp, ioV_t value) {
  ioV_t outval;

  if(inputs[inp].flags & PROCESS_CUSTOM)
    return inputs[inp].in2o_method(inp, value);

  // getting these only *once*, hope that's faster:
  // (as all calculations are in ioV_t i declare all operands ioV_t)
  ioV_t in_offset = inputs[inp].in_offset;
  if (in_offset)
    value += in_offset;

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

  ioV_t out_offset = inputs[inp].out_offset;
  if (out_offset)
    outval += out_offset;

  return outval;
}


/*
  bool Inputs::setup_raw(int inp)
  Setup input inp to pass through raw values without further processing.
  Oversampling and friends might still be active though.
*/
bool Inputs::setup_raw(int inp) {
  if ((inp < 0) or (inp >= inputs_allocated))
    return false;	// inp out of range

  inputs[inp].flags &= ~INPUT_PROCESSING;
  return true;
}


/*
  bool Inputs::setup_linear(int inp,\
         ioP_t in_offset, ioP_t mul, ioP_t div, ioV_t out_offset, bool inverse)
  Configure a linear or reverse linear in2out function on input inp.

  // aliases for io_calculation
  PROPORTIONAL	false
  INVERSE	true
*/
bool Inputs::setup_linear(int inp, \
       ioP_t in_offset, ioP_t mul, ioP_t div, ioV_t out_offset, bool inverse)
{
  if ((inp < 0) or (inp >= inputs_allocated))
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

  inputs[inp].in_offset = in_offset;
  inputs[inp].mul = mul;
  inputs[inp].div = div;
  inputs[inp].out_offset = out_offset;
  return true;
}


/*
  bool setup_in2o_custom(int inp, ioV_t (*method)(int inp, ioV_t value));
  setup a custom method to calculate output value from sample value.
*/
bool Inputs::setup_in2o_custom(int inp, ioV_t (*method)(int inp, ioV_t value)) {
  if ((inp < 0) or (inp >= inputs_allocated))
    return false;	// inp out of range

  inputs[inp].flags &= ~(PROCESS_LINEAR & PROCESS_INVERSE);
  inputs[inp].flags |= INPUT_PROCESSING;
  inputs[inp].flags |= PROCESS_CUSTOM;

  inputs[inp].in2o_method = method;
  return true;
}


/*
  bool sample_and_react(int inp);
  take a sample on inp
  react adequately,
    like: on completing a new oversampling set
          compute average, do in2o_calculation on that
	  influence the period of a pulse, or whatever...
*/
bool Inputs::sample_and_react(int inp) {
  if(! sample(inp))	// sample, and...
    return false;	//   return false  if there's nothing else to do

  if(!(inputs[inp].flags & INPUT_OUTPUT_REACTION))
    return false;	// no reactions configured, return false

  // ok, so let's prepare for reaction:
  // first get sample_value from oversampling and friends:
  int sample_value;
  if(inputs[inp].flags & OVERSAMLE_AVERAGE )
    sample_value = oversamples_average(inp);
  else
    sample_value = get_last_sampled(inp);

  // now do in2o_calculation:
  ioV_t output_value;
  if(inputs[inp].flags & INPUT_PROCESSING)
    output_value = in2o_calculation(inp, (ioV_t) sample_value);
  else
    output_value = sample_value;

  // call the reaction of that input with that output_value
  inputs[inp].out_reaction(inp, output_value);
  return true;
}


/*
  bool setup_io_reaction(int inp, void (*reaction)(int inp, ioV_t value));
  setup a reaction 'void reaction(int inp, ioV_t value)' to be done when
  reaching a full oversampling set
*/
bool Inputs::setup_io_reaction(int inp, void (*reaction)(int inp, ioV_t value)) {
  if ((inp < 0) or (inp >= inputs_allocated))
    return false;	// inp out of range

  inputs[inp].out_reaction = reaction;
  inputs[inp].flags |= INPUT_OUTPUT_REACTION;

  return true;
}


/* **************************************************************** */
/* **************************************************************** */
/*    SOURCE ENDS HERE.						    */
/* **************************************************************** */
/* **************************************************************** */
