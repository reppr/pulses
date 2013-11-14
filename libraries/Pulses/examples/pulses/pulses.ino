/* **************************************************************** */
/*
			pulses.ino

            http://github.com/reppr/pulses/
        Given as an example for the Menu library.


Copyright © Robert Epprecht  www.RobertEpprecht.ch   GPLv2



    Please do read README_pulses at the end of this file
*/

/* **************************************************************** */


/*  GNU General Public License GPLv2

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation version 2.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/


/* **************************************************************** */
// SOURCE CODE STARTS HERE:
/* **************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#ifdef ARDUINO
  #if ARDUINO >= 100
    #include "Arduino.h"
  #else
    #include "WProgram.h"
  #endif

  #include <Menu.h>
  #include <Pulses.h>

#else	// #include's for Linux PC test version
  #include <iostream>
  #include <Menu/Menu.h>
  #include <Pulses/Pulses.h>

  #include "Pulses/Pulses.cpp"		// why that?
  #include "Menu/Menu.cpp"		// why that?
#endif


/* **************************************************************** */
// some #define's:
#define ILLEGAL		-1
#define ILLEGAL_PIN	255

/* **************************************************************** */
// MENU

//  Menu(int size, int menuPages, int (*maybeInput)(void), STREAMTYPE & port);
/*
  This version definines the menu INPUT routine int men_getchar();
  in the *program* not inside the Menu class.
*/
#ifdef ARDUINO
  /* BAUDRATE for Serial:	uncomment one of the following lines:	*/
  #define BAUDRATE	115200		// works fine here
  //#define BAUDRATE	57600
  //#define BAUDRATE	38400
  //#define BAUDRATE	19200
  //#define BAUDRATE	9600		// failsafe
  //#define BAUDRATE	31250		// MIDI

  #define MENU_OUTSTREAM	Serial

  int men_getchar() {
    if (!Serial.available())	// ARDUINO
      return EOF;

    return Serial.read();
  }

#else

  #define MENU_OUTSTREAM	cout

  int men_getchar() {
    return getchar();		// c++ Linux PC test version
  }

#endif
Menu MENU(32, 3, &men_getchar, MENU_OUTSTREAM);


// FIXME: ################		// defined later on
void menu_pulses_display();		// defined later on
bool menu_pulses_reaction(char token);	// defined later on
void display_action(int pulse);		// defined later on
void do_jiffle (int pulse);		// defined later on
void do_throw_a_jiffle(int pulse);	// defined later on
void init_click_pins();			// defined later on
void init_click_pulses();		// defined later on
void init_rhythm_1(int sync);		// defined later on
void init_rhythm_2(int sync);		// defined later on
void init_rhythm_3(int sync);		// defined later on
void init_rhythm_4(int sync);		// defined later on
void setup_jiffles0(int sync);		// defined later on
void alive_pulses_info_lines();		// defined later on


// FIXME: ################
#ifndef CLICK_PULSES		// number of click frequencies
  #define CLICK_PULSES	7       // default number of click frequencies
#endif
uint8_t click_pin[CLICK_PULSES];	// ################


/* **************************************************************** */
// PULSES

#ifdef ARDUINO		// on ARDUINO
  #ifdef __SAM3X8E__
const int pl_max=32;		// could be more on DUE ;)
  #else
    #ifdef __AVR_ATmega328P__
const int pl_max=12;		// saving RAM on 328P
    #else
const int pl_max=16;		// default i.e. mega boards
    #endif
  #endif
#else			// *NOT* on ARDUINO...
const int pl_max=64;		// Linux PC test version
#endif

Pulses PULSES(pl_max);


/* **************************************************************** */
#ifdef ARDUINO
/* Arduino setup() and loop():					*/

// needed for MENU.add_page();
void softboard_display();		// defined later on
bool softboard_reaction(char token);	// defined later on

int softboard_page=-1;		// see: maybe_run_continuous()

void setup() {
  Serial.begin(BAUDRATE);	// Start serial communication.

#ifdef __AVR_ATmega32U4__
  /* on ATmega32U4		Leonardo, Mini, LilyPad Arduino USB
     to be able to use Serial.print() from setup()
     we *must* do that before:
  */
  while (!Serial) { ;}		// wait for Serial to open
#endif

  // some test output:
  MENU.outln(F("testing pulses.ino"));

  MENU.outln(F("http://github.com/reppr/pulses/\n"));

  MENU.out(F("pulses: "));
  MENU.outln(pl_max);

  MENU.out(F("sizeof(pulse_t) "));
  MENU.out(sizeof(pulse_t));
  MENU.out(F(" * "));
  MENU.out(pl_max);
  MENU.out(F(" pulses = \t"));
  MENU.outln(sizeof(pulse_t)*pl_max);


  // setting up the menu:

  // add pulses main page:
  MENU.add_page("pulses", 'P', \
		&menu_pulses_display, &menu_pulses_reaction, 'P');

  // add softboard page:
  softboard_page = MENU.add_page("Arduino Softboard", 'H',	\
		&softboard_display, &softboard_reaction, 'H');


  // display menu at startup:
  MENU.menu_display();


  // setup pulses, click pulses first:	// FIXME: ################
  #ifdef CLICK_PULSES
  /*
    click_pin[0] = 13;		// configure PINs here	inbuilt LED

    click_pin[1] = 2; 		// configure PINs here
    click_pin[2] = 3; 		// configure PINs here
    click_pin[3] = 4; 		// configure PINs here
    click_pin[4] = 5; 		// configure PINs here
    click_pin[5] = 6; 		// configure PINs here
    click_pin[6] = 7; 		// configure PINs here
  */

    click_pin[0] = 2;		// configure PINs here
    click_pin[1] = 3; 		// configure PINs here
    click_pin[2] = 3; 		// configure PINs here
    click_pin[3] = 4; 		// configure PINs here
    click_pin[4] = 5; 		// configure PINs here
    click_pin[5] = 6; 		// configure PINs here
    click_pin[6] = 7; 		// configure PINs here

    init_click_pins();		// set OUTPUT, LOW
  #endif


  // time and pulses *must* get initialized before setting up pulses:
  PULSES.init_time();		// start time
  PULSES.init_pulses();		// init pulses


  // for a demo one of these could be called from here:
  // init_rhythm_1(1);
  // init_rhythm_2(5);
  MENU.ln(); init_rhythm_3(3);
  // init_rhythm_4(1);
  // setup_jiffles0(1);


  PULSES.fix_global_next();	// we *must* call that here late in setup();

  // informations about alive pulses:
  MENU.ln();
  alive_pulses_info_lines();
}


void maybe_run_continuous();	// defined later on

void loop() {	// ARDUINO

  /* calling check_maybe_do() in a while loop
     increases timing priority of PULSES over the MENU.
     It has little influence on low system load levels
     but when on stress PULSES gets up to *all* available time.
     The intention is to have PULSES continue functioning and
     let the UI starve, when there is not enough time for everything.
  */
  while (PULSES.check_maybe_do()) { }	// in stress PULSES get's *first* priority.

  if(! MENU.lurk_then_do())		// MENU comes second in priority.
    {					// if MENU had nothing to do, then
      maybe_run_continuous();		//    lowest priority:
					//    maybe display input state changes.
    }
}

#else		// c++ Linux PC test version

int main() {

  printf("\nTesting pulses.ino\n");

  printf("\nNumber of pulses: %d\n", pl_max);

  printf("\nPULSES.init_pulses();\n");
  PULSES.init_pulses();
  printf("\n");


  printf("sizeof(pulse_t) %d * %d = \t%d\n\n",	\
	 sizeof(pulse_t), pl_max, sizeof(pulse_t)*pl_max );


  // setting up the menu:

  // add pulses main page:
  MENU.add_page("pulses", 'P', \
		&menu_pulses_display, &menu_pulses_reaction, 'P');

/*
  // softboard does not make sense on PC
  // add softboard page:
  softboard_page = MENU.add_page("Arduino Softboard", 'H',		\
		&softboard_display, &softboard_reaction, 'H');
*/

  // display menu at startup:
  MENU.menu_display();

  // time and pulses *must* get initialized before setting up pulses:
  PULSES.init_time();		// start time
  PULSES.init_pulses();		// init pulses

  PULSES.fix_global_next();	// we *must* call that here late in setup();

  // informations about alive pulses:
  MENU.ln();
  alive_pulses_info_lines();

  // main program loop:
  while (true) {
  /* calling check_maybe_do() in a while loop
     increases timing priority of PULSES over the MENU.
     It has little influence on low system load levels
     but when on stress PULSES gets up to *all* available time.
     The intention is to have PULSES continue functioning and
     let the UI starve, when there is not enough time for everything.
  */
  while (PULSES.check_maybe_do()) { }	// in stress PULSES get's *first* priority.

  if(! MENU.lurk_then_do())		// MENU comes second in priority.
    {					// if MENU had nothing to do, then
#ifdef ARDUINO
      maybe_run_continuous();		//    lowest priority:
					//    maybe display input state changes.
#endif
    }
  }

}

#endif


/* **************************************************************** */
// piezzo clicks on arduino i/o pins to *hear* the result:
// great help when debugging and a lot of fun to play with :)

/*
  This is the *old 'periodics' style* implementation
  copied over to the Pulses library.
  Click tasks have to be defined first to get the low index range.

  I plan to change the implementation soon.

*/

//   or connect LEDs, MOSFETs, MIDI, whatever...
//   these are just FlipFlop pins.

// Click_pulses are a sub-group of pulses that control an arduino
// digital output each.  By design they must be initiated first to get
// the low pulse indices. The pins are configured as outputs by init_click_pins()
// and get used by clicks, jiffles and the like.

// It's best to always leave click_pulses in memory.
// You can set DO_NOT_DELETE to achieve this.


// FIXME: ################
// By design click pulses *HAVE* to be defined *BEFORE* any other pulses:
void init_click_pulses() {
  for (int pulse=0; pulse<CLICK_PULSES; pulse++) {
    PULSES.init_pulse(pulse);
    // PULSES.pulses[pulse].flags |= DO_NOT_DELETE;
  }
}


void click(int pulse) {	// can be called from a pulse
  digitalWrite(PULSES.pulses[pulse].char_parameter_1, PULSES.pulses[pulse].counter & 1);
}


// pins for click_pulses:
// It is a bit obscure to held them in an array indexed by [pulse]
// but it's simple and working well...
// uint_8_t click_pin[CLICK_PULSES];


void init_click_pins() {
  for (int pulse=0; pulse<CLICK_PULSES; pulse++) {	// FIXME: ################
    pinMode(click_pin[pulse], OUTPUT);
    digitalWrite(click_pin[pulse], LOW);
  }
}


//  // unused? (I use the synced version more often)
//  int setup_click_pulse(void (*pulse_do)(int), unsigned char new_flags,
//  		     struct time when, struct time new_period) {
//    int pulse = PULSES.setup_pulse(pulse_do, new_flags, when, new_period);
//    if (pulse != ILLEGAL) {
//      PULSES.pulses[pulse].char_parameter_1 = click_pin[pulse];
//      pinMode(PULSES.pulses[pulse].char_parameter_1, OUTPUT);
//      digitalWrite(PULSES.pulses[pulse].char_parameter_1, LOW);
//    } else {
//      MENU.out(noFreePulses);
//    }
//
//    return pulse;
//  }


const char mutedAllPulses[] = "muted all pulses";
const char noFreePulses[] = "no free pulses";


void mute_all_clicks() {
  for (int pulse=0; pulse<CLICK_PULSES; pulse++)
    PULSES.pulses[pulse].flags &= ~ACTIVE;

  PULSES.fix_global_next();

  MENU.out(mutedAllPulses);
}


// make an existing pulse to a click pulse:
void en_click(int pulse)
{
  if (pulse != ILLEGAL) {
    PULSES.pulses[pulse].periodic_do = (void (*)(int)) &click;
    PULSES.pulses[pulse].char_parameter_1 = click_pin[pulse];
    pinMode(PULSES.pulses[pulse].char_parameter_1, OUTPUT);
    // digitalWrite(PULSES.pulses[pulse].char_parameter_1, LOW);	// ################
  }
}


int setup_click_synced(struct time when, unsigned long unit, unsigned long factor,
		       unsigned long divisor, int sync) {
  int pulse= PULSES.setup_pulse_synced(&click, ACTIVE, when, unit, factor, divisor, sync);

  if (pulse != ILLEGAL) {
    PULSES.pulses[pulse].char_parameter_1 = click_pin[pulse];
    pinMode(PULSES.pulses[pulse].char_parameter_1, OUTPUT);
    digitalWrite(PULSES.pulses[pulse].char_parameter_1, LOW);
  } else {
    MENU.out(noFreePulses);
  }

  return pulse;
}


// FIXME: ################
const float overflow_sec = 4294.9672851562600;	// overflow time in seconds

// display a time in seconds:
float display_realtime_sec(struct time duration) {
  float seconds=((float) duration.time / 1000000.0);	// FIXME: DUE ??? ################
  seconds += overflow_sec * (float) duration.overflow;

  float scratch = 1000.0;
  while (scratch > max(seconds, (float) 1.0)) {		// (float) for Linux PC tests
    MENU.out(' ');
    scratch /= 10.0;
  }

  MENU.out(seconds , 3);
  MENU.out('s');

  return seconds;
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

// const char arrays[]  to save RAM:
const char timeInfo[] = "*** TIME info\t";
const char ticOfl[] = "tic/ofl ";
const char now_[] = "now\t";
const char next_[] = "next\t";	// FIXME: scan strings and use it...

void display_real_ovfl_and_sec(struct time then) {
  MENU.out(ticOfl);
  MENU.out(then.time);
  MENU.out('/');
  MENU.out(then.overflow);
  MENU.space();
  MENU.out('=');
  display_realtime_sec(then);
}

void time_info()
{
  MENU.out(timeInfo);
  MENU.out(now_);
  PULSES.get_now();
  display_real_ovfl_and_sec(PULSES.now);
  MENU.tab();
  MENU.out(next_);
  display_real_ovfl_and_sec(PULSES.global_next);
  MENU.ln();
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

  PULSES.fix_global_next();
}


/* **************************************************************** */
// some pre-defined patterns:

const char rhythm_[] = "rhythm ";
const char sync_[] = "sync ";

void init_rhythm_1(int sync) {
  // By design click pulses *HAVE* to be defined *BEFORE* any other pulses:
  unsigned long divisor=1;
  unsigned long scaling=6;
  struct time now;

  MENU.out(rhythm_); MENU.out(1);
  MENU.out(' ');
  MENU.out(' ');
  MENU.out(' ');
  MENU.out(sync_); MENU.outln(sync);

  init_click_pulses();

  PULSES.get_now();
  now=PULSES.now;

  for (long factor=2L; factor<6L; factor++)	// 2, 3, 4, 5
    setup_click_synced(now, scaling*time_unit, factor, divisor, sync);

  // 2*2*3*5
  setup_click_synced(now, scaling*time_unit, 2L*2L*3L*5L, divisor, sync);

  PULSES.fix_global_next();
}


// frequencies ratio 1, 4, 6, 8, 10
void init_rhythm_2(int sync) {
  // By design click pulses *HAVE* to be defined *BEFORE* any other pulses:
  int scaling=60;
  unsigned long factor=1;
  unsigned long unit= scaling*time_unit;
  struct time now;

  MENU.out(rhythm_); MENU.out(2);
  MENU.out(' '); MENU.out(sync_); MENU.outln(sync);

  init_click_pulses();

  PULSES.get_now();
  now=PULSES.now;

  for (unsigned long divisor=4; divisor<12 ; divisor += 2)
    setup_click_synced(now, unit, factor, divisor, sync);

  // slowest *not* synced
  setup_click_synced(now, unit, 1, 1, 0);

  PULSES.fix_global_next();
}

// nice 2 to 3 to 4 to 5 pattern with phase offsets
void init_rhythm_3(int sync) {
  // By design click pulses *HAVE* to be defined *BEFORE* any other pulses:
  unsigned long factor, divisor=1L;
  const unsigned long scaling=5L;
  const unsigned long unit=scaling*time_unit;
  struct time now;

  MENU.out(rhythm_); MENU.out(3);
  MENU.out(' '); MENU.out(sync_); MENU.outln(sync);

  init_click_pulses();

  PULSES.get_now();
  now=PULSES.now;

  factor=2;
  setup_click_synced(now, unit, factor, divisor, sync);

  factor=3;
  setup_click_synced(now, unit, factor, divisor, sync);

  factor=4;
  setup_click_synced(now, unit, factor, divisor, sync);

  factor=5;
  setup_click_synced(now, unit, factor, divisor, sync);

  PULSES.fix_global_next();
}


void init_rhythm_4(int sync) {
  // By design click pulses *HAVE* to be defined *BEFORE* any other pulses:
  const unsigned long scaling=15L;
  struct time now;

  MENU.out(rhythm_); MENU.out(4);
  MENU.out(' '); MENU.out(sync_); MENU.outln(sync);

  init_click_pulses();

  PULSES.get_now();
  now=PULSES.now;

  setup_click_synced(now, scaling*time_unit, 1, 1, sync);     // 1
  init_ratio_sequence(now, 1, 1, 2, 1, 4, scaling, sync);     // 1/2, 2/3, 3/4, 4/5
}


// dest codes:
#define CODE_PULSES	0		// dest code pulses: apply selected_pulses
#define CODE_TIME_UNIT	1		// dest code time_unit
unsigned char dest = CODE_PULSES;


/* **************************************************************** */
/* Menu UI							*/

// FIXME: ################ type?
unsigned long selected_pulses=0L;	// pulse bitmask


// FIXME: ################ want to think that over again...
int sync=1;			// syncing edges or middles of square pulses


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


void print_period_in_time_units(int pulse) {
  float time_units, scratch;

  MENU.out(pulse_);
  time_units = ((float) PULSES.pulses[pulse].period.time / (float) time_unit);

  scratch = 1000.0;
  while (scratch > max(time_units, (float) 1.0)) {
    MENU.out(' ');
    scratch /= 10.0;
  }

  MENU.out((float) time_units, 3);
  MENU.out(timeUnits);
}


// pulse_info_1line():	one line pulse info, short version
void pulse_info_1line(int pulse) {
  unsigned long realtime=micros();	// let's take time *before* serial output
  struct time scratch;

  MENU.out(F("PULSE "));
  MENU.out(pulse);
  MENU.out('/');
  MENU.out((unsigned int) PULSES.pulses[pulse].counter);

  MENU.out(flags_);
  MENU.outBIN(PULSES.pulses[pulse].flags, 8);

  MENU.out('\t');
  print_period_in_time_units(pulse);

  MENU.out('\t');
  display_action(pulse);

  MENU.out('\t');
  MENU.out(expected_);
  MENU.out(F("     "));
  display_realtime_sec(PULSES.pulses[pulse].next);

  MENU.out('\t');
  MENU.out(F("now "));
  PULSES.get_now();
  scratch = PULSES.now;
  scratch.time = realtime;
  display_realtime_sec(scratch);

  if (selected_pulses & (1 << pulse)) {
    MENU.space();
    MENU.out('*');
  }

  MENU.ln();
}


// make an existing pulse to display 1 info line:
void en_info(int pulse)
{
  if (pulse != ILLEGAL) {
    PULSES.pulses[pulse].periodic_do = (void (*)(int)) &pulse_info_1line;
  }
}


const char noAlive[] = "no pulses alive";

void alive_pulses_info_lines()
{
  int count=0;

  for (int pulse=0; pulse<pl_max; ++pulse)
    if (PULSES.pulses[pulse].flags) {		// any flags set?
      pulse_info_1line(pulse);
      count++;
    }

  if (count == 0)
    MENU.outln(noAlive);

  MENU.ln();
}


void selected_pulses_info_lines()
{
  int count=0;

  for (int pulse=0; pulse<pl_max; ++pulse) {
    if (selected_pulses & (1 << pulse)) {
      pulse_info_1line(pulse);
      count++;
    }
  }

  if (count)
    MENU.ln();
}

// const char arrays[]  to save RAM:
const char pin_[] = "pin ";
const char _p1_[] = "\tp1 ";
const char _p2_[] = "\tp2 ";
const char _ul1_[] = "\tul1 ";

// pulse_info() as paylod for pulses: print pulse info:
void pulse_info(int pulse) {

  MENU.out(pulseInfo);
  MENU.out(pulse);
  MENU.out('/');
  MENU.out((unsigned int) PULSES.pulses[pulse].counter);

  MENU.out('\t');
  display_action(pulse);

  MENU.out(flags_);
  MENU.outBIN(PULSES.pulses[pulse].flags, 8);
  MENU.ln();

  MENU.out(pin_);  MENU.out((int) PULSES.pulses[pulse].char_parameter_1);
  MENU.out(index_);  MENU.out((int) PULSES.pulses[pulse].char_parameter_2);
  MENU.out(times_);  MENU.out(PULSES.pulses[pulse].int1);
  MENU.out(_p1_);  MENU.out(PULSES.pulses[pulse].parameter_1);
  MENU.out(_p2_);  MENU.out(PULSES.pulses[pulse].parameter_2);
  MENU.out(_ul1_);  MENU.out(PULSES.pulses[pulse].ulong_parameter_1);

  MENU.ln();		// start next line

  MENU.out((float) PULSES.pulses[pulse].period.time / (float) time_unit,3);
  MENU.out(timeUnits);

  MENU.out(pulseOvfl);
  MENU.out((unsigned int) PULSES.pulses[pulse].period.time);
  MENU.out('/');
  MENU.out(PULSES.pulses[pulse].period.overflow);

  MENU.out('\t');
  display_realtime_sec(PULSES.pulses[pulse].period);
  MENU.out(' ');
  MENU.out(pulse_);

  MENU.ln();		// start next line

  MENU.out(lastOvfl);
  MENU.out((unsigned int) PULSES.pulses[pulse].last.time);
  MENU.out('/');
  MENU.out(PULSES.pulses[pulse].last.overflow);

  MENU.out(nextOvfl);
  MENU.out(PULSES.pulses[pulse].next.time);
  MENU.out('/');
  MENU.out(PULSES.pulses[pulse].next.overflow);

  MENU.out('\t');
  MENU.out(expected_);
  display_realtime_sec(PULSES.pulses[pulse].next);

  MENU.ln();		// start last line
  time_info();

  MENU.ln();		// traling empty line
}


// make an existing pulse to display multiline pulse info:
void en_INFO(int pulse)
{
  if (pulse != ILLEGAL) {
    PULSES.pulses[pulse].periodic_do = (void (*)(int)) &pulse_info;
  }
}


void alive_pulses_info()
{
  int count=0;

  for (int pulse=0; pulse<pl_max; ++pulse)
    if (PULSES.pulses[pulse].flags) {		// any flags set?
      pulse_info(pulse);
      count++;
    }

  if (count == 0)
    MENU.outln(noAlive);
}


// const char arrays[]  to save RAM:
const char click_[] = "click  ";
const char DoJiffle[] = "do_jiffle";
const char SeedJiffle[] = "seed jiffle";
const char PulseInfo[] = "pulse_info";
const char InfoLine[] = "info line";
const char NULL_[] = "NULL\t";		// 8 char positions at least
const char UNKNOWN_[] = "UNKNOWN\t";	// 8 char positions at least

void display_action(int pulse) {
  void (*scratch)(int);

  scratch=&click;
  if (PULSES.pulses[pulse].periodic_do == scratch) {
    MENU.out(click_);
    MENU.out((int) PULSES.pulses[pulse].char_parameter_1);
    return;
  }

  scratch=&do_jiffle;
  if (PULSES.pulses[pulse].periodic_do == scratch) {
    MENU.out(DoJiffle);
    return;
  }

  scratch=&do_throw_a_jiffle;
  if (PULSES.pulses[pulse].periodic_do == scratch) {
    MENU.out(SeedJiffle);
    return;
  }

  scratch=&pulse_info;
  if (PULSES.pulses[pulse].periodic_do == scratch) {
    MENU.out(PulseInfo);
    return;
  }

  scratch=&pulse_info_1line;
  if (PULSES.pulses[pulse].periodic_do == scratch) {
    MENU.out(InfoLine);
    return;
  }

  scratch=NULL;
  if (PULSES.pulses[pulse].periodic_do == scratch) {
    MENU.out(NULL_);		// 8 char positions at least
    return;
  }

  MENU.out(UNKNOWN_);
}


// make an existing pulse to a jiffle thrower pulse:
void en_jiffle_thrower(int pulse, unsigned int *jiffletab)
{
  if (pulse != ILLEGAL) {
    PULSES.pulses[pulse].periodic_do = &do_throw_a_jiffle;
    PULSES.pulses[pulse].parameter_2 = (unsigned int) jiffletab;
  }
}


int init_jiffle(unsigned int *jiffletab, struct time when, struct time new_period, int origin_pulse)
{
  int pulse;
  struct time jiffle_period=new_period;

  jiffle_period.time = new_period.time * jiffletab[0] / jiffletab[1];

  pulse = PULSES.setup_pulse(&do_jiffle, ACTIVE, when, jiffle_period);
  if (pulse != ILLEGAL) {
    PULSES.pulses[pulse].char_parameter_1 = click_pin[origin_pulse];	// set pin
    // pinMode(click_pin[pulse], OUTPUT);				// should be ok already
    PULSES.pulses[pulse].char_parameter_2 = 0;				// init phase 0
    PULSES.pulses[pulse].parameter_1 = jiffletab[2];			// count of first phase
    PULSES.pulses[pulse].parameter_2 = (unsigned int) jiffletab;
    PULSES.pulses[pulse].ulong_parameter_1 = new_period.time;
  }

  return pulse;
}


void do_throw_a_jiffle(int pulse) {		// for pulse_do
  // pulses[pulse].parameter_2	= (unsigned int) jiffletab;

  // start a new jiffling pulse now (next [pulse] is not yet updated):
  init_jiffle((unsigned int *) PULSES.pulses[pulse].parameter_2, \
	      PULSES.pulses[pulse].next, PULSES.pulses[pulse].period, pulse);
}


/* **************************************************************** */
// pulses menu:

// what is selected?

// display helper function
char hex_chiffre(unsigned int c) {
  if ((c >= 0) && (c <= 9))
    return c + '0';

  if (c < 16 )
    return (c - 10 + 'a');

  return '?';
}


const char selected_[] = "selected ";

void print_selected_pulses() {
  const int hex_pulses=min(pl_max,16);	// displayed as hex chiffres

  MENU.out(selected_);
  for (int pulse=0; pulse<hex_pulses; pulse++) {
    if (selected_pulses & (1 << pulse))
      MENU.out((char) hex_chiffre(pulse));
    else
      MENU.out('.');
  }

  // more than 16 pulses?
  if (hex_pulses == pl_max) {	// no,
    MENU.ln();
    return;			// done
  }

  MENU.space();
  for (int pulse=16; pulse<pl_max; pulse++) {
    if (selected_pulses & (1 << pulse))
      MENU.out('+');
    else
      MENU.out('.');
  }
  MENU.ln();
}

void print_selected() {
  MENU.out(selected_);

  switch (dest) {
  case CODE_PULSES:
    print_selected_pulses();
    break;

  case CODE_TIME_UNIT:
    MENU.outln(timeUnit);
    break;
  }
}


// info_select_destination_with()
const char selectDestinationInfo[] =
  "SELECT DESTINATION for '= * / s K P n c j :' to work on:\t\t";
const char selectPulseWith[] = "Select pulse with ";
const char selectAllPulses[] =
  "\na=select *all* click pulses\tA=*all* pulses\tl=alive clicks\tL=all alive\tx=none\t~=invert selection";
const char uSelect[] = "u=select ";
const char selected__[] = "\t(selected)";

void info_select_destination_with(bool extended_destinations) {
  MENU.out(selectDestinationInfo);
  print_selected();  MENU.ln();

  MENU.out(selectPulseWith);

  // FIXME: use 16 here, when reaction will be prepared for a,b,c,d,e,f too.
  //  for (int pulse=0; pulse<min(pl_max,16); pulse++) {
  for (int pulse=0; pulse<min(pl_max,10); pulse++) {	// FIXME: use 16 here, when reaction will be prepared for a,b,c,d,e,f too.
    MENU.out(hex_chiffre(pulse));
    MENU.space();
    MENU.space();
  }

  MENU.outln(selectAllPulses);

  if(extended_destinations) {	// FIXME: will that ever be used??? ################
    MENU.out(uSelect);  MENU.out(timeUnit);
    if(dest == CODE_TIME_UNIT) {
      MENU.outln(selected__);
    } else
      MENU.ln();
    MENU.ln();
  }
}



// menu_program_display()
const char helpInfo[] = \
  "?=help\ti=info\t.=alive info\t:=selected info";
const char microSeconds[] = " microseconds";
const char muteKill[] = \
  "M=mute all\tK=kill\n\nCREATE PULSES\tstart with 'P'\nP=new pulse\tc=en-click\tj=en-jiffle\tf=en-info\tF=en-INFO\tn=sync now\nS=sync ";
const char perSecond_[] = " per second)";
const char equals_[] = " = ";
const char switchPulse[] = "s=switch pulse on/off";

void menu_pulses_display() {
  MENU.outln(helpInfo);

  MENU.ln();
  info_select_destination_with(false);

  MENU.out(uSelect);  MENU.out(timeUnit);
  MENU.out("  (");
  MENU.out(time_unit);
  MENU.out(microSeconds);
  MENU.out(equals_);
  MENU.out((float) (1000000.0 / (float) time_unit),3);
  MENU.outln(perSecond_);

  MENU.ln();
  MENU.out(switchPulse);
  MENU.out('\t');  MENU.out(muteKill);
  MENU.outln(sync);
}


const char setTimeUnit_[] = "Set time unit to ";

void set_time_unit_and_inform(unsigned long new_value) {
  time_unit = new_value;
  MENU.out(setTimeUnit_);
  MENU.out(time_unit);
  MENU.outln(microSeconds);
}


// menu interface to reset a pulse and prepare it to be edited:
void reset_and_edit_pulse(int pulse) {
  PULSES.init_pulse(pulse);
  PULSES.pulses[pulse].flags |= SCRATCH;	// set SCRATCH flag
  PULSES.pulses[pulse].flags &= ~ACTIVE;	// remove ACTIVE

  // set a default pulse length:
  struct time scratch;
  scratch.time = time_unit;
  scratch.overflow = 0;
  PULSES.mul_time(&scratch, 12);		// 12 looks like a usable default
  PULSES.pulses[pulse].period = scratch;
}


int setup_jiffle_thrower_synced(struct time when,
				unsigned long unit,
				unsigned long factor, unsigned long divisor,
				int sync, unsigned int *jiffletab)
{
  int pulse= PULSES.setup_pulse_synced(&do_throw_a_jiffle, ACTIVE,
			       when, unit, factor, divisor, sync);
  if (pulse != ILLEGAL) {
    PULSES.pulses[pulse].parameter_2 = (unsigned int) jiffletab;
  } else {
    MENU.out(noFreePulses);
  }

  return pulse;
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

// FIXME: ################
#define JIFFLETAB_ENTRIES	8	// how many triplets

// there *MUST* be a trailing zero in all jiffletabs.
unsigned int jiffletab[] =
  {1,16,2, 1,256,32, 1,128,8, 1,64,2, 1,32,1, 1,16,1, 1,8,2, 0,0,0, 0};	// there *must* be a trailing zero.


// void enter_jiffletab(unsigned int *jiffletab), edit jiffletab by hand:
const char jifftabFull[] = "jiffletab full";
const char enterJiffletabVal[] = "enter jiffletab values";

// FIXME: ################
void enter_jiffletab(unsigned int *jiffletab) {
  MENU.out(F("enter_jiffletab() NOT IMPLEMENTED YET 	################"));
}

// FIXME: ################
//	void enter_jiffletab(unsigned int *jiffletab)
//	{
//	  int menu_input;
//	  int new_value;
//	  int index=0;			// counts ints, *not* triplets
//
//	  while (true) {
//	    if (!char_available())
//	      MENU.outln(enterJiffletabVal);
//
//	    while (!char_available())	// wait for input
//	      ;
//
//	    // delay(WAITforSERIAL);
//
//	    switch (menu_input = get_char()) {
//	    case ' ': case ',': case '\t':	// white space, comma
//	      break;
//
//	    case '0': case '1': case '2': case '3': case '4':	// numeric
//	    case '5': case '6': case '7': case '8': case '9':
//	      char_store((char) menu_input);
//	      new_value = MENU.numeric_input(0);
//	      jiffletab[index++] = new_value;
//
//	      if (new_value == 0)
//		return;
//
//	      if (index == (JIFFLETAB_ENTRIES*JIFFLETAB_INDEX_STEP)) {	// jiffletab is full
//		jiffletab[JIFFLETAB_ENTRIES*JIFFLETAB_INDEX_STEP] = 0;	// trailing 0
//		MENU.outln(jifftabFull);
//		return;				// quit
//	      }
//	      break;
//
//	    case '}':	// end jiffletab input. Can be used to display jiffletab.
//	      display_jiffletab(jiffletab);
//	      return;
//	      break;
//
//	    default:	// default: end input sequence
//	      char_store((char) menu_input);
//	      return;
//	    }
//	  }
//	}
// FIXME: ################


void display_jiffletab(unsigned int *jiffletab)
{
  MENU.out("{");
  for (int i=0; i <= JIFFLETAB_ENTRIES*JIFFLETAB_INDEX_STEP; i++) {
    if ((i % JIFFLETAB_INDEX_STEP) == 0)
      MENU.out(' ');
    MENU.out(jiffletab[i]);
    if (jiffletab[i] == 0)
      break;
    MENU.out(",");
  }
  MENU.outln(" }");
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
  // PULSES.pulses[pulse].char_parameter_1	click pin
  // PULSES.pulses[pulse].char_parameter_2	jiffletab index
  // PULSES.pulses[pulse].parameter_1		count down
  // PULSES.pulses[pulse].parameter_2		jiffletab[] pointer
  // PULSES.pulses[pulse].ulong_parameter_1	base period = period of starting pulse

  digitalWrite(PULSES.pulses[pulse].char_parameter_1, PULSES.pulses[pulse].counter & 1);	// click

  if (--PULSES.pulses[pulse].parameter_1 > 0)				// countdown, phase endid?
    return;						//   no: return immediately

  // if we arrive here, phase endid, start next phase if any:
  unsigned int* jiffletab = (unsigned int *) PULSES.pulses[pulse].parameter_2;	// read jiffletab[]
  PULSES.pulses[pulse].char_parameter_2 += JIFFLETAB_INDEX_STEP;
  if (jiffletab[(int) PULSES.pulses[pulse].char_parameter_2] == 0) {		// no next phase, return
    PULSES.init_pulse(pulse);					// remove pulse
    return;						// and return
  }

  //initialize next phase, re-using the same pulse:
  int base_index = PULSES.pulses[pulse].char_parameter_2;		// readability
  PULSES.pulses[pulse].period.time =
    PULSES.pulses[pulse].ulong_parameter_1 * jiffletab[base_index] / jiffletab[base_index+1];
  PULSES.pulses[pulse].parameter_1 = jiffletab[base_index+2];		// count of next phase
}


void setup_jiffle_thrower(unsigned int *jiffletab, unsigned char new_flags, struct time when, struct time new_period) {
  int pulse = PULSES.setup_pulse(&do_throw_a_jiffle, new_flags, when, new_period);

  if (pulse != ILLEGAL) {
    PULSES.pulses[pulse].parameter_2 = (unsigned int) jiffletab;
  } else {
    MENU.out(noFreePulses);
  }
}


// pre-defined jiffle pattern:

// jiffletab0 is obsolete	DADA ################
unsigned int jiffletab0[] = {2,1024*3,4, 1,1024,64, 1,2048,64, 1,512,4, 1,64,3, 1,32,1, 1,16,2, 0};	// nice short jiffy

void setup_jiffles0(int sync) {
  unsigned long factor, divisor = 1;

  int scale=18;
  unsigned long unit=scale*time_unit;

  struct time when;

  MENU.out("jiffle0 ");
  MENU.out(sync_); MENU.outln(sync);

  PULSES.get_now();
  when=PULSES.now;

  factor=2;
  setup_jiffle_thrower_synced(when, unit, factor, divisor, sync, jiffletab0);

  factor=3;
  setup_jiffle_thrower_synced(when, unit, factor, divisor, sync, jiffletab0);

  factor=4;
  setup_jiffle_thrower_synced(when, unit, factor, divisor, sync, jiffletab0);

  factor=5;
  setup_jiffle_thrower_synced(when, unit, factor, divisor, sync, jiffletab0);

  // 2*3*2*5	(the 4 needs only another factor of 2)
  factor=2*3*2*5;
  setup_jiffle_thrower_synced(when, unit, factor, divisor, sync, jiffletab0);

  PULSES.fix_global_next();
}


// ****************************************************************
// menu_serial_program_reaction()
// const char selected_[] = "selected ";
const char killPulse[] = "kill pulse ";
const char killedAll[] = "killed all";
const char onlyPositive[] = "only positive sync ";
const char invalid_[] = "(invalid)";

// display helper function
void short_info() {
  MENU.ln();
  time_info();

  MENU.ln();
  alive_pulses_info_lines();
}

bool menu_pulses_reaction(char menu_input) {
  long new_value=0;
  struct time now, time_scratch;

  switch (menu_input) {

  case '?':	// help, overrides common menu entry for '?'
    MENU.menu_display();	// as common
    short_info();		// + short info
    break;

  case '.':	// time and alive pulses info
    short_info();
    break;

  case ':':
    MENU.ln();
    time_info();
    MENU.ln();
    print_selected_pulses();
    MENU.ln();
    selected_pulses_info_lines();
    break;

  // toggle pulse selection with chiffres:
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
    if((menu_input -'0') >= pl_max)
      return false;		// *only* responsible if pulse exists

    // existing pulse:
    selected_pulses ^= (1 << (menu_input - '0'));

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

    print_selected_pulses();
    break;

  case 'A':	// select destination: *all* pulses
    selected_pulses = ~0;

    print_selected_pulses();
    break;

  case 'l':	// select destination: alive CLICK_PULSES
    selected_pulses=0;
    for (int pulse=0; pulse<CLICK_PULSES; pulse++)
      if(PULSES.pulses[pulse].flags && (PULSES.pulses[pulse].flags != SCRATCH))
	selected_pulses |= (1 << pulse);

    print_selected_pulses();
    break;

  case 'L':	// select destination: all alive pulses
    selected_pulses=0;
    for (int pulse=0; pulse<pl_max; pulse++)
      if(PULSES.pulses[pulse].flags && (PULSES.pulses[pulse].flags != SCRATCH))
	selected_pulses |= (1 << pulse);

    print_selected_pulses();
    break;

  case '~':	// invert destination selection
    selected_pulses = ~selected_pulses;

    print_selected_pulses();
    break;

  case 'x':	// clear destination selection
    selected_pulses = 0;

    print_selected_pulses();
    break;

  case 's':	// switch pulse on/off
    for (int pulse=0; pulse<pl_max; pulse++) {
      if (selected_pulses & (1 << pulse)) {
	// special case: switching on an edited SCRATCH pulse:
	if((PULSES.pulses[pulse].flags & ACTIVE) == 0)	// was off
	  if (PULSES.pulses[pulse].flags & SCRATCH)	// SCRATCH set, like activating after edit
	    PULSES.pulses[pulse].flags &= ~SCRATCH;	// so we remove SCRATCH

	PULSES.pulses[pulse].flags ^= ACTIVE;

	if (PULSES.pulses[pulse].flags & ACTIVE) {	// DADA test ################
	  PULSES.get_now();	// ################################################################
	  PULSES.pulses[pulse].next = PULSES.now;
	  PULSES.pulses[pulse].last = PULSES.pulses[pulse].next;	// for overflow logic
	}
      }
    }

    PULSES.fix_global_next();
    PULSES.check_maybe_do();			// maybe do it *first*
    MENU.ln();
    alive_pulses_info_lines();			// *then* info ;)

    // info_select_destination_with(false);	// DADA ################
    break;

  case 'S':	// enter sync
    MENU.out(sync_);
    new_value = MENU.numeric_input(sync);
    if (new_value>=0 )
      sync = new_value;
    else
      MENU.out(onlyPositive);

    MENU.outln(sync);
    break;

  case 'i':	// info
    MENU.ln();
    time_info();
    MENU.ln();	// ################################################################
    alive_pulses_info();
    break;

  case 'M':	// mute
    mute_all_clicks();
    break;

  case '*':	// multiply destination
    switch (dest) {
    case CODE_PULSES:
      new_value = MENU.numeric_input(1);
      if (new_value>=0) {
	for (int pulse=0; pulse<pl_max; pulse++)
	  if (selected_pulses & (1 << pulse))
	    PULSES.multiply_period(pulse, new_value);

	MENU.ln();
	alive_pulses_info_lines();
      } else
	MENU.outln(invalid_);
      break;

    case CODE_TIME_UNIT:
      new_value = MENU.numeric_input(1);
      if (new_value>0)
	set_time_unit_and_inform(time_unit*new_value);
      else
	MENU.outln(invalid_);
      break;
    }
    break;

  case '/':	// divide destination
    switch (dest) {
    case CODE_PULSES:
      new_value = MENU.numeric_input(1);
      if (new_value>=0) {
	for (int pulse=0; pulse<pl_max; pulse++)
	  if (selected_pulses & (1 << pulse))
	    PULSES.divide_period(pulse, new_value);

	MENU.ln();
	alive_pulses_info_lines();
      } else
	MENU.outln(invalid_);
      break;

    case CODE_TIME_UNIT:
      new_value = MENU.numeric_input(1);
      if (new_value>0)
	set_time_unit_and_inform(time_unit/new_value);
      else
	MENU.outln(invalid_);
      break;
    }
    break;

  case '=':	// set destination to value
    switch (dest) {
    case CODE_PULSES:
      new_value = MENU.numeric_input(1);
      if (new_value>=0) {
	for (int pulse=0; pulse<pl_max; pulse++)
	  if (selected_pulses & (1 << pulse)) {
	    time_scratch.time = time_unit;
	    time_scratch.overflow = 0;
	    PULSES.mul_time(&time_scratch, new_value);
	    PULSES.set_new_period(pulse, time_scratch);
	  }

	MENU.ln();
	alive_pulses_info_lines();
      } else
	MENU.outln(invalid_);
      break;

    case CODE_TIME_UNIT:
      new_value = MENU.numeric_input(1);
      if (new_value>0)
	set_time_unit_and_inform(new_value);
      else
	MENU.outln(invalid_);
      break;
    }
    break;

  case 'K':	// kill selected pulses
    for (int pulse=0; pulse<pl_max; pulse++)	// DADA ################
      if (selected_pulses & (1 << pulse)) {
	PULSES.init_pulse(pulse);
	MENU.out(killPulse); MENU.outln(pulse);
      }
    MENU.ln();
    alive_pulses_info_lines(); MENU.ln();
    break;

  case 'P':	// pulse create and edit
    for (int pulse=0; pulse<pl_max; pulse++)
      if (selected_pulses & (1 << pulse)) {
	reset_and_edit_pulse(pulse);
      }

    MENU.ln();
    alive_pulses_info_lines();
    break;

  case 'n':	// synchronise to now
    // we work on pulses anyway, regardless dest
    PULSES.get_now();
    now=PULSES.now;

    for (int pulse=0; pulse<pl_max; pulse++)
      if (selected_pulses & (1 << pulse))
	PULSES.activate_pulse_synced(pulse, now, abs(sync));

    PULSES.fix_global_next();
    PULSES.check_maybe_do();			  // maybe do it *first*

    MENU.ln();
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

    MENU.ln();
    alive_pulses_info_lines();
    break;

  case 'j':	// en_jiffle_thrower
    // we work on CLICK_PULSES anyway, regardless dest
    for (int pulse=0; pulse<CLICK_PULSES; pulse++)
      if (selected_pulses & (1 << pulse))
	en_jiffle_thrower(pulse, jiffletab);

    MENU.ln();
    alive_pulses_info_lines();
    break;

  case 'f':	// en_info
    // we work on pulses anyway, regardless dest
    for (int pulse=0; pulse<pl_max; pulse++)
      if (selected_pulses & (1 << pulse))
	en_info(pulse);

    MENU.ln();
    alive_pulses_info_lines();
    break;

  case 'F':	// en_INFO
    // we work on pulses anyway, regardless dest
    for (int pulse=0; pulse<pl_max; pulse++)
      if (selected_pulses & (1 << pulse))
	en_INFO(pulse);

    MENU.ln();
    alive_pulses_info_lines();
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
/* README_pulses


README_pulses

Testing Pulses library in an early stage



   Copyright © Robert Epprecht  www.RobertEpprecht.ch   GPLv2

   http://github.com/reppr/pulses


*/
/* **************************************************************** */
#ifndef ARDUINO
  #warning "Not compiling softboard_page on PC."
#else

/*
 * ****************************************************************
 * included from softboard_page.ino
 * ****************************************************************
 */


/* **************************************************************** */
/* CODE TO INSERT INTO YOUR PROGRAM setup() follows: ************** */
/* Arduino setup()						*/

/*
  >>>>>>>>>>>>>>>> PUT this *BEFORE*  Arduino setup() <<<<<<<<<<<<<<<<
  // needed for MENU.add_page();
  void softboard_display();		// defined later on
  bool softboard_reaction(char token);	// defined later on
  >>>>>>>>>>>>>>>> PUT THESE BEFORE  Arduino setup() <<<<<<<<<<<<<<<<
*/

/*
  >>>>>>>>>>>>>>>> PUT this *IN*  Arduino setup() <<<<<<<<<<<<<<<<
  // add softboard to MENU:
  MENU.add_page("Arduino Softboard", 'H',	\
		&softboard_display, &softboard_reaction, 'H');

  >>>>>>>>>>>>>>>> PUT THIS IN Arduino setup() <<<<<<<<<<<<<<<<
*/

/* CODE TO INSERT INTO YOUR PROGRAM setup() ends. ***************** */
/* **************************************************************** */



/* **************************************************************** */
/* CODE TO INSERT INTO YOUR PROGRAM loop() follows: *************** */
/* Arduino loop()						*/

/*
  >>>>>>>>>>>>>>>> PUT this *BEFORE*  Arduino loop() <<<<<<<<<<<<<<<<
  void maybe_run_continuous();	// defined later on
  >>>>>>>>>>>>>>>> PUT this *BEFORE*  Arduino loop() <<<<<<<<<<<<<<<<
*/

/* Arduino loop() template:
//	void loop() {	// ARDUINO
//	  MENU.lurk_then_do();
//	  maybe_run_continuous();	// maybe display input pin state changes
//
//	  // Insert your own code here.
//	  // Do not let your program block program flow,
//	  // always return to the main loop as soon as possible.
//	}
*/

/* CODE TO INSERT INTO YOUR PROGRAM loop() ends. ****************** */
/* **************************************************************** */



/* **************************************************************** */
/* ARDUINO BOARD SPECIFIC THINGS  try to use ARDUINO MACROS: */
#include <Boards.h>


#ifndef NUM_DIGITAL_PINS		// try harder... ?
  #warning "#define NUM_DIGITAL_PINS	// FIXME: ################"

  #if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) // mega boards
    #define NUM_DIGITAL_PINS	70
  #elif defined(__SAM3X8E__)
    #ifdef PINS_COUNT	// on Arduino DUE	// FIXME: ################
      #define NUM_DIGITAL_PINS	PINS_COUNT	// FIXME: ################
    #else
      #warning "#define MISING NUM_DIGITAL_PINS	// FIXME: ################"
      #warning "#define NUM_DIGITAL_PINS 79	// FIXME: ################"
      #define NUM_DIGITAL_PINS	79 		// FIXME: DUE ################
    #endif
  #else						// FIXME: 168/328 boards ???
    #define NUM_DIGITAL_PINS	20
  #endif

  #ifndef NUM_DIGITAL_PINS
    #error "#define NUM_DIGITAL_PINS		// FIXME: ################"
  #endif
#endif


#ifndef NUM_ANALOG_INPUTS		// try harder... ?
  #warning "#define NUM_ANALOG_INPUTS	// FIXME: ################"

  #if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) // mega boards
    #define NUM_ANALOG_INPUTS	16
  #elif defined(__SAM3X8E__)
     #define NUM_ANALOG_INPUTS	16		// FIXME: DUE ################
  #else 					// FIXME: 168/328 boards ???
    #define NUM_ANALOG_INPUTS	6
  #endif

  #ifndef NUM_ANALOG_INPUTS
    #error "#define NUM_ANALOG_INPUTS		// FIXME: ################"
  #endif
#endif


#ifndef digitalPinHasPWM	// ################
  #ifdef __SAM3X8E__		// FIXME: ################
    #warning "#define MISSING digitalPinHasPWM(p)"
    #define digitalPinHasPWM(p)         ((p) >= 2 && (p) <= 13)
  #else
    #error #define digitalPinHasPWM
  #endif
#endif


/*
  DIGITAL_IOs
  number of arduino pins configured for digital I/O
  *not* counting analog inputs:
*/

#define DIGITAL_IOs	(NUM_DIGITAL_PINS - NUM_ANALOG_INPUTS)


/* **************************************************************** */
/* define some things early: */

#define ILLEGAL		-1

char PIN_digital = ILLEGAL;	// would be dangerous to default to zero
char PIN_analog = 0;		// 0 is save as default for analog pins

// Comment next line out if you do not want the analog2tone functionality:
#define has_PIEZZO_SOUND

#ifdef has_PIEZZO_SOUND
char PIN_tone = ILLEGAL;	// pin for tone output on a piezzo
#endif

// The following defaults *must* match with each other, choose one pair.
// either:
  bool extra_switch=false;	// extra functionality on digital pins
  int visible_digital_pins=DIGITAL_IOs;		// extra_switch==false
// or:
  // bool extra_switch=false;	// extra functionality on digital pins
  // int visible_digital_pins=NUM_DIGITAL_PINS;	// extra_switch==true

bool echo_switch=true;		// serial echo switch


/* ****************  info on DIGITAL pins:  **************** */

#ifdef __SAM3X8E__
  #ifndef portModeRegister
    #warning "#define *MISSING* portModeRegister(P)."
    #define portModeRegister(P) ( (volatile RwReg*)( pgm_read_word( port_to_mode_PGM + (P))) )
  #endif
#endif

/*
  pin_info_digital()
  display configuration and state of a pin:
*/
// const char pin_[] = "pin ";
const char high_[] = "HIGH";
const char low_[] = "LOW";

void pin_info_digital(uint8_t pin) {
  uint8_t mask = digitalPinToBitMask(pin);
#ifdef __SAM3X8E__	// FIXME: ################
  #warning "softboard does not run on Arduino Due yet! ################"
  Pio* const port = digitalPinToPort(pin);
#else
  uint8_t port = digitalPinToPort(pin);
#endif

#ifdef __SAM3X8E__	// FIXME: ################
  volatile RwReg* reg;

  if (port == NULL) return;

#else
  volatile uint8_t *reg;

  if (port == NOT_A_PIN) return;
#endif

  // selected sign * and pin:
  if (pin == PIN_digital )
    MENU.out('*');
  else
    MENU.space();
  MENU.out(pin_);
  MENU.out((int) pin);
  MENU.tab();

#ifdef __SAM3X8E__	// FIXME: !!! ################
  #warning "I/O pin configuration info *not implemented on Arduino DUE yet*."
  MENU.out(F("(pin_info_digital() not implemented on DUE yet)"));
#else
  // see: <Arduino.h>

  // input or output?
  reg = portModeRegister(port);
  // uint8_t oldSREG = SREG;	// let interrupt ACTIVE ;)
  // cli();
  if (*reg & mask) {		// digital OUTPUTS
    // SREG = oldSREG;
    MENU.out(F("O  "));

    // high or low?
    reg = portOutputRegister(port);
    // oldSREG = SREG;		// let interrupt ACTIVE ;)
    // cli();
    if (*reg & mask) {		    // HIGH
      // SREG = oldSREG;
      MENU.out(high_);
    } else {			    // LOW
      // SREG = oldSREG;
      MENU.out(low_);
    }
  } else {			// digital INPUTS
    // SREG = oldSREG;
    MENU.out(F("I  "));

    // pullup, tristate?
    reg = portOutputRegister(port);
    // oldSREG = SREG;		// let interrupt ACTIVE ;)
    // cli();
    if (*reg & mask) {		    // pull up resistor
      // SREG = oldSREG;
      MENU.out(F("pullup"));
    } else {			    // tri state high-Z
      // SREG = oldSREG;
      MENU.out(F("floating"));
    }
  }
#endif
  MENU.ln();
}


// display configuration and state of all digital pins:
void pins_info_digital() {
  for (uint8_t pin=0; pin<visible_digital_pins; pin++)
    pin_info_digital(pin);
}


/* continuously watch a digital pins readings: */
/*
  watch_digital_input(pin)
  continuously display digital input readings,
  whenever the input changes:
*/

#define IMPOSSIBLE	-9785	// just a value not possible on analog input

int watch_seen=IMPOSSIBLE;
void watch_digital_start(uint8_t pin) {
  watch_seen=IMPOSSIBLE;

  MENU.out(F("watching pin D"));
  MENU.out((int) pin);
  MENU.outln(F("\t\tr=stop"));
}


void watch_digital_input(int pin) {
  int value=digitalRead(PIN_digital);

  if (value != watch_seen) {
    watch_seen = value;
    MENU.out(F("*D"));  MENU.out((int) pin);
    MENU.tab();
    if (value)
      MENU.outln(high_);
    else
      MENU.outln(low_);
  }
}


bool run_watch_dI=false;

// toggle_watch()  toggle continuous digital watch display on/off.
void toggle_watch() {
  run_watch_dI = !run_watch_dI;
  if (run_watch_dI)
    watch_digital_start(PIN_digital);
  else
    MENU.ln();
}


/* ****************  info on ANALOG pins:  **************** */
/*
  bar_graph(value)
  print one value & bar graph line:
*/
const char value_[] = "value ";

void bar_graph(int value) {
  int i, length=64, scale=1023;
  int stars = ((long) value * (long) length) / scale + 1 ;


  if (value >=0 && value <= 1024) {
    MENU.out(value); MENU.tab();
    for (i=0; i<stars; i++) {
      if (i == 0 && value == 0)		// zero
	MENU.out('0');
					// middle or top
      else if \
	((i == length/2 && value == 512) || (i == length && value == scale))
	MENU.out('|');
      else
	MENU.out('*');
    }
    MENU.ln();
  } else {
    MENU.out(F("value "));
    MENU.out(value);
    MENU.OutOfRange();
  }
}


// Display analog pin name and value as number and bar graph:
void pin_info_analog(uint8_t pin) {
  if (pin == PIN_analog)
    MENU.out(F("*A"));
  else
    MENU.out(F(" A"));
  MENU.out((int) pin);
  MENU.tab();
  bar_graph(analogRead(pin));
}


/*
  pins_info_analog()
  Display analog snapshot read values and bar graphs, a line each analog input:
*/
const char analog_reads_title[] =	\
  "\npin\tvalue\t|\t\t\t\t|\t\t\t\t|";

void pins_info_analog() {
  int i, value;

  MENU.outln(analog_reads_title);

  for (i=0; i<NUM_ANALOG_INPUTS; i++)
    pin_info_analog(i);

  MENU.ln();
}


/* **************************************************************** */
/*
  Running 'continuous' display types or similar.
  Implemented without waiting allows 'run-through' programming.

  bar_graph_VU(pin):
  Continuous display changes on an analogue input.
  Display a scrolling bar graph over the readings.
  A new line is displayed as soon as the reading changes for more
  than +/- tolerance.

  tolerance can be changed by sending '+' or '-'

  run-through, don't wait...
*/
bool run_VU=false;


int VU_last=IMPOSSIBLE;

// tolerance default 0. Let the user *see* the noise...
int bar_graph_tolerance=0;

void feedback_tolerance(unsigned int tolerance) {
  MENU.out(F("tolerance "));
  MENU.outln(tolerance);
}


void VU_init(int pin) {
  VU_last=IMPOSSIBLE;	// just an impossible value

  MENU.out(F("pin\tval\t+/- set "));
  feedback_tolerance(bar_graph_tolerance);
}


// toggle_VU()  toggle continuous VU display on/off.
void toggle_VU() {
  run_VU = !run_VU;
  if (run_VU)
    VU_init(PIN_analog);
  else
    MENU.ln();
}

/*
  bar_graph_VU(pin):
  Continuous display changes exceeding a tolerance on an analogue input.
  Display a scrolling bar graph over the readings.
  A new line is displayed as soon as the reading changes for more
  than +/- tolerance.

  tolerance can be changed by sending '+' or '-'

  run-through, don't wait...
*/
void bar_graph_VU(int pin) {
  int value;

  value =  analogRead(pin);
  if (abs(value - VU_last) > bar_graph_tolerance) {
    MENU.out(F("*A"));
    MENU.out((int) pin);
    MENU.tab();
    bar_graph(value);
    VU_last = value;
  }
}


#ifdef has_PIEZZO_SOUND
/*
  analog2tone(int analogPIN, int PIN_tone):
  Simple acoustical feedback of analog readings using arduino tone()

  This very simple version is probably not compatible with pulses.

  Arduino tone() gives me some garbage below 32hz and is unusable below 16hz
  So analog2tone switches tone off for analog reading 0 and
  adds an offset of 31hz starting from analog reading 1.
*/
bool run_analog2tone = false;

//unsigned int analog2tone_tolerance = 1;		// tolerance regarding analog reading
unsigned int analog2tone_tolerance = 0;		// tolerance regarding analog reading
unsigned long analog2tone_timeInterval = 36;	// minimal time between measurements
int lastToneReading = -1;

const int unusable_tones = 31;		// tone() plays some garbage below 32hz :(
// const int unusable_tones = 15;	// tone() plays garbage below 16hz :(

void analog2tone(int analogPIN, int PIN_tone) {
  if (PIN_tone == ILLEGAL)
    return;

  static unsigned long lastToneTime = 0;
  unsigned long now = millis();

  if (now - lastToneTime >= analog2tone_timeInterval) {
    lastToneTime = now;

    int ToneReading=analogRead(analogPIN);
    if (abs(ToneReading - lastToneReading) > analog2tone_tolerance) {
      lastToneReading = ToneReading;

      // unsigned int hz = ToneReading*1;	// arbitrary factor
      unsigned int hz = ToneReading;		// arbitrary factor==1
      // arduino tone() delivers garbage below 16hz,
      // so we mend that a bit:
      if(hz) {
	hz += unusable_tones;	// start at lowest ok tone=16hz)
	tone(PIN_tone, hz);
      } else			// switch tone off for zero
	noTone(PIN_tone);
      if(MENU.verbosity >= VERBOSITY_CHATTY) {
	MENU.out(F("tone hz="));
	MENU.outln(hz);
      }
    }
  }
}


// toggle_tone()  toggle continuous sound pitch follower on/off.
void toggle_tone() {
  run_analog2tone = !run_analog2tone;
  if (run_analog2tone)
    lastToneReading = -1;
  else
    noTone(PIN_tone);

  if(MENU.verbosity >= VERBOSITY_SOME) {
    MENU.out(F("tone "));
    if (run_analog2tone)
      MENU.outln(F("ON"));
    else
      MENU.outln(F("off"));
  }
}
#endif


/*
  void maybe_run_continuous():
  Check if to continuously show/sound analog/digital input changes:
*/
void maybe_run_continuous() {
  if (run_VU)
    bar_graph_VU(PIN_analog);

#ifdef has_PIEZZO_SOUND
  if(run_analog2tone)
    analog2tone(PIN_analog, PIN_tone);
#endif

  if (run_watch_dI)
    watch_digital_input(PIN_digital);
}


void stop_continuous() {	// unused
  run_VU=false;
  run_watch_dI=false;
#ifdef has_PIEZZO_SOUND
  if(run_analog2tone)
    toggle_tone();
#endif
}


/* **************************************************************** */
/*
   Menu softboard display and reaction:
*/

/* **************************************************************** */
/* Building menu pages:

   The program defines what the menu pages display and how they react.

   The menu can have multiple pages, the selected one gets displayed.
   On start up the first one added will be selected.

   The menu pages define keys ("tokens") and the corresponding actions.

   Below the tokens of the selected/displayed page tokens of other pages
   can be configured to remain visible in groups of related pages.

   For each menu page we will need:

     hotkey		selects this menu page.
     title		identifies page to the user.
     display function	displays the menu page.
     action function	checks if it is responsible for a token,
     	    		if so do its trick and return true,
			else return false.
*/


/* ****************  softboard menu display:  **************** */
const char select_[] = "select ";
const char pinFor[] = " pin for ";
const char toWork_[] = " to work on:\t";
const char pin__[] = "pin (";


// Factored out display functions:

void _select_digital(bool key) {
  if (key)
    MENU.out(F("D="));
  MENU.out(select_);
  MENU.out(F("digital"));
  MENU.out(pinFor);
  MENU.out(F("'d, r, I, O, H, L, W'"));
}


void _select_analog(bool key) {
  if (key)
    MENU.out(F("\nA="));
  MENU.out(select_);
  MENU.out(F("analog"));
  MENU.out(pinFor);
#ifdef has_PIEZZO_SOUND
  MENU.out(F("'a, v, t'"));
#else
  MENU.out(F("'a, v'"));
#endif
}


void softboard_display() {
  _select_digital(true);
  MENU.out(toWork_);
  MENU.out(pin__);
  if (PIN_digital == ILLEGAL)
    MENU.out(F("no"));
  else
    MENU.out((int) PIN_digital);
  MENU.outln(')');
  MENU.out(F("O=OUTPUT\tI=INPUT\t\tH=HIGH\tL=LOW\tPWM: W=WRITE\td=pin info"));
  MENU.ln();

  _select_analog(true);
  MENU.out(toWork_);
  MENU.out(pin__);
  MENU.out((int) PIN_analog);
  MENU.outln(')');

#ifdef has_PIEZZO_SOUND
  MENU.out(F("watch over time:\tv=VU bar  r=read\tt=tone\tT=TonePIN ("));
  MENU.out(( int) PIN_tone);
  MENU.outln(F(")"));
#else
  MENU.outln(F("watch over time:\tv=VU bar\tr=read"));
#endif

  MENU.outln(F("\n.=all digital\t,=all analog\t;=both\tx=extra"));
}


/* ****************  softboard reactions:  **************** */

/*
  Toggle extra functionality
  * Visability of analog inputs as general digital I/O pins
    (only this one item implemented)
*/
void toggle_extra() {
  extra_switch = !extra_switch;
  if (extra_switch)
    visible_digital_pins=NUM_DIGITAL_PINS;
  else
    visible_digital_pins=DIGITAL_IOs;
}


// Factored out helper function
// digital_pin_ok()  Checks if PIN_digital has been set
bool digital_pin_ok() {
  // testing on ILLEGAL is enough in this context
  if (PIN_digital == ILLEGAL) {
    _select_digital(true);
    MENU.ln();
    return false;
  } else
    return true;
}


// bool softboard_reaction(char token)
// Try to react on token, return success flag.
const char pwm_[] = "PWM ";
const char noHw_[] = "no hardware ";
const char analogWrite_[] = "\tanalogWrite(";

bool softboard_reaction(char token) {
  long newValue;

  switch (token) {
  case 'A':
    _select_analog(false);
    MENU.tab();

    newValue = MENU.numeric_input(PIN_analog);
    if (newValue>=0 && newValue<NUM_ANALOG_INPUTS)
      PIN_analog = newValue;
    else
      MENU.OutOfRange();

    MENU.out(pin_);
    MENU.outln((int) PIN_analog);
    break;

  case 'D':
    _select_digital(false);
    MENU.tab();

    newValue = MENU.numeric_input(PIN_digital);
    if (newValue>=0 && newValue<visible_digital_pins) {
      PIN_digital = newValue;
      pin_info_digital((int) PIN_digital);
    } else
      if (newValue != ILLEGAL)
	MENU.OutOfRange();
    break;

  case 'O':
    if (digital_pin_ok()) {
      pinMode(PIN_digital, OUTPUT);
      pin_info_digital(PIN_digital);
    }
    break;

  case 'I':
    if (digital_pin_ok()) {
      pinMode(PIN_digital, INPUT);
      pin_info_digital(PIN_digital);
    }
    break;

  case 'H':  // no problem to use H here, same as the menu code.
    if (digital_pin_ok()) {
      digitalWrite(PIN_digital, HIGH);
      pin_info_digital(PIN_digital);
    }
    break;

  case 'L':
    if (digital_pin_ok()) {
      digitalWrite(PIN_digital, LOW);
      pin_info_digital(PIN_digital);
    }
    break;

  case 'W':
    if (digital_pin_ok()) {

      // can the pin do hardware PWM?
      if (!digitalPinHasPWM(PIN_digital)) {	// *no PWM* on this pin
	MENU.out(noHw_); MENU.out(pwm_);
	MENU.out(F("on ")); MENU.out(pin_);
	MENU.outln((int) PIN_digital);

	MENU.skip_numeric_input();
      } else {					// pin *can* do PWM
        MENU.out(pin_); MENU.out((int) PIN_digital);
	MENU.tab();
	MENU.out(pwm_); MENU.out(F("write "));
	newValue = MENU.numeric_input(ILLEGAL);
	if (newValue>=0 && newValue<=255) {
	  MENU.out(newValue);

	  analogWrite(PIN_digital, newValue);
	  MENU.out(F("\tanalogWrite(")); MENU.out((int) PIN_digital);
	  MENU.out(F(", ")); MENU.out(newValue);
	  MENU.outln(')');
	} else
	  MENU.OutOfRange();
      }
    }
    break;

  case 'a':
    MENU.outln(analog_reads_title);
    pin_info_analog(PIN_analog);
    break;

  case 'd':
    if (digital_pin_ok())
      pin_info_digital(PIN_digital);
    break;

  case 'v':
    toggle_VU();
    break;

  case '+':
#ifdef has_PIEZZO_SOUND
    if (!run_VU && !run_analog2tone)
      return false;    // *only* responsible if (run_VU || run_analog2tone)

    if (run_VU)
      feedback_tolerance(++bar_graph_tolerance);
    if (run_analog2tone)
      feedback_tolerance(++analog2tone_tolerance);
#else
    if (!run_VU)
      return false;    // *only* responsible if (run_VU)

    feedback_tolerance(++bar_graph_tolerance);
#endif
    break;

  case '-':
#ifdef has_PIEZZO_SOUND
    if (!run_VU && !run_analog2tone)
      return false;    // *only* responsible if (run_VU || run_analog2tone)

    if (run_VU)
      if (bar_graph_tolerance)		// only if not already zero
	feedback_tolerance(--bar_graph_tolerance);
    if (run_analog2tone)
      if (analog2tone_tolerance)	// only if not already zero
	feedback_tolerance(--analog2tone_tolerance);
#else
    if (! run_VU)
      return false;    // *only* responsible if (run_VU)

    if (bar_graph_tolerance)		// only if not already zero
      feedback_tolerance(--bar_graph_tolerance);
#endif

    break;

  case 'r':
    if (digital_pin_ok()) {
      toggle_watch();
      /*
	I do *not* call	pinMode(PIN_digital, INPUT) from here.
	Looks more flexible to me to let the user do that,
	if she wants.
      */
    }
    break;

#ifdef has_PIEZZO_SOUND
  case 't':
    toggle_tone();
    break;

  case 'T':
    if(run_analog2tone)
      toggle_tone();	// switch old tone off

    newValue = MENU.numeric_input(PIN_tone);
    if (newValue>=0 && newValue<NUM_DIGITAL_PINS) {
      PIN_tone = newValue;
      MENU.out(pin_);
      MENU.outln((int) PIN_tone);
      toggle_tone();	// switch tone ON
    } else {
      MENU.OutOfRange();
    }
    break;
#endif

  case '.':	// all digital
    MENU.ln();
    pins_info_digital();
    MENU.ln();
    break;

  case ',':	// all analog
    pins_info_analog();
    break;

  case ';':	// both ;)
    MENU.ln();
    pins_info_analog();
    pins_info_digital();
    MENU.ln();
    break;

  case 'x':	// toggle extended
    toggle_extra();
    if(MENU.verbosity >= VERBOSITY_SOME) {
      MENU.out(F("Show pins: "));
      MENU.outln(visible_digital_pins);
    }
    break;

  default:
    return 0;		// token not found in this menu
  }
  return 1;		// token found in this menu
}


/* **************************************************************** */
/* README

README softboard

   softboard  http://github.com/reppr/pulses

   Arduino hardware/software developing/testing tool.

   This version is implemented as an example for the Menu library.
   Using a library makes it easy to include in your own sketches.
   You can add more menu pages with other functionality to your sketch.


Description

   Softboard is a kind of an Arduino software breadboard.

   Simple Arduino hardware Menu interface over a serial line like USBserial.
   Softboard gives you an interface to the Arduino hardware
   by sending simple commands:
   * show infos about arduino pins and RAM usage
   * set pin mode
   * write pin states
   * read analog and digital inputs
   * write PWM values
   * continuously display changes on analog or digital inputs as bar graph
   * play analog readings back as the pitch of a tone


   Use it to test hardware like sensors, motors, things you want
   to run quick tests after setting something up on a (real) breadboard.

   Then you can use it to test software parts of your program while
   you write it and fit parts together.

   Later, when everything is working you can throw out the Menu code
   from your sketch, or keep it as an interface to your program.


   Send a sequence of one letter commands and numeric chiffres
   followed by a linebreak over a serial line (say from your computer)
   to the arduino to trigger menu actions and get infos.

   The arduino will buffer serial input byte by byte (without waiting)
   until a terminating linefeed is received as an end token.
   Any sequence of one or more '\n'  '\c'  '\0' is accepted as end token.


   Commands can read and set I/O pin configuration and states,
   switch pins on/off, read and write digital and analog values,
   switch pullup/high-z, continuously watch changing inputs over time.
   You can also connect a piezzo to a spare digital pin and listen
   to the changes on an analog input coded as pitch of a tone.


   Send '?' (and a linefeed) over serial line to see the menu.
   It displays some basic infos and shows some one-letter commands.


'run-through' cpu time friendly implementation:

   Softboard tries not to block the processor longer then needed and
   to let as much cpu time as possible to any other code possibly running.
   Menu will just run through it's code and return immediately.


Integrate your own code:

   It is easy to define Menu pages as an interface to your own code.
   Please have a look at the Menu/examples/ to see how to do this.

   You can define multiple Menu pages and define visability groups
   for pages sharing the same commands.

   Softboard is just a definition of a Menu page, you can add pages
   or delete them when compiling your program as you like.


Installation:

   Get it from http://github.com/reppr/pulses
   Put contents of the pulses/libraries/ folder into sketchbook/libraries/

   Softboard is implemented as an example of libraries/Menu/
   After a restart the Arduino GUI shows softboard under
   File >> Sketchbook >> libraries >> Menu >> softboard

	Arduino versions older than 1.0 need a fix:
	  see this thread:
	  http://code.google.com/p/arduino/issues/detail?id=604&start=200

	    short version (adapt to your arduino version)
	      edit file arduino-0022/hardware/arduino/cores/arduino/wiring.h
	      comment out line 79 (round macro)
	      #define round(x)     ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))
	      tested on arduino-0023

	For older arduino versions you also have to rename the sketch file
	from softboard.ino to softboard.pde


How it works:

   You communicate with the arduino over a serial connection
   that could be a real serial line or an usb cable.
   The library should be able to deal with other stream interfaces, btw.

   The minimalistic menu shows you one letter commands and listens to
   your input. Serial inputs are buffered until you send a linefeed.
   Then your inputs (commands and numbers as sequences of chiffres)
   will be read and acted upon.


Configure your terminal program:

   Do configure your terminal emulation program to send some sort of
   line ending code, usual culprits should work.

   Set arduino baud rate by editing the line starting with
   #define BAUDRATE in softboard.ino
   and set it to the same value in your terminal software.

   So if for example you use the Arduino 'Serial Monitor' window
   check that it *does* send 'Newline' (in the bottom window frame)
   and set baud rate to the same value as BAUDRATE on the arduino.


   Send '?' (and a linefeed) over serial line to see the menu.
   It displays some basic infos and shows some one-letter commands.

   'e' toggle serial echo.
   'x' toggle visibility of analog inputs as extra digital I/O pins.



Some examples:

       Always let your terminal send a linefeed after the example string
       to trigger the menu actions.

       Spaces in the strings can be omitted.



   Example 1:  'D13 OH' switch LED on   (D select pin, O output, H high)
               'L'      off again       (L low)


   Example 2:  writing high to an input activates internal pullup resistor

	       'D13 IH' pullup LED pin  (D select pin, I input, H high)

                                        LED glows at low level
               'O'      LED as OUTPUT   now the LED is on


   Example 3:  Watch an analog input like a VU meter, changing over time.
               See electric noise on unconnected floating A0 input
               scrolling over your serial terminal.
               (Touch the input if there is no visible signal.)

               Or connect a sensor to the input and explore its data...

               'A0 v'      A=select pin (both analog or digital)
                           v=display analog read values and bar graphs

			   Stop it by pressing 'v' again.


*A0	77	*****
*A0	63	****
*A0	74	*****
*A0	84	******
*A0	115	********
*A0	165	***********
*A0	237	***************
*A0	289	*******************
*A0	285	******************
*A0	241	****************
*A0	159	**********
*A0	97	*******
*A0	70	*****
*A0	63	****
*A0	76	*****
*A0	86	******
*A0	123	********
*A0	171	***********
*A0	248	****************
*A0	294	*******************
*A0	280	******************
*A0	227	***************
*A0	144	**********
*A0	87	******
*A0	66	*****



   Example 4:  Listen to an analog input value changing the pitch of a piezzo tone.
   	       This example does a similar thing as #3 playing an analog value back
	       as the pitch of a sound on a Piezzo connected from pin 8 to ground:

   	       'A0 T8'	Select analog input and the pin for the piezzo.
	       't'	toggles the tone on and off.

	       You will hear a sound on the piezzo now. The pitch will change
	       according to the reading on the analog pin.


   Example 5:  ','  Display snapshot values of all analog inputs.

                        [fixed font only]

pin     value   |                               |                               |
*A0     609     ***************************************
 A1     504     ********************************
 A2     451     *****************************
 A3     398     *************************
 A4     383     ************************
 A5     409     **************************



   Example 6:  '.'  Info on all digital pin's I/O configuration and state.

 pin 0	I  floating
 pin 1	I  floating
 pin 2	O  LOW
 pin 3	O  HIGH
 pin 4	I  floating
*pin 5	I  pullup       // * indicates the selected pin
 pin 6	I  floating
 pin 7	I  floating
 pin 8	I  floating
 pin 9	I  floating
 pin 10	I  floating
 pin 11	I  floating
 pin 12	I  floating
 pin 13	I  floating

   btw: The example output was generated after 'D2OLD3OHD5IH'.
        'D2OL D3OH D5IH' is the same, but easier to read.



   Copyright © Robert Epprecht  www.RobertEpprecht.ch   GPLv2

   http://github.com/reppr/pulses

*/
/* **************************************************************** */

#endif
