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
  do_A2(NULL),
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


   all times get stored in 'pulse_time_t', taking care of time overflow
   *always* get time by calling get_now() to have overflow treated correctly.

   in this version init_time() is called from constructor.

   'running-through' program design:
   never wait,
   check with check_maybe_do() if it's time to do something
   else return immediately.

   pulses know when their next turn will be, see 'pulse_time_t next[pulse]'.
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
  global_next.overflow=ILLEGAL32;
}


// *always* get time through get_now()
pulse_time_t Pulses::get_now() {	// get time, set now.time and now.overflow
  now.time = micros();

  if (now.time < last_now.time)	// manage now.overflow
    now.overflow++;

  last_now = now;		// manage last_now

  return now;
}


// add_time(), sub_time(), mul_time(), div_time():

void Pulses::add_time(pulse_time_t *delta, pulse_time_t *sum)
{
  unsigned long last=(*sum).time;

  (*sum).time += (*delta).time;
  (*sum).overflow += (*delta).overflow;
  if (last > (*sum).time)
    (*sum).overflow++;
}


// As time is unsigned we need a separate sub_time()
// to have access to the full unsigned value range.
void Pulses::sub_time(pulse_time_t *delta, pulse_time_t *sum)
{
  unsigned long last=(*sum).time;

  (*sum).time -= (*delta).time;
  (*sum).overflow -= (*delta).overflow;
  if (last < (*sum).time)
    (*sum).overflow--;
}


void Pulses::add_time(unsigned long time, pulse_time_t *sum)
{
  unsigned long last=(*sum).time;

  (*sum).time += time;
  if (last > (*sum).time)
    (*sum).overflow++;
}


// As time is unsigned we need a separate sub_time()
// to have access to the full unsigned value range.
void Pulses::sub_time(unsigned long time, pulse_time_t *sum)
{
  unsigned long last=(*sum).time;

  (*sum).time -= time;
  if (last < (*sum).time)
    (*sum).overflow--;
}


// #define OLD_BROKEN_ESP8266_COMPATIBILITY  // switches bug back on, might influence old setups ;)
//
void Pulses::mul_time(pulse_time_t *duration, unsigned int factor)
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


void Pulses::div_time(pulse_time_t *duration, unsigned int divisor)
{
  if(divisor==0)
    return;

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
  pulse_time_t new_period;

  new_period=pulses[pulse].period;
  mul_time(&new_period, factor);
  set_new_period(pulse, new_period);
}


void Pulses::divide_period(int pulse, unsigned long divisor) {	// integer math
  pulse_time_t new_period;

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

  pulses[pulse].next.overflow = ILLEGAL32;
  pulses[pulse].gpio = ILLEGAL8;
}

// called from constructor:
void Pulses::init_pulses() {
  for (int pulse=0; pulse<pl_max; pulse++) {
    init_pulse(pulse);
  }
}

int Pulses::highest_available_pulse() {
  for (int pulse=pl_max-1; pulse>=0; pulse--)
    if(pulses[pulse].flags == 0)
      return pulse;			// return topmost free pulse

  return ILLEGAL32;
}

// void wake_pulse(int pulse);	do one life step of the pulse
// gets called from check_maybe_do()
void Pulses::wake_pulse(int pulse) {
  if ((pulses[pulse].action_flags & DO_first) && !(pulses[pulse].action_flags & noACTION)) {	// unmuted DO_first action?
    (*pulses[pulse].do_first)(pulse);		// do do_first action
  }

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

    if (action_flags & PAYLOAD) {		// for flexibility payload comes *before* icode
      (*pulses[pulse].payload)(pulse);		// do payload
      action_flags = pulses[pulse].action_flags;	// re-read (pulse might be changed)
    }

    if (action_flags & doesICODE) {		// play icode   (payload already done)
      play_icode(pulse);
      if ((pulses[pulse].period.time == 0) && (pulses[pulse].period.overflow == 0))	// pulse got killed?
	return;
    }
  }

  // prepare future:
  pulses[pulse].last.time = pulses[pulse].next.time;	// when it *should* have happened
  pulses[pulse].last.overflow = pulses[pulse].next.overflow;
  pulses[pulse].next.time += pulses[pulse].period.time;	// when it should happen again
  pulses[pulse].next.overflow += pulses[pulse].period.overflow;

  if (pulses[pulse].last.time > pulses[pulse].next.time)
    pulses[pulse].next.overflow++;

  if (pulses[pulse].flags & COUNTED) {	// COUNTED pulse && end reached?
    if(--pulses[pulse].remaining < 1) {		// END REACHED with 0,	(below just as savety net ;)
      if (pulses[pulse].flags & DO_NOT_DELETE)	//   yes: DO_NOT_DELETE?
	pulses[pulse].flags &= ~ACTIVE;		//     yes: just deactivate
      else
	init_pulse(pulse);			//     no:  DELETE pulse
    }
  }
}


void Pulses::deactivate_pulse(int pulse) {	// clear ACTIVE flag, keep data
  if (pulse == ILLEGAL32)	// invalid?
    return;
  pulses[pulse].flags &= ~ACTIVE;

  fix_global_next();
}


void Pulses::set_do_first(int pulse, void (*do_first)(int)) {	// set and activate do_first
  pulses[pulse].do_first = do_first;
  pulses[pulse].action_flags |= DO_first;

  if (do_first == NULL)
    pulses[pulse].action_flags &= ~DO_first;	// clear DO_first bit if do_first==NULL
}


void Pulses::set_payload(int pulse, void (*payload)(int)) {	// set and activate payload
  pulses[pulse].payload = payload;
  pulses[pulse].action_flags |= PAYLOAD;

  if (payload == NULL)
    pulses[pulse].action_flags &= ~PAYLOAD;	// clear payload bit if payload==NULL
}


// set_payload_with_pin(int pulse, void (*payload)(int), gpio_pin_t pin)  set and activate payload with gpio
void Pulses::set_payload_with_pin(int pulse, void (*payload)(int), gpio_pin_t pin) {
  set_payload(pulse, payload);
  set_gpio(pulse, pin);
}


void Pulses::set_gpio(int pulse, gpio_pin_t pin) {
  pulses[pulse].gpio = pin;
  pulses[pulse].flags |= HAS_GPIO;
}


void Pulses::set_icode_p(int pulse, icode_t* icode_p, bool activate=false) {
  pulses[pulse].icode_p = icode_p;
  pulses[pulse].icode_index=0;		// resets index
  pulses[pulse].countdown=0;		// resets countdown
  pulses[pulse].flags |= HAS_ICODE;
  if (activate)
    pulses[pulse].action_flags |= doesICODE;
}


void Pulses::set_rgb_led_string(int pulse, uint8_t string_idx, uint8_t pixel) {
  pulses[pulse].rgb_string_idx = string_idx;
  pulses[pulse].rgb_pixel_idx = pixel;
  pulses[pulse].flags |= HAS_RGB_LEDs;
}


void Pulses::remove_rgb_led_string(int pulse) {
  pulses[pulse].flags &= ~HAS_RGB_LEDs;
  pulses[pulse].rgb_string_idx = 0;
  pulses[pulse].rgb_pixel_idx = 0;
}


#if defined USE_MCP23017
void Pulses::set_i2c_addr_pin(int pulse, uint8_t i2c_addr, uint8_t i2c_pin) {
  pulses[pulse].i2c_addr = i2c_addr;
  pulses[pulse].i2c_pin = i2c_pin;
  pulses[pulse].flags |= HAS_I2C_ADDR_PIN;
}
#endif


void Pulses::selected_toggle_no_actions() {	// selected toggle action flag noACTION  // TODO: do I want this?
  for (int pulse=0; pulse<pl_max; pulse++)
    if (pulse_is_selected(pulse))
      if(pulses[pulse].action_flags)
	pulses[pulse].action_flags ^= noACTION;
}


void Pulses::selected_set_no_actions() {	// selected set action flag noACTION  // TODO: do I want this?
  for (int pulse=0; pulse<pl_max; pulse++)
    if (pulse_is_selected(pulse))
      if(pulses[pulse].action_flags)
	pulses[pulse].action_flags |= noACTION;
}


void Pulses::selected_clear_no_actions() {	// selected clear action flag noACTION  // TODO: do I want this?
  for (int pulse=0; pulse<pl_max; pulse++)
    if (pulse_is_selected(pulse))
      if(pulses[pulse].action_flags)
	pulses[pulse].action_flags &= ~noACTION;
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
    (*MENU).out('G');
  else
    (*MENU).out('.');

  if(flags & PAYLOAD)
    (*MENU).out('P');
  else
    (*MENU).out('.');

  if(flags & doesICODE)
    (*MENU).out('C');
  else
    (*MENU).out('.');

  if(flags & DO_first)
    (*MENU).out('1');
  else
    (*MENU).out('.');

  if(flags & DO_after)
    (*MENU).out('A');
  else
    (*MENU).out('.');

  if(flags & noACTION)
    (*MENU).out('X');
  else
    (*MENU).space();
  (*MENU).space();
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


unsigned int Pulses::deselect_zombie_primaries() {
  unsigned int deselected=0;
  for (int pulse=0; pulse<pl_max; pulse++) {
    if (pulse_is_selected(pulse))
      if(pulses[pulse].flags==0 && pulses[pulse].groups & g_PRIMARY) {
	deselect_pulse(pulse);		// deselect unused primary pulses
	deselected++;
      }
  }
  return deselected;
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


void Pulses::activate_selected_stack_sync_now(pulse_time_t tick /*sign inverts direction*/, int sync) {
  pulse_time_t next_now = get_now();

  if(tick.overflow >= 0) {	// positive tick: ascending pulse order
    for (int pulse=0; pulse<pl_max; pulse++)
      if (pulse_is_selected(pulse)) {
	activate_pulse_synced(pulse, next_now, abs(sync));
	add_time(&tick, &next_now);
      }
  } else {	// negativetive tick: descending pulse order
    tick.overflow = 0;	// make it positive, disregarding overflow
    for (int pulse=(pl_max-1); pulse>=0; pulse--)
      if (pulse_is_selected(pulse)) {
	activate_pulse_synced(pulse, next_now, abs(sync));
	add_time(&tick, &next_now);
      }
  }

  fix_global_next();
  check_maybe_do();	  // maybe do it *first*	TODO: RETHINK:
}


void Pulses::time_skip_selected(pulse_time_t time_skip) {	// positive time intervall only
  for (int pulse=0; pulse<pl_max; pulse++)
    if (pulse_is_selected(pulse))
      sub_time(&time_skip, &pulses[pulse].next);

  fix_global_next();
  //  check_maybe_do();	  // maybe do it *first*	// TODO: maybe not...
}

// menu interface to reset a pulse and prepare it to be edited:
void Pulses::reset_and_edit_pulse(int pulse, unsigned long time_unit) {
  init_pulse(pulse);
  pulses[pulse].flags |= SCRATCH;	// set SCRATCH flag
  pulses[pulse].flags &= ~ACTIVE;	// remove ACTIVE

  // set a default pulse length:
  pulse_time_t scratch;
  scratch.time = time_unit;
  scratch.overflow = 0;
  pulses[pulse].period = scratch;
}


#ifdef IMPLEMENT_TUNING		// implies floating point
void Pulses::activate_tuning(int pulse) {	// copy period to other_time and set TUNING flag
  pulses[pulse].flags |= TUNED;
  pulses[pulse].other_time.time = pulses[pulse].period.time;
  pulses[pulse].other_time.overflow = pulses[pulse].period.overflow;

  fix_global_next();	// looks like we do not need this?
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


int Pulses::fastest_from_selected() {	// no overflow here...
  double min_period=0;
  int fast_pulse=-1;

  for (int pulse=pl_max-1; pulse>-1; pulse--) {	// start with *highest* pulse index downwards
    if(pulse_is_selected(pulse)) {		// SELECTED IS ENOUGH, *no need to be ACTIVE*
      if(min_period) {	// not first date
	if(pulses[pulse].period.time < min_period) {
	  min_period = pulses[pulse].period.time;
	  fast_pulse = pulse;
	}
      } else {		// first one
	min_period = pulses[pulse].period.time;
	fast_pulse = pulse;
      }
    }
  }

  return fast_pulse;
}

int Pulses::add_selected_to_group(group_flags_t groups_to_set) {	// add selected pulses to groups
  int set=0;
  for (int pulse=0; pulse<pl_max; pulse++)
    if (pulse_is_selected(pulse)) {
      pulses[pulse].groups |= groups_to_set;
      set++;
    }

  return set;
}

int Pulses::remove_selected_from_group(group_flags_t groups_to_remove) { // remove selected pulses from groups
  int cleared=0;

  for (int pulse=0; pulse<pl_max; pulse++)
    if (pulse_is_selected(pulse)) {
      pulses[pulse].groups &= ~groups_to_remove;
      cleared++;
    }

  return cleared;
}

void Pulses::show_group_mnemonics(int pulse) {
  char * mnemonics = "012345678LMH";
  char c;

  int i=0;
  while (c = *mnemonics) {
    if(pulses[pulse].groups & (1 << i++))
      (*MENU).out(c);
    mnemonics++;
  }

  (*MENU).space();
  (*MENU).tab();	// works only as long as i do not use too many groups
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
  global_next.overflow=ILLEGAL32;
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


bool Pulses::time_reached(pulse_time_t time_limit) {
  get_now();
  if(now.overflow == time_limit.overflow)
    return (now.time >= time_limit.time);
  else
    return (now.overflow > time_limit.overflow);
}


#if defined USE_DACs
//#define DEBUG_DACsq	// TODO: remove debugging code
void Pulses::DAC_output() {
  // for speed reasons i compile different versions for different numbers of DACs

  #if (USE_DACs > 2)	// too many DACs?  remind me if i use more then implemented ;)
    #error DAC_output():  only 2 DACs supported...
  #endif

  static int dac1_last=ILLEGAL32;
  static int dac2_last=ILLEGAL32;

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
			pulse_time_t when, pulse_time_t new_period)
{
//  (*MENU).outln("PULSES.setup_pulse");

  int pulse;

  if (new_flags == 0)				// illegal new_flags parameter
    return ILLEGAL32;				//   should not happen

  for (pulse=0; pulse<pl_max; pulse++) {	// search first free pulse
    if (pulses[pulse].flags == 0)		// flags==0 means empty pulse
      break;					//   found one
  }
  if (pulse == pl_max) {			// no pulse free :(
    return ILLEGAL32;				// ERROR
  }

  // initiaize new pulse				// yes, found a free pulse
  pulses[pulse].flags = new_flags;			// initialize pulse
  set_payload(pulse, pulse_do);

  pulses[pulse].next.time = when.time;			// next wake up time
  pulses[pulse].next.overflow = when.overflow;
  pulses[pulse].period.time = new_period.time;
  pulses[pulse].period.overflow = new_period.overflow;

  // fix_global_next();	// this version does *not* automatically call that here...

  return pulse;
}


int Pulses::setup_pulse(void (*pulse_do)(int), pulse_flags_t new_flags,	\
			pulse_time_t when, unsigned long new_period) {	// unsigned long *only* version
  pulse_time_t period = {new_period, 0};	// no overflow in period
  setup_pulse(pulse_do, new_flags, when, period);
}


void Pulses::set_new_period(int pulse, pulse_time_t new_period) {
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
				   pulse_time_t when, int sync)
{
  if (pulses[pulse].period.time==0)	// ignore invalid pulses with period==0
    return;

  if (sync) {
    pulse_time_t delta = pulses[pulse].period;
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


void Pulses::show_pulse_flag_mnemonics(pulse_flags_t flags) {	// show pulse flags as mnemonics
  char* pulses_ON_mnemonics  =	"!NGsdTIi";
  char* pulses_OFF_mnemonics =	"........";
  (*MENU).show_flag_mnemonics(flags, sizeof(pulse_flags_t)*8, pulses_ON_mnemonics, pulses_OFF_mnemonics);
}

/* **************************************************************** */
/* int icode player	*/

// code, [data1, ...]

void Pulses::show_icode_mnemonic(icode_t icode) {
  switch(icode) {
  case KILL:
    (*MENU).out(F("KILL"));
    break;
  case i2cW:
    (*MENU).out(F("i2cW"));
    break;
  case doA2:
    (*MENU).out(F("doA2"));
    break;
  case WAIT:
    (*MENU).out(F("WAIT"));
    break;
  case INFO:
    (*MENU).out(F("INFO"));
    break;
  case DONE:
    (*MENU).out(F("DONE"));
    break;
  default:
    (*MENU).out(icode);
  }
}


//#define DEBUG_ICODE	// TODO: remove debugging icode
void do_jiffle(int pulse);
void pulse_info_1line(int pulse);
void Pulses::play_icode(int pulse) {	// can be called by pulse_do
  /*
    pulses[pulse].icode_p	icode start pointer
    pulses[pulse].icode_index	icode index
    pulses[pulse].countdown	count down
    pulses[pulse].base_time	base period = period of starting pulse
    pulses[pulse].gpio		maybe click pin
  */

#if defined DEBUG_ICODE
      (*MENU).out(F("\niCODE\tpulse "));
      (*MENU).out(pulse);
      (*MENU).out(F("\t#"));
      (*MENU).out(pulses[pulse].counter);
      (*MENU).out(F("\t<****************"));
#endif
  if(!(pulses[pulse].flags & HAS_ICODE))
    return;				// error: pulse has no icode

  icode_t* icode_p = pulses[pulse].icode_p;
  if(icode_p == NULL)
    return;				// error: icode pointer is NULL

  // ok icode_p looks ok
  icode_p += pulses[pulse].icode_index;	// continue where we left off

  icode_t icode;
  long multiplier=0;
  long divisor=0;
  long counter=0;

  bool busy=true;
  while (busy) {
    icode = *icode_p;
    if (icode < 0 ) {	// ICODE or JIFF?
      icode_p++;
#if defined DEBUG_ICODE
      (*MENU).out(F("\nplay_icode "));
      show_icode_mnemonic(icode);
      (*MENU).ln();
#endif

      switch (icode) {
      case WAIT:
	multiplier = *icode_p++;
	divisor = *icode_p++;
	pulses[pulse].period.time = pulses[pulse].base_time * multiplier / divisor;
	// busy = false;
	break;

      case KILL:
	if(pulses[pulse].flags & HAS_GPIO)		// TODO: reset gpio here or not, see: (multiplier == 0 || divisor==0)
	  if(pulses[pulse].gpio != ILLEGAL8) {		// currently the gpios stay high after a jiff without
	    digitalWrite(pulses[pulse].gpio, 0);	// so yes, set gpio LOW
#if defined DEBUG_ICODE
	    (*MENU).out("reset GPIO ");
	    (*MENU).outln(pulses[pulse].gpio);
#endif
	  }

	init_pulse(pulse);
	icode_p = pulses[pulse].icode_p;	// programmers aesthetics...
	busy = false;
	break;

#if defined USE_MCP23017
      case i2cW:
	{
	  int value = *icode_p++;
	  if(pulses[pulse].flags & HAS_I2C_ADDR_PIN) {
	    if (*do_A2 != NULL)
	      (*do_A2)(pulses[pulse].i2c_pin, value);
	    else
	      if((*MENU).verbosity > VERBOSITY_SOME) {
		(*MENU).out(F("doA2 code for i2cW not set\tp="));
		(*MENU).outln(pulse);
	      }
	  } else
	    if((*MENU).verbosity > VERBOSITY_SOME) {
	      (*MENU).out(F("i2cW pulse does not have i2c_pin\t p="));
	      (*MENU).outln(pulse);
	    }
	}
	break;
#endif

      case doA2:
	{
	  int parameter1 = *icode_p++;
	  int parameter2 = *icode_p++;
	  if (*do_A2 != NULL)
	    (*do_A2)(parameter1, parameter2);
	  else
	    if((*MENU).verbosity > VERBOSITY_SOME) {
	      (*MENU).out(F("doA2 code not set\tp="));
	      (*MENU).outln(pulse);
	    }
	}
	break;

      case INFO:
	pulse_info_1line(pulse);
	break;

      case DONE:
	pulses[pulse].period.time = pulses[pulse].base_time;	// might make sense, sometimes
	pulses[pulse].countdown = 0;
	icode_p = pulses[pulse].icode_p;
	pulses[pulse].icode_index = 0;
	busy = false;
	break;

      default:
	(*MENU).error_ln(F("invalid icode"));
	busy = false;
      } // switch icode

    } else {	// icode >= 0, interpreted as a JIFFLE

      multiplier = icode;
      divisor =  *(icode_p +1);
      counter =  *(icode_p +2);
#if defined DEBUG_ICODE
      (*MENU).out("\ntry jiffle:");
      (*MENU).out("\tmultiplier ");
      (*MENU).out(multiplier);
      (*MENU).out("\tdivisor ");
      (*MENU).out(divisor);
      (*MENU).out("\tcounter ");
      (*MENU).out(counter);
      (*MENU).out("\tcountdown ");
      (*MENU).outln(pulses[pulse].countdown);
#endif
      // check for end of jiffle or WAIT
      // to be able to play unfinished jiffletabs while editing them
      // we check the first 2 items of the triple for zero
      if (multiplier == 0 || divisor==0) {	// multiplier|divisor==0 no next phase, KILL pulse
#if defined DEBUG_ICODE
	(*MENU).outln(F("multiplier|divisor==0,  END of JIFF, KILL"));
#endif
	// icode_p += 3;	// skip invalid data triple
	if(pulses[pulse].flags & HAS_GPIO)		// TODO: reset gpio here or not, see 'KILL'
	  if(pulses[pulse].gpio != ILLEGAL8) {		// currently the gpios stay high after a jiff without
	    digitalWrite(pulses[pulse].gpio, 0);	// so yes, set gpio LOW
#if defined DEBUG_ICODE
	    (*MENU).out("reset GPIO ");
	    (*MENU).outln(pulses[pulse].gpio);
#endif
	  }

	init_pulse(pulse);	// KILL pulse, makes old jiffletabs usable as icode
	icode_p = pulses[pulse].icode_p;	// programmers aesthetics...

	busy = false;
	//	  return;		// and return
      } else if (counter==0) {	// WAIT?
#if defined DEBUG_ICODE
      (*MENU).out("\nwait ");
      (*MENU).outln(pulses[pulse].base_time * multiplier / divisor);
#endif
	pulses[pulse].period.time = pulses[pulse].base_time * multiplier / divisor;
	pulses[pulse].counter--;	// *WAITING IS NOT COUNTED AS A SEPARATE PULSE*
	busy = false;
	icode_p += 3;
      } else { // JIFF valid jiffle data, *do jiffle*
	if (pulses[pulse].countdown == 0) {
#if defined DEBUG_ICODE
	  (*MENU).out(F("INIT jiff\tcounter "));
	  (*MENU).out(counter);
	  (*MENU).out(F("\ttim "));
	  (*MENU).outln(pulses[pulse].base_time * multiplier / divisor);
#endif
	  pulses[pulse].countdown = counter;		// initalise counter of next phase
	  pulses[pulse].period.time = pulses[pulse].base_time * multiplier / divisor;
	}

	if((pulses[pulse].flags & HAS_GPIO) && (pulses[pulse].gpio != ILLEGAL8))
	  digitalWrite(pulses[pulse].gpio, pulses[pulse].counter & 1);	// gpio click
#if defined DEBUG_ICODE
	(*MENU).out(F("GPIO\t"));
	if(pulses[pulse].counter & 1)
	  (*MENU).outln(F("HIGH"));
	else
	  (*MENU).outln(F("LOW"));
#endif
	if (--pulses[pulse].countdown > 0) {	// countdown, phase ended?
#if defined DEBUG_ICODE
	  (*MENU).out(F("countdown "));
	  (*MENU).outln(pulses[pulse].countdown);
#endif
	  busy = false;				//   no: return immediately
	} else {	// countdown finished, 1 JIFF done
#if defined DEBUG_ICODE
	  (*MENU).outln(F("1 JIFF done"));
#endif
	  icode_p += 3;
	  busy= false; // wait another period on last gpio state
	} //  1 JIFF done
      } // JIFF valid jiffle data, *do jiffle*
    } // JIFFLE, WAIT, JIFFLE END?
    pulses[pulse].icode_index = icode_p - pulses[pulse].icode_p;

#if defined DEBUG_ICODE
    (*MENU).out(F("icode_index "));
    (*MENU).outln(pulses[pulse].icode_index);
#endif
  } // while(busy)
#if defined DEBUG_ICODE
      (*MENU).out(F("iCODE\tpulse "));
      (*MENU).out(pulse);
      (*MENU).out(F("\t#"));
      (*MENU).out(pulses[pulse].counter);
      (*MENU).outln(F("\t1 step done  ----|"));
#endif
} // play_icode()

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
		       pulse_time_t when, unsigned long unit,
		       unsigned long factor, unsigned long divisor, int sync)
{
  (*MENU).outln("PULSES.setup_pulse_synced");
  pulse_time_t new_period;

  if (sync) {
    pulse_time_t delta;
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

  pulse_time_t sum = global_next;
  pulse_time_t delta = now;
  sub_time(&delta, &sum);
  (*MENU).out(F("global next in"));
  display_realtime_sec(sum);
  (*MENU).ln();
}


// display a time in seconds:
float Pulses::display_realtime_sec(pulse_time_t duration) {
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


// void display_time_human_format(pulse_time_t duration);	// everyday time format	d h m s  formatted with spaces
void Pulses::display_time_human_format(pulse_time_t duration) {
  unsigned long seconds = (((float) duration.time / 1000000.0) + 0.5);
  if (duration.overflow)
    seconds += duration.overflow * 4295;

  unsigned int days = seconds / 86400;
  seconds %= 86400;
  unsigned int hours = seconds / 3600;
  seconds %= 3600;
  unsigned int minutes = seconds / 60;
  seconds %= 60;

  if(days) {
    (*MENU).out(days);
    (*MENU).out(F("d "));
  }
  if(hours || days) {	// so 1d 0h 22' 13"  *will* show "0h"
    (*MENU).out(hours);
    (*MENU).out(F("h "));
  } else (*MENU).space(3);

  if(minutes<10)
    (*MENU).space();
  (*MENU).out(minutes);
  (*MENU).out(F("\' "));

  if(seconds<10)
    (*MENU).space();
  (*MENU).out(seconds);
  (*MENU).out(F("\" "));

  if(seconds<10 && minutes==0 && hours==0 && days==0)	// fall back to float time display below 10 seconds
    display_realtime_sec(duration);
}


void Pulses::display_time_human(pulse_time_t duration) {  // everyday time format d h m s short
  unsigned long seconds = (((float) duration.time / 1000000.0) + 0.5);
  if (duration.overflow)
    seconds += duration.overflow * 4295;

  unsigned int days = seconds / 86400;
  seconds %= 86400;
  unsigned int hours = seconds / 3600;
  seconds %= 3600;
  unsigned int minutes = seconds / 60;
  seconds %= 60;

  if(minutes==0 && hours==0 && days==0 && seconds && seconds < 20) {
    display_realtime_sec(duration);
    return;	// done
  }
  // seconds >= 20 only

  if(days) {
    (*MENU).out(days);
    (*MENU).out(F("d "));
  }
  if(hours || days) {
    (*MENU).out(hours);
    (*MENU).out(F("h "));
  }
  if(minutes || hours || days) {
    (*MENU).out(minutes);
    (*MENU).out(F("\' "));
  }
  (*MENU).out(seconds);
  (*MENU).out(F("\" "));
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
void Pulses::display_real_ovfl_and_sec(pulse_time_t then) {
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

  if((*MENU).is_chiffre((*MENU).peek()))	// more numeric input, so no display yet...	FIXME: not here
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


void Pulses::show_time_unit() {
  (*MENU).out(F("TIME_U:\t"));
  (*MENU).out(time_unit);
  (*MENU).out(F(" microseconds = "));
  (*MENU).out((float) (1000000.0 / (float) time_unit), 6);
  (*MENU).outln(F(" per second"));
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
