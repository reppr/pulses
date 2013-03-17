/* **************************************************************** */
/*
   Pulses.h

   Arduino library to raise actions in harmonical time intervalls.

   Copyright © Robert Epprecht  www.RobertEpprecht.ch  GPLv2
   http://github.com/reppr/pulses

*/
/* **************************************************************** */


#ifndef PULSES_h
#define PULSES_h

#include <stdio.h>
#include <stdlib.h>

#ifdef ARDUINO
  #define STREAMTYPE	Stream
#else	// c++ Linux PC test version
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

// fakes for some Arduino functions:
void pinMode(int p, int mode) {
  printf("Pin %d switched to mode %d\n", p, mode);
}

void digitalWrite(int p, int value) {
  printf("Pin %d set to %d\n", p, value);
}

long micros() { return 9999L; }

#endif	// Arduino  else  c++ Linux PC test version


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
// pulse:

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
// time:

struct time {
  unsigned long time;
  unsigned int overflow;
};


/* **************************************************************** */
class Pulses {
 public:
  Pulses(unsigned int pl_max);
  ~Pulses();
  void pulses_init();		// ################ move to constructor
  void init_time();
  void get_now();		// *always* use get_now() to get current time!
  void add_time(struct time *delta, struct time *sum);
  void sub_time(struct time *delta, struct time *sum);
  void mul_time(struct time *duration, unsigned int factor);
  void div_time(struct time *duration, unsigned int divisor);
  void global_shift(int global_octave);
  int  start_pulse(int pindex);
  void stop_pulse(int pindex);
  bool HIGHorLOW(int pindex);
  unsigned long nextFlip;	// ################...COMMENT PLEASE...
  long updateNextFlip();
  int16_t global_octave;		    // global octave shift. ONLY negative shifts atm
  unsigned long global_octave_mask;	    // this mask gets shifted and then actually used
  unsigned long current_global_octave_mask; // actually used mask to switch oscillators
  struct time now;
  struct time last_now;		// for simple overflow detection
				// DO WE NEED OVERFLOW PART of that? ################
  struct time global_next;	// next time that a pulse wants to be waken up
  unsigned int global_next_count; // how many tasks wait to be activated at the same time?

 private:
  unsigned int pindex;		// pulse index, not decided if i need it ################
  unsigned int pl_max;		// max pulses possible

  pulse * pulses;		// data pointer for pulses
  int * global_next_pulses;	// pulses that want to be waken up at the same next time.
				// there can't be more then pl_max
};

/* **************************************************************** */
#endif
