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

  #include <Pulses.h>
#else
  #include <iostream>

  #include <Pulses/Pulses.h>
#endif


/* **************************************************************** */
// time_unit that the user sees.
// it has no influence on inner working, but is a menu I/O thing only
// the user sees and edits times in time_units.
//
// I want time_unit to be dividable by a semi random selection of small integers
// avoiding rounding errors as much as possible.
//
// I consider factorials as a good choice:
// #define TIME_UNIT    40320L		// scaling timer to  8!, 0.040320s
// #define TIME_UNIT   362880L		// scaling timer to  9!, 0,362880s
#define TIME_UNIT	3628800L	// scaling timer to 10!, 3.628800s

/* **************************************************************** */
// Constructor/Destructor:

//	#ifndef ARDUINO		// WARNING: Using Stream MACRO hack when not on ARDUINO!
//	  #define Stream ostream
//	#endif

Pulses::Pulses(int pl_max, Menu *MENU):
  pl_max(pl_max),
  MENU(MENU),
  pulse(0),
  global_octave(0),
  global_octave_mask(1),
  current_global_octave_mask(1),
  global_next_count(0),
  selected_pulses(0),
  time_unit(TIME_UNIT),
  overflow_sec(4294.9672851562600)	// overflow time in seconds
#ifdef IMPLEMENT_TUNING
  , tuning(1.0)
#endif
{
  pulses = (pulse_t *) malloc(pl_max * sizeof(pulse_t));
  // ERROR ################

  global_next_pulses = (int*) malloc(pl_max * sizeof(int));
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
#if defined(ESP8266) || defined(ESP32)
  static unsigned long timer0_overflow_count=0;	// FIXME: hmm, really?
#else
  #if defined(ARDUINO)
    extern volatile unsigned long timer0_overflow_count;

    #ifdef __SAM3X8E__		// ################
      #warning 'cli() and sei() *not* on the DUE yet...	################'
      // cli();			// ################
      // timer0_overflow_count = 0;	// FIXME: timer overflow reset ################
      // sei();			// ################
    #else				// ################
      cli();
      timer0_overflow_count = 0;
      sei();
    #endif				// ################
  #else
    #warning 'init_time(); only on ARDUINO'
  #endif
#endif

  last_now.time = 0;		// make sure get_now() sees no overflow
  get_now();
  now.overflow = 0;		// start with now.overflow = 0

  last_now = now;		// correct overflow

  global_next.time=0;
  global_next.overflow=~0;	// ILLEGAL
}


// *always* get time through get_now()
struct time Pulses::get_now() {	// get time, set now.time and now.overflow
  now.time = micros();

  if (now.time < last_now.time)	// manage now.overflow
    now.overflow++;

  last_now = now;		// manage last_now

  return now;
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


// #define OLD_BROKEN_ESP8266_COMPATIBILITY  // switches bug back on, might influence old setups ;)
//
void Pulses::mul_time(struct time *duration, unsigned int factor)
 {
  unsigned long scratch;
  unsigned long result=0;
  unsigned long digit;
  unsigned int carry=0;
  unsigned int shift=8*sizeof(long)/2;
  unsigned long mask = 0;
  for (int i=0; i<shift; i++)
    mask |= 1 << i;

#ifdef OLD_BROKEN_ESP8266_COMPATIBILITY  // *with* bug that might influence setups ;)
  mask = (unsigned long) ((unsigned int) ~0);
  shift=16;
#endif

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
  unsigned int carry=0;
  unsigned int shift=8*sizeof(long)/2;
  unsigned long mask=0;
  for (int i=0; i<shift; i++)
    mask |= 1 << i;

#ifdef OLD_BROKEN_ESP8266_COMPATIBILITY  // *with* bug that might influence setups ;)
  mask = (unsigned long) ((unsigned int) ~0);
  shift=16;
#endif

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


void Pulses::multiply_period(int pulse, unsigned long factor) {	// integer math
  struct time new_period;

  new_period=pulses[pulse].period;
  mul_time(&new_period, factor);
  set_new_period(pulse, new_period);
}


void Pulses::divide_period(int pulse, unsigned long divisor) {	// integer math
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
  pulses[pulse].next.overflow = ~0;	// ILLEGAL
  pulses[pulse].parameter_1 = 0;
  pulses[pulse].parameter_2 = 0;
  pulses[pulse].ulong_parameter_1 = 0L;
  pulses[pulse].char_parameter_1 = 0;
  pulses[pulse].char_parameter_2 = 0;
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

#ifdef IMPLEMENT_TUNING
  if (pulses[pulse].flags & TUNED) {
    pulses[pulse].period.time = pulses[pulse].other_time.time / tuning;
    // period lengths with overflow are *not* supported with tuning
  }
#endif

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
  if ((pulses[pulse].flags & COUNTED) && \
      ((pulses[pulse].counter +1) == pulses[pulse].int1 )) {
    if (pulses[pulse].flags & DO_NOT_DELETE)	//   yes: DO_NOT_DELETE?
      pulses[pulse].flags &= ~ACTIVE;		//     yes: just deactivate
    else
      init_pulse(pulse);			//     no:  DELETE pulse
  }
}

void Pulses::deactivate_pulse(int pulse) {	// clear ACTIVE flag, keep data
  if (pulse == ILLEGAL)	// invalid?
    return;
  pulses[pulse].flags &= ~ACTIVE;

  fix_global_next();
}


void Pulses::deactivate_all_clicks() {
  for (int pulse=0; pulse<CLICK_PULSES; pulse++)
    pulses[pulse].flags &= ~ACTIVE;

  fix_global_next();
}


void Pulses::activate_selected_synced_now(int sync, unsigned long selected_pulses) {
  if (selected_pulses==0)
    return;

  get_now();
  for (int pulse=0; pulse<pl_max; pulse++)
    if (selected_pulses & (1 << pulse))
      activate_pulse_synced(pulse, now, abs(sync));

  fix_global_next();
  check_maybe_do();	  // maybe do it *first*
}


// menu interface to reset a pulse and prepare it to be edited:
void Pulses::reset_and_edit_pulse(int pulse, unsigned long time_unit) {
  init_pulse(pulse);
  pulses[pulse].flags |= SCRATCH;	// set SCRATCH flag
  pulses[pulse].flags &= ~ACTIVE;	// remove ACTIVE

  // set a default pulse length:
  struct time scratch;
  scratch.time = time_unit;
  scratch.overflow = 0;
  pulses[pulse].period = scratch;
}


#ifdef IMPLEMENT_TUNING		// implies floating point
void Pulses::activate_tuning(int pulse) {	// copy period to other_time and set TUNING flag
  pulses[pulse].flags |= TUNED;
  pulses[pulse].other_time.time = pulses[pulse].period.time;
  pulses[pulse].other_time.overflow = pulses[pulse].period.overflow;

  fix_global_next();	// looks like we do nod need this?
}


void Pulses::stop_tuning(int pulse) {	// the pulse stays as it is, but no further (de)tuning
  pulses[pulse].flags &= ~TUNED;	// clear TUNING flag
  // the base period from the tuning can stay in pulses[pulse].other_time, might be useful?
  // no harm, me thinks
}
#endif	// #ifdef IMPLEMENT_TUNING	implies floating point


// find fastest pulse (in case of emergency)
int Pulses::fastest_pulse() {	// *not* dealing with period overflow here...
  double min_period=0xefffffffffffffff;
  int fast_pulse=-1;

  for (int pulse=pl_max-1; pulse>-1; pulse--) {	// start with *highest* pulse index downwards
    if (pulses[pulse].flags & ACTIVE)
      if (pulses[pulse].flags & ACTIVE && pulses[pulse].period.time < min_period) {
	min_period = pulses[pulse].period.time;
	fast_pulse = pulse;
      }
  }
  return fast_pulse;
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

  for (int pulse=0; pulse<pl_max; pulse++) {		// check all pulses
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


// bool check_maybe_do();
// check if it's time to do something and do it if it's time
// calls wake_pulse(pulse); to do one life step of the pulse
// return true if something was done
// false if it's not time yet:
bool Pulses::check_maybe_do() {
  get_now();	// updates now

  if (global_next.overflow == now.overflow) {	// current overflow period?
    if (global_next.time <= now.time) {		//   yes: is it time?
      for (unsigned int i=0; i<global_next_count; i++)	//     yes:
	wake_pulse(global_next_pulses[i]);	//     wake next pulses up

      fix_global_next();			// determine next event[s] serie
      return true;		// *did* do something
    }
  } else					// (earlier or later overflow)
    if (global_next.overflow < now.overflow) {	// earlier overflow period?
      for (unsigned int i=0; i<global_next_count; i++)	//   yes, we're late...
	wake_pulse(global_next_pulses[i]);	//     wake next pulses up

      fix_global_next();			// determine next event[s] serie
      return true;		// *did* do something
    }

  return false;			// *no* not the right time yet
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
  if (pulses[pulse].period.time==0)	// ignore invalid pulses with period==0
    return;

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


// By design click pulses *HAVE* to be defined *BEFORE* any other pulses:
void Pulses::init_click_pulses() {
  for (int pulse=0; pulse<CLICK_PULSES; pulse++)
    init_pulse(pulse);
}


/* **************************************************************** */
// Menu output, info


void Pulses::display_now() {
  (*MENU).out(F("now  "));
  get_now();
  display_real_ovfl_and_sec(now);
}


void Pulses::time_info() {
  (*MENU).out(F("*** TIME info\t"));
  display_now();
  (*MENU).tab();
  (*MENU).out(F("next  "));
  display_real_ovfl_and_sec(global_next);
  (*MENU).ln();
}


// display a time in seconds:
float Pulses::display_realtime_sec(struct time duration) {
  float seconds=((float) ((unsigned long) duration.time) / 1000000.0);

  if (duration.overflow != ~0)		// ILLEGAL	FIXME: hmm? what about multiple negative overflows?
    seconds += overflow_sec * (float) duration.overflow;
  // seconds += overflow_sec * (float) ((signed long) duration.overflow);	// FIXME: overflow

  float scratch = 1000.0;
  float limit = abs(seconds);
  if (limit < (float) 1.0)	// (float) was once needed for Linux PC tests
    limit = 1.0;

  while (scratch > limit) {
    (*MENU).space();
    scratch /= 10.0;
  }

//  if (seconds >= 0)	// line up with automatic '-' sign
//    (*MENU).out('+');

  (*MENU).out(seconds , 6);
  (*MENU).out('s');

  return seconds;
}


void Pulses::print_period_in_time_units(int pulse) {
  float time_units = ((float) pulses[pulse].period.time / (float) time_unit);

  (*MENU).out(F("pulse "));

  float scratch = 1000.0;
  float limit = time_units;
  if (limit < (float) 1.0)	// (float) was once needed for Linux PC tests
    limit = 1.0;

  while (scratch > limit) {
    (*MENU).space();
    scratch /= 10.0;
  }

  (*MENU).out((float) time_units, 6);
  (*MENU).out(F(" time"));
}


void Pulses::display_real_ovfl_and_sec(struct time then) {
  (*MENU).out(F("tic/ofl "));
  (*MENU).out(then.time);
  (*MENU).slash();
  (*MENU).out((signed long) then.overflow);
  (*MENU).space();
  (*MENU).out('=');
  display_realtime_sec(then);
}


void Pulses::reset_and_edit_selected() {	// FIXME: replace
  for (int pulse=0; pulse<pl_max; pulse++)
    if (selected_pulses & (1 << pulse)) {
      reset_and_edit_pulse(pulse, time_unit);
    }
}


void Pulses::print_selected_mask() {
  int hex_pulses=pl_max;// displayed as hex chiffres
  if (hex_pulses > 16)
    hex_pulses=16;

  if((*MENU).is_chiffre((*MENU).cb_peek()))	// more numeric input, so no display yet...
    return;

  (*MENU).out_selected_();
  for (int pulse=0; pulse<hex_pulses; pulse++) {
    if (selected_pulses & (1 << pulse))
      (*MENU).out_hex_chiffre(pulse);
    else
      (*MENU).out('.');
  }

  // more than 16 pulses?
  if (hex_pulses == pl_max) {	// no,
    (*MENU).ln();
    return;			// done
  }

  (*MENU).space();
  for (int pulse=16; pulse<pl_max; pulse++) {
    if (selected_pulses & (1 << pulse))
      (*MENU).out('+');
    else
      (*MENU).out('.');
  }
  (*MENU).ln();
}


void Pulses::maybe_show_selected_mask() {
  if ((*MENU).maybe_display_more())
    print_selected_mask();
}


void Pulses::set_time_unit_and_inform(unsigned long new_value) {
  time_unit = new_value;
  (*MENU).out(F("Set time unit to "));
  (*MENU).out(time_unit);
  (*MENU).outln(F(" microseconds"));
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
