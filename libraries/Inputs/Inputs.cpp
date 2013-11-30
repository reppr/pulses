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



#ifdef DO_NOT_COMPILE_OLD_SOURCE
/*
  old source from
  pulses/misc/src/arduino-git1/polyphonic_oscillators/polyphonic_oscillators.pde
*/

/* **************************************************************** */
// #define INPUTs_ANALOG	16	// ANALOG INPUTS
/* **************************************************************** */
#ifdef INPUTs_ANALOG

unsigned int inp=8;				// index of the analog input
unsigned int analog_input_cyclic_index=0;	// cycle throug the inputs to return in time to the oscillators

unsigned char analog_IN_state[INPUTs_ANALOG];
char analog_IN_PIN[INPUTs_ANALOG];
short analog_IN_last[INPUTs_ANALOG];


unsigned char analog_in2out_method[INPUTs_ANALOG];
  // bitmask
/* ################ must be thought about more...... ################ */
/* ################ put them in the list display, too ################ */
  #define METHOD_add		1	// 0: set parameter 1: add to parameter
  #define METHOD_continuous	2	// act every time, not only on change
  #define METHOD_linear		4	// math method linear/multiply
  // valid bits, PLEASE ALWAYS DO UPDATE!
  unsigned char in2out_method_valid_mask=0x00000111;

// parameters for the in2out translation:
short analog_input_offset[INPUTs_ANALOG];
long  analog_output_offset[INPUTs_ANALOG];
double analog_in2out_scaling[INPUTs_ANALOG];
double analog_in2out_reminder[INPUTs_ANALOG];


// destination
unsigned char analog_in2out_destination_type[INPUTs_ANALOG];	// i.e. osc, analog out
#define TYPE_no			0	//
#define TYPE_osc_period		1	// oscillator period
// #define TYPE_osc_phase  	2	// oscillator phase
#define TYPE_analog_out		3	// analog output

unsigned char analog_in2out_destination_index[INPUTs_ANALOG];	// osc


// Do this once:
void analog_inputs_initialize() {
  int inp;

  for (inp=0; inp<INPUTs_ANALOG; inp++) {
    analog_IN_state[inp] = 0;
    analog_IN_last[inp] = ILLEGALinputVALUE;
    analog_in2out_method[inp] = 0;
    analog_input_offset[inp] = 0;
    analog_output_offset[inp] = 0;
    analog_in2out_scaling[inp] = 1.0;
    analog_in2out_reminder[inp] = 0.0;
    analog_in2out_destination_type[inp] = TYPE_no;
    analog_in2out_destination_index[inp] = 0;
  }
}


// Do this once for each analog input:
int analog_input_setup(char pin, unsigned char state, unsigned char method,  \ 
		       short in_offset, long out_offset, double i2o_scaling, \
		       unsigned char destination_type, unsigned char index) {
  static unsigned char analog_inputs=0;
  int inp= analog_inputs;

  if (inp >= INPUTs_ANALOG) {	// ERROR

#ifdef USE_SERIAL
    Serial.println("ERROR: too many analog inputs.");
#endif

    return ILLEGAL;
  }

  analog_IN_PIN[inp]			= pin;
  analog_IN_state[inp]			= state;

  analog_in2out_method[inp]		= method;
  analog_input_offset[inp]		= in_offset;
  analog_output_offset[inp]		= out_offset;
  analog_in2out_scaling[inp]		= i2o_scaling;
  analog_in2out_destination_type[inp]	= destination_type;
  analog_in2out_destination_index[inp]	= index;

  return analog_inputs++;
}


short analog_IN(int inp) {
  return analogRead(analog_IN_PIN[inp]);
}


long analog_inval2out(int inp, int input_raw_value) {
  // Despite the long integer return type we do compute a long float value.
  // When adding to parameters we can accumulate cut-offs in analog_in2out_reminder[inp]
  double value = input_raw_value;
  long out_value;

  if (analog_in2out_method[inp] & METHOD_linear) {
    value += analog_input_offset[inp];
    value *= analog_in2out_scaling[inp];;
    value += analog_output_offset[inp];
  } // else: ERROR

  if(analog_in2out_method[inp] & METHOD_add) {
    value += analog_in2out_reminder[inp];		// reminder from last time
    out_value = (long) value;				// integer part
    analog_in2out_reminder[inp] = value - out_value;	// new reminder
  }
    
  return (long) value;
}


/* **************************************************************** */
// analog_input_cyclic_poll(), main routine dealing with analog inputs.
// The top level loop calls it whenever there is time.
// The routine checks *one* active input and triggers action, if appropriate.
// Then it returns immediately not to block more realtime critical tasks.

void analog_input_cyclic_poll() {
  int actual_index=inp;
  int value, osc;
  long now;

  {
    unsigned int i, inp;
    // find the next active input starting from analog_input_cyclic_index: 
    for (i=0; i<=INPUTs_ANALOG; i++) {
      inp = (analog_input_cyclic_index++ % INPUTs_ANALOG); // try all inputs, starting
      if (analog_IN_state[inp] & ACTIVE_undecided)	// found next active input
	break;
      if (i == INPUTs_ANALOG)				// there's no active input
	goto cyclic_done;
    }

    value = analog_IN(inp);
    // actions are triggered if the input has changed or if it's in continuous mode:
    if ((analog_in2out_method[inp] & METHOD_continuous) || (value != analog_IN_last[inp])) {
      analog_IN_last[inp] = value;

      switch (analog_in2out_destination_type[inp]) {

      case TYPE_no:	// nothing
	break;

      case TYPE_osc_period: // set oscillator period
	osc = analog_in2out_destination_index[inp];

	// add or set?
	if (analog_in2out_method[inp] & METHOD_add)	// add to parameter
  	  period[osc] += analog_inval2out(inp, value);
	else						// set parameter
	  period[osc] = analog_inval2out(inp, value);

	// check if next[osc] is not too far in the future:
	now = micros();
	if ((now + period[osc]) < next[osc]) { // we will not wait that long...
	  next[osc] = now + period[osc];
	  nextFlip = updateNextFlip();
	}
	break;

      default:	// ERROR

#ifdef USE_SERIAL
	Serial.print("analog_input_cyclic_poll: unknown analog_in2out_destination_type[inp].");
	Serial.print("\tinp "); Serial.println((int) inp);
#endif

	break;

      }
    }
  }

  cyclic_done:
  inp = actual_index;	// restore index
}


void display_analog_inputs_info() {
  int global_inp=inp;
  int inp, value;

  Serial.println("  state\tpin\tvalue\tlast\tout val\tmethod\tin offs\toutoff\tscaling\tout\ttype");
  Serial.println("");
  for (inp=0; inp<INPUTs_ANALOG; inp++) {
    if (inp==global_inp)
      Serial.print(" * ");
    else
      Serial.print("   ");

    if (analog_IN_state[inp] & ACTIVE_undecided)
      Serial.print("ON\t");
    else
      Serial.print("off\t");

    Serial.print((int) analog_IN_PIN[inp]); Serial.print("\t");

    Serial.print(value = analogRead(analog_IN_PIN[inp])); Serial.print("\t");
    Serial.print(analog_IN_last[inp]); Serial.print("\t");
    Serial.print(analog_inval2out(inp,value)); Serial.print("\t");

    Serial.print((int) analog_in2out_method[inp]); Serial.print("\t");
    Serial.print(analog_input_offset[inp]); Serial.print("\t");
    Serial.print(analog_output_offset[inp]); Serial.print("\t");
    Serial.print(analog_in2out_scaling[inp]); Serial.print("\t");

    Serial.print((int) analog_in2out_destination_index[inp]); Serial.print("\t");
    switch(analog_in2out_destination_type[inp]) {
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

#endif	// INPUTs_ANALOG
/* **************************************************************** */

#endif  // DO_NOT_COMPILE_OLD_SOURCE
