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

  cli();
  timer0_overflow_count = 0;
  sei();
#endif

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
void Pulses::init_pulse(unsigned int pulse) {
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
  for (unsigned int pulse=0; pulse<pl_max; pulse++) {
    init_pulse(pulse);
  }
}


// void wake_pulse(unsigned int pulse);	do one life step of the pulse
// gets called from check_maybe_do()
void Pulses::wake_pulse(unsigned int pulse) {
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

#ifdef USE_SERIAL_BAUD
    const char noFreePulses[] = "no free pulses";
#endif

int Pulses::setup_pulse(void (*pulse_do)(unsigned int), unsigned char new_flags, \
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

#ifdef USE_SERIAL_BAUD
    serial_println_progmem(noFreePulses);
#endif

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


void Pulses::set_new_period(unsigned int pulse, struct time new_period) {
  pulses[pulse].period.time = new_period.time;
  pulses[pulse].period.overflow = new_period.overflow;

  pulses[pulse].next.time = pulses[pulse].last.time + pulses[pulse].period.time;
  pulses[pulse].next.overflow = pulses[pulse].last.overflow + pulses[pulse].period.overflow;
  if(pulses[pulse].next.time < pulses[pulse].last.time)
    pulses[pulse].next.overflow++;

  fix_global_next();	// it's saver to do that from here, but could be omitted.
}


/* **************************************************************** */
// piezzo clicks on arduino i/o pins
// great help when debugging and a lot of fun to play with :)

/*
  This is the *old 'periodics' style* implementation
  copied over to the Pulses library.
  Click tasks have to be defined first to get the low index range.

  I plan to change the implementation soon.

*/

// FlipFlop pins:

// clicks on piezzos to *hear* the result:
//   or connect LEDs, MOSFETs, MIDI, whatever...
//   these are just FlipFlop pins.

// Click_pulses are a sub-group of pulses that control an arduino
// digital output each.  By design they must be initiated first to get
// the low pulse indices. The pins are configured as outputs by init_click_pins()
// and get used by clicks, jiffles and the like.

// It's best to always leave click_pulses in memory.
// You can set DO_NOT_DELETE to achieve this.


// FIXME: ################
#ifndef CLICK_PULSES		// number of click frequencies
  #define CLICK_PULSES	6       // default number of click frequencies
#endif


// FIXME: ################
// By design click pulses *HAVE* to be defined *BEFORE* any other pulses:
void Pulses::init_click_pulses() {
  for (unsigned int pulse=0; pulse<CLICK_PULSES; pulse++) {
    init_pulse(pulse);
    // pulses[pulse].flags |= DO_NOT_DELETE;
  }
}


void Pulses::click(unsigned int pulse) {	// can be called from a pulse
  digitalWrite(pulses[pulse].char_parameter_1, pulses[pulse].counter & 1);
}


// pins for click_pulses:
// It is a bit obscure to held them in an array indexed by [pulse]
// but it's simple and working well...
unsigned char click_pin[CLICK_PULSES];

void Pulses::init_click_pins() {
  for (unsigned int pulse=0; pulse<CLICK_PULSES; pulse++) {
    pinMode(click_pin[pulse], OUTPUT);
    digitalWrite(click_pin[pulse], LOW);
  }
}


//  // unused? (I use the synced version more often)
//  int Pulses::setup_click_pulse(void (*pulse_do)(unsigned int), unsigned char new_flags,
//  		     struct time when, struct time new_period) {
//    int pulse = setup_pulse(pulse_do, new_flags, when, new_period);
//    if (pulse != ILLEGAL) {
//      pulses[pulse].char_parameter_1 = click_pin[pulse];
//      pinMode(pulses[pulse].char_parameter_1, OUTPUT);
//      digitalWrite(pulses[pulse].char_parameter_1, LOW);
//    }
//
//    return pulse;
//  }


// FIXME: ################
#ifdef USE_SERIAL_BAUD
  const char Pulses::mutedAllPulses[] = "muted all pulses";
#endif

void Pulses::mute_all_clicks() {
  for (unsigned int pulse=0; pulse<CLICK_PULSES; pulse++)
    pulses[pulse].flags &= ~ACTIVE;

  fix_global_next();

// FIXME: ################
#ifdef USE_SERIAL_BAUD
  serial_println_progmem(mutedAllPulses);
#endif
}


/* **************************************************************** */
// creating, editing, killing pulses

// (re-) activate pulse that has been edited or modified at a given time:
// (assumes everything else is set up in order)
// can also be used to sync running pulses on a given time

// currently only used in menu
void Pulses::activate_pulse_synced(unsigned int pulse, \
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


// currently only used in menu
// make an existing pulse to a click pulse:
void Pulses::en_click(unsigned int pulse)
{
  if (pulse != ILLEGAL) {
    pulses[pulse].periodic_do = (void (*)(unsigned int)) &Pulses::click;
    pulses[pulse].char_parameter_1 = click_pin[pulse];
    pinMode(pulses[pulse].char_parameter_1, OUTPUT);
    digitalWrite(pulses[pulse].char_parameter_1, LOW);
  }
}


// currently only used in menu
// make an existing pulse to display 1 info line:
void Pulses::en_info(unsigned int pulse)
{
  if (pulse != ILLEGAL) {
    pulses[pulse].periodic_do = (void (*)(unsigned int)) &Pulses::pulse_info_1line;
  }
}


// currently only used in menu
// make an existing pulse to display multiline pulse info:
void Pulses::en_INFO(unsigned int pulse)
{
  if (pulse != ILLEGAL) {
    pulses[pulse].periodic_do = (void (*)(unsigned int)) &Pulses::pulse_info;
  }
}


// currently only used in menu
// pulse_info_1line():	one line pulse info, short version
void Pulses::pulse_info_1line(unsigned int pulse) {
  unsigned long realtime=micros();	// let's take time *before* serial output

  Serial.print(F("PULSE "));
  Serial.print(pulse);
  slash();
  Serial.print((unsigned int) pulses[pulse].counter);

  serial_print_progmem(flags_);
  serial_print_BIN(pulses[pulse].flags, 8);

  tab();
  print_period_in_time_units(pulse);

  tab();
  display_action(pulse);

  tab();
  serial_print_progmem(expected_); spaces(5);
  display_realtime_sec(pulses[pulse].next);

  tab();
  serial_print_progmem(now_);
  struct time scratch = now;
  scratch.time = realtime;
  display_realtime_sec(scratch);

  if (selected_pulses & (1 << pulse))
    Serial.print(" *");

  Serial.println();
}


// pulse_info()
const char timeUnit[] = "time unit";
const char timeUnits[] = " time units";
const char pulseInfo[] = "*** PULSE info ";
const char flags_[] = "\tflags ";
const char pulseOvfl[] = "\tpulse/ovf ";
const char lastOvfl[] = "last/ovfl ";
const char nextOvfl[] = "   \tnext/ovfl ";
const char index_[] = "\tindex ";
const char times_[] = "\ttimes ";
const char pulse_[] = "pulse ";
const char expected_[] = "expected ";
const char ul1_[] = "\tul1 ";

// pulse_info() as paylod for pulses:
// Prints pulse info over serial and blinks the LED
void Pulses::pulse_info(unsigned int pulse) {

#ifdef LED_PIN
  digitalWrite(LED_PIN,HIGH);		// blink the LED
#endif

  serial_print_progmem(pulseInfo);
  Serial.print(pulse);
  slash();
  Serial.print((unsigned int) pulses[pulse].counter);

  tab();
  display_action(pulse);

  serial_print_progmem(flags_);
  serial_print_BIN(pulses[pulse].flags, 8);
  Serial.println();

  Serial.print("pin ");  Serial.print((int) pulses[pulse].char_parameter_1);
  serial_print_progmem(index_);  Serial.print((int) pulses[pulse].char_parameter_2);
  serial_print_progmem(times_);  Serial.print(pulses[pulse].int1);
  Serial.print("\tp1 ");  Serial.print(pulses[pulse].parameter_1);
  Serial.print("\tp2 ");  Serial.print(pulses[pulse].parameter_2);
  serial_print_progmem(ul1_);  Serial.print(pulses[pulse].ulong_parameter_1);

  Serial.println();		// start next line

  Serial.print((float) pulses[pulse].period.time / (float) time_unit,3);
  serial_print_progmem(timeUnits);

  serial_print_progmem(pulseOvfl);
  Serial.print((unsigned int) pulses[pulse].period.time);
  slash();
  Serial.print(pulses[pulse].period.overflow);

  tab();
  display_realtime_sec(pulses[pulse].period);
  spaces(1); serial_print_progmem(pulse_);

  Serial.println();		// start next line

  serial_print_progmem(lastOvfl);
  Serial.print((unsigned int) pulses[pulse].last.time);
  slash();
  Serial.print(pulses[pulse].last.overflow);

  serial_print_progmem(nextOvfl);
  Serial.print(pulses[pulse].next.time);
  slash();
  Serial.print(pulses[pulse].next.overflow);

  tab();
  serial_print_progmem(expected_);
  display_realtime_sec(pulses[pulse].next);

  Serial.println();		// start last line
  time_info();

  Serial.print("\n\n");			// traling empty line

#ifdef LED_PIN
  digitalWrite(LED_PIN,LOW);
#endif
}


void Pulses::alive_pulses_info()
{
  int count=0;

  for (unsigned int pulse=0; pulse<pl_max; ++pulse)
    if (pulses[pulse].flags) {				// any flags set?
      pulse_info(pulse);
      count++;
    }

  if (count == 0)
    Serial.println(F("no pulses alive"));
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
