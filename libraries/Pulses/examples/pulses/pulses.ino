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

#include "pulses_systems_and_boards.h"


/* **************** Menu **************** */
#include <Menu.h>

//  Menu(int size, int menuPages, int (*maybeInput)(void), STREAMTYPE & port);
/*
  This version definines the menu INPUT routine int men_getchar();
  in the *program* not inside the Menu class.
*/
#ifdef ARDUINO	// FIXME: why doesn't it work from pulses_systems_and_boards.h???
  #define MENU_OUTSTREAM	Serial

  int men_getchar() {
    if (!Serial.available())	// ARDUINO
      return EOF;

    return Serial.read();

  }
#endif

/* BAUDRATE for Serial:	uncomment one of the following lines:	*/
#define BAUDRATE	115200		// works fine here
//#define BAUDRATE	57600
//#define BAUDRATE	38400
//#define BAUDRATE	19200
//#define BAUDRATE	9600		// failsafe
//#define BAUDRATE	31250		// MIDI

#ifndef ARDUINO		// Linux PC test version  *NOT SUPPORTED*
  #include <Menu/Menu.h>
  #include <Pulses/Pulses.h>

  #include "Pulses/Pulses.cpp"		// why that?
  #include "Menu/Menu.cpp"		// why that?

  #define MENU_OUTSTREAM	cout

  int men_getchar() {
    return getchar();		// c++ Linux PC test version
  }
#endif

Menu MENU(32, 3, &men_getchar, MENU_OUTSTREAM);


/* **************** Pulses **************** */
#include <Pulses.h>
Pulses PULSES(pl_max);

/* **************** Harmonical **************** */
#include <Harmonical.h>

Harmonical HARMONICAL(3628800uL);	// old style for a first test

/* **************************************************************** */
// some #define's:
#define ILLEGAL		-1
#define ILLEGAL_PIN	255

// defined later on:
bool maybe_stop_sweeping();
void maybe_run_continuous();
void menu_pulses_display();
bool menu_pulses_reaction(char token);
void display_action(int pulse);
void do_jiffle (int pulse);
void do_throw_a_jiffle(int pulse);
void init_click_pins();
void flagged_pulses_info_lines();
void setup_jiffle128(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync);
void init_div_123456(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync);
void setup_jiffles0(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync); // Frogs
void setup_jiffles2345(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync);
void init_123456(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync);
void init_chord_1345689a(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync);
void init_rhythm_1(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync);
void init_rhythm_2(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync);
void init_rhythm_3(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync);
void init_rhythm_4(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync);
void setup_jifflesNEW(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync);;
void init_pentatonic(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync);



/* **************************************************************** */
uint8_t click_pin[CLICK_PULSES];

/* **************************************************************** */
#ifdef ESP8266	// hope it works on all ESP8266 boards, FIXME: test
  // to switch WiFi off I evaluate different methods:
  // activate *one* of these methods:
  #define WIFI_OFF_hackster
  //#define WIFI_OFF_mysensors

  #ifdef WIFI_OFF_hackster
    // see: https://www.hackster.io/rayburne/esp8266-turn-off-wifi-reduce-current-big-time-1df8ae

    #define FREQUENCY    80	// valid 80, 160
    //#define FREQUENCY    160	// valid 80, 160

    #include "ESP8266WiFi.h"  // after that min() and max() do not work any more. Say: std::min() std::max()
    extern "C" {
      #include "user_interface.h"
    }
  #endif

  #ifdef WIFI_OFF_mysensors
    // see: https://forum.mysensors.org/topic/5120/esp8266-with-wifi-off/3
    #include <ESP8266WiFi.h>  // after that min() and max() do not work any more. Say: std::min() std::max()
  #endif
#endif


// have a RAM jiffletab?
// unsigned int jiffle_data[JIFFLE_RAM_SIZE]
#ifdef ESP8266	// we have a lot of RAM
  #define JIFFLE_RAM_SIZE	256*3+1

  // ratios:
  #define RATIOS_RAM_SIZE	256*2+1
  #ifndef RATIOS_RAM_SIZE
    #define RATIOS_RAM_SIZE 9*2+1	// small buffer might fit on simple hardware
  #endif
  unsigned int ratios_data[RATIOS_RAM_SIZE] = {0};
  unsigned int ratios_data_length = RATIOS_RAM_SIZE;
  unsigned int ratios_write_index=0;
  unsigned int *ratios=ratios_data;

// FIXME: does not belong here
  unsigned int ratios_quot[] = {1,1, 1,2, 1,3, 1,4, 1,5, 1,6, 1,7, 1,8, 0,0};  // zero terminated
  unsigned int ratios_int[]  = {1,1, 2,1, 3,1, 4,1, 5,1, 6,1, 7,1, 8,1, 0,0};  // zero terminated
  unsigned int european_pentatonic[] = {1,1, 8,9, 4,5, 2,3, 3,5, 1,2, 4,9, 2,5, 1,3, 3,10,  1,4, 0,0 };  // zero terminated
  unsigned int pentatonic_minor[] = {1,1, 5,6, 3,4, 2,3, 5*2,6*3, 1,2, 5,12, 3,8, 1,3, 5,6*3, 1,4, 0,0 };  // zero terminated
  // nice first try with "wrong" note:
  //  unsigned int mimic_japan_pentatonic[] = {1,1, 8,9, 5,6, 2,3, 8*2,9*3, 1,2, 8,9*2, 5,12, 2,6, 8,9*3, 1,4, 0,0 };  // zero terminated
// second try:
  unsigned int mimic_japan_pentatonic[] = {1,1, 8,9, 5,6, 2,3, 2*15,3*16, 1,2, 8,9*2, 5,12, 2,6, 8,9*3, 1,4, 0,0 };  // zero terminated
#endif

// editing jiffle data
// if we have enough RAM, we work in an int array[]
// pre defined jiffletabs can be copied there before using and editing
#ifndef JIFFLE_RAM_SIZE
  #define JIFFLE_RAM_SIZE 9*3+1	// small buffer might fit on simple hardware
#endif
unsigned int jiffle_data[JIFFLE_RAM_SIZE];
unsigned int jiffle_data_length = JIFFLE_RAM_SIZE;
unsigned int jiffle_write_index=0;
unsigned int *jiffle=jiffle_data;

unsigned int harmonics4[] = {1,1,1024, 1,2,1024, 1,3,1024, 1,4,1024, 0};	// magnets on strings experiments
unsigned int ting1024[] = {1,4096,64, 1,1024,128, 1,1024*3,128, 1,1024*2,128, 1,1024*8,64, 1,1024,64, 0}; // magnet strings experiments 2
// unsigned int ting1024[] = {1,4096,64, 1,1024,128, 0};			// magnet strings experiments 2
unsigned int ting4096[] = {1,4096,1024, 0};			// magnet strings experiments 2
// unsigned int arpeggio4096[] = {4,4096*4,128, 4,4096*3,128, 4,4096*4,128, 4,4096*5,128, 4,4096*6,64, 0}; // pezzo strings E16
unsigned int arpeggio4096[] = {64,4096,1, 1,4096,256,  64,4096*2,1, 1,4096*2,256, 64,4096*3,1, 1,4096*3,256,  64,4096*4,1, 1,4096*4,256, 64,4096*5,1, 1,4096*5,256, 64,4096*6,1, 1,4096*6,256, 64,4096*8,1, 1,4096*8,256, 0};
// unsigned int halfway[] = {1,2,1, 1,4,1, 1,8,1, 1,16,1, 1,32,1, 1,64,1, 1,128,1, 1,256,1, 1,512,1, 1,1024,1, 1,2048,1, 1,4096,1, 1,8192,1, 1,16384, 0};


/* **************************************************************** */
// variables:

int sync=1;			// syncing edges or middles of square pulses
unsigned long multiplier=1;
unsigned long divisor=1;

int experiment=-1;
int voices=CLICK_PULSES;

unsigned long selected_pulses=0L;	// pulse bitmask for user interface

// #ifndef INTEGER_ONLY		FIXME: make a version without tuning and no floats
#ifdef IMPLEMENT_TUNING		// implies floating point
  #include <math.h>

// first try, see sweep_click_0()
/* tuning *= detune;
  called detune_number times
  will rise tuning by an octave	*/
  int sweep_up=1;	// sweep_up==0 no sweep, 1 up, -1 down
  double tuning=1.0;
  double detune_number=4096.0;
  double detune=1.0 / pow(2.0, 1/detune_number);

// second try, see sweep_click()
  unsigned long ticks_per_octave=10000000L;		// 10 seconds/octave
  // unsigned long ticks_per_octave=60000000L;		//  1 minute/octave
  // unsigned long ticks_per_octave=60000000L*10L;	// 10 minutes/octave
  // unsigned long ticks_per_octave=60000000L*60L;	//  1 hour/octave
  // unsigned long ticks_per_octave=60000000L*60L*24;	//  1 day/octave	FIXME: (not tested yet)

// re-implement, see tuned_sweep_click()
// PULSES.ticks_per_octave = ticks_per_octave;
//

#endif
// #endif


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
      // on ESP8266	// old 6 click  configuration:
      //	click_pin[0] = D0;	// D0 = 16
      //	click_pin[1] = D1;	// D1 = 5
      //	click_pin[2] = D6;	// D6 = 12
      //	click_pin[3] = D7;	// D7 = 13
      //	click_pin[4] = D4;	// D4 = 2
      //	click_pin[5] = D5;	// D5 = 14

      // 8 clicks, using D1 to D8:
      click_pin[0] = D1;	// D1 = 5
      click_pin[1] = D2;	// D2 = 4
      click_pin[2] = D3;	// D3 = 0
      click_pin[3] = D4;	// D4 = 2	must be HIGH on boot	board blue led on LOW
      click_pin[4] = D5;	// D5 = 14
      click_pin[5] = D6;	// D6 = 12
      click_pin[6] = D7;	// D7 = 13	LED?
      click_pin[7] = D8;	// D8 = 15

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

  #ifdef ESP8266	// hope it works on all ESP8266 boards, FIXME: test
    #ifdef WIFI_OFF_hackster
      // see: https://www.hackster.io/rayburne/esp8266-turn-off-wifi-reduce-current-big-time-1df8ae
      WiFi.forceSleepBegin();                  // turn off ESP8266 RF
      delay(1);                                // give RF section time to shutdown

      #ifdef FREQUENCY
	system_update_cpu_freq(FREQUENCY);
      #endif
    #endif

    #ifdef WIFI_OFF_mysensors
      // see:	https://forum.mysensors.org/topic/5120/esp8266-with-wifi-off/3
      WiFi.disconnect();
      WiFi.mode(WIFI_OFF);
      WiFi.forceSleepBegin();
      delay(1);
    #endif
  #endif

  // time and pulses *must* get initialized before setting up pulses:
  PULSES.init_time();		// start time
  PULSES.init_pulses();		// init pulses

#ifdef IMPLEMENT_TUNING		// implies floating point
  PULSES.ticks_per_octave = ticks_per_octave;
#endif

  MENU.ln();

  /* ****************  DEMO SETUPS  **************** */
  // for a demo one of these could be called from here:

  // void setup_jiffle128(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync)
  // setup_jiffle128(false, CLICK_PULSES, 2, 1, 15);	// 12345678 slow beginning of voices, nice

  // init_div_123456(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync);
  // init_div_123456(false, CLICK_PULSES, 1, 1, 0);

  // setup_jiffles0(false, CLICK_PULSES, 2, 3, 1);	// setup for ESP8266 Frog Orchester

  //       void setup_jiffles2345(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync);
  // setup_jiffles2345(0, CLICK_PULSES, 1, 2, 0);

  // init_123456(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync);
  // init_123456(false, CLICK_PULSES, 1, 1, 0);

  // init_chord_1345689a(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync);
  // init_chord_1345689a(0, CLICK_PULSES, 1, 1, 0);

  // init_rhythm_1   (0, CLICK_PULSES, 1, 6*7, 1);
  // init_rhythm_2(0, CLICK_PULSES, 1, 1, 5);
  // init_rhythm_3(0, CLICK_PULSES, 1, 1, 3);
  // init_rhythm_4(0, CLICK_PULSES, 1, 7*3, 1);

  // void setup_jifflesNEW(int sync, unsigned int multiplier, unsigned int divisor);
  // setup_jifflesNEW(0, CLICK_PULSES, 3, 1, 3);

  // init_pentatonic(0, CLICK_PULSES, 1, 1, 1);	// Harmonical Frogs Choir	Frogs set 2
  // init_pentatonic(0, CLICK_PULSES, 1, 6, 1);	// 8 strings on piezzos 2016-12-12

  // 2 strings setup 2016-12-15 sweep and drone on piezzos
  //  multiplier=1;
  //  divisor=5800;
  //  en_click(0);
  //  en_sweep_click(1);
  //  selected_pulses = 3;
  //  reset_and_edit_selected();
  //  activate_selected_synced_now(sync);	// FIXME:	something's wrong :(	################


//	  // testing ratios, prepare_ratios():
//	  // void prepare_ratios(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync, unsigned int *ratios)
//	  ratios = pentatonic_minor;
//	  // ratios = european_pentatonic;
//	  // ratios = ratios_quot;
//	  // ratios = ratios_int;
//	  selected_pulses=~0;
//	  // or: if nothing is selected all pulses with flags==0 get selected.
//	  int prepared = prepare_ratios(false, 8, 1, 1, 0, ratios);
//	  if (prepared != 8)
//	    MENU.out(F("prepared ")); MENU.out(prepared); MENU.slash(); MENU.outln(voices);
//	  //  selected_pulses=0;


// working on:
// prepare_magnets(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync) {
// pentatonic steel strings on 8 magnets
//			  ratios = pentatonic_minor;


/* ****************  END DEMO SETUPS  **************** */


  PULSES.fix_global_next();	// we *must* call that here late in setup();

  // informations about alive pulses:
  MENU.ln();
  flagged_pulses_info_lines();
}


// stress_emergency:  looks like the value does not matter too much
unsigned int stress_emergency=4096;	// high value seems appropriate
unsigned int stress_count=0;

void loop() {	// ARDUINO
  #ifdef ESP8266	// hope it works on all ESP8266 boards, FIXME: test
    #ifdef WIFI_OFF_hackster
      // see: https://www.hackster.io/rayburne/esp8266-turn-off-wifi-reduce-current-big-time-1df8ae
      // pat the watchdog timer
      wdt_reset();
    #endif
  #endif

  /* calling check_maybe_do() in a while loop
     increases timing priority of PULSES over the MENU.
     It has little influence on low system load levels
     but when on stress PULSES gets up to *all* available time.
     The intention is to have PULSES continue functioning and
     let the UI starve, when there is not enough time for everything.
  */
  stress_count=0;
  while (PULSES.check_maybe_do()) {	// in stress PULSES get's *first* priority.
    if (++stress_count >= stress_emergency) {
      // EMERGENCY
      // kill fastest pulse might do it? (i.e. fast sweeping up)

      if (MENU.maybe_display_more()) {
	MENU.out((int) PULSES.fastest_pulse());
	MENU.out(F(" deactivated  "));
      }

      PULSES.deactivate_pulse(PULSES.fastest_pulse());

      stress_count=0;				// seems best, maybe
      // stress_count = stress_emergency / 2;	// FIXME: further tests	################
    }
  }

  if(! MENU.lurk_then_do())		// MENU comes second in priority.
    {					// if MENU had nothing to do, then
      tuning = PULSES.tuning;	// FIXME: workaround for having all 3 sweep implementations in parallel
      if (!maybe_stop_sweeping())		// low priority control sweep range
	if(! maybe_display_tuning_steps())	// low priority tuning steps info
	  maybe_run_continuous();	// lowest priority:
					// maybe display input state changes.
    }
} // ARDUINO loop()

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
  flagged_pulses_info_lines();

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
      if (!maybe_stop_sweeping())		// low priority control sweep range
	if(! maybe_display_tuning_steps()) {	// low priority tuning steps info
	  ;
#ifdef ARDUINO
          maybe_run_continuous();		//    lowest priority:
#endif					//    maybe display input state changes.
      }
    }
  }

} // PC main()

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
// By design click pulses *HAVE* to be defined *BEFORE* any other pulses:


void click(int pulse) {	// can be called from a pulse
  digitalWrite(PULSES.pulses[pulse].char_parameter_1, PULSES.pulses[pulse].counter & 1);
}


void sweep_click(int pulse) {	// can be called from a pulse
  double period = PULSES.pulses[pulse].period.time;
  double detune_number = ticks_per_octave / PULSES.pulses[pulse].period.time;
  double detune = 1 / pow(2.0, 1/detune_number);

  switch (sweep_up) {
  case 1:
    period *= detune;
    tuning *= detune;
    break;
  case -1:
    period /= detune;
    tuning /= detune;
    break;
  }

  PULSES.pulses[pulse].period.time = period;
  // PULSES.pulses[pulse].period.overflow = 0;
  click(pulse);
}


void tuned_sweep_click(int pulse) {	// can be called from a pulse
  double detune_number = PULSES.ticks_per_octave / PULSES.pulses[pulse].period.time;
  double detune = pow(2.0, 1/detune_number);

  switch (sweep_up) {
  case 1:
    PULSES.tuning *= detune;
    break;
  case -1:
    PULSES.tuning /= detune;
    break;
  }

  // PULSES.pulses[pulse].period.overflow = 0;
  click(pulse);
}


// first try: octave is reached by a fixed number of steps:
void sweep_click_0(int pulse) {	// can be called from a sweeping pulse
  PULSES.pulses[pulse].period.time = PULSES.pulses[pulse].ulong_parameter_1 * tuning;
  PULSES.pulses[pulse].period.overflow = 0;
  click(pulse);

  switch (sweep_up) {
  case 1:
    tuning *= detune;
    break;
  case -1:
    tuning /= detune;
    break;
  }
}



double low_sweep_limit = 0.0;	// no limits, please ;)
				// well there's still zero and resource limitations...

double high_sweep_limit = 0.0;	// no limits, please ;)
				// well there's still zero and resource limitations...

void tuning_info() {
  MENU.out(F("tuning ")); MENU.out(PULSES.tuning);
  MENU.tab();
  MENU.out(F("slowest ")); MENU.out(low_sweep_limit);
  MENU.tab();
  MENU.out(F("fastest 1/")); MENU.out((double) 1/high_sweep_limit);
}


void sweep_info() {
  MENU.out(F("sweep "));
  switch (sweep_up) {
  case 1:
    MENU.out(F("up"));
    break;
  case -1:
    MENU.out(F("down"));
    break;
  case 0:
    MENU.out(F("off"));
    break;
  }
  MENU.tab();
  tuning_info();
  MENU.ln();
}


int is_octave(unsigned int integer) {
  int octave=0;
  for (; octave<32; octave++) {
    if (integer == 1 << octave)
      return octave;
  }

  return -1;
}


// inform user when tuning crosses integer or simple rational value
bool maybe_display_tuning_steps() {
  static int last_tuning_step=-1;  // impossible default
  static int last_fraction=-1;	   // impossible default
  static double last_tuning=1.0;

  bool did_something = false;

  if (last_tuning != tuning) {
    last_tuning = tuning;
    struct time now = PULSES.get_now();

    int tuning_step = tuning;			// integer part
    int current_fraction = 1.0/(double) tuning;	// integer part
    bool tuning_up = (tuning > last_tuning);

    if (tuning_step != last_tuning_step) {	// integer part changed
      last_tuning_step = tuning_step;
      MENU.out(F("tuning * "));
      MENU.out(tuning_step);
      MENU.space();
      if (is_octave((int) tuning_step) != -1)
	MENU.out(F("<<"));
      MENU.tab();
      display_realtime_sec(now); MENU.tab();
      sweep_info();
      did_something = true;
    }

    if (current_fraction != last_fraction) {
      last_fraction = current_fraction;
      MENU.out(F("tuning 1/"));
      MENU.out(current_fraction);
      MENU.space();
      if (is_octave((int) current_fraction) != -1)
	MENU.out(F("<<"));
      MENU.tab();
      display_realtime_sec(now); MENU.tab();
      sweep_info();
      did_something = true;
    }
  } // tuning has changed

  return did_something;
}


bool maybe_stop_sweeping() {
  if (sweep_up == 0)
    return false;

  if (low_sweep_limit != 0.0) {
    if (tuning > low_sweep_limit) {
      sweep_up=0;
      tuning=low_sweep_limit;
      MENU.out(F("sweep stopped "));
      MENU.outln(tuning);
      return true;
    }
  }

  if (high_sweep_limit != 0.0) {
    if (tuning < high_sweep_limit) {
      sweep_up=0;
      tuning=high_sweep_limit;
      MENU.out(F("sweep stopped "));
      MENU.outln(tuning);
      return true;
    }
  }

  return false;
}


void display_fraction(struct fraction *f) {
  MENU.out((*f).multiplier);
  MENU.slash();
  MENU.out((*f).divisor);
}


/* **************************************************************** */
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


const char deactivatedAllPulses[] = "deactivated all pulses";
const char noFreePulses[] = "no free pulses";

void deactivate_all_clicks() {
  for (int pulse=0; pulse<CLICK_PULSES; pulse++)
    PULSES.pulses[pulse].flags &= ~ACTIVE;

  PULSES.fix_global_next();

  if (MENU.verbosity)
    MENU.outln(deactivatedAllPulses);
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


// make an existing pulse to a sweep click pulse:
void en_sweep_click(int pulse)
{
  if (pulse != ILLEGAL) {
    en_click(pulse);
    PULSES.pulses[pulse].periodic_do = (void (*)(int)) &sweep_click;
  }
}


// make an existing pulse to a sweep_click_0 pulse:
void en_sweep_click_0(int pulse)
{
  if (pulse != ILLEGAL) {
    en_click(pulse);
    PULSES.pulses[pulse].ulong_parameter_1 = PULSES.pulses[pulse].period.time;
    PULSES.pulses[pulse].periodic_do = (void (*)(int)) &sweep_click_0;
  }
}


void en_tuned_sweep_click(int pulse)
{
  if (pulse != ILLEGAL) {
    en_click(pulse);
    PULSES.activate_tuning(pulse);
    PULSES.pulses[pulse].periodic_do = (void (*)(int)) &tuned_sweep_click;
  }
}


int setup_click_synced(struct time when, unsigned long unit, unsigned long multiplier,
		       unsigned long divisor, int sync) {
  int pulse= PULSES.setup_pulse_synced(&click, ACTIVE, when, unit, multiplier, divisor, sync);

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
  float seconds=((float) ((signed long) duration.time) / 1000000.0);

  if (duration.overflow != ~0)		// ILLEGAL	FIXME: hmm? what about multiple negative overflows?
    seconds += overflow_sec * (float) duration.overflow;
    // seconds += overflow_sec * (float) ((signed long) duration.overflow);

  float scratch = 1000.0;
  while (scratch > std::max(abs(seconds), (float) 1.0)) {	// (float) for Linux PC tests, "std::" for ESP8266
    MENU.space();
    scratch /= 10.0;
  }

  if (seconds >= 0)	// line up with automatic '-' sign
    MENU.out('+');

  MENU.out(seconds , 6);
  MENU.out('s');

  return seconds;
}


// time unit that the user sees.
// it has no influence on inner working, but is a menu I/O thing only
// the user sees and edits times in time units.
// unsigned long time_unit = 100000L;		// scaling timer to 10/s 0.1s

// I want time_unit to be dividable by a semi random selection of small integers
// avoiding rounding errors as much as possible.
//
// I consider factorials as a good choice:
// unsigned long time_unit =    40320L;		// scaling timer to  8!, 0.040320s
// unsigned long time_unit =   362880L;		// scaling timer to  9!, 0,362880s
unsigned long time_unit =  3628800L;		// scaling timer to 10!, 3.628800s
// const char arrays[]  to save RAM:
const char timeInfo[] = "*** TIME info\t";
const char ticOfl[] = "tic/ofl ";
const char now_[] = "now  ";
const char next_[] = "next  ";

void display_real_ovfl_and_sec(struct time then) {
  MENU.out(ticOfl);
  MENU.out(then.time);
  MENU.slash();
  MENU.out((signed long) then.overflow);
  MENU.space();
  MENU.out('=');
  display_realtime_sec(then);
}


void display_now() {
  MENU.out(now_);
  PULSES.get_now();
  display_real_ovfl_and_sec(PULSES.now);
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
const char sync_[] = "sync ";

//   init_div_123456(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync);
void init_div_123456(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync) {
//	  const unsigned long divisor=1L;
  unsigned long unit=multiplier*time_unit;
  unit /= divisor;

  display_name5pars("init_div_123456", inverse, voices, multiplier, divisor, sync);

  struct time now;
  PULSES.get_now();
  now=PULSES.now;

  int integer;
  if (! inverse) {	// low pin number has low note
    for (integer=1; integer<=voices; integer++) {
      setup_click_synced(now, unit, 1, integer, sync);
      MENU.out(integer);
    }
  } else {	// inverse: low pin number has high frequency
    for (integer=voices; integer>0; integer--) {
      setup_click_synced(now, unit, 1, integer, sync);
      MENU.out(integer);
    }
  }
  MENU.ln();

  PULSES.fix_global_next();
}

void init_123456(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync) {
  unsigned long unit = multiplier*time_unit;
  unit /= divisor;

  display_name5pars("init123456", inverse, voices, multiplier, divisor, sync);

  struct time now;
  PULSES.get_now();
  now=PULSES.now;

  divisor=2L*36L;
  int integer;
  if (! inverse) {	// bottom down/up click-pin mapping
    // low pin number has longest period
    for (integer=voices; integer>0; integer--) {
      setup_click_synced(now, unit, integer, divisor, sync);
      MENU.out(integer);
    }
  } else {
    // low pin number has shortest period
    for (integer=1; integer<=voices; integer++) {
      setup_click_synced(now, unit, integer, divisor, sync);
      MENU.out(integer);
    }
  }
  MENU.ln();

  PULSES.fix_global_next();
}


unsigned int gling128_0[] = {1,128,16, 0};
unsigned int gling128_1[] = {1,256,2, 1,128,16, 0};
unsigned int gling128_2[] = {1,512,4, 1,256,4, 1,128,16, 0};
unsigned int gling128[]   = {1,512,8, 1,256,4, 1,128,16, 0};

void init_pentatonic(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync) {
  /*
    I was looking at pentatonic scales
    one of my favorits follows the 'd f g a c' pattern

    Let's look at the  numeric relations:
    d   f   g   a   c   d

    5 : 6
    3   :   4
    2     :     3
    1         :         2
  */
  if (inverse) {
    no_inverse();
    return;
  }
  unsigned long unit=time_unit*multiplier;
  unit /= divisor;

  display_name5pars("init_pentatonic", inverse, voices, multiplier, divisor, sync);

  struct time now;
  PULSES.get_now();
  now=PULSES.now;

//  setup_click_synced(now, unit, 1, 1, sync);	// 'd'
//  setup_click_synced(now, unit, 5, 6, sync);	// 'f'
//  setup_click_synced(now, unit, 3, 4, sync);	// 'g'
//  setup_click_synced(now, unit, 2, 3, sync);	// 'a'
//  setup_click_synced(now, unit/3*2, 5, 6, sync); // 'c' is fifth from 'f'
//  setup_click_synced(now, unit, 1, 2, sync);	// 'd'

  setup_jiffle_thrower_synced(now, unit, 1, 1, sync, gling128);	// 'd'
  setup_jiffle_thrower_synced(now, unit, 5, 6, sync, gling128);	// 'f'
  setup_jiffle_thrower_synced(now, unit, 3, 4, sync, gling128);	// 'g'
  setup_jiffle_thrower_synced(now, unit, 2, 3, sync, gling128);	// 'a'
  if (voices > 4) {
    setup_jiffle_thrower_synced(now, unit/3*2, 5, 6, sync, gling128); // 'c' is fifth from 'f'
    setup_jiffle_thrower_synced(now, unit, 1, 2, sync, gling128);	// 'd'
  }
  if (voices > 6) {
    setup_jiffle_thrower_synced(now, unit, 5, 2*6, sync, gling128);	// 'f'
  }
  if (voices > 7) {
    // setup_jiffle_thrower_synced(now, unit, 3, 2*4, sync, gling128);	// 'g'
    setup_jiffle_thrower_synced(now, unit, 2, 2*3, sync, gling128);	// 'a' seems better on top with 8 voices
  }

  PULSES.fix_global_next();
}


// old style, obsolete
int prepare_magnets(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync) {
  if (inverse) {
    no_inverse();
    return 0;
  }

  ratios = pentatonic_minor;
  select_n(voices);

#define COMPATIBILITY_PERIOD_3110	// sets the period directly
#ifdef COMPATIBILITY_PERIOD_3110
  for (int pulse=0; pulse<voices; pulse++)
    if (selected_pulses & (1 << pulse)) {
      reset_and_edit_pulse(pulse);
      PULSES.pulses[pulse].period.time = 3110;	// brute force for compatibility ;)
      PULSES.pulses[pulse].period.overflow = 0;	// brute force for compatibility ;)
      en_jiffle_thrower(pulse, jiffle);
    }
  apply_ratios_on_periode(voices, ratios);
#else	// compatibility problems
  prepare_ratios(false, voices, multiplier, divisor, sync, ratios);
#endif

}


// ****************************************************************
int select_flagged() {
  selected_pulses=0;
  for (int pulse=0; pulse<pl_max; pulse++)
    if (PULSES.pulses[pulse].flags)
      selected_pulses |= (1 << pulse);

  return selected_pulses;
}


int select_all() {
  selected_pulses=0;
  for (int pulse=0; pulse<pl_max; pulse++)
    selected_pulses |= (1 << pulse);

  return selected_pulses;
}


int select_alive() {
  selected_pulses=0;
  for (int pulse=0; pulse<pl_max; pulse++)
    if(PULSES.pulses[pulse].flags && (PULSES.pulses[pulse].flags != SCRATCH))
      selected_pulses |= (1 << pulse);

  return selected_pulses;
}


int select_n(unsigned int n) {
  selected_pulses=0;
  if (n == 0)
    return selected_pulses;

  for (int pulse=0; pulse<n; pulse++)
    selected_pulses |= (1 << pulse);

  return selected_pulses;
}


// ****************************************************************
/* ratios[]
/* a ratios array has elements of multiplier/divisor pairs
   each is the integer representation of a rational number
   very useful for all kind of things like scales, chords, rhythms */

int prepare_ratios(bool inverse, int voices, unsigned long multiplier, unsigned long divisor, int sync, unsigned int *ratios) {
/* prepare a couple of pulses based on a ratio array.
   up to 'voices' pulses are created among the selected ones.
   return number of prepared pulses */
  if (inverse) {
    no_inverse();
    return 0;
  }
  int prepared=0;
  unsigned long unit=time_unit*multiplier;
  unit /= divisor;

  display_name5pars("prepare_ratios", inverse, voices, multiplier, divisor, sync);

  struct time now;
  PULSES.get_now();
  now=PULSES.now;

  unsigned long this_period;
  struct time new_period;
  int pulse=0;
  for (int ratio=0; ratio<voices; ratio++) {
    multiplier = ratios[ratio*2];
    if (multiplier==0)
      goto global_next;		// multiplier==0, end
    divisor=ratios[ratio*2+1];
    if (divisor==0)
      goto global_next;		// divisor==0, end

    for (; pulse<pl_max; pulse++) {
      if (selected_pulses & (1 << pulse)) {
	this_period = unit;
	this_period *= multiplier;
	this_period /= divisor;
	new_period.time = this_period;
	new_period.overflow = 0;
	PULSES.setup_pulse(NULL, SCRATCH, now, new_period);
	pulse++;
	prepared++;
	break;
      }
    }
  }

 global_next:
  PULSES.fix_global_next();
  return prepared;
}


int apply_ratios_on_periode(int voices, unsigned int *ratios) {
  struct time new_period;
  int applied=0;

  for (int ratio=0, pulse=0; ratio<voices; ratio++) {
    multiplier = ratios[ratio*2];
    if (multiplier==0)
      goto global_next;		// multiplier==0, end

    divisor=ratios[ratio*2+1];
    if (divisor==0)
      goto global_next;		// divisor==0, end
    for (; pulse<pl_max; pulse++) {
      if (selected_pulses & (1 << pulse)) {
	new_period = PULSES.pulses[pulse].period;
	PULSES.mul_time(&new_period, multiplier);
	PULSES.div_time(&new_period, divisor);
	PULSES.pulses[pulse].period = new_period;
	applied++;
	pulse++;
	break;
      }
    }
  }

 global_next:
  PULSES.fix_global_next();
  return applied;
}

// ****************************************************************
void init_chord_1345689a(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync) {
  unsigned long unit=multiplier*time_unit;
  unit /= divisor;

  display_name5pars("init_chord_1345689a", inverse, voices, multiplier, divisor, sync);

  struct time now;
  PULSES.get_now();
  now=PULSES.now;

  divisor=32L*36L;
  multiplier=1;
  setup_click_synced(now, unit, multiplier, divisor, sync);

  multiplier=3;
  setup_click_synced(now, unit, multiplier, divisor, sync);

  multiplier=4;
  setup_click_synced(now, unit, multiplier, divisor, sync);

  if(voices>3) {
    multiplier=5;
    setup_click_synced(now, unit, multiplier, divisor, sync);
  }

  PULSES.fix_global_next();
}

/* **************************************************************** */
// playing with rhythms:


// Generic setup pulse, stright or middle synced relative to 'when'.
// Pulse time and phase sync get deviated from unit, which is first
// multiplied by multiplier and divided by divisor.
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
			 int multiplier0, int multiplier_step,
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
  unsigned long multiplier=multiplier0;
  unsigned long divisor=divisor0;

  for (; count; count--) {
    setup_click_synced(when, unit, multiplier, divisor, sync);
    multiplier += multiplier_step;
    divisor += divisor_step;
  }

  PULSES.fix_global_next();
}


/* **************************************************************** */
// some pre-defined patterns:

void init_rhythm_1(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync) {
  unsigned long unit=multiplier*time_unit;
  unit /= divisor;

  struct time now;

  display_name5pars("init_rhythm_1", inverse, voices, multiplier, divisor, sync);

  PULSES.get_now();
  now=PULSES.now;

  divisor=1;
  for (long multiplier=2L; multiplier<6L; multiplier++)	// 2, 3, 4, 5
    setup_click_synced(now, unit, multiplier, divisor, sync);

  // 2*2*3*5
  setup_click_synced(now, unit, 2L*2L*3L*5L, divisor, sync);

  PULSES.fix_global_next();
}


// frequencies ratio 1, 4, 6, 8, 10
void init_rhythm_2(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync) {
  unsigned long unit= multiplier*time_unit;
  unit /= divisor;

  display_name5pars("init_rhythm_2", inverse, voices, multiplier, divisor, sync);

  struct time now;
  PULSES.get_now();
  now=PULSES.now;

  multiplier=1;
  for (unsigned long divisor=4; divisor<12 ; divisor += 2)
    setup_click_synced(now, unit, multiplier, divisor, sync);

  // slowest *not* synced
  setup_click_synced(now, unit, 1, 1, 0);

  PULSES.fix_global_next();
}

// nice 2 to 3 to 4 to 5 pattern with phase offsets
void init_rhythm_3(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync) {
  unsigned long unit=multiplier*time_unit;
  unit /= divisor;

  display_name5pars("init_rhythm_3", inverse, voices, multiplier, divisor, sync);

  struct time now;
  PULSES.get_now();
  now=PULSES.now;

  divisor=36L;

  multiplier=2;
  setup_click_synced(now, unit, multiplier, divisor, sync);

  multiplier=3;
  setup_click_synced(now, unit, multiplier, divisor, sync);

  multiplier=4;
  setup_click_synced(now, unit, multiplier, divisor, sync);

  if(voices>3) {
    multiplier=5;
    setup_click_synced(now, unit, multiplier, divisor, sync);
  }

  PULSES.fix_global_next();
}


void init_rhythm_4(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync) {
  unsigned long unit = multiplier*time_unit;
  unit /= divisor;

  struct time now;

  display_name5pars("init_rhythm_4", inverse, voices, multiplier, divisor, sync);

  PULSES.get_now();
  now=PULSES.now;

  divisor=1;
  setup_click_synced(now, unit, 1, 1, sync);     // 1
  // init_ratio_sequence(when, multiplier0, multiplier_step, divisor0, divisor_step, count, scaling, sync);
  if (voices>=5)
    init_ratio_sequence(now, 1, 1, 2, 1, 4, 1, sync);     // 1/2, 2/3, 3/4, 4/5
  else
    init_ratio_sequence(now, 1, 1, 2, 1, voices-1, 1, sync);     // 1/2, 2/3, 3/4  or  1/2, 2/3
}


// dest codes:
#define CODE_PULSES	0		// dest code pulses: apply selected_pulses
#define CODE_TIME_UNIT	1		// dest code time_unit
unsigned char dest = CODE_PULSES;


/* **************************************************************** */
/* Menu UI							*/

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
  while (scratch > std::max(time_units, (float) 1.0)) {    // "std::" for ESP8266
    MENU.space();
    scratch /= 10.0;
  }

  MENU.out((float) time_units, 6);
  MENU.out(timeUnits);
}


// pulse_info_1line(pulse):	one line pulse info, short version
void pulse_info_1line(int pulse) {
  PULSES.get_now();	// let's take time *before* serial output
  struct time now = PULSES.now;
  struct time next = PULSES.pulses[pulse].next;

  MENU.out(F("PULSE "));
  if (selected_pulses & (1 << pulse))
    MENU.out('*');
  else
    MENU.space();

  if (pulse<100)	// left padding 'pulse'
    MENU.space();
  if (pulse<10)
    MENU.space();
  MENU.out(pulse);
  MENU.slash();
  MENU.out((unsigned int) PULSES.pulses[pulse].counter);
  // right padding 'PULSES.pulses[pulse].counter'
  if (PULSES.pulses[pulse].counter<100000)
    MENU.space();
  if (PULSES.pulses[pulse].counter<10000)
    MENU.space();
  if (PULSES.pulses[pulse].counter<1000)
    MENU.space();
  if (PULSES.pulses[pulse].counter<100)
    MENU.space();
  if (PULSES.pulses[pulse].counter<10)
    MENU.space();
  MENU.space();

  MENU.out_flags_();
  MENU.outBIN(PULSES.pulses[pulse].flags, 8);

  MENU.tab();
  print_period_in_time_units(pulse);

  MENU.tab();
  display_action(pulse);

  MENU.tab();
  MENU.out(expected_);
  PULSES.sub_time(&now, &next);
  display_realtime_sec(next);

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
    //    PULSES.pulses[pulse].periodic_do = (void (*)(int)) &Pulses::pulse_info_1line;
  }
}


const char noAlive[] = "no pulses alive";

void flagged_pulses_info_lines()
{
  int count=0;

  for (int pulse=0; pulse<pl_max; ++pulse)
    if (PULSES.pulses[pulse].flags) {		// any flags set?
      pulse_info_1line(pulse);
      count++;
    }

  if (count == 0) {
    MENU.outln(noAlive);
    if(selected_pulses)		// special feature ;)
      print_selected_mask();
  }

  MENU.ln();
}

void activate_selected_synced_now(int sync) {
  if (selected_pulses==0)
    return;

  PULSES.get_now();
  struct time now=PULSES.now;

  for (int pulse=0; pulse<pl_max; pulse++)
    if (selected_pulses & (1 << pulse))
      PULSES.activate_pulse_synced(pulse, now, abs(sync));

  PULSES.fix_global_next();
  PULSES.check_maybe_do();	  // maybe do it *first*
}


void reset_and_edit_selected() {
  for (int pulse=0; pulse<pl_max; pulse++)
    if (selected_pulses & (1 << pulse)) {
      reset_and_edit_pulse(pulse);
    }
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
  MENU.slash();
  MENU.out((unsigned int) PULSES.pulses[pulse].counter);

  MENU.tab();
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

  MENU.out((float) PULSES.pulses[pulse].period.time / (float) time_unit,6);
  MENU.out(timeUnits);

  MENU.out(pulseOvfl);
  MENU.out((unsigned int) PULSES.pulses[pulse].period.time);
  MENU.slash();
  MENU.out(PULSES.pulses[pulse].period.overflow);

  MENU.tab();
  display_realtime_sec(PULSES.pulses[pulse].period);
  MENU.space();
  MENU.out(pulse_);

  MENU.ln();		// start next line

  MENU.out(lastOvfl);
  MENU.out((unsigned int) PULSES.pulses[pulse].last.time);
  MENU.slash();
  MENU.out(PULSES.pulses[pulse].last.overflow);

  MENU.out(nextOvfl);
  MENU.out(PULSES.pulses[pulse].next.time);
  MENU.slash();
  MENU.out(PULSES.pulses[pulse].next.overflow);

  MENU.tab();
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


void flagged_pulses_info()
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
const char DoJiffle_[] = "do_jiffle ";
const char SeedJiffle[] = "seed jiffle ";
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

  scratch=&tuned_sweep_click;
  if (PULSES.pulses[pulse].periodic_do == scratch) {
    MENU.out(F("tuned_sweep_click "));
    MENU.out((int) PULSES.pulses[pulse].char_parameter_1);
    return;
  }

  scratch=&sweep_click;
  if (PULSES.pulses[pulse].periodic_do == scratch) {
    MENU.out(F("sweep_click "));
    MENU.out((int) PULSES.pulses[pulse].char_parameter_1);
    return;
  }

  scratch=&sweep_click_0;
  if (PULSES.pulses[pulse].periodic_do == scratch) {
    MENU.out(F("sweep_click_0 "));
    MENU.out((int) PULSES.pulses[pulse].char_parameter_1);
    return;
  }

  scratch=&do_jiffle;
  if (PULSES.pulses[pulse].periodic_do == scratch) {
    MENU.out(DoJiffle_);
    MENU.out((int) PULSES.pulses[pulse].char_parameter_1);
    return;
  }

  scratch=&do_throw_a_jiffle;
  if (PULSES.pulses[pulse].periodic_do == scratch) {
    MENU.out(SeedJiffle); MENU.out((int) click_pin[pulse]);
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
  unsigned int *this_jiff=(unsigned int *) PULSES.pulses[pulse].parameter_2;

  // check for empty jiffle first:
  bool has_data=true;
  for (int i=0; i<3; i++)
    if (this_jiff[i]==0)
      has_data=false;

  if (has_data)	// there *is* jiffle data
    init_jiffle((unsigned int *) PULSES.pulses[pulse].parameter_2,	\
	      PULSES.pulses[pulse].next, PULSES.pulses[pulse].period, pulse);
  else	// zero in first triplet, *invalid* jiffle table.
    MENU.outln(F("no jiffle"));
}


/* **************************************************************** */
// pulses menu:

// what is selected?

void print_selected_mask() {
  const int hex_pulses=std::min(pl_max,16);  // displayed as hex chiffres, "std::" for ESP8266

  if(MENU.is_chiffre(MENU.cb_peek()))	// more numeric input, so no display yet...
    return;

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
    print_selected_mask();
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
const char selectPulseWith[] = "select pulse with ";
const char selectAllPulses[] =
  "\na=select *all* click pulses\tA=*all* pulses\tl=alive click voices\tL=all alive\tx=none\t~=invert selection";
const char uSelect[] = "u=select ";
const char selected__[] = "\t(selected)";
const char pulses_[] = "pulses ";

void info_select_destination_with(bool extended_destinations) {
  MENU.out(pulses_);
  MENU.out(PULSES.get_pl_max());
  MENU.tab();
  MENU.out(selectDestinationInfo);
  print_selected();
  MENU.out(selectPulseWith);

  // FIXME: use 16 here, when reaction will be prepared for a,b,c,d,e,f too.
  MENU.out_ticked_hexs(std::min(pl_max,10));	// "std::" for ESP8266

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
  "?=help\ti=info\t.=flagged info\t:=selected info";
const char microSeconds[] = " microseconds";
const char deactivateKill[] = \
  "M=deactivate all\tR=remove all\tK=kill\n\nCREATE PULSES\tstart with 'P'\nP=new pulse\tc=en-click\tj=en-jiffle\tf=en-info\tF=en-INFO\tn=sync now\nS=sync ";
const char perSecond_[] = " per second)";
const char equals_[] = " = ";
const char switchPulse[] = "s=switch pulse on/off";


// variables used to setup the experiments

bool inverse=false;	// bottom DOWN/up click-pin mapping
/*
  'inverse' works when setting up an experiment creating pulses
 	    other pulses aren't affected
	    some experiments do not implement that

  'reverse_click_pins()' as alternative:
  'reverse_click_pins()' works on the global click_pin[] array
 			 the pulses won't notice but play with new pin mapping */


/* **************************************************************** */
// special menu modes, like numeric input for jiffletabs
int menu_mode=0;
#define JIFFLE_ENTRY_UNTIL_ZERO_MODE	1	// menu_mode for unsigned integer data entry, stop at zero

/* **************************************************************** */
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
  MENU.out((float) (1000000.0 / (float) time_unit),6);
  MENU.outln(perSecond_);

  MENU.ln();
  MENU.out(switchPulse);
  MENU.tab();  MENU.out(deactivateKill);
  MENU.outln(sync);

  MENU.out(F("E=enter experiment (")); MENU.out(experiment); MENU.out(F(")"));
  MENU.out(F("\t\tV=voices for experiment (")); MENU.out(voices); MENU.outln(F(")"));
   MENU.out(F("b=toggle pin mapping (bottom "));
  if (inverse)
    MENU.out(F("up"));
  else
    MENU.out(F("down"));
  MENU.outln(F(")\tZ=reverse_click_pins"));

  MENU.out(F("Scale (")); MENU.out(multiplier);MENU.slash();  MENU.out(divisor);
  MENU.out(F(")\tm=multiplier d=divisor"));
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
  // PULSES.mul_time(&scratch, 12);		// 12 looks like a usable default	FIXME: change to 1
  PULSES.pulses[pulse].period = scratch;
}


int setup_jiffle_thrower_synced(struct time when,
				unsigned long unit,
				unsigned long multiplier, unsigned long divisor,
				int sync, unsigned int *jiffletab)
{
  int pulse= PULSES.setup_pulse_synced(&do_throw_a_jiffle, ACTIVE,
			       when, unit, multiplier, divisor, sync);
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
// the following jiffletab value counts how many times the pulse will get
// woken up with this new computed period
// then continue with next jiffletab entries
// a zero multiplier ends the jiffle

// jiffletabs define melody:
// up to 256 triplets of {multiplier, divisor, count}
// multiplier and divisor determine period based on the starting pulses period
// a multiplier of zero indicates end of jiffle

#define JIFFLETAB_INDEX_STEP	3

// FIXME: ################
#define JIFFLETAB_ENTRIES	8	// how many triplets

// there *MUST* be a trailing zero in all jiffletabs.
unsigned int jiffletab[] =
  {1,16,2, 1,256,32, 1,128,8, 1,64,2, 1,32,1, 1,16,1, 1,8,2, 0,0,0, 0};	// there *must* be a trailing zero.

unsigned int jiffletab_december[] =
  {1,1024,4, 1,64,4, 1,28,16, 1,512,8, 1,1024,128, 0 };

unsigned int jiffletab_december128[] =
  {1,1024,4, 1,64,4, 1,128,16, 1,512,8, 1,1024,128, 0 };

unsigned int jiffletab_december_pizzicato[] =
  {1,1024,4, 1,64,4, 1,28,16, 1,512,8, 1,1024,128, 1,2048,8, 0 };


// void enter_jiffletab(unsigned int *jiffletab), edit jiffletab by hand:
const char jifftabFull[] = "jiffletab full";
const char enterJiffletabVal[] = "enter jiffletab values";


long complete_numeric_input(long first_value) {
  char token = MENU.cb_peek();

  if (token < '0')	// if next token is not a chiffre
    return first_value;	//    return already read first_value
  if (token > '9')	//
    return first_value;	//

  int chiffre;
  for (int i=0;; i++) {
    chiffre=MENU.cb_peek(i);
    if (chiffre > '9')
      break;
    if (chiffre < '0')
      break;

    first_value *= 10;
  }
  // now we know that there are i more chiffres
  long more_digits=MENU.numeric_input(0);

  return first_value + more_digits;
}

int is_chiffre(char token) {
  if (token > '9' || token < '0')
    return false;
  return token;
}


void store_jiffle_data(int new_value) {
  jiffle[jiffle_write_index]=new_value;

  if (++jiffle_write_index >= jiffle_data_length) {	// array is full
    store_jiffle_zero_stop();

    // drop all remaining numbers and delimiters from input
    bool yes=true;
    while (yes) {
      switch (MENU.drop_input_token()) {
      case ' ':  case ',':
      case '0':  case '1':  case '2':  case '3':  case '4':
      case '5':  case '6':  case '7':  case '8':  case '9':
	yes=true;
      break;

      default:
	MENU.restore_input_token();
	yes=false;
      }
    }
  }
}


void store_jiffle_zero_stop() {
  if (jiffle_write_index>=jiffle_data_length)
    jiffle_write_index=jiffle_data_length-1;
  jiffle[jiffle_write_index]=0;	   // store a trailing zero
  jiffle[jiffle_data_length-1]=0;  // and last arry element (as a savety net)
  menu_mode=0;			   // stop numeric data input
  jiffle_write_index=0;		   // aesthetics, but hmm...

  display_jiffletab(jiffle);		  // put that here for now
}


void copy_jiffle_data(unsigned int *source) {	// zero terminated
  unsigned int data;
  for (int d=0, i=jiffle_write_index; i<jiffle_data_length; i++) {
    data=source[d++];
    store_jiffle_data(data);
    if (data==0) {
      break;
    }
  }
}


struct fraction jiffletab_len(unsigned int *jiffletab) {
  static struct fraction f;	// keep return value
  struct fraction scratch;
  unsigned int multiplier, divisor, count;

  f.multiplier = 0;
  f.divisor=1;
  for (int i=0; i <= JIFFLETAB_ENTRIES*JIFFLETAB_INDEX_STEP; i+=JIFFLETAB_INDEX_STEP) {
    multiplier = jiffletab[i];
    divisor    = jiffletab[i+1];
    count      = jiffletab[i+2];
    if (multiplier == 0 || divisor == 0 || count == 0)
      break;

    scratch.multiplier = multiplier * count;
    scratch.divisor = divisor;
    HARMONICAL.add_fraction(&scratch, &f);
  }

  return f;
}


void display_jiffletab(unsigned int *jiffletab) {
  struct fraction sum;
  sum.multiplier = 0;
  sum.divisor = 1;

  MENU.out("{");
  for (int i=0; i <= JIFFLETAB_ENTRIES*JIFFLETAB_INDEX_STEP; i++) {
    if ((i % JIFFLETAB_INDEX_STEP) == 0)
      MENU.space();
    if (i==jiffle_write_index)
      MENU.out("<");
    MENU.out(jiffletab[i]);
    if (i==jiffle_write_index)
      MENU.out(">");
    if (jiffletab[i] == 0)
      break;
    MENU.out(",");
  }

  MENU.out(F(" }  buffer "));
  MENU.out(jiffle_write_index); MENU.slash(); MENU.out(jiffle_data_length);

  sum = jiffletab_len(jiffletab);
  MENU.tab();
  MENU.out(F("length ")); display_fraction(&sum);
  MENU.ln();
}



unsigned int jiffletab01[] = {1,512,8, 1,1024,16, 1,2048,32, 1,1024,16, 0};
unsigned int jiffletab02[] = {1,128,2, 1,256,6, 1,512,10, 1,1024,32, 1,3*128,20, 1,64,8, 0};
unsigned int jiffletab03[] = {1,32,4, 1,64,8, 1,128,16, 1,256,32, 1,512,64, 1,1024,128, 0};	// testing octaves
unsigned int jiffletab04[] =
  {1,2096,4, 1,512,2, 1,128,2, 1,256,2, 1,512,8, 1,1024,32, 1,512,4, 1,256,3, 1,128,2, 1,64,1, 0};
unsigned int jiffletab05[] = {2,1024*3,4, 1,1024,64, 1,2048,64, 1,512,2, 1,64,1, 1,32,1, 1,16,2, 0};
unsigned int jiffletab06[] = {1,32,2, 0};	// doubleclick
unsigned int jiffletab1[]  = {1,1024,64, 1,512,4, 1,128,2, 1,64,1, 1,32,1, 1,16,1, 0};


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
  int base_index = PULSES.pulses[pulse].char_parameter_2;		// readability
  long multiplier=jiffletab[base_index];
  if (multiplier == 0) {	// multiplier==0 no next phase, return
    PULSES.init_pulse(pulse);	// remove pulse
    return;			// and return
  }

  //initialize next phase, re-using the same pulse:

  // to be able to play unfinished jiffletabs while editing them
  // we check the other 2 items of the triple for zeroes
  long divisor=jiffletab[base_index+1];
  long counter=jiffletab[base_index+2];
  if (divisor==0 || counter==0 ) {	// no next phase, return
    PULSES.init_pulse(pulse);		// remove pulse
    return;				// and return
  }
  PULSES.pulses[pulse].period.time =
    PULSES.pulses[pulse].ulong_parameter_1 * jiffletab[base_index] / jiffletab[base_index+1];
  PULSES.pulses[pulse].parameter_1 = jiffletab[base_index+2];		// count of next phase
}


// huch, unused?	FIXME: unused?
void setup_jiffle_thrower(unsigned int *jiffletab, unsigned char new_flags, struct time when, struct time new_period) {
  int pulse = PULSES.setup_pulse(&do_throw_a_jiffle, new_flags, when, new_period);

  if (pulse != ILLEGAL) {
    PULSES.pulses[pulse].parameter_2 = (unsigned int) jiffletab;
  } else {
    MENU.out(noFreePulses);
  }
}


// pre-defined jiffle pattern:
void setup_jiffles2345(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync) {
  if (inverse) {
    no_inverse();
    return;
  }
  unsigned long unit=multiplier*time_unit;
  unit /= divisor;

  display_name5pars("jiffles2345", inverse, voices, multiplier, divisor, sync);

  struct time when;
  PULSES.get_now();
  when=PULSES.now;

  divisor=1;

  multiplier=2;
  setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiffletab);

  multiplier=3;
  setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiffletab);

  multiplier=4;
  setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiffletab);

  multiplier=5;
  setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiffletab);

  // up to 8 voices
  if (voices > 5) {
    multiplier=6;
    setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiffletab);

    // skip multiplier 7

    if (voices > 6) {
      multiplier=8;
      setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiffletab);

      if (voices > 7) {
	multiplier=9;
	setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiffletab);
      }
    }

    // next would be i.e. 10, 12, 14, 15, 16
    // so 2 3 4 5 6 8 9 10 12 14 15 16
  }

  PULSES.fix_global_next();
}

// triplets {multiplier, divisor, count}
// multiplier==0 means end

// jiffletab0 is obsolete	DADA ################
unsigned int jiffletab0[] =
  {2,1024*3,4, 1,1024,64, 1,2048,64, 1,512,4, 1,64,3, 1,32,1, 1,16,2, 0};	// nice short jiffy

unsigned int jiff0[] =
  {1,16,4, 1,24,6, 1,128,16, 1,1024,64, 1,2048,128, 1,4096,256, 1,2048,64, 1,4096,128, 1,32,2, 0}; // there *must* be a trailing zero.);

unsigned int jiff1[] =
  {1,512,8, 1,1024,16, 1,2048,32, 1,1024,16, 0};

unsigned int jiff2[] =
  {1,2096,4, 1,512,2, 1,128,2, 1,256,2, 1,512,8, 1,1024,32, 1,512,4, 1,256,3, 1,128,2, 1,64,1, 0};


void setup_jifflesNEW(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync) {
  if (inverse) {
    no_inverse();
    return;
  }
  unsigned long unit=multiplier*time_unit;
  unit /= divisor;

  display_name5pars("setup_jifflesNEW", inverse, voices, multiplier, divisor, sync);

  struct time when;
  PULSES.get_now();
  when=PULSES.now;

  multiplier=1;

  divisor=1;
  setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiff0);

  divisor=3;
  setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiff1);

  divisor=4;
  setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiff2);

  if (voices > 5) {
    divisor=6;
    setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiffletab0);
  }

  PULSES.fix_global_next();
}


void setup_jiffle128(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync) {
  /*
  multiplier and divisor are used twice:
  first to scale unit from time_unit
  then reset to build the jiffle thrower pulses
    multiplier=1
    divisor = 1, 2, 3, 4, ...
  */
  unsigned long unit=multiplier*time_unit;
  unit /= divisor;

  display_name5pars("setup_jiffle128", inverse, voices, multiplier, divisor, sync);

  PULSES.get_now();
  struct time when=PULSES.now;

  multiplier=1;
  if (!inverse) {
    for (int click=0; click<voices; click++) {
      divisor=click+1;
      setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, gling128);
    }
  } else {
    for (int click=voices-1; click>=0; click--) {
      divisor=click+1;
      setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, gling128);
    }
  }

  PULSES.fix_global_next();
}

// setup for ESP8266 Frog Orchester
void setup_jiffles0(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync) {
  if (inverse) {
    no_inverse();
    return;
  }
  unsigned long unit=multiplier*time_unit;
  unit /= divisor;

  display_name5pars("setup_jiffles0", inverse, voices, multiplier, divisor, sync);

  struct time when;
  PULSES.get_now();
  when=PULSES.now;

  divisor=3;	// this was just the default for the frogs

  multiplier=2;
  setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiffletab0);

  multiplier=3;
  setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiffletab0);

  multiplier=4;
  setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiffletab0);

  multiplier=5;
  setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiffletab0);

  if (voices>4) {
    // 2*3*2*5	(the 4 needs only another multiplier of 2)
    multiplier=2*3*2*5;
    setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiffletab0);

    if (voices>5) {
      multiplier=16;
      setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiffletab0);
    }
  }

  PULSES.fix_global_next();
}

/*
  'inverse' works when setting up an experiment creating pulses
 	    other pulses aren't affected
	    some experiments do not implement that

  'reverse_click_pins()' as alternative:
  'reverse_click_pins()' works on the global click_pin[] array
 			 the pulses won't notice but play with new pin mapping */

void reverse_click_pins() {
  uint8_t scratch;
  for (int i=0, j=CLICK_PULSES-1; i<j; i++, j--) {
      scratch=click_pin[i];
      click_pin[i]=click_pin[j];
      click_pin[j]=scratch;
  }
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
  flagged_pulses_info_lines();
}

// helper functions to display parameters of menu functions:
void display_next_par(long parameter) {
  MENU.out(F(", "));
  MENU.out(parameter);
}

void display_last_par(long parameter) {
  MENU.out(F(", "));
  MENU.out(parameter);
  MENU.outln(F(")"));
}

void display_name5pars(char* name, bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync) {
  if (MENU.verbosity) {
    MENU.out((char *) name);
    MENU.out("(");
    MENU.out(inverse);
    display_next_par(voices);
    display_next_par(multiplier);
    display_next_par(divisor);
    display_last_par(sync);
  }
}

// display helper function no_inverse()
void no_inverse() {
    MENU.outln(F("no 'inverse'\ttry 'Z' instead"));
    extern bool inverse;
    inverse=false;
}

// display factor function show_scale();
void show_scale() {
  MENU.out(F("multiplier/divisor "));
  MENU.out(multiplier); MENU.slash(); MENU.outln(divisor);
}

void maybe_show_selected_mask() {
  if (MENU.maybe_display_more())
    print_selected_mask();
}


bool menu_pulses_reaction(char menu_input) {
  static long result=0;
  static long calc_result=0;
  long new_value=0;
  struct time now, time_scratch;
  unsigned long bitmask;
  char next_token;	// for multichar commandos

  switch (menu_input) {
  case '?':	// help, overrides common menu entry for '?'
    MENU.menu_display();	// as common
    short_info();		// + short info
    break;

  case '.':	// time and flagged pulses info
    short_info();
    break;

  case ':':
    MENU.ln();
    time_info();
    MENU.ln();
    maybe_show_selected_mask();
    selected_pulses_info_lines();
    break;

  case ',':	// accept as noop in normal mode. used as delimiter to input data, displaying info. see 'menu_mode'
    if (menu_mode==JIFFLE_ENTRY_UNTIL_ZERO_MODE)
	display_jiffletab(jiffle);
    break;

  // in normal mode toggle pulse selection with chiffres
  // else input data. see 'menu_mode'
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
    switch (menu_mode) {
    case 0:	// normal input, no special menu_mode
      if((menu_input -'0') >= pl_max)
	return false;		// *only* responsible if pulse exists

      // existing pulse:
      selected_pulses ^= (1 << (menu_input - '0'));

      maybe_show_selected_mask();
      break;

    case JIFFLE_ENTRY_UNTIL_ZERO_MODE:	// first chiffre already seen
      new_value = complete_numeric_input(menu_input - '0');
      if (new_value)
	store_jiffle_data(new_value);
      else	// zero stops the input mode
	store_jiffle_zero_stop();
      break;
    }
    break;

  case 'u':	// calculating or select destination: time_unit
    {
      signed long new_value=(signed long) time_unit;
      if (MENU.maybe_calculate_input(&new_value)) {
	MENU.out("==> "), MENU.outln(new_value);
	time_unit=new_value;
      }
      else
	dest = CODE_TIME_UNIT;		// FIXME: obsolete?
    }
    break;

  case 'a':	// select destination: all pulses with flags
    select_flagged();
    maybe_show_selected_mask();

    if (MENU.maybe_display_more())
      flagged_pulses_info_lines();

    break;

  case 'A':	// select destination: *all* pulses
    select_all();
    maybe_show_selected_mask();
    break;

  case 'l':	// select destination: alive voices
    selected_pulses=0;
    for (int pulse=0; pulse<voices; pulse++)
      if(PULSES.pulses[pulse].flags && (PULSES.pulses[pulse].flags != SCRATCH))
	selected_pulses |= (1 << pulse);

    maybe_show_selected_mask();
    break;

  case 'L':	// select destination: all alive pulses
    select_alive();
    maybe_show_selected_mask();
    break;

  case '~':	// invert destination selection
    selected_pulses = ~selected_pulses;
    bitmask=0;
    for (int pulse=0; pulse<pl_max; pulse++)
      bitmask |= (1 << pulse);
    selected_pulses &= bitmask;
    maybe_show_selected_mask();
    break;

  case 'x':	// clear destination selection
    selected_pulses = 0;
    maybe_show_selected_mask();
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

    if (MENU.maybe_display_more()) {
      MENU.ln();
      flagged_pulses_info_lines();			// *then* info ;)
    }
    // info_select_destination_with(false);	// DADA ################
    break;

  case 'S':	// enter sync
    if (MENU.maybe_calculate_input(&result)) {
      if (result>=0 )
	sync = result;
      else
	MENU.out(onlyPositive);
    }

    if (MENU.maybe_display_more()) {
      MENU.out(sync_);
      MENU.outln(sync);
    }

    break;

  case 'i':	// info
    MENU.ln();
    time_info();
    MENU.ln();	// ################################################################
    flagged_pulses_info();
    break;

  case 'M':	// "mute", no deactivate all clicks, see 'N'
    deactivate_all_clicks();
    break;

  case '*':	// multiply destination
    switch (dest) {
    case CODE_PULSES:
      new_value = MENU.numeric_input(1);
      if (new_value>=0) {
	for (int pulse=0; pulse<pl_max; pulse++)
	  if (selected_pulses & (1 << pulse))
	    PULSES.multiply_period(pulse, new_value);

	if (MENU.maybe_display_more()) {
	  MENU.ln();
	  flagged_pulses_info_lines();
	}
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

	if (MENU.maybe_display_more()) {
	  MENU.ln();
	  flagged_pulses_info_lines();
	}
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

	if (MENU.maybe_display_more()) {
	  MENU.ln();
	  flagged_pulses_info_lines();
	}
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
    if (selected_pulses) {
      MENU.out(killPulse);
      for (int pulse=0; pulse<pl_max; pulse++)
	if (selected_pulses & (1 << pulse)) {
	  PULSES.init_pulse(pulse);
	  if (MENU.maybe_display_more()) {
	    MENU.out(pulse);
	    MENU.space();
	  }
	}
      MENU.ln();
    }

    if (MENU.maybe_display_more()) {
      flagged_pulses_info_lines();
      MENU.ln();
    }
    break;

  case 'P':	// pulse create and edit
    reset_and_edit_selected();
    if (MENU.maybe_display_more()) {
      MENU.ln();
      flagged_pulses_info_lines();
    }
    break;

  case 'n':	// synchronise to now
    // we work on pulses anyway, regardless dest
    activate_selected_synced_now(sync);	// sync and activate
    if (MENU.maybe_display_more()) {		// *then* info ;)
      MENU.ln();
      flagged_pulses_info_lines();
    }
    break;

  case 'N':	// NULLs payload
    // we work on pulses anyway, regardless dest
    for (int pulse=0; pulse<voices; pulse++)
      if (selected_pulses & (1 << pulse)) {
	PULSES.pulses[pulse].periodic_do = NULL;
	if (pulse<CLICK_PULSES)		// set clicks on LOW
	  digitalWrite(click_pin[pulse], LOW);
      }

    PULSES.fix_global_next();	// just in case?

    if (MENU.maybe_display_more()) {	// *then* info ;)
      MENU.ln();
      flagged_pulses_info_lines();
    }
    break;

  case 'c':	// en_click
    // we work on voices anyway, regardless dest
    for (int pulse=0; pulse<voices; pulse++)
      if (selected_pulses & (1 << pulse))
	en_click(pulse);

    if (MENU.maybe_display_more()) {
      MENU.ln();
      flagged_pulses_info_lines();
    }

    break;

  case 'w':
    sweep_up *= -1;	// toggle direction up down

    if (sweep_up==0)	// start sweeping if it was disabled
      sweep_up=1;

    if (MENU.maybe_display_more())
      sweep_info();

    break;

  case 'W':
    next_token = MENU.cb_peek();
    if (next_token != (char) EOF) {	// there is input after 'W'
      switch(next_token) {	// examine following input token
      case '~': case 't':
	MENU.drop_input_token();
	sweep_up *= -1;		// toggle sweep direction up down
	break;
      case '0':
	MENU.drop_input_token();
	sweep_up = 0;		// sweep off
	break;
      case '+': case '1':
	MENU.drop_input_token();
	sweep_up = 1;		// sweep up
	break;
      case '-':
	MENU.drop_input_token();
	sweep_up = -1;		// sweep down
	break;
      case '?':			// info only
	MENU.drop_input_token();
	// if verbosity is too low sweep_info will not be called below,
	// so we do it here
	if (MENU.verbosity < VERBOSITY_SOME)
	  sweep_info();

	break;
      }
    } else {			// no input follows 'W' token:
      if (sweep_up==0)
	sweep_up=1;		// start sweeping up if disabled
      else			// if already sweeping:
	sweep_up *= -1;		//    toggle sweep direction up/down
    }

    if (MENU.verbosity >= VERBOSITY_SOME)
      sweep_info();
    break;

  case 't':	// en_sweep_click
    // we work on voices anyway, regardless dest
    for (int pulse=0; pulse<voices; pulse++)
      if (selected_pulses & (1 << pulse))
	en_sweep_click(pulse);

    if (MENU.maybe_display_more()) {
      MENU.ln();
      flagged_pulses_info_lines();
    }

    break;

  case 'o':	// en_sweep_click_0
    // we work on voices anyway, regardless dest
    for (int pulse=0; pulse<voices; pulse++)
      if (selected_pulses & (1 << pulse))
	en_sweep_click_0(pulse);

    if (MENU.maybe_display_more()) {
      MENU.ln();
      flagged_pulses_info_lines();
    }

    break;

  case 'p':	// en_tuned_sweep_click
    // we work on voices anyway, regardless dest
    for (int pulse=0; pulse<voices; pulse++)
      if (selected_pulses & (1 << pulse))
	en_tuned_sweep_click(pulse);

    if (MENU.maybe_display_more()) {
      MENU.ln();
      flagged_pulses_info_lines();
    }

    break;

  case 'T':	// 'T<integer-number>' sets tuning, 'T' toggles TUNED
    result = (long) PULSES.tuning;
    if (MENU.maybe_calculate_input(&result))	{	// T1 sets tuning to 1.0
      PULSES.tuning = (double) result;
      tuning_info();
      MENU.ln();
    } else {	// toggle TUNED on selected pulses
      // we work on voices anyway, regardless dest
      for (int pulse=0; pulse<voices; pulse++)
	if (selected_pulses & (1 << pulse))
	  if (PULSES.pulses[pulse].flags & TUNED)
	    PULSES.stop_tuning(pulse);
	  else
	    PULSES.activate_tuning(pulse);
    }

    if (MENU.maybe_display_more()) {
      MENU.ln();
      flagged_pulses_info_lines();
    }
    break;


  case 'j':	// en_jiffle_thrower
    // we work on voices anyway, regardless dest
    for (int pulse=0; pulse<voices; pulse++)
      if (selected_pulses & (1 << pulse))
	en_jiffle_thrower(pulse, jiffle);

    if (MENU.maybe_display_more()) {
      MENU.ln();
      flagged_pulses_info_lines();
    }
    break;

  case 'J':	// select jiffle
    // temporary interface to some jiffles from source, some very old
    // FIXME:	review and delete	################
    switch (MENU.numeric_input(0)) {
    case 0:	// temporary interface to some jiffles from source, some very old
      jiffle = jiffle_data;
      break;
    case 1:
      jiffle=gling128;
      break;
    case 2:
      jiffle = jiffletab;
      break;
    case 3:
      jiffle = jiffletab_december;
      break;
    case 4:
      jiffle = jiffletab_december128;
      break;
    case 5:
      jiffle = jiffletab_december_pizzicato;
      break;
    case 6:
      jiffle = jiffletab01;
      break;
    case 7:
      jiffle = jiffletab01;
      break;
    case 8:
      jiffle = jiffletab02;
      break;
    case 9:
      jiffle = jiffletab03;
      break;
    case 10:
      jiffle = jiffletab04;
      break;
    case 11:
      jiffle = jiffletab05;
      break;
    case 12:
      jiffle = jiffletab06;
      break;
    case 13:
      jiffle = jiffletab06;
      break;
    case 14:
      jiffle = gling128_0;
      break;
    case 15:
      jiffle = gling128_1;
      break;
    case 16:
      jiffle = gling128_2;
      break;
    case 17:
      jiffle = harmonics4;
      break;
    case 18:
      jiffle = ting1024;
      break;
    case 19:
      jiffle = ting4096;
      break;
    case 20:
      jiffle = arpeggio4096;
      break;
//  case 21:
//    jiffle = halfway;
//    break;
    case 21:
      jiffle = mimic_japan_pentatonic;
      break;
    default:
      if (MENU.verbosity >= VERBOSITY_SOME)
	MENU.outln(invalid_);
    }

    if (MENU.maybe_display_more())
      display_jiffletab(jiffle);

    break;

  case 'f':	// en_info
    // we work on pulses anyway, regardless dest
    for (int pulse=0; pulse<pl_max; pulse++)
      if (selected_pulses & (1 << pulse))
	en_info(pulse);

    if (MENU.maybe_display_more()) {
      MENU.ln();
      flagged_pulses_info_lines();
    }
    break;

  case 'F':	// en_INFO
    // we work on pulses anyway, regardless dest
    for (int pulse=0; pulse<pl_max; pulse++)
      if (selected_pulses & (1 << pulse))
	en_INFO(pulse);

    if (MENU.maybe_display_more()) {
      MENU.ln();
      flagged_pulses_info_lines();
    }
    break;

  case '{':	// enter_jiffletab
    menu_mode=JIFFLE_ENTRY_UNTIL_ZERO_MODE;
    jiffle = jiffle_data;
    jiffle_write_index=0;
    if(MENU.cb_peek()==EOF)
      if (MENU.verbosity)
	display_jiffletab(jiffle);
    break;

  case '}':	// display jiffletab / end editing jiffletab
    if (MENU.verbosity)
      display_jiffletab(jiffle);

    menu_mode=0;
    jiffle_write_index=0;
    break;

  case 'd':	// divisor
    if(MENU.cb_peek()==EOF)
      MENU.outln(F("divisor"));

    new_value = MENU.numeric_input(divisor);
    if (new_value>0 )
      divisor = new_value;
    else
      MENU.outln(F("small positive integer only"));

    if (MENU.maybe_display_more())
      show_scale();
    break;

  case 'C':	// Calculator *integers only*	Calculates simply *left to right*
    if (MENU.maybe_calculate_input(&calc_result))
      MENU.out("==> "), MENU.outln(calc_result);
    break;

  case 'D':	// DADA debug
    //    MENU.outln(HARMONICAL.harmonical_base);

    //    MENU.outln(F("small primes:"));
    //    for (int i=0; HARMONICAL.small_primes[i]; i++) {
    //      MENU.out(i+1);
    //      MENU.tab();
    //      MENU.outln(HARMONICAL.small_primes[i]);
    //    }
    {
      unsigned long lcm=1L;
      for (int pulse=0; pulse<pl_max; pulse++)
	if (selected_pulses & (1 << pulse))
	  lcm = HARMONICAL.LCM(lcm, PULSES.pulses[pulse].period.time);
      MENU.out(F("lcm ")); MENU.outln(lcm);

      for (int pulse=0; pulse<pl_max; pulse++)
	if ((selected_pulses & (1 << pulse)) && PULSES.pulses[pulse].period.time) {
	  MENU.out(pulse);
	  MENU.tab();
	  MENU.outln(lcm/PULSES.pulses[pulse].period.time);
	}
    }

//	    {
//	    unsigned int lcm=1;
//	    // simplest case for test: 0 to voices-1
//	    for (int pulse=voices-1; pulse>-1; pulse--)
//	      if (selected_pulses & (1 << pulse)) {
//		lcm = LCM(PULSES.pulses[pulse].period.time, lcm);
//		MENU.outln(lcm/PULSES.pulses[0].period.time);
//	      }
//	    }
//

//    fraction bruch1;
//    fraction bruch2;
//    bruch1.multiplier=11;
//    bruch1.divisor=44;
//    //    reduce_fraction(&bruch1);
//    bruch2.multiplier=100;
//    bruch2.divisor=300;
//
//    HARMONICAL.add_fraction(&bruch1, &bruch2);
//    MENU.out(bruch2.multiplier); MENU.slash(); MENU.outln(bruch2.divisor);


    // copy_jiffle_data(gling128);	// zero terminated
    // copy_jiffle_data(jiffletab_december_pizzicato);
    // display_jiffletab(jiffle);

    //    tuning = PULSES.tuning; // FIXME: workaround for having all 3 sweep implementations in parallel
    //    sweep_info();
    //    {
    //      int fastest=PULSES.fastest_pulse();
    //      MENU.ln();
    //      PULSES.deactivate_pulse(fastest);
    //    }
    break;

  case 'm':	// multiplier
    if(MENU.cb_peek()==EOF)
      MENU.outln(F("multiplier"));

    new_value = MENU.numeric_input(multiplier);
    if (new_value>0 )
      multiplier = new_value;
    else
      MENU.outln(F("small positive integer only"));

    if (MENU.maybe_display_more())
      show_scale();

    break;

  case 'V':	// set voices
    if(MENU.cb_peek()==EOF)
      MENU.out(F("voices "));

    new_value = MENU.numeric_input(voices);
    if (new_value>0 && new_value<=CLICK_PULSES)
      voices = new_value;
    else
      MENU.outln(invalid_);

    if (MENU.maybe_display_more())
      MENU.outln(voices);

    break;

  case 'b':	// toggle bottom down/up click-pin mapping bottom up/down
    if (MENU.verbosity)
      MENU.out(F("pin mapping bottom "));

    inverse = !inverse;	// toggle bottom up/down click-pin mapping

    if (MENU.maybe_display_more()) {
      if (MENU.verbosity) {
	if (inverse)
	  MENU.outln(F("up"));
	else
	  MENU.outln(F("down"));
      }
    }

    break;

  case 'R':	// remove (=reset) all pulses
    { int cnt=0;
      for (int pulse=0; pulse<pl_max; pulse++) {	// tabula rasa
	if (PULSES.pulses[pulse].flags) {
	  PULSES.init_pulse(pulse);
	  cnt++;
	}
      }

      // By design click pulses *HAVE* to be defined *BEFORE* any other pulses:
      PULSES.init_click_pulses();
      init_click_pins();		// switch them on LOW, output	current off, i.e. magnets

      if (MENU.verbosity) {
	MENU.out(F("\nremoved all pulses ")); MENU.outln(cnt);
	MENU.outln(F("switched pins off."));
      }
    }
    break;

  case 'Z':
    reverse_click_pins();

    if (MENU.maybe_display_more())
      MENU.outln(F("reverse_click_pins"));

    break;

  case 'E':	// enter experiment

    if (MENU.maybe_display_more())
      MENU.out(F("experiment "));

    new_value = MENU.numeric_input(experiment);
    if (new_value>=0 )
      experiment = new_value;
    else
      MENU.out(onlyPositive);

    if (MENU.maybe_display_more())
      MENU.outln(experiment);

    switch (experiment) {	// initialize defaults, but do not start yet
    case 1:
      multiplier=2;
      divisor=1;
      sync=15;

      if (MENU.verbosity >= VERBOSITY_SOME)
	display_name5pars("setup_jiffle128", inverse, voices, multiplier, divisor, sync);
      break;
    case 2:
      sync=0;
      multiplier=1;
      divisor=1;

      if (MENU.verbosity >= VERBOSITY_SOME)
	display_name5pars("init_div_123456", inverse, voices, multiplier, divisor, sync);
      break;
    case 3:
      sync=1;
      multiplier=2;
      divisor=3;

      if (MENU.verbosity >= VERBOSITY_SOME) {
	// display_name5pars("setup_jiffles0", inverse, voices, multiplier, divisor, sync);
	MENU.out(F("setup_jiffles0("));
	MENU.out(inverse);
	display_next_par(voices);
	display_next_par(multiplier);
	display_next_par(divisor);
	display_next_par(sync);
	MENU.outln(F(")  ESP8266 Frogs"));
      }
      break;
    case 4:
      multiplier=1;
      divisor=2;
      sync=0;
      jiffle=jiffletab;

      if (MENU.verbosity >= VERBOSITY_SOME)
	display_name5pars("setup_jiffles2345", inverse, voices, multiplier, divisor, sync);
      break;
    case 5:
      sync=0;		// FIXME: test and select ################
      multiplier=3;
      divisor=1;

      if (MENU.verbosity >= VERBOSITY_SOME)
	display_name5pars("init_123456", inverse, voices, multiplier, divisor, sync);
      break;
    case 6:
      sync=0;		// FIXME: test and select ################
      multiplier=1;
      divisor=1;

      if (MENU.verbosity >= VERBOSITY_SOME)
	display_name5pars("init_chord_1345689a", inverse, voices, multiplier, divisor, sync);
      break;
    case 7:
      sync=1;
      multiplier=1;
      divisor=6*7;

      if (MENU.verbosity >= VERBOSITY_SOME)
	display_name5pars("init_rhythm_1", inverse, voices, multiplier, divisor, sync);
      break;
    case 8:
      sync=5;
      multiplier=1;
      divisor=1;

      if (MENU.verbosity >= VERBOSITY_SOME)
	display_name5pars("init_rhythm_2", inverse, voices, multiplier, divisor, sync);
      break;
    case 9:
      sync=3;
      multiplier=1;
      divisor=1;

      if (MENU.verbosity >= VERBOSITY_SOME)
	display_name5pars("init_rhythm_3", inverse, voices, multiplier, divisor, sync);
      break;
    case 10:
      sync=1;
      multiplier=1;
      divisor=7L*3L;

      if (MENU.verbosity >= VERBOSITY_SOME)
	display_name5pars("init_rhythm_4", inverse, voices, multiplier, divisor, sync);
      break;
    case 11:
      sync=3;
      multiplier=3;
      divisor=1;

      if (MENU.verbosity >= VERBOSITY_SOME)
	display_name5pars("setup_jifflesNEW", inverse, voices, multiplier, divisor, sync);
      break;
    case 12:
      sync=1;
      multiplier=1;
      divisor=1;

      if (MENU.verbosity >= VERBOSITY_SOME)
	display_name5pars("init_pentatonic", inverse, voices, multiplier, divisor, sync);
      break;
    case 13:
      sync=1;	// or: sync=0;
      multiplier=1;
      divisor=1;
      voices=8;	//just for 'The Harmonical Strings Christmas Evening Sounds'
      inverse=false;
      // unsigned int harmonics4 = {1,1,1024, 1,2,1024, 1,3,1024, 1,4,1024, 0};
      jiffle=harmonics4;
      select_n(voices);
      display_name5pars("prepare_magnets", inverse, voices, multiplier, divisor, sync);
      prepare_magnets(inverse, voices, multiplier, divisor, sync);

      if (MENU.verbosity >= VERBOSITY_SOME)
	flagged_pulses_info_lines();
      break;
    case 14:
      // magnets on strings, second take
      multiplier=1;
      divisor=1;
      inverse=false;

      ratios = pentatonic_minor;
      select_n(voices);
      prepare_ratios(false, voices, multiplier * 1024 , divisor * 1167, sync, ratios);
      jiffle=ting1024;
      select_n(voices);
      display_name5pars("E14", inverse, voices, multiplier, divisor, sync);

      if (MENU.verbosity >= VERBOSITY_SOME)
	flagged_pulses_info_lines();
      break;
    case 15:
      // magnets on strings, third take
      multiplier=1;
      divisor=1;
      inverse=false;

      ratios = pentatonic_minor;
      select_n(voices);
      prepare_ratios(false, voices, multiplier * 4096 , divisor * 1167, sync, ratios);
      jiffle=ting4096;
      select_n(voices);
      display_name5pars("E15", inverse, voices, multiplier, divisor, sync);

      if (MENU.verbosity >= VERBOSITY_SOME)
	flagged_pulses_info_lines();
      break;

    case 16:
      // piezzos on low strings 2016-12-28
      multiplier=4096;
      divisor=256;
      inverse=false;

      ratios = european_pentatonic;
      select_n(voices);
      prepare_ratios(false, voices, multiplier, divisor, sync, ratios);
      jiffle=ting4096;
      // jiffle = arpeggio4096;
      display_name5pars("E16 european_pent", inverse, voices, multiplier, divisor, sync);

      if (MENU.verbosity >= VERBOSITY_SOME)
	flagged_pulses_info_lines();
      break;

    case 17:
      // magnets on steel strings, "japanese"
      multiplier=1;	// click
      // multiplier=4096;	// jiffle ting4096
      divisor=256*5;

      ratios = mimic_japan_pentatonic;
      select_n(voices);
      prepare_ratios(false, voices, multiplier, divisor, sync, ratios);
      jiffle=ting4096;
      display_name5pars("E17 mimic japan", inverse, voices, multiplier, divisor, sync);

      if (MENU.verbosity >= VERBOSITY_SOME)
	flagged_pulses_info_lines();

      break;

    case 18:	// nylon stringed wooden box, piezzos
      ratios = pentatonic_minor;
      multiplier=1;	// click
      // multiplier=4096;	// jiffle ting4096
      // divisor=2048;

      // string tuning on 8/9
//      multiplier=8;
//      divisor=9*1024;
      multiplier=1;
      divisor=9*128;
      jiffle=ting4096;

      select_n(voices);
      prepare_ratios(false, voices, multiplier, divisor, sync, ratios);
      display_name5pars("E18 pentatonic minor", inverse, voices, multiplier, divisor, sync);

      if (MENU.verbosity >= VERBOSITY_SOME)
	flagged_pulses_info_lines();

      break;

    default:
      if (MENU.verbosity >= VERBOSITY_SOME)
	MENU.outln(invalid_);

      experiment=0;
      break;
    }
    if (MENU.verbosity >= VERBOSITY_SOME)
      MENU.outln(F("Press '!' to start"));

    break;

  case '!':			// '!' setup and start experiment
    switch (experiment) {
    case -1:
    case 0:
      break;
    case 1:
      setup_jiffle128(inverse, voices, multiplier, divisor, sync);
      break;
    case 2:
      init_div_123456(inverse, voices, multiplier, divisor, sync);
      break;
    case 3:
      setup_jiffles0(inverse, voices, multiplier, divisor, sync);    // ESP8266 Frog Orchester
      break;
    case 4:
      setup_jiffles2345(inverse, voices, multiplier, divisor, sync);
      break;
    case 5:
      init_123456(inverse, voices, multiplier, divisor, sync);
      break;
    case 6:
      init_chord_1345689a(inverse, voices, multiplier, divisor, sync);
      break;
    case 7:
      init_rhythm_1(inverse, voices, multiplier, divisor, sync);
      break;
    case 8:
      init_rhythm_2(inverse, voices, multiplier, divisor, sync);
      break;
    case 9:
      init_rhythm_3(inverse, voices, multiplier, divisor, sync);
      break;
    case 10:
      init_rhythm_4(inverse, voices, multiplier, divisor, sync);
      break;
    case 11:
      setup_jifflesNEW(inverse, voices, multiplier, divisor, sync);
      break;
    case 12:
      init_pentatonic(inverse, voices, multiplier, divisor, sync);
      break;
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
      // FIXME:	maybe make that default?
      activate_selected_synced_now(sync);	// sync and activate
      if (MENU.maybe_display_more()) {		// *then* maybe info ;)
	MENU.ln();
	flagged_pulses_info_lines();
      }
      break;

    default:	// invalid
      experiment=0;

      if (MENU.verbosity)
	MENU.outln(invalid_);
      break;
    }
    break;

  default:
    return false;	// menu entry not found
    break;
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
/* **************************************************************** */
/* **************************************************************** */


/* **************************************************************** */
/* README_pulses


README_pulses

Testing Pulses library in an early stage



   Copyright © Robert Epprecht  www.RobertEpprecht.ch   GPLv2

   http://github.com/reppr/pulses


*/
/* **************************************************************** */


void display_fraction(struct fraction *f) {
  MENU.out((*f).multiplier);
  MENU.slash();
  MENU.out((*f).divisor);
}

/* **************************************************************** */
/* **************************************************************** */
/* **************************************************************** */






