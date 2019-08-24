/* **************************************************************** */
/*
   Pulses.h

   Arduino library to raise actions in harmonical time intervalls.

   Copyright © Robert Epprecht  www.RobertEpprecht.ch  GPLv2
   http://github.com/reppr/pulses

*/
/* **************************************************************** */


#ifndef PULSES_h
#define PULSES_h

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "../../pulses/pulses_project_conf.h"

#if ! defined ILLEGAL8
  #define ILLEGAL8	255
#endif
#if ! defined ILLEGAL32
  #define ILLEGAL32	0xffffffff
#endif


typedef int icode_t;

//#include <limits.h>
enum icode {	// names are all four letter words ?	// maybe 8?
  KILL=INT_MIN,
  i2cW,		// i2c write, based on pulses[pulse].i2c_addr and pulses[pulse].i2c_pin
  doA2,		// function A, taking 2 int parameters
  INFO,
  WAIT,
  DONE,
  PRES,		// 1 parameter: int preset
  MACRO_NOW,	// 1 parameter: char * macro
  N_HI,		// (bare)  HI are you there?	for connection and ping pong
  N_HO,		// (bare)  HERE I AM
  N_WHO,	// 	//(bare)  WHO is here?
  N_ID,		// peer_ID_t my_IDENTITY	share personal data
  N_ST,		// (bare)   receiver sets sync to its individual time slice IDENTITY.esp_now_time_slice;
};

#ifndef pulse_flags_t
  #define pulse_flags_t		uint16_t
#endif

#ifndef action_flags_t
  #define action_flags_t	uint8_t
#endif

#ifndef group_flags_t
  #define group_flags_t		uint16_t
#endif

#ifndef gpio_pin_t
  #define gpio_pin_t	int8_t			// negative values might be used for pin extensions
//  #define gpio_pin_t		short		// negative values might be used for pin extensions
#endif


#include "../../pulses/pulses_systems.h"
#include "../../pulses/pulses_boards.h"

#include <Menu.h>


/* **************************************************************** */
/* Some basic #define's						    */

#define pulses_mask_t  uint32_t		// ???	uint64_t compiles, but does not work


/* **************************************************************** */
// Preprocessor macro logic:

/* Debugging macros:		*/


/* **************************************************************** */
// time:

// TODO: time (and related stuff should move to Harmonics::
typedef struct {
  unsigned long time;
  unsigned int overflow;
} pulse_time_t;


/* **************************************************************** */
// pulse_t:

struct pulse_t {
  unsigned long counter;	// counts wake up's
  pulse_time_t period;		// period (including overflow count)
  pulse_time_t last;		// convenient, but not really needed...	maybe yes :)
  pulse_time_t next;		// next time the pulse wants to wake up

#ifdef IMPLEMENT_TUNING		// implies floating point
  pulse_time_t other_time;	// used by tuning,	TODO: CONFLICTS with musicBox_butler(p)
				// used by musicBox_butler(p) as soft_end_time	TODO: CONFLICTS with tuning
#endif


  pulse_flags_t flags;

// #define pulses[pulse].flags masks:
#define ACTIVE			1	// switches pulse on/off
#define COUNTED			2	// repeats 'pulses[p].remaining' times, then vanishes
#define HAS_GPIO		4	// has an associated GPIO pin	use set_gpio(pulse, pin)
#define SCRATCH			8	// edit (or similar) in progress
#define DO_NOT_DELETE	       16	// dummy to avoid being thrown out
#define TUNED		       32	// do not set directly, use activate_tuning(pulse)
#define HAS_ICODE	       64	// do not set directly, use set_icode_p(int pulse, icode_t* icode_p)
#define HAS_I2C_ADDR_PIN      128	// do not set directly, use set_i2c_addr_pin(uint8_t adr, uint8_t pin)
#define HAS_RGB_LEDs	      256	// has RGB LED string, set_rgb_led_string(pulse, string_idx, pixel)

  //#define INVERSE_LOGIC	      128	// TODO: implement


  action_flags_t action_flags;

// #define pulses.action_flags masks:
// for easy menu interfacing:	1,2 DACsq

//	>>>>>>>	change all flag display code if something changes here <<<<<<<
#define DACsq1			1	// DAC1 output value as square wave, harmonical timing
#define DACsq2			2	// DAC2 output value as square wave, harmonical timing
#define CLICKs			4	// GPIO 'click' inbuilt GPIO toggling	see: set_gpio(pulse, pin)
#define PAYLOAD			8	// do payload(pulse)
#define doesICODE		16	// plays icode

#define DO_first		32	// do before counting and any other actions
					//   use set_do_first(...) to set and activate

#define DO_after		64	// do after other actions	// TODO: UNUSED:

#define noACTION		128	// 'mutes' all actions

#define ACTION_MASK_BITS	8	// >>>>>>>> *DO CHANGE* number of flags changes here <<<<<<<
					// ACTION_MASK_BITS is used by mask displaying code (only)

  group_flags_t groups;			// flags like primary pulse, pentatonic, bass, octave and the like

// role groups:
#define g_MASTER	1	// master pulse that generates and controls other pulses that perform a piece
#define g_PRIMARY	2	// a pulse that seeds 'throws' other pulses like do_throw_a_jiffle(p)
#define g_SECONDARY	4	// a one shot player like do_jiffle(p);
#define g_AUXILIARY	8	// like a pulse used to test a jiffle		//  FIXME:  CRASH g_AUXILIARY <--> g_TRIAD

// tuning groups:
//#define g_TRIAD		8	// (any) 3 note subset of a scale	//  FIXME:  CRASH g_AUXILIARY <--> g_TRIAD
//#define g_TETRA		16	// 4 note subset of pulses tuned to a tetrachord (or similar)
//#define g_PENTA		32	// pentatonic 5 note subset of tuned pulses
//#define g_HEXA		64	// (any) 6 tone subset
//#define g_HEPTA		128	// 7 tone heptatonic (sub)set

#define g_OCTAVE	64	// tuned to (any) octave of the root note
#define g_FOURTH	128	// tuned to (any) fourth
#define g_FIFTH		256	// tuned to (any) fifth

// voice groups:
#define g_LOW_END	512	// deep bass & rhytm & structure & below ;)
#define g_MIDDLE	1024	// bass & melody	TODO: split in 2 g_BASS and g_MELODY
#define g_HIGH_END	2048	// high up in the skies


  void (*do_first)(int);	// see: action flag DO_first

  // internal parameter:
  unsigned int remaining;		// if COUNTED, gives number of remaining executions

  // custom parameters[pulse]	//  comment/uncomment as appropriate:
  // ============>>> adapt init_pulse() IF YOU CHANGE SOMETHING HERE <<<============
  // these parameters can be used by payload(pulse):
  int countdown;
  /*
    used by do_jiffle for count down
  */

  int data;
  /*
    used by do_jiffle *jiffletab
  */

  int index;		// index
  /*
    used by do_jiffle as jiffletab index
  */

  int* icode_p;
  /*
    used by icode_player *icode_array
  */

  int icode_index;
  /*
    used by icode_player as array index
  */

  int other_pulse;
  /*
    used by seed_icode_player()

    in a seeder pulse, the other_pulse is the child,
    in the child, it is the parent
  */

#if defined USE_i2c
  #if defined USE_MCP23017
    uint8_t i2c_addr;
    uint8_t i2c_pin;
    /*
      see: HAS_I2C_ADDR_PIN, set_i2c_addr_pin(uint8_t adr, uint8_t pin)
      used by icode_player i2cW
      and custom functions
    */
  #endif
#endif

  unsigned long base_time;
  /*
    used by do_jiffle as base period
    used by tuned_click_0 as base period
  */

  gpio_pin_t gpio;		// pin	use set_gpio(pulse, pin)
  /*
    used by CLICKs	as pin
    used by click	as pin
    used by tuned_click	as pin
    used by do_jiffle	as pin
  */

  action_flags_t dest_action_flags;
  /*
    used by:
    pulse_info_1line(int pulse)
    en_jiffle_thrower(int pulse, unsigned int *jiffletab, action_flags_t action_mask)
    init_jiffle(unsigned int *jiffletab, pulse_time_t when, pulse_time_t new_period, int origin_pulse)
  */

  // pointer on  void something(int pulse)  functions:
  // the pulses will do that, if the pointer is not NULL
  void (*payload)(int);
  //					some example functions:
  //
  //					click(pulse)
  //					pulse_info_1line(pulse)
  //					pulse_info(pulse)
  //					do_jiffle(pulse)
  //					do_throw_a_jiffle(pulse)
  //					musicBox_butler(pulse)
  // ============>>> adapt init_pulse() IF YOU CHANGE SOMETHING HERE <<<============


#if defined USE_DACs
  //  int (*dac1_wave_function)(int pulse, int volume);
  int dac1_intensity;
  #if (USE_DACs > 1)	// only 1 or 2 DACs supported
    //  int (*dac2_wave_function)(int pulse, int volume);
    int dac2_intensity;
  #endif
#endif

  uint8_t rgb_string_idx;	// use  set_rgb_led_string(), remove_rgb_led_string()
  uint8_t rgb_pixel_idx;
};

/* **************************************************************** */
class Pulses {
 public:
  Pulses(int pl_max, Menu *MENU);
  ~Pulses();
  void init_time();
  pulse_time_t get_now();	// *always* use get_now() to get current time!
				// get_now() works directly on 'now'
				// also returns a pointer to 'now'

  void add_time(pulse_time_t *delta, pulse_time_t *sum);
  void sub_time(pulse_time_t *delta, pulse_time_t *sum);
  void add_time(unsigned long time, pulse_time_t *sum);	// only unsigned long
  void sub_time(unsigned long time, pulse_time_t *sum);	// only unsigned long
  void mul_time(pulse_time_t *duration, unsigned int factor);
  void div_time(pulse_time_t *duration, unsigned int divisor);
  void multiply_period(int pulse, unsigned long factor);
  void divide_period(int pulse, unsigned long factor);
  void global_shift(int global_octave);
  int global_octave;			    // global octave shift. ONLY negative shifts atm
  unsigned long global_octave_mask;	    // this mask gets shifted and then actually used
  unsigned long current_global_octave_mask; // actually used mask to switch oscillators
  pulse_time_t now;
  pulse_time_t last_now;		// for simple overflow detection
				// DO WE NEED OVERFLOW PART of that? ################
  pulse_time_t global_next;	// next time that a pulse wants to be waken up
  unsigned int global_next_count; // how many tasks wait to be activated at the same time?


#if defined USE_DACs
  void DAC_output();		// calculate and output on DAC

// TODO: use or remove code later
//	 #if (USE_DACs == 1)
//	  int en_DAC(int pulse, int DACs_count /* must be 1 or 2 */,
//		     int (*dac1_wave_function)(int pulse), int volume);
//	 #elif (USE_DACs == 2)
//	  int en_DAC(int pulse, int DACs_count /* must be 1 or 2 */,
//		     int (*dac1_wave_function)(int pulse, int volume0),
//		     int (*dac2_wave_function)(int pulse, int volume1));
//	 #else
//	   #error NO DACs FIXME: REMOVE DEBUG CODE ################
//	 #endif	// USE_DAC ok
//
//	// Functions for  adcX_wave_function(int pulse, int volume )
//	  int function_square_wave(int pulse, int volume);	// simple square
//	  int function_wave_OFF(int pulse, int volume);		// ZERO
#endif


#ifdef IMPLEMENT_TUNING		// implies floating point
  unsigned long ticks_per_octave;
  double tuning;
  void activate_tuning(int pulse);	// copy period to other_time and set TUNING flag
  void stop_tuning(int pulse);		// the pulse stays as it is, but no further (de)tuning
#endif

  void init_pulse(int pulse);		// init, reset or kill a pulse
  void init_pulses();			// init all pulses
  int highest_available_pulse();	// first free pulse from top (or ILLEGAL32)
  void wake_pulse(int pulse);		// wake a pulse up, called from check_maybe_do()
  void deactivate_pulse(int pulse);	// clear ACTIVE flag, keep data
  void set_do_first(int pulse, void (*do_first)(int));	// set and activate do_first
  void set_payload(int pulse, void (*payload)(int));	// set and activate payload
  void set_payload_with_pin(int pulse, void (*payload)(int), gpio_pin_t pin);	// set and activate payload with gpio
  void set_gpio(int pulse, gpio_pin_t pin);		// set gpio
  void set_icode_p(int pulse, icode_t* icode_p, bool activate);			// set icode. maybe activate

  void set_rgb_led_string(int pulse, uint8_t string_idx, uint8_t pixel);
  void remove_rgb_led_string(int pulse);


#if defined USE_i2c
  #if defined USE_MCP23017
    void set_i2c_addr_pin(int pulse, uint8_t i2c_addr, uint8_t i2c_pin);
  #endif
#endif

  void play_icode(int pulse);			// payload to play icode
  void show_icode_mnemonic(icode_t icode);	// display icode mnemonic

//  DOES NOT WORK: non static member function	TODO: fix or remove ################
//  void seed_icode_player(int pulse);		// used as payload to seed play_icode() pulses
//  bool setup_icode_seeder(int pulse, pulse_time_t period, icode_t* icode_p, action_flags_t dest_action_flags);

  void show_pulse_flag_mnemonics(pulse_flags_t flags);	// show pulse flags as mnemonics
  void show_action_flags(action_flags_t flags);	// show action flags as mnemonics
  void selected_toggle_no_actions();		// selected toggle action flag noACTION		// TODO: do I want this?
  void selected_set_no_actions();		// selected set    action flag noACTION		// TODO: do I want this?
  void selected_clear_no_actions();		// selected clear action flag noACTION		// TODO: do I want this?

  bool select_pulse(int pulse);			// select in user interface
  void deselect_pulse(int pulse);
  void toggle_selection(int pulse);
  void clear_selection(void);
  int select_n(unsigned int n);
  int select_from_to(unsigned int from, unsigned int to);
  unsigned int deselect_zombie_primaries();	// deselect zombie primary pulses
  bool pulse_is_selected(int pulse, pulses_mask_t * mask);
  bool pulse_is_selected(int pulse);
  short how_many_selected(void);

  void activate_selected_synced_now(int sync);
  void activate_selected_stack_sync_now(pulse_time_t tick, int sync);
  void time_skip_selected(pulse_time_t time_shift);
  void reset_and_edit_pulse(int pulse, unsigned long time_unit);	// FIXME: time_unit as time_t
  int fastest_pulse();			// fastest pulse, *not* dealing with overflow...
  int fastest_from_selected();		// fastest from selected, no overflow...
  int add_selected_to_group(group_flags_t groups);	// add selected pulses to groups
  int remove_selected_from_group(group_flags_t groups);	// remove selected pulses from groups
  void show_group_mnemonics(int pulse);		// show group mnemonics
  void fix_global_next();	// determine next event, prepare everything
				// for *all* pulses that wait for this exact time
				// they will be called in fast sequence then
  bool check_maybe_do();	// FIXME; comment ################
  int setup_pulse(void (*pulse_do)(int), pulse_flags_t new_flags, \
		  pulse_time_t when, pulse_time_t new_period);
  int setup_pulse(void (*pulse_do)(int), pulse_flags_t new_flags, \
		  pulse_time_t when, unsigned long new_period);	// unsigned long *only* version
  int setup_counted_pulse(void (*pulse_do)(int), pulse_flags_t new_flags, \
			  pulse_time_t when, pulse_time_t new_period, unsigned int count);
  void set_new_period(int pulse, pulse_time_t new_period);

  void activate_pulse_synced(int pulse,			\
			    pulse_time_t when, int sync);	// ################

  int setup_pulse_synced(void (*pulse_do)(int), pulse_flags_t new_flags,
			 pulse_time_t when, unsigned long unit,
			 unsigned long factor, unsigned long divisor, int sync);

  unsigned long time_unit;

  // Menu output, info
  const float overflow_sec;				// overflow time in seconds
  void display_now();					// display current time
  void time_info();					// same with digital details and global next
  float display_realtime_sec(pulse_time_t duration);	// display duration in seconds
  void display_time_human(pulse_time_t duration);	// everyday time format	d h m s  short
  void display_time_human_format(pulse_time_t duration);	// everyday time format	d h m s  formatted with spaces
  void display_real_ovfl_and_sec(pulse_time_t then);	//
  void short_info(void);				// give some infos, depending verbosity
  //  void selected_or_flagged_pulses_info_lines(void);
  //  void pulse_info_1line(int pulse);			// 1 line pulse info, depending verbosity
  void print_period_in_time_units(int pulse);

  void reset_and_edit_selected();
  void show_selected_mask();				// show mask of selected pulses
  void maybe_show_selected_mask();			// maybe show that
  void show_time_unit();
  void set_time_unit_and_inform(unsigned long new_value);

  unsigned int get_pl_max() { return pl_max; }	// inlined

  pulse_t * pulses;		// data pointer for pulses
  Menu *MENU;			// pointer to the menu
  short hex_input_mask_index;
  short selection_masks(void);	// how many selection masks

  // pointer on  void something(int, int) functions:
  // used by iCODE doA2, if not NULL
  void (*do_A2)(int, int);

 private:
  int pulse;			// pulse index	// DO WE NEED THAT? ################
  int pl_max;			// max pulses possible

  int * global_next_pulses;	// indices of all pulses involved @ coming 'next'
				//   pulses that want to be waken up at the same time.
				//   (there can't be more then pl_max)

  pulses_mask_t * selected_pulses_p;  // bitmask pointer, a bit for each selected pulse
};

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
#endif
