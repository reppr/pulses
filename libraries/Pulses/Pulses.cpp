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


// FIXME: ################
void display_action(int pulse);
float display_realtime_sec(struct time duration);
void pulse_info_1line(int pulse);
void click(int pulse);
pulse_t * pulses;
void do_jiffle (int pulse);
void do_throw_a_jiffle(int pulse);


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

#ifdef USE_SERIAL_BAUD
    Serial.println(F("no free pulses"));
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


// unused?
int setup_counted_pulse(void (*pulse_do)(int), unsigned char new_flags, \
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
  for (int pulse=0; pulse<CLICK_PULSES; pulse++) {
    init_pulse(pulse);
    // pulses[pulse].flags |= DO_NOT_DELETE;
  }
}


void Pulses::click(int pulse) {	// can be called from a pulse
  digitalWrite(pulses[pulse].char_parameter_1, pulses[pulse].counter & 1);
}


// pins for click_pulses:
// It is a bit obscure to held them in an array indexed by [pulse]
// but it's simple and working well...
unsigned char click_pin[CLICK_PULSES];

void Pulses::init_click_pins() {
  for (int pulse=0; pulse<CLICK_PULSES; pulse++) {
    pinMode(click_pin[pulse], OUTPUT);
    digitalWrite(click_pin[pulse], LOW);
  }
}


//  // unused? (I use the synced version more often)
//  int Pulses::setup_click_pulse(void (*pulse_do)(int), unsigned char new_flags,
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
  for (int pulse=0; pulse<CLICK_PULSES; pulse++)
    pulses[pulse].flags &= ~ACTIVE;

  fix_global_next();

// FIXME: ################
#ifdef USE_SERIAL_BAUD
  Serial.println(mutedAllPulses);
#endif
}


/* **************************************************************** */
// creating, editing, killing pulses


// FIXME: ################ type?
unsigned long selected_pulses=0L;	// pulse bitmask


// FIXME: ################ want to think that over again...
int sync=1;			// syncing edges or middles of square pulses


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


// currently only used in menu
// make an existing pulse to a click pulse:
void Pulses::en_click(int pulse)
{
  if (pulse != ILLEGAL) {
    pulses[pulse].periodic_do = (void (*)(int)) &Pulses::click;
    pulses[pulse].char_parameter_1 = click_pin[pulse];
    pinMode(pulses[pulse].char_parameter_1, OUTPUT);
    digitalWrite(pulses[pulse].char_parameter_1, LOW);
  }
}


// currently only used in menu
// make an existing pulse to display 1 info line:
void en_info(int pulse)
{
  if (pulse != ILLEGAL) {
    pulses[pulse].periodic_do = (void (*)(int)) &Pulses::pulse_info_1line;
  }
}


// currently only used in menu
// make an existing pulse to display multiline pulse info:
void en_INFO(int pulse)
{
  if (pulse != ILLEGAL) {
    pulses[pulse].periodic_do = (void (*)(int)) &Pulses::pulse_info;
  }
}


// binary print flags:
// print binary numbers with leading zeroes and a space
void serial_print_BIN(unsigned long value, int bits) {
  int i;
  unsigned long mask=0;

  for (i = bits - 1; i >= 0; i--) {
    mask = (1 << i);
      if (value & mask)
	Serial.print(1);
      else
	Serial.print(0);
  }
  Serial.print(' ');
}


// time unit that the user sees.
// it has no influence on inner working, but is a menu I/O thing only
// the user sees and edits times in time units.
unsigned long time_unit = 100000L;		// scaling timer to 10/s 0.1s

// I want time_unit to be dividable by a semi random selection of small integers
// avoiding rounding errors as much as possible.
//
// I consider factorials as a good choice:
// unsigned long time_unit =    40320L;		// scaling timer to  8!, 0.040320s
// unsigned long time_unit =   362880L;		// scaling timer to  9!, 0,362880s
// unsigned long time_unit =  3628800L;		// scaling timer to 10!, 3.628800s




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


void print_period_in_time_units(int pulse) {
  float time_units, scratch;

  Serial.print(pulse_);
  time_units = ((float) pulses[pulse].period.time / (float) time_unit);

  scratch = 1000.0;
  while (scratch > max(time_units, (float) 1.0)) {
    Serial.print(' ');
    scratch /= 10.0;
  }

  Serial.print((float) time_units, 3);
  Serial.print(timeUnits);
}


// currently only used in menu
// pulse_info_1line():	one line pulse info, short version
void Pulses::pulse_info_1line(int pulse) {
  unsigned long realtime=micros();	// let's take time *before* serial output

  Serial.print(F("PULSE "));
  Serial.print(pulse);
  Serial.print('/');
  Serial.print((unsigned int) pulses[pulse].counter);

  Serial.print(flags_);
  serial_print_BIN(pulses[pulse].flags, 8);

  Serial.print('\t');
  print_period_in_time_units(pulse);

  Serial.print('\t');
  display_action(pulse);

  Serial.print('\t');
  Serial.print(expected_);
  Serial.print(F("     "));
  display_realtime_sec(pulses[pulse].next);

  Serial.print('\t');
  Serial.print(F("now "));
  struct time scratch = now;
  scratch.time = realtime;
  display_realtime_sec(scratch);

  if (selected_pulses & (1 << pulse))
    Serial.print(" *");

  Serial.println();
}

const char noAlive[] = "no pulses alive";

void alive_pulses_info_lines()
{
  int count=0;

  for (int pulse=0; pulse<pl_max; ++pulse)	// first port used
    if (pulses[pulse].flags) {				// any flags set?
      pulse_info_1line(pulse);
      count++;
    }

  if (count == 0) {				// second port used
    Serial.print(noAlive);
      count++;
    }

  if (count == 0)
    Serial.println(noAlive);

  Serial.println();
}


// pulse_info() as paylod for pulses:
// Prints pulse info over serial and blinks the LED
void Pulses::pulse_info(int pulse) {

#ifdef LED_PIN
  digitalWrite(LED_PIN,HIGH);		// blink the LED
#endif

  Serial.print(pulseInfo);
  Serial.print(pulse);
  Serial.print('/');
  Serial.print((unsigned int) pulses[pulse].counter);

  Serial.print('\t');
  display_action(pulse);

  Serial.print(flags_);
  Serial.print_BIN(pulses[pulse].flags, 8);
  Serial.println();

  Serial.print("pin ");  Serial.print((int) pulses[pulse].char_parameter_1);
  Serial.print(index_);  Serial.print((int) pulses[pulse].char_parameter_2);
  Serial.print(times_);  Serial.print(pulses[pulse].int1);
  Serial.print("\tp1 ");  Serial.print(pulses[pulse].parameter_1);
  Serial.print("\tp2 ");  Serial.print(pulses[pulse].parameter_2);
  Serial.print(ul1_);  Serial.print(pulses[pulse].ulong_parameter_1);

  Serial.println();		// start next line

  Serial.print((float) pulses[pulse].period.time / (float) time_unit,3);
  Serial.print(timeUnits);

  Serial.print(pulseOvfl);
  Serial.print((unsigned int) pulses[pulse].period.time);
  Serial.print('/');
  Serial.print(pulses[pulse].period.overflow);

  Serial.print('\t');
  display_realtime_sec(pulses[pulse].period);
  Serial.print(' ');
  Serial.print(pulse_);

  Serial.println();		// start next line

  Serial.print(lastOvfl);
  Serial.print((unsigned int) pulses[pulse].last.time);
  Serial.print('/');
  Serial.print(pulses[pulse].last.overflow);

  Serial.print(nextOvfl);
  Serial.print(pulses[pulse].next.time);
  Serial.print('/');
  Serial.print(pulses[pulse].next.overflow);

  Serial.print('\t');
  Serial.print(expected_);
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

  for (int pulse=0; pulse<pl_max; ++pulse)
    if (pulses[pulse].flags) {				// any flags set?
      pulse_info(pulse);
      count++;
    }

  if (count == 0)
    Serial.println(noAlive);
}


// FIXME: ################
const float overflow_sec = 4294.9672851562600;	// overflow time in seconds


// display a time in seconds:
float display_realtime_sec(struct time duration) {
  float seconds=((float) duration.time / 1000000.0);
  seconds += overflow_sec * (float) duration.overflow;

  float scratch = 1000.0;
  while (scratch > max(seconds, 1.0)) {
    Serial.print(' ');
    scratch /= 10.0;
  }


  Serial.print(seconds , 3);
  Serial.print("s");

  return seconds;
}


void display_action(int pulse) {
  void (*scratch)(int);

  scratch=&click;
  if (pulses[pulse].periodic_do == scratch) {
    Serial.print("click\t");	// 8 chars at least
    return;
  }

  scratch=&do_jiffle;
  if (pulses[pulse].periodic_do == scratch) {
    Serial.print("do_jiffle");
    return;
  }

  scratch=&do_throw_a_jiffle;
  if (pulses[pulse].periodic_do == scratch) {
    Serial.print("seed jiffle");
    return;
  }

  scratch=&pulse_info;
  if (pulses[pulse].periodic_do == scratch) {
    Serial.print("pulse_info");
    return;
  }

  scratch=&pulse_info_1line;
  if (pulses[pulse].periodic_do == scratch) {
    Serial.print("info line");
    return;
  }

  scratch=NULL;
  if (pulses[pulse].periodic_do == scratch) {
    Serial.print("NULL\t");	// 8 chars at least
    return;
  }

  Serial.print("UNKNOWN\t");
}


// make an existing pulse to a jiffle thrower pulse:
void en_jiffle_thrower(int pulse, unsigned int *jiffletab)
{
  if (pulse != ILLEGAL) {
    pulses[pulse].periodic_do = &do_throw_a_jiffle;
    pulses[pulse].parameter_2 = (unsigned int) jiffletab;
  }
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
int setup_pulse_synced(void (*pulse_do)(int), unsigned char new_flags,
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


int setup_click_synced(struct time when, unsigned long unit, unsigned long factor,
		       unsigned long divisor, int sync) {
  int pulse= setup_pulse_synced(&click, ACTIVE, when, unit, factor, divisor, sync);

  if (pulse != ILLEGAL) {
    pulses[pulse].char_parameter_1 = click_pin[pulse];
    pinMode(pulses[pulse].char_parameter_1, OUTPUT);
    digitalWrite(pulses[pulse].char_parameter_1, LOW);
  }

  return pulse;
}


// some default rhythms:

// helper function to generate certain types of sequences of harmonic relations:
// for harmonics I use rational number sequences a lot.
// this is a versatile function to create them:
void init_ratio_sequence(struct time when,
			 int factor0, int factor_step,
			 int divisor0, int divisor_step, int count,
			 unsigned int scaling, int sync
			 )
// By design click pulses *HAVE* to be defined *BEFORE* any other pulses:
//
// usage:
// 1,2,3,4 pattern	init_ratio_sequence(now, 1, 1, 1, 0, 4, scaling, sync)
// 3,5,7,9 pattern	init_ratio_sequence(now, 3, 2, 1, 0, 4, scaling, sync)
// 1/2, 2/3, 3/4, 4/5	init_ratio_sequence(now, 1, 1, 2, 1, 4, scaling, sync)
{
  const unsigned long unit=scaling*time_unit;
  unsigned long factor=factor0;
  unsigned long divisor=divisor0;

  // init_click_pulses();

  for (; count; count--) {
    setup_click_synced(when, unit, factor, divisor, sync);
    factor += factor_step;
    divisor += divisor_step;
  }

  fix_global_next();
}


/* **************************************************************** */
// some pre-defined patterns:

const char rhythm_[] = "rhythm ";
const char sync_[] = "sync ";

void init_rhythm_1(int sync) {
  // By design click pulses *HAVE* to be defined *BEFORE* any other pulses:
  unsigned long divisor=1;
  unsigned long scaling=6;

#ifdef USE_SERIAL_BAUD
  Serial.print(F()rhythm_); Serial.print(1);
  spaces(1);
  Serial.print(' ');
   Serial.print(' ');
   Serial.print(F()sync_); Serial.println(sync);
#endif

  init_click_pulses();
  get_now();

  for (long factor=2L; factor<6L; factor++)	// 2, 3, 4, 5
    setup_click_synced(now, scaling*time_unit, factor, divisor, sync);

  // 2*2*3*5
  setup_click_synced(now, scaling*time_unit, 2L*2L*3L*5L, divisor, sync);

  fix_global_next();
}


// frequencies ratio 1, 4, 6, 8, 10
void init_rhythm_2(int sync) {
  // By design click pulses *HAVE* to be defined *BEFORE* any other pulses:
  int scaling=60;
  unsigned long factor=1;
  unsigned long unit= scaling*time_unit;

#ifdef USE_SERIAL_BAUD
  Serial.print(F()rhythm_); Serial.print(2);
  Serial.print(' '); Serial.print(F()sync_); Serial.println(sync);
#endif

  init_click_pulses();
  get_now();

  for (unsigned long divisor=4; divisor<12 ; divisor += 2)
    setup_click_synced(now, unit, factor, divisor, sync);

  // slowest *not* synced
  setup_click_synced(now, unit, 1, 1, 0);

  fix_global_next();
}

// nice 2 to 3 to 4 to 5 pattern with phase offsets
void init_rhythm_3(int sync) {
  // By design click pulses *HAVE* to be defined *BEFORE* any other pulses:
  unsigned long factor, divisor=1L;
  const unsigned long scaling=5L;
  const unsigned long unit=scaling*time_unit;

#ifdef USE_SERIAL_BAUD
  Serial.print(F()rhythm_); Serial.print(3);
  Serial.print(' '); Serial.print(F()sync_); Serial.println(sync);
#endif

  init_click_pulses();
  get_now();

  factor=2;
  setup_click_synced(now, unit, factor, divisor, sync);

  factor=3;
  setup_click_synced(now, unit, factor, divisor, sync);

  factor=4;
  setup_click_synced(now, unit, factor, divisor, sync);

  factor=5;
  setup_click_synced(now, unit, factor, divisor, sync);

  fix_global_next();
}


void init_rhythm_4(int sync) {
  // By design click pulses *HAVE* to be defined *BEFORE* any other pulses:
  const unsigned long scaling=15L;

#ifdef USE_SERIAL_BAUD
  Serial.print(F()rhythm_); Serial.print(4);
  Serial.print(' '); Serial.print(F()sync_); Serial.println(sync);
#endif

  init_click_pulses();
  get_now();

  setup_click_synced(now, scaling*time_unit, 1, 1, sync);     // 1
  init_ratio_sequence(now, 1, 1, 2, 1, 4, scaling, sync);     // 1/2, 2/3, 3/4, 4/5
}


// dest codes:
#define CODE_PULSES	0		// dest code pulses: apply selected_pulses
#define CODE_TIME_UNIT	1		// dest code time_unit
unsigned char dest = CODE_PULSES;


/* **************************************************************** */
// pulses menu:

// what is selected?

void print_selected_pulses() {

#ifdef CLICK_PULSES
  for (int pulse=0; pulse<min(CLICK_PULSES,8); pulse++)
    if (selected_pulses & (1 << pulse))
      Serial.print(pulse, HEX);
    else
      Serial.print(".");
#endif

#if (CLICK_PULSES > 8)
   Serial.print(' ');
   Serial.print(' ');
  for (int pulse=8; pulse<min(CLICK_PULSES,16); pulse++)
    if (selected_pulses & (1 << pulse))
      Serial.print(pulse, HEX);
    else
      Serial.print(".");
#endif

#if (pl_max > CLICK_PULSES)
   Serial.print(' ');
   Serial.print(' ');
  for (int pulse=CLICK_PULSES; pulse<pl_max; pulse++)
    if (selected_pulses & (1 << pulse))
      Serial.write('+');
    else
      Serial.write('.');
#endif

  Serial.println();
}

const char selected_[] = "selected ";

void print_selected() {
  Serial.print(F()selected_);

  switch (dest) {
  case CODE_PULSES:
    print_selected_pulses();
    break;

  case CODE_TIME_UNIT:
    Serial.println(F()timeUnit);
    break;
  }
}


// info_select_destination_with()
const char selectDestinationInfo[] =
  "SELECT DESTINATION for '= * / s K p n c j' to work on:\t\t";
const char selectPulseWith[] = "Select puls with ";
const char selectAllPulses[] =
  "\na=select *all* click pulses\tA=*all* pulses\tl=alive clicks\tL=all alive\tx=none\t~=invert selection";
const char uSelect[] = "u=select ";
const char selected__[] = "\t(selected)";

void info_select_destination_with(boolean extended_destinations) {
  Serial.print(F()selectDestinationInfo);
  print_selected();  Serial.println();

  Serial.print(F()selectPulseWith);
  for (int pulse=0; pulse<CLICK_PULSES; pulse++) {
    Serial.print(pulse);
     Serial.print(' ');
     Serial.print(' ');
  }

  Serial.println(F()selectAllPulses);

  if(extended_destinations) {
    Serial.print(F()uSelect);  Serial.print(F()timeUnit);
    if(dest == CODE_TIME_UNIT) {
      Serial.println(F()selected__);
    } else
      Serial.println();
    Serial.println();
  }
}



// menu_program_display()
const char helpInfo[] = \
  "?=help\tm=menu\ti=info\t.=short info";
const char microSeconds[] = " microseconds";
const char muteKill[] = \
  "M=mute all\tK=kill\n\nCREATE PULSES\tstart with 'P'\nP=new pulse\tc=en-click\tj=en-jiffle\tf=en-info\tF=en-INFO\tn=sync now\nS=sync ";
const char perSecond_[] = " per second)";
const char equals_[] = " = ";
const char switchPulse[] = "s=switch pulse on/off";

void menu_program_display() {
  Serial.println(F()helpInfo);

  Serial.println();
  info_select_destination_with(false);

  Serial.print(F()uSelect);  Serial.print(F()timeUnit);
  Serial.print("  (");
  Serial.print(time_unit);
  Serial.print(F()microSeconds);
  Serial.print(F()equals_);
  Serial.print((float) (1000000.0 / (float) time_unit),3);
  Serial.println(F()perSecond_);

  Serial.println();
  Serial.print(F()switchPulse);
  Serial.print('\t');  Serial.print(F()muteKill);
  Serial.println(sync);
}


/* **************************************************************** */
// functions called from the menu:

void multiply_period(int pulse, unsigned long factor) {
  struct time new_period;

  new_period=pulses[pulse].period;
  mul_time(&new_period, factor);
  set_new_period(pulse, new_period);
}


void divide_period(int pulse, unsigned long divisor) {
  struct time new_period;

  new_period=pulses[pulse].period;
  div_time(&new_period, divisor);
  set_new_period(pulse, new_period);
}


const char setTimeUnit_[] = "Set time unit to ";

void set_time_unit_and_inform(unsigned long new_value) {
  time_unit = new_value;
  Serial.print(F()setTimeUnit_);
  Serial.print(time_unit);
  Serial.println(F()microSeconds);
}


// menu interface to reset a pulse and prepare it to be edited:
void reset_and_edit_pulse(int pulse) {
  init_pulse(pulse);
  pulses[pulse].flags |= SCRATCH;	// set SCRATCH flag
  pulses[pulse].flags &= ~ACTIVE;	// remove ACTIVE

  // set a default pulse length:
  struct time scratch;
  scratch.time = time_unit;
  scratch.overflow = 0;
  mul_time(&scratch, 12);		// 12 looks like a usable default
  pulses[pulse].period = scratch;
}


// ****************************************************************
// menu_serial_program_reaction()
// const char selected_[] = "selected ";
const char killPulse[] = "kill pulse ";
const char killedAll[] = "killed all";
const char onlyPositive[] = "only positive sync ";

bool menu_serial_program_reaction(char menu_input) {
  long new_value=0;
  struct time time_scratch;

  switch (menu_input) {

  case '?':	// help
    menu_serial_display();
    alive_pulses_info_lines();
    time_info();  Serial.print('\t'); RAM_info();
    break;

  case '.':	// alive pulses info
    Serial.println();
    // time_info(); Serial.println();
    // RAM_info();
    alive_pulses_info_lines();
    break;

    // *do* change this line if you change CLICK_PULSES
  case '0': case '1': case '2': case '3': case '4':	// toggle pulse selection
  // case '5': case '6': case '7': case '8': case '9':
    selected_pulses ^= (1 << (menu_input - '0'));

    Serial.print(F()selected_);
    print_selected_pulses();
    break;

  case 'u':	// select destination: time_unit
    dest = CODE_TIME_UNIT;
    print_selected();
    break;

  case 'a':	// select destination: all click pulses
    selected_pulses=0;
    for (int pulse=0; pulse<CLICK_PULSES; pulse++)
      selected_pulses |= (1 << pulse);

    Serial.print(F()selected_);
    print_selected_pulses();
    break;

  case 'A':	// select destination: *all* pulses
    selected_pulses = ~0;

    Serial.print(F()selected_);
    print_selected_pulses();
    break;

  case 'l':	// select destination: alive CLICK_PULSES
    selected_pulses=0;
    for (int pulse=0; pulse<CLICK_PULSES; pulse++)
      if(pulses[pulse].flags && (pulses[pulse].flags != SCRATCH))
	selected_pulses |= (1 << pulse);

    Serial.print(F()selected_);
    print_selected_pulses();
    break;

  case 'L':	// select destination: all alive pulses
    selected_pulses=0;
    for (int pulse=0; pulse<pl_max; pulse++)
      if(pulses[pulse].flags && (pulses[pulse].flags != SCRATCH))
	selected_pulses |= (1 << pulse);

    Serial.print(F()selected_);
    print_selected_pulses();
    break;

  case '~':	// invert destination selection
    selected_pulses = ~selected_pulses;

    Serial.print(F()selected_);
    print_selected_pulses();
    break;

  case 'x':	// clear destination selection
    selected_pulses = 0;

    Serial.print(F()selected_);
    print_selected_pulses();
    break;

  case 's':	// switch pulse on/off
    for (int pulse=0; pulse<CLICK_PULSES; pulse++) {
      if (selected_pulses & (1 << pulse)) {
	// special case: switching on an edited SCRATCH pulse:
	if((pulses[pulse].flags & ACTIVE) == 0)	// was off
	  if (pulses[pulse].flags & SCRATCH)	// SCRATCH set, like activating after edit
	    pulses[pulse].flags &= ~SCRATCH;	// so we remove SCRATCH

	pulses[pulse].flags ^= ACTIVE;

	if (pulses[pulse].flags & ACTIVE) {	// DADA test ################
	  get_now();	// ################################################################
	  pulses[pulse].next = now;
	  pulses[pulse].last = pulses[pulse].next;	// for overflow logic
	}
      }
    }

    fix_global_next();
    check_maybe_do();				  // maybe do it *first*
    Serial.println();
    alive_pulses_info_lines();			  // *then* info ;)

    // info_select_destination_with(false);	// DADA ################
    break;

  case 'S':	// enter sync
    Serial.print(F()sync_);
    new_value = numeric_input(sync);
    if (new_value>=0 )
      sync = new_value;
    else
      Serial.print(F()onlyPositive);
    Serial.println(sync);
    break;

  case 'i':	// info
    RAM_info();
    Serial.println();
    alive_pulses_info();
    break;

  case 'M':	// mute
    mute_all_clicks();
    break;

  case '*':	// multiply destination
    switch (dest) {
    case CODE_PULSES:
      new_value = numeric_input(1);
      if (new_value>=0) {
	for (int pulse=0; pulse<CLICK_PULSES; pulse++)
	  if (selected_pulses & (1 << pulse))
	    multiply_period(pulse, new_value);

	Serial.println();
	alive_pulses_info_lines();
      } else
	Serial.println(F()invalid_);
      break;

    case CODE_TIME_UNIT:
      new_value = numeric_input(1);
      if (new_value>0)
	set_time_unit_and_inform(time_unit*new_value);
      else
	Serial.println(F()invalid_);
      break;
    }
    break;

  case '/':	// divide destination
    switch (dest) {
    case CODE_PULSES:
      new_value = numeric_input(1);
      if (new_value>=0) {
	for (int pulse=0; pulse<CLICK_PULSES; pulse++)
	  if (selected_pulses & (1 << pulse))
	    divide_period(pulse, new_value);

	Serial.println();
	alive_pulses_info_lines();
      } else
	Serial.println(F()invalid_);
      break;

    case CODE_TIME_UNIT:
      new_value = numeric_input(1);
      if (new_value>0)
	set_time_unit_and_inform(time_unit/new_value);
      else
	Serial.println(F()invalid_);
      break;
    }
    break;

  case '=':	// set destination to value
    switch (dest) {
    case CODE_PULSES:
      new_value = numeric_input(1);
      if (new_value>=0) {
	for (int pulse=0; pulse<CLICK_PULSES; pulse++)
	  if (selected_pulses & (1 << pulse)) {
	    time_scratch.time = time_unit;
	    time_scratch.overflow = 0;
	    mul_time(&time_scratch, new_value);
	    set_new_period(pulse, time_scratch);
	  }

	Serial.println();
	alive_pulses_info_lines();
      } else
	Serial.println(F()invalid_);
      break;

    case CODE_TIME_UNIT:
      new_value = numeric_input(1);
      if (new_value>0)
	set_time_unit_and_inform(new_value);
      else
	Serial.println(F()invalid_);
      break;
    }
    break;

  case 'K':	// kill selected pulses
    for (int pulse=0; pulse<CLICK_PULSES; pulse++)	// DADA ################
      if (selected_pulses & (1 << pulse)) {
	init_pulse(pulse);
	Serial.print(F()killPulse); Serial.println(pulse);
      }
    Serial.println();
    alive_pulses_info_lines(); Serial.println();
    break;

  case 'P':	// pulse create and edit
    for (int pulse=0; pulse<CLICK_PULSES; pulse++)	// DADA ################
      if (selected_pulses & (1 << pulse)) {
	reset_and_edit_pulse(pulse);
      }

    Serial.println();
    alive_pulses_info_lines();
    break;

  case 'n':	// synchronise to now
    // we work on CLICK_PULSES anyway, regardless dest
    get_now();
    for (int pulse=0; pulse<CLICK_PULSES; pulse++)
      if (selected_pulses & (1 << pulse))
	activate_pulse_synced(pulse, now, abs(sync));

    fix_global_next();
    check_maybe_do();				  // maybe do it *first*

    Serial.println();
    alive_pulses_info_lines();			  // *then* info ;)
   break;


    // debugging entries: DADA ###############################################
  case 'd':	// hook for debugging
    break;

  case 'Y':	// hook for debugging
    init_rhythm_1(sync);
    break;

  case 'X':	// hook for debugging
    init_rhythm_2(sync);
    break;

  case 'C':	// hook for debugging
    init_rhythm_3(sync);
    break;

  case 'V':	// hook for debugging
    init_rhythm_4(sync);
    break;

  case 'B':	// hook for debugging
    setup_jiffles0(sync);
    break;

    // debugging entries: DADA ###############################################

  case 'c':	// en_click
    // we work on CLICK_PULSES anyway, regardless dest
    for (int pulse=0; pulse<CLICK_PULSES; pulse++)
      if (selected_pulses & (1 << pulse))
	en_click(pulse);

    Serial.println();
    alive_pulses_info_lines();
    break;

  case 'j':	// en_jiffle_thrower
    // we work on CLICK_PULSES anyway, regardless dest
    for (int pulse=0; pulse<CLICK_PULSES; pulse++)
      if (selected_pulses & (1 << pulse))
	en_jiffle_thrower(pulse, jiffletab);

    Serial.println();
    alive_pulses_info_lines();
    break;

  case 'f':	// en_info
    // we work on CLICK_PULSES anyway, regardless dest
    for (int pulse=0; pulse<CLICK_PULSES; pulse++)
      if (selected_pulses & (1 << pulse))
	en_info(pulse);

    Serial.println();
    alive_pulses_info_lines();
    break;

  case 'F':	// en_INFO
    // we work on CLICK_PULSES anyway, regardless dest
    for (int pulse=0; pulse<CLICK_PULSES; pulse++)
      if (selected_pulses & (1 << pulse))
	en_INFO(pulse);

    Serial.println();
    alive_pulses_info();
    break;

  case '{':	// enter_jiffletab
    enter_jiffletab(jiffletab);
    display_jiffletab(jiffletab);
    break;

  case '}':	// display jiffletab / end editing jiffletab
    display_jiffletab(jiffletab);
    break;

  default:
    return false;	// menu entry not found
  }
  return true;		// menu entry found
}


/* **************************************************************** */
// jiffles:
// jiffles are (click) patterns defined in jiffletabs and based on a base period
//
// the base period is multiplied/divided by two int values
// the following jiffleteab value counts how many times the pulse will get
// woken up with this new computed period
// then continue with next jiffletab entries
// a zero multiplicator ends the jiffle

// jiffletabs define melody:
// up to 256 triplets of {multiplicator, dividend, count}
// multiplicator and dividend determine period based on the starting pulses period
// a multiplicator of zero indicates end of jiffle
#define JIFFLETAB_INDEX_STEP	3

// put these in PROGMEM or EEPROM	DADA ################
// jiffletab0 is obsolete	DADA ################
unsigned int jiffletab0[] = {2,1024*3,4, 1,1024,64, 1,2048,64, 1,512,4, 1,64,3, 1,32,1, 1,16,2, 0};	// nice short jiffy

// DADA ################
#define JIFFLETAB_ENTRIES	8	// how many triplets
// there *MUST* be a trailing zero in all jiffletabs.

unsigned int jiffletab[] =
  {1,16,2, 1,256,32, 1,128,8, 1,64,2, 1,32,1, 1,16,1, 1,8,2, 0,0,0, 0};	// there *must* be a trailing zero.


// enter_jiffleSerial.print('\t'), edit jiffletab by hand:
#ifdef USE_SERIAL_BAUD
  const char jifftabFull[] = "jiffletab full";
  const char enterJiffletabVal[] = "enter jiffletab values";
#endif

void enter_jiffletab(unsigned int *jiffletab)
{
  int menu_input;
  int new_value;
  int index=0;			// counts ints, *not* triplets

  while (true) {
    if (!char_available())
      Serial.println(F()enterJiffletabVal);

    while (!char_available())	// wait for input
      ;

    // delay(WAITforSERIAL);

    switch (menu_input = get_char()) {
    case ' ': case ',': case '\t':	// white space, comma
      break;

    case '0': case '1': case '2': case '3': case '4':	// numeric
    case '5': case '6': case '7': case '8': case '9':
      char_store((char) menu_input);
      new_value = numeric_input(0);
      jiffletab[index++] = new_value;

      if (new_value == 0)
	return;

      if (index == (JIFFLETAB_ENTRIES*JIFFLETAB_INDEX_STEP)) {	// jiffletab is full
	jiffletab[JIFFLETAB_ENTRIES*JIFFLETAB_INDEX_STEP] = 0;	// trailing 0

#ifdef USE_SERIAL_BAUD
	Serial.println(F()jifftabFull);
#endif

	return;				// quit
      }
      break;

    case '}':	// end jiffletab input. Can be used to display jiffletab.
      display_jiffletab(jiffletab);
      return;
      break;

    default:	// default: end input sequence
      char_store((char) menu_input);
      return;
    }
  }
}


void display_jiffletab(unsigned int *jiffletab)
{
  Serial.print("{");
  for (int i=0; i <= JIFFLETAB_ENTRIES*JIFFLETAB_INDEX_STEP; i++) {
    if ((i % JIFFLETAB_INDEX_STEP) == 0)
      Serial.print(' ');
    Serial.print(jiffletab[i]);
    if (jiffletab[i] == 0)
      break;
    Serial.print(",");
  }
  Serial.println(" }");
}


// DADA
//	// unsigned int jiffletab0[] = {1,512,8, 1,1024,16, 1,2048,32, 1,1024,16, 0};
//	// unsigned int jiffletab0[] = {1,128,2, 1,256,6, 1,512,10, 1,1024,32, 1,3*128,20, 1,64,8, 0};
//	// unsigned int jiffletab0[] = {1,32,4, 1,64,8, 1,128,16, 1,256,32, 1,512,64, 1,1024,128, 0};	// testing octaves
//
//	// unsigned int jiffletab0[] =
//	//   {1,2096,4, 1,512,2, 1,128,2, 1,256,2, 1,512,8, 1,1024,32, 1,512,4, 1,256,3, 1,128,2, 1,64,1, 0};
//
//	// unsigned int jiffletab0[] = {2,1024*3,4, 1,1024,64, 1,2048,64, 1,512,2, 1,64,1, 1,32,1, 1,16,2, 0};
//
//
//	// unsigned int jiffletab0[] = {1,32,2, 0};	// doubleclick
//
//	/*
//	unsigned int jiffletab1[] =
//	  {1,1024,64, 1,512,4, 1,128,2, 1,64,1, 1,32,1, 1,16,1, 0};
//	*/


void do_jiffle (int pulse) {	// to be called by pulse_do
  // pulses[pulse].char_parameter_1	click pin
  // pulses[pulse].char_parameter_2	jiffletab index
  // pulses[pulse].parameter_1		count down
  // pulses[pulse].parameter_2		jiffletab[] pointer
  // pulses[pulse].ulong_parameter_1	base period = period of starting pulse

  digitalWrite(pulses[pulse].char_parameter_1, pulses[pulse].counter & 1);	// click

  if (--pulses[pulse].parameter_1 > 0)				// countdown, phase endid?
    return;						//   no: return immediately

  // if we arrive here, phase endid, start next phase if any:
  unsigned int* jiffletab = (unsigned int *) pulses[pulse].parameter_2;	// read jiffletab[]
  pulses[pulse].char_parameter_2 += JIFFLETAB_INDEX_STEP;
  if (jiffletab[pulses[pulse].char_parameter_2] == 0) {		// no next phase, return
    init_pulse(pulse);					// remove pulse
    return;						// and return
  }

  //initialize next phase, re-using the same pulse:
  int base_index = pulses[pulse].char_parameter_2;		// readability
  pulses[pulse].period.time =
    pulses[pulse].ulong_parameter_1 * jiffletab[base_index] / jiffletab[base_index+1];
  pulses[pulse].parameter_1 = jiffletab[base_index+2];		// count of next phase
}


void setup_jiffle_thrower(unsigned int *jiffletab, unsigned char new_flags, struct time when, struct time new_period) {
  int jiffle_pulse = setup_pulse(&do_throw_a_jiffle, new_flags, when, new_period);
  if (jiffle_pulse != ILLEGAL) {
    parameter_2[jiffle_pulse] = (unsigned int) jiffletab;
  }
}


// pre-defined jiffle pattern:
void setup_jiffles0(int sync) {
  unsigned long factor, divisor = 1;

  int scale=18;
  unsigned long unit=scale*time_unit;

  struct time when, delta, templ, new_period;

#ifdef USE_SERIAL_BAUD
  Serial.print("jiffle0 ");
  Serial.print(F()sync_); Serial.println(sync);
#endif

  get_now();
  when=now;

  factor=2;
  setup_jiffle_thrower_synced(now, unit, factor, divisor, sync, jiffletab0);

  factor=3;
  setup_jiffle_thrower_synced(now, unit, factor, divisor, sync, jiffletab0);

  factor=4;
  setup_jiffle_thrower_synced(now, unit, factor, divisor, sync, jiffletab0);

  factor=5;
  setup_jiffle_thrower_synced(now, unit, factor, divisor, sync, jiffletab0);

  // 2*3*2*5	(the 4 needs only another factor of 2)
  factor=2*3*2*5;
  setup_jiffle_thrower_synced(now, unit, factor, divisor, sync, jiffletab0);

  fix_global_next();
}


int setup_jiffle_thrower_synced(struct time when,
				unsigned long unit,
				unsigned long factor, unsigned long divisor,
				int sync, unsigned int *jiffletab)
{
  int pulse= setup_pulse_synced(&do_throw_a_jiffle, ACTIVE,
			       when, unit, factor, divisor, sync);
  if (pulse != ILLEGAL)
    pulses[pulse].parameter_2 = (unsigned int) jiffletab;

  return pulse;
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
