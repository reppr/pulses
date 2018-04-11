/* **************************************************************** */
/*
			pulses.ino

            http://github.com/reppr/pulses/

Copyright © Robert Epprecht  www.RobertEpprecht.ch   GPLv2


		Please do read README_pulses
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
/* **************************************************************** */
// SOURCE CODE STARTS HERE:
/* **************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
using namespace std;	// ESP8266 needs that


#include "pulses_project_conf.h"	// define special projects like instruments

#include "pulses_systems.h"		// different software systems
#include "pulses_boards.h"		// different boards
#include "pulses_configuration.h"	// your configuration
#include "pulses_sanity_checks.h"	// check some pp macros


/* **************** Menu **************** */
class Menu;
/*
  This version definines the menu INPUT routine int men_getchar();
  and the menu OUTPUT streams
  from the *program*
  not inside the Menu class
*/
#include "menu_IO_configuration.h"
#include <Menu.h>

#if defined RAM_IS_SCARE
  #define CB_SIZE	32
#else
  #define CB_SIZE	128
#endif
Menu MENU(CB_SIZE, 4, &men_getchar, MENU_OUTSTREAM, MENU_OUTSTREAM2);


/* **************** Pulses **************** */
#include <Pulses.h>

Pulses PULSES(pl_max, &MENU);


/* **************** Harmonical **************** */
#include <Harmonical.h>

Harmonical HARMONICAL(3628800uL);	// old style for a first test

#include "jiffles.h"
#include "int_edit.h"			// displaying and editing unsigned int arrays
#include "array_descriptors.h"		// make data arrays accessible for the menu, give names to the data arrays

/* **************************************************************** */
// some #define's:
#define ILLEGAL		-1
#define ILLEGAL_PIN	255


/* **************************************************************** */
// define uint8_t click_pin[CLICK_PULSES]	// see: pulses_boards.h		FIXME: CLICK_PULSES
#ifdef CLICK_PULSES
  #include "pulses_CLICK_PIN_configuration.h"	// defines click_pin[]
#endif

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

    #include "ESP8266WiFi.h"  // after that min() and max() do not work any more. Say: std::min() std::max() or _min _max
    extern "C" {
      #include "user_interface.h"
    }
  #endif

  #ifdef WIFI_OFF_mysensors
    // see: https://forum.mysensors.org/topic/5120/esp8266-with-wifi-off/3
    #include <ESP8266WiFi.h>  // after that min() and max() do not work any more. Say: std::min() std::max() or _min _max
  #endif
#endif	// ESP8266


/* **************************************************************** */
// scales
int selected_scale=ILLEGAL;

#ifndef SCALES_RAM_SIZE	// scales on small harware ressources, FIXME: test	################
  #define SCALES_RAM_SIZE 9*2+2	// small buffer might fit on simple hardware
#endif

#ifdef SCALES_RAM_SIZE
  // scales:
  unsigned int scale_RAM[SCALES_RAM_SIZE] = {0};
  unsigned int scale_RAM_length = SCALES_RAM_SIZE;
  unsigned int scale_write_index=0;
  unsigned int *scale=scale_RAM;
#else
  #error SCALES_RAM_SIZE is not defined
#endif // SCALES_RAM_SIZE

#ifndef RAM_IS_SCARE	// enough RAM?
char * scale_names[] = {
      "scale_RAM",		// 0
      "pentatonic_minor",	// 1
      "european_pentatonic",	// 2
      "mimic_japan_pentatonic",	// 3
      "minor_scale",		// 4
      "major_scale",		// 5
      "tetrachord",		// 6
      "scale_int",		// 7
      "scale_rationals",	// 8
      "octaves",		// 9
      "octaves_fifths",		// 10
      "octaves_fourths",	// 11
      "octaves_fourths_fifths",	// 12
  };

  #define n_scale_names (sizeof (scale_names) / sizeof (const char *))
#endif


// scaletabs *MUST* have 2 trailing zeros


unsigned int octaves[] = {1,1, 0,0};  				// zero terminated
unsigned int octaves_fifths[] = {1,1, 2,3, 0,0};			// zero terminated
unsigned int octaves_fourths[] = {1,1, 3,4, 0,0};		// zero terminated
unsigned int octaves_fourths_fifths[] = {1,1, 3,4, 2,3, 0,0};	// zero terminated

unsigned int scale_int[]  = {1,1, 2,1, 3,1, 4,1, 5,1, 6,1, 7,1, 8,1, 0,0};  // zero terminated
unsigned int overnotes[]  = {1,1, 1,2, 1,3, 1,4, 1,5, 1,6, 1,7, 1,8, 1,9, 1,10, 1,11, 1,12, 1,13, 1,14, 1,15, 1,16, 0,0};
unsigned int scale_rationals[]  = {1,1, 1,2, 2,3, 3,4, 5,6, 6,7, 7,8, 8,9, 9,10, 0,0};  // zero terminated

unsigned int european_pentatonic[] = {1,1, 8,9, 4,5, 2,3, 3,5, 0,0};	// scale each octave	zero terminated

unsigned int pentatonic_minor[] = {1,1, 5,6, 3,4, 2,3, 5*2,6*3, 0,0};	// scale each octave	zero terminated
// nice first try with "wrong" note:
//  unsigned int mimic_japan_pentatonic[] = {1,1, 8,9, 5,6, 2,3, 8*2,9*3, 1,2, 8,9*2, 5,12, 2,6, 8,9*3, 1,4, 0,0 };  // zero terminated
// second try:
unsigned int mimic_japan_pentatonic[] = {1,1, 8,9, 5,6, 2,3, 2*15,3*16, 0,0 };	// scale each octave	zero terminated


unsigned int minor_scale[] = {1,1, 8,9, 5,6, 3,4, 2,3, 5,8, 5,9, 0,0};	// scale each octave	zero terminated
// 1/1	9/8	6/5	4/3	3/2	8/5	9/5	2/1

// for doric_scale i try minor scale with major scale's sixth:
unsigned int doric_scale[] = {1,1, 8,9, 5,6, 3,4, 2,3, 3,5, 5,9, 0,0};	// scale each octave	zero terminated
// 1/1	9/8	6/5	4/3	3/2	8/5	9/5	2/1


unsigned int major_scale[] = {1,1, 8,9, 4,5, 3,4, 2,3, 3,5, 8,15, 0,0};	// scale each octave	zero terminated
// 24	27	30	32	36	40	45	48
// 1:1	9:8	5:4	4:3	3:2	5:3	15:8	2:1

unsigned int tetrachord[] = {1,1, 8,9, 4,5, 3,4, 0,0};			// scale each octave	zero terminated


/* **************************************************************** */

// editing jiffle data
// if we have enough RAM, we work in an int array[]
// pre defined jiffletabs can be copied there before using and editing
#ifndef JIFFLE_RAM_SIZE
  // jiffletabs *MUST* have 2 trailing zeros
  #define JIFFLE_RAM_SIZE 9*3+2	// small buffer might fit on simple hardware
#endif
unsigned int jiffle_RAM[JIFFLE_RAM_SIZE] = {0};
unsigned int jiffle_write_index=0;
unsigned int jiffle_range_bottom=0;
unsigned int jiffle_range_top=0;
unsigned int *jiffle=jiffle_RAM;


void fix_jiffle_range() {
  unsigned int i;

  if (jiffle_range_top >= JIFFLE_RAM_SIZE)
    jiffle_range_top=JIFFLE_RAM_SIZE-1;

  if (jiffle_range_bottom > jiffle_range_top) {		// swap
    i = jiffle_range_bottom;
    jiffle_range_bottom = jiffle_range_top;
    jiffle_range_top = i;
  }
};


/* **************************************************************** */
/*
  void display_names(char** names, int count, int selected);
  display an array of strings like names of scales, jiffles, experiments
  mark the selected one with an asterisk
*/
void display_names(char** names, int count, int selected) {
  MENU.ln();
  for (int i = 0; i < count; i++) {
    if (i==selected)
      MENU.out('*');
    else
      MENU.space();
    MENU.space();

    MENU.out(i);
    MENU.tab(); MENU.outln(names[i]);
  }
  MENU.ln();
}


// reset, remove all (flagged) pulses, restart selections at none
// switch GPIO and DACs off
int reset_all_flagged_pulses_GPIO_OFF() {	// reset pulses, switches GPIO and DACs off
  int cnt=0;
  for (int pulse=0; pulse<pl_max; pulse++) {  // tabula rasa
    if (PULSES.pulses[pulse].flags) {
      PULSES.init_pulse(pulse);
      cnt++;
    }
  }

  // By design click pulses *HAVE* to be defined *BEFORE* any other pulses:		FIXME: CLICK_PULSES ################
  PULSES.init_click_pulses();		//						FIXME: CLICK_PULSES ################
  init_click_pins_OutLow();		// switch them on LOW, output	current off, i.e. magnets    FIXME: CLICK_PULSES ################
  PULSES.clear_selection();		// restart selections at none

#if defined USE_DACs			// reset DACs
  dacWrite(BOARD_DAC1, 0);
#if (USE_DACs > 1)
  dacWrite(BOARD_DAC2, 0);
#endif
#endif

  PULSES.fix_global_next();

  if (MENU.verbosity) {
    MENU.out(F("\nremoved all pulses ")); MENU.outln(cnt);
    MENU.outln(F("switched pins off."));
  }

  return cnt;
};


// make selected pulses jiffle throwers
void en_jiffle_throw_selected(uint8_t action_flags) {
  for (int pulse=0; pulse<pl_max; pulse++)
    if (PULSES.pulse_is_selected(pulse))
      en_jiffle_thrower(pulse, jiffle, action_flags);

  PULSES.fix_global_next();		// just in case?

  if (!PULSES.check_maybe_do())		// maybe do it *first*
    if (MENU.maybe_display_more()) {	// else
      MENU.ln();
      selected_or_flagged_pulses_info_lines();
    }
};

// make selected pulses click
int en_click_selected() {
  int cnt=0;

  // we work on voices anyway, regardless dest
  for (int pulse=0; pulse<pl_max; pulse++)
    if (PULSES.pulse_is_selected(pulse))
      if (en_click(pulse))
	cnt++;

  if (!PULSES.check_maybe_do())		// maybe do it *first*
    if (MENU.maybe_display_more()) {	// else
      MENU.ln();
      selected_pulses_info_lines();
    }

  return cnt;
};

#if defined USE_DACs
// set_action_flags(pulse, DACsq1 | DACsq2) activates both DACs
bool set_action_flags(int pulse, unsigned int action_flags) {
  if (pulse != ILLEGAL) {
    if (pulse < pl_max) {
      PULSES.pulses[pulse].action_flags = (PULSES.pulses[pulse].action_flags | action_flags);
      return true;
    }
  }

  return false;
};

// make selected pulses produce square waves with harmonical timing

int selected_set_action_flags(unsigned int action_flags) {
  int cnt=0;

  for (int pulse=0; pulse<pl_max; pulse++)
    if (PULSES.pulse_is_selected(pulse))
      if (set_action_flags(pulse, action_flags))
	cnt++;

  return cnt;
};

// share DAC intensity between selected pulses
int selected_share_DACsq_intensity(int intensity, int channel) {
  int cnt=0;

  for (int pulse=0; pulse<pl_max; pulse++)
    if (PULSES.pulse_is_selected(pulse))
      cnt++;

  if (cnt) {
    intensity /= cnt;
    for (int pulse=0; pulse<pl_max; pulse++) {
      if (PULSES.pulse_is_selected(pulse)) {
	switch (channel) {
	case 1:
	  PULSES.pulses[pulse].dac1_intensity=intensity;
	  break;
	case 2:
	  PULSES.pulses[pulse].dac2_intensity=intensity;
	  break;
	}
      }
    }
  } else
    intensity=0;

  return intensity;
};

// share DAC intensity of selected pulses, proportional to period
void selected_DACsq_intensity_proportional(int intensity, int channel) {
  struct time sum;
  sum.time=0;
  sum.overflow=0;
  float factor;

  for (int pulse=0; pulse<pl_max; pulse++)
    if (PULSES.pulse_is_selected(pulse))
      PULSES.add_time(&PULSES.pulses[pulse].period, &sum);

  if (sum.overflow)	// FIXME: if ever needed ;)
    MENU.outln(F("ERROR: sum.overflow"));

  if (sum.time) {
    for (int pulse=0; pulse<pl_max; pulse++) {
      if (PULSES.pulse_is_selected(pulse)) {
	factor = (float) PULSES.pulses[pulse].period.time / (float) sum.time;

	switch (channel) {
	case 1:
	  PULSES.pulses[pulse].dac1_intensity = factor * intensity;
	  break;
	case 2:
	  PULSES.pulses[pulse].dac2_intensity = factor * intensity;
	  break;
	}
      }
    }
  }
}
#endif // USE_DACs



/* **************************************************************** */
// user interface variables:

int sync=1;			// syncing edges or middles of square pulses
unsigned long multiplier=1;
unsigned long divisor=1;

int selected_experiment=-1;
int voices=CLICK_PULSES;

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
  // unsigned long ticks_per_octave=10000000L;		// 10 seconds/octave
  // unsigned long ticks_per_octave=60000000L;		//  1 minute/octave
  // unsigned long ticks_per_octave=60000000L*10L;	// 10 minutes/octave
  unsigned long ticks_per_octave=60000000L*60L;	//  1 hour/octave
  // unsigned long ticks_per_octave=60000000L*60L*24;	//  1 day/octave  FIXME: overflows

// re-implement, see tuned_sweep_click()
// PULSES.ticks_per_octave = ticks_per_octave;	// do that in setup
//

#endif


#ifndef RAM_IS_SCARE	// ################ FIXME: USE_INPUTS default condition ################
  #define USE_INPUTS
#endif

#ifdef USE_INPUTS
  #include <Inputs.h>
  Inputs INPUTS(4);

  bool maybe_check_inputs() {
    static unsigned long maybe_check_inputs_cnt=0;
    static unsigned long input_priority_part=3;	// check inputs every 3rd time, priority slice

    if(INPUTS.active_inputs==0)			// something active?
      return false;

    if (++maybe_check_inputs_cnt % input_priority_part == 0 ) {	// an input's turn?
      INPUTS.do_next_input();
      return true;				// at least a sample was taken, maybe more actions
    }

    return false;				// i can wait for my turn ;)
  }
#endif // USE_INPUTS

/* **************************************************************** */
#ifdef ARDUINO
/* Arduino setup() and loop():					*/

// needed for MENU.add_page();
// void softboard_display();
// bool softboard_reaction(char token);
int softboard_page=-1;		// see: maybe_run_continuous()


#ifndef STARTUP_DELAY
  #define STARTUP_DELAY	0	// noop or yield()
#endif

#ifndef RAM_IS_SCARE	// enough RAM?
  #include "jiffles.h"
#endif

void setup() {
  delay(STARTUP_DELAY);		// yield()
  Serial.begin(BAUDRATE);	// Start serial communication.

#if defined(__AVR_ATmega32U4__) || defined(ESP8266) || defined(ESP32)	// FIXME: test ESP32  ################
  /* on ATmega32U4		Leonardo, Mini, LilyPad Arduino USB
     to be able to use Serial.print() from setup()
     we *must* do that before:
  */
  while (!Serial) { yield(); }		// wait for Serial to open
#endif

  delay(STARTUP_DELAY);

  // try to get rid of menu input garbage, "dopplet gnaeht hebt vilicht besser" ;)
  while (Serial.available())  { Serial.read(); yield(); }
  while (MENU.cb_peek() != EOF) { MENU.drop_input_token(); yield(); }

  delay(STARTUP_DELAY);

  #include "array_descriptor_setup.h"

  MENU.outln(F("\nhttp://github.com/reppr/pulses/\n"));

#ifdef USE_WIFI_telnet_menu		// do we use WIFI?
  #ifdef AUTOSTART_WIFI		// start wifi on booting? see: WiFi_stuff.ino
    AUTOSTART_WIFI
  #endif
#else // WiFi not in use, switch it off:

  #if defined(ESP8266) || defined(ESP32)	// ################ FIXME: test ################
    MENU.outln("switch WiFi off");

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
  #endif	// ESP8266
#endif // to WiFi or not

  MENU.ln();
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
  softboard_page = MENU.add_page("Hardware Softboard", 'H',	\
		 &softboard_display, &softboard_reaction, 'H');

#ifdef USE_INPUTS
  // add inputs page:
  MENU.add_page("Inputs", 'I', &inputs_display, &inputs_reaction, 'I');

  int inp=0;
  INPUTS.setup_analog_read(inp, 0, 8);				// A0, oversample=0
  INPUTS.setup_raw(inp);					// no calculations
  INPUTS.selected_inputs |= ++inp;				// selected for editor

  INPUTS.setup_analog_read(inp, 0, 8);				// A0, oversample=0
  INPUTS.setup_raw(inp);					// no calculations
  INPUTS.setup_linear(inp, 0, 255, 1023, 0, PROPORTIONAL);	// 255*x/1023
  INPUTS.selected_inputs |= ++inp;				// selected for editor
#endif


#ifdef USE_WIFI_telnet_menu
  // add WiFi page:
  MENU.add_page("WIFI", 'Y', &WiFi_menu_display, &WiFi_menu_reaction, 'Y');
#endif

  // display menu at startup:
  MENU.menu_display();

  #ifdef CLICK_PULSES
    init_click_pins_OutLow();		// make them GPIO, OUTPUT, LOW
  #endif

  #ifdef IMPLEMENT_TUNING		// implies floating point
    PULSES.ticks_per_octave = ticks_per_octave;
  #endif

  // time and pulses *must* get initialized before setting up pulses:
  PULSES.init_time();			// start time
  PULSES.init_pulses();			// init pulses
  MENU.ln();

  PULSES.fix_global_next();		// we *must* call that here late in setup();

  #ifdef AUTOSTART			// see: pulses_project_conf.h
    AUTOSTART
  #endif

  // informations about alive pulses:
  MENU.ln();
  selected_or_flagged_pulses_info_lines();

  // try to get rid of menu input garbage, "dopplet gnaeht hebt vilicht besser" ;)
  while (Serial.available())  { Serial.read(); yield(); }
  while (MENU.cb_peek() != EOF) { MENU.drop_input_token(); yield(); }
};


// bool lower_priority_tasks();
// check lower priority tasks and do the first one that needs to be done
// return true if something was done
bool low_priority_tasks() {

#ifdef IMPLEMENT_TUNING		// tuning, sweeping priority below menu		*implies floating point*
  tuning = PULSES.tuning;	// FIXME: workaround for having all 3 sweep implementations in parallel
  if (maybe_stop_sweeping())		// low priority control sweep range
    return true;

  if(maybe_display_tuning_steps())	// low priority tuning steps info
    return true;
#endif

  if (maybe_run_continuous())	        // even lower priority: maybe display input state changes.
    return true;

  return false;
}


bool lowest_priority_tasks() {

#ifdef USE_WIFI_telnet_menu
// ################ FIXME: cleanup old code ################
  // check telnet connection
//  MENU.out("check TELNET:	");

  // look for telnet client connect trial
//	  MENU.out("telnet server ");
//	  if (telnet_server.available())
//	    MENU.out("running	");
//	  else {
//	    MENU.out("trying to start	");
//	    telnet_server.begin();
//	    telnet_server.setNoDelay(true);
//	//    delay(1000);
//	    if (telnet_server.available()) {
//	      MENU.out("OK	");
//	//      delay(8000);
//	    }
//	    else
//	      MENU.out("still dead 	");
//	    }

  if (telnet_server.hasClient()) {
    MENU.out("telnet client	");

    // ################  FIXME: something sends "exit" to the menu ;)  ################
    if (!server_client || !server_client.connected()) {
      if (server_client) {
	MENU.out("server_client.stop()\t");
	server_client.stop();
      }
      server_client = telnet_server.available();
      server_client.flush();
      MENU.out("new telnet client	");
    }
// don't know about 'else', runs well without:
//	    else
//	      {
//		MENU.outln("telnet client disconnect");
//		//      server_client.flush();
//		telnet_server.available().stop();
//	      }

    MENU.ln();
  }
#endif // USE_WIFI_telnet_menu

  return false;		// ################ FIXME: return values ################
}



// stress_emergency:  looks like the value does not matter too much
//#if defined(ESP32)	// ################ FIXME: ESP32 stress ################
//unsigned int stress_emergency=4096*4096;	// high value seems appropriate
//#else
unsigned int stress_emergency=4096;	// high value seems appropriate
//#endif
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

  // descend through priorities and do first thing found
#ifdef USE_INPUTS
  if(! maybe_check_inputs())		// reading inputs can be time critical, so check early
#endif
    if(! MENU.lurk_then_do())		// MENU second in priority, check if something to do,
      if (! low_priority_tasks())		// if not, check low_priority_tasks()
	lowest_priority_tasks();		// if still nothing done, check lowest_priority_tasks()

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
  selected_or_flagged_pulses_info_lines();

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

#if defined(ESP32) || defined(ESP8266)
  yield();	// maybe good to do that, who knows?
#endif

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

// FIXME: CLICK_PULSES	comment
// Click_pulses are a sub-group of pulses that control an arduino
// digital output each.  By design they must be initiated first to get
// the low pulse indices. The pins are configured as outputs by init_click_pins_OutLow()
// and get used by clicks, jiffles and the like.

// It's best to always leave click_pulses in memory.
// You can set DO_NOT_DELETE to achieve this.
// By design click pulses *HAVE* to be defined *BEFORE* any other pulses:


void click(int pulse) {	// can be called from a pulse
  digitalWrite(PULSES.pulses[pulse].gpio, PULSES.pulses[pulse].counter & 1);
}


#ifdef IMPLEMENT_TUNING		// implies floating point
void sweep_click(int pulse) {	// can be called from a pulse
  double period = PULSES.pulses[pulse].period.time;
  double detune_number = PULSES.ticks_per_octave / PULSES.pulses[pulse].period.time;
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
  double detune = pow(2.0, 1.0/detune_number);	// fails on Arduino Mega2560

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
  PULSES.pulses[pulse].period.time = PULSES.pulses[pulse].base_time * tuning;
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
  MENU.out(F("tuning ")); MENU.out(PULSES.tuning, 6);
  MENU.tab();

  if (low_sweep_limit) {
    MENU.out(F("slowest "));
    MENU.out(low_sweep_limit, 6);
    MENU.tab();
  }
  if (high_sweep_limit) {
    MENU.out(F("fastest 1/"));
    MENU.out((double) 1/high_sweep_limit, 6);
    MENU.tab();
  }
}

void sweep_info() {
  struct time duration;

  MENU.out(F("sweep "));
  switch (sweep_up) {
  case 0:
    MENU.out(F("off"));
    break;
  case 1:
    MENU.out(F("up"));
    break;
  case -1:
    MENU.out(F("down"));
    break;
  }

  MENU.out(F("\ttime/octave "));
  duration.time = (unsigned long) PULSES.ticks_per_octave;
  duration.overflow=0;
  PULSES.display_realtime_sec(duration);

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
      PULSES.display_realtime_sec(now); MENU.tab();
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
      PULSES.display_realtime_sec(now); MENU.tab();
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
#endif // ifdef IMPLEMENT_TUNING	implies floating point


/* **************************************************************** */
#ifdef ESP32
  #include "driver/gpio.h"
  //#include "driver/rtc_io.h"
  //#include "soc/rtc_io_reg.h"
  //#include "soc/rtc.h"
  //#include "soc/io_mux_reg.h"
#endif


void init_click_pins_OutLow() {		// make them GPIO, OUTPUT, LOW
/* uint8_t click_pin[CLICK_PULSES];
   hardware pins for click_pulses:
// FIXME: CLICK_PULSES comment
   It is a bit obscure to hold them in an array indexed by [pulse]
   but it's simple and working well
*/

  int pin;

  for (int pulse=0; pulse<CLICK_PULSES; pulse++) {	// FIXME: CLICK_PULSES
    pin=click_pin[pulse];

#ifdef ESP8266	// pin 14 must be switched to GPIO on ESP8266
    // http://www.esp8266.com/wiki/lib/exe/detail.php?id=esp8266_gpio_pin_allocations&media=pin_functions.png
    if (pin==14)
      pinMode(pin, FUNCTION_3); // pin 14 must be switched to GPIO on ESP8266

#elif defined ESP32
    // see http://wiki.ai-thinker.com/_media/esp32/docs/esp32_chip_pin_list_en.pdf

    if (GPIO_IS_VALID_OUTPUT_GPIO(pin) && (pin < 6 || pin > 11))
      PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[pin], PIN_FUNC_GPIO);

    pinMode(pin, OUTPUT);	// on oldstyle Arduinos this is enough
    digitalWrite(pin, LOW);	// on oldstyle Arduinos this is enough
 }

  gpio_config_t gpioConfig;
  gpioConfig.pin_bit_mask = (1 << 2) | (1 << 14) | (1 << 32) | (1 << 33);
  gpioConfig.mode = GPIO_MODE_OUTPUT;
  gpioConfig.pull_up_en = GPIO_PULLUP_DISABLE;
  gpioConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
  gpioConfig.intr_type = GPIO_INTR_DISABLE;

  #if defined DEBUG_GPIO_OUTPUT_INITIALISATION
    MENU.outln(" gpio_config(&gpioConfig)");
  #endif

  gpio_config(&gpioConfig);
#elif
  #error TODO: fix init_click_pins_OutLow() for non ESP boards	################
#endif	// ESP32
}


void out_noFreePulses() {
  MENU.out(F("no free pulses"));
}


// make an existing pulse to a click pulse:
bool en_click(int pulse) {
  if (pulse != ILLEGAL) {
    if (pulse < CLICK_PULSES) {	// FIXME: CLICK_PULSES
      PULSES.pulses[pulse].periodic_do = (void (*)(int)) &click;
      PULSES.pulses[pulse].gpio = click_pin[pulse];
      pinMode(PULSES.pulses[pulse].gpio, OUTPUT);
      return true;
    }
  }

  return false;
};

#ifdef IMPLEMENT_TUNING		// implies floating point
// make an existing pulse to a sweep click pulse:
int en_sweep_click(int pulse)
{
  int cnt=0;

  if (pulse != ILLEGAL) {
    if (en_click(pulse)) {
      PULSES.pulses[pulse].periodic_do = (void (*)(int)) &sweep_click;
      cnt++;
    }
  }

  return cnt;
};



// make an existing pulse to a sweep_click_0 pulse:
void en_sweep_click_0(int pulse)
{
  if (pulse != ILLEGAL)
    if (en_click(pulse)) {
      PULSES.pulses[pulse].base_time = PULSES.pulses[pulse].period.time;
      PULSES.pulses[pulse].periodic_do = (void (*)(int)) &sweep_click_0;
    }
};


void en_tuned_sweep_click(int pulse)
{
  if (pulse != ILLEGAL)
    if (en_click(pulse)) {
      PULSES.activate_tuning(pulse);
      PULSES.pulses[pulse].periodic_do = (void (*)(int)) &tuned_sweep_click;
    }
}
#endif	// #ifdef IMPLEMENT_TUNING	implies floating point


int setup_click_synced(struct time when, unsigned long unit, unsigned long multiplier,
		       unsigned long divisor, int sync) {
  int pulse= PULSES.setup_pulse_synced(&click, ACTIVE, when, unit, multiplier, divisor, sync);

  if (pulse != ILLEGAL) {
    PULSES.pulses[pulse].gpio = click_pin[pulse];
    pinMode(PULSES.pulses[pulse].gpio, OUTPUT);
    digitalWrite(PULSES.pulses[pulse].gpio, LOW);
  } else {
    out_noFreePulses();
  }

  return pulse;
}


/* **************************************************************** */
// playing with chords:

//   init_div_123456(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync);
void init_div_123456(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync) {
//	  const unsigned long divisor=1L;
  unsigned long unit = multiplier * PULSES.time_unit;
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
  unsigned long unit = multiplier * PULSES.time_unit;
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
    no_g_inverse();
    return;
  }
  unsigned long unit = multiplier * PULSES.time_unit;
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


// old style, obsolete	// TODO: remove?
int prepare_magnets(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync) {
  if (inverse) {
    no_g_inverse();
    return 0;
  }

  scale = pentatonic_minor;
  PULSES.select_n(voices);

#define COMPATIBILITY_PERIOD_3110	// sets the period directly
#ifdef COMPATIBILITY_PERIOD_3110	// TODO: remove
  for (int pulse=0; pulse<voices; pulse++)
    if (PULSES.pulse_is_selected(pulse)) {
      PULSES.reset_and_edit_pulse(pulse, PULSES.time_unit);
      PULSES.pulses[pulse].period.time = 3110;	// brute force for compatibility ;)
      PULSES.pulses[pulse].period.overflow = 0;	// brute force for compatibility ;)
      en_jiffle_thrower(pulse, jiffle, 0);
    }
  int apply_scale_on_period(int voices, unsigned int *scale, bool octaves=true);	// this code is obsolete anyway ################
  apply_scale_on_period(voices, scale, true);
#else	// compatibility problems
  prepare_scale(false, voices, multiplier, divisor, sync, scale, true);
#endif

  // int apply_scale_on_period(int voices, unsigned int *scale) {

  // prepare_scale(false, voices, 4096*12, 41724, 0, scale);
  // prepare_scale(false, voices, multiplier, divisor, sync, scale);
//	  for (int pulse=0; pulse<pl_max; pulse++)
//	    if (PULSES.pulse_is_selected(pulse)) {
//	      PULSES.reset_and_edit_pulse(pulse, PULSES.time_unit);
//	    }

  //  apply_scale_on_period(voices, scale);

  // jiffle=jiff4096;
//  prepare_scale(false, voices, 1, 1, 0, scale);
//  apply_scale_on_period(voices, scale);
//  prepare_scale(false, 8, 32768, 41727, 0, scale);
//	 scale = pentatonic_minor;
//	  PULSES.selected_pulses=~0;
//	  int prepared = prepare_scale(false, 8, 32768, 41727, 0, scale);
//	  if (prepared != 8)
//	    MENU.out(F("prepared ")); MENU.out(prepared); MENU.slash(); MENU.outln(voices);
//	  select_flagged();
//	  //  PULSES.reset_and_edit_selected();
//	//	  for (int pulse=0; pulse<pl_max; pulse++)
//	//	    if (PULSES.pulse_is_selected(pulse))
//	//	      PULSES.divide_period(pulse, 41724);
//
//	  //  select_flagged();
  //jiffle=harmonics4;
  // unsigned int harmonics4 = {1,1,1024, 1,2,1024, 1,3,1024, 1,4,1024, 0,0};

//  selected_or_flagged_pulses_info_lines();
}


// ****************************************************************
void select_flagged() {
  PULSES.clear_selection();
  for (int pulse=0; pulse<pl_max; pulse++)
    if (PULSES.pulses[pulse].flags)
      PULSES.select_pulse(pulse);
}


void select_all() {
  PULSES.clear_selection();
  for (int pulse=0; pulse<pl_max; pulse++)
    PULSES.select_pulse(pulse);
}


void select_alive() {
  PULSES.clear_selection();
  for (int pulse=0; pulse<pl_max; pulse++)
    if(PULSES.pulses[pulse].flags && (PULSES.pulses[pulse].flags != SCRATCH))
      PULSES.select_pulse(pulse);
}


// ****************************************************************
/* scale[]
/* a scale array has elements of multiplier/divisor pairs
   each is the integer representation of a rational number
   very useful for all kind of things like scales, chords, rhythms */

int prepare_scale(bool inverse, int voices, unsigned long multiplier, unsigned long divisor, int sync, unsigned int *scale, bool octaves=true) {
/* prepare a couple of pulses based on a scale array.
   up to 'voices' pulses are created among the selected ones.
   return number of prepared pulses */
  if(scale[0]==0)  return 0;	// error, no data

  if (inverse) {
    no_g_inverse();
    return 0;
  }

  int prepared=0;
  unsigned long unit = multiplier * PULSES.time_unit;
  unit /= divisor;

  if (octaves)
    display_name5pars("prepare_scale fill octaves", inverse, voices, multiplier, divisor, sync);
  else
    display_name5pars("prepare_scale", inverse, voices, multiplier, divisor, sync);

  struct time now;
  PULSES.get_now();
  now=PULSES.now;

  unsigned long this_period;
  struct time new_period;
  int pulse=0;
  int octave=1;	// 1,2,4,8,...
  for (int note=0; prepared<=voices; note++) {
    multiplier = scale[note*2];
    if (multiplier==0) {
      if (octaves) {
	octave *= 2;	// one octave higher
	note = 0;		// restart at first note
	multiplier = scale[note*2];
      } else
	goto global_next;		// multiplier==0, end
    }

    divisor = scale[note*2+1];
    if (divisor==0)  goto global_next;	// divisor==0, error, end
    divisor *= octave;

    for (; pulse<pl_max; pulse++) {
      if (PULSES.pulse_is_selected(pulse)) {
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
    if (pulse==pl_max)	// all available pulses have been tried, give up
      break;
  }

 global_next:
  PULSES.fix_global_next();
  return prepared;
}


int apply_scale_on_period(int voices, unsigned int *scale, bool octaves=true) {
  // FIXME: octaves are untested here ################
  if(scale[0]==0)  return 0;	// error, no data

  struct time new_period;
  int applied=0;

//  int pulse=0;
  int octave=1;	// 1,2,4,8,...
  for (int note=0, pulse=0; applied<voices; note++) {
    multiplier = scale[note*2];
    if (multiplier==0) {
      if (octaves) {
	octave *= 2;	// one octave higher
	note = 0;	// restart at first note
	multiplier = scale[note*2];
      } else
	goto global_next;		// multiplier==0, end
    }

    divisor=scale[note*2+1];
    if (divisor==0)
      goto global_next;		// divisor==0: error, end
    divisor *= octave;

    for (; pulse<pl_max; pulse++) {
      if (PULSES.pulse_is_selected(pulse)) {
	new_period = PULSES.pulses[pulse].period;
	PULSES.mul_time(&new_period, multiplier);
	PULSES.div_time(&new_period, divisor);
	PULSES.pulses[pulse].period = new_period;
	applied++;
	pulse++;
	break;
      }
    }
    if (pulse==pl_max)	// all available pulses have been tried, give up
      break;
  }

 global_next:
  PULSES.fix_global_next();
  return applied;
}


bool tune_2_scale(int voices, unsigned long multiplier, unsigned long divisor, int sync, unsigned int selected_scale, unsigned int *scale) {
  int pulse;
  struct time base_period;
  base_period.overflow = 0;
  base_period.time = multiplier * PULSES.time_unit;
  base_period.time /= divisor;

  if (selected_scale != ILLEGAL) {
    if (MENU.verbosity >= VERBOSITY_SOME) {
      MENU.out(F("tune to scale "));
      MENU.out(selected_scale);
      MENU.space();
      MENU.out(scale_names[selected_scale]);
      MENU.tab();
      MENU.out(voices);
      MENU.outln(F(" voices"));
    }

    if (voices>0) {
      PULSES.select_n(voices);

      for (pulse=0; pulse<voices; pulse++) {
	//	PULSES.init_pulse(pulse);			// initialize new
	PULSES.pulses[pulse].period = base_period;		// set all voices to base note
	//	PULSES.pulses[pulse].period.overflow = 0;
      }

      // now apply scale:
      apply_scale_on_period(voices, scale, true);
      return true;
    }
  } else
    if (MENU.verbosity >= VERBOSITY_ERROR)
      MENU.outln(F("no scale"));

  return false;
};


// ****************************************************************
void init_chord_1345689a(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync) {
  unsigned long unit = multiplier * PULSES.time_unit;
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
  const unsigned long unit = scaling * PULSES.time_unit;
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
  unsigned long unit = multiplier * PULSES.time_unit;
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
  unsigned long unit = multiplier * PULSES.time_unit;
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
  unsigned long unit = multiplier * PULSES.time_unit;
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
  unsigned long unit = multiplier * PULSES.time_unit;
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
#define CODE_PULSES	0		// dest code pulses: apply PULSES.selected_pulses
#define CODE_TIME_UNIT	1		// dest code PULSES.time_unit
unsigned char dest = CODE_PULSES;


/* **************************************************************** */
/* Menu UI							*/

// make an existing pulse to display 1 info line:
void en_info(int pulse)
{
  if (pulse != ILLEGAL) {
    PULSES.pulses[pulse].periodic_do = (void (*)(int)) &pulse_info_1line;
  }
}

void pulse_info_1line(int pulse) {	// one line pulse info, short version
  unsigned long realtime=micros();		// let's take time *before* serial output

  if (PULSES.pulse_is_selected(pulse))
    MENU.out('*');
  else
    MENU.space();


  MENU.out(F(" PULSE "));
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

  if (MENU.verbosity >= VERBOSITY_SOME) {
    MENU.space();
    MENU.out_flags_();
    MENU.outBIN(PULSES.pulses[pulse].flags, 8);
  }

  MENU.tab();
  PULSES.print_period_in_time_units(pulse);

  MENU.tab();
  display_payload(pulse);

  if (PULSES.pulses[pulse].action_flags) {
    MENU.tab();
    MENU.out(F("Af:"));
    MENU.outBIN(PULSES.pulses[pulse].action_flags, 4);
    MENU.space();
    if (PULSES.pulses[pulse].action_flags & CLICKs)
      MENU.out('C');
    if (PULSES.pulses[pulse].action_flags & DACsq1) {
      MENU.out(F("Q1:"));
      MENU.pad(PULSES.pulses[pulse].dac1_intensity, 4);
    }
    if (PULSES.pulses[pulse].action_flags & DACsq2) {
      MENU.out(F("Q2:"));
      MENU.pad(PULSES.pulses[pulse].dac2_intensity, 4);
    }
  }

  if(PULSES.pulses[pulse].dest_action_flags \
     || (PULSES.pulses[pulse].dac1_intensity) || (PULSES.pulses[pulse].dac2_intensity)) {
    MENU.tab();
    MENU.out(F("daf:"));
    MENU.outBIN(PULSES.pulses[pulse].dest_action_flags, 4);

    MENU.out(F("i1:"));
    MENU.pad(PULSES.pulses[pulse].dac1_intensity, 4);

    MENU.out(F("i2:"));
    MENU.pad(PULSES.pulses[pulse].dac2_intensity, 4);
  }

  if (MENU.verbosity >= VERBOSITY_SOME) {
    struct time sum = PULSES.pulses[pulse].next;
    PULSES.get_now();
    struct time delta =PULSES.now;
    PULSES.sub_time(&delta, &sum);

    MENU.tab();
    MENU.out(F("expected in"));
    PULSES.display_realtime_sec(sum);
    // PULSES.display_real_ovfl_and_sec(sum);	// debugging
  }

  if (PULSES.pulse_is_selected(pulse)) {
    MENU.space();
    MENU.out('*');
  }

  MENU.ln();
}


// pulse_info() as paylod for pulses: print pulse info:
void pulse_info(int pulse) {

  MENU.out(F("*** PULSE info "));
  MENU.out(pulse);
  MENU.slash();
  MENU.out((unsigned int) PULSES.pulses[pulse].counter);

  MENU.tab();
  display_payload(pulse);

  MENU.out_flags_();
  MENU.outBIN(PULSES.pulses[pulse].flags, 8);
  MENU.ln();

  MENU.out(F("pin ")); MENU.out((int) PULSES.pulses[pulse].gpio);
  MENU.out(F("\tindex ")); MENU.out(PULSES.pulses[pulse].index);
  MENU.out(F("\ttimes ")); MENU.out(PULSES.pulses[pulse].count);
  MENU.out(F("\tcntd ")); MENU.out(PULSES.pulses[pulse].countdown);
  MENU.out(F("\tdata ")); MENU.out(PULSES.pulses[pulse].data);
  MENU.out(F("\ttime ")); MENU.out(PULSES.pulses[pulse].base_time);

  MENU.ln();		// start next line

  MENU.out((float) PULSES.pulses[pulse].period.time / (float) PULSES.time_unit, 6);
  MENU.out(F(" time"));

  MENU.out(F("\tpulse/ovf "));
  MENU.out((unsigned int) PULSES.pulses[pulse].period.time);
  MENU.slash();
  MENU.out(PULSES.pulses[pulse].period.overflow);

  MENU.tab();
  PULSES.display_realtime_sec(PULSES.pulses[pulse].period);
  MENU.space();
  MENU.out(F("pulse "));

  MENU.ln();		// start next line

  MENU.out(F("last/ovfl "));
  MENU.out((unsigned int) PULSES.pulses[pulse].last.time);
  MENU.slash();
  MENU.out(PULSES.pulses[pulse].last.overflow);

  MENU.out(F("   \tnext/ovfl "));
  MENU.out(PULSES.pulses[pulse].next.time);
  MENU.slash();
  MENU.out(PULSES.pulses[pulse].next.overflow);

  MENU.tab();
  MENU.out(F("expected "));
  PULSES.display_realtime_sec(PULSES.pulses[pulse].next);

  MENU.ln();		// start last line
  PULSES.time_info();

  MENU.ln();		// traling empty line
}


// make an existing pulse to display multiline pulse info:
void en_INFO(int pulse) {	// FIXME: to lib Pulses
  if (pulse != ILLEGAL) {
    PULSES.pulses[pulse].periodic_do = (void (*)(int)) &pulse_info;
  }
}


void display_payload(int pulse) {
  void (*scratch)(int);
  scratch=&click;
  if (PULSES.pulses[pulse].periodic_do == scratch) {
    MENU.out("click  ");
    MENU.out((int) PULSES.pulses[pulse].gpio);
    return;
  }

#ifdef IMPLEMENT_TUNING		// implies floating point
  scratch=&tuned_sweep_click;
  if (PULSES.pulses[pulse].periodic_do == scratch) {
    MENU.out(F("tuned_sweep_click "));
    MENU.out((int) PULSES.pulses[pulse].gpio);
    return;
  }

  scratch=&sweep_click;
  if (PULSES.pulses[pulse].periodic_do == scratch) {
    MENU.out(F("sweep_click "));
    MENU.out((int) PULSES.pulses[pulse].gpio);
    return;
  }

  scratch=&sweep_click_0;
  if (PULSES.pulses[pulse].periodic_do == scratch) {
    MENU.out(F("sweep_click_0 "));
    MENU.out((int) PULSES.pulses[pulse].gpio);
    return;
  }
#endif	// #ifdef IMPLEMENT_TUNING	implies floating point

  scratch=&do_jiffle;
  if (PULSES.pulses[pulse].periodic_do == scratch) {
    MENU.out(F("do_jiffle:"));
    MENU.out(array2name(JIFFLES, (unsigned int*) PULSES.pulses[pulse].data));
    MENU.space(2);
    MENU.out((int) PULSES.pulses[pulse].gpio);
    return;
  }

  scratch=&do_throw_a_jiffle;
  if (PULSES.pulses[pulse].periodic_do == scratch) {
    MENU.out(F("seed jiff:"));
    MENU.out(array2name(JIFFLES, (unsigned int*) PULSES.pulses[pulse].data));
    MENU.space(2);
    MENU.out((int) click_pin[pulse]);
    return;
  }

  scratch=&pulse_info;
  if (PULSES.pulses[pulse].periodic_do == scratch) {
    MENU.out(F("pulse_info"));
    return;
  }

  scratch=&pulse_info_1line;
  if (PULSES.pulses[pulse].periodic_do == scratch) {
    MENU.out(F("info line"));
    return;
  }

  scratch=NULL;
  if (PULSES.pulses[pulse].periodic_do == scratch) {
    MENU.out(F("NULL\t"));		// 8 char positions at least
    return;
  }

  MENU.out(F("UNKNOWN\t"));
}


void selected_pulses_info_lines() {
  int count=0;

  for (int pulse=0; pulse<pl_max; ++pulse) {
    if (PULSES.pulse_is_selected(pulse)) {
      pulse_info_1line(pulse);
      count++;
    }
  }

  if (count)
    MENU.ln();
}


void flagged_pulses_info() {
  int count=0;

  for (int pulse=0; pulse<pl_max; ++pulse)
    if (PULSES.pulses[pulse].flags) {		// any flags set?
      pulse_info_1line(pulse);
      count++;
    }

  if (count == 0)
    MENU.outln(F("no flagged pulses"));
}


void selected_or_flagged_pulses_info_lines() {
  int count=0;
  for (int pulse=0; pulse<pl_max; ++pulse)
    if (PULSES.pulses[pulse].flags || (PULSES.pulse_is_selected(pulse))) { // any flags || selected
      pulse_info_1line(pulse);
      count++;
    }

  if (count == 0) {
    MENU.outln(F("no selected or flagged pulses"));
    if(PULSES.how_many_selected())
      PULSES.show_selected_mask();
  }

  MENU.ln();
}


/* **************************************************************** */
// make an existing pulse to a jiffle thrower pulse:
void en_jiffle_thrower(int pulse, unsigned int *jiffletab, uint8_t action_mask)
{
  if (pulse != ILLEGAL) {
    PULSES.pulses[pulse].periodic_do = &do_throw_a_jiffle;
    PULSES.pulses[pulse].data = (unsigned int) jiffletab;
    PULSES.pulses[pulse].dest_action_flags |= action_mask;
  }
}


int init_jiffle(unsigned int *jiffletab, struct time when, struct time new_period, int origin_pulse)
{
  int pulse;
  struct time jiffle_period=new_period;

  jiffle_period.time = new_period.time * jiffletab[0] / jiffletab[1];

  pulse = PULSES.setup_pulse(&do_jiffle, ACTIVE, when, jiffle_period);
  if (pulse != ILLEGAL) {
    PULSES.pulses[pulse].action_flags = PULSES.pulses[origin_pulse].dest_action_flags; // set actions
    PULSES.pulses[pulse].gpio = click_pin[origin_pulse];	// set pin
    PULSES.pulses[pulse].index = 0;				// init phase 0
    PULSES.pulses[pulse].countdown = jiffletab[2];		// count of first phase
    PULSES.pulses[pulse].data = (unsigned int) jiffletab;
    PULSES.pulses[pulse].base_time = new_period.time;
#if defined USE_DACs
    PULSES.pulses[pulse].dac1_intensity = PULSES.pulses[origin_pulse].dac1_intensity;
  #if (USE_DACs > 1)
    PULSES.pulses[pulse].dac2_intensity = PULSES.pulses[origin_pulse].dac2_intensity;
  #endif
#endif
  }

  return pulse;
}


void do_throw_a_jiffle(int pulse) {		// for pulse_do
  // pulses[pulse].data	= (unsigned int) jiffletab;

  // start a new jiffling pulse now (next [pulse] is not yet updated):
  unsigned int *this_jiff=(unsigned int *) PULSES.pulses[pulse].data;

  // check for empty jiffle first:
  bool has_data=true;
  for (int i=0; i<3; i++)
    if (this_jiff[i]==0)
      has_data=false;

  if (has_data)	// there *is* jiffle data
    init_jiffle((unsigned int *) PULSES.pulses[pulse].data,	\
	      PULSES.pulses[pulse].next, PULSES.pulses[pulse].period, pulse);
  else	// zero in first triplet, *invalid* jiffle table.
    MENU.outln(F("no jiffle"));
}


/* **************************************************************** */
// pulses menu:

// what is selected?

void print_selected() {
  switch (dest) {
  case CODE_PULSES:
    PULSES.show_selected_mask();
    break;

  case CODE_TIME_UNIT:
    MENU.out_selected_();
    MENU.outln(F("time unit"));
    break;
  }
}


void info_select_destination_with(bool extended_destinations) {
  MENU.outln(F("SELECT DESTINATION for '= * / s K P n g i F j :' to work on:"));
  print_selected();
  MENU.out(F("select pulse with "));

  MENU.out_ticked_hexs(min(pl_max,16));

  MENU.outln(F("\n'.a'=select *all* click pulses\t'.A'=*all* pulses\t'.v'=voices\t'.L'=all alive\t'x'=none\t'.~'=invert selection"));

  if(extended_destinations) {	// FIXME: will that ever be used??? ################
    MENU.out(F("u=select "));  MENU.out(F("time unit"));
    if(dest == CODE_TIME_UNIT) {
      MENU.outln(F("\t(selected)"));
    } else
      MENU.ln();
    MENU.ln();
  }
}


// variables used to setup the experiments

#ifndef RAM_IS_SCARE	// enough RAM?
char * experiment_names[] = {		// FIXME: const char * experiment_names would be better
      "(invalid)",				// 0
      "setup_jiffle128",			// 1
      "init_div_123456",			// 2
      "ESP8266 Frogs",				// 3
      "setup_jiffles2345",			// 4
      "init_123456",				// 5
      "init_chord_1345689a",			// 6
      "init_rhythm_1",				// 7
      "init_rhythm_2",				// 8
      "init_rhythm_3",				// 9
      "init_rhythm_4",				// 10
      "setup_jifflesNEW",			// 11
      "init_pentatonic",			// 12
      "magnets: The Harmonical Strings Christmas Evening Sounds",  // 13
      "magnets on strings 2",			// 14
      "magnets on strings 3",			// 15
      "piezzos on low strings 2016-12-28",	// 16
      "magnets on steel strings, japanese",	// 17
      "nylon stringed wooden box, piezzos",	// 18
      "tuned sweep",				// 19
      "arpeggio4096\tpentatonic",		// 20
      "arpeggio4096down\tpentatonic",		// 21
      "arpeggio_cont\tpentatonic",		// 22
      "arpeggio_and_down\tpentatonic",		// 23
      "stepping_down\tpentatonic",		// 24
      "back_to_ground\tpentatonic rhythm slowdown", // 25
      "arpeggio_and_sayling\tpentatonic",	// 26
      "simple_theme\tpentatonic",		// 27
      "peepeep4096\tpentatonic\tfor tuning",	// 28
      "KALIMBA7 tuning",			// 29
      "KALIMBA7 jiff pent minor",		// 30
      "KALIMBA7 jiff pent euro",		// 31
      "ESP32_12 pent euro",			// 32
      "minor scale",				// 33
      "major scale",				// 34
      "tetrachord",				// 35
      "more voices please",			// 36
      "Guitar and other Instruments"		// 37
//    "(invalid)",				// over
  };

  #define n_experiment_names (sizeof (experiment_names) / sizeof (const char *))
#endif // ! RAM_IS_SCARE


bool g_inverse=false;	// bottom DOWN/up GPIO click-pin mapping
/*
  'g_inverse' works when setting up an experiment creating pulses
	      other pulses aren't affected
	      some experiments do not implement that

  'reverse_click_pins()' as alternative:
  'reverse_click_pins()' works on the global click_pin[] array
 			 the pulses won't notice but play with new pin mapping */


/* **************************************************************** */
// special menu modes, like numeric input for jiffletabs
#define JIFFLE_ENTRY_UNTIL_ZERO_MODE	1	// menu_mode for unsigned integer data entry, stop at zero

/* **************************************************************** */
uint8_t selected_actions = DACsq1 | DACsq2;

void menu_pulses_display() {
  MENU.outln(F("http://github.com/reppr/pulses/"));

  MENU.out(F("pulses "));
  MENU.out(PULSES.get_pl_max());
  MENU.space(2);
  MENU.out(F("GPIO "));
  MENU.outln(CLICK_PULSES);
  MENU.ln();
  MENU.outln(F("?=help\t.=flagged info\t:=selected info"));

  MENU.ln();
  info_select_destination_with(false);

  MENU.out(F("u=select "));  MENU.out(F("time unit"));
  MENU.out("  (");
  MENU.out(PULSES.time_unit);
  MENU.out(F(" microseconds"));
  MENU.out(F(" = "));
  MENU.out((float) (1000000.0 / (float) PULSES.time_unit), 6);
  MENU.outln(F(" per second)"));

  MENU.ln();
  MENU.out(F("s=switch pulse on/off"));
  MENU.tab();
  MENU.out(F("M=deactivate ALL\tX=remove ALL\tK=kill\n\nCREATE PULSES\tstart with 'P'\nP=new pulse\tg=en-click\tj=en-jiffle\tN=en-noop\ti=en-info\tF=en-INFO\nS=sync\tn=sync now "));
  MENU.outln(sync);

  MENU.out(F("E=enter experiment (")); MENU.out(selected_experiment); MENU.out(')');
  MENU.out(F("\tV=voices for experiment (")); MENU.out(voices); MENU.out(F(")"));
  MENU.out(F("\tO=action_flags (")); MENU.outBIN(selected_actions, 4); MENU.outln(')');
  MENU.out(F("g~=toggle pin mapping (bottom "));
  if (g_inverse)
    MENU.out(F("up"));
  else
    MENU.out(F("down"));
  MENU.outln(F(")\tZ=reverse_click_pins"));

  MENU.out(F("Scale (")); MENU.out(multiplier);MENU.slash();  MENU.out(divisor);
  MENU.out(F(")\t'*!'=multiplier '/!'=divisor"));

  MENU.ln();
}


int setup_jiffle_thrower_synced(struct time when,
				unsigned long unit,
				unsigned long multiplier, unsigned long divisor,
				int sync, unsigned int *jiffletab)
{
  int pulse= PULSES.setup_pulse_synced(&do_throw_a_jiffle, ACTIVE,
			       when, unit, multiplier, divisor, sync);
  if (pulse != ILLEGAL) {
    PULSES.pulses[pulse].data = (unsigned int) jiffletab;
  } else {
    out_noFreePulses();
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

void set_jiffle_RAM_value(int new_value) {
  char input;
  jiffle[jiffle_write_index++]=new_value;

  // jiffletabs *MUST* have 2 trailing zeros	// ################ FIXME: ################
  if (jiffle_write_index >= (JIFFLE_RAM_SIZE - 2)) {	// array full?
    jiffle[jiffle_write_index--]=0;

    // drop all remaining numbers and delimiters from input
    bool yes=true;
    while (yes) {
      input=MENU.drop_input_token();
      switch (input) {
      case ' ':  case ',':
      case '0':  case '1':  case '2':  case '3':  case '4':
      case '5':  case '6':  case '7':  case '8':  case '9':
	yes=true;
	break;

      default:
	yes=false;
      }
    }
  } else
    if (new_value==0)	// value was zero, stop input
      set_jiffle_RAM_value_0_stop();
}


// as a zero was written stop receiving further input, cleanup
void set_jiffle_RAM_value_0_stop() {
  if (jiffle_write_index >= (JIFFLE_RAM_SIZE - 2))
    jiffle_write_index = JIFFLE_RAM_SIZE - 2;
//  jiffle[jiffle_write_index] = 0;	// store a trailing zero

  jiffle[JIFFLE_RAM_SIZE - 1 ] = 0;	// zero out last 2 array elements (savety net)
  jiffle[JIFFLE_RAM_SIZE - 2 ] = 0;
  MENU.menu_mode=0;				// stop numeric data input
  //  jiffle_write_index=0;		// no, we leave write index as is

  display_jiffletab(jiffle);		// put that here for now
}


void load2_jiffle_RAM(unsigned int *source) {	// double zero terminated
  unsigned int data;
  unsigned int cnt=0;	// data counter

  while (jiffle_write_index < (JIFFLE_RAM_SIZE - 2)) {
    data=source[cnt++];
    if (data==0) {
      --cnt;
      break;
    }

    set_jiffle_RAM_value(data);
  }

  //  jiffle_write_index += cnt;
}


struct fraction jiffletab_len(unsigned int *jiffletab) {
  static struct fraction f;	// keep return value
  struct fraction scratch;
  unsigned int multiplier, divisor, count;

  f.multiplier = 0;
  f.divisor=1;
  for (int i=0;; i+=JIFFLETAB_INDEX_STEP) {
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


void display_jiffletab(unsigned int *jiffle) {
  if (jiffle == NULL)	// silently ignore undefined
    return;

  struct fraction sum;
  sum.multiplier = 0;
  sum.divisor = 1;
  bool was_zero=false;

  int i = pointer2index(JIFFLES, jiffle);	// check DB first
  unsigned int len=0;
  if (i > 0)
    len = JIFFLES[i].len/sizeof(int);		// len is known

  // first line:
#ifndef RAM_IS_SCARE	// FIXME: ?? ################
  MENU.out(JIFFLES[0].name); MENU.out(':'); MENU.space();
  MENU.out(array2name(JIFFLES, jiffle));  MENU.tab();
#endif
  MENU.out(F("editing "));
  if (MENU.menu_mode==JIFFLE_ENTRY_UNTIL_ZERO_MODE) {
    MENU.out(F("on\tclose with '}'"));
    if (MENU.verbosity >= VERBOSITY_SOME)
      MENU.out(F("\tmove cursor < >\trange borders | \""));
  } else
    MENU.out(F("off\tstart edit with '{'"));

  MENU.ln();

  // second line {data,data, ...}:
  MENU.out('{');
  for (int i=0; ; i++) {
    if ((i % JIFFLETAB_INDEX_STEP) == 0)
      MENU.space();
    if (i==jiffle_range_bottom)
      if (jiffle_range_top)	// no range, no sign
	MENU.out('|');
    if (i==jiffle_write_index)
      MENU.out("<");
    MENU.out(jiffle[i]);
    if (i==jiffle_write_index)
      MENU.out(">");
    if (i==jiffle_range_top)
      if (jiffle_range_top)	// no range, no sign
	MENU.out('"');
    MENU.out(",");
    if (jiffle[i] == 0) {
      if (was_zero)	// second zero done, stop it
	break;

      was_zero=true;	// first zero, continue *including* both zeroes
    } else
      was_zero=false;
  }
  MENU.space(); MENU.out('}');

  sum = jiffletab_len(jiffle);
  MENU.tab();
  MENU.out(F("length ")); display_fraction(&sum);
  MENU.tab();
  MENU.out(F("cursor "));
  MENU.out(jiffle_write_index); MENU.slash(); MENU.out(len);
  MENU.ln();
}


void do_jiffle (int pulse) {	// to be called by pulse_do
  // PULSES.pulses[pulse].gpio	click pin
  // PULSES.pulses[pulse].index	jiffletab index
  // PULSES.pulses[pulse].countdown		count down
  // PULSES.pulses[pulse].data			jiffletab[] pointer
  // PULSES.pulses[pulse].base_time		base period = period of starting pulse

  digitalWrite(PULSES.pulses[pulse].gpio, PULSES.pulses[pulse].counter & 1);	// click

  if (--PULSES.pulses[pulse].countdown > 0)				// countdown, phase endid?
    return;						//   no: return immediately

  // if we arrive here, phase endid, start next phase if any:
  unsigned int* jiffletab = (unsigned int *) PULSES.pulses[pulse].data;	// read jiffletab[]
  PULSES.pulses[pulse].index += JIFFLETAB_INDEX_STEP;
  int base_index = PULSES.pulses[pulse].index;		// readability
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
    PULSES.pulses[pulse].base_time * jiffletab[base_index] / jiffletab[base_index+1];
  PULSES.pulses[pulse].countdown = jiffletab[base_index+2];		// count of next phase
}

// pre-defined jiffle pattern:
void setup_jiffles2345(bool g_inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync) {
  if (g_inverse) {
    no_g_inverse();
    return;
  }
  unsigned long unit = multiplier * PULSES.time_unit;
  unit /= divisor;

  display_name5pars("jiffles2345", g_inverse, voices, multiplier, divisor, sync);

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
  {2,1024*3,4, 1,1024,64, 1,2048,64, 1,512,4, 1,64,3, 1,32,1, 1,16,2, 0,0};	// nice short jiffy

unsigned int jiff0[] =
  {1,16,4, 1,24,6, 1,128,16, 1,1024,64, 1,2048,128, 1,4096,256, 1,2048,64, 1,4096,128, 1,32,2, 0,0}; // there *must* be a trailing zero.);

unsigned int jiff1[] =
  {1,512,8, 1,1024,16, 1,2048,32, 1,1024,16, 0,0};

unsigned int jiff2[] =
  {1,2096,4, 1,512,2, 1,128,2, 1,256,2, 1,512,8, 1,1024,32, 1,512,4, 1,256,3, 1,128,2, 1,64,1, 0,0};


void setup_jifflesNEW(bool g_inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync) {
  if (g_inverse) {
    no_g_inverse();
    return;
  }
  unsigned long unit = multiplier * PULSES.time_unit;
  unit /= divisor;

  display_name5pars("setup_jifflesNEW", g_inverse, voices, multiplier, divisor, sync);

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
  first to scale unit from PULSES.time_unit
  then reset to build the jiffle thrower pulses
    multiplier=1
    divisor = 1, 2, 3, 4, ...
  */
  unsigned long unit = multiplier * PULSES.time_unit;
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
void setup_jiffles0(bool g_inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync) {
  if (g_inverse) {
    no_g_inverse();
    return;
  }
  unsigned long unit = multiplier * PULSES.time_unit;
  unit /= divisor;

  display_name5pars("setup_jiffles0", g_inverse, voices, multiplier, divisor, sync);

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

  multiplier=6;
  setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiffletab0);

//	  multiplier=8;
//	  setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiffletab0);
//
//	  multiplier=9;
//	  setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiffletab0);
//
//	  multiplier=10;
//	  setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiffletab0);

  PULSES.fix_global_next();
}

/*
  'g_inverse' works when setting up an experiment creating pulses
 	      other pulses aren't affected
	      some experiments do not implement that

  'reverse_click_pins()' as alternative:
  'reverse_click_pins()' works on the global click_pin[] array
 			 the pulses won't notice but play with new pin mapping */

bool click_pins_inverted=false;
void reverse_click_pins() {
  uint8_t scratch;
  for (int i=0, j=CLICK_PULSES-1; i<j; i++, j--) {
      scratch=click_pin[i];
      click_pin[i]=click_pin[j];
      click_pin[j]=scratch;
  }

  click_pins_inverted=!click_pins_inverted;
}

// ****************************************************************
// menu_serial_program_reaction()


// display helper functions:
void short_info() {
  MENU.ln();
  PULSES.time_info();

  MENU.ln();
  selected_or_flagged_pulses_info_lines();
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

void display_name5pars(char* name, bool g_inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync) {
  if (MENU.verbosity) {
    MENU.out((char *) name);
    MENU.out("(");
    MENU.out(g_inverse);
    display_next_par(voices);
    display_next_par(multiplier);
    display_next_par(divisor);
    display_last_par(sync);
  }
}

// display helper function no_g_inverse()
void no_g_inverse() {
    MENU.outln(F("no 'g_inverse'\ttry 'Z' instead"));
    extern bool g_inverse;
    g_inverse=false;
}

// display factor function show_scaling();
void show_scaling() {
  MENU.out(F("multiplier/divisor "));
  MENU.out(multiplier); MENU.slash(); MENU.outln(divisor);
}

// for old style 'experiment'
void Press_toStart() {
  MENU.outln(F("Press '!' to start"));
}

#if defined USE_DACs	// ################ TODO: remove
int s1=0;
int s2=0;
#endif

bool menu_pulses_reaction(char menu_input) {
  static unsigned long input_value=0;
  static unsigned long calc_result=0;
  struct time now, time_scratch;
  pulses_mask_t bitmask;
  char next_token;	// for multichar commands

  switch (menu_input) {
  case '?':	// help, overrides common menu entry for '?'
    MENU.menu_display();	// as common
    short_info();		// + short info
    break;

  case '.':	// ".xxx" select 16bit pulses masks  or  "." short info: time and flagged pulses info
    switch (MENU.cb_peek()) {
    case ' ':
      MENU.drop_input_token();	// ' ' no 'break;'  display short_info

    case EOF:			// '.' and '. ' display short_info
      short_info();
      break;

    case '?':			// '.?' PULSES.show_selected_mask();
      MENU.drop_input_token();
      PULSES.show_selected_mask();
      break;

    case 'M':  case 'm':	// '.M<num>' select HEX 16bit mask
      MENU.drop_input_token();
      input_value = PULSES.hex_input_mask_index;
      if (MENU.maybe_calculate_input(&input_value)) {
	if (input_value >= 0) {
	  if (input_value < (PULSES.selection_masks() * sizeof(pulses_mask_t) / 2)) {	// two bytes 16bit hex masks
	    PULSES.hex_input_mask_index = input_value;
	    PULSES.maybe_show_selected_mask();
	  } else
	    MENU.outln_invalid();
	} else
	  MENU.outln_invalid();
      }
      break;

    case '~':	// '.~' invert destination selection
      MENU.drop_input_token();
      for (int pulse=0; pulse<pl_max; pulse++)
	PULSES.toggle_selection(pulse);
      PULSES.maybe_show_selected_mask();
      break;

    case 'x':	// '.x' clear destination selection	also on 'x'
      MENU.drop_input_token();
      PULSES.clear_selection();
      PULSES.maybe_show_selected_mask();
      break;

    case 'a':	// '.a' select_flagged
      MENU.drop_input_token();
      select_flagged();
      PULSES.maybe_show_selected_mask();			// TODO: factor that out {

      if (MENU.maybe_display_more())
	selected_or_flagged_pulses_info_lines();		// TODO: factor that out }
      break;

    case 'A':	// '.A' select destination: *all* pulses
      MENU.drop_input_token();
      select_all();
      PULSES.maybe_show_selected_mask();
      break;

    case 'v':	// '.v' select destination: select_n(voices)
      MENU.drop_input_token();
      PULSES.select_n(voices);
      break;

    case 'V':	// '.V' select destination: alive voices
      MENU.drop_input_token();
      PULSES.clear_selection();
      for (int pulse=0; pulse<voices; pulse++)
	if(PULSES.pulses[pulse].flags && (PULSES.pulses[pulse].flags != SCRATCH))
	  PULSES.select_pulse(pulse);

      PULSES.maybe_show_selected_mask();
      break;

    case 'L':	// '.L' select destination: all alive pulses
      MENU.drop_input_token();
      select_alive();
      PULSES.maybe_show_selected_mask();
      break;
    }
    break;

  case ':':	// info
    MENU.ln();
    PULSES.time_info();
    MENU.ln();
    PULSES.maybe_show_selected_mask();
    selected_pulses_info_lines();
    break;

  case ',':	// accept as noop in normal mode. used as delimiter to input data, displaying info. see 'menu_mode'
    if (MENU.menu_mode==JIFFLE_ENTRY_UNTIL_ZERO_MODE)
      display_jiffletab(jiffle);
    else
      if (MENU.verbosity >= VERBOSITY_SOME) {
	MENU.out_noop();
	MENU.ln();
      }
    break;

  case '|':	// accept as noop in normal mode. used as range bottom delimiter in arrays
    if (MENU.menu_mode==JIFFLE_ENTRY_UNTIL_ZERO_MODE) {
      jiffle_range_bottom = jiffle_write_index;
      fix_jiffle_range();

      if(MENU.cb_peek()==EOF)
	if (MENU.verbosity)
	  display_jiffletab(jiffle);
    } else
      if (MENU.verbosity >= VERBOSITY_SOME) {
	MENU.out_noop();
	MENU.ln();
      }
    break;

  case '"':	// accept as reserved noop in normal mode. used as range top delimiter in arrays
    if (MENU.menu_mode==JIFFLE_ENTRY_UNTIL_ZERO_MODE) {
      jiffle_range_top = jiffle_write_index;
      fix_jiffle_range();

      if(MENU.cb_peek()==EOF)
	if (MENU.verbosity)
	  display_jiffletab(jiffle);
    } else
      if (MENU.verbosity >= VERBOSITY_SOME)
	MENU.outln(F("reserved"));	// reserved for string input
    break;

  // in normal mode toggle pulse selection with chiffres
  // else input data. see 'menu_mode'
  case '0':	// toggle selection
  case '1':	// toggle selection
  case '2':	// toggle selection
  case '3':	// toggle selection
  case '4':	// toggle selection
  case '5':	// toggle selection
  case '6':	// toggle selection
  case '7':	// toggle selection
  case '8':	// toggle selection
  case '9':	// toggle selection
    switch (MENU.menu_mode) {
    case 0:	// normal input, no special menu_mode
      menu_input -= '0';
      menu_input += (PULSES.hex_input_mask_index * 16);
      if((menu_input < 0) || (menu_input >= pl_max))
	return false;		// *only* responsible if pulse exists

      // existing pulse:
      PULSES.toggle_selection(menu_input);

      PULSES.maybe_show_selected_mask();
      break;

    case JIFFLE_ENTRY_UNTIL_ZERO_MODE:	// first chiffre already seen
      MENU.restore_input_token();
      input_value = MENU.numeric_input(0);
      if (input_value)
	set_jiffle_RAM_value(input_value);

      else	// zero stops the input mode
	set_jiffle_RAM_value_0_stop();
      break;
    }
    break;

  case 'a':	// hex toggle selection
  case 'b':	// hex toggle selection
  case 'c':	// hex toggle selection
  case 'd':	// hex toggle selection
  case 'e':	// hex toggle selection
  case 'f':	// hex toggle selection
    switch (MENU.menu_mode) {
    case 0:	// normal input, no special menu_mode
      menu_input -= 'a';
      menu_input += 10;	// 'a' == 10
      menu_input += (PULSES.hex_input_mask_index * 16);
      if((menu_input < 0) || (menu_input >= pl_max))
	return false;		// *only* responsible if pulse exists

      // existing pulse:
      PULSES.toggle_selection(menu_input);

      PULSES.maybe_show_selected_mask();
      break;

    default:	// 'a' to 'f' keys not used in other menu modes
      if (MENU.verbosity >= VERBOSITY_SOME) {
	MENU.out_noop();
	MENU.ln();
      }
    }

    break;

  case '<':	// cursor left
    switch (MENU.menu_mode) {
    case JIFFLE_ENTRY_UNTIL_ZERO_MODE:
      if (jiffle_write_index)
	jiffle_write_index--;

      if(MENU.cb_peek()==EOF)
	if (MENU.verbosity)
	  display_jiffletab(jiffle);
      break;
    default:
      if (MENU.verbosity >= VERBOSITY_SOME) {
	MENU.out_noop();
	MENU.ln();
      }
    }
    break;

  case '>':	// cursor right
    switch (MENU.menu_mode) {
    case JIFFLE_ENTRY_UNTIL_ZERO_MODE:
      if (++jiffle_write_index >= (JIFFLE_RAM_SIZE - 2))
	jiffle_write_index = JIFFLE_RAM_SIZE - 2;

      if(MENU.cb_peek()==EOF)
	if (MENU.verbosity)
	  display_jiffletab(jiffle);
      break;
    default:
      if (MENU.verbosity >= VERBOSITY_SOME) {
	MENU.out_noop();
	MENU.ln();
      }
    }
    break;

  case 'u':	// calculating or select destination: PULSES.time_unit
    {
      unsigned long input_value=PULSES.time_unit;
      if (MENU.maybe_calculate_input(&input_value)) {
	MENU.out("==> "), MENU.outln(input_value);
	PULSES.time_unit=input_value;
      }
      else
	dest = CODE_TIME_UNIT;		// FIXME: obsolete?
    }
    break;

  case 'x':	// clear destination selection  also on '.x'
    PULSES.clear_selection();
    PULSES.maybe_show_selected_mask();
    break;

  case 's':	// switch pulse ACTIVE on/off
    for (int pulse=0; pulse<pl_max; pulse++) {
      if (PULSES.pulse_is_selected(pulse)) {
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
      selected_or_flagged_pulses_info_lines();			// *then* info ;)
    }
    // info_select_destination_with(false);	// DADA ################
    break;

  case 'S':	// enter sync
    if (MENU.maybe_calculate_input(&input_value)) {
      if (input_value>=0 )
	sync = input_value;
      else
	MENU.out(F("positive integer only"));
    }

    if (MENU.maybe_display_more()) {
      MENU.out(F("sync "));
      MENU.outln(sync);
    }

    break;

  case 'i':	// en_info
    for (int pulse=0; pulse<pl_max; pulse++)
      if (PULSES.pulse_is_selected(pulse))
	en_info(pulse);

    if (MENU.maybe_display_more()) {
      MENU.ln();
      selected_or_flagged_pulses_info_lines();
    }
    break;

  case 'M':	// "mute", no deactivate all clicks, see 'N'
    PULSES.deactivate_all_clicks();
    PULSES.check_maybe_do();	// maybe do it *first*

    if (MENU.verbosity)
      MENU.outln(F("deactivated all pulses"));
    break;

  case '*':	// multiply destination
    if(MENU.cb_peek() != '!') {		// '*' (*not* '*!<num>' set multiplier)
      switch (dest) {
      case CODE_PULSES:
	input_value = MENU.numeric_input(1);
	if (input_value>=0) {
	  for (int pulse=0; pulse<pl_max; pulse++)
	    if (PULSES.pulse_is_selected(pulse))
	      PULSES.multiply_period(pulse, input_value);

	  PULSES.fix_global_next();
	  PULSES.check_maybe_do();	// maybe do it *first*

	  if (MENU.maybe_display_more()) {
	    MENU.ln();
	    selected_or_flagged_pulses_info_lines();
	  }
	} else
	  MENU.outln_invalid();
	break;

      case CODE_TIME_UNIT:
	input_value = MENU.numeric_input(1);
	if (input_value>0)
	  PULSES.set_time_unit_and_inform(PULSES.time_unit * input_value);
	else
	  MENU.outln_invalid();
	break;
      }
    } else {		// '*!<num>' set multiplier
      MENU.drop_input_token();
      if(MENU.cb_peek()==EOF)
	MENU.outln(F("multiplier"));

      input_value = MENU.numeric_input(multiplier);
      if (input_value>0 )
	multiplier = input_value;
      else
	MENU.outln(F("small positive integer only"));

      if (MENU.maybe_display_more())
	show_scaling();
    }
    break;

 case '/':	// '/' divide destination  '/!<num>' set divisor
    if(MENU.cb_peek() == '!') {		// '/!<num>' set divisor
      MENU.drop_input_token();

      if(MENU.cb_peek()==EOF)
	MENU.outln(F("divisor"));

      input_value = MENU.numeric_input(divisor);
      if (input_value>0 )
	divisor = input_value;
      else
	MENU.outln(F("small positive integer only"));

      if (MENU.maybe_display_more())
	show_scaling();

    } else {	// '/' divide destination
      switch (dest) {
      case CODE_PULSES:
	input_value = MENU.numeric_input(1);
	if (input_value>=0) {
	  for (int pulse=0; pulse<pl_max; pulse++)
	    if (PULSES.pulse_is_selected(pulse))
	      PULSES.divide_period(pulse, input_value);

	  PULSES.fix_global_next();
	  PULSES.check_maybe_do();	// maybe do it *first*

	  if (MENU.maybe_display_more()) {
	    MENU.ln();
	    selected_or_flagged_pulses_info_lines();
	  }
	} else
	  MENU.outln_invalid();
	break;

      case CODE_TIME_UNIT:
	input_value = MENU.numeric_input(1);
	if (input_value>0)
	  PULSES.set_time_unit_and_inform(PULSES.time_unit / input_value);
	else
	  MENU.outln_invalid();
	break;
      }
    }
    break;

  case '=':	// set destination to value
    switch (dest) {
    case CODE_PULSES:
      input_value = MENU.numeric_input(1);
      if (input_value>=0) {
	for (int pulse=0; pulse<pl_max; pulse++)
	  if (PULSES.pulse_is_selected(pulse)) {
	    time_scratch.time = PULSES.time_unit;
	    time_scratch.overflow = 0;
	    PULSES.mul_time(&time_scratch, input_value);
	    PULSES.set_new_period(pulse, time_scratch);
	  }

	PULSES.fix_global_next();
	PULSES.check_maybe_do();	// maybe do it *first*

	if (MENU.maybe_display_more()) {
	  MENU.ln();
	  selected_or_flagged_pulses_info_lines();
	}
      } else
	MENU.outln_invalid();
      break;

    case CODE_TIME_UNIT:
      input_value = MENU.numeric_input(1);
      if (input_value>0)
	PULSES.set_time_unit_and_inform(input_value);
      else
	MENU.outln_invalid();
      break;
    }
    break;

  case 'K':	// kill selected pulses
    if (PULSES.how_many_selected()) {
      MENU.out(F("kill pulse "));
      for (int pulse=0; pulse<pl_max; pulse++)
	if (PULSES.pulse_is_selected(pulse)) {
	  PULSES.init_pulse(pulse);
	  if (MENU.maybe_display_more()) {
	    MENU.out(pulse);
	    MENU.space();
	  }
	}
      PULSES.fix_global_next();
      PULSES.check_maybe_do();	// maybe do it *first*

      MENU.ln();
    }

    if (MENU.maybe_display_more()) {
      selected_or_flagged_pulses_info_lines();
      MENU.ln();
    }
    break;

  case 'P':	// pulse create and edit
    PULSES.reset_and_edit_selected();
    if (MENU.maybe_display_more()) {
      MENU.ln();
      selected_or_flagged_pulses_info_lines();
    }
    break;

  case 'n':	// synchronize to now
    // we work on pulses anyway, regardless dest
    PULSES.activate_selected_synced_now(sync);	// sync and activate

    if (MENU.maybe_display_more()) {	// *then* info ;)
      MENU.ln();
      selected_or_flagged_pulses_info_lines();
    }
    break;

  case 'N':	// NULLs payload
    // we work on pulses anyway, regardless dest
    for (int pulse=0; pulse<voices; pulse++)
      if (PULSES.pulse_is_selected(pulse)) {
	PULSES.pulses[pulse].periodic_do = NULL;
	if (pulse<CLICK_PULSES)		// set clicks on LOW	// FIXME: CLICK_PULSES
	  digitalWrite(click_pin[pulse], LOW);
      }

    PULSES.fix_global_next();	// just in case?
    PULSES.check_maybe_do();	// maybe do it *first*

    if (MENU.maybe_display_more()) {	// *then* info ;)
      MENU.ln();
      selected_or_flagged_pulses_info_lines();
    }
    break;

  case 'g':	// 'g' "GPIO" [was: en_click]	'g~' toggle up/down pin mapping
    if(MENU.cb_peek() == '~') {	      // 'g~' toggle up/down pin mapping
          if (MENU.verbosity)
	    MENU.out(F("pin mapping bottom "));

	  g_inverse = !g_inverse;	// toggle bottom up/down click-pin mapping

	  if (MENU.maybe_display_more()) {
	    if (MENU.verbosity) {
	      if (g_inverse)
		MENU.outln(F("up"));
	      else
		MENU.outln(F("down"));
	    }
	  }
    } else
      en_click_selected();	// 'g' en_click  "GPIO"
    break;

#ifdef IMPLEMENT_TUNING		// implies floating point
  case 'w':	// sweep activate or toggle direction
    sweep_up *= -1;	// toggle direction up down

    if (sweep_up==0)	// start sweeping if it was disabled
      sweep_up=1;

    PULSES.check_maybe_do();	// maybe do it *first*

    if (MENU.maybe_display_more())
      sweep_info();

    break;

  case 'W':	// sweep info and control
    next_token = MENU.cb_peek();
    if (next_token == (char) EOF) {	// *no* input after 'W': maybe start, info
      if (sweep_up==0)
	sweep_up=1;		//    start sweeping up if disabled

      if (MENU.verbosity >= VERBOSITY_SOME)
	sweep_info();
      break;		// done
    }
    // there *is* input after 'W'

    // 'W<number>' does (calculating) positive integer input on PULSES.ticks_per_octave
    // exception: 'W0' switches sweeping off
    if (MENU.cb_peek()!='0' && MENU.maybe_calculate_input((unsigned long*) &PULSES.ticks_per_octave)) {	// hmmm !!!
      ticks_per_octave = PULSES.ticks_per_octave;	// FIXME: obsolete
      MENU.out(PULSES.ticks_per_octave);
      MENU.outln(F(" ticks/octave"));
    } else {	// no numeric input (except '0') follows 'W'
      next_token = MENU.cb_peek();
      switch(next_token) {	// examine following input token
      case '~': case 't':	// 'W~' 'Wt'  start sweep or toggle direction
	MENU.drop_input_token();
	if(sweep_up==0)			// start if not active
	  sweep_up = 1;
	else
	  sweep_up *= -1;		// or toggle sweep direction up down
	break;
      case '0':			// 'W0' switches sweeping off
	MENU.drop_input_token();
	sweep_up = 0;			// sweep off
	break;
      case '+': case '1':	// 'W+' 'W1"  sweep up
	MENU.drop_input_token();
	sweep_up = 1;			// sweep up
	break;
      case '-':			// 'W-' sweep down
	MENU.drop_input_token();
	sweep_up = -1;			// sweep down
	break;
      case '?':			// 'W?' info only
	MENU.drop_input_token();
	// if verbosity is too low sweep_info will not be called below,
	// so we do it here
	if (MENU.verbosity < VERBOSITY_SOME)
	  sweep_info();
	break;
      }
    }

    if (MENU.verbosity >= VERBOSITY_SOME)
      sweep_info();
    break;

  case 't':	// en_sweep_click
    // we work on voices anyway, regardless dest
    for (int pulse=0; pulse<voices; pulse++)
      if (PULSES.pulse_is_selected(pulse))
	en_sweep_click(pulse);

    PULSES.fix_global_next();	// just in case?
    PULSES.check_maybe_do();	// maybe do it *first*

    if (MENU.maybe_display_more()) {
      MENU.ln();
      selected_or_flagged_pulses_info_lines();
    }

    break;

  case 'o':	// en_sweep_click_0
    // we work on voices anyway, regardless dest
    for (int pulse=0; pulse<voices; pulse++)
      if (PULSES.pulse_is_selected(pulse))
	en_sweep_click_0(pulse);

    PULSES.fix_global_next();	// just in case?
    PULSES.check_maybe_do();	// maybe do it *first*

    if (MENU.maybe_display_more()) {
      MENU.ln();
      selected_or_flagged_pulses_info_lines();
    }

    break;

  case 'p':	// en_tuned_sweep_click
    // we work on voices anyway, regardless dest
    for (int pulse=0; pulse<voices; pulse++)
      if (PULSES.pulse_is_selected(pulse))
	en_tuned_sweep_click(pulse);

    PULSES.fix_global_next();	// just in case?
    PULSES.check_maybe_do();	// maybe do it *first*

    if (MENU.maybe_display_more()) {
      MENU.ln();
      selected_or_flagged_pulses_info_lines();
    }

    break;

  case 'T':	// 'T<integer-number>' sets tuning, 'T' toggles TUNED
    input_value = (unsigned long) PULSES.tuning;
    if (MENU.maybe_calculate_input(&input_value))	{	// T1 sets tuning to 1.0
      if (input_value > 0)
	PULSES.tuning = (double) input_value;
      tuning_info();
      MENU.ln();
    } else {	// toggle TUNED on selected pulses
      // we work on voices anyway, regardless dest
      for (int pulse=0; pulse<voices; pulse++)
	if (PULSES.pulse_is_selected(pulse))
	  if (PULSES.pulses[pulse].flags & TUNED)
	    PULSES.stop_tuning(pulse);
	  else
	    PULSES.activate_tuning(pulse);
    }

    PULSES.fix_global_next();	// just in case?
    PULSES.check_maybe_do();	// maybe do it *first*

    if (MENU.maybe_display_more()) {
      MENU.ln();
      selected_or_flagged_pulses_info_lines();
    }
    break;
#endif	// #ifdef IMPLEMENT_TUNING	implies floating point

  case 'j':	// en_jiffle_thrower
    en_jiffle_throw_selected(selected_actions);
    break;

  case 'J':	// select, edit, load jiffle
    /*
      'J'  shows registered jiffle names and display_jiffletab(<selected_jiffle>)
      'J7' selects jiffle #7 and display_jiffletab()
      'J!' copy selected jiffle in jiffle_RAM and display_jiffletab(jiffle_RAM)
    */
    // some jiffles from source, some very old FIXME:	review and delete	################
    if (MENU.cb_peek() == '!') {  // 'J!' copies an already selected jiffletab to RAM
      MENU.drop_input_token();
      if(jiffle != jiffle_RAM) {
	unsigned int * source=jiffle;
	// jiffle_write_index=0;	// no, write starting at jiffle_write_index #### FIXME: ####

	jiffle = jiffle_RAM;
	select_array_in_DB(JIFFLES, jiffle_RAM);
	load2_jiffle_RAM(source);
      }
    } else {	// select jiffle
      unsigned int* p = select_from_DB(JIFFLES);
      if (p != NULL)
	jiffle = p;
    }

    if (MENU.verbosity >= VERBOSITY_SOME)
	display_jiffletab(jiffle);
    break;

  case 'R':	// scale  was: ratio
    if (MENU.maybe_display_more())
      MENU.out(F("scale "));

    // 'R!' tune selected pulses to a scale starting from lowest
    if (MENU.cb_peek()=='!') {
      tune_2_scale(voices, multiplier, divisor, sync, selected_scale, scale);
    } else {
      selected_scale=MENU.numeric_input(-1);
      switch (selected_scale) {
      case ILLEGAL:
      case 0:
	scale = scale_RAM;
	break;
      default:
	if (selected_scale >= n_scale_names) {
	  selected_scale=0;
	  if (MENU.verbosity >= VERBOSITY_SOME)
	    MENU.outln_invalid();
	} else				// trailing '!' applies tuning
	  if (MENU.cb_peek()=='!')
	    tune_2_scale(voices, multiplier, divisor, sync, selected_scale, scale);
      }
#ifndef RAM_IS_SCARE	// enough RAM?	display jiffle names
      display_names(scale_names, n_scale_names, selected_scale);
#endif
    }
    display_arr(scale, 2);

    break;

  case 'F':	// en_INFO
    // we work on pulses anyway, regardless dest
    for (int pulse=0; pulse<pl_max; pulse++)
      if (PULSES.pulse_is_selected(pulse))
	en_INFO(pulse);

    if (MENU.maybe_display_more()) {
      MENU.ln();
      selected_or_flagged_pulses_info_lines();
    }
    break;

  case '{':	// enter_jiffletab
    MENU.menu_mode=JIFFLE_ENTRY_UNTIL_ZERO_MODE;
    jiffle = jiffle_RAM;
    jiffle_write_index=0;	// ################ FIXME: ################
    select_array_in_DB(JIFFLES, jiffle_RAM);
    if(MENU.cb_peek()==EOF)
      if (MENU.verbosity)
	display_jiffletab(jiffle);
    break;

  case '}':	// display jiffletab, stop editing jiffletab
    MENU.menu_mode=0;
    if (MENU.verbosity)
      display_jiffletab(jiffle);

    jiffle_write_index=0;	// ################ FIXME: ################
    break;

  case 'C':	// Calculator simply *left to right*	*positive integers only*
    MENU.maybe_calculate_input(&calc_result);

    if (MENU.maybe_display_more()) {
      MENU.out("==> "), MENU.outln(calc_result);

      if (calc_result > 1) {   	// show prime factors if(result >= 2)
	int p_factors_size=6;	// for harmonics I rarely use more than three, sometimes four ;)
	unsigned int p_factors[p_factors_size];
	MENU.out(F("prime factors of ")); MENU.out(calc_result);
	int highest = HARMONICAL.prime_factors(p_factors, p_factors_size, calc_result);
	for (int i=0; HARMONICAL.small_primes[i]<=highest; i++) {
	  MENU.tab();
	  MENU.out(F("("));
	  MENU.out(HARMONICAL.small_primes[i]);
	  MENU.out(F(")^"));
	  MENU.out(p_factors[i]);
	}
	MENU.ln();
      }
    }

    break;

  case 'D':	// DADA reserved for temporary code   testing debugging ...
//    MENU.out_noop(); MENU.ln();

    input_value=MENU.numeric_input(-1);
    if (input_value > -1)
      PULSES.select_pulse(input_value);
/*
    // ESP32 DAC test
    MENU.out(F("DAC test "));
    dacWrite(BOARD_DAC1, input_value=MENU.numeric_input(-1));
    dacWrite(BOARD_DAC2, input_value);
    MENU.outln(input_value);
*/

/*
    // display DACsq max intensity
    s1=0;
    s2=0;
    for(int p=0; p<CLICK_PULSES; p++) {	// FIXME: CLICK_PULSES
      s1 += PULSES.pulses[p].dac1_intensity;
      s2 += PULSES.pulses[p].dac2_intensity;
    }
    MENU.out(F("max DACsq intensity")); MENU.tab(); MENU.out(s1); MENU.tab(); MENU.outln(s2);
*/

    break;

  case 'y':	// DADA reserved for temporary code   testing debugging ...
    {
      // temporary least-common-multiple  test code, unfinished...	// ################ FIXME: ################
      unsigned long lcm=1L;
      for (int pulse=0; pulse<pl_max; pulse++) {
	if (PULSES.pulse_is_selected(pulse)) {
	  MENU.out(pulse); MENU.tab(); PULSES.pulses[pulse].period.time; MENU.tab();
	  lcm = HARMONICAL.LCM(lcm, PULSES.pulses[pulse].period.time);
	  MENU.outln(lcm);
	}
      }

      MENU.out(F("==> lcm ")); MENU.out(lcm);
      struct time length;
      length.time = lcm;
      length.overflow = 0;
      PULSES.display_realtime_sec(length);
      MENU.ln();

      for (int pulse=0; pulse<pl_max; pulse++)
	if ((PULSES.pulse_is_selected(pulse)) && PULSES.pulses[pulse].period.time) {
	  MENU.out(pulse);
	  MENU.tab();
	  MENU.outln(lcm/PULSES.pulses[pulse].period.time);
	}
    }
  break;

  case 'V':	// set voices	V[num]! PULSES.select_n_voices
    if(MENU.cb_peek()==EOF)
      MENU.out(F("voices "));

    input_value = MENU.numeric_input(voices);
    if (input_value>0 && input_value<=pl_max) {
      voices = input_value;
      if (voices>CLICK_PULSES) {	// FIXME: CLICK_PULSES
	if (MENU.verbosity)
	  MENU.outln(F("WARNING: voices > gpio"));
      }
    }
    else
      MENU.outln_invalid();

    if (voices==0)
      voices=CLICK_PULSES;	// just a guess

    if (MENU.maybe_display_more())
      MENU.outln(voices);

    if(MENU.cb_peek()=='!')
      PULSES.select_n(voices);

    break;

  case 'O':	// configure selected_actions
    if (MENU.cb_peek() == EOF) {
      MENU.out(F("action flags "));
      MENU.outBIN(selected_actions, 4);
      MENU.ln();
    } else {
      input_value = MENU.numeric_input(selected_actions);
      if (input_value != selected_actions)
	selected_actions = input_value;

      if (MENU.verbosity >= VERBOSITY_SOME) {
	MENU.out(F("action flags "));
	MENU.outBIN(selected_actions, 4);
	MENU.ln();
      }
    }
    break;

  case 'X':	// PANIC BUTTON  reset, remove all (flagged) pulses, restart selections at none
/* DEACTIVATED 'X!' reboot ESP8266
// not very useful, you cannot continue on the same serial line, deactivated
#ifdef ESP8266	// 'X!' reboot ESP8266		hope it works on all ESP8266 boards, FIXME: test
    // 'X!' reboots ESP8266
    next_token = MENU.cb_peek();
    switch(next_token) {	// examine following input token
    case '!':	// 'X!' reboots ESP8266
      MENU.drop_input_token();	// ;)
      MENU.outln(F("HARDWARE RESET"));
      pinMode(7, OUTPUT);	// setting pin 7 to output reboots my board
      break;
    }
#endif
*/
    // reset, remove all (flagged) pulses, restart selections at none
    reset_all_flagged_pulses_GPIO_OFF();
    break;

  case 'Z':	// reverse_click_pins
    reverse_click_pins();

    if (MENU.maybe_display_more())
      MENU.outln(F("reverse_click_pins"));

    break;

  case 'E':	// enter experiment
    if (MENU.maybe_display_more()) {
      MENU.out(F("experiment "));
    }

    input_value = MENU.numeric_input(-1);

    if (input_value==-1)
      display_names(experiment_names, n_experiment_names, selected_experiment);

    else if (input_value>=0 ) {
      selected_experiment = input_value;
      switch (selected_experiment) {	// initialize defaults, but do not start yet
      case 1:
	multiplier=2;
	divisor=1;
	sync=15;

	if (MENU.maybe_display_more()) {
	  display_name5pars("setup_jiffle128", g_inverse, voices, multiplier, divisor, sync);
	  Press_toStart();
	}
	break;

      case 2:
	sync=0;
	multiplier=1;
	divisor=1;

	if (MENU.maybe_display_more()) {
	  display_name5pars("init_div_123456", g_inverse, voices, multiplier, divisor, sync);
	  Press_toStart();
	}
	break;

      case 3:
	sync=1;
	multiplier=8;
	divisor=3;
	reverse_click_pins();	// ################ FIXME: CLICK_PULSES not here ################

	if (MENU.maybe_display_more()) {
	  // display_name5pars("setup_jiffles0", g_inverse, voices, multiplier, divisor, sync);
	  MENU.out(F("setup_jiffles0("));
	  MENU.out(g_inverse);
	  display_next_par(voices);
	  display_next_par(multiplier);
	  display_next_par(divisor);
	  display_next_par(sync);
	  MENU.outln(F(")  ESP8266 Frogs"));
	  Press_toStart();
	}
	break;

      case 4:
	multiplier=1;
	divisor=2;
	sync=0;
	jiffle=jiffletab;

	if (MENU.maybe_display_more()) {
	  display_name5pars("setup_jiffles2345", g_inverse, voices, multiplier, divisor, sync);
	  Press_toStart();
	}
	break;

      case 5:
	sync=0;		// FIXME: test and select ################
	multiplier=3;
	divisor=1;

	if (MENU.maybe_display_more()) {
	  display_name5pars("init_123456", g_inverse, voices, multiplier, divisor, sync);
	  Press_toStart();
	}
	break;

      case 6:
	sync=0;		// FIXME: test and select ################
	multiplier=1;
	divisor=1;

	if (MENU.maybe_display_more()) {
	  display_name5pars("init_chord_1345689a", g_inverse, voices, multiplier, divisor, sync);
	  Press_toStart();
	}
	break;

      case 7:
	sync=1;
	multiplier=1;
	divisor=6*7;

	if (MENU.maybe_display_more()) {
	  display_name5pars("init_rhythm_1", g_inverse, voices, multiplier, divisor, sync);
	  Press_toStart();
	}
	break;

      case 8:
	sync=5;
	multiplier=1;
	divisor=1;

	if (MENU.maybe_display_more()) {
	  display_name5pars("init_rhythm_2", g_inverse, voices, multiplier, divisor, sync);
	  Press_toStart();
	}
	break;

      case 9:
	sync=3;
	multiplier=1;
	divisor=1;

	if (MENU.maybe_display_more()) {
	  display_name5pars("init_rhythm_3", g_inverse, voices, multiplier, divisor, sync);
	  Press_toStart();
	}
	break;

      case 10:
	sync=1;
	multiplier=1;
	divisor=7L*3L;

	if (MENU.maybe_display_more()) {
	  display_name5pars("init_rhythm_4", g_inverse, voices, multiplier, divisor, sync);
	  Press_toStart();
	}
	break;

      case 11:
	sync=3;
	multiplier=3;
	divisor=1;

	if (MENU.maybe_display_more()) {
	  display_name5pars("setup_jifflesNEW", g_inverse, voices, multiplier, divisor, sync);
	  Press_toStart();
	}
	break;

      case 12:
	sync=1;	// or: sync=0;
	multiplier=1;
	divisor=1;

	if (MENU.maybe_display_more()) {
	  display_name5pars("init_pentatonic", g_inverse, voices, multiplier, divisor, sync);
	  Press_toStart();
	}
	break;

      case 13:
	sync=1;	// or: sync=0;
	multiplier=1;
	divisor=1;
	voices=8;	//just for 'The Harmonical Strings Christmas Evening Sounds'
	g_inverse=false;
	// unsigned int harmonics4 = {1,1,1024, 1,2,1024, 1,3,1024, 1,4,1024, 0,0};
	jiffle=harmonics4;
	PULSES.select_n(voices);
	display_name5pars("prepare_magnets", g_inverse, voices, multiplier, divisor, sync);
	prepare_magnets(g_inverse, voices, multiplier, divisor, sync);

	if (MENU.maybe_display_more()) {
	  selected_or_flagged_pulses_info_lines();
	  Press_toStart();
	}
	break;

      case 14:
	// magnets on strings, second take
	multiplier=1;
	divisor=1;
	g_inverse=false;

	scale = pentatonic_minor;
	PULSES.select_n(voices);
	prepare_scale(false, voices, multiplier * 1024 , divisor * 1167, sync, scale);
	jiffle=ting1024;
	PULSES.select_n(voices);
	display_name5pars("E14", g_inverse, voices, multiplier, divisor, sync);

	if (MENU.maybe_display_more())
	  selected_or_flagged_pulses_info_lines();
	break;

      case 15:
	// magnets on strings, third take
	multiplier=1;
	divisor=1;
	g_inverse=false;

	scale = pentatonic_minor;
	PULSES.select_n(voices);
	prepare_scale(false, voices, multiplier * 4096 , divisor * 1167, sync, scale);
	jiffle=ting4096;
	PULSES.select_n(voices);
	display_name5pars("E15", g_inverse, voices, multiplier, divisor, sync);

	if (MENU.verbosity >= VERBOSITY_SOME)
	  selected_or_flagged_pulses_info_lines();
	break;

      case 16:
	// piezzos on low strings 2016-12-28
	multiplier=4096;
	divisor=256;
	g_inverse=false;

	scale = european_pentatonic;
	PULSES.select_n(voices);
	prepare_scale(false, voices, multiplier, divisor, sync, scale);
	jiffle=ting4096;
	// jiffle = arpeggio4096;
	display_name5pars("E16 european_pent", g_inverse, voices, multiplier, divisor, sync);

	if (MENU.verbosity >= VERBOSITY_SOME)
	  selected_or_flagged_pulses_info_lines();
	break;

      case 17:
	// magnets on steel strings, "japanese"
	multiplier=1;	// click
	// multiplier=4096;	// jiffle ting4096
	divisor=256*5;

	scale = mimic_japan_pentatonic;
	PULSES.select_n(voices);
	prepare_scale(false, voices, multiplier, divisor, sync, scale);
	jiffle=ting4096;
	display_name5pars("E17 mimic japan", g_inverse, voices, multiplier, divisor, sync);

	if (MENU.verbosity >= VERBOSITY_SOME)
	  selected_or_flagged_pulses_info_lines();

	break;

      case 18:	// nylon stringed wooden box, piezzos
	scale = pentatonic_minor;
	multiplier=1;	// click
	// multiplier=4096;	// jiffle ting4096
	// divisor=2048;

	// string tuning on 8/9
	//   multiplier=8;
	//   divisor=9*1024;
	// multiplier=1;
	// divisor=9*128;

	// multiplier=8*4096;	// jiffle ting4096
	// divisor=9*1024;
	multiplier=32;	// reduced
	//#if defined(ESP32)	// used as test setup with 16 clicks
	// ################ FIXME: ESP32 16 click ################
	//      multiplier *= 4;
	//#endif
	divisor=9;	// reduced
	jiffle=ting4096;

	PULSES.select_n(voices);
	prepare_scale(false, voices, multiplier, divisor, sync, scale);
	display_name5pars("E18 pentatonic minor", g_inverse, voices, multiplier, divisor, sync);

	if (MENU.verbosity >= VERBOSITY_SOME)
	  selected_or_flagged_pulses_info_lines();

	break;

#ifdef IMPLEMENT_TUNING		// implies floating point
      case 19:	// TUNING: tuned sweep
	PULSES.clear_selection();
	PULSES.select_pulse(0);
	sweep_up=1;
	PULSES.reset_and_edit_pulse(0, PULSES.time_unit);
	PULSES.divide_period(0, 1024);
	en_tuned_sweep_click(0);

	PULSES.fix_global_next();	// just in case?
	PULSES.check_maybe_do();	// maybe do it *first*

	if (MENU.maybe_display_more()) {
	  MENU.ln();
	  selected_or_flagged_pulses_info_lines();
	}
	break;
#endif

      case 20:
	jiffle = arpeggio4096;
	MENU.play_KB_macro("X E12! .a N *8 J20-.");
	break;

      case 21:
	jiffle = arpeggio4096down;
	MENU.play_KB_macro("X E12! .a N *16 J21-.");
	break;

      case 22:
	jiffle = arpeggio_cont;				// :)	with pizzs
	MENU.play_KB_macro("X E12! .a N *16 J22-.");
	break;

      case 23:
	jiffle = arpeggio_and_down;			// :) :)  arpeggio down instead pizzs
	MENU.play_KB_macro("X E12! .a N *16 J23-.");
	break;

      case 24:
	jiffle = stepping_down;				// :) :)  stepping down
	MENU.play_KB_macro("X E12 S=0 ! .a N *16 J24-.");
	break;

      case 25:
	jiffle = back_to_ground;		// rhythm slowdown
	MENU.play_KB_macro("X E12! .a N *32 J25-.");		// :)	rhythm slowdown
	break;

      case 26:
	jiffle = arpeggio_and_sayling;
	MENU.play_KB_macro("X E12! .a N *32 J26-.");
	break;

      case 27:
	jiffle = simple_theme;
	MENU.play_KB_macro("X E12! .a N *2 -.");
	break;

      case 28:				// for tuning
	jiffle = peepeep4096;
	MENU.play_KB_macro("X E12! .a N *2 -.");

	break;

      case 29:				// KALIMBA7 tuning
	reset_all_flagged_pulses_GPIO_OFF();

#if defined KALIMBA7_v2	// ESP32 version  european_pentatonic
	scale = european_pentatonic;
	voices=7;
#else
	scale = pentatonic_minor;	// default, including KALIMBA7_v1
#endif
#if defined  KALIMBA7_v1
	voices=7;
#endif

	multiplier=1;
	divisor=1024;
	PULSES.select_n(voices);
	prepare_scale(false, voices, multiplier, divisor, sync, scale);
	display_name5pars("E29 KALIMBA7 tuning", g_inverse, voices, multiplier, divisor, sync);
	en_click_selected();							// for tuning ;)
	PULSES.activate_selected_synced_now(sync);	// sync and activate
	MENU.ln();

	if (MENU.verbosity >= VERBOSITY_SOME)
	  selected_or_flagged_pulses_info_lines();

	break;

      case 30:				// KALIMBA7 jiffle
	scale = pentatonic_minor;
	voices=7;
	// voices=8;
	multiplier=4;
	divisor=1;
	// jiffle = peepeep4096;
	jiffle = ting4096;
	// jiffle = tingeling4096;
	PULSES.select_n(voices);
	prepare_scale(false, voices, multiplier, divisor, sync, scale);
	display_name5pars("E30 KALIMBA7 jiff", g_inverse, voices, multiplier, divisor, sync);
	en_jiffle_throw_selected(selected_actions);
	PULSES.activate_selected_synced_now(sync);	// sync and activate

	MENU.ln();

	if (MENU.verbosity >= VERBOSITY_SOME)
	  selected_or_flagged_pulses_info_lines();

	break;

      case 31:				// KALIMBA7 jiffle
	scale = european_pentatonic;
	voices=8;
	multiplier=4;
	divisor=1;
	jiffle = ting4096;
	PULSES.select_n(voices);
	prepare_scale(false, voices, multiplier, divisor, sync, scale);
	display_name5pars("E31 KALIMBA7 jiff", g_inverse, voices, multiplier, divisor, sync);
	en_jiffle_throw_selected(selected_actions);
	PULSES.activate_selected_synced_now(sync);	// sync and activate;
	MENU.ln();

	if (MENU.verbosity >= VERBOSITY_SOME)
	  selected_or_flagged_pulses_info_lines();

	break;

      case 32:				// ESP32_12
	scale = major_scale;
	voices=12;
	multiplier=4;
	divisor=1;
	// jiffle = ting4096;
	jiffle = tigg_ding4096;
	PULSES.select_n(voices);
	prepare_scale(false, voices, multiplier, divisor, sync, scale);
	display_name5pars("E32 ESP32_12", g_inverse, voices, multiplier, divisor, sync);
	en_jiffle_throw_selected(selected_actions);
	PULSES.activate_selected_synced_now(sync);	// sync and activate;
	MENU.ln();

	if (MENU.verbosity >= VERBOSITY_SOME)
	  selected_or_flagged_pulses_info_lines();

	break;

      case 33:
	scale = minor_scale;
	voices=12;
	multiplier=4;
	divisor=1;
	jiffle = ting4096;
	PULSES.select_n(voices);
	prepare_scale(false, voices, multiplier, divisor, sync, scale);
	display_name5pars("minor", g_inverse, voices, multiplier, divisor, sync);
	en_jiffle_throw_selected(selected_actions);
	PULSES.activate_selected_synced_now(sync);	// sync and activate;
	MENU.ln();

	if (MENU.verbosity >= VERBOSITY_SOME)
	  selected_or_flagged_pulses_info_lines();

	break;

      case 34:
	scale = major_scale;
	voices=12;
	multiplier=4;
	divisor=1;
	jiffle = ting4096;
	PULSES.select_n(voices);
	prepare_scale(false, voices, multiplier, divisor, sync, scale);
	display_name5pars("major", g_inverse, voices, multiplier, divisor, sync);
	en_jiffle_throw_selected(selected_actions);
	PULSES.activate_selected_synced_now(sync);	// sync and activate;
	MENU.ln();

	if (MENU.verbosity >= VERBOSITY_SOME)
	  selected_or_flagged_pulses_info_lines();

	break;

      case 35:
	scale=tetrachord;
	voices=12;
	multiplier=4;
	divisor=1;
	jiffle = ting4096;
	PULSES.select_n(voices);
	prepare_scale(false, voices, multiplier, divisor, sync, scale);
	display_name5pars("tetra", g_inverse, voices, multiplier, divisor, sync);
	en_jiffle_throw_selected(selected_actions);
	PULSES.activate_selected_synced_now(sync);	// sync and activate;
	MENU.ln();

	if (MENU.verbosity >= VERBOSITY_SOME)
	  selected_or_flagged_pulses_info_lines();

	break;

      case 36:
	//scale=major_scale;
	scale = pentatonic_minor;
	voices=16;	//	################ FIXME: more voices, please ;) ################
	multiplier=6;
	divisor=1;
	jiffle = ting4096;
	PULSES.select_n(voices);
	prepare_scale(false, voices, multiplier, divisor, sync, scale);
	display_name5pars("BIG major", g_inverse, voices, multiplier, divisor, sync);
	en_jiffle_throw_selected(selected_actions);
	PULSES.activate_selected_synced_now(sync);	// sync and activate;
	MENU.ln();

	if (MENU.verbosity >= VERBOSITY_SOME)
	  selected_or_flagged_pulses_info_lines();

	break;

      case 37:	// Guitar
	reset_all_flagged_pulses_GPIO_OFF();

	PULSES.time_unit=1000000;

	// default tuning e
	multiplier=4096;
	// divisor=440;			// a4
	divisor=330; // 329.36		// e4  ***not*** harmonical
	// divisor=165; // 164.81		// e3  ***not*** harmonical

	selected_scale = 4;
	scale = minor_scale;		// default e minor

	jiffle = ting4096;		// default jiffle
	//	voices = 16;			// for DAC output
	if (voices == 0)
	  voices = 15;			// default (diatonic)	// for DAC output

	if(MENU.cb_peek()!=EOF) {		// second letters e E a A	e|a  minor|major
	  //	 ################ FIXME: ################
	  switch (MENU.cb_peek()) {
	  case 'e':	// e minor scale
	    MENU.drop_input_token();
	    selected_scale = 4;
	    scale=minor_scale;
	    break;

	  case 'E':	// E major scale
	    MENU.drop_input_token();
	    selected_scale = 5;
	    scale=major_scale;
	    break;

	  case 'a':	// a minor scale
	    MENU.drop_input_token();
	    selected_scale = 4;
	    divisor = 440;
	    scale=minor_scale;
	    break;

	  case 'A':	// A major scale
	    MENU.drop_input_token();
	    selected_scale = 5;
	    divisor = 440;
	    scale=major_scale;
	    break;

	  case 'd':	// d minor scale
	    MENU.drop_input_token();
	    selected_scale = 4;
	    divisor = 294;	// 293.66 = D4
	    // divisor = 147;	// 146.83 = D3
	    scale=minor_scale;
	    break;

	  case 'D':	// D major scale
	    MENU.drop_input_token();
	    selected_scale = 5;
	    divisor = 294;	// 293.66 = D4
	    // divisor = 147;	// 146.83 = D3
	    scale=major_scale;
	    break;
	  }
	}

	switch (MENU.cb_peek()) {	// (second or) third letters for other scales
	case EOF:
	  break;
	case '6':	// doric scale
	  scale = doric_scale;
//	  selected_scale=;
	  break;
	case '5':	// 5  pentatonic (minor|major) scale
	  MENU.drop_input_token();
	  if (scale==major_scale | scale==tetrachord) {
	    selected_scale = 2;
	    scale = european_pentatonic;
	    voices = 16;	// default (pentatonic)	// for DAC output
	  } else {
	    selected_scale = 1;
	    scale = pentatonic_minor;
	    voices = 16;	// default (pentatonic)	// for DAC output
	  }
	  break;
	case '4':	// 4  tetrachord
	  MENU.drop_input_token();
	  selected_scale = 6;
	  scale = tetrachord;
	  break;
	case '3':	// 3  octaves fourths fifths
	  MENU.drop_input_token();
	  selected_scale = 13;
	  scale = octaves_fourths_fifths;
	  multiplier *=8;
	  divisor /= 2;
	  break;
	case '2':	// 2  octaves fifths
	  MENU.drop_input_token();
	  selected_scale = 11;
	  scale = octaves_fifths;
	  multiplier *= 4;
	  divisor /= 4;
	  break;
	case '1':	// 1  octaves
	  MENU.drop_input_token();
	  selected_scale = 10;
	  scale = octaves;
	  multiplier *= 8;	// ################ FIXME: ################
	  divisor /= 8;
	  break;
	}

	// jiffle = ting4096;
	// jiffle = piip2048;
	// jiffle = tanboura; divisor *= 2;

	// ################ FIXME: remove redundant code ################
	PULSES.select_n(voices);
//	prepare_scale(false, voices, multiplier, divisor, 0, scale);
//	display_name5pars("GUITAR", g_inverse, voices, multiplier, divisor, sync);
	tune_2_scale(voices, multiplier, divisor, sync, selected_scale, scale);

  #ifndef USE_DACs	// TODO: review and use test code
	en_jiffle_throw_selected(selected_actions);
  #else // *do* use dac
	selected_share_DACsq_intensity(255, 1);

    #if (USE_DACs == 1)
	en_jiffle_throw_selected(DACsq1);
    #else
	selected_DACsq_intensity_proportional(255, 2);

	en_jiffle_throw_selected(DACsq1 | DACsq2);
    #endif
  #endif

	/*	################ TODO: try bass octaves on DACsq ################
	if(voices > CLICK_PULSES) {	// try bass octaves on DACsq		// FIXME: CLICK_PULSES
	  MENU.outln(F("DADA"));
	}
	*/

	PULSES.activate_selected_synced_now(sync);	// sync and activate;
	MENU.ln();

	if (MENU.verbosity >= VERBOSITY_SOME)
	  selected_or_flagged_pulses_info_lines();

	break;

      default:
	if (MENU.verbosity >= VERBOSITY_SOME)
	  MENU.outln_invalid();

	selected_experiment=0;
	break;
      }
    }
    break;

  // ################ FIXME: that's a mess!	################
  case '!':			// '!' setup and start experiment
    switch (selected_experiment) {
    case -1:
    case 0:
      break;
    case 1:
      setup_jiffle128(g_inverse, voices, multiplier, divisor, sync);
      break;
    case 2:
      init_div_123456(g_inverse, voices, multiplier, divisor, sync);
      break;
    case 3:
      setup_jiffles0(g_inverse, voices, multiplier, divisor, sync);    // ESP8266 Frog Orchester
      break;
    case 4:
      setup_jiffles2345(g_inverse, voices, multiplier, divisor, sync);
      break;
    case 5:
      init_123456(g_inverse, voices, multiplier, divisor, sync);
      break;
    case 6:
      init_chord_1345689a(g_inverse, voices, multiplier, divisor, sync);
      break;
    case 7:
      init_rhythm_1(g_inverse, voices, multiplier, divisor, sync);
      break;
    case 8:
      init_rhythm_2(g_inverse, voices, multiplier, divisor, sync);
      break;
    case 9:
      init_rhythm_3(g_inverse, voices, multiplier, divisor, sync);
      break;
    case 10:
      init_rhythm_4(g_inverse, voices, multiplier, divisor, sync);
      break;
    case 11:
      setup_jifflesNEW(g_inverse, voices, multiplier, divisor, sync);
      break;
    case 12:
      init_pentatonic(g_inverse, voices, multiplier, divisor, sync);
      break;
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
      // FIXME:	maybe make that default?
      PULSES.activate_selected_synced_now(sync); // sync and activate

      if (MENU.maybe_display_more()) {		// *then* maybe info ;)
	MENU.ln();
	selected_or_flagged_pulses_info_lines();
      }
      break;

    default:	// invalid
      selected_experiment=0;

      if (MENU.verbosity)
	MENU.outln_invalid();
      break;
    }

    PULSES.fix_global_next();	// just in case?
    PULSES.check_maybe_do();	// maybe do it *first*

    break;

  default:
    return false;	// menu entry not found
    break;
  }
  return true;		// menu entry found
}


/* **************************************************************** */



/* **************************************************************** */
/* **************************************************************** */

/* please se README_pulses					    */

/*
   Copyright © Robert Epprecht  www.RobertEpprecht.ch   GPLv2
   http://github.com/reppr/pulses
*/

/* **************************************************************** */
/* **************************************************************** */
/*				END OF CODE			    */
/* **************************************************************** */
/* **************************************************************** */
