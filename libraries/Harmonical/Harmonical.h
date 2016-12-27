/* **************************************************************** */
/*
   Harmonics.h
*/
/* **************************************************************** */
#ifndef HARMONICS_h
#define HARMONICS_h

#include <stdio.h>
#include <stdlib.h>

#ifdef ARDUINO
  #define STREAMTYPE	Stream

  /* Keep ARDUINO GUI happy ;(				*/
  #if ARDUINO >= 100
    #include "Arduino.h"
  #else
    #include "WProgram.h"
  #endif
#else
  #include <iostream>

  #ifndef STREAMTYPE		// could have been #define'd in Pulses.h
    using namespace std;
    ostream & Serial=cout;	// nice trick from johncc
  #endif
  #define STREAMTYPE	ostream
#endif

/* **************************************************************** */
/* class Harmonics {}							*/

class Harmonics {
 public:
  ~Harmonics();
  Harmonics();
};


/* **************************************************************** */
#endif HARMONICS_h
