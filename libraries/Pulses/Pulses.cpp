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
/* Some basic #define's						    */

#define ILLEGAL			-1	// illegal value for many topics

#define ILLEGALinputVALUE	-1	// impossible analog input value
#define ILLEGALpin		-1	// a pin that is not assigned

#define ACTIVE_undecided	1	// globally used in many status bitmaps


/* **************************************************************** */
// Constructor/Destructor:

#ifndef ARDUINO		// WARNING: Using Stream MACRO hack when not on ARDUINO!
  #define Stream ostream
#endif

Pulses::Pulses(int max_pl):
  max_pl(max_pl),
  p(0)
{
}

#ifndef ARDUINO		// WARNING: Using Stream MACRO hack when not on ARDUINO!
  #undef Stream
#endif


Pulses::~Pulses() {
}


/* **************************************************************** */

/* **************************************************************** */
