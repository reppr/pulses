#define USE_BLUETOOTH_SERIAL_MENU	// SKETCH GETS TOO BIG ;)

// #define ESP32_G15_T01	boards_layout/G15-T1-esp32_dev.h	//
#define HARMONICAL_MUSIC_BOX
#define MAGICAL_TOILET_HACKS	// some quick dirty hacks

// TODO: change PERIPHERAL_POWER_SWITCH_PIN as GPIO12 is active during boot process...
#define PERIPHERAL_POWER_SWITCH_PIN	12	// == MORSE_TOUCH_INPUT_PIN

//#define USE_MORSE	// incomplete	DEACTIVATED MORSE OUTPUT, (== PERIPHERAL_POWER_SWITCH_PIN ;)
//#define USE_INPUTS
//#define USE_LEDC	// to be written ;)

#define USE_RTC_MODULE
//#define USE_i2c_SCANNER

#define USE_BATTERY_CONTROL
//#define USE_LEDC_AUDIO	// not written yet ;)

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

#if defined USE_BLUETOOTH_SERIAL_MENU
  #include "BluetoothSerial.h"
  BluetoothSerial BLUEtoothSerial;
#endif

#if defined USE_i2c
  #include <Wire.h>
  #if defined USE_MCP23017
    #include "MCP23017.h"			// Adafruit, simple test version only
  #endif

//  #if defined USE_DS1307_RTC
//    // #include <Wire.h>
//    #include <Time.h>
//    #include <DS1307RTC.h>
//  #endif

  #if defined USE_i2c_scanner
    #include "i2c_scan.h"
  #endif
#endif // USE_i2c


// some early definitions keep compiler happy

// stress managment
unsigned int stress_emergency=4096*6;	// magical musicBox test			  TODO: fine tune
unsigned int stress_event_level=512;	// just TESTING...				  TODO: fine tune
int stress_event_cnt=0;			// counting stress_event_level events
uint8_t stress_event_cnt_MAX=2;		// if the count reaches MAX stress release needed TODO: fine tune
unsigned int stress_count=0;		// low level stress count


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
Menu MENU(CB_SIZE, 6, &men_getchar, MENU_OUTSTREAM, MENU_OUTSTREAM2);


/* **************** Pulses **************** */
#include <Pulses.h>

Pulses PULSES(PL_MAX, &MENU);


/* **************** Harmonical **************** */
#include <Harmonical.h>

Harmonical HARMONICAL(3628800uL);	// old style for a first test

#include "iCODE.h"			// icode programs
#include "jiffles.h"			// old style jiffles (could also be used as icode)
#include "int_edit.h"			// displaying and editing unsigned int arrays
#include "array_descriptors.h"		// make data arrays accessible for the menu, give names to the data arrays

action_flags_t selected_actions = DACsq1 | DACsq2;	// TODO: better default actions

/* **************************************************************** */
// some #define's:
#define ILLEGAL		-1
#define ILLEGAL_PIN	255


/* **************************************************************** */
// define gpio_pin_t gpio_pins[GPIO_PINS]	// see: pulses_boards.h
#ifdef GPIO_PINS
  #include "pulses_CLICK_PIN_configuration.h"	// defines gpio_pins[]
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
fraction pitch={1,1};	// pitch to tune a scale


/* **************************************************************** */
bool DO_or_maybe_display(unsigned char verbosity_level) { // the flag tells *if* to display
  if (PULSES.check_maybe_do())
    return false;		// no time to display anything...

  return MENU.maybe_display_more(verbosity_level);	// flag depending verbosity and menu input
}


/* **************************************************************** */
// iCode

bool setup_icode_seeder(int pulse, struct time period, icode_t* icode_p, action_flags_t dest_action_flags) {
  if ((pulse > ILLEGAL) && (pulse < PL_MAX)) {
    PULSES.pulses[pulse].period = period;
    PULSES.set_icode_p(pulse, icode_p, false);		// icode inactive in seeder
    PULSES.set_payload(pulse, seed_icode_player);
    PULSES.pulses[pulse].dest_action_flags |= dest_action_flags;
    return true;
  }
  return false;
}


void seed_icode_player(int seeder_pulse) {	// as payload for seeder
  int dest_pulse = PULSES.setup_pulse(NULL, ACTIVE, PULSES.pulses[seeder_pulse].next, PULSES.pulses[seeder_pulse].period);
  if (dest_pulse != ILLEGAL) {
    /*
      pulses[pulse].icode_p	icode start pointer
      pulses[pulse].icode_index	icode index
      pulses[pulse].countdown	count down
      pulses[pulse].base_time	base period = period of starting pulse
      pulses[pulse].gpio	maybe click pin
    */
    PULSES.set_icode_p(dest_pulse, PULSES.pulses[seeder_pulse].icode_p, true);	// set (and activate) icode
    PULSES.pulses[dest_pulse].base_time   = PULSES.pulses[seeder_pulse].period.time;
    PULSES.pulses[dest_pulse].action_flags = PULSES.pulses[seeder_pulse].dest_action_flags;
    if(PULSES.pulses[seeder_pulse].flags & HAS_GPIO)		// if the seeder has gpio,
      PULSES.set_gpio(dest_pulse, PULSES.pulses[seeder_pulse].gpio);	// copy gpio
    if(PULSES.pulses[seeder_pulse].dac1_intensity)		// if the seeder has dac1_intensity
      PULSES.pulses[dest_pulse].dac1_intensity = PULSES.pulses[seeder_pulse].dac1_intensity; // set intensity
    if(PULSES.pulses[seeder_pulse].dac2_intensity)		// if the seeder has dac2_intensity
      PULSES.pulses[dest_pulse].dac2_intensity = PULSES.pulses[seeder_pulse].dac2_intensity; // set intensity

#if defined USE_i2c
  #if defined USE_MCP23017
      if(PULSES.pulses[seeder_pulse].flags & HAS_I2C_ADDR_PIN)	// if the seeder has i2c_addr and i2c_pin
	PULSES.set_i2c_addr_pin(dest_pulse, PULSES.pulses[seeder_pulse].i2c_addr, PULSES.pulses[seeder_pulse].i2c_pin);
  #endif
#endif

  }
}

/* **************************************************************** */
// scales

#ifndef SCALES_RAM_SIZE	// scales on small harware ressources, FIXME: test	################
  #define SCALES_RAM_SIZE 9*2+2	// small buffer might fit on simple hardware
#endif

#ifdef SCALES_RAM_SIZE
  // scales:
  unsigned int scale_RAM[SCALES_RAM_SIZE] = {0};
  unsigned int scale_RAM_length = SCALES_RAM_SIZE;
  unsigned int scale_write_index=0;
//  unsigned int *scale=scale_RAM;	// OBSOLETE? see: selected_in(SCALES)
#else
  #error SCALES_RAM_SIZE is not defined
#endif // SCALES_RAM_SIZE


// scaletabs *MUST* have 2 trailing zeros

unsigned int octaves[] = {1,1, 0,0};				// zero terminated
unsigned int octaves_fifths[] = {1,1, 2,3, 0,0};			// zero terminated
unsigned int octaves_fourths[] = {1,1, 3,4, 0,0};		// zero terminated
unsigned int octaves_fourths_fifths[] = {1,1, 3,4, 2,3, 0,0};	// zero terminated

unsigned int scale_int[]  = {1,1, 2,1, 3,1, 4,1, 5,1, 6,1, 7,1, 8,1, 0,0};  // zero terminated
unsigned int overnotes[]  = {1,1, 1,2, 1,3, 1,4, 1,5, 1,6, 1,7, 1,8, 1,9, 1,10, 1,11, 1,12, 1,13, 1,14, 1,15, 1,16, 0,0};
unsigned int scale_rationals[]  = {1,1, 1,2, 2,3, 3,4, 5,6, 6,7, 7,8, 8,9, 9,10, 0,0};  // zero terminated

unsigned int european_PENTAtonic[] = {1,1, 8,9, 4,5, 2,3, 3,5, 0,0};	// scale each octave	zero terminated

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

unsigned int tetraCHORD[] = {1,1, 8,9, 4,5, 3,4, 0,0};		// scale major tetraCHORD
unsigned int tetrachord[] = {1,1, 8,9, 5,6, 3,4, 0,0};		// scale minor tetrachord
unsigned int pentaCHORD[] = {1,1, 8,9, 4,5, 3,4, 2,3, 0,0};	// major
unsigned int pentachord[] = {1,1, 8,9, 5,6, 3,4, 2,3, 0,0};	// minor

unsigned int trichord[] = {1,1, 8,9, 4,5, 0,0};			// major

unsigned int TRIAD[] ={1,1, 4,5, 2,3, 0,0};
unsigned int triad[] ={1,1, 5,6, 2,3, 0,0};

// TODO: time (and related stuff should move to Harmonics::
struct time harmonical_CYCLE;		// TODO: move to Harmonical?

// TODO: move to Harmonical::scale2harmonical_cycle()	################
struct fraction harmonical_cycle_fraction={1, 1 };

//#define SCALE2CYCLE_INFO	// for debugging, but interesting to watch anyway ;)
struct time scale2harmonical_cycle(unsigned int* scale, struct time* duration) {		// returns harmonical cycle of a scale
  fraction f_LCM;
  f_LCM.multiplier = 1;
  f_LCM.divisor = 1;

  fraction f_F2;

  for(int i=0; scale[i]; i+=2) {
    f_F2.multiplier = selected_in(SCALES)[i];
    f_F2.divisor = selected_in(SCALES)[i+1];
#if defined SCALE2CYCLE_INFO
    display_fraction(&f_F2);
    MENU.tab();
#endif

    HARMONICAL.fraction_LCM(&f_F2, &f_LCM);

#if defined SCALE2CYCLE_INFO
    display_fraction(&f_LCM);
      MENU.ln();
#endif
  }

  harmonical_cycle_fraction = f_LCM;		// remember as a side effect

  PULSES.mul_time(duration, f_LCM.multiplier);
  PULSES.div_time(duration, f_LCM.divisor);

  return *duration;
}

// TODO: move to Harmonical::	################
void scale_time(struct time *duration, fraction * F) {
  PULSES.mul_time(duration, (*F).multiplier);
  PULSES.div_time(duration, (*F).divisor);
}

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


void fix_jiffle_range() {	// FIXME: use new implementation
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
void display_names(char** names, int count, int selected) {	// TODO: maybe obsolete?  (only for experiment names)
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


/*	TODO: move to library Pulses
  gpio_pin_t next_gpio()	return next unused GPIO click pin (or ILLEGAL)
  gpio_pin_t next_gpio(index)	*reset* to give gpio_pins[index] on next call
*/
gpio_pin_t next_gpio(gpio_pin_t set_i=-1) {
  static gpio_pin_t next_free_gpio_i=0;

  gpio_pin_t ret=ILLEGAL;

  if (set_i == -1) {	// normal use: next_gpio()  *get next unused gpio pin**
    if(next_free_gpio_i < GPIO_PINS)
      ret = gpio_pins[next_free_gpio_i];	// return next free pin
    else
      if (MENU.verbosity > VERBOSITY_LOWEST)
	MENU.outln(F("no free GPIO"));
    next_free_gpio_i++;				// count pins, available or not
  } else {		// next_gpio(set_i != ILLEGAL)   *reset* to return 'set_i' on next next_gpio() call
    if ((set_i < GPIO_PINS) && (set_i >= 0)) {
      next_free_gpio_i = set_i;
      ret = gpio_pins[next_free_gpio_i];	// rarely used, same as next call to next_gpio()
    } else {	// should not happen!
      MENU.out(F("ERROR: GPIO "));
      MENU.outln_invalid();
    }
  }
  return ret;
}


gpio_pin_t this_or_next_gpio(int pulse) {
  if (PULSES.pulses[pulse].flags & HAS_GPIO)	// if a pulse already has gpio, return it
    return PULSES.pulses[pulse].gpio;

  return next_gpio();				// else return  next_gpio()  or ILLEGAL
}


// reset, remove all (flagged) pulses, restart selections at none
// switch GPIO and DACs off
int reset_all_flagged_pulses_GPIO_OFF() {
  /*
    reset pulses, switches GPIO and DACs off
    MCP23017_OUT_LOW();
  */
  int cnt=0;
  for (int pulse=0; pulse<PL_MAX; pulse++) {  // tabula rasa
    if (PULSES.pulses[pulse].flags) {
      PULSES.init_pulse(pulse);
      cnt++;
    }
  }

  init_click_GPIOs_OutLow();		// switch them on LOW, output	current off, i.e. magnets
  PULSES.clear_selection();		// restart selections at none

#if defined USE_DACs			// reset DACs
  dacWrite(BOARD_DAC1, 0);
#if (USE_DACs > 1)
  dacWrite(BOARD_DAC2, 0);
#endif
#endif

#if defined USE_i2c
  #if defined USE_MCP23017
    MCP23017_OUT_LOW();
  #endif
#endif

  PULSES.fix_global_next();
  next_gpio(0);				// reset used gpio
  PULSES.hex_input_mask_index = 0;	// for convenience

  if (MENU.verbosity > VERBOSITY_LOWEST) {
    MENU.out(F("removed all pulses "));
    MENU.out(cnt);
    MENU.out(F("  switched pins off"));
    MENU.out(F("  freed GPIOs"));
#if defined USE_i2c
  #if defined USE_MCP23017
    MENU.out(F("  MCP23017_OUT_LOW();"));
  #endif
#endif
    MENU.ln();
  } else if(MENU.verbosity) {
    MENU.out(F("tabula rasa "));
    MENU.outln(cnt);
  }

  return cnt;
};


// make an existing pulse an old style click pulse:
bool en_click(int pulse, gpio_pin_t pin) {
  if ((pulse > ILLEGAL) && (pulse < PL_MAX)) {
    PULSES.set_payload_with_pin(pulse, &click, pin);
    return true;
  }

  return false;
};


// make selected pulses click
int en_click_selected() {
  int cnt=0;

  for (int pulse=0; pulse<PL_MAX; pulse++)
    if (PULSES.pulse_is_selected(pulse))
      if (en_click(pulse, this_or_next_gpio(pulse)))
	cnt++;

  if (DO_or_maybe_display(VERBOSITY_MORE)) {
    MENU.ln();
    selected_or_flagged_pulses_info_lines();
  }

  return cnt;
};

#if defined GPIO_PINS
void show_GPIOs() {
  MENU.out(GPIO_PINS);
  MENU.out(F(" GPIO pins\t{"));
  for(int i=0; i < GPIO_PINS; i++) {
    MENU.out(gpio_pins[i]);
    MENU.out_comma_();
  }
  MENU.outln('}');
}
#endif //  GPIO_PINS

#if defined USE_DACs	// TODO: move to library Pulses
// set_action_flags(pulse, DACsq1 | DACsq2) activates both DACs
bool set_action_flags(int pulse, unsigned int action_flags) {
  if ((pulse > ILLEGAL) && (pulse < PL_MAX)) {
    PULSES.pulses[pulse].action_flags |= action_flags;
    return true;
  }

  return false;
};

// make selected pulses produce square waves with harmonical timing

int selected_set_action_flags(unsigned int action_flags) {
  int cnt=0;

  for (int pulse=0; pulse<PL_MAX; pulse++)
    if (PULSES.pulse_is_selected(pulse))
      if (set_action_flags(pulse, action_flags))
	cnt++;

  return cnt;
};

// share DAC intensity between selected pulses
int selected_share_DACsq_intensity(int intensity, int channel) {
  int cnt=0;

  for (int pulse=0; pulse<PL_MAX; pulse++)
    if (PULSES.pulse_is_selected(pulse))
      cnt++;

  if (cnt) {
    intensity /= cnt;
    for (int pulse=0; pulse<PL_MAX; pulse++) {
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

  for (int pulse=0; pulse<PL_MAX; pulse++)
    if (PULSES.pulse_is_selected(pulse))
      PULSES.add_time(&PULSES.pulses[pulse].period, &sum);

  if (sum.overflow)	// FIXME: if ever needed ;)
    MENU.error_ln(F("sum.overflow"));

  if (sum.time) {
    for (int pulse=0; pulse<PL_MAX; pulse++) {
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
#endif // USE_DACs	// TODO: move to library Pulses



/* **************************************************************** */
// user interface variables:

int sync=1;			// syncing edges or middles of square pulses
unsigned long multiplier=1;
unsigned long divisor=1;

int selected_experiment=-1;
int voices=GPIO_PINS;

bool scale_user_selected=false;
bool jiffle_user_selected=false;
bool pitch_user_selected=false;	// TODO: not really implemented yet
bool octave_user_selected=false;
bool icode_user_selected=false;
bool sync_user_selected=false;


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
  #if ! defined SMALL_SKETCH_SIZE_TEST
    #define USE_INPUTS
  #endif
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

#if defined USE_MORSE
  #include "morse.h"
#endif

#if defined USE_LEDC
  #include "ledc_tone.h"
#endif

#if defined USE_RTC_MODULE
  #include "RTC_DS1307_module.h"
#endif

/* **************************************************************** */
#ifdef ARDUINO
/* Arduino setup() and loop():					*/

// needed for MENU.add_page();
// void softboard_display();
// bool softboard_reaction(char token);
int8_t softboard_page=-1;	// see: maybe_run_continuous()
int8_t musicBox_page=ILLEGAL;	// NOTE: musicBox_page is not used



#ifndef STARTUP_DELAY
  #define STARTUP_DELAY	0	// noop or yield()
#endif

#ifndef RAM_IS_SCARE	// enough RAM?
  #include "jiffles.h"
#endif

#include "random_entropy.h"	// TODO: only if used

#if defined PERIPHERAL_POWER_SWITCH_PIN
  #include "peripheral_power_switch.h"
#endif

#if defined USE_BATTERY_CONTROL
  #include "battery_control.h"
#endif

#if defined USE_LEDC_AUDIO
  #include "ledc_audio.h"
#endif

#if defined USE_BLUETOOTH_SERIAL_MENU
  #include "bluetoothSerialMenu.h"
  #include "bluetooth_menu_page.h"	// hi jacking USE_BLUETOOTH_SERIAL_MENU
#endif

#if defined HARMONICAL_MUSIC_BOX
  #include "musicBox.h"
#endif

#if defined ESP32
    // ESP32  int getChipRevision()	see: Andreas Spiess ESP32_Version.ino, register names updated
    #include "soc/efuse_reg.h"
    int get_ESP32_ChipRevision() {
      return (REG_READ(EFUSE_BLK0_RDATA3_REG) >> (EFUSE_RD_CHIP_VER_REV1_S)&&EFUSE_RD_CHIP_VER_REV1_V) ;
    }
#endif


int autostart_counter=0;	// can be used to change AUTOSTART i.e. for the very first one

void setup() {
#if defined RANDOM_ENTROPY_H	// *one* call would be enough, getting crazy on it ;)
  random_entropy();	// start gathering entropy before initialisation
#endif

#if defined PERIPHERAL_POWER_SWITCH_PIN	// output not possible yet, but switch peripheral power on already
  // for some strange reason i had to repeat this at the end of setup(), see below
  peripheral_power_switch_ON();		// default peripheral power supply ON
  //  peripheral_power_switch_OFF();	// default peripheral power supply OFF
  delay(100);	// wait a bit longer
#endif

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

#if defined RANDOM_ENTROPY_H	// *one* call would be enough, getting crazy on it ;)
  random_entropy();	// gathering entropy from serial noise
#endif
  delay(STARTUP_DELAY);

  MENU.outln(F("\n\nPULSES  http://github.com/reppr/pulses/\ninitialising\n"));

#if defined PERIPHERAL_POWER_SWITCH_PIN	// output now possible, so give info now
  MENU.out(F("peripheral POWER"));
  MENU.out_ON_off(digitalRead(PERIPHERAL_POWER_SWITCH_PIN));
  MENU.space();
  MENU.outln(PERIPHERAL_POWER_SWITCH_PIN);
#endif

#if defined ESP32
  MENU.out(F("ESP32 revision "));
  MENU.outln(get_ESP32_ChipRevision());

  MENU.out(F("ESP IDF version "));
  MENU.outln(esp_get_idf_version());
  MENU.ln();
#endif

#if defined USE_BLUETOOTH_SERIAL_MENU	// double, 1/2	test brownout recognition	TODO: review
  #if defined BLUETOOTH_ENABLE_PIN
  if(bluetooth_switch_())
  #endif
    {
      bluetooth_setup();
      MENU.ln();
    }
#endif

#include "array_descriptors_setup.h"

#ifdef USE_NVS
  #include "nvs_pulses_setup.h"
#endif

#ifdef GPIO_PINS
  show_GPIOs();
  MENU.ln();
#endif

#ifdef USE_MORSE
  #include "morse_setup.h"
#endif

#if defined USE_LEDC
  #include "ledc_tone_setup.h"
#endif

#if defined USE_BATTERY_CONTROL
  battery_control_setup();
#endif

#ifdef HARMONICAL_MUSIC_BOX
  musicBox_setup();
#endif

#ifdef GPIO_PINS
  init_click_GPIOs_OutLow();		// make them GPIO, OUTPUT, LOW
#endif

// #include "melody_jiffles.h"	// TODO: test only

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

#if defined USE_BLUETOOTH_SERIAL_MENU && false	// double, 2/2	test brownout recognition	TODO: review
  #if defined BLUETOOTH_ENABLE_PIN
  if(bluetooth_switch_())
  #endif
    bluetooth_setup();
#endif

#if defined USE_i2c
  Wire.begin();

  #if defined USE_MCP23017
    MCP23017.begin();
    Wire.setClock(100000L);	// must be *after* Wire.begin()
    MCP23017_OUT_LOW();
    PULSES.do_A2 = &MCP23017_write;
  #endif

  #if defined  USE_RTC_MODULE
    MENU.ln();
    show_DS1307_time_stamp();
    MENU.ln();
  #endif

#endif // USE_i2c

#ifdef USE_LEDC_AUDIO
  ledc_audio_setup();
#endif

#if defined RANDOM_ENTROPY_H	// *one* call would be enough, getting crazy on it ;)
  random_entropy();	// more entropy from hardware like wifi, etc
#endif

  MENU.out(F("\nsizeof(pulse_t) "));
  MENU.out(sizeof(pulse_t));
  MENU.out(F(" * "));
  MENU.out(PL_MAX);
  MENU.out(F(" pulses = \t"));
  MENU.outln(sizeof(pulse_t)*PL_MAX);


  // setting up the menu:

  // add pulses main page:
  MENU.add_page("Pulses", 'P', \
		&menu_pulses_display, &menu_pulses_reaction, 'P');

  // add softboard page:
  softboard_page = MENU.add_page("Hardware Softboard", 'H',		\
			 &softboard_display, &softboard_reaction, 'H');

  #ifdef USE_INPUTS
    // add inputs page:
    MENU.add_page("Inputs", 'I', &inputs_display, &inputs_reaction, 'I');

    int inp=0;
    INPUTS.setup_analog_read(inp, 0, 8);			// A0, oversample=0
    INPUTS.setup_raw(inp);					// no calculations
    INPUTS.selected_inputs |= ++inp;				// selected for editor

    INPUTS.setup_analog_read(inp, 0, 8);				// A0, oversample=0
    INPUTS.setup_raw(inp);					// no calculations
    INPUTS.setup_linear(inp, 0, 255, 1023, 0, PROPORTIONAL);	// 255*x/1023
    INPUTS.selected_inputs |= ++inp;				// selected for editor
  #endif


  #ifdef USE_WIFI_telnet_menu
    // add WiFi page:
  MENU.add_page("WiFi", 'W', &WiFi_menu_display, &WiFi_menu_reaction, 'W');
  #endif

  #ifdef HARMONICAL_MUSIC_BOX
    // NOTE: musicBox_page is not used
    musicBox_page = MENU.add_page("musicBox", 'M', &musicBox_display, &musicBox_reaction, 'P');
  #endif

  #if defined  USE_BLUETOOTH_SERIAL_MENU	// hi jacking USE_BLUETOOTH_SERIAL_MENU
    MENU.add_page("bluetooth", 'B', &bluetooth_menu_display, &bluetooth_menu_reaction, 'P');
  #endif

  // display menu at startup, but not in music box
  #if ! defined HARMONICAL_MUSIC_BOX
    MENU.menu_display();
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
    autostart_counter++;
    MENU.out(F("\nAUTOSTART "));
    MENU.outln(autostart_counter);

    AUTOSTART
  #endif

  // informations about alive pulses:
  if(MENU.verbosity >= VERBOSITY_SOME) { // can be switched of by autostart ;)
    MENU.ln();
    selected_or_flagged_pulses_info_lines();
  }

  // try to get rid of menu input garbage, "dopplet gnaeht hebt vilicht besser" ;)
  while (Serial.available())  { Serial.read(); yield(); }
  while (MENU.cb_peek() != EOF) { MENU.drop_input_token(); yield(); }

#if defined RANDOM_ENTROPY_H	// *one* call would be enough, getting crazy on it ;)
  random_entropy();	// entropy from thit and that
#endif

#ifdef HARMONICAL_MUSIC_BOX
  #if ! defined MAGICAL_TOILET_HACKS	// some quick dirty hacks, *no* interrupt
    magic_trigger_ON();
  #else
    pinMode(MUSICBOX_TRIGGER_PIN, INPUT);
  #endif
#endif


#if defined PERIPHERAL_POWER_SWITCH_PIN	// no idea why this is needed again, but it is
  peripheral_power_switch_ON();		// default peripheral power supply ON
  //  peripheral_power_switch_OFF();	// default peripheral power supply OFF
#endif
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

  if (maybe_run_continuous())		// even lower priority: maybe display input state changes.
    return true;

  return false;
}


bool lowest_priority_tasks() {

#if defined USE_MORSE
  if(MENU.verbosity >= VERBOSITY_MORE) {
    if(morse_stat_ID != morse_stat_seen_ID) {
      morse_show_saved_stats();	// sets morse_stat_seen_ID
      return true;
    }
  }

  // morse auto adapt
  if(MENU.verbosity >= VERBOSITY_SOME) {
    if(morse_stats_mean_dash_factor != 1.0) {
      morse_stats_mean_dash_factor += 1.0;
      morse_stats_mean_dash_factor /= 2.0;
      morse_TimeUnit *= morse_stats_mean_dash_factor;

      MENU.out(F("morse auto adapt "));
      MENU.out(morse_stats_mean_dash_factor);
      MENU.tab();
      MENU.outln(morse_TimeUnit);

      morse_stats_mean_dash_factor = 1.0;	// reset and switch display off
      return true;
    }
  }
#endif // defined USE_MORSE

#ifdef USE_WIFI_telnet_menu
// ################ FIXME: cleanup old WIFI code ################
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
//	      MENU.out("still dead	");
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
} // lowest_priority_tasks()


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
    ++stress_count;

#if defined HARMONICAL_MUSIC_BOX    // magical_stress_release();
    if (stress_count >= stress_emergency) {
      magical_stress_release();
      stress_count = 0;
      stress_event_cnt = -1;	// one heavy stress event expected after magical_stress_release()...
    }

    if(do_pause_musicBox) {
      //      MENU.outln(F(STRINGIFY(MUSICBOX_ENDING_FUNCTION)));
      do_pause_musicBox = false;
      stress_count = 0;
      stress_event_cnt = 0;
      delay(600);	// send remaining output

      (*musicBox_when_done)();		//      MUSICBOX_ENDING_FUNCTION

      if(musicBox_when_done != &user)	// user() is a flag *NOT to autostart* musicBox
	start_musicBox();
    }
#endif

    /*
  // stress release code written to kill sweep pulses getting to fast
  // in time machine this makes the situation worse, so deactivated for now

    if (stress_count >= stress_emergency) {
      // EMERGENCY
      // kill fastest pulse might do it? (i.e. fast sweeping up)

      if (MENU.maybe_display_more()) {	// TODO: rethink that
	MENU.out((int) PULSES.fastest_pulse());
	MENU.out(F(" deactivated  "));
      }

      PULSES.deactivate_pulse(PULSES.fastest_pulse());

      stress_count=0;				// seems best, maybe
      // stress_count = stress_emergency / 2;	// FIXME: further tests	################
    }
*/
  }

//#define STRESS_MONITOR_LEVEL	64	// TODO: menu interface
#if defined STRESS_MONITOR_LEVEL
  if (stress_count > STRESS_MONITOR_LEVEL) {	// just a simple test tool
    if (stress_count > stress_event_level) {
      MENU.out(F("STRESS "));
      MENU.out(stress_event_cnt);
      MENU.space();
    } else
      MENU.out(F("stress   "));

    int s=STRESS_MONITOR_LEVEL;
    while(stress_count > s) {
      MENU.out('!');
      s *= 2;
    }
    MENU.tab();
    MENU.out(stress_count);
    MENU.ln();
  }
#endif

  if (stress_count > stress_event_level) {
    if(++stress_event_cnt > stress_event_cnt_MAX) {
#if defined HARMONICAL_MUSIC_BOX			// magical_stress_release();
      magical_stress_release();
#else
      MENU.outln(F("need stress release"));	// TODO: other stress release strategies
#endif
    }
  } else if(stress_event_cnt < 0)	// stress event was expected, but did not happen
    stress_event_cnt = 0;		// reset expectations

#if defined HARMONICAL_MUSIC_BOX
 #if defined MAGICAL_TOILET_HACKS	// some quick dirty hacks
  if(digitalRead(MUSICBOX_TRIGGER_PIN)) {
    digitalWrite(2,HIGH);	// REMOVE: for field testing only
    musicBox_trigger_got_hot();	// must be called when musicBox trigger was detected high
  }
 #else
  if(switch_magical_trigger_off) {
    magic_trigger_OFF();
  }
 #endif
#endif

// descend through priorities and do first thing found
#ifdef USE_INPUTS
  if(! maybe_check_inputs())		// reading inputs can be time critical, so check early
#endif
    if(MENU.lurk_then_do()) {		// MENU second in priority, check if something to do,
      stress_event_cnt = -1;		//   after many menu actions there will be a stress event, ignore that
    } else					// no, menu did not do much
      if (! low_priority_tasks())		// check low_priority_tasks()
	lowest_priority_tasks();		// if still nothing done, check lowest_priority_tasks()

} // ARDUINO loop()

#else		// c++ Linux PC test version

int main() {

  printf("\nTesting pulses.ino\n");

  printf("\nNumber of pulses: %d\n", PL_MAX);

  printf("\nPULSES.init_pulses();\n");
  PULSES.init_pulses();
  printf("\n");


  printf("sizeof(pulse_t) %d * %d = \t%d\n\n",	\
	 sizeof(pulse_t), PL_MAX, sizeof(pulse_t)*PL_MAX );


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

//   or connect LEDs, MOSFETs, MIDI, whatever...
//   these are just FlipFlop pins.

// Click_pulses are a sub-group of pulses that control an arduino
// digital output each.
// The pins are configured as outputs by init_click_GPIOs_OutLow()
// and get used by clicks, jiffles and the like.

// It's best to always leave click_pulses in memory.
// You can set DO_NOT_DELETE to achieve this.


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


// TODO: move to library Pulses
void init_click_GPIOs_OutLow() {		// make them GPIO, OUTPUT, LOW
/* gpio_pin_t gpio_pins[GPIO_PINS];
   hardware pins for click_pulses
*/
  gpio_pin_t pin;

  for (int i=0; i<GPIO_PINS; i++) {
    pin=gpio_pins[i];

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

#elif defined ARDUINO	// *non* ESP, Arduino

  for (int i=0; i<GPIO_PINS; i++) {
    pin=gpio_pins[i];
    pinMode(pin, OUTPUT);	// on oldstyle Arduinos this is enough
    digitalWrite(pin, LOW);	// on oldstyle Arduinos this is enough
  }

#else
  #error TODO: fix init_click_GPIOs_OutLow()
#endif	// board
}


void out_noFreePulses() {
  MENU.out(F("no free pulses"));
}


#ifdef IMPLEMENT_TUNING		// implies floating point
// make an existing pulse to a sweep click pulse:
bool en_sweep_click(int pulse) {
  if ((pulse > ILLEGAL) && (pulse < PL_MAX)) {	// gpio set
    if (en_click(pulse, this_or_next_gpio(pulse))) {
      PULSES.set_payload(pulse, &sweep_click);	// gpio set

      return true;
    }
  }

  return false;
};


// make an existing pulse to a sweep_click_0 pulse:
bool en_sweep_click_0(int pulse) {
  if ((pulse > ILLEGAL) && (pulse < PL_MAX)) {
    if (en_click(pulse, this_or_next_gpio(pulse))) {	// gpio set
      PULSES.pulses[pulse].base_time = PULSES.pulses[pulse].period.time;
      PULSES.set_payload(pulse, &sweep_click_0);	// gpio set
      return true;
    }
  }

  return false;
};


bool en_tuned_sweep_click(int pulse) {
  if ((pulse > ILLEGAL) && (pulse < PL_MAX)) {
    if (en_click(pulse, this_or_next_gpio(pulse))) {	// gpio set
      PULSES.activate_tuning(pulse);
      PULSES.set_payload(pulse, &tuned_sweep_click);	// gpio set
      return true;
    }
  }

  return false;
}
#endif	// #ifdef IMPLEMENT_TUNING	implies floating point


int setup_click_synced(struct time when, unsigned long unit, unsigned long multiplier,
		       unsigned long divisor, int sync) {
  int pulse= PULSES.setup_pulse_synced(&click, ACTIVE, when, unit, multiplier, divisor, sync);

  if ((pulse > ILLEGAL) && (pulse < PL_MAX))
    PULSES.set_gpio(pulse, this_or_next_gpio(pulse));
  else // no free pulse
    out_noFreePulses();

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




// ****************************************************************
void select_flagged() {
  PULSES.clear_selection();
  for (int pulse=0; pulse<PL_MAX; pulse++)
    if (PULSES.pulses[pulse].flags)
      PULSES.select_pulse(pulse);
}


void select_all() {
  PULSES.clear_selection();
  for (int pulse=0; pulse<PL_MAX; pulse++)
    PULSES.select_pulse(pulse);
}


void select_alive() {
  PULSES.clear_selection();
  for (int pulse=0; pulse<PL_MAX; pulse++)
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

    for (; pulse<PL_MAX; pulse++) {
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
    if (pulse==PL_MAX)	// all available pulses have been tried, give up
      break;
  }

 global_next:
  PULSES.fix_global_next();
  return prepared;
}


// TODO: OBSOLETE?	################
int selected_apply_scale_on_period(int voices, unsigned int *scale, bool octaves=true) {
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

    for (; pulse<PL_MAX; pulse++) {
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
    if (pulse==PL_MAX)	// all available pulses have been tried, give up
      break;
  }

 global_next:
  PULSES.fix_global_next();
  return applied;
}


int tune_selected_2_scale_limited(fraction scaling, unsigned int *scale, unsigned long shortest_limit) {
/*
  tune all selected pulses to the scale, start with lowest selected
  scale 'PULSES.time_unit' by 'scaling' for base_period

  check if shortest period is longer than 'shortest_limit',
  else repeat 1 octave lower setting

  shortest limit *can* be zero to switch it off

  return octave_hift
*/

  int octave_shift=0;

  if (MENU.verbosity >= VERBOSITY_LOWEST) {
    MENU.out(F("tune_selected_2_scale_limited("));
    display_fraction(&scaling);
    MENU.out_comma_();
    MENU.out(array2name(SCALES, selected_in(SCALES)));
    MENU.out_comma_();
    MENU.out(shortest_limit);
    MENU.outln(')');
  }

  if ((scale != NULL) && scale[0] && scaling.divisor) {
    struct time base_period = {PULSES.time_unit, 0};
    base_period.time *= scaling.multiplier;
    base_period.time /= scaling.divisor;

    // check if highest note is within limit
    struct time this_period = {0, 0};	// bluff the very first test to pass
    while (this_period.time <= shortest_limit) { // SHORTEST LIMIT *CAN* BE ZERO (to switch it off)
      int octave=1;  // 1,2,4,8,...	the octave of this note   (*not* octave_shift)
      int note = 0;
      int multiplier=0;
      int divisor=0;

      for(int pulse=0; pulse<PL_MAX; pulse++) {
	if (PULSES.pulse_is_selected(pulse)) {
	  if ((multiplier = scale[note*2]) == 0) {	// next octave?
	    octave *= 2;	// one octave higher
	    note = 0;	// restart at first note
	    multiplier = scale[note*2];
	  }

	  divisor=scale[note*2+1];
//	if (divisor==0)
//	  goto global_next;		// divisor==0: error, end
	  divisor *= octave;

	  this_period = base_period;
	  PULSES.mul_time(&this_period, multiplier);
	  PULSES.div_time(&this_period, divisor);
	  PULSES.pulses[pulse].period = this_period;
	  note++;
	} // selected
      } // pulse

      if(this_period.time > shortest_limit) {
	if (octave_shift || MENU.verbosity > VERBOSITY_SOME) {
	  MENU.out(octave_shift);
	  MENU.outln(F(" octaves shifted"));
	}
	return octave_shift;		// OK, tuning fine and within limit, RETURN
      }
      // else repeat one octave lower...
      PULSES.mul_time(&base_period, 2);
      octave_shift--;
    } // while off limit, tune octaves down...
  } else MENU.error_ln(F("invalid tuning"));

  return octave_shift;
}


// TODO: OBSOLETE?	################
bool tune_2_scale(int voices, unsigned long multiplier, unsigned long divisor, unsigned int *scale)
{
  int pulse;
  struct time base_period;
  base_period.overflow = 0;
  base_period.time = multiplier * PULSES.time_unit;
  base_period.time /= divisor;

  if (scale != NULL) {
    if (MENU.verbosity >= VERBOSITY_LOWEST) {	// debug output
      MENU.out(F("tune_2_scale("));
      MENU.out(voices);
      MENU.out_comma_();
      MENU.out(multiplier);
      MENU.out_comma_();
      MENU.out(divisor);
      MENU.out_comma_();
      MENU.out(array2name(SCALES, scale));
      MENU.outln(')');
    }

    if (voices>0) {
      PULSES.select_n(voices);

      for (pulse=0; pulse<voices; pulse++) {
	PULSES.pulses[pulse].period = base_period;		// set all voices to base note
	//	PULSES.pulses[pulse].period.overflow = 0;
      }

      // now apply scale:
      selected_apply_scale_on_period(voices, scale, true);
      return true;

    } else
      MENU.error_ln(F("no voices"));
  } else
      MENU.error_ln(F("no scale"));

  return false;
};


// if shortest period of selected pulses is too short, shift all periods by octaves
// TODO: OBSOLETE?	################
int lower_audio_if_too_high(unsigned long limit) {
  unsigned int shortest=~0;		// shortest period.time (no overflow implemented here)
  unsigned int fastest_pulse=~0;	// pulse index with shortest period.time
  int octave_shift=0;
  int pulse;

  for (pulse=0; pulse<PL_MAX; pulse++) {	// find fastest selected pulse
    if (PULSES.pulse_is_selected(pulse)) {
      if(shortest > PULSES.pulses[pulse].period.time) {
	fastest_pulse =  pulse;
	shortest =  PULSES.pulses[pulse].period.time;
      }
    }
  }

  if(MENU.verbosity >= VERBOSITY_SOME || shortest==0) {	// DEBUGGING and normal feedback
    MENU.out(F("lower_audio_if_too_high shortest "));
    MENU.out(fastest_pulse);
    MENU.tab();
    MENU.outln(shortest);
    if(shortest==0) {
      // MENU.play_KB_macro(".");	// REMOVE: DEBUGGING:	################
      MENU.outln(F(" DEBUG: shortest==0 "));
      MENU.ln();
    }
  }

  while (PULSES.pulses[fastest_pulse].period.time < limit) {	// too fast?
    octave_shift--;
    for (pulse=0; pulse<PL_MAX; pulse++) {
      if (PULSES.pulse_is_selected(pulse)) {
	PULSES.mul_time(&PULSES.pulses[pulse].period, 2);	// octave shift down
      }
    }
  }

  if (octave_shift) {
    if (MENU.verbosity > VERBOSITY_LOWEST) {
      MENU.out(octave_shift);
      MENU.outln(F(" octaves shifted"));
    }
  }

  return octave_shift;
}


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
bool en_info(int pulse) {
  if ((pulse > ILLEGAL) && (pulse < PL_MAX)) {
    PULSES.set_payload(pulse, &pulse_info_1line);
    return true;
  }

  return false;
}

// TODO: move to Pulses.cpp
void pulse_info_1line(int pulse) {	// one line pulse info, short version
  unsigned long realtime=micros();		// let's take time *before* serial output

  if (PULSES.pulse_is_selected(pulse))
    MENU.out('*');
  else
    MENU.space();
  MENU.space();
  PULSES.show_group_mnemonics(pulse);

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

  if(PULSES.pulses[pulse].flags & COUNTED)	// if COUNTED
    MENU.out(PULSES.pulses[pulse].remaining);	//   show remaining
  else
    MENU.space();

  MENU.out(F(" Pf:"));
  PULSES.show_pulse_flag_mnemonics(PULSES.pulses[pulse].flags);
  if (PULSES.pulses[pulse].action_flags) {
    MENU.out(F(" Af: "));
    PULSES.show_action_flags(PULSES.pulses[pulse].action_flags);
  } else
    MENU.tab();

  if (PULSES.pulses[pulse].flags & HAS_GPIO) {
    MENU.out(F(" p"));
    if(PULSES.pulses[pulse].gpio != ILLEGAL) {
      MENU.out((int) PULSES.pulses[pulse].gpio);
      if (PULSES.pulses[pulse].gpio<10)	// padding?
	MENU.space();
    } else
      MENU.out(F(" _"));
  } else
    MENU.space(4);
  MENU.space();

  if (PULSES.pulses[pulse].action_flags & doesICODE) {
    MENU.out(':');
    char * name=array2name(iCODEs, (unsigned int *) PULSES.pulses[pulse].icode_p);
    if (name != "")
      MENU.out(name);
    else	// maybe it is a jiffle, played as iCode?
      MENU.out("J:");
    MENU.out(array2name(JIFFLES, (unsigned int *) PULSES.pulses[pulse].icode_p));
    MENU.space();
  }

  display_payload(pulse);

  MENU.space(); MENU.tab();
  if(PULSES.pulses[pulse].dest_action_flags || \
     PULSES.pulses[pulse].dac1_intensity || PULSES.pulses[pulse].dac2_intensity) {
    MENU.out(F("dAf: "));
    PULSES.show_action_flags(PULSES.pulses[pulse].dest_action_flags);

    MENU.out(F(" i1:"));
    MENU.pad(PULSES.pulses[pulse].dac1_intensity, 4);

    MENU.out(F("i2:"));
    MENU.pad(PULSES.pulses[pulse].dac2_intensity, 4);
  } else
    MENU.tab(2);

  if (MENU.verbosity >= VERBOSITY_SOME) {
    MENU.tab();
    PULSES.print_period_in_time_units(pulse);
  }

  if (MENU.verbosity > VERBOSITY_SOME) {
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


// pulse_info() as paylod for pulses: print pulse info:	// ################ TODO: update ################
void pulse_info(int pulse) {
  MENU.out(F("*** PULSE info "));
  MENU.out(pulse);
  MENU.slash();
  MENU.out((unsigned int) PULSES.pulses[pulse].counter);

  MENU.tab();
  display_payload(pulse);

  MENU.out_flags_();
  MENU.outBIN(PULSES.pulses[pulse].flags, sizeof(pulse_flags_t) * 8);
  MENU.ln();

  MENU.out(F("pin ")); MENU.out((int) PULSES.pulses[pulse].gpio);
  MENU.out(F("\tindex ")); MENU.out(PULSES.pulses[pulse].index);
  MENU.out(F("\ttimes ")); MENU.out(PULSES.pulses[pulse].remaining);
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
bool en_INFO(int pulse) {	// FIXME: to lib Pulses
  if ((pulse > ILLEGAL) && (pulse < PL_MAX)) {
    PULSES.set_payload(pulse, &pulse_info);
    return true;
  }

  return false;
}

// TODO: move to Pulses.cpp ################
void display_payload(int pulse) {
  void (*scratch)(int);

  MENU.out(F(" P:"));

  scratch=&click;
  if (PULSES.pulses[pulse].payload == scratch) {
    MENU.out("click");
    return;
  }

#if defined HARMONICAL_MUSIC_BOX
  scratch=&musicBox_butler;
  if (PULSES.pulses[pulse].payload == scratch) {
    MENU.out("musicBox_butler");
    return;
  }

  scratch=&magical_butler;
  if (PULSES.pulses[pulse].payload == scratch) {
    MENU.out("magical_butler");
    return;
  }

  scratch=&magical_cleanup;
  if (PULSES.pulses[pulse].payload == scratch) {
    MENU.out("magical_cleanup");
    return;
  }

  scratch=&cycle_monitor;
  if (PULSES.pulses[pulse].payload == scratch) {
    MENU.out("cycle_monitor");
    return;
  }

#endif

#ifdef IMPLEMENT_TUNING		// implies floating point
  scratch=&tuned_sweep_click;
  if (PULSES.pulses[pulse].payload == scratch) {
    MENU.out(F("tuned_sweep_click"));
    return;
  }

  scratch=&sweep_click;
  if (PULSES.pulses[pulse].payload == scratch) {
    MENU.out(F("sweep_click"));
    return;
  }

  scratch=&sweep_click_0;
  if (PULSES.pulses[pulse].payload == scratch) {
    MENU.out(F("sweep_click_0"));
    return;
  }
#endif	// #ifdef IMPLEMENT_TUNING	implies floating point

  scratch=&seed_icode_player;
  if (PULSES.pulses[pulse].payload == scratch) {
    MENU.out("seed_iCode ");
//    if (MENU.verbosity > VERBOSITY_LOWEST) {	// normally we *want* to see the names
//      char * name=array2name(iCODEs, (unsigned int *) PULSES.pulses[pulse].icode_p);
//      if (name != "")
//	MENU.out(name);
//      else	// maybe it is a jiffle, played as iCode?
//	MENU.out("J:");
//	MENU.out(array2name(JIFFLES, (unsigned int *) PULSES.pulses[pulse].icode_p));
//    }
    if (MENU.verbosity > VERBOSITY_LOWEST) {	// normally we *want* to see the names
      char * name=array2name(iCODEs, (unsigned int *) PULSES.pulses[pulse].icode_p);
      if (name != "")
	MENU.out(name);		// display iCode name
      else {			// maybe it is a jiffle, played as iCode?
	name=array2name(JIFFLES, (unsigned int *) PULSES.pulses[pulse].icode_p);
	if (name != "")
	  MENU.out(name);	// name of jiffle, played as iCode
	else
	  MENU.outln(F("(none)"));
      }
    }
    return;
  }

  scratch=&do_jiffle;
  if (PULSES.pulses[pulse].payload == scratch) {
    MENU.out(F("do_jiffle:"));
    MENU.out(array2name(JIFFLES, (unsigned int*) PULSES.pulses[pulse].data));
    return;
  }

  scratch=&do_throw_a_jiffle;
  if (PULSES.pulses[pulse].payload == scratch) {
    MENU.out(F("seed jiff:"));
    MENU.out(array2name(JIFFLES, (unsigned int*) PULSES.pulses[pulse].data));
    return;
  }

  scratch=&pulse_info;
  if (PULSES.pulses[pulse].payload == scratch) {
    MENU.out(F("pulse_info"));
    return;
  }

  scratch=&pulse_info_1line;
  if (PULSES.pulses[pulse].payload == scratch) {
    MENU.out(F("info line"));
    MENU.tab(2);
    return;
  }

  scratch=NULL;
  if (PULSES.pulses[pulse].payload == scratch) {
    MENU.tab();
    //    MENU.out(F("NULL"));
    return;
  }

  MENU.out(F("UNKNOWN\t"));
}


void flagged_pulses_info() {
  int count=0;

  for (int pulse=0; pulse<PL_MAX; ++pulse)
    if (PULSES.pulses[pulse].flags) {		// any flags set?
      pulse_info_1line(pulse);
      count++;
    }

  if (count == 0)
    MENU.outln(F("no flagged pulses"));
}


void selected_or_flagged_pulses_info_lines() {
  int count=0;
  for (int pulse=0; pulse<PL_MAX; ++pulse)
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
bool en_jiffle_thrower(int pulse, unsigned int *jiffle, gpio_pin_t pin, action_flags_t action_mask)
{
  if ((pulse > ILLEGAL) && (pulse < PL_MAX)) {
    PULSES.pulses[pulse].dest_action_flags |= action_mask;
    PULSES.set_payload_with_pin(pulse, &do_throw_a_jiffle, pin);
    PULSES.pulses[pulse].data = (unsigned int) jiffle;
    return true;
  }

  return false;
}


// make selected pulses jiffle throwers with gpio pin
int setup_jiffle_thrower_selected(action_flags_t action_flags) {	// FIXME: obsolete? ################
  int cnt=0;

  for (int pulse=0; pulse<PL_MAX; pulse++)
    if (PULSES.pulse_is_selected(pulse))
      if(en_jiffle_thrower(pulse, selected_in(JIFFLES), this_or_next_gpio(pulse), action_flags))
	cnt++;

  PULSES.fix_global_next();		// just in case?

  if (DO_or_maybe_display(VERBOSITY_MORE)) {
    MENU.ln();
    selected_or_flagged_pulses_info_lines();
  }

  return cnt;
};



int init_jiffle(unsigned int *jiffle, struct time when, struct time new_period, int origin_pulse)
{
  int pulse;
  struct time jiffle_period=new_period;

  jiffle_period.time = new_period.time * jiffle[0] / jiffle[1];

  pulse = PULSES.setup_pulse(&do_jiffle, ACTIVE, when, jiffle_period);
  if ((pulse > ILLEGAL) && (pulse < PL_MAX)) {
    PULSES.pulses[pulse].action_flags |= PULSES.pulses[origin_pulse].dest_action_flags; // set actions
    PULSES.set_gpio(pulse, PULSES.pulses[origin_pulse].gpio);	// copy pin from origin pulse
    PULSES.pulses[pulse].index = 0;				// init phase 0
    PULSES.pulses[pulse].countdown = jiffle[2];			// count of first phase
    PULSES.pulses[pulse].data = (unsigned int) jiffle;
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
  // pulses[pulse].data	= (unsigned int) jiffle;

  // start a new jiffling pulse now (next [pulse] is not yet updated):
  unsigned int *this_jiff=(unsigned int *) PULSES.pulses[pulse].data;

  // check for empty jiffle first:
  bool has_data=true;
  for (int i=0; i<3; i++)
    if (this_jiff[i]==0)
      has_data=false;

  if (has_data)	{ // there *is* jiffle data
    int p = init_jiffle((unsigned int *) PULSES.pulses[pulse].data,	\
	      PULSES.pulses[pulse].next, PULSES.pulses[pulse].period, pulse);
    PULSES.pulses[p].groups |= g_SECONDARY;

  } else	// zero in first triplet, *invalid* jiffle table.
    MENU.outln(F("no jiffle"));
}


// old style, obsolete	// TODO: remove?
int prepare_magnets(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync) {
  if (inverse) {
    no_g_inverse();
    return 0;
  }

  select_array_in(SCALES, pentatonic_minor);
  PULSES.select_n(voices);

#define COMPATIBILITY_PERIOD_3110	// sets the period directly
#ifdef COMPATIBILITY_PERIOD_3110	// TODO: remove
  for (int pulse=0; pulse<voices; pulse++)
    if (PULSES.pulse_is_selected(pulse)) {
      PULSES.reset_and_edit_pulse(pulse, PULSES.time_unit);
      PULSES.pulses[pulse].period.time = 3110;	// brute force for compatibility ;)
      PULSES.pulses[pulse].period.overflow = 0;	// brute force for compatibility ;)
      en_jiffle_thrower(pulse, selected_in(JIFFLES), this_or_next_gpio(pulse), 0);
    }
  //  int selected_apply_scale_on_period(int voices, unsigned int *scale, bool octaves=true);
  selected_apply_scale_on_period(voices, selected_in(SCALES), true);
#else	// compatibility problems
  prepare_scale(false, voices, multiplier, divisor, sync, selected_in(SCALES), true);
#endif
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
  MENU.out(F("select Pulse with "));

  MENU.out_ticked_hexs(min(PL_MAX,16));
  MENU.space();
  MENU.outln(F("select Mask with '.m<num>'"));

  MENU.outln(F("'.a'=select *all* click pulses\t'.A'=*all* pulses\t'.v'=voices\t'.L'=all alive\t'x'=none\t'.~'=invert selection"));

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
      "tetraCHORD",				// 35
      "more voices please",			// 36
      "Guitar and other Instruments",		// 37
      "Time Machine 1",				// 38
      "Time Machine 2",				// 39
      "Time Machine iCodeplayer",		// 40
//    "(invalid)",				// over
  };

  #define n_experiment_names (sizeof (experiment_names) / sizeof (const char *))
#endif // ! RAM_IS_SCARE


bool g_inverse=false;	// bottom DOWN/up GPIO click-pin mapping	// TODO: update!
/*
  'g_inverse' works when setting up an experiment creating pulses
	      other pulses aren't affected
	      some experiments do not implement that

  'reverse_gpio_pins()' as alternative:
  'reverse_gpio_pins()' works on the global gpio_pins[] array
			 the pulses won't notice but play with new pin mapping */


/* **************************************************************** */
// special menu modes, like numeric input for jiffles
#define JIFFLE_ENTRY_UNTIL_ZERO_MODE	1	// menu_mode for unsigned integer data entry, stop at zero

/* **************************************************************** */

void menu_pulses_display() {
  MENU.outln(F("http://github.com/reppr/pulses/"));

  MENU.out(F("\npulses "));
  MENU.out(PULSES.get_pl_max());
  MENU.space(2);
  MENU.out(F("GPIO "));
  MENU.outln(GPIO_PINS);
  if (MENU.verbosity > VERBOSITY_SOME)		// maybe display gpio_pins[]
    show_GPIOs();

  MENU.ln();
  MENU.outln(F("?=help\t.=flagged info\t:=UI selections"));

  MENU.ln();
  info_select_destination_with(false);

  MENU.out(F("u=select "));
  PULSES.show_time_unit();

  MENU.ln();
  MENU.out(F("s=switch pulse on/off"));
  MENU.tab();
  MENU.out(F("M=mute ALL actions\tX=remove ALL\tK=kill\n\nCREATE PULSES\tstart with 'P'\nP=new pulse\tg=en-click\tj=en-jiffle\tN=en-noop\ti=en-info\tF=en-INFO\nS=sync\tn=sync now "));
  MENU.outln(sync);

  MENU.out(F("E=enter experiment (")); MENU.out(selected_experiment); MENU.out(')');
  MENU.out(F("\tV=voices for experiment (")); MENU.out(voices); MENU.out(F(")"));
  MENU.out(F("\tO=action_flags (")); PULSES.show_action_flags(selected_actions); MENU.outln(')');

  MENU.out(F("g~=toggle pin mapping (bottom "));
  if (g_inverse)
    MENU.out(F("up"));
  else
    MENU.out(F("down"));
  MENU.outln(F(")\tZ=reverse_gpio_pins"));

  MENU.out(F("Scale (")); MENU.out(multiplier);MENU.slash();  MENU.out(divisor);
  MENU.out(F(")\t'*!'=multiplier '/!'=divisor"));

  MENU.ln();
}


// FIXME: obsolete? ################
int setup_jiffle_thrower_synced(struct time when,
				unsigned long unit,
				unsigned long multiplier, unsigned long divisor,
				int sync, unsigned int *jiffle)
{
 int pulse= PULSES.setup_pulse_synced(&do_throw_a_jiffle, ACTIVE,
			       when, unit, multiplier, divisor, sync);
  if ((pulse > ILLEGAL) && (pulse < PL_MAX)) {
    PULSES.set_gpio(pulse, this_or_next_gpio(pulse));
    PULSES.pulses[pulse].data = (unsigned int) jiffle;
  } else {
    out_noFreePulses();
  }

  return pulse;
}


/* **************************************************************** */
// jiffles:
// jiffles are (click) patterns defined in jiffletabs and based on a base period
//
// jiffletabs are unsigned int arrays
// the base period is multiplied/divided by two int values
// the following jiffletab value counts how many times the pulse will get
// woken up with this new computed period
// then continue with next jiffletab entries
// a zero multiplier ends the jiffle

// jiffletabs can define melodies:
// up to 256 triplets of {multiplier, divisor, count}
// multiplier and divisor determine period based on the starting pulses period
// a multiplier of zero indicates end of jiffle

#define JIFFLETAB_INDEX_STEP	3

void set_jiffle_RAM_value(int new_value) {
  char input;
  //  unsigned int* jiffle = selected_in(JIFFLES);

  jiffle_RAM[jiffle_write_index++]=new_value;

  // jiffletabs *MUST* have 2 trailing zeros	// ################ FIXME: ################
  if (jiffle_write_index >= (JIFFLE_RAM_SIZE - 2)) {	// array full?
    jiffle_RAM[jiffle_write_index--]=0;

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

  jiffle_RAM[JIFFLE_RAM_SIZE - 1 ] = 0;		// zero out last 2 array elements (savety net)
  jiffle_RAM[JIFFLE_RAM_SIZE - 2 ] = 0;
  MENU.menu_mode=0;				// stop numeric data input
  //  jiffle_write_index=0;		// no, we leave write index as is

  display_jiffletab(jiffle_RAM);		// put that here for now
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
  MENU.out(JIFFLES[0].name); MENU.out(':'); MENU.space();
  MENU.out(array2name(JIFFLES, jiffle));  MENU.tab();
  MENU.out(F("editing "));
  if (MENU.menu_mode==JIFFLE_ENTRY_UNTIL_ZERO_MODE) {
    MENU.out(F("on\tclose with '}'"));
    if (MENU.verbosity >= VERBOSITY_SOME)
      MENU.out(F("\tmove cursor < >\trange borders | \""));
  } else
    MENU.out(F("off\tstart edit with '{'"));	// FIXME: in both int_edit.h and pulses.ino ################

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
  MENU.out(F("length ")); display_fraction(&sum);// FIXME: in both int_edit.h and pulses.ino ################
  MENU.tab();
  MENU.out(F("cursor "));
  MENU.out(jiffle_write_index); MENU.slash(); MENU.out(len);
  MENU.ln();
}


void do_jiffle (int pulse) {	// to be called by pulse_do
/*
  PULSES.pulses[pulse].gpio		click pin
  PULSES.pulses[pulse].index		jiffletab index
  PULSES.pulses[pulse].countdown	count down
  PULSES.pulses[pulse].data		jiffletab[] pointer
  PULSES.pulses[pulse].base_time	base period = period of starting pulse
*/
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


// TODO: REWORK:  setup_bass_middle_high()  used in musicBox, but not really compatible
void setup_bass_middle_high(short bass_pulses, short middle_pulses, short high_pulses) {
  {
    MENU.out(F("setup_bass_middle_high("));	// ################ verbosity
    MENU.out(bass_pulses);
    MENU.out_comma_();
    MENU.out(middle_pulses);
    MENU.out_comma_();
    MENU.out(high_pulses);
    MENU.outln(')');
  }

  voices = bass_pulses + middle_pulses + high_pulses;	// init *all* primary pulses
  PULSES.select_n(voices);

  PULSES.add_selected_to_group(g_PRIMARY);

  // tune *all* primary pulses
  tune_2_scale(voices, multiplier, divisor, selected_in(SCALES));
  lower_audio_if_too_high(409600*2);	// 2 bass octaves  // TODO: adjust appropriate...

  // prepare primary pulse groups:

  // bass on DAC1 and broad angle LED lamps:
  // select_array_in(JIFFLES, d4096_512);

  PULSES.select_from_to(0, bass_pulses - 1);
  for(int pulse=0; pulse<bass_pulses; pulse++) {
    setup_icode_seeder(pulse, PULSES.pulses[pulse].period, (icode_t*) selected_in(iCODEs) , DACsq1 | doesICODE);

#if defined USE_i2c
  #if defined USE_MCP23017
    PULSES.set_i2c_addr_pin(pulse, 0x20, pulse);	// ???
  #endif
#endif
  }
  PULSES.add_selected_to_group(g_LOW_END);

  PULSES.select_from_to(0,bass_pulses);			// pulse[bass_pulses] belongs to both output groups
  // selected_DACsq_intensity_proportional(255, 1);
  selected_share_DACsq_intensity(255, 1);		// bass DAC1 intensity

  // 2 middle octaves on 15 gpios
  PULSES.select_from_to(bass_pulses, bass_pulses + middle_pulses -1);

  for(int pulse=bass_pulses; pulse<bass_pulses+middle_pulses; pulse++) {
    setup_icode_seeder(pulse, PULSES.pulses[pulse].period, (icode_t*) selected_in(JIFFLES), DACsq1 | doesICODE | CLICKs);
    PULSES.set_gpio(pulse, next_gpio());
  }
  PULSES.add_selected_to_group(g_MIDDLE);

  // fix topmost bass pulse pulse[bass_pulses] that belongs to both groups:
  PULSES.pulses[bass_pulses].dest_action_flags |= DACsq1;

  // high octave on DAC2
  PULSES.select_from_to(bass_pulses + middle_pulses -1, bass_pulses + middle_pulses + high_pulses -1);
  for(int pulse = bass_pulses + middle_pulses; pulse<voices; pulse++) {	// pulse[21] belongs to both groups
    setup_icode_seeder(pulse, PULSES.pulses[pulse].period, (icode_t*) d4096_256, DACsq2 | doesICODE);
  }
  PULSES.add_selected_to_group(g_HIGH_END);

  // fix pulse[21] belonging to both output groups
  PULSES.pulses[bass_pulses + middle_pulses - 1].dest_action_flags |= DACsq2;
  selected_share_DACsq_intensity(255, 2);
  //	selected_DACsq_intensity_proportional(255, 2);

  PULSES.select_n(voices);	// select all primary voices again
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

  'reverse_gpio_pins)' as alternative:
  'reverse_gpio_pins()' works on the global gpio_pins[] array
			 the pulses won't notice but play with new pin mapping */

// TODO: move to library Pulses
bool gpio_pins_inverted=false;
void reverse_gpio_pins() {	// TODO: fix next_gpio()  ???? ################
  gpio_pin_t scratch;
  for (int i=0, j=GPIO_PINS-1; i<j; i++, j--) {
      scratch=gpio_pins[i];
      gpio_pins[i]=gpio_pins[j];
      gpio_pins[j]=scratch;
  }

  gpio_pins_inverted=!gpio_pins_inverted;
}

// ****************************************************************
// menu_pulses_reaction()

// display helper functions:
void short_info() {
  if (MENU.verbosity > VERBOSITY_SOME) {
    MENU.ln();
    PULSES.time_info();
  }

  if (MENU.verbosity >= VERBOSITY_SOME) {
    MENU.ln();
    print_selected();
  }

  MENU.ln();
  selected_or_flagged_pulses_info_lines();
}


// helper functions to display parameters of menu functions:
void display_next_par(long parameter) {
  MENU.out_comma_();
  MENU.out(parameter);
}

void display_last_par(long parameter) {
  MENU.out_comma_();
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
  MENU.out(multiplier); MENU.slash(); MENU.out(divisor);
}

void show_UI_basic_setup() {
  MENU.out(F("SYNC: "));
  MENU.out(sync);
  MENU.space(4);

  MENU.out(F("SCALE: "));
  MENU.out(array2name(SCALES, selected_in(SCALES)));
  MENU.space(3);

  MENU.out(F("JIFFLE: "));
  MENU.out(array2name(JIFFLES, selected_in(JIFFLES)));
  MENU.space(3);

  MENU.out(F("SCALING: "));
  MENU.out(multiplier);
  MENU.slash();
  MENU.out(divisor);
  MENU.space(3);
  if(some_metric_tunings_only) {
    MENU.out(F("*metric* "));
    MENU.out(metric_mnemonic);
  }

  if (g_inverse)	// FIXME: TODO: check where that *is* used ################
    MENU.out(F("\tGPIO BOTTOM UP"));
  MENU.ln();	// maybe...
}

void show_UI_settings() {
  MENU.ln();
  show_UI_basic_setup();

  MENU.out(F("Af:"));
  PULSES.show_action_flags(selected_actions);
  MENU.tab();

  MENU.out(F("voices: "));
  MENU.out(voices);
  MENU.tab();

  MENU.verbosity_info();
  MENU.ln();

  PULSES.show_time_unit();
}

// for old style 'experiment'
void Press_toStart() {
  MENU.outln(F("Press '!' to start"));
}


void user_select_scale(unsigned int* scale) {
  if(scale != NULL) {
    select_array_in(SCALES, scale);
    scale_user_selected = true;
  }
}

void select_scale__UI() {
  /*
    void select_scale__UI() examples:
    "a"		metric a=440 *minor* scale
    "E"		metric E=330 *major* scale
    "d5"	metric d *minor pentatonic* scale
    "C5"	metric C *european pentatonic* scale
    "F4"	*tetraCHORD* (as a scale) on metric F
  */

  switch (MENU.cb_peek()) {
  case EOF:
    break;

  case 'u':	// harmonical time unit, minor
    metric_mnemonic = "u ";
    MENU.drop_input_token();
    user_select_scale(minor_scale);
    PULSES.time_unit=TIME_UNIT;	// switch to harmonical time unit
    multiplier=1;
    divisor=1;
    break;

  case 'U':	// harmonical time unit, major
    metric_mnemonic = "U ";
    MENU.drop_input_token();
    user_select_scale(major_scale);
    PULSES.time_unit=TIME_UNIT;	// switch to harmonical time unit
    multiplier=1;
    divisor=1;
    break;

  case 'c':	// c minor
    metric_mnemonic = "c ";
    MENU.drop_input_token();
    user_select_scale(minor_scale);
    PULSES.time_unit=1000000;	// switch to metric time unit
    divisor=262; // 261.63	// C4  ***not*** harmonical
    break;

  case 'C':	// c major
    metric_mnemonic = "C ";
    MENU.drop_input_token();
    user_select_scale(major_scale);
    PULSES.time_unit=1000000;	// switch to metric time unit
    divisor=262; // 261.63	// C4  ***not*** harmonical
    MENU.outln(" ok");
    break;

  case 'd':	// d minor scale
    metric_mnemonic = "d ";
    MENU.drop_input_token();
    PULSES.time_unit=1000000;	// switch to metric time unit
    divisor = 294;		// 293.66 = D4
    // divisor = 147;		// 146.83 = D3
    user_select_scale(minor_scale);
    break;

  case 'D':	// D major scale
    metric_mnemonic = "D ";
    MENU.drop_input_token();
    PULSES.time_unit=1000000;	// switch to metric time unit
    divisor = 294;		// 293.66 = D4
    // divisor = 147;		// 146.83 = D3
    user_select_scale(major_scale);
    break;

  case 'e':	// e minor scale
    metric_mnemonic = "e ";
    MENU.drop_input_token();
    user_select_scale(minor_scale);
    PULSES.time_unit=1000000;	// switch to metric time unit
    divisor=330; // 329.36	// e4  ***not*** harmonical
    break;

  case 'E':	// E major scale
    metric_mnemonic = "E ";
    MENU.drop_input_token();
    user_select_scale(major_scale);
    PULSES.time_unit=1000000;	// switch to metric time unit
    divisor=330; // 329.36	// e4  ***not*** harmonical
    break;

  case 'f':	// f minor
    metric_mnemonic = "f ";
    MENU.drop_input_token();
    user_select_scale(minor_scale);
    PULSES.time_unit=1000000;	// switch to metric time unit
    divisor=175; // 174.16	// F3  ***not*** harmonical
    break;

  case 'F':	// f major
    metric_mnemonic = "F ";
    MENU.drop_input_token();
    user_select_scale(major_scale);
    PULSES.time_unit=1000000;	// switch to metric time unit
    divisor=175; // 174.16	// F3  ***not*** harmonical
    break;

  case 'g':	// g minor
    metric_mnemonic = "g ";
    MENU.drop_input_token();
    user_select_scale(minor_scale);
    PULSES.time_unit=1000000;	// switch to metric time unit
    divisor=196; // 196.00	// G3  ***not*** harmonical
    break;

  case 'G':	// g major
    metric_mnemonic = "G ";
    MENU.drop_input_token();
    user_select_scale(major_scale);
    PULSES.time_unit=1000000;	// switch to metric time unit
    divisor=196; // 196.00	// G3  ***not*** harmonical
    break;

  case 'a':	// a minor scale
    metric_mnemonic = "a ";
    MENU.drop_input_token();
    PULSES.time_unit=1000000;	// switch to metric time unit
    divisor = 440;
    user_select_scale(minor_scale);
    break;

  case 'A':	// A major scale
    metric_mnemonic = "A ";
    MENU.drop_input_token();
    PULSES.time_unit=1000000;	// switch to metric time unit
    divisor = 440;
    user_select_scale(major_scale);
    break;

  case 'b':	// b minor
    metric_mnemonic = "b ";
    MENU.drop_input_token();
    user_select_scale(minor_scale);
    PULSES.time_unit=1000000;	// switch to metric time unit
    divisor=247; // 246.94	// B3  ***not*** harmonical
    break;

  case 'B':	// b major
    metric_mnemonic = "B ";
    MENU.drop_input_token();
    user_select_scale(major_scale);
    PULSES.time_unit=1000000;	// switch to metric time unit
    divisor=247; // 246.94	// B3  ***not*** harmonical
    break;
  }

  switch (MENU.cb_peek()) {	// (second or) third letters for other scales
  case EOF:
    break;
  case '6':	// doric scale	// TODO: check 14
    user_select_scale(doric_scale);
    break;
  case '5':	// 5  pentatonic (minor|major) scale
    MENU.drop_input_token();
    if ((selected_in(SCALES) == major_scale) || (selected_in(SCALES) == tetraCHORD)) {
      user_select_scale(european_PENTAtonic);
    } else {
      user_select_scale(pentatonic_minor);
    }
    break;
  case '4':	// 4  tetraCHORD | tetrachord
    MENU.drop_input_token();
    if ((selected_in(SCALES) == minor_scale) || (selected_in(SCALES) == pentatonic_minor) || \
	(selected_in(SCALES) == doric_scale) || (selected_in(SCALES) == triad))
      user_select_scale(tetrachord);
    else
      user_select_scale(tetraCHORD);
    break;
  case '3':	// 3  octaves fourths fifths
    MENU.drop_input_token();
    user_select_scale(octaves_fourths_fifths);
    break;
  case '2':	// 2  octaves fifths
    MENU.drop_input_token();
    user_select_scale(octaves_fifths);
    break;
  case '1':	// 1  octaves
    MENU.drop_input_token();
    user_select_scale(octaves);
    break;
  }
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
      for (int pulse=0; pulse<PL_MAX; pulse++)
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

      PULSES.maybe_show_selected_mask();
      if (DO_or_maybe_display(VERBOSITY_LOWEST))
	selected_or_flagged_pulses_info_lines();
      break;

    case 'A':	// '.A' select destination: *all* pulses
      MENU.drop_input_token();
      select_all();
      PULSES.maybe_show_selected_mask();
      if (DO_or_maybe_display(VERBOSITY_LOWEST))
	selected_or_flagged_pulses_info_lines();
      break;

    case 'v':	// '.v' select destination: select_n(voices)
      MENU.drop_input_token();
      PULSES.select_n(voices);
      if (DO_or_maybe_display(VERBOSITY_LOWEST))
	selected_or_flagged_pulses_info_lines();
      break;

    case 'V':	// '.V' select destination: alive voices
      MENU.drop_input_token();
      PULSES.clear_selection();
      for (int pulse=0; pulse<voices; pulse++)
	if(PULSES.pulses[pulse].flags && (PULSES.pulses[pulse].flags != SCRATCH))
	  PULSES.select_pulse(pulse);

      PULSES.maybe_show_selected_mask();
      if (DO_or_maybe_display(VERBOSITY_LOWEST))
	selected_or_flagged_pulses_info_lines();
      break;

    case 'L':	// '.L' select destination: all alive pulses
      MENU.drop_input_token();
      select_alive();
      PULSES.maybe_show_selected_mask();
      if (DO_or_maybe_display(VERBOSITY_LOWEST))
	selected_or_flagged_pulses_info_lines();
      break;
    }
    break;

/*
  case ':':	// reserved for MENU_MENU_PAGES_KEY
    break;
*/

  case ',':	// in normal mode show_UI_settings and selected_mask, else data input delimiter	see: 'menu_mode'
    if (MENU.menu_mode==JIFFLE_ENTRY_UNTIL_ZERO_MODE)	// TODO: REVIEW:
      display_jiffletab(selected_in(JIFFLES));
    else {
      show_UI_settings();
      PULSES.show_selected_mask();	// if called from here (not musicBox) *do* show selected mask
    }
    break;

  case '|':	// accept as noop in normal mode. used as range bottom delimiter in arrays
    if (MENU.menu_mode==JIFFLE_ENTRY_UNTIL_ZERO_MODE) {
      jiffle_range_bottom = jiffle_write_index;
      fix_jiffle_range();

      if(MENU.cb_peek()==EOF)
	if (MENU.verbosity)
	  display_jiffletab(selected_in(JIFFLES));
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
	  display_jiffletab(selected_in(JIFFLES));
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
      if((menu_input < 0) || (menu_input >= PL_MAX))
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
      if((menu_input < 0) || (menu_input >= PL_MAX))
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
	  display_jiffletab(selected_in(JIFFLES));
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
	  display_jiffletab(selected_in(JIFFLES));
      break;
    default:
      if (MENU.verbosity >= VERBOSITY_SOME) {
	MENU.out_noop();
	MENU.ln();
      }
    }
    break;

  case 'u':	// PULSES.time_unit calculate or else select time_unit as destination
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

  case 'x':	// clear destination selection  same as '.x'
    PULSES.clear_selection();
    PULSES.maybe_show_selected_mask();
    break;

  case 's':	// switch pulse ACTIVE on/off
    for (int pulse=0; pulse<PL_MAX; pulse++) {
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

    if (DO_or_maybe_display(VERBOSITY_MORE)) {
      MENU.ln();
      selected_or_flagged_pulses_info_lines();
    }
    break;

  case 'S':	// enter sync
    if (MENU.maybe_calculate_input(&input_value)) {
      if (input_value>=0 ) {
	sync = input_value;
	sync_user_selected = true;
      }
      else
	MENU.out(F("positive integer only"));
    }

    if (DO_or_maybe_display(VERBOSITY_LOWEST)) {
      MENU.out(F("sync "));
      MENU.outln(sync);
    }

    break;

  case 'i':	// en_info	TODO: free for '*' en_iCode_thrower
    for (int pulse=0; pulse<PL_MAX; pulse++)
      if (PULSES.pulse_is_selected(pulse))
	en_info(pulse);

    if (DO_or_maybe_display(VERBOSITY_LOWEST)) {
      MENU.ln();
      selected_or_flagged_pulses_info_lines();
    }
    break;
/*
  // RESERVED
  case 'I':	// iCode
    // icode_user_selected = true;
    break;
*/
  case 'M':	// "mute", see 'N' as alternative
    PULSES.mute_all_actions();	// TODO: dead end street

    if (DO_or_maybe_display(VERBOSITY_LOWEST))
      MENU.outln(F("muted all actions"));
    break;

//	#if defined USE_MORSE	// TODO: fix or remove
//	  case 'm':	// toggle MORSE on/off
//	    morse = ! morse;
//	    break;
//	#endif

  case '*':	// multiply destination
    if(MENU.cb_peek() != '!') {		// '*' (*not* '*!<num>' set multiplier)
      switch (dest) {
      case CODE_PULSES:
	input_value = MENU.numeric_input(1);
	if (input_value>=0) {
	  for (int pulse=0; pulse<PL_MAX; pulse++)
	    if (PULSES.pulse_is_selected(pulse))
	      PULSES.multiply_period(pulse, input_value);

	  PULSES.fix_global_next();

	  if (DO_or_maybe_display(VERBOSITY_MORE)) {	// TODO: '*' maybe VERBOSITY_SOME
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

      if (DO_or_maybe_display(VERBOSITY_LOWEST)) {
	show_scaling();
	MENU.ln();
      }
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

      if (DO_or_maybe_display(VERBOSITY_LOWEST)) {
	show_scaling();
	MENU.ln();
      }

    } else {	// '/' divide destination
      switch (dest) {
      case CODE_PULSES:
	input_value = MENU.numeric_input(1);
	if (input_value>=0) {
	  for (int pulse=0; pulse<PL_MAX; pulse++)
	    if (PULSES.pulse_is_selected(pulse))
	      PULSES.divide_period(pulse, input_value);

	  PULSES.fix_global_next();

	  if (DO_or_maybe_display(VERBOSITY_MORE)) {
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
	for (int pulse=0; pulse<PL_MAX; pulse++)
	  if (PULSES.pulse_is_selected(pulse)) {
	    time_scratch.time = PULSES.time_unit;
	    time_scratch.overflow = 0;
	    PULSES.mul_time(&time_scratch, input_value);
	    PULSES.set_new_period(pulse, time_scratch);
	  }

	PULSES.fix_global_next();

	if (DO_or_maybe_display(VERBOSITY_MORE)) {
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
      for (int pulse=0; pulse<PL_MAX; pulse++)
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

    if (DO_or_maybe_display(VERBOSITY_SOME)) {
      selected_or_flagged_pulses_info_lines();
      MENU.ln();
    }
    break;

  case 'P':	// pulse create and edit
    PULSES.reset_and_edit_selected();
    if (DO_or_maybe_display(VERBOSITY_LOWEST)) {
      MENU.ln();
      selected_or_flagged_pulses_info_lines();
    }
    break;

  case 'n':	// synchronize to now
    // see also '!'	one of 'n' and '!' is obsolete...
    PULSES.activate_selected_synced_now(sync);	// sync and activate

    if (DO_or_maybe_display(VERBOSITY_HIGH)) {
      MENU.ln();
      selected_or_flagged_pulses_info_lines();
    }
    break;

  case 'N':	// NULLs payload
    // we work on pulses anyway, regardless dest
    for (int pulse=0; pulse<PL_MAX; pulse++)
      if (PULSES.pulse_is_selected(pulse)) {
	if ((PULSES.pulses[pulse].flags & HAS_GPIO) && (PULSES.pulses[pulse].gpio != ILLEGAL))
	  digitalWrite(PULSES.pulses[pulse].gpio, LOW);	// set clicks on LOW
	PULSES.set_payload(pulse, NULL);
      }

    if (DO_or_maybe_display(VERBOSITY_SOME)) {
      MENU.ln();
      selected_or_flagged_pulses_info_lines();
    }
    break;

  case 'g':	// 'g' "GPIO"	'g~' toggle up/down pin mapping
    if(MENU.cb_peek() == '~') {	      // 'g~' toggle up/down pin mapping
	  if (MENU.verbosity)
	    MENU.out(F("pin mapping bottom "));

	  g_inverse = !g_inverse;	// toggle bottom up/down click-pin mapping

	  if (DO_or_maybe_display(VERBOSITY_LOWEST)) {
	    if (g_inverse)
	      MENU.outln(F("up"));
	    else
	      MENU.outln(F("down"));
	  }
    } else
      en_click_selected();	// 'g' en_click  "GPIO"
    break;

#ifdef IMPLEMENT_TUNING		// implies floating point
  case 'w':	// sweep activate or toggle direction
    sweep_up *= -1;	// toggle direction up down

    if (sweep_up==0)	// start sweeping if it was disabled
      sweep_up=1;

    if (DO_or_maybe_display(VERBOSITY_LOWEST))
      sweep_info();

    break;

  case 'W':	// sweep info and control
    next_token = MENU.cb_peek();
    if (next_token == (char) EOF) {	// *no* input after 'W': maybe start, info
      if (sweep_up==0)
	sweep_up=1;		//    start sweeping up if disabled

      if (DO_or_maybe_display(VERBOSITY_LOWEST))
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

    if (DO_or_maybe_display(VERBOSITY_LOWEST))
      sweep_info();
    break;

  case 't':	// en_sweep_click
    // we work on voices anyway, regardless dest
    for (int pulse=0; pulse<voices; pulse++)
      if (PULSES.pulse_is_selected(pulse))
	en_sweep_click(pulse);

    PULSES.fix_global_next();	// just in case?

    if (DO_or_maybe_display(VERBOSITY_MORE)) {
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

    if (DO_or_maybe_display(VERBOSITY_MORE)) {
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

    if (DO_or_maybe_display(VERBOSITY_MORE)) {
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

    if (DO_or_maybe_display(VERBOSITY_MORE)) {
      MENU.ln();
      selected_or_flagged_pulses_info_lines();
    }
    break;
#endif	// #ifdef IMPLEMENT_TUNING	implies floating point

  case 'j':	// en_jiffle_thrower
    setup_jiffle_thrower_selected(selected_actions);
    break;

  case 'J':	// select, edit, load jiffle
    /*
      'J'  shows registered jiffle names and display_jiffletab(<selected_jiffle>)
      'J7' selects jiffle #7 and display_jiffletab()
      'J!' copy selected jiffle in jiffle_RAM, select jiffle_RAM, display_jiffletab(jiffle_RAM)
      'J9!' copy jiffle #9 in jiffle_RAM, select jiffle_RAM, display_jiffletab(jiffle_RAM)
    */
    // some jiffles from source, some very old FIXME:	review and delete	################
    if (MENU.cb_peek() != '!')		// 'J<num>' selects jiffle
      if (UI_select_from_DB(JIFFLES))	// select jiffle UI
	jiffle_user_selected = true;

    if (MENU.cb_peek() == '!') {	// 'J[<num>]!' copies an already selected jiffletab to RAM, selects RAM
      MENU.drop_input_token();
      if(selected_in(JIFFLES) != jiffle_RAM) {
	unsigned int * source=selected_in(JIFFLES);
	// jiffle_write_index=0;	// no, write starting at jiffle_write_index #### FIXME: ####
	load2_jiffle_RAM(source);
      }
      select_array_in(JIFFLES, jiffle_RAM);
      jiffle_user_selected = true;
    }

    if (MENU.verbosity >= VERBOSITY_SOME)
	display_jiffletab(selected_in(JIFFLES));
    break;

  // 'R' OBSOLETE????	################
  case 'R':	// scale  was: ratio	// TODO: see musicBox 'R'	make them identical? ################
    if (DO_or_maybe_display(VERBOSITY_SOME))
      MENU.out(F("scale "));

    // 'R!' tune selected pulses to a scale starting from lowest
    if (MENU.cb_peek()=='!') {
      tune_2_scale(voices, multiplier, divisor, selected_in(SCALES));	// tune-2-scale FIXME: *selected*
      scale_user_selected = true;
    }
    else	// ui select a scale
      if(UI_select_from_DB(SCALES))	// select scale
	scale_user_selected = true;

    if (DO_or_maybe_display(VERBOSITY_LOWEST))
      display_arr(selected_in(SCALES), 2);

    break;

  case 'F':	// en_INFO
    // we work on pulses anyway, regardless dest
    for (int pulse=0; pulse<PL_MAX; pulse++)
      if (PULSES.pulse_is_selected(pulse))
	en_INFO(pulse);

    if (DO_or_maybe_display(VERBOSITY_MORE)) {
      MENU.ln();
      selected_or_flagged_pulses_info_lines();
    }
    break;

  case '{':	// enter_jiffletab
    MENU.menu_mode=JIFFLE_ENTRY_UNTIL_ZERO_MODE;
    select_array_in(JIFFLES, jiffle_RAM);
    jiffle_write_index=0;	// ################ FIXME: ################
    select_array_in(JIFFLES, jiffle_RAM);
    if(MENU.cb_peek()==EOF)
      if (MENU.verbosity)
	display_jiffletab(selected_in(JIFFLES));
    break;

  case '}':	// display jiffletab, stop editing jiffletab
    MENU.menu_mode=0;
    if (MENU.verbosity)
      display_jiffletab(selected_in(JIFFLES));

    jiffle_write_index=0;	// ################ FIXME: ################
    break;

  case 'C':	// Calculator simply *left to right*	*positive integers only*
    MENU.maybe_calculate_input(&calc_result);

    MENU.out(F("Calc => ")), MENU.outln(calc_result);
    if (calc_result > 1) {	// show prime factors if(result >= 2)
      int p_factors_size=6;	// for harmonics I rarely use more than three, sometimes four ;)
      unsigned int p_factors[p_factors_size];
      MENU.out(F("prime factors of ")); MENU.out(calc_result);
      int highest = HARMONICAL.prime_factors(p_factors, calc_result);
      for (int i=0; HARMONICAL.small_primes[i]<=highest; i++) {
	MENU.tab();
	MENU.out(F("("));
	MENU.out(HARMONICAL.small_primes[i]);
	MENU.out(F(")^"));
	MENU.out(p_factors[i]);
      }
      MENU.ln();
    }
    break;

  case 'D':	// DADA reserved for temporary code   testing debugging ...
    // MENU.out_noop(); MENU.ln();
    #include "menu_D_tmp.h"
    break;

  case 'y':	// DADA reserved for temporary code   testing debugging ...
    // MENU.out_noop(); MENU.ln();
    #include "menu_y_tmp.h"
    break;

  case 'z':	// DADA reserved for temporary code   testing debugging ...
    // MENU.out_noop(); MENU.ln();
    #include "menu_z_tmp.h"
    break;

  case 'V':	// set voices	V[num]! PULSES.select_n_voices
    if(MENU.cb_peek()==EOF)
      MENU.out(F("voices "));

    input_value = MENU.numeric_input(voices);
    if (input_value>0 && input_value<=PL_MAX) {
      voices = input_value;
      if (voices>GPIO_PINS) {
	if (DO_or_maybe_display(VERBOSITY_LOWEST))
	  MENU.outln(F("WARNING: voices > gpio"));
      }
    }
    else
      MENU.outln_invalid();

    if (voices==0)
      voices=GPIO_PINS;	// just a guess

    if (DO_or_maybe_display(VERBOSITY_LOWEST))
      MENU.outln(voices);

    if(MENU.cb_peek()=='!')
      PULSES.select_n(voices);

    break;

  case 'O':	// configure selected_actions
    if (MENU.cb_peek() == EOF) {
      MENU.out(F("action flags "));
      PULSES.show_action_flags(selected_actions);
      MENU.ln();
    } else {
      // TODO: allow mnemonics ################
      input_value = MENU.numeric_input(selected_actions);
      if (input_value != selected_actions)
	selected_actions = input_value;

      if (MENU.verbosity >= VERBOSITY_SOME) {
	MENU.out(F("action flags "));
	PULSES.show_action_flags(selected_actions);
	MENU.ln();
      }
    }
    break;

#if defined AUTOSTART
  case 'A':	// autostart
    AUTOSTART
    break;
#endif

  case 'X':	// PANIC BUTTON  reset, remove all (flagged) pulses, restart selections at none
    // reset, remove all (flagged) pulses, restart selections at none, reset selection mask
    // set MCP23017 pins low
    reset_all_flagged_pulses_GPIO_OFF();

#if defined HARMONICAL_MUSIC_BOX
    set_MusicBoxState(OFF);
#endif

    if(MENU.cb_peek() == '!') {		// 'X!' does 'X' *and* resets time_unit
      MENU.drop_input_token();

      if(PULSES.time_unit != TIME_UNIT) {
	MENU.outln(F("reset time_unit"));
	PULSES.time_unit = TIME_UNIT;	// reset time_unit
      }
    } else
      if(PULSES.time_unit != TIME_UNIT)	// info time_unit not reset
	MENU.outln(F("'X!' to reset time_unit"));

    break;

  case 'Z':	// reverse_gpio_pins
    reverse_gpio_pins();	// TODO: fix next_gpio()  ???? ################

    if (DO_or_maybe_display(VERBOSITY_LOWEST))
      MENU.outln(F("reverse_gpio_pins"));

    break;

  case 'E':	// experiment, setups, instruments
    if (MENU.maybe_display_more()) {
      MENU.out(F("experiment "));
    }

    input_value = MENU.numeric_input(-1);	// select experiment

    if (input_value==-1)
      display_names(experiment_names, n_experiment_names, selected_experiment);

    else if (input_value>=0 ) {	// select, initialize experiment
      selected_experiment = input_value;
      switch (selected_experiment) {
	/* some old style setups just initialise defaults,  but do not start yet, press '!'
	   others do start if you add a '!' as last char
	   you can edit and then press '!' or 'n' (== synchronise to *now*)
	   sorry, it's messy!	TODO: unmess
	*/

//      case 0:	// free setup planed
//	  break;

      case 1:	// setup_jiffle128
	multiplier=2;
	divisor=1;
	sync=15;

	if (MENU.maybe_display_more()) {
	  display_name5pars("setup_jiffle128", g_inverse, voices, multiplier, divisor, sync);
	  Press_toStart();
	}
	break;

      case 2:	// init_div_123456
	sync=0;
	multiplier=1;
	divisor=1;

	if (MENU.maybe_display_more()) {
	  display_name5pars("init_div_123456", g_inverse, voices, multiplier, divisor, sync);
	  Press_toStart();
	}
	break;

      case 3:	// setup_jiffles0
	sync=1;
	multiplier=8;
	divisor=3;
	reverse_gpio_pins();

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

      case 4:	// setup_jiffles2345
	multiplier=1;
	divisor=2;
	sync=0;
	select_array_in(JIFFLES, jiffletab);

	if (MENU.maybe_display_more()) {
	  display_name5pars("setup_jiffles2345", g_inverse, voices, multiplier, divisor, sync);
	  Press_toStart();
	}
	break;

      case 5:	// init_123456
	sync=0;		// FIXME: test and select ################
	multiplier=3;
	divisor=1;

	if (MENU.maybe_display_more()) {
	  display_name5pars("init_123456", g_inverse, voices, multiplier, divisor, sync);
	  Press_toStart();
	}
	break;

      case 6:	// init_chord_1345689a
	sync=0;		// FIXME: test and select ################
	multiplier=1;
	divisor=1;

	if (MENU.maybe_display_more()) {
	  display_name5pars("init_chord_1345689a", g_inverse, voices, multiplier, divisor, sync);
	  Press_toStart();
	}
	break;

      case 7:	// init_rhythm_1
	sync=1;
	multiplier=1;
	divisor=6*7;

	if (MENU.maybe_display_more()) {
	  display_name5pars("init_rhythm_1", g_inverse, voices, multiplier, divisor, sync);
	  Press_toStart();
	}
	break;

      case 8:	// init_rhythm_2
	sync=5;
	multiplier=1;
	divisor=1;

	if (MENU.maybe_display_more()) {
	  display_name5pars("init_rhythm_2", g_inverse, voices, multiplier, divisor, sync);
	  Press_toStart();
	}
	break;

      case 9:  // init_rhythm_3
	sync=3;
	multiplier=1;
	divisor=1;

	if (MENU.maybe_display_more()) {
	  display_name5pars("init_rhythm_3", g_inverse, voices, multiplier, divisor, sync);
	  Press_toStart();
	}
	break;

      case 10:	// init_rhythm_4
	sync=1;
	multiplier=1;
	divisor=7L*3L;

	if (MENU.maybe_display_more()) {
	  display_name5pars("init_rhythm_4", g_inverse, voices, multiplier, divisor, sync);
	  Press_toStart();
	}
	break;

      case 11:	// setup_jifflesNEW
	sync=3;
	multiplier=3;
	divisor=1;

	if (MENU.maybe_display_more()) {
	  display_name5pars("setup_jifflesNEW", g_inverse, voices, multiplier, divisor, sync);
	  Press_toStart();
	}
	break;

      case 12:	// init_pentatonic
//	multiplier=4;
	multiplier=1;
	divisor=1;

	select_array_in(SCALES, minor_scale);		// default e minor

//	if (MENU.maybe_display_more()) {
//	  display_name5pars("init_pentatonic", g_inverse, voices, multiplier, divisor, sync);
//	}

//	init_pentatonic(g_inverse, voices, multiplier, divisor, sync);
	select_array_in(JIFFLES, piip2048);		// default jiffle FIXME: ################

	if (voices == 0)
	  voices = GPIO_PINS;

	PULSES.select_n(voices);
	tune_2_scale(voices, multiplier, divisor, selected_in(SCALES));

  #ifndef USE_DACs	// TODO: review and use test code
	setup_jiffle_thrower_selected(selected_actions);
  #else // *do* use dac		// TODO: not here ################
	selected_share_DACsq_intensity(255, 1);

    #if (USE_DACs == 1)
	setup_jiffle_thrower_selected(DACsq1);
    #else
	selected_DACsq_intensity_proportional(255, 2);
//	selected_share_DACsq_intensity(255, 2);

	setup_jiffle_thrower_selected(DACsq1 | DACsq2);
    #endif
  #endif

	PULSES.activate_selected_synced_now(sync);	// sync and activate;
	MENU.ln();

	if (MENU.maybe_display_more())
	  selected_or_flagged_pulses_info_lines();

	break;


      case 13:	// prepare_magnets
	sync=1;	// or: sync=0;
	multiplier=1;
	divisor=1;
	voices=8;	//just for 'The Harmonical Strings Christmas Evening Sounds'
	g_inverse=false;
	// unsigned int harmonics4 = {1,1,1024, 1,2,1024, 1,3,1024, 1,4,1024, 0,0};
	select_array_in(JIFFLES, harmonics4);
	PULSES.select_n(voices);
	display_name5pars("prepare_magnets", g_inverse, voices, multiplier, divisor, sync);
	prepare_magnets(g_inverse, voices, multiplier, divisor, sync);

	if (MENU.maybe_display_more()) {
	  selected_or_flagged_pulses_info_lines();
	  Press_toStart();
	}
	break;

      case 14:	// E14
	// magnets on strings, second take
	multiplier=1;
	divisor=1;
	g_inverse=false;

	select_array_in(SCALES, pentatonic_minor);
	PULSES.select_n(voices);
	prepare_scale(false, voices, multiplier * 1024 , divisor * 1167, sync, selected_in(SCALES));
	select_array_in(JIFFLES, ting1024);
	PULSES.select_n(voices);
	display_name5pars("E14", g_inverse, voices, multiplier, divisor, sync);

	if (MENU.maybe_display_more())
	  selected_or_flagged_pulses_info_lines();
	break;

      case 15:	// E15
	// magnets on strings, third take
	multiplier=1;
	divisor=1;
	g_inverse=false;

	select_array_in(SCALES, pentatonic_minor);
	PULSES.select_n(voices);
	prepare_scale(false, voices, multiplier * 4096 , divisor * 1167, sync, selected_in(SCALES));
	select_array_in(JIFFLES, ting4096);
	PULSES.select_n(voices);
	display_name5pars("E15", g_inverse, voices, multiplier, divisor, sync);

	if (MENU.verbosity >= VERBOSITY_SOME)
	  selected_or_flagged_pulses_info_lines();
	break;

      case 16:	// E16 european_PENTAtonic
	// piezzos on low strings 2016-12-28
	multiplier=4096;
	divisor=256;
	g_inverse=false;

	select_array_in(SCALES, european_PENTAtonic);
	PULSES.select_n(voices);
	prepare_scale(false, voices, multiplier, divisor, sync, selected_in(SCALES));
	select_array_in(JIFFLES, ting4096);
	// select_array_in(JIFFLES, arpeggio4096);
	display_name5pars("E16 PENTAtonic", g_inverse, voices, multiplier, divisor, sync);

	if (MENU.verbosity >= VERBOSITY_SOME)
	  selected_or_flagged_pulses_info_lines();
	break;

      case 17:	// E17 mimic japan
	// magnets on steel strings, "japanese"
	multiplier=1;	// click
	// multiplier=4096;	// jiffle ting4096
	divisor=256*5;

	select_array_in(SCALES, mimic_japan_pentatonic);
	PULSES.select_n(voices);
	prepare_scale(false, voices, multiplier, divisor, sync, selected_in(SCALES));
	select_array_in(JIFFLES, ting4096);
	display_name5pars("E17 mimic japan", g_inverse, voices, multiplier, divisor, sync);

	if (MENU.verbosity >= VERBOSITY_SOME)
	  selected_or_flagged_pulses_info_lines();

	break;

      case 18:	// E18 pentatonic minor  nylon stringed wooden box, piezzos
	select_array_in(SCALES, pentatonic_minor);
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
	select_array_in(JIFFLES, ting4096);

	PULSES.select_n(voices);
	prepare_scale(false, voices, multiplier, divisor, sync, selected_in(SCALES));
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

      case 20:	// macro
	select_array_in(JIFFLES, arpeggio4096);
	MENU.play_KB_macro("X E12! .a N *8 J20-.");
	break;

      case 21:	// macro
	select_array_in(JIFFLES, arpeggio4096down);
	MENU.play_KB_macro("X E12! .a N *16 J21-.");
	break;

      case 22:	// macro
	select_array_in(JIFFLES, arpeggio_cont);				// :)	with pizzs
	MENU.play_KB_macro("X E12! .a N *16 J22-.");
	break;

      case 23:	// macro
	select_array_in(JIFFLES, arpeggio_and_down);			// :) :)  arpeggio down instead pizzs
	MENU.play_KB_macro("X E12! .a N *16 J23-.");
	break;

      case 24:	// macro
	select_array_in(JIFFLES, stepping_down);				// :) :)  stepping down
	MENU.play_KB_macro("X E12 S=0 ! .a N *16 J24-.");
	break;

      case 25:	// macro
	select_array_in(JIFFLES, back_to_ground);		// rhythm slowdown
	MENU.play_KB_macro("X E12! .a N *32 J25-.");		// :)	rhythm slowdown
	break;

      case 26:	// macro
	select_array_in(JIFFLES, arpeggio_and_sayling);
	MENU.play_KB_macro("X E12! .a N *32 J26-.");
	break;

      case 27:	// macro
	select_array_in(JIFFLES, simple_theme);
	MENU.play_KB_macro("X E12! .a N *2 -.");
	break;

      case 28:	// macro   for tuning
	select_array_in(JIFFLES, peepeep4096);
	MENU.play_KB_macro("X E12! .a N *2 -.");

	break;

      case 29:	// E29 KALIMBA7 tuning
	reset_all_flagged_pulses_GPIO_OFF();

#if defined KALIMBA7_v2	// ESP32 version  european_PENTAtonic
	select_array_in(SCALES, european_PENTAtonic);
	voices=7;
#else
	select_array_in(SCALES, pentatonic_minor);	// default, including KALIMBA7_v1
#endif
#if defined  KALIMBA7_v1
	voices=7;
#endif

	multiplier=1;
	divisor=1024;
	PULSES.select_n(voices);
	prepare_scale(false, voices, multiplier, divisor, sync, selected_in(SCALES));
	display_name5pars("E29 KALIMBA7 tuning", g_inverse, voices, multiplier, divisor, sync);
	en_click_selected();							// for tuning ;)
	PULSES.activate_selected_synced_now(sync);	// sync and activate
	MENU.ln();

	if (MENU.verbosity >= VERBOSITY_SOME)
	  selected_or_flagged_pulses_info_lines();

	break;

      case 30:	// KALIMBA7 jiff
	select_array_in(SCALES, pentatonic_minor);
	voices=7;
	// voices=8;
	multiplier=4;
	divisor=1;
	// select_array_in(JIFFLES, peepeep4096);
	select_array_in(JIFFLES, ting4096);
	// select_array_in(JIFFLES, tingeling4096);
	PULSES.select_n(voices);
	prepare_scale(false, voices, multiplier, divisor, sync, selected_in(SCALES));
	display_name5pars("E30 KALIMBA7 jiff", g_inverse, voices, multiplier, divisor, sync);
	setup_jiffle_thrower_selected(selected_actions);
	PULSES.activate_selected_synced_now(sync);	// sync and activate

	MENU.ln();

	if (MENU.verbosity >= VERBOSITY_SOME)
	  selected_or_flagged_pulses_info_lines();

	break;

      case 31:	// E31 KALIMBA7 jiff
	select_array_in(SCALES, european_PENTAtonic);
	voices=7;
	multiplier=4;
	divisor=1;
	select_array_in(JIFFLES, ting4096);
	PULSES.select_n(voices);
	prepare_scale(false, voices, multiplier, divisor, sync, selected_in(SCALES));
	display_name5pars("E31 KALIMBA7 jiff", g_inverse, voices, multiplier, divisor, sync);
	setup_jiffle_thrower_selected(selected_actions);
	PULSES.activate_selected_synced_now(sync);	// sync and activate;
	MENU.ln();

	if (MENU.verbosity >= VERBOSITY_SOME)
	  selected_or_flagged_pulses_info_lines();

	break;

      case 32:	// ESP32_12 ff
	select_array_in(SCALES, major_scale);
	voices=GPIO_PINS;
	multiplier=4;
	divisor=1;
	// select_array_in(JIFFLES, ting4096);
	select_array_in(JIFFLES, tigg_ding4096);
	PULSES.select_n(voices);
	prepare_scale(false, voices, multiplier, divisor, sync, selected_in(SCALES));
	display_name5pars("E32 ESP32_12", g_inverse, voices, multiplier, divisor, sync);
	setup_jiffle_thrower_selected(selected_actions);
	PULSES.activate_selected_synced_now(sync);	// sync and activate;
	MENU.ln();

	if (MENU.verbosity >= VERBOSITY_SOME)
	  selected_or_flagged_pulses_info_lines();

	break;

      case 33:	// variation
	select_array_in(SCALES, minor_scale);
	voices=GPIO_PINS;
	multiplier=4;
	divisor=1;
	select_array_in(JIFFLES, ting4096);
	PULSES.select_n(voices);
	prepare_scale(false, voices, multiplier, divisor, sync, selected_in(SCALES));
	display_name5pars("minor", g_inverse, voices, multiplier, divisor, sync);
	setup_jiffle_thrower_selected(selected_actions);
	PULSES.activate_selected_synced_now(sync);	// sync and activate;
	MENU.ln();

	if (MENU.verbosity >= VERBOSITY_SOME)
	  selected_or_flagged_pulses_info_lines();

	break;

      case 34:	// same, major?
	select_array_in(SCALES, major_scale);
	voices=GPIO_PINS;
	multiplier=4;
	divisor=1;
	select_array_in(JIFFLES, ting4096);
	PULSES.select_n(voices);
	prepare_scale(false, voices, multiplier, divisor, sync, selected_in(SCALES));
	display_name5pars("major", g_inverse, voices, multiplier, divisor, sync);
	setup_jiffle_thrower_selected(selected_actions);
	PULSES.activate_selected_synced_now(sync);	// sync and activate;
	MENU.ln();

	if (MENU.verbosity >= VERBOSITY_SOME)
	  selected_or_flagged_pulses_info_lines();

	break;

      case 35:	// tetra
	select_array_in(SCALES, tetraCHORD);
	voices=GPIO_PINS;
	multiplier=4;
	divisor=1;
	select_array_in(JIFFLES, ting4096);
	PULSES.select_n(voices);
	prepare_scale(false, voices, multiplier, divisor, sync, selected_in(SCALES));
	display_name5pars("tetra", g_inverse, voices, multiplier, divisor, sync);
	setup_jiffle_thrower_selected(selected_actions);
	PULSES.activate_selected_synced_now(sync);	// sync and activate;
	MENU.ln();

	if (MENU.verbosity >= VERBOSITY_SOME)
	  selected_or_flagged_pulses_info_lines();

	break;

      case 36:	// BIG major
	select_array_in(SCALES, pentatonic_minor);
	voices=GPIO_PINS;
	multiplier=6;
	divisor=1;
	select_array_in(JIFFLES, ting4096);
	PULSES.select_n(voices);
	prepare_scale(false, voices, multiplier, divisor, sync, selected_in(SCALES));
	display_name5pars("BIG major", g_inverse, voices, multiplier, divisor, sync);
	setup_jiffle_thrower_selected(selected_actions);
	PULSES.activate_selected_synced_now(sync);	// sync and activate;
	MENU.ln();

	if (MENU.verbosity >= VERBOSITY_SOME)
	  selected_or_flagged_pulses_info_lines();

	break;

      case 37:	// Guitar and other instruments
	reset_all_flagged_pulses_GPIO_OFF();
	next_gpio(0);	// reset gpio usage

	PULSES.time_unit=1000000;

	// default tuning e
	multiplier=4096;
	// divisor=440;			// a4
	divisor=330; // 329.36		// e4  ***not*** harmonical
	// divisor=165; // 164.81		// e3  ***not*** harmonical

	select_array_in(SCALES, minor_scale);	// default e minor

	select_array_in(JIFFLES, ting4096);	// default jiffle
	//	voices = 16;			// for DAC output
	if (voices == 0)
	  voices = 15;			// default (diatonic)	// for DAC output

	select_scale__UI();	// second/third letters choose metric scales

	// select_array_in(JIFFLES, ting4096);
	// select_array_in(JIFFLES, piip2048);
	// select_array_in(JIFFLES, tanboura); divisor *= 2;

	// ################ FIXME: remove redundant code ################
	PULSES.select_n(voices);
//	prepare_scale(false, voices, multiplier, divisor, 0, selected_in(SCALES));
//	display_name5pars("GUITAR", g_inverse, voices, multiplier, divisor, sync);
	tune_2_scale(voices, multiplier, divisor, selected_in(SCALES));
	lower_audio_if_too_high(409600);

  #ifndef USE_DACs	// TODO: review and use test code
	setup_jiffle_thrower_selected(selected_actions);
  #else // *do* use dac		// TODO: not here ################
	selected_share_DACsq_intensity(255, 1);

    #if (USE_DACs == 1)
	setup_jiffle_thrower_selected(DACsq1);
    #else
	selected_DACsq_intensity_proportional(255, 2);
//	selected_share_DACsq_intensity(255, 2);

	setup_jiffle_thrower_selected(DACsq1 | DACsq2);
    #endif
  #endif

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


      case 38:	// 'E38' time machine setup
	// ESP32_15_clicks_no_display_TIME_MACHINE1

	// if (voices == 0)	// maybe, maybe not...
	voices = 32;	// init *all* primary pulses
	PULSES.select_n(voices);

	PULSES.time_unit=1000000;	// TODO:  maybe, maybe not...

	// default tuning a
	multiplier=4096;	// bass octave added and one lower...
	//	divisor=440;		// a
	//	divisor=220;		// a
	divisor=110;		// a
	// divisor=55;		// a

	// divisor=330; // 329.36		// e4  ***not*** harmonical
	// divisor=165; // 164.81		// e3  ***not*** harmonical

	select_array_in(SCALES, minor_scale);		// default e minor
	select_scale__UI();	// second/third letters choose metric scales

	// tune *all* primary pulses
	tune_2_scale(voices, multiplier, divisor, selected_in(SCALES));
	lower_audio_if_too_high(409600);

	// prepare primary pulse groups:
	select_array_in(JIFFLES, d4096_512);

	// bass on DAC1 and planed broad angle LED lamps
	// select_array_in(JIFFLES, d4096_512);
	PULSES.select_from_to(0,6);
	for(int pulse=0; pulse<=6; pulse++) {
	  en_jiffle_thrower(pulse, selected_in(JIFFLES), ILLEGAL, DACsq1);	// FIXME: use inbuilt click
	}
	PULSES.select_from_to(0,7);
	// selected_DACsq_intensity_proportional(255, 1);
	selected_share_DACsq_intensity(255, 1);

	// 2 middle octaves on 15 gpios
	// select_array_in(JIFFLES, d4096_512);
	//	select_array_in(JIFFLES, d4096_256);
	PULSES.select_from_to(7,6+15);
	setup_jiffle_thrower_selected(0);	// overwrites pulse[7]
	//	setup_jiffle_thrower_selected(DACsq2);
	//	selected_share_DACsq_intensity(255, 2);

	// fix pulse[7] that belongs to both groups:
	PULSES.pulses[7].dest_action_flags |= DACsq1;

	// high octave on DAC2
	//	select_array_in(JIFFLES, d4096_64);
	//select_array_in(JIFFLES, d4096_256);

	PULSES.select_from_to(21, 31);
	for(int pulse=22; pulse<=31; pulse++) {	// pulse[21] belongs to both groups
	  en_jiffle_thrower(pulse, selected_in(JIFFLES), ILLEGAL, DACsq2);	// FIXME: use inbuilt click
//	  PULSES.pulses[pulse].dest_action_flags |= (DACsq2);
//	  PULSES.set_payload(pulse, &do_throw_a_jiffle);
//	  PULSES.pulses[pulse].data = (unsigned int) jiffle;
	}
	// fix pulse[21] belonging to both groups
	PULSES.pulses[21].dest_action_flags |= DACsq2;
	selected_share_DACsq_intensity(255, 2);
	//	selected_DACsq_intensity_proportional(255, 2);

	PULSES.select_n(voices);	// select all primary voices

	// maybe start?
	if(MENU.cb_peek() == '!') {		// 'E38!' starts E38
	  MENU.drop_input_token();
	  PULSES.activate_selected_synced_now(sync);	// sync and activate;
	}
	break; // E38

      case 39:	// 'E39' time machine 2 setup
	// #define ESP32_15_clicks_no_display_TIME_MACHINE2
	{ // local scope only right now
	  short bass_pulses=14;
	  short middle_pulses=15;
	  short high_pulses=7;

	  voices = bass_pulses + middle_pulses + high_pulses;	// init *all* primary pulses
	  PULSES.select_n(voices);

	  PULSES.time_unit=1000000;	// default metric
	  multiplier=4096;		// uses 1/4096 jiffles
	  divisor = 294;		// 293.66 = D4	// default tuning D4
	  // divisor = 147;	// 146.83 = D3
	  // divisor=55;	// default tuning a

	  select_array_in(SCALES, minor_scale);		// default e minor
	  // tune *all* primary pulses
	  tune_2_scale(voices, multiplier, divisor, selected_in(SCALES));
	  lower_audio_if_too_high(409600*2);	// 2 bass octaves

	  // prepare primary pulse groups:
	  select_array_in(JIFFLES, d4096_512);		// default jiffle

	  // bass on DAC1 and planed broad angle LED lamps
	  // select_array_in(JIFFLES, d4096_512);
	  PULSES.select_from_to(0, bass_pulses - 1);
	  for(int pulse=0; pulse<bass_pulses; pulse++)
	    en_jiffle_thrower(pulse, selected_in(JIFFLES), ILLEGAL, DACsq1);	// TODO: FIXME: use inbuilt click
	  PULSES.select_from_to(0,bass_pulses);			// pulse[bass_pulses] belongs to both groups
	  // selected_DACsq_intensity_proportional(255, 1);
	  selected_share_DACsq_intensity(255, 1);		// bass DAC1 intensity

	  // 2 middle octaves on 15 gpios
	  // select_array_in(JIFFLES, d4096_512);
	  //	select_array_in(JIFFLES, d4096_256);
	  PULSES.select_from_to(bass_pulses, bass_pulses + middle_pulses -1);
	  setup_jiffle_thrower_selected(selected_actions=0);		// overwrites topmost bass pulse
									// TODO: 'selected_actions=...' or '|='
	  // fix topmost bass pulse pulse[bass_pulses] that belongs to both groups:
	  PULSES.pulses[bass_pulses].dest_action_flags |= DACsq1;

	  // high octave on DAC2
	  //	select_array_in(JIFFLES, d4096_64);
	  //select_array_in(JIFFLES, d4096_256);
	  PULSES.select_from_to(bass_pulses + middle_pulses -1, bass_pulses + middle_pulses + high_pulses -1);
	  for(int pulse = bass_pulses + middle_pulses; pulse<voices; pulse++) {	// pulse[21] belongs to both groups
	    en_jiffle_thrower(pulse, selected_in(JIFFLES), ILLEGAL, DACsq2);	// FIXME: use inbuilt click
	    //	  PULSES.pulses[pulse].dest_action_flags |= (DACsq2);
	    //	  PULSES.set_payload(pulse, &do_throw_a_jiffle);
	    //	  PULSES.pulses[pulse].data = (unsigned int) jiffle;
	  }
	  // fix pulse[21] belonging to both groups
	  PULSES.pulses[bass_pulses + middle_pulses - 1].dest_action_flags |= DACsq2;
	  selected_share_DACsq_intensity(255, 2);
	  //	selected_DACsq_intensity_proportional(255, 2);

	  PULSES.select_n(voices);	// select all primary voices

	  // maybe start?
	  if(MENU.cb_peek() == '!') {		// 'E39!' starts E39
	    MENU.drop_input_token();
	    PULSES.activate_selected_synced_now(sync);	// sync and activate, no display
	  } else
	    if (DO_or_maybe_display(VERBOSITY_LOWEST))	// maybe ok for here?
	      selected_or_flagged_pulses_info_lines();
	} // case E39 { }
	break;

      case 40:	// 'E40' time machine with icode player   *adapted to musicBox*
	// #define ESP32_15_clicks_no_display_TIME_MACHINE2
	reset_all_flagged_pulses_GPIO_OFF();
	{ // local scope 'E40' only right now	// TODO: factor out
	  PULSES.time_unit=1000000;	// default metric
	  multiplier=4096;		// uses 1/4096 jiffles
	  multiplier *= 8;	// TODO: adjust appropriate...
	  divisor = 294;		// 293.66 = D4	// default tuning D4
	  // divisor = 147;	// 146.83 = D3
	  // divisor=55;	// default tuning a
	  if(!scale_user_selected)	// see musicBox
	    select_array_in(SCALES, minor_scale);		// default e minor

	  select_scale__UI();	// second/third letters choose metric scales

	  if(!jiffle_user_selected)				// see musicBox
	    select_array_in(JIFFLES, d4096_512);		// default jiffle

	  if(!icode_user_selected) {	// see musicBox
#if defined USE_i2c
	    select_array_in(iCODEs, (unsigned int*) d4096_1024_i2cLED);
#else
	    select_array_in(iCODEs, (unsigned int*) d4096_1024_icode_jiff);
#endif
	  }

	  setup_bass_middle_high(bass_pulses, middle_pulses, high_pulses);

	  // maybe start?
	  if(MENU.cb_peek() == '!') {		// 'E40!' starts E40
	    MENU.drop_input_token();
	    PULSES.activate_selected_synced_now(sync);	// sync and activate, no display
	  } else
	    if (DO_or_maybe_display(VERBOSITY_LOWEST))	// maybe ok for here?
	      selected_or_flagged_pulses_info_lines();
	} // case 'E40' { }
	break;


      } // switch (selected_experiment)
    } // if experiment >= 0
    break;


  case '!':	// '!' sync and activate selected  (or: setup and start old style experiments)
    // see also 'n'
    // one of 'n' and '!' is obsolete...

    switch (selected_experiment) {	// setup some old style experiments:
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

    default:	// normal use case: sync and activate
      PULSES.activate_selected_synced_now(sync); // sync and activate

      if (DO_or_maybe_display(VERBOSITY_HIGH)) {
	MENU.ln();
	selected_or_flagged_pulses_info_lines();
      }
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
}  // menu_pulses_reaction()


/* **************************************************************** */



/* **************************************************************** */
/* **************************************************************** */

/* please see: README_pulses					    */

/*
   Copyright © Robert Epprecht  www.RobertEpprecht.ch   GPLv2
   http://github.com/reppr/pulses
*/

/* **************************************************************** */
/* **************************************************************** */
/*				END OF CODE			    */
/* **************************************************************** */
/* **************************************************************** */
