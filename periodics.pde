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
*/
/* **************************************************************** */



/* **************************************************************** */
// CONFIGURATION:

#define PERIODICS		4	// maximal number of tasks

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
#define TIME_READY_CONDITION		((now >= next[task]) && (overflow == next_overflow[task]))


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


unsigned int woke_up_count[PERIODICS];	// counts how many times the task woke up
TIMER_TYPE wake_up_period[PERIODICS];
OVERFLOW_TYPE wake_up_overflow[PERIODICS]; // overflow of period (for very long periods)

TIMER_TYPE last[PERIODICS];		// convenient, but not really needed
OVERFLOW_TYPE last_overflow[PERIODICS];	// same

TIMER_TYPE next[PERIODICS];		// next wake up time
OVERFLOW_TYPE next_overflow[PERIODICS];	// overflow of next wake up

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


/* **************************************************************** */

// init, reset or kill a task: 
void init_task(int task) {
  flags[task] = 0;
  periodic_do[task] = NULL;
  woke_up_count[task] = ILLEGAL;
  int1[task] = 0;
  wake_up_period[task] = 0;
  last[task] = 0;
  last_overflow[task] = 0;
  next[task] = 0;
  next_overflow[task] = 0;
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
  woke_up_count[task]++;					//      count

  if (periodic_do[task] != NULL) {				// there *is* something to do?
    (*periodic_do[task])(task);					//      do it
  } // we *did* do something
 
  // prepare future:
  last[task] = next[task];						// when it *should* have happened
  last_overflow[task] = next_overflow[task];
  next[task] += wake_up_period[task];					// when it should happen again
  next_overflow[task] += wake_up_overflow[task];

  if (last[task] > next[task])
    next_overflow[task]++;

  if ((flags[task] & COUNTED) && (woke_up_count[task] == int1[task]))	// COUNTED task && end reached?
    if (flags[task] & DO_NOT_DELETE)					//  yes: DO_NOT_DELETE?
      flags[task] &= ~ACTIVE;						//       yes: just deactivate
    else
      init_task(task);							//       no:  delete task

  // fix_global_next();			// planed soon...
}


// always get time through here
TIMER_TYPE get_now() {		// get time and set overflow
  now = TIMER;

  if (now < last_now)		// manage overflows
    overflow++;

  last_now = now;

  return now;
}


void check_maybe_do() {
  now = get_now();

  for (task=0; task<PERIODICS; task++) {				// check all tasks once
    if (flags[task] & ACTIVE) {						// task active?
      if ((now >= next[task]) && (overflow == next_overflow[task])) {	//   yes, is it time?
	wake_task(task);						//     yes, wake task up
      }
    }
  }
}


int setup_task(void (*task_do)(int), unsigned char new_flags, TIMER_TYPE when, OVERFLOW_TYPE when_overflow, TIMER_TYPE new_wake_up_period, OVERFLOW_TYPE new_wake_up_overflow) {
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
  next_overflow[task] = when_overflow;
  wake_up_period[task] = new_wake_up_period;
  wake_up_overflow[task] = new_wake_up_overflow;

  // fix_global_next();			// planed soon...

  return task;
}


int setup_counted_task(void (*task_do)(int), unsigned char new_flags, TIMER_TYPE when, OVERFLOW_TYPE when_overflow, TIMER_TYPE new_wake_up_period, OVERFLOW_TYPE new_wake_up_overflow, unsigned int count) {
  int task;

  task= setup_task(task_do, new_flags|COUNTED, when, when_overflow, new_wake_up_period, new_wake_up_overflow);
  int1[task]= count;
}


void set_new_period(int task, TIMER_TYPE new_wake_up_period, OVERFLOW_TYPE new_wake_up_oveflow) {
  wake_up_period[task] = new_wake_up_period;
  wake_up_overflow[task] = new_wake_up_oveflow;
  next[task] = last[task] + wake_up_period[task];
  next_overflow[task] = last_overflow[task] + wake_up_overflow[task];
  // fix_global_next();			// planed soon...
}


/* **************************************************************** */
// debugging:


// click on a piezzo to hear result:
#define CLICK_PIN	12			// pin with a piezzo
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
void inside_task_info(int task) {
  digitalWrite(LED_PIN,HIGH);

  Serial.print("*** TASK INFO ");
  Serial.print(task);
  Serial.print("/");
  Serial.print((unsigned int) woke_up_count[task]);

  Serial.print("\ttime/ovfl ");
  Serial.print((int) TIMER);
  Serial.print("/");
  Serial.print((int) overflow);

  Serial.print("    \tnext/ovfl ");
  Serial.print((int) next[task]);
  Serial.print("/");
  Serial.print((OVERFLOW_TYPE) next_overflow[task]);

  Serial.print("   \tperiod/ovfl ");
  Serial.print((unsigned int) wake_up_period[task]);
  Serial.print("/");
  Serial.print((OVERFLOW_TYPE) wake_up_overflow[task]);

  Serial.print("\n\t\t");		// start next line

  Serial.print("\tlast/ovfl ");
  Serial.print((unsigned int) last[task]);
  Serial.print("/");
  Serial.print((OVERFLOW_TYPE) last_overflow[task]);

  Serial.print("   \tflags ");
  serial_print_BIN(flags[task], 8);
// Serial.print((int) flags[task], BIN);
  Serial.print("\t");

  // no overflow in times yet ################################
  Serial.print("\treal seconds ");
  Serial.print((float) millis() / 1000.0, 2);

  Serial.print("  \tperiod S ");
  Serial.print((float) wake_up_period[task] * (float) TIMER_SLOWDOWN / 1000.0, 4);

  Serial.print("\n\n");			// traling empty line

  digitalWrite(LED_PIN,LOW);
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

  // setup_task(task_do, new_flags|COUNTED, when, when_overflow, new_wake_up_period, new_wake_up_overflow);

  /*
  // 3 to 5 pattern with phase offset:
  int slowdown=20;
  setup_task(&inside_task_info, ACTIVE, now,             overflow, 3*slowdown, 0);
  setup_task(&inside_task_info, ACTIVE, now+5*slowdown/2, overflow, 5*slowdown, 0);
  */

  // 1 to 2 second pattern straight (for easy to read inside_task_info() output)
  // setup_task(task_do, new_flags|COUNTED, when, when_overflow, new_wake_up_period, new_wake_up_overflow);
  setup_task(&inside_task_info, ACTIVE, now, overflow, 100, 0);
  setup_task(&inside_task_info, ACTIVE, now, overflow, 200, 0);

  /*
  // testing COUNTED tasks:
  setup_counted_task(&click, ACTIVE, now, 30, 0);			// test an ordinary task
  setup_counted_task(&inside_task_info, ACTIVE|COUNTED, now+10, 10, 3);	// test a three shot task
  setup_counted_task(&inside_task_info, ACTIVE|COUNTED, now+25, 0, 1);	// test a one shot task
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

    if (overflow > 5) {
      Serial.println("\n(stopped)");
      while (true) ;
    }
  }

  check_maybe_do();
}

/* **************************************************************** */
