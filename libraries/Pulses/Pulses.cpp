/* **************************************************************** */
/*
  Pulses.cpp
*/

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

#else
  #include <iostream>
#endif

#include <Pulses.h>


/* **************************************************************** */
// Constructor/Destructor:

//	#ifndef ARDUINO		// WARNING: Using Stream MACRO hack when not on ARDUINO!
//	  #define Stream ostream
//	#endif

Pulses::Pulses(unsigned int max_pl):
  max_pl(max_pl),
  pl_i(0),
  global_octave(0),
  global_octave_mask(1),
  current_global_octave_mask(1)
{
  pulses = (pulse*) malloc(max_pl * sizeof(pulse));
  // ERROR ################
}

//	#ifndef ARDUINO		// WARNING: Using Stream MACRO hack when not on ARDUINO!
//	  #undef Stream
//	#endif


Pulses::~Pulses() {
  free(pulses);
}


/* **************************************************************** */
void Pulses::pulses_init() {
  unsigned int pl_i;

  for (pl_i=0; pl_i<max_pl; pl_i++) {
    pulses[pl_i].counter=0;

    // use pl_mask's like 0x1 or 0x3
    pulses[pl_i].pl_mask = 0x1 << DEFAULT_OCTAVE_SHIFT;

    pulses[pl_i].period = pulses[pl_i].next = farest_future;
    pulses[pl_i].pl_flags = 0;		// off
    pinMode(pulses[pl_i].pl_PIN, OUTPUT);
    pulses[pl_i].pl_pin_mask=ILLEGALpin;
  }
  nextFlip = farest_future;
}


void Pulses::global_shift(int global_octave) {
  if (global_octave>0) {
    // ################ no Serial here...
    // Serial.print("global_shift: ERROR only negative octave shifts implemented.\n");
    return;
  }
  current_global_octave_mask = global_octave_mask << -global_octave;
}


int Pulses::start_pulse(int pl_i) {
  unsigned long now = micros();

  if (pl_i >= max_pl ) {	// ERROR recovery needed! #########

#ifdef USE_SERIAL
    // ################
    // Serial.println("ERROR: too many oscillators.");
#endif

    return 1;
  }

  if (pulses[pl_i].pl_PIN == ILLEGALpin)
    return 1;

  digitalWrite(pulses[pl_i].pl_PIN, HIGH);

  pulses[pl_i].pl_flags |= pACTIVE;	// active ON
  pulses[pl_i].next = now + pulses[pl_i].period;
  nextFlip = updateNextFlip();

  /*
  Serial.print("Started oscillator "); Serial.print(pl_i);
  Serial.print("\tpin "); Serial.print(pulses[pl_i].pl_PIN);
  Serial.print("\tperiod "); Serial.println(pulses[pl_i].period);
  */

  return 0;
}


void Pulses::stop_pulse(int pl_i) {
  pulses[pl_i].pl_flags &= ~pACTIVE;	// active OFF
  digitalWrite(pulses[pl_i].pl_PIN, LOW);
  nextFlip = updateNextFlip();
}


bool Pulses::HIGHorLOW(int pl_i) {		// is pulse HIGH or LOW?
  return (pulses[pl_i].counter & current_global_octave_mask);
  // ################  return (pulses[pl_i].counter & current_global_octave_mask) !=0;
}


// compute when the next flip (in any of the active oscillators is due
long Pulses::updateNextFlip() {
  int pl_i;
  nextFlip |= -1;

  for (pl_i=0; pl_i<max_pl; pl_i++) {
    if (pulses[pl_i].pl_flags & pACTIVE)
      if (pulses[pl_i].next < nextFlip)
	nextFlip = pulses[pl_i].next;
  }
  // Serial.print("NEXT "); Serial.println(nextFlip);
  return nextFlip;
}


/* **************************************************************** */

/* **************************************************************** */
