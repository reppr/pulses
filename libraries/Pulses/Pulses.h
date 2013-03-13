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

  #ifndef INPUT
    #define INPUT	0
  #endif
  #ifndef OUTPUT
    #define OUTPUT	1
  #endif
  #ifndef LOW
    #define LOW		0
  #endif
  #ifndef HIGH
    #define HIGH	1
  #endif

void pinMode(int p, int mode) {
  printf("Pin %d switched to mode %d\n", p, mode);
}

void digitalWrite(int p, int value) {
  printf("Pin %d set to %d\n", p, value);
}

long micros() { return 9999L; }

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

struct pulse {
  unsigned long period;
  unsigned long next;
  unsigned long counter;	// counts pulses
  unsigned long pl_mask;	// for octaves and rhythms
  unsigned char pl_flags;	// ################comment
				// mask pACTIVE
				// mask pMUTE
  unsigned char pl_PIN;		// for clicks and the like
  unsigned char pl_pin_mask;	// ################comment
};


// ################comment
#define pACTIVE	1
#define pMUTE	2

/* **************************************************************** */

class Pulses {
 public:
  Pulses(unsigned int max_pl);
  ~Pulses();
  void pulses_init();
  void global_shift(int global_octave);
  int start_pulse(int pl_i);
  void stop_pulse(int pl_i);
  bool HIGHorLOW(int pl_i);
  long updateNextFlip();


 private:
  unsigned int pl_i;		// pulse index, not decided if i need it ################
  unsigned int max_pl;		// max pulses possible
  unsigned long nextFlip;	// ################...comment please...

  int16_t global_octave;		    // global octave shift. ONLY negative shifts atm
  unsigned long global_octave_mask;	    // this mask gets shifted and then actually used
  unsigned long current_global_octave_mask; // actually used mask to switch oscillators
  pulse * pulses;		// data pointer for pulses
};

/* **************************************************************** */
#endif
