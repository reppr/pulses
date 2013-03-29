/* **************************************************************** */
// pulses
/* **************************************************************** */
/*

       Copyright © Robert Epprecht  www.RobertEpprecht.ch   GPLv2
 
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
/*
   Periodically do multiple independent tasks named 'pulses'.

   New logic version to determine when to wake up pulses.
   Saving times along with an overflow count makes everything
   simple and flexible.

   struct time stores time and overflow.

   This version calculates global next event[s] *once* when the next
   event could possibly have changed. See fix_global_next().

   If a series of pulses are sceduled for the same time they will
   be called in fast sequence *without* fix_global_next() in between them.
   After all pulse with the same time are done fix_global_next() is called.

   If a pulse sets up another pulse with the same next it *can* decide to do
   the fix itself, if it seems appropriate. Normally this is not required.

   "play" version: having fun while debugging...
     clicks
     rhythms
     jiffles

   Selecting, creating, editing, syncing, killing pulses from serial menu.

   Entering jiffletab by hand.


*/
/* **************************************************************** */
#define ILLEGAL		-1



/* **************************************************************** */
// CONFIGURATION:

#define PULSES		16	// maximal number of pulses
				// plenty of free pulses for jiffles ;)

// having fun while debugging: clicks on piezzos to *hear* the result:
// *do* change 'case' statement in menu_serial_reaction() if you change this.
#ifndef CLICK_PULSES		// number of click frequencies
  #define CLICK_PULSES	 5 	// default number of click frequencies
#endif



// #define USE_SERIAL_BAUD
// to switch on serial #define USE_SERIAL_BAUD <baud>  (otherwise serial will be *off*)
//
// You can switch all serial line and menu code *off* by not defining USE_SERIAL_BAUD
// You can also do this later to save program memory.
//   your sketch could include the menu, but later grow too much,
//   or you'd want to move to another processor with less program memory.
//
// #define USE_SERIAL_BAUD	115200		// works fine here
#define USE_SERIAL_BAUD	57600
// #define USE_SERIAL_BAUD	38400

#ifdef USE_SERIAL_BAUD	// activate minimalistic menus over serial line?
  // menu over serial, basics:
  #define MENU_over_serial	// we *do* use serial menu

  #define PROGRAM_menu		// and a program specific menu

  // simple menu to access arduino hardware:
  #define HARDWARE_menu		// menu interface to hardware configu[Bration
  	  			// this will let you read digital and analog inputs
  				// watch changes on inputs as value or as bar graphs
  	  			// set digital and analog outputs, etc.

  #ifdef HARDWARE_menu	// user reported problems with older arduino versions, fix.
    // arduino versions
 
    // needed for pre 1.0 versions to be compatible with HARDWARE_menu:
    #if (ARDUINO < 100)
      #include <pins_arduino.h>
    
      // triggers another bug.
      // see this thread:
      // http://code.google.com/p/arduino/issues/detail?id=604&start=200
    
      // short version (adapt to your arduino version)
      // edit file arduino-0022/hardware/arduino/cores/arduino/wiring.h
      // comment out line 79 (round macro)
      // #define round(x)     ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))
    
      // tested on arduino-0023
    #endif	// older arduino versions
  
    char hw_PIN = ILLEGAL;
  #endif // HARDWARE_menu
#endif	// USE_SERIAL_BAUD



/* **************************************************************** */
// board specific things:

#if defined(NUM_ANALOG_INPUTS) && defined(NUM_DIGITAL_PINS)	// use arduino macros
  #define ANALOG_INPUTs	NUM_ANALOG_INPUTS
  #define DIGITAL_PINs	(NUM_DIGITAL_PINS - NUM_ANALOG_INPUTS)
#else	// savety net
  #if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)   // mega boards
    #define ANALOG_INPUTs	16
    #define DIGITAL_PINs	54
  #else								   // 168/328 boards
    #define ANALOG_INPUTs	6
    #define DIGITAL_PINs	14
  #endif
#endif

#if defined(LED_BUILTIN)
  #define LED_PIN	LED_BUILTIN
#else
  #define LED_PIN	13
#endif



/* **************************************************************** */
/*
	RAM usage, PROGMEM to save RAM.
*/
/* **************************************************************** */

// determine RAM usage:
extern int __bss_end;
extern void *__brkval;

int get_free_RAM() {
  int free;

  if ((int) __brkval == 0)
    return ((int) &free) - ((int) &__bss_end);
  else
    return ((int) &free) - ((int) __brkval);
}


/* **************************************************************** */
// use PROGMEM to save RAM:

// menu strings use PROGMEM to save RAM:
#if (defined(USE_SERIAL_BAUD) || defined(USE_LCD))	// Serial and LCD strings...

#include <avr/pgmspace.h>

// to save RAM constant strings are stored in PROGMEM
const unsigned char programName[] PROGMEM = "PULSES";
const unsigned char programLongName[] PROGMEM = \
  "*** Play with PULSES v0.2 ***";
const unsigned char version[] PROGMEM = "version 0.2";

const unsigned char tab_[] PROGMEM = "\t";
void tab() {
  serial_print_progmem(tab_);
}

const unsigned char space_[] PROGMEM = " ";
void spaces(int count) {
  for (; count>0; count--)
    serial_print_progmem(space_);
}

const unsigned char slash_[] PROGMEM = "/";
void slash() {
  serial_print_progmem(slash_);
}


//	#ifdef USE_LCD
//	// LCD.print() for progmem strings:
//	// void LCD_print_progmem(const prog_uchar *str)   // does not work :(
//	void LCD_print_progmem(const unsigned char *str) {
//	  unsigned char c;
//	  while((c = pgm_read_byte(str++)))
//	    LCD.print(c);
//	}
//	
//	void LCD_print_at_progmem(unsigned char col, unsigned char row, const unsigned char *str) {
//	  LCD.setCursor(col, row);
//	  LCD_print_progmem(str);
//	}
//	
//	// clear line and print on LCD at line 'row'
//	const unsigned char LCD_empty[] PROGMEM = "                    ";
//	void LCD_print_line_progmem(unsigned char row, const unsigned char *str) {
//	  LCD_print_at_progmem(0, row, LCD_empty);
//	  LCD_print_at_progmem(0, row, str);
//	}
//	
//	#endif


#ifdef USE_SERIAL_BAUD	// inside #if (defined(USE_SERIAL_BAUD) || defined(USE_LCD))
  // Serial.print() for progmem strings:
  // void serial_print_progmem(const prog_uchar *str)	// does not work :(
  void serial_print_progmem(const unsigned char *str) {
    unsigned char c;
    while((c = pgm_read_byte(str++)))
      Serial.write(c);
  }


  void serial_println_progmem(const unsigned char *str) {
    serial_print_progmem(str);
    Serial.println();
  }
#endif // USE_SERIAL_BAUD


#endif	// #if (defined(USE_SERIAL_BAUD) || defined(USE_LCD))


// RAM_info()
#ifdef USE_SERIAL_BAUD
  const unsigned char freeRAM[] PROGMEM = "free RAM: ";
  const unsigned char bytes_[] PROGMEM = " bytes";

  void RAM_info() {
    serial_print_progmem(freeRAM);
    Serial.print(get_free_RAM());
    serial_println_progmem(bytes_);
    Serial.println();
  }
#endif


/* **************************************************************** */
/*
     TIME


   all times get stored in 'struct time', taking care of time overflow
   *always* get time by calling get_now() to have overflow treated correctly.

   run init_time() from setup to initialize time internals.

   'running-through' program design:
   never wait,
   check with check_maybe_do() if it's time to do something
   else return immediately.

   pulses know when their next turn will be, see 'struct time next[pulse]'.
   fix_global_next() determines which event will be next globally
   fix_global_next() is updated automatically when appropriate.

*/
/* **************************************************************** */


// global variables, time.

struct time {
  unsigned long time;
  unsigned int overflow;
};

struct time now, last_now;

struct time global_next;
unsigned int global_next_count=0;
int global_next_pulses[PULSES];	// there can't be more then PULSES same next events

int sync=1;			// syncing edges or middles of square pulses

const float overflow_sec = 4294.9672851562600;	// overflow time in seconds

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



// init time:

// do this once from setup()
void init_time()
{
  extern volatile unsigned long timer0_overflow_count;

  cli();
  timer0_overflow_count = 0;
  sei();

  last_now.time = 0;		// make sure get_now() sees no overflow
  get_now();
  now.overflow = 0;		// start with now.overflow = 0

  last_now = now;		// correct overflow

  global_next.time=0;
  global_next.overflow=~0;	// ILLEGAL
}


// *always* get time through get_now()
void get_now() {		// get time, set now.time and now.overflow
  now.time = micros();

  if (now.time < last_now.time)	// manage now.overflows
    now.overflow++;

  last_now = now;		// manage last_now
}


// add_time(), sub_time(), mul_time(), div_time():

void add_time(struct time *delta, struct time *sum)
{
  unsigned long last=(*sum).time;

  (*sum).time += (*delta).time;
  (*sum).overflow += (*delta).overflow;
  if (last > (*sum).time)
    (*sum).overflow++;
}


// As time is unsigned we need a separate sub_time()
// to have access to the full unsigned value range.
void sub_time(struct time *delta, struct time *sum)
{
  unsigned long last=(*sum).time;

  (*sum).time -= (*delta).time;
  (*sum).overflow -= (*delta).overflow;
  if (last < (*sum).time)
    (*sum).overflow--;
}


void mul_time(struct time *duration, unsigned int factor)
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


void div_time(struct time *duration, unsigned int divisor) {
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



/* **************************************************************** */
/*

     PULSES


   periodically wake up, count and do something

*/
/* **************************************************************** */

// variables for pulses in arrays[pulse]:
int pulse;		// index

// ============>>> adapt init_pulse() IF YOU CHANGE SOMETHING HERE <<<============
unsigned char flags[PULSES];
// flag masks:
#define ACTIVE			1	// switches pulse on/off
#define COUNTED			2	// repeats 'int1[]' times, then vanishes
#define SCRATCH			8	// edit (or similar) in progress
#define DO_NOT_DELETE	       16	// dummy to avoid being thrown out
#define CUSTOM_1	       32	// can be used by periodic_do()
#define CUSTOM_2	       64	// can be used by periodic_do()
#define CUSTOM_3	      128	// can be used by periodic_do()


unsigned int pulse_count[PULSES];	// counts how many times the pulse woke up
struct time period[PULSES];		// timer steps, overflows
struct time last[PULSES];		// convenient, but not really needed
struct time next[PULSES];		// next wake up time, overflow


// internal parameter:
unsigned int int1[PULSES];		// if COUNTED, gives number of executions
//					   else free for other internal use

// custom parameters[pulse]		//  comment/uncomment as appropriate:
					//  then *DO ADAPT init_pulse()* 

// ============>>> adapt init_pulse() IF YOU CHANGE SOMETHING HERE <<<============
// these parameters can be used by periodic_do(pulse):
int parameter_1[PULSES];
/*
	used by do_jiffle for count down
*/

int parameter_2[PULSES];
/*
	used by do_jiffle *jiffletab
*/

// int parameter_3[PULSES];
// int parameter_4[PULSES];

unsigned long ulong_parameter_1[PULSES];
/*
	used by do_jiffle as base period
*/

// unsigned long ulong_parameter_2[PULSES];
// unsigned long ulong_parameter_3[PULSES];
// unsigned long ulong_parameter_4[PULSES];

char char_parameter_1[PULSES];		// pin
/*
	used by click      as pin
	used by do_jiffle  as pin
*/

char char_parameter_2[PULSES];		// index
/*
	used by do_jiffle as jiffletab index
*/
// char char_parameter_3[PULSES];
// char char_parameter_4[PULSES];


// pointers on  void something(int pulse)  functions:
// the pulses will do that, if the pointer is not NULL
void (*periodic_do[PULSES])(int);
//					some example functions:
//
//					click(pulse)
//					pulse_info_1line(pulse)
//					pulse_info(pulse)
//					do_jiffle(pulse)
//					do_throw_a_jiffle(pulse)


// ============>>> adapt init_pulse() IF YOU CHANGE SOMETHING HERE <<<============



/* **************************************************************** */
// init pulses:

// init, reset or kill a pulse: 
void init_pulse(int pulse) {
  flags[pulse] = 0;
  periodic_do[pulse] = NULL;
  pulse_count[pulse] = 0;	//  or pulse_count[pulse] = ILLEGAL; ???
  int1[pulse] = 0;
  period[pulse].time = 0;
  last[pulse].time = 0;
  last[pulse].overflow = 0;
  next[pulse].time = 0;
  next[pulse].overflow = 0;
  parameter_1[pulse] = 0;
  parameter_2[pulse] = 0;
  // parameter_3[pulse] = 0;
  // parameter_4[pulse] = 0;

  ulong_parameter_1[pulse] = 0L;
  // ulong_parameter_2[pulse] = 0L;
  // ulong_parameter_3[pulse] = 0L;
  // ulong_parameter_4[pulse] = 0L;
  char_parameter_1[pulse] = 0;
  char_parameter_2[pulse] = 0;
  // char_parameter_3[pulse] = 0;
  // char_parameter_4[pulse] = 0;

  // you *must* call fix_global_next(); late in setup()
}


// do this once from setup():
void init_pulses() {
  for (int pulse=0; pulse<PULSES; pulse++) {
    init_pulse(pulse);
  }
}


// void wake_pulse(int pulse);	do one life step of the pulse
// gets called from check_maybe_do()
void wake_pulse(int pulse) {
  pulse_count[pulse]++;				//      count

  if (periodic_do[pulse] != NULL) {		// there *is* something else to do?
    (*periodic_do[pulse])(pulse);			//   yes: do it
  }

  // prepare future:
  last[pulse].time = next[pulse].time;		// when it *should* have happened
  last[pulse].overflow = next[pulse].overflow;
  next[pulse].time += period[pulse].time;		// when it should happen again
  next[pulse].overflow += period[pulse].overflow;

  if (last[pulse].time > next[pulse].time)
    next[pulse].overflow++;

  //						// COUNTED pulse && end reached?
  if ((flags[pulse] & COUNTED) && ((pulse_count[pulse] +1) == int1[pulse] ))
    if (flags[pulse] & DO_NOT_DELETE)		//   yes: DO_NOT_DELETE?
      flags[pulse] &= ~ACTIVE;			//     yes: just deactivate
    else
      init_pulse(pulse);				//     no:  DELETE pulse

}



// void fix_global_next();
// determine when the next event[s] will happen:
//
// to start properly you *must* call this once, late in setup()
// will automagically get updated later on
void fix_global_next() {
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

  for (pulse=0; pulse<PULSES; pulse++) {		// check all pulses
    if (flags[pulse] & ACTIVE) {				// pulse active?
      if (next[pulse].overflow < global_next.overflow) {	// yes: earlier overflow?
	global_next.time = next[pulse].time;		//   yes: reset search
	global_next.overflow = next[pulse].overflow;
	global_next_pulses[0] = pulse;
	global_next_count = 1;
      } else {					// same (or later) overflow:
	if (next[pulse].overflow == global_next.overflow) {	// same overflow?
	  if (next[pulse].time < global_next.time) {		//   yes: new next?
	    global_next.time = next[pulse].time;		//     yes: reset search
	    global_next_pulses[0] = pulse;
	    global_next_count = 1;
	  } else					// (still *same* overflow)
	    if (next[pulse].time == global_next.time)		//    *same* next?
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
void check_maybe_do() {
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
    const unsigned char noFreePulses[] PROGMEM = "no free pulses";
#endif

int setup_pulse(void (*pulse_do)(int), unsigned char new_flags, struct time when, struct time new_period) {
  int pulse;

  if (new_flags == 0)				// illegal new_flags parameter
    return ILLEGAL;				//   should not happen

  for (pulse=0; pulse<PULSES; pulse++) {	// search first free pulse
    if (flags[pulse] == 0)			// flags==0 means empty pulse
      break;					//   found one
  }
  if (pulse == PULSES) {			// no pulse free :(

#ifdef USE_SERIAL_BAUD
    serial_println_progmem(noFreePulses);
#endif

    return ILLEGAL;			// ERROR
  }

  // initiaize new pulse				// yes, found a free pulse
  flags[pulse] = new_flags;			// initialize pulse
  periodic_do[pulse] = pulse_do;			// payload
  next[pulse].time = when.time;			// next wake up time
  next[pulse].overflow = when.overflow;
  period[pulse].time = new_period.time;
  period[pulse].overflow = new_period.overflow;

  // fix_global_next();	// this version does *not* automatically call that here...

  return pulse;
}


// unused?
int setup_counted_pulse(void (*pulse_do)(int), unsigned char new_flags, struct time when, struct time new_period, unsigned int count) {
  int pulse;

  pulse= setup_pulse(pulse_do, new_flags|COUNTED, when, new_period);
  int1[pulse]= count;

  return pulse;
}


void set_new_period(int pulse, struct time new_period) {
  period[pulse].time = new_period.time;
  period[pulse].overflow = new_period.overflow;

  next[pulse].time = last[pulse].time + period[pulse].time;
  next[pulse].overflow = last[pulse].overflow + period[pulse].overflow;
  if(next[pulse].time < last[pulse].time)
    next[pulse].overflow++;

  fix_global_next();	// it's saver to do that from here, but could be omitted.
}


/* **************************************************************** */
// piezzo clicks on arduino i/o pins
// great help when debugging and a lot of fun to play with :)

// FlipFlop pins:

#if ( CLICK_PULSES > 0)

// clicks on piezzos to *hear* the result:
//   or connect LEDs, MOSFETs, MIDI, whatever...
//   these are just FlipFlop pins.

// Click_pulses are a sub-group of pulses that control an arduino
// digital output each.  By design they must be initiated first to get
// the low pulse indices. The pins are configured as outputs by init_click_pins()
// and get used by clicks, jiffles and the like. 

// It's best to always leave click_pulses in memory.
// You can set DO_NOT_DELETE to achieve this.


//  configured further up:
//  // *do* change 'case' statement in menu_serial_reaction() if you change this.
//  #ifndef CLICK_PULSES		// number of click frequencies
//    #define CLICK_PULSES	5       // default number of click frequencies
//  #endif
//  // *do* change 'case' statement in menu_serial_reaction() if you change this.



// By design click pulses *HAVE* to be defined *BEFORE* any other pulses:
void init_click_pulses() {
  for (int pulse=0; pulse<CLICK_PULSES; pulse++) {
    init_pulse(pulse);
    // flags[pulse] |= DO_NOT_DELETE;
  }
}



void click(int pulse) {			// can be called from a pulse
  digitalWrite(char_parameter_1[pulse], pulse_count[pulse] & 1);
}


// pins for click_pulses:
// It is a bit obscure to held them in an array indexed by [pulse]
// but it's simple and working well...
unsigned char click_pin[CLICK_PULSES];

void init_click_pins() {
  for (int pulse=0; pulse<CLICK_PULSES; pulse++) {
    pinMode(click_pin[pulse], OUTPUT);
    digitalWrite(click_pin[pulse], LOW);
  }
}


//  // unused? (I use the synced version more often)
//  int setup_click_pulse(void (*pulse_do)(int), unsigned char new_flags,
//  		     struct time when, struct time new_period) {
//    int pulse = setup_pulse(pulse_do, new_flags, when, new_period);
//    if (pulse != ILLEGAL) {
//      char_parameter_1[pulse] = click_pin[pulse];
//      pinMode(char_parameter_1[pulse], OUTPUT);
//      digitalWrite(char_parameter_1[pulse], LOW);
//    }
//  
//    return pulse;
//  }



#ifdef USE_SERIAL_BAUD
  const unsigned char mutedAllPulses[] PROGMEM = "muted all pulses";
#endif

void mute_all_clicks () {
  for (int pulse=0; pulse<CLICK_PULSES; pulse++)
    flags[pulse] &= ~ACTIVE;

  fix_global_next();

#ifdef USE_SERIAL_BAUD
  serial_println_progmem(mutedAllPulses);
#endif
}



/* **************************************************************** */
// creating, editing, killing pulses

// (re-) activate pulse that has been edited or modified at a given time:
// (assumes everything else is set up in order)
// can also be used to sync running pulses on a given time
int activate_pulse_synced(int pulse, struct time when, int sync)
{
  if (sync) {
    struct time delta = period[pulse];
    mul_time(&delta, sync);
    div_time(&delta, 2);
    add_time(&delta, &when);
  }

  last[pulse] = when;	// replace possibly random last with something a bit better
  next[pulse] = when;

  // now switch it on
  flags[pulse] |= ACTIVE;	// set ACTIVE
  flags[pulse] &= ~SCRATCH;	// clear SCRATCH
}



// make an existing pulse to a click pulse:
void en_click(int pulse)
{
  if (pulse != ILLEGAL) {
    periodic_do[pulse] = &click;
    char_parameter_1[pulse] = click_pin[pulse];
    pinMode(char_parameter_1[pulse], OUTPUT);
    digitalWrite(char_parameter_1[pulse], LOW);
  }
}


// make an existing pulse to a jiffle thrower pulse:
void en_jiffle_thrower(int pulse, unsigned int *jiffletab)
{
  if (pulse != ILLEGAL) {
    periodic_do[pulse] = &do_throw_a_jiffle;
    parameter_2[pulse] = (unsigned int) jiffletab;
  }
}


// make an existing pulse to display 1 info line:
void en_info(int pulse)
{
  if (pulse != ILLEGAL) {
    periodic_do[pulse] = &pulse_info_1line;
  }
}


// make an existing pulse to display multiline pulse info:
void en_INFO(int pulse)
{
  if (pulse != ILLEGAL) {
    periodic_do[pulse] = &pulse_info;
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
    char_parameter_1[pulse] = click_pin[pulse];
    pinMode(char_parameter_1[pulse], OUTPUT);
    digitalWrite(char_parameter_1[pulse], LOW);
  }

  return pulse;
}


int setup_jiffle_thrower_synced(struct time when,
				unsigned long unit,
				unsigned long factor, unsigned long divisor,
				int sync, unsigned int *jiffletab)
{
  int pulse= setup_pulse_synced(&do_throw_a_jiffle, ACTIVE,
			       when, unit, factor, divisor, sync);
  if (pulse != ILLEGAL)
    parameter_2[pulse] = (unsigned int) jiffletab;

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

const unsigned char rhythm_[] PROGMEM = "rhythm ";
const unsigned char sync_[] PROGMEM = "sync ";

void init_rhythm_1(int sync) {
  // By design click pulses *HAVE* to be defined *BEFORE* any other pulses:
  unsigned long divisor=1;
  unsigned long scaling=6;

#ifdef USE_SERIAL_BAUD
  serial_print_progmem(rhythm_); Serial.print(1);
  spaces(1); serial_print_progmem(sync_); Serial.println(sync);
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
  serial_print_progmem(rhythm_); Serial.print(2);
  spaces(1); serial_print_progmem(sync_); Serial.println(sync);
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
  serial_print_progmem(rhythm_); Serial.print(3);
  spaces(1); serial_print_progmem(sync_); Serial.println(sync);
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
  serial_print_progmem(rhythm_); Serial.print(4);
  spaces(1); serial_print_progmem(sync_); Serial.println(sync);
#endif

  init_click_pulses();
  get_now();

  setup_click_synced(now, scaling*time_unit, 1, 1, sync);     // 1
  init_ratio_sequence(now, 1, 1, 2, 1, 4, scaling, sync);     // 1/2, 2/3, 3/4, 4/5
}


#endif	//  #if ( CLICK_PULSES > 0)



/* **************************************************************** */
// infos on serial:

#ifdef USE_SERIAL_BAUD

// dest and selected_pulses
// destination of menu functions '*' '/' '=' and 's'

unsigned long selected_pulses=0L;	// pulse bitmask

// dest codes:
#define CODE_PULSES	0		// dest code pulses: apply selected_pulses
#define CODE_TIME_UNIT	1		// dest code time_unit
unsigned char dest = CODE_PULSES;



// display a time in seconds:
float display_realtime_sec(struct time duration) {
  float seconds=((float) duration.time / 1000000.0);
  seconds += overflow_sec * (float) duration.overflow;

  float scratch = 1000.0;
  while (scratch > max(seconds, 1.0)) {
    spaces(1);
    scratch /= 10.0;
  }


  Serial.print(seconds , 3);
  Serial.print("s");

  return seconds;
}


// time_info()
const unsigned char timeInfo[] PROGMEM = "*** TIME info\t\t";
const unsigned char timeOvfl[] PROGMEM = "time/ovfl ";
const unsigned char now_[] PROGMEM = "now ";

void time_info()
{
  unsigned long realtime = micros();

  serial_print_progmem(timeInfo);
  serial_print_progmem(timeOvfl);
  Serial.print(realtime);
  slash();
  Serial.print(now.overflow);		// cheating a tiny little bit...
  tab();
  struct time scratch=now;
  scratch.time = realtime;		// update running time
  serial_print_progmem(now_);
  display_realtime_sec(scratch);
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
  spaces(1);
}


void display_action(int pulse) {
  void (*scratch)(int);

  scratch=&click;
  if (periodic_do[pulse] == scratch) {
    Serial.print("click\t");	// 8 chars at least
    return;
  }

  scratch=&do_jiffle;
  if (periodic_do[pulse] == scratch) {
    Serial.print("do_jiffle");
    return;
  }

  scratch=&do_throw_a_jiffle;
  if (periodic_do[pulse] == scratch) {
    Serial.print("seed jiffle");
    return;
  }

  scratch=&pulse_info;
  if (periodic_do[pulse] == scratch) {
    Serial.print("pulse_info");
    return;
  }

  scratch=&pulse_info_1line;
  if (periodic_do[pulse] == scratch) {
    Serial.print("info line");
    return;
  }

  scratch=NULL;
  if (periodic_do[pulse] == scratch) {
    Serial.print("NULL\t");	// 8 chars at least
    return;
  }

  Serial.print("UNKNOWN\t");
}



// pulse_info()
const unsigned char timeUnit[] PROGMEM = "time unit";
const unsigned char timeUnits[] PROGMEM = " time units";
const unsigned char pulseInfo[] PROGMEM = "*** PULSE info ";
const unsigned char flags_[] PROGMEM = "\tflags ";
const unsigned char pulseOvfl[] PROGMEM = "\tpulse/ovf ";
const unsigned char lastOvfl[] PROGMEM = "last/ovfl ";
const unsigned char nextOvfl[] PROGMEM = "   \tnext/ovfl ";
const unsigned char index_[] PROGMEM = "\tindex ";
const unsigned char times_[] PROGMEM = "\ttimes ";
const unsigned char pulse_[] PROGMEM = "pulse ";
const unsigned char expected_[] PROGMEM = "expected ";
const unsigned char ul1_[] PROGMEM = "\tul1 ";

// pulse_info() as paylod for pulses:
// Prints pulse info over serial and blinks the LED
void pulse_info(int pulse) {

#ifdef LED_PIN
  digitalWrite(LED_PIN,HIGH);		// blink the LED
#endif

  serial_print_progmem(pulseInfo);
  Serial.print(pulse);
  slash();
  Serial.print((unsigned int) pulse_count[pulse]);

  tab();
  display_action(pulse);

  serial_print_progmem(flags_);
  serial_print_BIN(flags[pulse], 8);
  Serial.println();

  Serial.print("pin ");  Serial.print((int) char_parameter_1[pulse]);
  serial_print_progmem(index_);  Serial.print((int) char_parameter_2[pulse]);
  serial_print_progmem(times_);  Serial.print(int1[pulse]);
  Serial.print("\tp1 ");  Serial.print(parameter_1[pulse]);
  Serial.print("\tp2 ");  Serial.print(parameter_2[pulse]);
  serial_print_progmem(ul1_);  Serial.print(ulong_parameter_1[pulse]);

  Serial.println();		// start next line

  Serial.print((float) period[pulse].time / (float) time_unit,3);
  serial_print_progmem(timeUnits);

  serial_print_progmem(pulseOvfl);
  Serial.print((unsigned int) period[pulse].time);
  slash();
  Serial.print(period[pulse].overflow);

  tab();
  display_realtime_sec(period[pulse]);
  spaces(1); serial_print_progmem(pulse_);

  Serial.println();		// start next line

  serial_print_progmem(lastOvfl);
  Serial.print((unsigned int) last[pulse].time);
  slash();
  Serial.print(last[pulse].overflow);

  serial_print_progmem(nextOvfl);
  Serial.print(next[pulse].time);
  slash();
  Serial.print(next[pulse].overflow);

  tab();
  serial_print_progmem(expected_);
  display_realtime_sec(next[pulse]);

  Serial.println();		// start last line
  time_info();

  Serial.print("\n\n");			// traling empty line

#ifdef LED_PIN
  digitalWrite(LED_PIN,LOW);
#endif
}


const unsigned char noAlive[] PROGMEM = "no pulses alive";

void alive_pulses_info()
{
  int count=0;

  for (int pulse=0; pulse<PULSES; ++pulse)
    if (flags[pulse]) {				// any flags set?
      pulse_info(pulse);
      count++;
    }

  if (count == 0)
    serial_println_progmem(noAlive);
}



// pulse_info_1line():	one line pulse info, short version
const unsigned char Pulse_[] PROGMEM = "PULSE ";

void pulse_info_1line(int pulse) {
  unsigned long realtime=micros();	// let's take time *before* serial output

  serial_print_progmem(Pulse_);
  Serial.print(pulse);
  slash();
  Serial.print((unsigned int) pulse_count[pulse]);

  serial_print_progmem(flags_);
  serial_print_BIN(flags[pulse], 8);

  tab();
  print_period_in_time_units(pulse);

  tab();
  display_action(pulse);

  tab();
  serial_print_progmem(expected_); spaces(5);
  display_realtime_sec(next[pulse]);

  tab();
  serial_print_progmem(now_);
  struct time scratch = now;
  scratch.time = realtime;
  display_realtime_sec(scratch);

  if (selected_pulses & (1 << pulse))
    Serial.print(" *");

  Serial.println();
}


void alive_pulses_info_lines()
{
  int count=0;

  for (int pulse=0; pulse<PULSES; ++pulse)	// first port used
    if (flags[pulse]) {				// any flags set?
      pulse_info_1line(pulse);
      count++;
    }

  if (count == 0) {				// second port used
    serial_print_progmem(noAlive);
      count++;
    }

  if (count == 0)
    serial_println_progmem(noAlive);

  Serial.println();
}


void print_period_in_time_units(int pulse) {
  float time_units, scratch;

  serial_print_progmem(pulse_);
  time_units = ((float) period[pulse].time / (float) time_unit);

  scratch = 1000.0;
  while (scratch > max(time_units, 1.0)) {
    spaces(1);
    scratch /= 10.0;
  }

  Serial.print((float) time_units, 3);
  serial_print_progmem(timeUnits);
}
#endif	// #ifdef USE_SERIAL_BAUD



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


// enter_jiffletab(), edit jiffletab by hand:
#ifdef USE_SERIAL_BAUD
  const unsigned char jifftabFull[] PROGMEM = "jiffletab full";
  const unsigned char enterJiffletabVal[] PROGMEM = "enter jiffletab values";
#endif

void enter_jiffletab(unsigned int *jiffletab)
{
  int menu_input;
  int new_value;
  int index=0;			// counts ints, *not* triplets

  while (true) {
    if (!char_available())
      serial_println_progmem(enterJiffletabVal);

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
	serial_println_progmem(jifftabFull);
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
      spaces(1);
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
  // char_parameter_1[pulse]	click pin
  // char_parameter_2[pulse]	jiffletab index
  // parameter_1[pulse]		count down
  // parameter_2[pulse]		jiffletab[] pointer
  // ulong_parameter_1[pulse]	base period = period of starting pulse

  digitalWrite(char_parameter_1[pulse], pulse_count[pulse] & 1);	// click

  if (--parameter_1[pulse] > 0)				// countdown, phase endid?
    return;						//   no: return immediately

  // if we arrive here, phase endid, start next phase if any:
  unsigned int* jiffletab = (unsigned int *) parameter_2[pulse];	// read jiffletab[]
  char_parameter_2[pulse] += JIFFLETAB_INDEX_STEP;
  if (jiffletab[char_parameter_2[pulse]] == 0) {		// no next phase, return
    init_pulse(pulse);					// remove pulse
    return;						// and return
  }

  //initialize next phase, re-using the same pulse:
  int base_index = char_parameter_2[pulse];		// readability
  period[pulse].time =
    ulong_parameter_1[pulse] * jiffletab[base_index] / jiffletab[base_index+1];
  parameter_1[pulse] = jiffletab[base_index+2];		// count of next phase
}


int init_jiffle(unsigned int *jiffletab, struct time when, struct time new_period, int origin_pulse)
{
  struct time jiffle_period=new_period;

  jiffle_period.time = new_period.time * jiffletab[0] / jiffletab[1];

  int jiffle_pulse = setup_pulse(&do_jiffle, ACTIVE, when, jiffle_period);
  if (jiffle_pulse != ILLEGAL) {
    char_parameter_1[jiffle_pulse] = click_pin[origin_pulse];	// set pin
    // pinMode(click_pin[pulse++], OUTPUT);			// should be ok already
    char_parameter_2[jiffle_pulse] = 0;				// init phase 0
    parameter_1[jiffle_pulse] = jiffletab[2];			// count of first phase
    parameter_2[jiffle_pulse] = (unsigned int) jiffletab;
    ulong_parameter_1[jiffle_pulse] = new_period.time;
  }

  return pulse;
}


void do_throw_a_jiffle(int pulse) {		// for pulse_do
  // parameter_2[pulse]	= (unsigned int) jiffletab;

  // start a new jiffling pulse now (next [pulse] is not yet updated):
  init_jiffle((unsigned int *) parameter_2[pulse], next[pulse], period[pulse], pulse);
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
  serial_print_progmem(sync_); Serial.println(sync);
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



/* **************************************************************** */
// menu over different interfaces
// only MENU_over_serial implemented
//
// we could add other menus, like LCD or MIDI or whoknows,
// but nothing there in this version, sorry.
//
/* **************************************************************** */
// #define MENU_over_serial	// do we use serial menu?
/* **************************************************************** */
#ifdef MENU_over_serial

// inside  #ifdef MENU_over_serial
// ****************************************************************
// basic menu I/O:

// sometimes serial is not ready quick enough:
#define WAITforSERIAL 10



// char input with one byte buffering:
// (I have not needed more then one char yet...)
char stored_char, chars_stored=0;



int get_char() {
  if(!char_available())
    return ILLEGAL;		// EOF no input available

  if (chars_stored) {
    --chars_stored;
    return stored_char;
  } else if (Serial.available())
    return Serial.read();
  else
    return ILLEGAL;		// EOF no input available
}



const unsigned char storeFull[] PROGMEM = "char_store buffer full";
int char_store(char c) {
  if (chars_stored) {	// ERROR.  I have not needed more then one char yet...
    serial_println_progmem(storeFull);
  }

  chars_stored++;
  stored_char = c;
}



int char_available() {
  if (chars_stored || Serial.available()) 
    return 1;
  return 0;
}



// inside  #ifdef MENU_over_serial
// ****************************************************************
// menu I/O functions:

// get numeric input from serial
long numeric_input(long oldValue) {
  long input, num, sign=1;

  do {
    while (!char_available())	// wait for input
      ;

    delay(WAITforSERIAL);	// sometimes the second byte was not ready without that


    input = get_char();		// get first chiffre
  } while (input == ' ');	// skip leading space

  if (input == '-') {		//	check for sign
    sign = -1;
    input = get_char(); }
  else if (input == '+')
    input = get_char();

  if (input >= '0' && input <= '9')	// numeric?
    num = input - '0';
  else {				// NAN
    char_store(input);
    return oldValue;
  }

  while (char_available()) {
    input = get_char();
    if (input >= '0' && input <= '9')	// numeric?
      num = 10 * num + (input - '0');
    else {
      char_store(input);	// put NAN chars back into input buffer
      break;
    }
  }

  return sign * num;
}



// bar_graph()
// print one value & bar graph line:
const unsigned char outOfRange[] PROGMEM = " out of range.";
const unsigned char value_[] PROGMEM = "value ";

void bar_graph(int value) {
  int i, length=64, scale=1023;
  int stars = ((long) value * (long) length) / scale + 1 ;


  if (value >=0 && value <= 1024) {
    Serial.print(value); tab();
    for (i=0; i<stars; i++) {
      if (i == 0 && value == 0)		// zero
	Serial.print("0");

					// middle or top
      else if \
	((i == length/2 && value == 512) || (i == length && value == scale))
	Serial.print("|");
      else
	Serial.print("*");
    }
  } else {
    serial_print_progmem(value_);
    Serial.print(value);
    serial_println_progmem(outOfRange);
  }

  Serial.println();
}



// bar_graph_VU(pin):
// display a scrolling bar graph over the readings of an analog input.
//
// whenever the reading changes for more then +/- tolerance a new line is displayed
// tolerance can be changed by pressing '+' or '-'
// any other key stops VU display

// tolerance default 0. Let the user *see* the noise...
int bar_graph_tolerance=0;



// follow_info(pin)
// info line for bar graph:
const unsigned char followPin[] PROGMEM = "Follow pin ";
const unsigned char tolerance_[] PROGMEM = "\ttolerance ";

void follow_info(int pin) {
  // display info about pin and tolerance
  serial_print_progmem(followPin);
  Serial.print((int) pin);
  serial_print_progmem(tolerance_);
  Serial.println(bar_graph_tolerance);
}



// bar_graph_VU(pin):
// display a scrolling bar graph over the readings of an analog input.
//
// whenever the reading changes for more then +/- tolerance a new line is displayed
// tolerance can be changed by pressing '+' or '-'
// any other key stops VU display

const unsigned char VU_title[] PROGMEM = \
  "values\t\t +/- set tolerance\t(any other byte to stop)\n";
const unsigned char quit_[] PROGMEM = "(quit)";

void bar_graph_VU(int pin) {
  int value, oldValue=-9997;		// just an unlikely value
  int menu_input;

  follow_info(pin);

  serial_println_progmem(VU_title);
  while (true) {
    value =  analogRead(pin);
    if (abs(value - oldValue) > bar_graph_tolerance) {
      bar_graph(value);
      oldValue = value;
    }

    if (char_available()) {
      switch (menu_input = get_char()) {
      case '+':
	bar_graph_tolerance++;
	follow_info(pin);
	break;
      case '-':
	if (bar_graph_tolerance)
	  bar_graph_tolerance--;
	  follow_info(pin);
	break;
      case '\n': case '\r':	// linebreak after sending 'V'
        break;
      default:
	serial_println_progmem(quit_);
	return;		// exit
      }
    }
  }    
}



#ifdef HARDWARE_menu	// inside MENU_over_serial
// ****************************************************************
// hw info display functions:


// display_analog_reads()
// display analog snapshot read values and bar graphs, a line each input:
const unsigned char analog_reads_title[] PROGMEM = \
  "\npin\tvalue\t|\t\t\t\t|\t\t\t\t|";

void display_analog_reads() {
  int i, value;

  serial_println_progmem(analog_reads_title);

  for (i=0; i<ANALOG_INPUTs; i++) {
    value = analogRead(i);
    Serial.print(i); tab(); bar_graph(value);
  }

  Serial.println();
}



// pin_info()
// display configuration and state of a pin:
const unsigned char pin_[] PROGMEM = "pin ";
const unsigned char high_[] PROGMEM = "high";
const unsigned char low_[] PROGMEM = "low";
const unsigned char pullup_[] PROGMEM = "pullup";
const unsigned char hiZ_[] PROGMEM = "hi-z";

// display configuration and state of a pin:
void pin_info(uint8_t pin) {
  uint8_t mask = digitalPinToBitMask(pin);
  uint8_t port = digitalPinToPort(pin);
  volatile uint8_t *reg;

  if (port == NOT_A_PIN) return;

  // selected sign * and pin:
  if (pin == hw_PIN )
    Serial.print("*");
  else
    Serial.print(" ");
  serial_print_progmem(pin_);
  Serial.print((int) pin);
  serial_print_progmem(tab_);

  // input or output?
  reg = portModeRegister(port);
  // uint8_t oldSREG = SREG;	// let interrupt ACTIVE ;)
  // cli();
  if (*reg & mask) {		// digital OUTPUTS
    // SREG = oldSREG;
    Serial.print("O  ");

    // high or low?
    reg = portOutputRegister(port);
    // oldSREG = SREG;		// let interrupt ACTIVE ;)
    // cli();
    if (*reg & mask) {		    // HIGH
      // SREG = oldSREG;
      serial_print_progmem(high_);
    } else {			    // LOW
      // SREG = oldSREG;
      serial_print_progmem(low_);
    }
  } else {			// digital INPUTS
    // SREG = oldSREG;
    Serial.print("I  ");

    // pullup, tristate?
    reg = portOutputRegister(port);
    // oldSREG = SREG;		// let interrupt ACTIVE ;)
    // cli();
    if (*reg & mask) {		    // pull up resistor
      // SREG = oldSREG;
      serial_print_progmem(pullup_);
    } else {			    // tri state high-Z
      // SREG = oldSREG;
      serial_print_progmem(hiZ_);
    }
  }
  Serial.println();
}



// display configuration and state of all pins:
void pins_info() {
  for (uint8_t pin=0; pin<DIGITAL_PINs; pin++)
    pin_info(pin);
}



// watch_digital_input(pin)
// continuously display digital input readings, whenever the input changes:
const unsigned char watchingINpin[] PROGMEM = "watching digital input pin ";
const unsigned char anyStop[] PROGMEM = "\t\t(send any byte to stop)";
const unsigned char is_[] PROGMEM = " is ";

void watch_digital_input(int pin) {
  int value, old_value=-9997;

  serial_print_progmem(watchingINpin);
  Serial.print((int) pin);
  serial_println_progmem(anyStop);

  while (!char_available()) {
    value = digitalRead(hw_PIN);
    if (value != old_value) {
      old_value = value;
      serial_print_progmem(pin_); Serial.print((int) pin);
      serial_print_progmem(is_);
      if (value)
	serial_println_progmem(high_);
      else
	serial_println_progmem(low_);
    }
  }
  serial_println_progmem(quit_);
  get_char();
}



// ****************************************************************
// menu hardware display and reaction:

// menu_hardware_display()  display hardware menu:
const unsigned char hwMenuTitle[] PROGMEM = \
  "\n***  HARDWARE menu  ***\t\t";
const unsigned char selectPin[] PROGMEM = \
  "P=select pin for 'I, O, H, L, r, W, d, v' to work on ";
const unsigned char PPin[] PROGMEM = "\tP=pin (";
const unsigned char OIHLWr[] PROGMEM = \
  "O=OUTPUT\tI=INPUT\t\tH=HIGH\tL=LOW\tanalog:\tW=WRITE\t    r=read";
const unsigned char dv_[] PROGMEM = "d=digiwatch\tv=VU bar\t";
const unsigned char aAnalog_[] PROGMEM = "a=all analog\t.=pins_info";

void menu_hardware_display() {
  serial_print_progmem(hwMenuTitle);
  Serial.println(get_free_RAM());
  Serial.println();

  serial_print_progmem(selectPin);
  serial_print_progmem(PPin);
  if (hw_PIN == ILLEGAL)
    Serial.print("no");
  else
    Serial.print((int) hw_PIN);
  Serial.println(")");

  serial_println_progmem(OIHLWr);
  serial_print_progmem(dv_);
  serial_println_progmem(aAnalog_);
}

#endif	// HARDWARE_menu inside MENU_over_serial



/* **************************************************************** */
// program specific menu:



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
  spaces(2);
  for (int pulse=8; pulse<min(CLICK_PULSES,16); pulse++)
    if (selected_pulses & (1 << pulse))
      Serial.print(pulse, HEX);
    else
      Serial.print(".");
#endif

#if (PULSES > CLICK_PULSES)
  spaces(2);
  for (int pulse=CLICK_PULSES; pulse<PULSES; pulse++)
    if (selected_pulses & (1 << pulse))
      Serial.write('+');
    else
      Serial.write('.');
#endif

  Serial.println();
}

const unsigned char selected_[] PROGMEM = "selected ";

void print_selected() {
  serial_print_progmem(selected_);

  switch (dest) {
  case CODE_PULSES:
    print_selected_pulses();
    break;

  case CODE_TIME_UNIT:
    serial_println_progmem(timeUnit);
    break;
  }
}


// info_select_destination_with()
const unsigned char selectDestinationInfo[] PROGMEM =
  "SELECT DESTINATION for '= * / s K p n c j' to work on:\t\t";
const unsigned char selectPulseWith[] PROGMEM = "Select puls with ";
const unsigned char selectAllPulses[] PROGMEM =
  "\na=select *all* click pulses\tA=*all* pulses\tl=alive clicks\tL=all alive\tx=none\t~=invert selection";
const unsigned char uSelect[] PROGMEM = "u=select ";
const unsigned char selected__[] PROGMEM = "\t(selected)";

void info_select_destination_with(boolean extended_destinations) {
  serial_print_progmem(selectDestinationInfo);
  print_selected();  Serial.println();

  serial_print_progmem(selectPulseWith);
  for (int pulse=0; pulse<CLICK_PULSES; pulse++) {
    Serial.print(pulse); spaces(2);
  }

  serial_println_progmem(selectAllPulses);

  if(extended_destinations) {
    serial_print_progmem(uSelect);  serial_print_progmem(timeUnit);
    if(dest == CODE_TIME_UNIT) {
      serial_println_progmem(selected__);
    } else
      Serial.println();
    Serial.println();
  }
}



// menu_program_display()
const unsigned char helpInfo[] PROGMEM = \
  "?=help\tm=menu\ti=info\t.=short info";
const unsigned char microSeconds[] PROGMEM = " microseconds";
const unsigned char muteKill[] PROGMEM = \
  "M=mute all\tK=kill\n\nCREATE PULSES\tstart with 'P'\nP=new pulse\tc=en-click\tj=en-jiffle\tf=en-info\tF=en-INFO\tn=sync now\nS=sync ";
const unsigned char perSecond_[] PROGMEM = " per second)";
const unsigned char equals_[] PROGMEM = " = ";
const unsigned char switchPulse[] PROGMEM = "s=switch pulse on/off";

void menu_program_display() {
  serial_println_progmem(helpInfo);

  Serial.println();
  info_select_destination_with(false);

  serial_print_progmem(uSelect);  serial_print_progmem(timeUnit);
  Serial.print("  (");
  Serial.print(time_unit);
  serial_print_progmem(microSeconds);
  serial_print_progmem(equals_);
  Serial.print((float) (1000000.0 / (float) time_unit),3);
  serial_println_progmem(perSecond_);

  Serial.println();
  serial_print_progmem(switchPulse);
  tab();  serial_print_progmem(muteKill);
  Serial.println(sync);
}



#ifdef HARDWARE_menu

// please_select_pin()
// give a warning that no valid pin was selected
void please_select_pin() {
  serial_println_progmem(selectPin);
}


// bool menu_hardware_reaction(menu_input)
// try to react on menu_input, return success flag
const unsigned char invalid_[] PROGMEM = "(invalid)";
const unsigned char setToLow[] PROGMEM = " was set to LOW.";
const unsigned char analogWriteValue[] PROGMEM = "analog write value ";
const unsigned char analogWrite_[] PROGMEM = "analogWrite(";
const unsigned char analogValueOnPin[] PROGMEM = "analog value on pin ";

bool menu_hardware_reaction(char menu_input) {
  long newValue;

  switch (menu_input) {
//  // DEACTIVATED as it is in the menu already
//  case  'M':
//    serial_print_progmem(freeRAM);
//    Serial.print(get_free_RAM());
//    serial_println_progmem(bytes_);
//    break;
// or:
//  case  'M':	// RAM
//    RAM_info();
//    break;

  case 'P':	// uppercase/lowercase
  case 'p':	// we accept lowercase as it does not change the chip
    serial_print_progmem(selectPin);
    serial_print_progmem(tab_);

    newValue = numeric_input(hw_PIN);
    if (newValue>=0 && newValue<DIGITAL_PINs) {
      hw_PIN = newValue;
      pin_info(hw_PIN);
    } else
      serial_println_progmem(invalid_);
    break;

  case 'O':
    if (hw_PIN == ILLEGAL)
      please_select_pin();
    else {
      pinMode(hw_PIN, OUTPUT);
      pin_info(hw_PIN);
    }
    break;

  case 'I':
    if (hw_PIN == ILLEGAL)
      please_select_pin();
    else {
      pinMode(hw_PIN, INPUT);
      pin_info(hw_PIN);
    }
    break;

 case 'H':
    if (hw_PIN == ILLEGAL)
      please_select_pin();
    else {
      digitalWrite(hw_PIN, HIGH);
      pin_info(hw_PIN);
    }
    break;

  case 'L':
    if (hw_PIN == ILLEGAL)
      please_select_pin();
    else {
      digitalWrite(hw_PIN, LOW);
      pin_info(hw_PIN);
    }
    break;

  case 'W':
    if (hw_PIN == ILLEGAL)
      please_select_pin();
    else {
      serial_print_progmem(analogWriteValue);
      newValue = numeric_input(-1);
      if (newValue>=0 && newValue<=255) {
	Serial.println(newValue);

	analogWrite(hw_PIN, newValue);
	serial_print_progmem(analogWrite_); Serial.print((int) hw_PIN);
	Serial.print(", "); Serial.print(newValue); Serial.println(")");
      } else
	serial_println_progmem(quit_);
    }
    break;

  case 'r':
    if ((hw_PIN == ILLEGAL) | (hw_PIN >= ANALOG_INPUTs))
      please_select_pin();
    else {
      serial_print_progmem(analogValueOnPin); Serial.print((int) hw_PIN);
      serial_print_progmem(is_); Serial.println(analogRead(hw_PIN));
    }
    break;

  case 'v':
  case 'V':	// ok i'm still used to uppercase V
    if ((hw_PIN == ILLEGAL) | (hw_PIN >= ANALOG_INPUTs))
      please_select_pin();
    else
      bar_graph_VU(hw_PIN);
    break;

  case 'd':
    if (hw_PIN == ILLEGAL)
      please_select_pin();
    else {
      pinMode(hw_PIN, INPUT);
      pin_info(hw_PIN);
      watch_digital_input(hw_PIN);
    }
    break;

  case 'a':
    display_analog_reads();
    break;

  case '.':
    Serial.println();
    pins_info();
    Serial.println();
    break;

  default:
    return false;	// menu_input not found in this menu
  }
  return true;		// menu_input found in this menu
}

#endif // HARDWARE_menu
/* **************************************************************** */



/* **************************************************************** */
// functions called from the menu:

void multiply_period(int pulse, unsigned long factor) {
  struct time new_period;

  new_period=period[pulse];
  mul_time(&new_period, factor);
  set_new_period(pulse, new_period);
}


void divide_period(int pulse, unsigned long divisor) {
  struct time new_period;

  new_period=period[pulse];
  div_time(&new_period, divisor);
  set_new_period(pulse, new_period);
}


const unsigned char setTimeUnit_[] PROGMEM = "Set time unit to ";

void set_time_unit_and_inform(unsigned long new_value) {
  time_unit = new_value;
  serial_print_progmem(setTimeUnit_);
  Serial.print(time_unit);
  serial_println_progmem(microSeconds);
}


// menu interface to reset a pulse and prepare it to be edited:
void reset_and_edit_pulse(int pulse) {
  init_pulse(pulse);
  flags[pulse] |= SCRATCH;	// set SCRATCH flag
  flags[pulse] &= ~ACTIVE;	// remove ACTIVE

  // set a default pulse length:
  struct time scratch;
  scratch.time = time_unit;
  scratch.overflow = 0;
  mul_time(&scratch, 12);		// 12 looks like a usable default
  period[pulse] = scratch;
}



/* **************************************************************** */
//    MENU core
/* **************************************************************** */
// inside #defined MENU_over_serial
#if (defined(MENU_over_serial) || defined(MENU_LCD) ) 
  // global menu variable switches active menu:

  // menu codes:
  #define MENU_CODE_UNDECIDED	0
  #define MENU_CODE_PROGRAM	1
  #define MENU_CODE_HARDWARE	2

  unsigned char menu=MENU_CODE_UNDECIDED;

#endif	// (MENU_over_serial || MENU_LCD ) 


// ****************************************************************
// inside #defined MENU_over_serial
// top level serial menu display and reactions:


// menu_serial_common_display()
// display menu items common to all menus:
const unsigned char common_[] PROGMEM = \
  "\nPress 'm' or '?' for menu, 'q' quit this menu.";

#ifdef PROGRAM_menu
const unsigned char program_[] PROGMEM = \
  " 'P' program menu ";
#endif

#ifdef HARDWARE_menu
const unsigned char hardware_[] PROGMEM = \
  " 'H' hardware menu ";
#endif

// menu_serial_common_display()
// display menu items common to all menus:
void menu_serial_common_display() {
  serial_print_progmem(common_);
#ifdef PROGRAM_menu
  if (menu != MENU_CODE_PROGRAM)
    if (menu != MENU_CODE_HARDWARE)	// hardware menu hides 'P'
      serial_print_progmem(program_);
#endif
#ifdef HARDWARE_menu
  if (menu != MENU_CODE_HARDWARE)
    serial_print_progmem(hardware_);
#endif
}



// menu_serial_display()
// top level serial menu display function
void menu_serial_display() {
  serial_println_progmem(programLongName);

  switch (menu) {
  case MENU_CODE_UNDECIDED:
#ifdef PROGRAM_menu
  case MENU_CODE_PROGRAM:
    menu_program_display();
    break;
#endif
#ifdef HARDWARE_menu
  case MENU_CODE_HARDWARE:
    menu_hardware_display();
    break;
#endif

  default:		// ERROR: unknown menu code
    ;
  }
  menu_serial_common_display();
  Serial.println();

  Serial.println();
}



// menu_serial_common_reaction(menu_input)
// test menu_input for being a common menu entry key
// if yes, do it
// return success flag:
bool menu_serial_common_reaction(char menu_input) {
  switch (menu_input) {
  case 'm': case '?':	// menu
    menu_serial_display();
    break;

  case 'q':	// quit
    menu=MENU_CODE_UNDECIDED;
    menu_serial_display();
    break;
#ifdef PROGRAM_menu
  case 'P':	// PROGRAM menu
    menu = MENU_CODE_PROGRAM;
    menu_program_display();
    break;
#endif
#ifdef HARDWARE_menu
  case 'H':	// HARDWARE menu
    menu = MENU_CODE_HARDWARE;
    menu_serial_display();
    break;
#endif
  default:
    return false;	// menu entry not found
  }
  return true;		// menu entry found
}



// menu_serial_reaction(), react on serial menu input.
// check for serial input, wait if there´s none
// react on all available serial input
const unsigned char unknownMenuInput[] PROGMEM = "unknown menu input: ";

void menu_serial_reaction() {
  char menu_input;
  bool found;

  while(!char_available())
    ;

  if (char_available()) {
    while (char_available()) {
      found=false;

      switch (menu_input = get_char()) {	// submenu forking

      case ' ': case '\t':		// skip white chars
      case '\n': case '\r':		// skip newlines
	break;

      default:				// no whitespace, check menus:
	switch (menu) {			// check active menu:
	case MENU_CODE_UNDECIDED:
#ifdef PROGRAM_menu
	case MENU_CODE_PROGRAM:
	  menu=MENU_CODE_PROGRAM;	// case MENU_CODE_UNDECIDED
	  found = menu_serial_program_reaction(menu_input);
	  break;
#endif
#ifdef HARDWARE_menu
	case MENU_CODE_HARDWARE:
	  menu=MENU_CODE_HARDWARE;	// case MENU_CODE_UNDECIDED
	  found = menu_hardware_reaction(menu_input);
	  break;
#endif
	default:		// ERROR: unknown menu code
	  ;
	} // menu branching

	if (!found)		// common menu entry?
	  found = menu_serial_common_reaction(menu_input);

	if (!found) {		// unknown menu entry
	  serial_print_progmem(unknownMenuInput); Serial.println(menu_input);
	  while (char_available() > 0) {
	    menu_input = get_char();
	    Serial.print(byte(menu_input));
	  }
	  Serial.println();
	  break;
	}
      } // submenu forking

      if (!char_available())
	delay(WAITforSERIAL);
    } // input loop
  } // any input?
} // menu_serial_reaction()



// ****************************************************************
// menu_serial_program_reaction()
// const unsigned char selected_[] PROGMEM = "selected ";
const unsigned char killPulse[] PROGMEM = "kill pulse ";
const unsigned char killedAll[] PROGMEM = "killed all";
const unsigned char onlyPositive[] PROGMEM = "only positive sync ";

bool menu_serial_program_reaction(char menu_input) {
  long new_value=0;
  struct time time_scratch;

  switch (menu_input) {

  case '?':	// help
    menu_serial_display();
    alive_pulses_info_lines();
    time_info();  tab(); RAM_info();
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

    serial_print_progmem(selected_);
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

    serial_print_progmem(selected_);
    print_selected_pulses();
    break;

  case 'A':	// select destination: *all* pulses
    selected_pulses = ~0;

    serial_print_progmem(selected_);
    print_selected_pulses();
    break;

  case 'l':	// select destination: alive CLICK_PULSES
    selected_pulses=0;
    for (int pulse=0; pulse<CLICK_PULSES; pulse++)
      if(flags[pulse] && (flags[pulse] != SCRATCH))
	selected_pulses |= (1 << pulse);

    serial_print_progmem(selected_);
    print_selected_pulses();
    break;

  case 'L':	// select destination: all alive pulses
    selected_pulses=0;
    for (int pulse=0; pulse<PULSES; pulse++)
      if(flags[pulse] && (flags[pulse] != SCRATCH))
	selected_pulses |= (1 << pulse);

    serial_print_progmem(selected_);
    print_selected_pulses();
    break;

  case '~':	// invert destination selection
    selected_pulses = ~selected_pulses;

    serial_print_progmem(selected_);
    print_selected_pulses();
    break;

  case 'x':	// clear destination selection
    selected_pulses = 0;

    serial_print_progmem(selected_);
    print_selected_pulses();
    break;

  case 's':	// switch pulse on/off
    for (int pulse=0; pulse<CLICK_PULSES; pulse++) {
      if (selected_pulses & (1 << pulse)) {
	// special case: switching on an edited SCRATCH pulse:
	if((flags[pulse] & ACTIVE) == 0)	// was off
	  if (flags[pulse] & SCRATCH)	// SCRATCH set, like activating after edit
	    flags[pulse] &= ~SCRATCH;	// so we remove SCRATCH

	flags[pulse] ^= ACTIVE;

	if (flags[pulse] & ACTIVE) {	// DADA test ################
	  get_now();	// ################################################################
	  next[pulse] = now;
	  last[pulse] = next[pulse];	// for overflow logic
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
    serial_print_progmem(sync_);
    new_value = numeric_input(sync);
    if (new_value>=0 )
      sync = new_value;
    else
      serial_print_progmem(onlyPositive);
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
	serial_println_progmem(invalid_);
      break;

    case CODE_TIME_UNIT:
      new_value = numeric_input(1);
      if (new_value>0)
	set_time_unit_and_inform(time_unit*new_value);
      else
	serial_println_progmem(invalid_);
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
	serial_println_progmem(invalid_);
      break;

    case CODE_TIME_UNIT:
      new_value = numeric_input(1);
      if (new_value>0)
	set_time_unit_and_inform(time_unit/new_value);
      else
	serial_println_progmem(invalid_);
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
	serial_println_progmem(invalid_);
      break;

    case CODE_TIME_UNIT:
      new_value = numeric_input(1);
      if (new_value>0)
	set_time_unit_and_inform(new_value);
      else
	serial_println_progmem(invalid_);
      break;
    }
    break;

  case 'K':	// kill selected pulses
    for (int pulse=0; pulse<CLICK_PULSES; pulse++)	// DADA ################
      if (selected_pulses & (1 << pulse)) {
	init_pulse(pulse);
	serial_print_progmem(killPulse); Serial.println(pulse);
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


#endif	// MENU_over_serial



/* **************************************************************** */
// setup:


void setup() {
  #ifdef USE_SERIAL_BAUD
    Serial.begin(USE_SERIAL_BAUD);
  #endif

//	#ifdef USE_LCD
//	  LCD.begin(LCD_COLs, LCD_ROWs);
//	  LCD_print_line_progmem(0, programName);
//	  LCD_print_line_progmem(1, version);
//	  LCD_print_line_progmem(3, freeRAM);
//	  LCD.print(get_free_RAM());
//	#endif


#ifdef USE_SERIAL_BAUD
  #ifdef MENU_over_serial	// show message about menu
    menu_serial_display();
  #else
    serial_println_progmem(programLongName);
  #endif

  RAM_info(); Serial.println();
#endif

//	#ifdef USE_LCD
//	  delay(3000);
//	  LCD.clear();
//	  LCD_print_line_progmem(TOP, programName);
//	#endif


#ifdef LED_PIN
  pinMode(LED_PIN, OUTPUT);
#endif

#ifdef CLICK_PULSES
  click_pin[0]= 3;		// configure PINs here
  click_pin[1]= 4;		// configure PINs here
  click_pin[2]= 5;		// configure PINs here
  click_pin[3]= 6;		// configure PINs here
  click_pin[4]= 7;		// configure PINs here

  init_click_pins();
#endif


  // time and pulses *must* get initialized before setting up pulses:
  init_time();
  init_pulses();

  // By design click pulses *HAVE* to be defined *BEFORE* any other pulses:
  // init_rhythm_1(1);
  // init_rhythm_2(5);
  init_rhythm_3(3);
  // init_rhythm_4(1);
  // setup_jiffles0(1);

#ifdef MENU_over_serial
  Serial.println();
  alive_pulses_info_lines();
#endif

  fix_global_next();	// we *must* call that here late in setup();
}


/* **************************************************************** */
// main loop:

// overflow detection:
unsigned int last_overflow_displayed=0;


void loop() {
  get_now();

  if(now.overflow != last_overflow_displayed) {
    last_overflow_displayed = now.overflow;
    Serial.print("====> OVERFLOW <====  ");
    Serial.println((int) now.overflow);
    time_info();
    Serial.println();
  }

  check_maybe_do();

#ifdef MENU_over_serial
  if (char_available())
    menu_serial_reaction();
#endif

}

/* **************************************************************** */