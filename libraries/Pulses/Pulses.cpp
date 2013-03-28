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
  pulse(0),
  global_octave(0),
  global_octave_mask(1),
  current_global_octave_mask(1),
//  nextFlip(farest_future),	// ################ ???
  global_next_count(0)
{
  pulses = (pulse_t *) malloc(pl_max * sizeof(pulse_t));
  // ERROR ################

  global_next_pulses = (unsigned int*) malloc(pl_max * sizeof(int));
  // ERROR ################

  init_time();
  init_pulses();
}

//	#ifndef ARDUINO		// WARNING: Using Stream MACRO hack when not on ARDUINO!
//	  #undef Stream
//	#endif


Pulses::~Pulses() {
  free(pulses);
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
void Pulses::init_time() {
#ifdef ARDUINO
  extern volatile unsigned long timer0_overflow_count;

#ifdef __SAM3X8E__		// ################
  #warning 'cli() and sei() *not* on the DUE yet...	################'
  // cli();			// ################
  timer0_overflow_count = 0;	// ################ unknown!
  // sei();			// ################
#else				// ################
  cli();
  timer0_overflow_count = 0;
  sei();
#endif				// ################

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


void Pulses::multiply_period(int pulse, unsigned long factor) {
  struct time new_period;

  new_period=pulses[pulse].period;
  mul_time(&new_period, factor);
  set_new_period(pulse, new_period);
}


void Pulses::divide_period(int pulse, unsigned long divisor) {
  struct time new_period;

  new_period=pulses[pulse].period;
  div_time(&new_period, divisor);
  set_new_period(pulse, new_period);
}


// FIXME: comment	################
void Pulses::global_shift(int global_octave) {
  if (global_octave>0)
    current_global_octave_mask = global_octave_mask >> global_octave;
  else
    current_global_octave_mask = global_octave_mask << -global_octave;
}


/* **************************************************************** */
/*
   PULSES

	periodically wake up, count and maybe do something

*/
/* **************************************************************** */

/* **************************************************************** */
// init pulses:

// init, reset or kill a pulse:	// fill with zero??? ################
// memset(&arr[0], 0, sizeof(arr)); ################
void Pulses::init_pulse(int pulse) {
  pulses[pulse].flags = 0;
  pulses[pulse].periodic_do = NULL;
  pulses[pulse].counter = 0;
  pulses[pulse].int1 = 0;
  pulses[pulse].period.time = 0;
  pulses[pulse].last.time = 0;
  pulses[pulse].last.overflow = 0;
  pulses[pulse].next.time = 0;
  pulses[pulse].next.overflow = 0;
  pulses[pulse].parameter_1 = 0;
  pulses[pulse].parameter_2 = 0;
  pulses[pulse].ulong_parameter_1 = 0L;
  pulses[pulse].char_parameter_1 = 0;
  pulses[pulse].char_parameter_2 = 0;

  // you *must* call fix_global_next(); late in setup()
}

// called from constructor:
void Pulses::init_pulses() {
  for (int pulse=0; pulse<pl_max; pulse++) {
    init_pulse(pulse);
  }
}


// void wake_pulse(int pulse);	do one life step of the pulse
// gets called from check_maybe_do()
void Pulses::wake_pulse(int pulse) {
  pulses[pulse].counter++;			//      count

  if (pulses[pulse].periodic_do != NULL) {	// there *is* something else to do?
    (*pulses[pulse].periodic_do)(pulse);	//   yes: do it
  }

  // prepare future:
  pulses[pulse].last.time = pulses[pulse].next.time;	// when it *should* have happened
  pulses[pulse].last.overflow = pulses[pulse].next.overflow;
  pulses[pulse].next.time += pulses[pulse].period.time;	// when it should happen again
  pulses[pulse].next.overflow += pulses[pulse].period.overflow;

  if (pulses[pulse].last.time > pulses[pulse].next.time)
    pulses[pulse].next.overflow++;

  //						// COUNTED pulse && end reached?
  if ((pulses[pulse].flags & COUNTED) && ((pulses[pulse].counter +1) == pulses[pulse].int1 ))
    if (pulses[pulse].flags & DO_NOT_DELETE)	//   yes: DO_NOT_DELETE?
      pulses[pulse].flags &= ~ACTIVE;		//     yes: just deactivate
    else
      init_pulse(pulse);			//     no:  DELETE pulse

}



// void fix_global_next();
// determine when the next event[s] will happen:
//
// to start properly you *must* call this once, late in setup()
// will automagically get updated later on
void Pulses::fix_global_next() {
/* This version calculates global next event[s] *once* when the next
   event could possibly have changed.

   If a series of pulses are sceduled for the same time they will
   be called in fast sequence *without* fix_global_next() in between them.
   After all pulse with the same time are done fix_global_next() is called.

   If a pulse sets up another pulse with the same next it *can* decide to do
   the fix itself, if it seems appropriate. Normally this is not required.
*/

  // we work directly on the global variables here:
  global_next.overflow=~0;	// ILLEGAL value
  global_next_count=0;

  for (pulse=0; pulse<pl_max; pulse++) {		// check all pulses
    if (pulses[pulse].flags & ACTIVE) {				// pulse active?
      if (pulses[pulse].next.overflow < global_next.overflow) {	// yes: earlier overflow?
	global_next.time = pulses[pulse].next.time;		//   yes: reset search
	global_next.overflow = pulses[pulse].next.overflow;
	global_next_pulses[0] = pulse;
	global_next_count = 1;
      } else {					// same (or later) overflow:
	if (pulses[pulse].next.overflow == global_next.overflow) {	// same overflow?
	  if (pulses[pulse].next.time < global_next.time) {		//   yes: new next?
	    global_next.time = pulses[pulse].next.time;		//     yes: reset search
	    global_next_pulses[0] = pulse;
	    global_next_count = 1;
	  } else					// (still *same* overflow)
	    if (pulses[pulse].next.time == global_next.time)		//    *same* next?
	      global_next_pulses[global_next_count++]=pulse; //  yes: save pulse, count
	}
	// (*later* overflows are always later)
      }
    } // active?
  } // pulse loop
}


// void check_maybe_do();
// check if it's time to do something and do it if it's time
// calls wake_pulse(pulse); to do one life step of the pulse
void Pulses::check_maybe_do() {
  get_now();	// updates now

  if (global_next.overflow == now.overflow) {	// current overflow period?
    if (global_next.time <= now.time) {		//   yes: is it time?
      for (int i=0; i<global_next_count; i++)	//     yes:
	wake_pulse(global_next_pulses[i]);	//     wake next pulses up

      fix_global_next();			// determine next event[s] serie
    }
  } else					// (earlier or later overflow)
    if (global_next.overflow < now.overflow) {	// earlier overflow period?
      for (int i=0; i<global_next_count; i++)	//     yes, we're late...
	wake_pulse(global_next_pulses[i]);	//     wake next pulses up

      fix_global_next();			// determine next event[s] serie
    }
}


int Pulses::setup_pulse(void (*pulse_do)(int), unsigned char new_flags, \
			struct time when, struct time new_period)
{
  int pulse;

  if (new_flags == 0)				// illegal new_flags parameter
    return ILLEGAL;				//   should not happen

  for (pulse=0; pulse<pl_max; pulse++) {	// search first free pulse
    if (pulses[pulse].flags == 0)			// flags==0 means empty pulse
      break;					//   found one
  }
  if (pulse == pl_max) {			// no pulse free :(
    return ILLEGAL;			// ERROR
  }

  // initiaize new pulse				// yes, found a free pulse
  pulses[pulse].flags = new_flags;			// initialize pulse
  pulses[pulse].periodic_do = pulse_do;			// payload
  pulses[pulse].next.time = when.time;			// next wake up time
  pulses[pulse].next.overflow = when.overflow;
  pulses[pulse].period.time = new_period.time;
  pulses[pulse].period.overflow = new_period.overflow;

  // fix_global_next();	// this version does *not* automatically call that here...

  return pulse;
}


// unused?
int Pulses::setup_counted_pulse(void (*pulse_do)(int), unsigned char new_flags, \
			struct time when, struct time new_period, unsigned int count)
{
  int pulse;

  pulse= setup_pulse(pulse_do, new_flags|COUNTED, when, new_period);
  pulses[pulse].int1= count;

  return pulse;
}


void Pulses::set_new_period(int pulse, struct time new_period) {
  pulses[pulse].period.time = new_period.time;
  pulses[pulse].period.overflow = new_period.overflow;

  pulses[pulse].next.time = pulses[pulse].last.time + pulses[pulse].period.time;
  pulses[pulse].next.overflow = pulses[pulse].last.overflow + pulses[pulse].period.overflow;
  if(pulses[pulse].next.time < pulses[pulse].last.time)
    pulses[pulse].next.overflow++;

  fix_global_next();	// it's saver to do that from here, but could be omitted.
}


/* **************************************************************** */
// creating, editing, killing pulses


/* void activate_pulse_synced(pulse, when, sync)
   (re-) activate pulse that has been edited or modified at a given time:
   (assumes everything else is set up in order)
   can also be used to sync running pulses on a given time		*/
// currently only used in menu
void Pulses::activate_pulse_synced(int pulse, \
				   struct time when, int sync)
{
  if (sync) {
    struct time delta = pulses[pulse].period;
    mul_time(&delta, sync);
    div_time(&delta, 2);
    add_time(&delta, &when);
  }

  pulses[pulse].last = when;	// replace possibly random last with something a bit better
  pulses[pulse].next = when;

  // now switch it on
  pulses[pulse].flags |= ACTIVE;	// set ACTIVE
  pulses[pulse].flags &= ~SCRATCH;	// clear SCRATCH
}


/* **************************************************************** */
// playing with rhythms:


// Generic setup pulse, stright or middle synced relative to 'when'.
// Pulse time and phase sync get deviated from unit, which is first
// multiplied by factor and divided by divisor.
// sync=0 gives stright syncing, sync=1 middle pulses synced.
// other values possible,
// negative values should not reach into the past
// (that's why the menu only allows positive. it syncs now related,
//  so a negative sync value would always reach into past.)
int Pulses::setup_pulse_synced(void (*pulse_do)(int), unsigned char new_flags,
		       struct time when, unsigned long unit,
		       unsigned long factor, unsigned long divisor, int sync)
{
  struct time new_period;

  if (sync) {
    struct time delta;
    delta.time = sync*unit/2L;
    delta.overflow = 0;

    mul_time(&delta, factor);
    div_time(&delta, divisor);

    add_time(&delta, &when);
  }

  new_period.time = unit;
  new_period.overflow = 0;
  mul_time(&new_period, factor);
  div_time(&new_period, divisor);

  return setup_pulse(pulse_do, new_flags, when, new_period);
}


/* **************************************************************** */


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
