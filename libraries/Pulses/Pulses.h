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
/* Some basic #define's						    */

#define ILLEGAL			-1	// illegal value for many topics

#define ILLEGALinputVALUE	-1	// impossible analog input value
#define ILLEGALpin		-1	// a pin that is not assigned

#define ACTIVE_undecided	1	// globally used in many status bitmaps


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
#define DEFAULT_OCTAVE_SHIFT	6
const unsigned long farest_future = ~0L ;


/* **************************************************************** */

// ################comment
// enum flag_codes {ACTIVE=1, MUTE=2} pl_flag_codes;
enum {ACTIVE=1, MUTE=2} pl_flag_code;

struct pulse {
  unsigned long period;
  unsigned long next;
  unsigned long counter;	// counts pulses
  unsigned long pl_mask;	// for octaves and rhythms
  uint8 pl_flags;		// ################comment
  uint8 pl_PIN;			// for clicks and the like
  uint8 pl_pin_mask;		// ################comment
};


/* **************************************************************** */

class Pulses {
 public:
  Pulses(unsigned int max_pl);
  ~Pulses();
 private:
  unsigned int pl_i;		// pulse index, not decided if i need it ################
  unsigned int max_pl;		// max pulses possible
  unsigned long nextFlip;	// ################...comment please...

  // ################ int16 ??? ################
  int8 global_octave;			    // global octave shift. ONLY negative shifts atm
  unsigned long global_octave_mask;	    // this mask gets shifted and then actually used
  unsigned long current_global_octave_mask; // actually used mask to switch oscillators
};

/* **************************************************************** */
#endif
