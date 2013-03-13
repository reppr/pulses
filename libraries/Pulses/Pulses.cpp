/* **************************************************************** */
/*
  Pulses.cpp
*/

/* **************************************************************** */
// Preprocessor #includes:

#include <stdio.h>
#include <stdlib.h>

#ifdef ARDUINO
  /* Keep ARDUINO GUI happy ;(		*/
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
}

//	#ifndef ARDUINO		// WARNING: Using Stream MACRO hack when not on ARDUINO!
//	  #undef Stream
//	#endif


Pulses::~Pulses() {
}


void Pulses::global_shift(int global_octave) {
  if (global_octave>0) {
    // ################ no Serial here...
    Serial.println("global_shift: ERROR only negative octave shifts implemented.");
    return;
  }
  current_global_octave_mask = global_octave_mask << -global_octave;
}

/* **************************************************************** */

/* **************************************************************** */
