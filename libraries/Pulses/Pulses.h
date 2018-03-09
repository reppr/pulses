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

#include "examples/pulses/pulses_systems.h"
#include "examples/pulses/pulses_boards.h"

#include <Menu.h>

/* **************************************************************** */
/* Some basic #define's						    */

#define ILLEGAL			-1	// illegal value for many topics


/* **************************************************************** */
// Preprocessor macro logic:

/* Debugging macros:		*/
#ifdef DEBUGGING_ALL

#endif

/* **************************************************************** */
// time:

struct time {
  unsigned long time;
  unsigned int overflow;
};


/* **************************************************************** */
// pulse_t:

struct pulse_t {
  unsigned long counter;	// counts wake up's
  struct time period;		// period (including overflow count)
  struct time last;		// convenient, but not really needed
  struct time next;		// next time the pulse wants to wake up

#ifdef IMPLEMENT_TUNING		// implies floating point
  struct time other_time;	// used by tuning, maybe others to come
#endif

  uint8_t flags;
	// ACTIVE		switches pulse on/off
	// COUNTED		repeats 'int1[]' times, then vanishes
	// SCRATCH		edit (or similar) in progress
	// DO_NOT_DELETE	dummy to avoid being thrown out
	// TUNED		do not set directly, use activate_tuning(pulse)
	// CUSTOM_1		can be used by periodic_do()
	// CUSTOM_2		can be used by periodic_do()

  // internal parameter:
  unsigned int int1;		// if COUNTED, gives number of executions
  //				// else free for other *internal* use

  // custom parameters[pulse]	//  comment/uncomment as appropriate:
  // ============>>> adapt init_pulse() IF YOU CHANGE SOMETHING HERE <<<============
  // these parameters can be used by periodic_do(pulse):
  int parameter_1;
  /*
    used by do_jiffle for count down
  */

  int parameter_2;
  /*
    used by do_jiffle *jiffletab
  */

  unsigned long ulong_parameter_1;
  /*
    used by do_jiffle as base period
    used by tuned_click_0 as base period
  */

  char char_parameter_1;		// pin
  /*
    used by click	as pin
    used by tuned_click	as pin
    used by do_jiffle	as pin
  */

  char char_parameter_2;		// index	################ FIXME: unsigned int
  /*
    used by do_jiffle as jiffletab index
  */

  char parameter_3;				// ################ FIXME: pointer
  /*
    used by do_jiffle jiffle_ID		// index selecting jiffletab
  */

  char parameter_4;
  /*
    not used
  */


  // pointer on  void something(int pulse)  functions:
  // the pulses will do that, if the pointer is not NULL
  void (*periodic_do)(int);
  //					some example functions:
  //
  //					click(pulse)
  //					pulse_info_1line(pulse)
  //					pulse_info(pulse)
  //					do_jiffle(pulse)
  //					do_throw_a_jiffle(pulse)
  // ============>>> adapt init_pulse() IF YOU CHANGE SOMETHING HERE <<<============
};

// #define pulses.flags masks:
#define ACTIVE			1	// switches pulse on/off
#define COUNTED			2	// repeats 'int1[]' times, then vanishes
#define SCRATCH			8	// edit (or similar) in progress
#define DO_NOT_DELETE	       16	// dummy to avoid being thrown out
#define TUNED		       32	// do not set directly, use activate_tuning(pulse)
// #define CLICKs	       64	// inbuilt GPIO click
#define DACs		      128	// uses DAC output


/* **************************************************************** */
class Pulses {
 public:
  Pulses(int pl_max, Menu *MENU);
  ~Pulses();
  void init_time();
  struct time get_now();	// *always* use get_now() to get current time!
				// get_now() works directly on 'now'
				// also returns a pointer to 'now'

  void add_time(struct time *delta, struct time *sum);
  void sub_time(struct time *delta, struct time *sum);
  void mul_time(struct time *duration, unsigned int factor);
  void div_time(struct time *duration, unsigned int divisor);
  void multiply_period(int pulse, unsigned long factor);
  void divide_period(int pulse, unsigned long factor);
  void global_shift(int global_octave);
  int start_pulse(int pulse);
  void stop_pulse(int pulse);
  int global_octave;			    // global octave shift. ONLY negative shifts atm
  unsigned long global_octave_mask;	    // this mask gets shifted and then actually used
  unsigned long current_global_octave_mask; // actually used mask to switch oscillators
  struct time now;
  struct time last_now;		// for simple overflow detection
				// DO WE NEED OVERFLOW PART of that? ################
  struct time global_next;	// next time that a pulse wants to be waken up
  unsigned int global_next_count; // how many tasks wait to be activated at the same time?

#ifdef IMPLEMENT_TUNING		// implies floating point
  unsigned long ticks_per_octave;
  double tuning;
  void activate_tuning(int pulse);	// copy period to other_time and set TUNING flag
  void stop_tuning(int pulse);		// the pulse stays as it is, but no further (de)tuning
#endif

  void init_pulse(int pulse);		// init, reset or kill a pulse
  void init_pulses();			// init all pulses
  void wake_pulse(int pulse);		// wake a pulse up, called from check_maybe_do()
  void deactivate_pulse(int pulse);	// clear ACTIVE flag, keep data
  void deactivate_all_clicks();
  void activate_selected_synced_now(int sync, pulses_mask_t  selected_pulses);
  void reset_and_edit_pulse(int pulse, unsigned long time_unit);	// FIXME: time_unit as struct time
  int  fastest_pulse();			// fastest pulse, *not* dealing with overflow...

  void fix_global_next();	// determine next event, prepare everything
				// for *all* pulses that wait for this exact time
				// they will be called in fast sequence then
  bool check_maybe_do();	// FIXME; comment ################
  int setup_pulse(void (*pulse_do)(int), unsigned char new_flags, \
		  struct time when, struct time new_period);
  int setup_counted_pulse(void (*pulse_do)(int), unsigned char new_flags, \
			  struct time when, struct time new_period, unsigned int count);
  void set_new_period(int pulse, struct time new_period);

  void activate_pulse_synced(int pulse,			\
			    struct time when, int sync);	// ################
  int setup_pulse_synced(void (*pulse_do)(int), unsigned char new_flags,
			 struct time when, unsigned long unit,
			 unsigned long factor, unsigned long divisor, int sync);
  void init_click_pulses();

  unsigned long time_unit;

  // Menu output, info
  const float overflow_sec;				// overflow time in seconds
  void display_now();					// display current time
  void time_info();					// same with digital details and global next
  float display_realtime_sec(struct time duration);	// display duration in seconds
  void display_real_ovfl_and_sec(struct time then);	//
  void print_period_in_time_units(int pulse);

  void reset_and_edit_selected();
  void print_selected_mask();				// show mask of selected pulses
  void maybe_show_selected_mask();			// maybe show that
  void set_time_unit_and_inform(unsigned long new_value);

  unsigned int get_pl_max() { return pl_max; }	// inlined

  pulses_mask_t selected_pulses;		// a bit for each selected pulse
  pulse_t * pulses;		// data pointer for pulses
  Menu *MENU;			// pointer to the menu

 private:
  int pulse;			// pulse index	// DO WE NEED THAT? ################
  int pl_max;			// max pulses possible

  int * global_next_pulses;	// indices of all pulses involved @ coming 'next'
				//   pulses that want to be waken up at the same time.
				//   (there can't be more then pl_max)
};

/* **************************************************************** */
#endif
