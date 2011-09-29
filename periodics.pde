/* **************************************************************** */
// periodics
/* **************************************************************** */


/* **************************************************************** */
/*
   Periodically do multiple independent tasks.
   This is a very early experimental test version
   used to test timer overflow strategies.

   New logic version to determine when to wake up tasks.
   Saving times along with an overflow count makes everything
   simple and flexible.

   This version calculates a global next event once whenever the next
   event could possible have changed. See fix_global_next().

*/
/* **************************************************************** */



/* **************************************************************** */
// CONFIGURATION:

#define PERIODICS		8	// maximal number of tasks

#define LED_PIN			13

// to use serial define USE_SERIAL as serial speed: 
//  #define USE_SERIAL	115200
#define USE_SERIAL	57600
//  #define USE_SERIAL	38400



/* **************************************************************** */
// for testing timer overflow:
#define TIMER_TYPE	unsigned char

#define TIMER_SLOWDOWN	10L	// gives a hundred per second

#define TIMER		(TIMER_TYPE) ((unsigned long) millis() / TIMER_SLOWDOWN)
#define OVERFLOW_TYPE	unsigned int

// The is-it-time-now-condition:
#define TIME_READY_CONDITION		((now >= next[task]) && (overflow == next_ovrfl[task]))


/* **************************************************************** */
// variables for tasks in arrays[task]:

// ================>>> adapt init_task() IF YOU CHANGE SOMETHING HERE <<<================

unsigned char flags[PERIODICS];
// flag masks:
#define ACTIVE			1	// switches task on/off
#define COUNTED			2	// repeats int1 times, then vanishes
#define DO_NOT_DELETE	       16	// dummy to avoid being thrown out
#define CUSTOM_1	       32	// can be used by periodic_do()
#define CUSTOM_2	       64	// can be used by periodic_do()
#define CUSTOM_3	      128	// can be used by periodic_do()


unsigned int pulse_count[PERIODICS];	// counts how many times the task woke up
TIMER_TYPE  pulse_period[PERIODICS];	// timer steps
OVERFLOW_TYPE pulse_ovrfl[PERIODICS];	// overflow of pulse_period (for very long periods)

TIMER_TYPE last[PERIODICS];		// convenient, but not really needed
OVERFLOW_TYPE last_ovrfl[PERIODICS];	// same

TIMER_TYPE next[PERIODICS];		// next wake up time
OVERFLOW_TYPE next_ovrfl[PERIODICS];	// overflow of next wake up

// internal parameter:
unsigned int int1[PERIODICS];		// if COUNTED, gives number of executions
//					   else free for other internal use

// custom parameters[task]		//  comment/uncomment as appropriate:
					//  then *DO ADAPT init_task()* 

// ================>>> adapt init_task() IF YOU CHANGE SOMETHING HERE <<<================
int parameter_1[PERIODICS];			//  can be used by periodic_do()
int parameter_2[PERIODICS];			//  can be used by periodic_do()
// int parameter_3[PERIODICS];			//  can be used by periodic_do()
// int parameter_4[PERIODICS];			//  can be used by periodic_do()
// unsigned long ulong_parameter_1[PERIODICS];	//  can be used by periodic_do()
// unsigned long ulong_parameter_2[PERIODICS];	//  can be used by periodic_do()
// unsigned long ulong_parameter_3[PERIODICS];	//  can be used by periodic_do()
// unsigned long ulong_parameter_4[PERIODICS];	//  can be used by periodic_do()
char char_parameter_1[PERIODICS];		//  can be used by periodic_do()
char char_parameter_2[PERIODICS];		//  can be used by periodic_do()
// char char_parameter_3[PERIODICS];		//  can be used by periodic_do()
// char char_parameter_4[PERIODICS];		//  can be used by periodic_do()

// pointers on  void something(int task)  functions:
void (*periodic_do[PERIODICS])(int);
// ================>>> adapt init_task() IF YOU CHANGE SOMETHING HERE <<<================



/* **************************************************************** */
#define ILLEGAL		-1

// global variables:
int task;
TIMER_TYPE now=0, last_now=~0;
OVERFLOW_TYPE overflow=0;

TIMER_TYPE global_next;
OVERFLOW_TYPE global_next_ovfl=~0;
unsigned int global_next_count=0;
int global_next_tasks[PERIODICS];

/* **************************************************************** */

// init, reset or kill a task: 
void init_task(int task) {
  flags[task] = 0;
  periodic_do[task] = NULL;
  pulse_count[task] = ILLEGAL;
  int1[task] = 0;
  pulse_period[task] = 0;
  last[task] = 0;
  last_ovrfl[task] = 0;
  next[task] = 0;
  next_ovrfl[task] = 0;
  parameter_1[task] = 0;
  parameter_2[task] = 0;
  // parameter_3[task] = 0;
  // parameter_4[task] = 0;

  // ulong_parameter_1[task] = 0L;
  // ulong_parameter_2[task] = 0L;
  // ulong_parameter_3[task] = 0L;
  // ulong_parameter_4[task] = 0L;
  char_parameter_1[task] = 0;
  char_parameter_2[task] = 0;
  // char_parameter_3[task] = 0;
  // char_parameter_4[task] = 0;
}


void init_tasks() {
  for (int task=0; task<PERIODICS; task++) {
    init_task(task);
  }
}


void wake_task(int task) {
  pulse_count[task]++;					//      count

  if (periodic_do[task] != NULL) {				// there *is* something to do?
    (*periodic_do[task])(task);					//      do it
  } // we *did* do something
 
  // prepare future:
  last[task] = next[task];						// when it *should* have happened
  last_ovrfl[task] = next_ovrfl[task];
  next[task] += pulse_period[task];					// when it should happen again
  next_ovrfl[task] += pulse_ovrfl[task];

  if (last[task] > next[task])
    next_ovrfl[task]++;

  if ((flags[task] & COUNTED) && ((pulse_count[task] +1) == int1[task] ))	// COUNTED task && end reached?
    if (flags[task] & DO_NOT_DELETE)						//  yes: DO_NOT_DELETE?
      flags[task] &= ~ACTIVE;							//       yes: just deactivate
    else
      init_task(task);								//       no:  DELETE task

  fix_global_next();
}


// *always* get time through get_now()
TIMER_TYPE get_now() {		// get time and set overflow
  now = TIMER;

  if (now < last_now)		// manage overflows
    overflow++;

  last_now = now;

  return now;
}

void fix_global_next() {
  // we work directly on the global variables here:
  global_next_ovfl=~0;	// ILLEGAL value
  global_next_count=0;

  for (task=0; task<PERIODICS; task++) {	// check all tasks
    if (flags[task] & ACTIVE) {			// task active?
      if (next_ovrfl[task] < global_next_ovfl) {	// yes: earlier overflow?
	global_next = next[task];			//   yes: reset search
	global_next_ovfl = next_ovrfl[task];
	global_next_tasks[0] = task;
	global_next_count = 1;
      } else {					// same (or later) overflow:
	if (next_ovrfl[task] == global_next_ovfl) {	// same overflow?
	  if (next[task] < global_next) {		//   yes: new next?
	    global_next = next[task];			//     yes: reset search
	    global_next_tasks[0] = task;
	    global_next_count = 1;
	  } else					// (still *same* overflow)
	    if (next[task] == global_next)		//    *same* next?
	      global_next_tasks[global_next_count++]=task; //  yes: save task, count
	}
	// (*later* overflows are always later)
      }
    } // active?
  } // task loop
}


void check_maybe_do() {
  now = get_now();	// sets overflow too

  if (global_next_ovfl == overflow) {		// current overflow period?
    if (global_next <= now)			//   yes: is it time?
      for (int i=global_next_count; i; )	//     yes:
	wake_task(global_next_tasks[--i]);	//     wake next tasks up
  } else					// (earlier or later overflow)
    if (global_next_ovfl < overflow)		// earlier overflow period?
      for (int i=global_next_count; i; )	//     yes, we're late
	wake_task(global_next_tasks[--i]);	//     wake next tasks up
}


// Add a time interval to a given start time:
// 	Returns and sets other_time,
//      *and* sets other_ovrfl
TIMER_TYPE other_time;			// might be convenient sometimes. Not really needed.
OVERFLOW_TYPE other_ovrfl;		// both variables set by other_time(...)
TIMER_TYPE set_other_time(TIMER_TYPE this_time, OVERFLOW_TYPE this_overflow, TIMER_TYPE interval , OVERFLOW_TYPE interval_ovrfl) {
  TIMER_TYPE start_time=this_time;

  other_time = this_time + interval;

  other_ovrfl = interval_ovrfl;
  if (other_time < start_time)
    other_ovrfl++;

  return other_time;
}


//	// Multiply a given time interval by a (small) positive integer factor.
//	// 	Returns and sets multiple, sets multiple_ovrfl
//	// 	These global variables are shared with divided_interval(...)
//	TIMER_TYPE multiple;
//	OVERFLOW_TYPE multiple_ovrfl;
//	TIMER_TYPE multiple_interval(TIMER_TYPE interval, OVERFLOW_TYPE interval_ovrfl, unsigned int factor) {
//	  TIMER_TYPE scratch;
//	
//	  multiple=0;
//	  multiple_ovrfl = interval_ovrfl * factor;
//	
//	  for (; factor>0; factor--) {
//	    scratch = multiple;
//	    multiple += interval;
//	    if (multiple < scratch)
//	      multiple_ovrfl++;
//	  }
//	
//	    return multiple;
//	}
//	
//	
//	// not implemented yet:
//	TIMER_TYPE divided_interval(TIMER_TYPE interval, OVERFLOW_TYPE interval_ovrfl, unsigned int divisor);



int setup_task(void (*task_do)(int), unsigned char new_flags, TIMER_TYPE when, OVERFLOW_TYPE when_ovrfl, TIMER_TYPE new_pulse_period, OVERFLOW_TYPE new_pulse_ovrfl) {
  int task;

  if (new_flags == 0)				// illegal new_flags parameter
    return ILLEGAL;				//   should not happen

  for (task=0; task<PERIODICS; task++) {	// search first free task
    if (flags[task] == 0)			// flags==0 means empty task
      break;					//   found one
  }
  if (task == PERIODICS)			// no task free :(
    return ILLEGAL;				// ERROR

  // initiaize new task				// yes, found a free task
  flags[task] = new_flags;			// initialize task
  periodic_do[task] = task_do;			// payload
  next[task] = when;				// next wake up time
  next_ovrfl[task] = when_ovrfl;
  pulse_period[task] = new_pulse_period;
  pulse_ovrfl[task] = new_pulse_ovrfl;

  fix_global_next();

  return task;
}


int setup_counted_task(void (*task_do)(int), unsigned char new_flags, TIMER_TYPE when, OVERFLOW_TYPE when_ovrfl, TIMER_TYPE new_pulse_period, OVERFLOW_TYPE new_pulse_ovrfl, unsigned int count) {
  int task;

  task= setup_task(task_do, new_flags|COUNTED, when, when_ovrfl, new_pulse_period, new_pulse_ovrfl);
  int1[task]= count;
}


void set_new_period(int task, TIMER_TYPE new_pulse_period, OVERFLOW_TYPE new_pulse_oveflow) {
  pulse_period[task] = new_pulse_period;
  pulse_ovrfl[task] = new_pulse_oveflow;
  next[task] = last[task] + pulse_period[task];
  next_ovrfl[task] = last_ovrfl[task] + pulse_ovrfl[task];
  fix_global_next();
}


/* **************************************************************** */
// debugging:


// click on a piezzo to hear result:
#define CLICK_PIN	6			// pin with a piezzo
unsigned long clicks=0;
void click(int task) {
  digitalWrite(CLICK_PIN, ++clicks & 1);
}


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
  Serial.print(" ");
}

// task info as paylod for tasks:
// blink the LED
void inside_task_info(int task) {
  unsigned long realtime = millis();

#ifdef LED_PIN
  digitalWrite(LED_PIN,HIGH);		// blink the LED
#endif

  Serial.print("*** TASK INFO ");
  Serial.print(task);
  Serial.print("/");
  Serial.print((unsigned int) pulse_count[task]);

  Serial.print("\ttime/ovfl ");
  Serial.print((int) TIMER);
  Serial.print("/");
  Serial.print((int) overflow);

  Serial.print("    \tnext/ovfl ");
  Serial.print((int) next[task]);
  Serial.print("/");
  Serial.print((OVERFLOW_TYPE) next_ovrfl[task]);

  Serial.print("   \tperiod/ovfl ");
  Serial.print((unsigned int) pulse_period[task]);
  Serial.print("/");
  Serial.print((OVERFLOW_TYPE) pulse_ovrfl[task]);

  Serial.print("\n\t\t");		// start next line

  Serial.print("\tlast/ovfl ");
  Serial.print((unsigned int) last[task]);
  Serial.print("/");
  Serial.print((OVERFLOW_TYPE) last_ovrfl[task]);

  Serial.print("   \tflags ");
  serial_print_BIN(flags[task], 8);
  //  Serial.print("\t");

  Serial.print("\n\t\t");		// start next line

  // no overflow in times yet ################################
  Serial.print("\texpected seconds ");
  Serial.print((float) now / 1000.0, 4);
  Serial.print("s");

  Serial.print("\treal ");
  Serial.print((float) millis() / 1000.0, 2);
  Serial.print("s");

  Serial.print("  \tperiod ");
  Serial.print((float) pulse_period[task] * (float) TIMER_SLOWDOWN / 1000.0, 4);
  Serial.print("s");

  Serial.print("\n\n");			// traling empty line

#ifdef LED_PIN
  digitalWrite(LED_PIN,LOW);
#endif
}


void click_n_info(int task) {
  click(task);
  inside_task_info(task);
}


/* **************************************************************** */
// setup:

void setup() {
  #ifdef USE_SERIAL
    Serial.begin(USE_SERIAL);
  #endif

#ifdef CLICK_PIN
  pinMode(CLICK_PIN, OUTPUT);
#endif

#ifdef LED_PIN
  pinMode(LED_PIN, OUTPUT);
#endif

  init_tasks();

  Serial.println("\nPERIODICS\n");

  now = get_now();
  overflow = 0;		// start with overflow = 0

  // setup_task(task_do, new_flags|COUNTED, when, when_ovrfl, new_pulse_period, new_pulse_ovrfl);

  /*
  // 1 to 2 second pattern straight (for easy to read inside_task_info() output)
  // setup_task(task_do, new_flags|COUNTED, when, when_ovrfl, new_pulse_period, new_pulse_ovrfl);
  setup_task(&inside_task_info, ACTIVE, now, overflow, 100, 0);
  setup_task(&inside_task_info, ACTIVE, now, overflow, 200, 0);
  */

  // nice 1 to 3 (to 4) to 5 second pattern with phase offsets
  // setup_task(task_do, new_flags|COUNTED, when, when_ovrfl, new_pulse_period, new_pulse_ovrfl);
  setup_task(&click, ACTIVE, now+100/2, overflow, 100, 0);
  setup_task(&click, ACTIVE, now+300/2, overflow, 300, 0);
  //  setup_task(&click, ACTIVE, now+400/2, overflow, 400, 0);
  setup_task(&click, ACTIVE, now+500/2, overflow, 500, 0);


  /*
  // testing periods longer then overflow:
  setup_task(&inside_task_info, ACTIVE, now, overflow, 100, 1);
  setup_task(&inside_task_info, ACTIVE, now, overflow, 1, 2);
  */

  /*
  // 3 to 5 click_n_info pattern with phase offset:
  int scale=50;
  setup_task(&click, ACTIVE, now+3*scale/2, overflow, 3*scale, 0);
  setup_task(&click, ACTIVE, now+5*scale/2, overflow, 5*scale, 0);
  // testing COUNTED tasks:
  setup_counted_task(&click, ACTIVE|COUNTED, now, 13, scale/8, 0, 16);
  setup_counted_task(&click, ACTIVE|COUNTED, now, 17, scale/32, 0, 32);
  setup_counted_task(&click, ACTIVE|COUNTED, now, 21, scale/4, 0, 64);
  setup_counted_task(&click, ACTIVE|COUNTED, now, 23, scale/2, 0, 64);
  setup_counted_task(&click, ACTIVE|COUNTED, now, 27, scale, 0, 16);
  */

}


/* **************************************************************** */
// main loop:

// overflow detection:
OVERFLOW_TYPE last_overflow_displayed=0;


void loop() {
  now= get_now();

  if(overflow != last_overflow_displayed) {

    last_overflow_displayed = overflow;
    Serial.print("====> OVERFLOW <====  ");
    Serial.println((int) overflow);
    Serial.println();

//	    if (overflow > 5) {
//	      Serial.println("\n(stopped)");
//	      while (true) ;
//	    }
  }

  check_maybe_do();
}

/* **************************************************************** */
