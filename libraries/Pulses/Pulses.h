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
// time:

struct time {
  unsigned long time;
  unsigned int overflow;
};


/* **************************************************************** */
// pulse_t:

struct pulse_t {
  unsigned long counter;	// counts wake up's
  struct time period;		// period (including overflow count)
  struct time last;		// convenient, but not really needed
  struct time next;		// next time the pulse wants to wake up

  uint8_t flags;
	// ACTIVE		switches pulse on/off
	// COUNTED		repeats 'int1[]' times, then vanishes
	// SCRATCH		edit (or similar) in progress
	// DO_NOT_DELETE	dummy to avoid being thrown out
	// CUSTOM_1		can be used by periodic_do()
	// CUSTOM_2		can be used by periodic_do()
	// CUSTOM_3		can be used by periodic_do()

  // internal parameter:
  unsigned int int1;		// if COUNTED, gives number of executions
  //				// else free for other *internal* use

  // custom parameters[pulse]	//  comment/uncomment as appropriate:
  // ============>>> adapt init_pulse() IF YOU CHANGE SOMETHING HERE <<<============
  // these parameters can be used by periodic_do(pulse):
  int parameter_1;
  /*
    used by do_jiffle for count down
  */

  int parameter_2;
  /*
    used by do_jiffle *jiffletab
  */

  unsigned long ulong_parameter_1;
  /*
    used by do_jiffle as base period
  */

  char char_parameter_1;		// pin
  /*
    used by click      as pin
    used by do_jiffle  as pin
  */

  char char_parameter_2;		// index
  /*
    used by do_jiffle as jiffletab index
  */

  // pointers on  void something(unsigned int pulse)  functions:
  // the pulses will do that, if the pointer is not NULL
  void (*periodic_do)(int);
  //					some example functions:
  //
  //					click(pulse)
  //					pulse_info_1line(pulse)
  //					pulse_info(pulse)
  //					do_jiffle(pulse)
  //					do_throw_a_jiffle(pulse)
  // ============>>> adapt init_pulse() IF YOU CHANGE SOMETHING HERE <<<============
};

// #define pulses.flags masks:
#define ACTIVE			1	// switches pulse on/off
#define COUNTED			2	// repeats 'int1[]' times, then vanishes
#define SCRATCH			8	// edit (or similar) in progress
#define DO_NOT_DELETE	       16	// dummy to avoid being thrown out
#define CUSTOM_1	       32	// can be used by periodic_do()
#define CUSTOM_2	       64	// can be used by periodic_do()
#define CUSTOM_3	      128	// can be used by periodic_do()

//	// ################comment
//	#define pACTIVE	1
//	#define pMUTE	2


/* **************************************************************** */
class Pulses {
 public:
  Pulses(unsigned int pl_max);
  ~Pulses();
  void init_time();
  void get_now();		// *always* use get_now() to get current time!
  void add_time(struct time *delta, struct time *sum);
  void sub_time(struct time *delta, struct time *sum);
  void mul_time(struct time *duration, unsigned int factor);
  void div_time(struct time *duration, unsigned int divisor);
  void global_shift(int global_octave);
  int start_pulse(unsigned int pulse);
  void stop_pulse(unsigned int pulse);
  int global_octave;			    // global octave shift. ONLY negative shifts atm
  unsigned long global_octave_mask;	    // this mask gets shifted and then actually used
  unsigned long current_global_octave_mask; // actually used mask to switch oscillators
  struct time now;
  struct time last_now;		// for simple overflow detection
				// DO WE NEED OVERFLOW PART of that? ################
  struct time global_next;	// next time that a pulse wants to be waken up
  unsigned int global_next_count; // how many tasks wait to be activated at the same time?

  void init_pulse(unsigned int pulse);	// init, reset or kill a pulse
  void init_pulses();			// init all pulses
  void wake_pulse(unsigned int pulse);	// wake a pulse up, called from check_maybe_do()
  void fix_global_next();	// determine next event, prepare everything
				// for *all* pulses that wait for this exact time
				// they will be called in fast sequence then

 private:
  unsigned int pulse;		// pulse index
  unsigned int pl_max;		// max pulses possible

  pulse_t * pulses;		// data pointer for pulses
  unsigned int * global_next_pulses;	// indices of all pulses involved @ coming 'next'
					// pulses that want to be waken up at the same time.
					// (there can't be more then pl_max)
};

/* **************************************************************** */
#endif
