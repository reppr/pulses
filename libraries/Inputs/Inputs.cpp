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


#ifdef ESP32	// ESP32 has no analogWrite() yet
  void analogWrite(int channel, int value) {
    ledcWrite(channel, value);
  }
#endif


/* **************************************************************** */
// Constructor:
Inputs::Inputs(int inputs_to_allocate):
  selected_inputs(0),
  active_inputs(0)
{

  inputs = (input_t *) malloc(inputs_to_allocate * sizeof(input_t));
  if (inputs == NULL)
    inputs_allocated=0;
  else {
    memset(inputs, 0, inputs_to_allocate * sizeof(input_t));
    inputs_allocated=inputs_to_allocate;
    next_input=0;

    for (int inp=0; inp<inputs_to_allocate; inp++) {
      inputs[inp].sample_method = NULL;
      inputs[inp].samples = NULL;
      inputs[inp].in2o_method = NULL;
    }
  }
}

/* ****************  Destructor:  **************** */
Inputs::~Inputs() {
  for (int inp=0; inp<inputs_allocated; inp++)
    free(inputs[inp].samples);

  free(inputs);
}


/* ****************  top level user function:  **************** */
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
bool Inputs::do_next_input(void) {
  int inp;

  for (int i=0; i<inputs_allocated ; i++) {
    inp=next_input;
    next_input = ++next_input % inputs_allocated;

    if(inputs[inp].flags & INPUT_ACTIVE)	// active inputs only
      return sample_and_react(inp);		// take sample, maybe react...
  }

  return false;		// *no* inputs active
}

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
bool Inputs::sample_and_react(int inp) {
  if(!(inputs[inp].flags & INPUT_ACTIVE))	// active inputs only
    return false;

  if(! sample(inp))	// sample, and...
    return false;	//   return false  if there's nothing else to do

  if(!(inputs[inp].flags & INPUT_OUTPUT_REACTION))
    return false;	// no reactions configured, return false

  // ok, so let's prepare for reaction:
  // first get sample_value from oversampling and friends:
  int sample_value;
  if(inputs[inp].flags & OVERSAMPLE_AVERAGE)
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
  if (inputs[inp].flags & SET_PWM) {
    if (output_value > PWMRANGE)
      output_value=PWMRANGE;
    else
      if (output_value < 0)
	output_value=0;
  }
//  else
//    inputs[inp].out_reaction(inp, output_value);	// custom output	FIXME: todo ################

#if defined(ARDUINO)
  #ifdef ESP32
    // Setup timer and attach timer to a led pin
    #define LEDC_CHANNEL_0	0
    #define LEDC_BASE_FREQ	5000
    #define LEDC_TIMER_13_BIT	13	// ################ FIXME: test PWMBASE
    #define LED_PIN		2
    ledcAttach(LED_PIN, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);	// FIXME: TEST new LEDC espressif software version
  #endif

  analogWrite(inputs[inp].out_A, output_value);	// inbuilt PWM
#endif

  inputs[inp].last_output=output_value;

  return true;
}


/*
  bool malloc_samples(int inp, unsigned int oversample);
  (free old sample memory, and)
  malloc memory for 'oversample' bytes:
*/
bool Inputs::malloc_samples(int inp, unsigned int oversample) {
  free(inputs[inp].samples);
  inputs[inp].samples = (int *) malloc(oversample * sizeof(int));
  if (inputs[inp].samples == NULL) {
    inputs[inp].oversample = 0;
    return false;	// not enough RAM
  }

  memset(inputs[inp].samples, 0, (oversample * sizeof(int)));
  inputs[inp].oversample = oversample;
  return true;		// ok
}


/*
  bool sample(int inp);
  Take a sample on inp.
  Return true every time a new oversampling set is ready
  as a trigger for possible reactions.
*/
bool Inputs::sample(int inp) {
  int value;
  unsigned int cyclic_index;
  if (inputs[inp].flags & INPUT_ANALOG_internal)	// analogRead
    value = analogRead(inputs[inp].inp_A);
  else {
    if (inputs[inp].sample_method == NULL)	// ERROR no sample method known
      return false;	// Silently return, no further error treatment here...

    value=(*(*inputs[inp].sample_method))((int) inputs[inp].inp_A);
  }

  cyclic_index = (inputs[inp].counter++ % inputs[inp].oversample);
  inputs[inp].samples[cyclic_index]=value;

  if (++cyclic_index == inputs[inp].oversample)	// oversampling set ready?
    return true;		// oversampling set is ready, trigger possible reactions
  else
    return false;		// oversampling continues
}

void Inputs::fix_active_inputs() {
  active_inputs=0;
  for (int inp=0; inp<inputs_allocated ; inp++) {
    if(inputs[inp].flags & INPUT_ACTIVE)	// active inputs only
      active_inputs |= (1 << inp);
  }
}

/* ****************  setup sampling functions:  **************** */
/*
  bool setup_analog_read(int inp, gpio_pin_t addr, unsigned int oversample);
  Setup internal analogRead(pin), and reserve memory for oversampling:
*/
bool Inputs::setup_analog_read(int inp, gpio_pin_t addr, unsigned int oversample) {
  if ((inp < 0) or (inp >= inputs_allocated))
    return false;	// inp out of range

  if (malloc_samples(inp, oversample)) {
    inputs[inp].flags |= INPUT_ANALOG_internal;
    inputs[inp].inp_A = addr;
    if(oversample>1)
      inputs[inp].flags |= OVERSAMPLE_AVERAGE;
    return true;	// everything ok
  } else
    return false;	// not enough RAM
}


/*
  bool setup_sample_method(int inp, int (*sample_method)(int addr), \
			   gpio_pin_t addr, unsigned int oversample)
  Setup a custom input sample method and reserve memory for oversampling:
*/
bool Inputs::setup_sample_method(int inp, int (*take_sample)(int addr), \
				 gpio_pin_t addr, unsigned int oversample)
{
  if ((inp < 0) or (inp >= inputs_allocated))
    return false;	// inp out of range

  inputs[inp].sample_method = take_sample;
  inputs[inp].inp_A = addr;

  if (malloc_samples(inp, oversample))
    return true;	// everything ok
  else
    return false;	// not enough RAM
}


/* ****************  oversampling:  **************** */
/*
  int oversamples_average(int inp);
  return average of the saved samples
*/
int Inputs::oversamples_average(int inp) const {
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
unsigned int Inputs::oversamples_deviation(int inp) const {
  int oversample=inputs[inp].oversample;
  int average=oversamples_average(inp);

  int deviation=0;
  for(int i=0; i < oversample; i++)
    deviation += abs(average - inputs[inp].samples[i]);

  deviation  += oversample / 2;	// integers rounding, no float samples...
  return deviation / oversample;
}


/* ****************  input to output calculation:  **************** */
/*
  ioV_t Inputs::in2o_calculation(int inp, ioV_t value)
  Calculate and return the output value from an input value
  according the roules for input inp.
*/
ioV_t Inputs::in2o_calculation(int inp, ioV_t value) const {
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


/* ****************  setup input to output calculation functions:  *** */
/*
  bool Inputs::setup_raw(int inp)
  Setup input inp to pass through raw values without further processing.
  Oversampling and friends might still be active though.
*/
bool Inputs::setup_raw(int inp) {
  if ((inp < 0) or (inp >= inputs_allocated))
    return false;	// inp out of range

  inputs[inp].flags |= INPUT_OUTPUT_REACTION;
  inputs[inp].flags &= ~INPUT_PROCESSING;
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

  inputs[inp].in_offset = in_offset;
  inputs[inp].mul = mul;
  inputs[inp].div = div;
  inputs[inp].out_offset = out_offset;
  return true;
}


/* ****************  output reaction functions:  **************** */
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


/*
  bool setup_PWM(int inp, gpio_pin_t pin);
  Setup PWM output on input inp, using PWM pin (no checks...).
  On ARDUINO initialize pin.
*/
bool Inputs::setup_PWM(int inp, gpio_pin_t pin) {
  if ((inp < 0) or (inp >= inputs_allocated))
    return false;	// inp out of range

  inputs[inp].out_A = pin;		// no checks here...
  inputs[inp].flags |= SET_PWM;
  inputs[inp].flags |= INPUT_OUTPUT_REACTION;

  // on ARDUINO initialize pin:
#if defined(ARDUINO)
  #if ! defined(ESP32)	// ################ FIXME: anbalogWrite() on ESP32 ################
    pinMode(pin, OUTPUT);
  #endif
  analogWrite(pin, 0);
#endif

  return true;
}


/* **************************************************************** */
/* **************************************************************** */
/*    SOURCE ENDS HERE.						    */
/* **************************************************************** */
/* **************************************************************** */
