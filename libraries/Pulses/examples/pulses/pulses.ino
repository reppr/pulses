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
void init_chord2345(int sync);		// defined later on
void init_rhythm_1(int sync);		// defined later on
void init_rhythm_2(int sync);		// defined later on
void init_rhythm_3(int sync);		// defined later on
void init_rhythm_4(int sync);		// defined later on
void setup_jiffles0(int sync);		// defined later on
void alive_pulses_info_lines();		// defined later on


// FIXME: ################
#ifndef CLICK_PULSES		// default number of click frequencies
  #ifdef ESP8266
    #define CLICK_PULSES	4       // default number of click frequencies
  #else
    #define CLICK_PULSES	6       // default number of click frequencies
  #endif
#endif
uint8_t click_pin[CLICK_PULSES];	// ################


/* **************************************************************** */
// PULSES

#ifdef ARDUINO		// on ARDUINO
  #ifdef __SAM3X8E__
const int pl_max=32;		// could be more on DUE ;)
  #else
    #ifdef ESP8266
const int pl_max=16;		// ESP8266
    #else
      #ifdef __AVR_ATmega328P__
const int pl_max=12;		// saving RAM on 328P
      #else
const int pl_max=16;		// default i.e. mega boards
      #endif
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

#if defined(__AVR_ATmega32U4__) || defined(ESP8266)
  /* on ATmega32U4		Leonardo, Mini, LilyPad Arduino USB
     to be able to use Serial.print() from setup()
     we *must* do that before:
  */
  while (!Serial) { ;}		// wait for Serial to open
#endif

  MENU.outln(F("http://github.com/reppr/pulses/\n"));

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

    #ifdef ESP8266	// configure PINs here
      /*
        // on NodeMCU ESP8266 board Arduino defines digital pins
        static const uint8_t D0   = 16;
        static const uint8_t D1   = 5;
        static const uint8_t D2   = 4;
        static const uint8_t D3   = 0;
        static const uint8_t D4   = 2;
        static const uint8_t D5   = 14;
        static const uint8_t D6   = 12;
        static const uint8_t D7   = 13;
        static const uint8_t D8   = 15;
        static const uint8_t D9   = 3;
        static const uint8_t D10  = 1;
      */
      // on ESP8266
      click_pin[0] = D0;		// configure PINs here
      click_pin[1] = D1; 		// configure PINs here
      click_pin[2] = D6; 		// configure PINs here
      click_pin[3] = D7; 		// configure PINs here
   #else // *not* on ESP8266 i.e. Arduino
      click_pin[0] = 2;			// configure PINs here
      click_pin[1] = 3; 		// configure PINs here
      click_pin[2] = 4; 		// configure PINs here
      click_pin[3] = 5; 		// configure PINs here
      click_pin[4] = 6; 		// configure PINs here
      click_pin[5] = 7; 		// configure PINs here
    #endif

    init_click_pins();		// set OUTPUT, LOW
  #endif


  // time and pulses *must* get initialized before setting up pulses:
  PULSES.init_time();		// start time
  PULSES.init_pulses();		// init pulses


  // for a demo one of these could be called from here:
  // MENU.ln(); init_chord2345(0);
  // init_rhythm_1(1);
  // init_rhythm_2(5);
  // MENU.ln(); init_rhythm_3(3);
  // init_rhythm_4(1);
  MENU.ln(); setup_jiffles0(1);


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
// playing with chords:
void init_chord2345(int sync) {
  // By design click pulses *HAVE* to be defined *BEFORE* any other pulses:
  unsigned long factor, divisor=256L;
  const unsigned long scaling=1L;
  const unsigned long unit=scaling*time_unit;
  struct time now;

  MENU.out('chord2345'); MENU.out(' '); MENU.out(sync_); MENU.outln(sync);

  init_click_pulses();

  PULSES.get_now();
  now=PULSES.now;

  factor=2;
  setup_click_synced(now, unit, factor, divisor, sync);

  factor=3;
  setup_click_synced(now, unit, factor, divisor, sync);

  factor=4;
  setup_click_synced(now, unit, factor, divisor, sync);

  if(CLICK_PULSES>3) {
    factor=5;
    setup_click_synced(now, unit, factor, divisor, sync);
  }

  PULSES.fix_global_next();
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
  const unsigned long scaling=1L;
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

  if(CLICK_PULSES>3) {
    factor=5;
    setup_click_synced(now, unit, factor, divisor, sync);
  }

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

  MENU.out_flags_();
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
const char pIn_[] = "pin ";	// pin_ is taken by softboard...
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

  MENU.out_flags_();
  MENU.outBIN(PULSES.pulses[pulse].flags, 8);
  MENU.ln();

  MENU.out(pIn_);  MENU.out((int) PULSES.pulses[pulse].char_parameter_1);
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

void print_selected_pulses() {
  const int hex_pulses=min(pl_max,16);	// displayed as hex chiffres

  MENU.out_selected_();
  for (int pulse=0; pulse<hex_pulses; pulse++) {
    if (selected_pulses & (1 << pulse))
      MENU.out_hex_chiffre(pulse);
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
  switch (dest) {
  case CODE_PULSES:
    print_selected_pulses();
    break;

  case CODE_TIME_UNIT:
    MENU.out_selected_();
    MENU.outln(timeUnit);
    break;
  }
}


// info_select_destination_with()
const char selectDestinationInfo[] =
  "SELECT DESTINATION for '= * / s K P n c j :' to work on:\t";
const char selectPulseWith[] = "Select pulse with ";
const char selectAllPulses[] =
  "\na=select *all* click pulses\tA=*all* pulses\tl=alive clicks\tL=all alive\tx=none\t~=invert selection";
const char uSelect[] = "u=select ";
const char selected__[] = "\t(selected)";
const char pulses_[] = "pulses ";

void info_select_destination_with(bool extended_destinations) {
  MENU.out(pulses_);
  MENU.out(PULSES.get_pl_max());
  MENU.tab();
  MENU.out(selectDestinationInfo);
  print_selected();
  MENU.ln();
  MENU.out(selectPulseWith);

  // FIXME: use 16 here, when reaction will be prepared for a,b,c,d,e,f too.
  MENU.out_ticked_hexs(min(pl_max,10));

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
  MENU.outln(F("http://github.com/reppr/pulses/\n"));
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
  unsigned long bitmask;

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
    bitmask=0;
    for (int pulse=0; pulse<pl_max; pulse++)
      bitmask |= (1 << pulse);
    selected_pulses &= bitmask;
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
// include softboard on arduinos:
#ifdef ARDUINO
//  #include "libraries/Pulses/examples/pulses/softboard_page.ino"
//  #include <examples/softboard/softboard_page.ino>
#else
  #warning "Not compiling softboard_page on PC."
#endif

/* **************************************************************** */
/* README_pulses


README_pulses

Testing Pulses library in an early stage



   Copyright © Robert Epprecht  www.RobertEpprecht.ch   GPLv2

   http://github.com/reppr/pulses


*/
/* **************************************************************** */
