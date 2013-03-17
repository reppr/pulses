/* **************************************************************** */
/*
  Pulses.cpp

  Arduino library to raise actions in harmonical time intervalls.

    Copyright © Robert Epprecht  www.RobertEpprecht.ch  GPLv2

              http://github.com/reppr/pulses
*/
/* **************************************************************** */


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
  nextFlip(farest_future),
  global_next_count(0)
{
  pulses = (pulse*) malloc(pl_max * sizeof(pulse));
  // ERROR ################

  global_next_pulses = (int*) malloc(pl_max * sizeof(int));
  // ERROR ################

  init_time();
  pulses_init();
}

//	#ifndef ARDUINO		// WARNING: Using Stream MACRO hack when not on ARDUINO!
//	  #undef Stream
//	#endif


Pulses::~Pulses() {
  free(pulses);
}


/* **************************************************************** */
void Pulses::pulses_init() {	// called from constructor
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


/* **************************************************************** */
/*
     TIME


   all times get stored in 'struct time', taking care of time overflow
   *always* get time by calling get_now() to have overflow treated correctly.

   in this version init_time() is called from constructor.

   'running-through' program design:
   never wait,
   check with check_maybe_do() if it's time to do something
   else return immediately.

   pulses know when their next turn will be, see 'struct time next[pulse]'.
   fix_global_next() determines which event will be next globally
   fix_global_next() is updated automatically when appropriate.

*/
/* **************************************************************** */
// init time:

// do this once from setup()	################
void Pulses::init_time()
{
  extern volatile unsigned long timer0_overflow_count;

  cli();
  timer0_overflow_count = 0;
  sei();

  last_now.time = 0;		// make sure get_now() sees no overflow
  get_now();
  now.overflow = 0;		// start with now.overflow = 0

  last_now = now;		// correct overflow

  global_next.time=0;
  global_next.overflow=~0;	// ILLEGAL
}


// *always* get time through get_now()
void Pulses::get_now() {	// get time, set now.time and now.overflow
  now.time = micros();

  if (now.time < last_now.time)	// manage now.overflows
    now.overflow++;

  last_now = now;		// manage last_now
}


// add_time(), sub_time(), mul_time(), div_time():

void Pulses::add_time(struct time *delta, struct time *sum)
{
  unsigned long last=(*sum).time;

  (*sum).time += (*delta).time;
  (*sum).overflow += (*delta).overflow;
  if (last > (*sum).time)
    (*sum).overflow++;
}


// As time is unsigned we need a separate sub_time()
// to have access to the full unsigned value range.
void Pulses::sub_time(struct time *delta, struct time *sum)
{
  unsigned long last=(*sum).time;

  (*sum).time -= (*delta).time;
  (*sum).overflow -= (*delta).overflow;
  if (last < (*sum).time)
    (*sum).overflow--;
}


void Pulses::mul_time(struct time *duration, unsigned int factor)
 {
  unsigned long scratch;
  unsigned long result=0;
  unsigned long digit;
  unsigned int carry=0;
  unsigned long mask = (unsigned long) ((unsigned int) ~0);
  unsigned int shift=16;

  for (int i=0; i<2; i++) {
    scratch = (*duration).time & mask;
    (*duration).time >>= shift;

    scratch *= factor;
    scratch += carry;

    digit = scratch & mask;
    digit <<= (i * shift);
    result |= digit;

    carry = scratch >> shift;
  }

  (*duration).overflow *= factor;
  (*duration).overflow += carry;

  (*duration).time=result;
}


void Pulses::div_time(struct time *duration, unsigned int divisor)
{
  unsigned long scratch;
  unsigned long result=0;
  unsigned long digit;
  unsigned int carry=0;
  unsigned long mask = (unsigned long) ((unsigned int) ~0);
  unsigned int shift=16;

  scratch=(*duration).overflow;
  carry=(*duration).overflow % divisor;
  (*duration).overflow /= divisor;

  for (int i=0; i<2; i++) {
    scratch = carry;						// high digit
    scratch <<= shift;						// high digit
    scratch |= ((*duration).time >> ((1-i)*shift)) & mask;	// low digit

    carry = scratch % divisor;
    scratch /= divisor;
    result <<=shift;
    result |= (scratch & mask);
  }

  (*duration).time=result;
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
/*
README_pulses

Pulses

  Arduino library to raise actions in harmonical time intervalls.
  Periodically do multiple independent tasks named 'pulses'.


  Copyright © Robert Epprecht  www.RobertEpprecht.ch  GPLv2

  http://github.com/reppr/pulses

*/
/* **************************************************************** */
