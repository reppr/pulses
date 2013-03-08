/* **************************************************************** */
/*
   Pulses.h
*/
/* **************************************************************** */
#ifndef PULSES_h
#define PULSES_h

#include <stdio.h>
#include <stdlib.h>

#ifdef ARDUINO
  #define STREAMTYPE	Stream
#else
  #include <iostream>

  using namespace std;
  ostream & Serial=cout;	// nice trick from johncc
  #define STREAMTYPE	ostream
#endif

/* **************************************************************** */
#ifndef USE_F_MACRO
  // For tests and on PC:  Fake Arduino F("string") macro as noop:
  #define F(s)	(s)
#endif


/* **************************************************************** */
// Preprocessor macro logic:

/* Debugging macros:		*/
#ifdef DEBUGGING_ALL

#endif


#if defined(ARDUINO)
  #if ARDUINO >= 100
    #include "Arduino.h"
  #else
    #include "WProgram.h"
  #endif
#endif // ARDUINO

/* **************************************************************** */

class Pulses {
 public:
  Pulses(int max_pl);
  ~Pulses();
 private:
  int max_pl:
};

/* **************************************************************** */
#endif
