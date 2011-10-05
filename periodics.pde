/* **************************************************************** */
// periodics
/* **************************************************************** */


/* **************************************************************** */
/*
   Periodically do multiple independent tasks.
   This is a early experimental test version
   used to test timer overflow strategies.

   New logic version to determine when to wake up tasks.
   Saving times along with an overflow count makes everything
   simple and flexible.

   struct time stores time and overflow.

   This version calculates global next event[s] *once* when the next
   event could possibly have changed. See fix_global_next().

   If a series of tasks are sceduled for the same time they will
   be called in fast sequence *without* fix_global_next() in between them.
   After all task with the same time are done fix_global_next() is called.

   If a task sets up another task with the same next it *can* decide to do
   the fix itself, if it seems appropriate. Normally this is not required.

   "play" version: having fun while debugging...
     clicks
     rhythms
     jiffles

*/
/* **************************************************************** */
#define ILLEGAL		-1



/* **************************************************************** */
// CONFIGURATION:

#define PERIODICS		15	// maximal number of tasks
					// plenty of free tasks for jiffles ;)

// clicks on piezzos to *hear* the result:
// *do* change 'case' statement in menu_serial_reaction() if you change this.
#ifndef CLICK_PERIODICS			// number of click frequencies
  #define CLICK_PERIODICS	 5 	// default number of click frequencies
#endif


#define LED_PIN			13

// to use serial define USE_SERIAL as serial speed: 
//  #define USE_SERIAL	115200
#define USE_SERIAL	57600
//  #define USE_SERIAL	38400


#ifdef USE_SERIAL		// simple menus over serial line?
  // menu basics:
  #define MENU_over_serial	// do we use a serial menu?

  // simple menu to access arduino hardware:
  #define HARDWARE_menu		// menu interface to hardware configu[Bration
  	  			// this will let you read digital and analog inputs
  				// watch changes on inputs as value or as bar graphs
  	  			// set digital and analog outputs, etc.
  #ifdef HARDWARE_menu
    char hw_PIN = ILLEGAL;
  #endif // HARDWARE_menu
#endif	// USE_SERIAL



/* **************************************************************** */
// board specific things:
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)	// mega boards
  #define ANALOG_INPUTs	16
  #define DIGITAL_PINs	54
#else								// 168/328 boards
  #define ANALOG_INPUTs	6
  #define DIGITAL_PINs	14
#endif	// board specific initialisations

#define LED_PIN	13



/* **************************************************************** */
/* **************************************************************** */

// global variables, task and time.
int task;

struct time {
  unsigned long time;
  unsigned int overflow;
};

struct time now, last_now;

struct time global_next;
unsigned int global_next_count=0;
int global_next_tasks[PERIODICS];

const float overflow_sec = 4294.9672851562600;	// overflow time in seconds

unsigned long time_unit = 100000L;		// scaling timer to 10/s 0.1s

// I want time_unit to be dividable by a semi random selection of small integers
// avoiding rounding errors as much as possible.
//
// I consider factorials as a good choice:
// unsigned long time_unit =    40320L;		// scaling timer to  8!, 0.040320s
// unsigned long time_unit =   362880L;		// scaling timer to  9!, 0,362880s 
// unsigned long time_unit =  3628800L;		// scaling timer to 10!, 3.628800s


int sync=1;

/* **************************************************************** */
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
// variables for tasks in arrays[task]:

// ============>>> adapt init_task() IF YOU CHANGE SOMETHING HERE <<<============
unsigned char flags[PERIODICS];
// flag masks:
#define ACTIVE			1	// switches task on/off
#define COUNTED			2	// repeats 'times[]' times, then vanishes
#define SCRATCH			8	// edit (or similar) in progress
#define DO_NOT_DELETE	       16	// dummy to avoid being thrown out
#define CUSTOM_1	       32	// can be used by periodic_do()
#define CUSTOM_2	       64	// can be used by periodic_do()
#define CUSTOM_3	      128	// can be used by periodic_do()


unsigned int pulse_count[PERIODICS];	// counts how many times the task woke up
struct time pulse[PERIODICS];		// timer steps, overflows
struct time last[PERIODICS];		// convenient, but not really needed
struct time next[PERIODICS];		// next wake up time, overflow


// internal parameter:
unsigned int times[PERIODICS];		// if COUNTED, gives number of executions
//					   else free for other internal use

// custom parameters[task]		//  comment/uncomment as appropriate:
					//  then *DO ADAPT init_task()* 

// ============>>> adapt init_task() IF YOU CHANGE SOMETHING HERE <<<============
// these parameters can be used by periodic_do(task):
int parameter_1[PERIODICS];
int parameter_2[PERIODICS];
// int parameter_3[PERIODICS];
// int parameter_4[PERIODICS];
unsigned long ulong_parameter_1[PERIODICS];
// unsigned long ulong_parameter_2[PERIODICS];
// unsigned long ulong_parameter_3[PERIODICS];
// unsigned long ulong_parameter_4[PERIODICS];
char char_parameter_1[PERIODICS];		// pin
char char_parameter_2[PERIODICS];		// index
// char char_parameter_3[PERIODICS];
// char char_parameter_4[PERIODICS];

// pointers on  void something(int task)  functions:
void (*periodic_do[PERIODICS])(int);
// ============>>> adapt init_task() IF YOU CHANGE SOMETHING HERE <<<============



/* **************************************************************** */
// use PROGMEM to save RAM:

#if (defined(USE_SERIAL) || defined(USE_LCD))
#include <avr/pgmspace.h>

const unsigned char programName[] PROGMEM = "PERIODICS";
const unsigned char programLongName[] PROGMEM = "*** Play with PERIODICS v0.2 ***";
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
//	// void LCD_print_progmem(const prog_uchar *str)	// does not work :(
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


#ifdef USE_SERIAL
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
#endif

#endif	// #if (defined(USE_SERIAL) || defined(USE_LCD))


/* **************************************************************** */
// init, reset or kill a task: 
void init_task(int task) {
  flags[task] = 0;
  periodic_do[task] = NULL;
  pulse_count[task] = 0;	//  or pulse_count[task] = ILLEGAL; ???
  times[task] = 0;
  pulse[task].time = 0;
  last[task].time = 0;
  last[task].overflow = 0;
  next[task].time = 0;
  next[task].overflow = 0;
  parameter_1[task] = 0;
  parameter_2[task] = 0;
  // parameter_3[task] = 0;
  // parameter_4[task] = 0;

  ulong_parameter_1[task] = 0L;
  // ulong_parameter_2[task] = 0L;
  // ulong_parameter_3[task] = 0L;
  // ulong_parameter_4[task] = 0L;
  char_parameter_1[task] = 0;
  char_parameter_2[task] = 0;
  // char_parameter_3[task] = 0;
  // char_parameter_4[task] = 0;

  // you *must* call fix_global_next(); late in setup()
}


// do this once from setup():
void init_tasks() {
  for (int task=0; task<PERIODICS; task++) {
    init_task(task);
  }
}


// void wake_task(int task);	do one life step of the task
// gets called from check_maybe_do()
void wake_task(int task) {
  pulse_count[task]++;				//      count

  if (periodic_do[task] != NULL) {		// there *is* something else to do?
    (*periodic_do[task])(task);			//   yes: do it
  }

  // prepare future:
  last[task].time = next[task].time;		// when it *should* have happened
  last[task].overflow = next[task].overflow;
  next[task].time += pulse[task].time;		// when it should happen again
  next[task].overflow += pulse[task].overflow;

  if (last[task].time > next[task].time)
    next[task].overflow++;

  //						// COUNTED task && end reached?
  if ((flags[task] & COUNTED) && ((pulse_count[task] +1) == times[task] ))
    if (flags[task] & DO_NOT_DELETE)		//   yes: DO_NOT_DELETE?
      flags[task] &= ~ACTIVE;			//     yes: just deactivate
    else
      init_task(task);				//     no:  DELETE task

}


// void fix_global_next();
// determine when the next event[s] will happen:
//
// to start properly you *must* call this once, late in setup()
// will automagically get updated later on
void fix_global_next() {
/* This version calculates global next event[s] *once* when the next
   event could possibly have changed.

   If a series of tasks are sceduled for the same time they will
   be called in fast sequence *without* fix_global_next() in between them.
   After all task with the same time are done fix_global_next() is called.

   If a task sets up another task with the same next it *can* decide to do
   the fix itself, if it seems appropriate. Normally this is not required.
*/

  // we work directly on the global variables here:
  global_next.overflow=~0;	// ILLEGAL value
  global_next_count=0;

  for (task=0; task<PERIODICS; task++) {		// check all tasks
    if (flags[task] & ACTIVE) {				// task active?
      if (next[task].overflow < global_next.overflow) {	// yes: earlier overflow?
	global_next.time = next[task].time;		//   yes: reset search
	global_next.overflow = next[task].overflow;
	global_next_tasks[0] = task;
	global_next_count = 1;
      } else {					// same (or later) overflow:
	if (next[task].overflow == global_next.overflow) {	// same overflow?
	  if (next[task].time < global_next.time) {		//   yes: new next?
	    global_next.time = next[task].time;		//     yes: reset search
	    global_next_tasks[0] = task;
	    global_next_count = 1;
	  } else					// (still *same* overflow)
	    if (next[task].time == global_next.time)		//    *same* next?
	      global_next_tasks[global_next_count++]=task; //  yes: save task, count
	}
	// (*later* overflows are always later)
      }
    } // active?
  } // task loop
}


// void check_maybe_do();
// check if it's time to do something and do it if it's time
// calls wake_task(task); to do one life step of the task
void check_maybe_do() {
  get_now();	// updates now

  if (global_next.overflow == now.overflow) {	// current overflow period?
    if (global_next.time <= now.time) {		//   yes: is it time?
      for (int i=0; i<global_next_count; i++)	//     yes:
	wake_task(global_next_tasks[i]);	//     wake next tasks up

      fix_global_next();			// determine next event[s] serie
    }
  } else					// (earlier or later overflow)
    if (global_next.overflow < now.overflow) {	// earlier overflow period?
      for (int i=0; i<global_next_count; i++)	//     yes, we're late...
	wake_task(global_next_tasks[i]);	//     wake next tasks up

      fix_global_next();			// determine next event[s] serie
    }
}



#ifdef USE_SERIAL
    const unsigned char noFreeTasks[] PROGMEM = "no free tasks";
#endif

int setup_task(void (*task_do)(int), unsigned char new_flags, struct time when, struct time new_pulse) {
  int task;

  if (new_flags == 0)				// illegal new_flags parameter
    return ILLEGAL;				//   should not happen

  for (task=0; task<PERIODICS; task++) {	// search first free task
    if (flags[task] == 0)			// flags==0 means empty task
      break;					//   found one
  }
  if (task == PERIODICS) {			// no task free :(

#ifdef USE_SERIAL
    serial_println_progmem(noFreeTasks);
#endif

    return ILLEGAL;			// ERROR
  }

  // initiaize new task				// yes, found a free task
  flags[task] = new_flags;			// initialize task
  periodic_do[task] = task_do;			// payload
  next[task].time = when.time;			// next wake up time
  next[task].overflow = when.overflow;
  pulse[task].time = new_pulse.time;
  pulse[task].overflow = new_pulse.overflow;

  // fix_global_next();	// this version does *not* automatically call that here...

  return task;
}


// unused?
int setup_counted_task(void (*task_do)(int), unsigned char new_flags, struct time when, struct time new_pulse, unsigned int count) {
  int task;

  task= setup_task(task_do, new_flags|COUNTED, when, new_pulse);
  times[task]= count;

  return task;
}


void set_new_period(int task, struct time new_pulse) {
  pulse[task].time = new_pulse.time;
  pulse[task].overflow = new_pulse.overflow;

  next[task].time = last[task].time + pulse[task].time;
  next[task].overflow = last[task].overflow + pulse[task].overflow;
  if(next[task].time < last[task].time)
    next[task].overflow++;

  if (flags[task] == 0)
    flags[task] = SCRATCH;	// looks like we're editing by hand,
				// so prevent deletion of the task

  fix_global_next();	// it's saver to do that from here, but could be omitted.
}


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


#ifdef USE_SERIAL
  const unsigned char freeRAM[] PROGMEM = "free RAM ";

  void RAM_info() {
    serial_print_progmem(freeRAM);
    Serial.print(get_free_RAM());
  }
#endif

/* **************************************************************** */
// some little things to play with:

// FlipFlop pins:

#if ( CLICK_PERIODICS > 0)
// clicks on piezzos to *hear* the result:
//   or connect LEDs, MOSFETs, MIDI, whatever...
//   these are just FlipFlop pins.

//  configured further up:
//  // *do* change 'case' statement in menu_serial_reaction() if you change this.
//  #ifndef CLICK_PERIODICS		// number of click frequencies
//    #define CLICK_PERIODICS	5       // default number of click frequencies
//  #endif
//  // *do* change 'case' statement in menu_serial_reaction() if you change this.



void init_click_tasks() {
  for (int task=0; task<CLICK_PERIODICS; task++) {
    init_task(task);
  }
}


void click(int task) {			// can be called from a task
  digitalWrite(char_parameter_1[task], pulse_count[task] & 1);
}



unsigned char click_pin[CLICK_PERIODICS];

void init_click_pins() {
  for (int task=0; task<CLICK_PERIODICS; task++) {
    pinMode(click_pin[task], OUTPUT);
    digitalWrite(click_pin[task], LOW);
  }
}


//  // unused? (I use the synced version more often)
//  int setup_click_task(void (*task_do)(int), unsigned char new_flags,
//  		     struct time when, struct time new_pulse) {
//    int task = setup_task(task_do, new_flags, when, new_pulse);
//    if (task != ILLEGAL) {
//      char_parameter_1[task] = click_pin[task];
//      pinMode(char_parameter_1[task], OUTPUT);
//      digitalWrite(char_parameter_1[task], LOW);
//    }
//  
//    return task;
//  }


// playing with rhythms:


// (re-) activate pulse that has been edited or modified at a given time:
// (assumes everything else is set up in order)
// can also be used to sync running pulses on a given time
int activate_pulse_synced(int task, struct time when, int sync)
{
  if (sync) {
    struct time delta = pulse[task];
    mul_time(&delta, sync);
    div_time(&delta, 2);
    add_time(&delta, &when);
  }

  last[task] = when;	// replace possibly random last with something a bit better
  next[task] = when;

  // now switch it on
  flags[task] |= ACTIVE;	// set ACTIVE
  flags[task] &= ~SCRATCH;	// clear SCRATCH

  fix_global_next();
}

// Generic setup pulse, stright or middle synced relative to 'when'.
// Pulse time and phase sync get deviated from unit, which is first
// multiplied by factor and divided by divisor.
// sync=0 gives stright syncing, sync=1 middle pulses synced.
int setup_pulse_synced(void (*task_do)(int), unsigned char new_flags,
		       struct time when, unsigned long unit,
		       unsigned long factor, unsigned long divisor, int sync)
{
  struct time new_pulse;

  if (sync) {
    struct time delta;
    delta.time = sync*unit/2L;
    delta.overflow = 0;

    mul_time(&delta, factor);
    div_time(&delta, divisor);

    add_time(&delta, &when);
  }

  new_pulse.time = unit;
  new_pulse.overflow = 0;
  mul_time(&new_pulse, factor);
  div_time(&new_pulse, divisor);

  return setup_task(task_do, new_flags, when, new_pulse);
}


// make an existing task to a click task:
void en_click(int task)
{
  if (task != ILLEGAL) {
    periodic_do[task] = &click;
    char_parameter_1[task] = click_pin[task];
    pinMode(char_parameter_1[task], OUTPUT);
    digitalWrite(char_parameter_1[task], LOW);
  }
}


// make an existing task to a jiffle thrower task:
void en_jiffle_thrower(int task, unsigned int *jiffletab)
{
  if (task != ILLEGAL) {
    periodic_do[task] = &do_throw_a_jiffle;
    parameter_2[task] = (unsigned int) jiffletab;
  }
}


// make an existing task to display 1 info line:
void en_info(int task)
{
  if (task != ILLEGAL) {
    periodic_do[task] = &task_info_1line;
  }
}


// make an existing task to display multiline task info:
void en_INFO(int task)
{
  if (task != ILLEGAL) {
    periodic_do[task] = &task_info;
  }
}



int setup_click_synced(struct time when, unsigned long unit, unsigned long factor,
		       unsigned long divisor, int sync) {
  int task= setup_pulse_synced(&click, ACTIVE, when, unit, factor, divisor, sync);

  if (task != ILLEGAL) {
    char_parameter_1[task] = click_pin[task];
    pinMode(char_parameter_1[task], OUTPUT);
    digitalWrite(char_parameter_1[task], LOW);
  }

  return task;
}


int setup_jiffle_thrower_synced(struct time when,
				unsigned long unit,
				unsigned long factor, unsigned long divisor,
				int sync, unsigned int *jiffletab)
{
  int task= setup_pulse_synced(&do_throw_a_jiffle, ACTIVE,
			       when, unit, factor, divisor, sync);
  if (task != ILLEGAL)
    parameter_2[task] = (unsigned int) jiffletab;

  return task;
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
// By design click tasks *HAVE* to be defined *BEFORE* any other tasks:
//
// usage:
// 1,2,3,4 pattern	init_ratio_sequence(now, 1, 1, 1, 0, 4, scaling, sync)
// 3,5,7,9 pattern	init_ratio_sequence(now, 3, 2, 1, 0, 4, scaling, sync)
// 1/2, 2/3, 3/4, 4/5	init_ratio_sequence(now, 1, 1, 2, 1, 4, scaling, sync)
{
  const unsigned long unit=scaling*time_unit;
  unsigned long factor=factor0;
  unsigned long divisor=divisor0;

  // init_click_tasks();

  for (; count; count--) {
    setup_click_synced(when, unit, factor, divisor, sync);
    factor += factor_step;
    divisor += divisor_step;
  }

  fix_global_next();
}


void init_rhythm_1(int sync) { 
  // By design click tasks *HAVE* to be defined *BEFORE* any other tasks:
  unsigned long divisor=1;
  unsigned long scaling=6;

  init_click_tasks();
  get_now();

  for (long factor=2L; factor<6L; factor++)	// 2, 3, 4, 5
    setup_click_synced(now, scaling*time_unit, factor, divisor, sync);

  // 2*2*3*5
  setup_click_synced(now, scaling*time_unit, 2L*2L*3L*5L, divisor, sync);

  fix_global_next();
}


// frequencies ratio 1, 4, 6, 8, 10
void init_rhythm_2(int sync) { 
  // By design click tasks *HAVE* to be defined *BEFORE* any other tasks:
  int scaling=60;
  unsigned long factor=1;
  unsigned long unit= scaling*time_unit;

  init_click_tasks();
  get_now();

  for (unsigned long divisor=4; divisor<12 ; divisor += 2)
    setup_click_synced(now, unit, factor, divisor, sync);

  // slowest *not* synced
  setup_click_synced(now, unit, 1, 1, 0);

  fix_global_next();
}

// nice 2 to 3 to 4 to 5 pattern with phase offsets
void init_rhythm_3(int sync) { 
  // By design click tasks *HAVE* to be defined *BEFORE* any other tasks:
  unsigned long factor, divisor=1L;
  const unsigned long scaling=5L;
  const unsigned long unit=scaling*time_unit;

  init_click_tasks();
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
  // By design click tasks *HAVE* to be defined *BEFORE* any other tasks:
  const unsigned long scaling=15L;

  init_click_tasks();
  get_now();

  setup_click_synced(now, scaling*time_unit, 1, 1, sync);     // 1
  init_ratio_sequence(now, 1, 1, 2, 1, 4, scaling, sync);     // 1/2, 2/3, 3/4, 4/5
}


#endif	//  #if ( CLICK_PERIODICS > 0)


/* **************************************************************** */
// debugging:

// clicking
// to let a task click a piezzo see 'click(int task)' as payload.
// flip flopping pins...


// infos on serial:
#ifdef USE_SERIAL

// I'll need these soon:
// destination of menu functions '*' '/' '=' and 's'
unsigned char selected_destination=~0;
// destinations codes (other then 0, 1, 2, ... for individual clicker tasks):
#define ALL_PERIODICS	PERIODICS		// destination code
#define TIME_UNIT	(PERIODICS + 1)		// destination code



const unsigned char timeInfo[] PROGMEM = "*** TIME info\t\t";
const unsigned char timeOvfl[] PROGMEM = "time/ovfl ";
const unsigned char now_[] PROGMEM = "now ";

// display a time in seconds:
float display_realtime_sec(struct time duration) {
  float seconds=((float) duration.time / 1000000.0);
  seconds += overflow_sec * (float) duration.overflow;

  Serial.print(seconds , 3);
  Serial.print("s");

  return seconds;
}


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


void print_action(int task) {
  void (*scratch)(int);

  scratch=&click;
  if (periodic_do[task] == scratch) {
    Serial.print("click\t");	// 8 chars at least
    return;
  }

  scratch=&do_jiffle0;
  if (periodic_do[task] == scratch) {
    Serial.print("do_jiffle0");
    return;
  }

  scratch=&do_throw_a_jiffle;
  if (periodic_do[task] == scratch) {
    Serial.print("seed jiffle");
    return;
  }

  scratch=&task_info;
  if (periodic_do[task] == scratch) {
    Serial.print("task_info");
    return;
  }

  scratch=&task_info_1line;
  if (periodic_do[task] == scratch) {
    Serial.print("info line");
    return;
  }

  scratch=NULL;
  if (periodic_do[task] == scratch) {
    Serial.print("NULL");
    return;
  }

  Serial.print("UNKNOWN\t");
}


const unsigned char timeUnit[] PROGMEM = "time unit";
const unsigned char timeUnits[] PROGMEM = " time units";
const unsigned char taskInfo[] PROGMEM = "*** TASK info ";
const unsigned char flags_[] PROGMEM = "\tflags ";
const unsigned char pulseOvfl[] PROGMEM = "\tpulse/ovf ";
const unsigned char lastOvfl[] PROGMEM = "last/ovfl ";
const unsigned char nextOvfl[] PROGMEM = "   \tnext/ovfl ";
const unsigned char index_[] PROGMEM = "\tindex ";
const unsigned char times_[] PROGMEM = "\ttimes ";
const unsigned char pulse_[] PROGMEM = "pulse ";
const unsigned char expected_[] PROGMEM = "expected ";
const unsigned char ul1_[] PROGMEM = "\tul1 ";

// task_info() as paylod for tasks:
// Prints task info over serial and blinks the LED
void task_info(int task) {

#ifdef LED_PIN
  digitalWrite(LED_PIN,HIGH);		// blink the LED
#endif

  serial_print_progmem(taskInfo);
  Serial.print(task);
  slash();
  Serial.print((unsigned int) pulse_count[task]);

  tab();
  print_action(task);

  serial_print_progmem(flags_);
  serial_print_BIN(flags[task], 8);
  Serial.println();

  Serial.print("pin ");  Serial.print((int) char_parameter_1[task]);
  serial_print_progmem(index_);  Serial.print((int) char_parameter_2[task]);
  serial_print_progmem(times_);  Serial.print(times[task]);
  Serial.print("\tp1 ");  Serial.print(parameter_1[task]);
  Serial.print("\tp2 ");  Serial.print(parameter_2[task]);
  serial_print_progmem(ul1_);  Serial.print(ulong_parameter_1[task]);

  Serial.println();		// start next line

  Serial.print((float) pulse[task].time / (float) time_unit,3);
  serial_print_progmem(timeUnits);

  serial_print_progmem(pulseOvfl);
  Serial.print((unsigned int) pulse[task].time);
  slash();
  Serial.print(pulse[task].overflow);

  tab();
  display_realtime_sec(pulse[task]);
  spaces(1); serial_print_progmem(pulse_);

  Serial.println();		// start next line

  serial_print_progmem(lastOvfl);
  Serial.print((unsigned int) last[task].time);
  slash();
  Serial.print(last[task].overflow);

  serial_print_progmem(nextOvfl);
  Serial.print(next[task].time);
  slash();
  Serial.print(next[task].overflow);

  tab();
  serial_print_progmem(expected_);
  display_realtime_sec(next[task]);

  Serial.println();		// start last line
  time_info();

  Serial.print("\n\n");			// traling empty line

#ifdef LED_PIN
  digitalWrite(LED_PIN,LOW);
#endif
}

void alive_tasks_info()
{
  for (int task=0; task<PERIODICS; ++task)
    if (flags[task])				// any flags set?
      task_info(task);
}


// one line task info version:
const unsigned char task_[] PROGMEM = "TASK ";

void task_info_1line(int task) {

  serial_print_progmem(task_);
  Serial.print(task);
  slash();
  Serial.print((unsigned int) pulse_count[task]);

  serial_print_progmem(flags_);
  serial_print_BIN(flags[task], 8);

  tab();
  print_pulse_in_time_units(task);

  tab();
  print_action(task);

  tab();
  serial_print_progmem(expected_);
  display_realtime_sec(next[task]);

  tab();
  serial_print_progmem(now_);
  struct time scratch = now;
  scratch.time= micros();		// update running time
  display_realtime_sec(scratch);

  if ((ALL_PERIODICS == selected_destination) || (task == selected_destination))
    Serial.print(" *");

  Serial.println();
}


void alive_tasks_info_lines()
{
  for (int task=0; task<PERIODICS; ++task)
    if (flags[task])				// any flags set?
      task_info_1line(task);
}


#endif	// #ifdef USE_SERIAL


/* **************************************************************** */
// functions to deal with clicks (i.e. from the menus):

#ifdef USE_SERIAL

const unsigned char mutedAllPulses[] PROGMEM = "muted all pulses";
#endif

void mute_all_clicks () {
  for (int task=0; task<CLICK_PERIODICS; task++)
    flags[task] &= ~ACTIVE;

  // fix_global_next();			// planed soon...

#ifdef USE_SERIAL
  serial_println_progmem(mutedAllPulses);
#endif
}



void print_pulse_in_time_units(int task) {
  float time_units, scratch;

  serial_print_progmem(pulse_);
  time_units = ((float) pulse[task].time / (float) time_unit);

  scratch = 1000.0;
  while (scratch > max(time_units, 1.0)) {
    spaces(1);
    scratch /= 10.0;
  }

  Serial.print((float) time_units, 3);
  serial_print_progmem(timeUnits);
}



/* **************************************************************** */
// jiffles:

// jiffletabs define melody:
// up to 256 triplets of {multiplicator, dividend, count}
// multiplicator and dividend determine period based on the startin tasks period
// a multiplicator of zero indicates end of jiffle
#define JIFFLETAB_INDEX_STEP	3
// unsigned int jiffletab0[] = {1,512,8, 1,1024,16, 1,2048,32, 1,1024,16, 0};
// unsigned int jiffletab0[] = {1,128,2, 1,256,6, 1,512,10, 1,1024,32, 1,3*128,20, 1,64,8, 0};
// unsigned int jiffletab0[] = {1,32,4, 1,64,8, 1,128,16, 1,256,32, 1,512,64, 1,1024,128, 0};	// testing octaves

// unsigned int jiffletab0[] =
//   {1,2096,4, 1,512,2, 1,128,2, 1,256,2, 1,512,8, 1,1024,32, 1,512,4, 1,256,3, 1,128,2, 1,64,1, 0};

// unsigned int jiffletab0[] = {2,1024*3,4, 1,1024,64, 1,2048,64, 1,512,2, 1,64,1, 1,32,1, 1,16,2, 0};


// unsigned int jiffletab0[] = {1,32,2, 0};	// doubleclick

unsigned int jiffletab0[] = {2,1024*3,4, 1,1024,64, 1,2048,64, 1,512,4, 1,64,3, 1,32,1, 1,16,2, 0};	// nice short jiffy

/*
unsigned int jiffletab1[] =
  {1,1024,64, 1,512,4, 1,128,2, 1,64,1, 1,32,1, 1,16,1, 0};
*/

void do_jiffle0 (int task) {	// to be called by task_do
  // char_parameter_1[task]	click pin
  // char_parameter_2[task]	jiffletab index
  // parameter_1[task]		count down
  // parameter_2[task]		jiffletab[] pointer
  // ulong_parameter_1[task]	base period = period of starting task

  digitalWrite(char_parameter_1[task], pulse_count[task] & 1);	// click

  if (--parameter_1[task] > 0)				// countdown, phase endid?
    return;						//   no: return immediately

  // if we arrive here, phase endid, start next phase if any:
  unsigned int* jiffletab = (unsigned int *) parameter_2[task];	// read jiffletab[]
  char_parameter_2[task] += JIFFLETAB_INDEX_STEP;
  if (jiffletab[char_parameter_2[task]] == 0) {		// no next phase, return
    init_task(task);					// remove task
    return;						// and return
  }

  //initialize next phase, re-using the same task:
  int base_index = char_parameter_2[task];			// readability
  pulse[task].time =
    ulong_parameter_1[task] * jiffletab[base_index] / jiffletab[base_index+1];
  parameter_1[task] = jiffletab[base_index+2];			// count of next phase
  // fix_global_next();
}

int init_jiffle(unsigned int *jiffletab, struct time when, struct time new_pulse, int origin_task)
{
  struct time jiffle_pulse=new_pulse;

  jiffle_pulse.time = new_pulse.time * jiffletab[0] / jiffletab[1];

  int jiffle_task = setup_task(&do_jiffle0, ACTIVE, when, jiffle_pulse);
  if (jiffle_task != ILLEGAL) {
    char_parameter_1[jiffle_task] = click_pin[origin_task];	// set pin
    // pinMode(click_pin[task++], OUTPUT);			// should be ok already
    char_parameter_2[jiffle_task] = 0;				// init phase 0
    parameter_1[jiffle_task] = jiffletab[2];			// count of first phase
    parameter_2[jiffle_task] = (unsigned int) jiffletab;
    ulong_parameter_1[jiffle_task] = new_pulse.time;
  }

  return task;
}


void do_throw_a_jiffle(int task) {		// for task_do
  // parameter_2[task]	= (unsigned int) jiffletab;

  // start a new jiffling task now (next [task] is not yet updated):
  init_jiffle((unsigned int *) parameter_2[task], next[task], pulse[task], task);
}


void setup_jiffle_thrower(unsigned int *jiffletab, unsigned char new_flags, struct time when, struct time new_pulse) {
  int jiffle_task = setup_task(&do_throw_a_jiffle, new_flags, when, new_pulse);
  if (jiffle_task != ILLEGAL) {
    parameter_2[jiffle_task] = (unsigned int) jiffletab;
  }
}


void setup_jiffles0(int sync) {
  unsigned long factor, divisor = 1;

  int scale=18;
  unsigned long unit=scale*time_unit;

  struct time when, delta, templ, new_pulse;

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
// #define MENU_over_serial	// do we use a serial menu?
/* **************************************************************** */
#ifdef MENU_over_serial

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



const unsigned char storeFull[] PROGMEM = "char_store: sorry, buffer full.";
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



// get numeric input from serial
long numericInput(long oldValue) {
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



const unsigned char ON_off[] PROGMEM = "ON/off";
const unsigned char on_OFF[] PROGMEM = "on/OFF";

// serial display helper function
int ONoff(int value, int mode, int tab) {
  if (value) {
    switch (mode) {
    case 0:		// ON		off
    case 1:		// ON		OFF
      Serial.print("ON");
      break;
    case 2:		// ON/off	on/OFF
      serial_print_progmem(ON_off);
      break;
    }
  }
  else {
    switch (mode) {
    case 0:		// ON		off
      Serial.print("off");
      break;
    case 1:		// ON		OFF
      Serial.print("OFF");
      break;
    case 2:		// ON/off	on/OFF
      serial_print_progmem(on_OFF);
      break;
    }
  }

  if (tab)
    serial_print_progmem(tab_);

  return value;
}



const unsigned char outOfRange[] PROGMEM = " out of range.";
const unsigned char value_[] PROGMEM = "value ";

// display helper function
void bar_graph(int value) {
  int i, length=64, scale=1023;
  int stars = ((long) value * (long) length) / scale + 1 ;


  if (value >=0 && value <= 1024) {
    Serial.print(value); serial_print_progmem(tab_);
    for (i=0; i<stars; i++) {
      if (i == 0 && value == 0)		// zero
	Serial.print("0");
      else if ((i == length/2 && value == 512) || (i == length && value == scale))	// middle or top
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



const unsigned char followPin[] PROGMEM = "Follow pin ";
const unsigned char VU_title[] PROGMEM = "values\t\t +/- tolerance (send any other byte to stop)\n";
const unsigned char quit_[] PROGMEM = "(quit)";

void bar_graph_VU(int pin) {
  int value, oldValue=-9997;	// just an unlikely value...
  int tolerance=1, menu_input;

  serial_println_progmem(followPin);
  Serial.print((int) pin);
  serial_println_progmem(VU_title);

  while (true) {
    value =  analogRead(pin);
    if (abs(value - oldValue) > tolerance) {
      bar_graph(value);
      oldValue = value;
    }

    if (char_available()) {
      switch (menu_input = get_char()) {
      case '+':
	tolerance++;
	break;
      case '-':
	if (tolerance)
	  tolerance--;
	break;
      default:
	serial_println_progmem(quit_);
	return;		// exit
      }
    }
  }    
}



#ifdef HARDWARE_menu	// inside MENU_over_serial
const unsigned char analog_reads_title[] PROGMEM = "\npin\tvalue\t|\t\t\t\t|\t\t\t\t|";

void display_analog_reads() {
  int i, value;

  serial_println_progmem(analog_reads_title);

  for (i=0; i<ANALOG_INPUTs; i++) {
    value = analogRead(i);
    Serial.print(i); serial_print_progmem(tab_); bar_graph(value);
  }

  Serial.println();
}



const unsigned char watchingINpin[] PROGMEM = "watching digital input pin ";
const unsigned char anyStop[] PROGMEM = "\t\t(send any byte to stop)";
const unsigned char pin_[] PROGMEM = "pin ";		// defined already
const unsigned char is_[] PROGMEM = " is ";
const unsigned char high_[] PROGMEM = "HIGH";
const unsigned char low_[] PROGMEM = "LOW";

void watch_digital_input(int pin) {
  int value, old_value=-9997;

  serial_println_progmem(watchingINpin);
  Serial.print((int) pin);
  serial_println_progmem(anyStop);

  while (!char_available()) {
    value = digitalRead(hw_PIN);
    if (value != old_value) {
      old_value = value;
      serial_println_progmem(pin_); Serial.print((int) pin); serial_print_progmem(is_);
      if (value)
	serial_println_progmem(high_);
      else
	serial_println_progmem(low_);
    }
  }
  serial_println_progmem(quit_);
  get_char();
}


const unsigned char hwMenuTitle[] PROGMEM = "\n***  HARDWARE  ***\t\tfree RAM=";
const unsigned char PPin[] PROGMEM = "P=PIN (";
const unsigned char HLWR[] PROGMEM = ")\tH=set HIGH\tL=set LOW\tW=analog write\tR=read";
const unsigned char VI[] PROGMEM = "V=VU bar\tI=digiwatch\t";
const unsigned char aAnalogRead[] PROGMEM = "a=analog read";


void menu_hardware_display() {
  serial_print_progmem(hwMenuTitle);
  Serial.println(get_free_RAM());
  Serial.println();

  serial_print_progmem(PPin);
  if (hw_PIN == ILLEGAL)
    Serial.print("no");
  else
    Serial.print((int) hw_PIN);
  serial_println_progmem(HLWR);
  serial_print_progmem(VI);
  serial_println_progmem(aAnalogRead);
}

#endif	// HARDWARE_menu inside MENU_over_serial



/* **************************************************************** */
// program specific menu:

const unsigned char switchPulse[] PROGMEM = "s=switch pulse on/off";
// const unsigned char freeRAM[] PROGMEM = "free RAM ";
const unsigned char pPin[] PROGMEM = "p=pin (";
const unsigned char none_[] PROGMEM = "(none)";

const unsigned char selectDestinationInfo[] PROGMEM =
  "SELECT DESTINATION for '= * / s K p n c j' to work on:";
const unsigned char selectPulseWith[] PROGMEM = "Select puls with ";
const unsigned char killPulse[] PROGMEM = "kill pulse ";
const unsigned char resetPulse[] PROGMEM = "reset pulse ";
const unsigned char all_[] PROGMEM = "(ALL)";
const unsigned char selectAllPulses[] PROGMEM = "A=select *all* pulses";
const unsigned char uSelect[] PROGMEM = "u=select ";
const unsigned char selected__[] PROGMEM = "\t(selected)";

void info_select_destination_with(boolean extended_destinations) {
  serial_println_progmem(selectDestinationInfo);
  serial_print_progmem(selectPulseWith);
  for (int task=0; task<CLICK_PERIODICS; task++) {
    Serial.print(task); spaces(2);
  }
  serial_print_progmem(tab_);
  if (selected_destination < CLICK_PERIODICS) {
    Serial.print("(");
    Serial.print((int) selected_destination);
    Serial.println(")");
  } else 
    if(selected_destination == ALL_PERIODICS) {
      serial_println_progmem(all_);
    } else
      serial_println_progmem(none_);

  serial_println_progmem(selectAllPulses);


  if(extended_destinations) {
    serial_print_progmem(uSelect);  serial_print_progmem(timeUnit);
    if(selected_destination == TIME_UNIT) {
    serial_println_progmem(selected__);
    } else
      Serial.println();
    Serial.println();
  }
}



const unsigned char pressm[] PROGMEM = "\nPress 'm' or '?' for menu.\n\n";

const unsigned char helpInfo[] PROGMEM = "?=help\tm=menu\ti=info\t.=short info";
const unsigned char microSeconds[] PROGMEM = " microseconds";
const unsigned char muteKill[] PROGMEM = "M=mute all\tK=kill\n\nCREATE PULSES\tstart with 'p'\np=new pulse\tc=en-click\tj=en-jiffle\tf=en-info\tF=en-INFO\tn=sync now\nS=sync ";
const unsigned char perSecond_[] PROGMEM = " per second)";
const unsigned char equals_[] PROGMEM = " = ";

void display_serial_menu() {
  serial_println_progmem(programLongName);
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

#ifdef HARDWARE_menu	// inside MENU_over_serial
  menu_hardware_display();
#endif

  serial_print_progmem(pressm);
}



const unsigned char selectPin[] PROGMEM = "Select a pin with P.";

void please_select_pin() {
  serial_println_progmem(selectPin);
}

//	const unsigned char timeInterval[] PROGMEM = "time interval ";
//	const unsigned char milliseconds_[] PROGMEM = "milliseconds\t";
//	const unsigned char unchanged_[] PROGMEM = "(unchanged)";

const unsigned char bytes_[] PROGMEM = " bytes";


#ifdef HARDWARE_menu
const unsigned char numberOfPin[] PROGMEM = "Number of pin to work on: ";
const unsigned char sync_[] PROGMEM = "sync ";
const unsigned char invalid[] PROGMEM = "(invalid)";
const unsigned char setToHigh[] PROGMEM = " was set to HIGH.";
const unsigned char setToLow[] PROGMEM = " was set to LOW.";
const unsigned char analogWriteValue[] PROGMEM = "analog write value ";
const unsigned char analogWrite_[] PROGMEM = "analogWrite(";
const unsigned char analogValueOnPin[] PROGMEM = "analog value on pin ";

int hardware_menu_reaction(char menu_input) {
  long newValue;

  switch (menu_input) {
  case  'M':					// hidden by program menus 'M'
    serial_print_progmem(freeRAM);
    Serial.print(get_free_RAM());
    serial_println_progmem(bytes_);
    break;

  case 'P':
    serial_print_progmem(numberOfPin);
    newValue = numericInput(hw_PIN);
    if (newValue>=0 && newValue<DIGITAL_PINs) {
      hw_PIN = newValue;
      Serial.println((int) hw_PIN);
    } else
      serial_println_progmem(none_);
    break;

  case 'H':
    if (hw_PIN == ILLEGAL)
      please_select_pin();
    else {
      pinMode(hw_PIN, OUTPUT);
      digitalWrite(hw_PIN, HIGH);
      serial_print_progmem(pin_); Serial.print((int) hw_PIN);
      serial_println_progmem(setToHigh);
    }
    break;

  case 'L':
    if (hw_PIN == ILLEGAL)
      please_select_pin();
    else {
      pinMode(hw_PIN, OUTPUT);
      digitalWrite(hw_PIN, LOW);
      serial_print_progmem(pin_); Serial.print((int) hw_PIN);
      serial_println_progmem(setToLow);
    }
    break;

  case 'W':
    if (hw_PIN == ILLEGAL)
      please_select_pin();
    else {
      serial_print_progmem(analogWriteValue);
      newValue = numericInput(-1);
      if (newValue>=0 && newValue<=255) {
	Serial.println(newValue);

	analogWrite(hw_PIN, newValue);
	serial_print_progmem(analogWrite_); Serial.print((int) hw_PIN);
	Serial.print(", "); Serial.print(newValue); Serial.println(")");
      } else
	serial_println_progmem(quit_);
    }
    break;

  case 'R':
    if (hw_PIN == ILLEGAL)
      please_select_pin();
    else {
      serial_print_progmem(analogValueOnPin); Serial.print((int) hw_PIN);
      serial_print_progmem(is_); Serial.println(analogRead(hw_PIN));
    }
    break;

  case 'V':
    if ((hw_PIN == ILLEGAL) | (hw_PIN >= ANALOG_INPUTs))
      please_select_pin();
    else
      bar_graph_VU(hw_PIN);
    break;

  case 'I':
    if (hw_PIN == ILLEGAL)
      please_select_pin();
    else {
      pinMode(hw_PIN, INPUT);
      watch_digital_input(hw_PIN);
    }
    break;

  case 'a':
    display_analog_reads();
    break;
  default:
    return 0;		// menu_input not found in this menu
  }
  return 1;		// menu_input found in this menu
}
#endif



/* **************************************************************** */
// functions called from the menu:

const unsigned char multipliedPulse_[] PROGMEM = "Multiplied pulse[";

void multiply_period_and_inform(int task, unsigned long factor) {
  struct time new_pulse;

  new_pulse=pulse[task];
  mul_time(&new_pulse, factor);
  set_new_period(task, new_pulse);

  serial_print_progmem(multipliedPulse_);
  Serial.print((int) task);
  Serial.print("] by ");
  Serial.println(factor);

  task_info_1line(task);
}


const unsigned char dividedPulse_[] PROGMEM = "Divided pulse[";

void divide_period_and_inform(int task, unsigned long divisor) {
  struct time new_pulse;

  new_pulse=pulse[task];
  div_time(&new_pulse, divisor);
  set_new_period(task, new_pulse);

  serial_print_progmem(dividedPulse_);
  Serial.print((int) task);
  Serial.print("] by ");
  Serial.println(divisor);
  task_info_1line(task);
}


const unsigned char setPulse_[] PROGMEM = "Set pulse[";

void set_new_period_and_inform(int task, struct time new_pulse) {
  set_new_period(task, new_pulse);

  serial_print_progmem(setPulse_);
  Serial.print((int) task);
  Serial.print("] to ");
  print_pulse_in_time_units(task);
  //  Serial.print(value);
  Serial.println();

  task_info_1line(task);
}


const unsigned char setTimeUnit_[] PROGMEM = "Set time unit to ";

void set_time_unit_and_inform(unsigned long newValue) {
  time_unit = newValue;
  serial_print_progmem(setTimeUnit_);
  Serial.print(time_unit);
  serial_println_progmem(microSeconds);
}

const unsigned char periodic_[] PROGMEM = "periodic ";
const unsigned char switched_[] PROGMEM = "Switched ";

void switch_periodic_and_inform(int task) {
  // special case: switching on an edited SCRATCH task:
  if((flags[task] & ACTIVE) == 0)	// was off
    if (flags[task] & SCRATCH)		// SCRATCH set, like activating after edit
      flags[task] &= ~SCRATCH;		// so we remove SCRATCH

  flags[task] ^= ACTIVE;

  serial_print_progmem(switched_);  serial_print_progmem(periodic_);
  Serial.print((int)  task);
  if (flags[task] & ACTIVE) {
    get_now();
    next[task] = now;
    last[task] = next[task];	// for overflow logic

    Serial.println(" on\t");
    task_info_1line(task);
  } else
    Serial.println (" off");

  fix_global_next();
}



/* **************************************************************** */
// menu reaction:

const unsigned char unknownMenuInput[] PROGMEM = "unknown menu input: ";

const unsigned char selected_[] PROGMEM = "Selected ";
const unsigned char allPulses[] PROGMEM = "*all* pulses";
const unsigned char killedAll[] PROGMEM = "killed all";
const unsigned char onlyPositive[] PROGMEM = "only positive sync ";

void menu_serial_reaction() {
  char menu_input;
  long newValue;
  struct time time_scratch;

  while(!char_available())
    ;

  if (char_available()) {
    while (char_available()) {
      newValue=0;

      switch (menu_input = get_char()) {
      case ' ': case '\t':		// skip white chars
	break;

      case '?':
	display_serial_menu();
	alive_tasks_info_lines();
	time_info();  tab(); RAM_info(); Serial.println();
	break;

      case 'm':
	display_serial_menu();
	break;

      case '.':
	time_info(); Serial.println();
	alive_tasks_info_lines();
	// RAM_info(); Serial.println();
	Serial.println();
	break;

      // *do* change this line if you change CLICK_PERIODICS
      case '0': case '1': case '2': case '3': case '4':
	// display(menu_input - '0');
	selected_destination = menu_input - '0';
	serial_print_progmem(selected_);
	serial_print_progmem(periodic_);
	Serial.println((int)  menu_input - '0');
	break;

      case 'u':
	selected_destination = TIME_UNIT;
	serial_print_progmem(selected_);
	serial_println_progmem(timeUnit);
	break;

      case 'A':
	selected_destination = ALL_PERIODICS;
	serial_print_progmem(selected_);
	serial_println_progmem(allPulses);
	break;

      case 's':
	if (selected_destination < CLICK_PERIODICS) {
	  switch_periodic_and_inform(selected_destination);
	} else
	  switch (selected_destination) {
	  case ALL_PERIODICS:
	    for (int task=0; task<CLICK_PERIODICS; task++ ) {
	      switch_periodic_and_inform(task);
	    }
	    Serial.println();
	    break;

	  default:
	    info_select_destination_with(false);
	  }
	break;

      case 'S':
	serial_print_progmem(sync_);
	newValue = numericInput(sync);
	if (newValue>=0 )
	  sync = newValue;
	else
	  serial_print_progmem(onlyPositive);
	Serial.println(sync);
	break;

      case 'i':
	RAM_info(); Serial.println(); Serial.println();
	alive_tasks_info();
	break;

      case 'M':					// hides hardware menus 'M'
	mute_all_clicks();
	break;

      case '*':
	if (selected_destination < CLICK_PERIODICS) {
	  newValue = numericInput(1);
	  if (newValue>=0) {
	    multiply_period_and_inform(selected_destination, newValue);
	  } else
	    serial_println_progmem(invalid);

	} else {
	  newValue = numericInput(1);

	  switch (selected_destination) {
	  case ALL_PERIODICS:
	    for (int task=0; task<CLICK_PERIODICS; task++ ) {
	      multiply_period_and_inform(task, newValue);
	    }
	    Serial.println();
	    break;

	  case TIME_UNIT:
	    set_time_unit_and_inform(time_unit*newValue);
	    break;

	  default:
	      info_select_destination_with(true);
	  }
	}
	break;

      case '/':
	if (selected_destination < CLICK_PERIODICS) {
	  newValue = numericInput(1);
	  if (newValue>0) {
	    divide_period_and_inform(selected_destination, newValue);
	  } else
	    serial_println_progmem(invalid);

	} else {
	  newValue = numericInput(1);

	  switch (selected_destination) {
	  case ALL_PERIODICS:
	    for (int task=0; task<CLICK_PERIODICS; task++ ) {
	      divide_period_and_inform(task, newValue);
	    }
	    Serial.println();
	    break;

	  case TIME_UNIT:
	    set_time_unit_and_inform(time_unit/newValue);
	    break;

	  default:
	    info_select_destination_with(true);
	  }
	}
	break;

      case '=':
	if (selected_destination < CLICK_PERIODICS) {		// periodics
	newValue = numericInput(pulse[selected_destination].time / time_unit);
	if (newValue>=0) {
	  time_scratch.time = newValue * time_unit;
	  time_scratch.overflow = 0;
	  set_new_period_and_inform(selected_destination, time_scratch);
	} else
	  serial_println_progmem(invalid);

	} else {
	  newValue = numericInput(1);

	  switch (selected_destination) {
	  case ALL_PERIODICS:
	    time_scratch.time = newValue * time_unit;
	    time_scratch.overflow = 0;
	    for (int task=0; task<CLICK_PERIODICS; task++ ) {
	      set_new_period_and_inform(selected_destination, time_scratch);
	    }
	    Serial.println();
	    break;

	  case TIME_UNIT:			// time_unit
	    set_time_unit_and_inform(newValue);
	    break;

	  default:
	    info_select_destination_with(true);
	  }
	}
	break;

      case 'K':
	if (selected_destination < CLICK_PERIODICS) {		// periodics
	  init_task(selected_destination);
	  serial_print_progmem(killPulse); Serial.println(selected_destination);
	  alive_tasks_info_lines(); Serial.println();
	} else
	  if (selected_destination == ALL_PERIODICS) {
	    init_tasks();
	    serial_println_progmem(killedAll);
	  } else
	    info_select_destination_with(false);
	break;

      case 'p':
	if (selected_destination < CLICK_PERIODICS) {		// periodics
	  init_task(selected_destination);
	  serial_print_progmem(resetPulse); Serial.println(selected_destination);
	  flags[selected_destination] |= SCRATCH;	// set SCRATCH flag
	  flags[selected_destination] &= ~ACTIVE;	// remove ACTIVE

	  // set a default pulse length:
	  struct time scratch;
	  scratch.time = time_unit;
	  scratch.overflow = 0;
	  mul_time(&scratch, 12);		// 12 looks like a usable default
	  pulse[selected_destination] = scratch;

	  task_info_1line(selected_destination);
	} else
	  if (selected_destination == ALL_PERIODICS) {
	    Serial.println("Select *one* task, 0");	// DADA ############
	    selected_destination=0;
	    //	    init_tasks();
	  } else
	    info_select_destination_with(false);
	break;

      case 'n':
	if (selected_destination < CLICK_PERIODICS) {		// periodics
	  get_now();

	  activate_pulse_synced(selected_destination, now, abs(sync));
	  check_maybe_do();				  // maybe do it *first*
	  task_info_1line(selected_destination);	  // *then* info ;)
	} else
	  if (selected_destination == ALL_PERIODICS) {
	    // DADA
	  } else
	    info_select_destination_with(false);
	break;

      // debugging entries: DADA ###############################################
      case 'd':				// hook for debugging

	break;

      case 'Y':				// hook for debugging
	Serial.print("rhthm 1, sync "); Serial.println(sync);
	init_tasks();
	init_rhythm_1(sync);
	break;

      case 'X':				// hook for debugging
	Serial.print("rhthm 2, sync "); Serial.println(sync);
	init_tasks();
	init_rhythm_2(sync);
	break;

      case 'C':				// hook for debugging
	Serial.print("rhthm 3, sync "); Serial.println(sync);
	init_tasks();
	init_rhythm_3(sync);
	break;

      case 'V':				// hook for debugging, clash with HARDWARE V
	Serial.print("rhthm 4, sync "); Serial.println(sync);
	init_tasks();
	init_rhythm_4(sync);
	break;

      case 'B':				// hook for debugging
	Serial.print("jiffles0, sync "); Serial.println(sync);
	init_tasks();
	setup_jiffles0(sync);
	break;

      // debugging entries: DADA ###############################################

      case 'c':
	if (selected_destination < CLICK_PERIODICS) {		// periodics
	  en_click(selected_destination);
	  task_info_1line(selected_destination);
	} else
	  if (selected_destination == ALL_PERIODICS) {
	    // DADA
	  } else
	    info_select_destination_with(false);
	break;

      case 'j':
	if (selected_destination < CLICK_PERIODICS) {		// periodics
	  en_jiffle_thrower(selected_destination, jiffletab0);
	  task_info_1line(selected_destination);
	} else
	  if (selected_destination == ALL_PERIODICS) {
	    // DADA
	  } else
	    info_select_destination_with(false);
	break;

      case 'f':
	if (selected_destination < CLICK_PERIODICS) {		// periodics
	  en_info(selected_destination);
	  task_info_1line(selected_destination);
	} else
	  if (selected_destination == ALL_PERIODICS) {
	    // DADA
	  } else
	    info_select_destination_with(false);
	break;

      case 'F':
	if (selected_destination < CLICK_PERIODICS) {		// periodics
	  en_INFO(selected_destination);
	  task_info_1line(selected_destination);
	} else
	  if (selected_destination == ALL_PERIODICS) {
	    // DADA
	  } else
	    info_select_destination_with(false);
	break;

      default:
	// maybe it's in a submenu?

#ifdef HARDWARE_menu				// quite a hack...
	if (hardware_menu_reaction(menu_input))
	  ;
#else						// quite a hack...
	if (false)
	  ;
#endif // submenu reactions
	else {
	  serial_print_progmem(unknownMenuInput); Serial.println(menu_input);
	  while (char_available() > 0) {
	    menu_input = get_char();
	    Serial.print(byte(menu_input));
	  }
	  Serial.println();
	break;
	}
      }

      if (!char_available())
	delay(WAITforSERIAL);
    }
  }
} // menu_serial_reaction()


#endif	// MENU_over_serial



/* **************************************************************** */
// setup:


void setup() {
  #ifdef USE_SERIAL
    Serial.begin(USE_SERIAL);
  #endif

//	#ifdef USE_LCD
//	  LCD.begin(LCD_COLs, LCD_ROWs);
//	  LCD_print_line_progmem(0, programName);
//	  LCD_print_line_progmem(1, version);
//	  LCD_print_line_progmem(3, freeRAM);
//	  LCD.print(get_free_RAM());
//	#endif


#ifdef USE_SERIAL
  #ifdef MENU_over_serial	// show message about menu
    display_serial_menu();
  #else
    serial_println_progmem(programLongName);
  #endif

  RAM_info(); Serial.println(); Serial.println();
#endif

//	#ifdef USE_LCD
//	  delay(3000);
//	  LCD.clear();
//	  LCD_print_line_progmem(TOP, programName);
//	#endif


#ifdef LED_PIN
  pinMode(LED_PIN, OUTPUT);
#endif

#ifdef CLICK_PERIODICS
  click_pin[0]= 2;		// configure PINs here
  click_pin[1]= 3;		// configure PINs here
  click_pin[2]= 4;		// configure PINs here
  click_pin[3]= 5;		// configure PINs here
  click_pin[4]= 6;		// configure PINs here

  init_click_pins();
#endif


  // time and tasks *must* get initialized before setting up tasks:
  init_time();
  init_tasks();

  // By design click tasks *HAVE* to be defined *BEFORE* any other tasks:
  // init_rhythm_1(1);
  // init_rhythm_2(1);
  // init_rhythm_3(1);
  init_rhythm_4(1);
  // setup_jiffles0(1);

/*
  // 1 to 2 second pattern straight (for easy to read task_info() output)
  // setup_task(task_do, new_flags|COUNTED, when, new_pulse);
  when=now;
  new_pulse.overflow=0;
  new_pulse.time=1000000L;
  setup_task(&task_info, ACTIVE, when, new_pulse);
  new_pulse.time=2000000L;
  setup_task(&task_info, ACTIVE, when, new_pulse);
*/

#ifdef MENU_over_serial
  alive_tasks_info_lines(); Serial.println();
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


