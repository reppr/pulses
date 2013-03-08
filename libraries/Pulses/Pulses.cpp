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

#ifndef ARDUINO		// WARNING: Using Stream MACRO hack when not on ARDUINO!
  #define Stream ostream
#endif

Pulses::Pulses(int max_pl):
  max_pl(max_pl)
{
}

#ifndef ARDUINO		// WARNING: Using Stream MACRO hack when not on ARDUINO!
  #undef Stream
#endif


Pulses::~Pulses() {
}


/* **************************************************************** */

/* **************************************************************** */
