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
  time_unit(TIME_UNIT),
  hex_input_mask_index(0),
  overflow_sec(4294.9672851562600)	// overflow time in seconds
#ifdef IMPLEMENT_TUNING
  , tuning(1.0)
#endif
{
  pulses = (pulse_t*) malloc(pl_max * sizeof(pulse_t));
  // ERROR ################

  global_next_pulses = (int*) malloc(pl_max * sizeof(int));
  // ERROR ################

  selected_pulses_p = (pulses_mask_t*) malloc((selection_masks() * sizeof(pulses_mask_t)));	// size in bytes
  // ERROR ################
  clear_selection();

  init_time();
  init_pulses();
}

//	#ifndef ARDUINO		// WARNING: Using Stream MACRO hack when not on ARDUINO!
//	  #undef Stream
//	#endif


Pulses::~Pulses() {
  free(pulses);
  free(global_next_pulses);
  free(selected_pulses_p);
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

void Pulses::init_pulse(int pulse) {
  memset(&pulses[pulse], 0, sizeof(pulse_t));

  pulses[pulse].next.overflow = ~0;	// ILLEGAL
  pulses[pulse].gpio = ~0;		// ILLEGAL
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

  if (pulses[pulse].action_flags && !(pulses[pulse].action_flags & noACTION)) {	// unmuted action?
    int action_flags = pulses[pulse].action_flags;

    if (action_flags & CLICKs)
      digitalWrite(pulses[pulse].gpio, pulses[pulse].counter & 1);

#if defined USE_DACs
    if (action_flags & (DACsq1|DACsq2))
      DAC_output();
#endif

    if (action_flags & PAYLOAD)
      (*pulses[pulse].periodic_do)(pulse);	// do payload
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
      ((pulses[pulse].counter +1) == pulses[pulse].count )) {
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


void Pulses::put_payload(int pulse, void (*payload)(int)) { 	// set and activate payload
  pulses[pulse].periodic_do = payload;
  pulses[pulse].action_flags |= PAYLOAD;

  if (payload == NULL)
    pulses[pulse].action_flags &= ~PAYLOAD;	// clear payload bit if payload==NULL
}


// put_payload_with_pin(int pulse, void (*payload)(int), gpio_pin_t pin)  set and activate payload with gpio
void Pulses::put_payload_with_pin(int pulse, void (*payload)(int), gpio_pin_t pin) {
  put_payload(pulse, payload);
  set_gpio(pulse, pin);
}


void Pulses::set_gpio(int pulse, gpio_pin_t pin) {
  pulses[pulse].gpio = pin;

  //  if (pulses[pulse].gpio != ILLEGAL)	// TODO: yes or no?
    pulses[pulse].flags |= HAS_GPIO;
}


void Pulses::mute_all_actions() {
  for (int pulse=0; pulse<pl_max; pulse++)
    if(pulses[pulse].action_flags)
      pulses[pulse].action_flags |= noACTION;
}


void Pulses::show_action_flags(action_flags_t flags) {
  if(flags & DACsq1)
    (*MENU).out('D');
  else
    (*MENU).out('.');

  if(flags & DACsq2)
    (*MENU).out('D');
  else
    (*MENU).out('.');

  if(flags & CLICKs)
    (*MENU).out('C');
  else
    (*MENU).out('.');

  if(flags & PAYLOAD)
    (*MENU).out('P');
  else
    (*MENU).out('.');

  if(flags & noACTION)
    (*MENU).out('X');
  else
    (*MENU).out('!');
}



short Pulses::selection_masks(void) {
  short masks = pl_max;
  masks += (sizeof(pulses_mask_t) * 8) - 1 ;	// add room for any remaining bits in next mask
  masks /= sizeof(pulses_mask_t) * 8;		// how many masks needed?
  return masks;
}


// selection for user interface
bool Pulses::select_pulse(int pulse) {
  if ((pulse < pl_max) && (pulse > -1)) {
    short i = ( pulse / (sizeof(pulses_mask_t) * 8));	// mask index
    pulses_mask_t * mask = selected_pulses_p + i;	// mask pointer

    pulse %= sizeof(pulses_mask_t) * 8;
    *mask |= (pulses_mask_t) (1 << pulse);

    return true;
  }

  return false;
}


void Pulses::deselect_pulse(int pulse) {
  if (pulse < pl_max) {
    short i = (pulse / (sizeof(pulses_mask_t) * 8));	// mask index
    pulses_mask_t * mask = selected_pulses_p + i;	// mask pointer

    pulse %= sizeof(pulses_mask_t) * 8;
    *mask &= (pulses_mask_t) (1 << pulse);
  }
}


void Pulses::toggle_selection(int pulse) {
  if (pulse < pl_max) {
    short i = (pulse / (sizeof(pulses_mask_t) * 8));	// mask index
    pulses_mask_t * mask = selected_pulses_p + i;	// mask pointer

    pulse %= sizeof(pulses_mask_t) * 8;
    *mask ^= (pulses_mask_t) (1 << pulse);
  }
}


void Pulses::clear_selection() {
  int size = pl_max;
  size += 7;	// add room for remaining bits in next mask
  size /= 8;
  memset(selected_pulses_p, 0, size);
}


int Pulses::select_n(unsigned int n) {
  clear_selection();
  if (n) {
    if (n>pl_max)	// sanity check
      n=pl_max;

    for (int pulse=0; pulse<n; pulse++)
      select_pulse(pulse);
  }

  return n;
}


int Pulses::select_from_to(unsigned int from, unsigned int to) {
  if(from > to) {	// sanity checks: swap?
    unsigned int scratch = from;
    from = to;
    to = scratch;
  }
  if(from >= pl_max)		// insane?
    return 0;	// 0

  // ok:
  clear_selection();
  int n=0;
  for (pulse=from; pulse<=to && pulse<pl_max; pulse++) {
    select_pulse(pulse);
    n++;
  }

  return n;
}


bool Pulses::pulse_is_selected(int pulse, pulses_mask_t * mask) {
  if ((pulse < pl_max) && (pulse > -1)) {
    short i = (pulse / (sizeof(pulses_mask_t) * 8));	// mask index
    mask += i;						// mask pointer
    pulse %= sizeof(pulses_mask_t) * 8;			// pulse bit in mask

    return (*mask & (1 << pulse));
  }

 return false;
}


bool Pulses::pulse_is_selected(int pulse) {
  return pulse_is_selected(pulse, (pulses_mask_t *) selected_pulses_p);
}


short Pulses::how_many_selected() {
  short n=0;
  for (int pulse=0; pulse<pl_max; pulse++)
    if (pulse_is_selected(pulse))
      n++;

  return n;
}


void Pulses::activate_selected_synced_now(int sync) {
  get_now();
  for (int pulse=0; pulse<pl_max; pulse++)
    if (pulse_is_selected(pulse))
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

#if defined USE_DACs
//#define DEBUG_DACsq
void Pulses::DAC_output() {
  // for speed reasons i compile different versions for different numbers of DACs

  #if (USE_DACs > 2)	// too many DACs?  remind me if i use more then implemented ;)
    #error DAC_output():  only 2 DACs supported...
  #endif

  static int dac1_last=ILLEGAL;
  static int dac2_last=ILLEGAL;

  int dac1_value=0;
  int dac2_value=0;

  for(int p=0; p < pl_max; p++) {
    if (pulses[p].flags & ACTIVE) {
      if (pulses[p].counter & 1) {
	if (pulses[p].action_flags & DACsq1)
	  {
	    dac1_value += pulses[p].dac1_intensity;
#if defined DEBUG_DACsq
	    Serial.print("p:"); Serial.print(p); Serial.print(' '); Serial.print(dac1_value); Serial.print(' ');
#endif
	  }
    #if (USE_DACs > 1)	// only 2 DACs implemented
	if (pulses[p].action_flags & DACsq2)
	  dac2_value += pulses[p].dac2_intensity;
    #endif
      // TODO: use or remove code later
      /*
      if (pulses[p].dac1_wave_function != NULL) {
	dac1_value += pulses[p].dac1_wave_function(p, pulses[p].dac1_intensity);
      }
      #if (USE_DACs > 1)
        if (pulses[p].dac2_wave_function != NULL) {
	  dac2_value += pulses[p].dac2_wave_function(p, pulses[p].dac2_intensity);
        }
      #endif
      */
      } // counter & 1
    } // ACTIVE
  } // loop
#if defined DEBUG_DACsq
Serial.println();
#endif
  if(dac1_value != dac1_last) {
#if defined DEBUG_DACsq
//Serial.print(dac1_value); Serial.print(' ');
#endif
    dacWrite(BOARD_DAC1, dac1_value);
    dac1_last = dac1_value;
  }

  if(dac2_value != dac2_last) {
    dacWrite(BOARD_DAC2, dac2_value);
    dac2_last = dac2_value;
  }
}
#endif	// USE_DACs


// TODO: use or remove code later
//	#if (USE_DACs != 0)
//	 #if (USE_DACs == 1)
//	int Pulses::en_DAC(int pulse, int DACs_count /* must be 1 or 2 */,
//			   int (*wave_function1)(int pulse, int volume)) {
//	  pulses[pulse].dac1_wave_function = wave_function1;
//	  pulses[pulse].action_flags |= DACsq;
//	}
//
//	 #elif (USE_DACs == 2)
//	int Pulses::en_DAC(int pulse, int DACs_count /* must be 1 or 2 */,
//			   int (*wave_function1)(int pulse, int volume) ,
//			   int (*wave_function2)(int pulse, int volume)) {
//	  pulses[pulse].dac1_wave_function = wave_function1;
//	  pulses[pulse].dac2_wave_function = wave_function2;
//	  pulses[pulse].action_flags |= DACsq;
//	}
//	 #endif // allowed number of DACs
//
//
//	// Functions for  adcX_wave_function(int pulse)
//
//	// off
//	int Pulses::function_wave_OFF(int pulse, int volume) {	// ZERO
//	  return 0;
//	}
//
//	// square wave
//	int Pulses::function_square_wave(int pulse, int volume) {	// simple square
//	  if (pulses[pulse].counter & 1)
//	    return volume;
//	  else
//	    return 0;
//	}
//	#endif // (USE_DACs > 0)
//	#endif // USE_DACs


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


int Pulses::setup_pulse(void (*pulse_do)(int), pulse_flags_t new_flags, \
			struct time when, struct time new_period)
{
  int pulse;

  if (new_flags == 0)				// illegal new_flags parameter
    return ILLEGAL;				//   should not happen

  for (pulse=0; pulse<pl_max; pulse++) {	// search first free pulse
    if (pulses[pulse].flags == 0)		// flags==0 means empty pulse
      break;					//   found one
  }
  if (pulse == pl_max) {			// no pulse free :(
    return ILLEGAL;				// ERROR
  }

  // initiaize new pulse				// yes, found a free pulse
  pulses[pulse].flags = new_flags;			// initialize pulse
  put_payload(pulse, pulse_do);

  pulses[pulse].next.time = when.time;			// next wake up time
  pulses[pulse].next.overflow = when.overflow;
  pulses[pulse].period.time = new_period.time;
  pulses[pulse].period.overflow = new_period.overflow;

  // fix_global_next();	// this version does *not* automatically call that here...

  return pulse;
}


// unused?
int Pulses::setup_counted_pulse(void (*pulse_do)(int), pulse_flags_t new_flags, \
			struct time when, struct time new_period, unsigned int count)
{
  int pulse;

  pulse= setup_pulse(pulse_do, new_flags|COUNTED, when, new_period);
  pulses[pulse].count= count;

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
int Pulses::setup_pulse_synced(void (*pulse_do)(int), pulse_flags_t new_flags,
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

  struct time sum = global_next;
  struct time delta = now;
  sub_time(&delta, &sum);
  (*MENU).out(F("global next in"));
  display_realtime_sec(sum);
  (*MENU).ln();
}


// display a time in seconds:
float Pulses::display_realtime_sec(struct time duration) {
  float seconds=((float) ((unsigned long) duration.time) / 1000000.0);

  if (duration.overflow != 0)
    seconds += overflow_sec * (float) ((signed long) duration.overflow);

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

  // (*MENU).out(F("pulse "));

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


// mainly for debugging
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
    if (pulse_is_selected(pulse)) {
      reset_and_edit_pulse(pulse, time_unit);
    }
}


void Pulses::show_selected_mask() {
  int hex_pulses=pl_max;// displayed as hex chiffres
  if (hex_pulses > 16)
    hex_pulses=16;

  if((*MENU).is_chiffre((*MENU).cb_peek()))	// more numeric input, so no display yet...	FIXME: not here
    return;

  (*MENU).out_selected_();

  for (int pulse=0, i=0; pulse<pl_max;) {
    (*MENU).space();

    if (i == hex_input_mask_index)
      (*MENU).out('*');
    else
      (*MENU).space();

    (*MENU).out('[');
    (*MENU).out(i);
    (*MENU).out(F("] "));
    if (i < 10)
      (*MENU).space();

    int p;
    for (p=0; (p<hex_pulses) && ((pulse+p)<pl_max); p++) {
      if (pulse_is_selected(pulse + p))
	(*MENU).out_hex_chiffre(p);
      else
	(*MENU).out('.');
    }
    pulse += p;

    if (i == hex_input_mask_index)
      (*MENU).out('*');
    else
      (*MENU).space();
    i++;		// hex mask index

    if(i%4 == 0) {		//   start a new line and indent
      (*MENU).out(F("\n"));
      (*MENU).space(9);	//   (displaying a tab can differ from 8 spaces)
    }
  }

  (*MENU).ln();
}


void Pulses::maybe_show_selected_mask() {
  if ((*MENU).maybe_display_more())
    show_selected_mask();
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
