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

Pulses::Pulses(unsigned int pl_max):
  pl_max(pl_max),
  pindex(0),
  global_octave(0),
  global_octave_mask(1),
  current_global_octave_mask(1),
  nextFlip(farest_future)
{
  pulses = (pulse*) malloc(pl_max * sizeof(pulse));
  // ERROR ################
}

//	#ifndef ARDUINO		// WARNING: Using Stream MACRO hack when not on ARDUINO!
//	  #undef Stream
//	#endif


Pulses::~Pulses() {
  free(pulses);
}


/* **************************************************************** */
void Pulses::pulses_init() {	// ################ move to constructor
  unsigned int pindex;

  for (pindex=0; pindex<pl_max; pindex++) {
    pulses[pindex].counter=0;

    // use pl_mask's like 0x1 or 0x3
    pulses[pindex].pl_mask = 0x1 << DEFAULT_OCTAVE_SHIFT;

    pulses[pindex].period = pulses[pindex].next = farest_future;
    pulses[pindex].pl_flags = 0;		// off
    pinMode(pulses[pindex].pl_PIN, OUTPUT);
    pulses[pindex].pl_pin_mask=ILLEGALpin;
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


int Pulses::start_pulse(int pindex) {
  unsigned long now = micros();

  if (pindex >= pl_max ) {	// ERROR recovery needed! #########

#ifdef USE_SERIAL
    // ################
    // Serial.println("ERROR: too many oscillators.");
#endif

    return 1;
  }

  if (pulses[pindex].pl_PIN == ILLEGALpin)
    return 1;

  digitalWrite(pulses[pindex].pl_PIN, HIGH);

  pulses[pindex].pl_flags |= pACTIVE;	// active ON
  pulses[pindex].next = now + pulses[pindex].period;
  nextFlip = updateNextFlip();

  /*
  Serial.print("Started oscillator "); Serial.print(pindex);
  Serial.print("\tpin "); Serial.print(pulses[pindex].pl_PIN);
  Serial.print("\tperiod "); Serial.println(pulses[pindex].period);
  */

  return 0;
}


void Pulses::stop_pulse(int pindex) {
  pulses[pindex].pl_flags &= ~pACTIVE;	// active OFF
  digitalWrite(pulses[pindex].pl_PIN, LOW);
  nextFlip = updateNextFlip();
}


bool Pulses::HIGHorLOW(int pindex) {		// is pulse HIGH or LOW?
  return (pulses[pindex].counter & current_global_octave_mask);
  // ################  return (pulses[pindex].counter & current_global_octave_mask) !=0;
}


// compute when the next flip (in any of the active oscillators is due
long Pulses::updateNextFlip() {
  int pindex;
  nextFlip |= -1;

  for (pindex=0; pindex<pl_max; pindex++) {
    if (pulses[pindex].pl_flags & pACTIVE)
      if (pulses[pindex].next < nextFlip)
	nextFlip = pulses[pindex].next;
  }
  // Serial.print("NEXT "); Serial.println(nextFlip);
  return nextFlip;
}


/* **************************************************************** */

/* **************************************************************** */
