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



/* **************************************************************** */
// CONFIGURATION:

#define PERIODICS		8	// maximal number of tasks

#define LED_PIN			13

// to switch on serial #define USE_SERIAL <baud> 
//	#define USE_SERIAL	115200		// works fine here
#define USE_SERIAL	57600
//  #define USE_SERIAL	38400



/* **************************************************************** */
/* **************************************************************** */
// struct time

struct time {
  unsigned long time;
  unsigned int overflow;
};

/* **************************************************************** */
// variables for tasks in arrays[task]:

// ============>>> adapt init_task() IF YOU CHANGE SOMETHING HERE <<<============

unsigned char flags[PERIODICS];
// flag masks:
#define ACTIVE			1	// switches task on/off
#define COUNTED			2	// repeats int1 times, then vanishes
#define DO_NOT_DELETE	       16	// dummy to avoid being thrown out
#define CUSTOM_1	       32	// can be used by periodic_do()
#define CUSTOM_2	       64	// can be used by periodic_do()
#define CUSTOM_3	      128	// can be used by periodic_do()


unsigned int pulse_count[PERIODICS];	// counts how many times the task woke up
struct time pulse[PERIODICS];		// timer steps, overflows
struct time last[PERIODICS];		// convenient, but not really needed
struct time next[PERIODICS];		// next wake up time, overflow


// internal parameter:
unsigned int int1[PERIODICS];		// if COUNTED, gives number of executions
//					   else free for other internal use

// custom parameters[task]		//  comment/uncomment as appropriate:
					//  then *DO ADAPT init_task()* 

// ============>>> adapt init_task() IF YOU CHANGE SOMETHING HERE <<<============
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
// ============>>> adapt init_task() IF YOU CHANGE SOMETHING HERE <<<============



/* **************************************************************** */
#define ILLEGAL		-1

// global variables, task and time.
int task;

struct time now;
struct time last_now;

struct time global_next;
unsigned int global_next_count=0;
int global_next_tasks[PERIODICS];


/* **************************************************************** */
// init time:

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


/* **************************************************************** */
// init, reset or kill a task: 
void init_task(int task) {
  flags[task] = 0;
  periodic_do[task] = NULL;
  pulse_count[task] = ILLEGAL;
  int1[task] = 0;
  pulse[task].time = 0;
  last[task].time = 0;
  last[task].overflow = 0;
  next[task].time = 0;
  next[task].overflow = 0;
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
  if ((flags[task] & COUNTED) && ((pulse_count[task] +1) == int1[task] ))
    if (flags[task] & DO_NOT_DELETE)		//   yes: DO_NOT_DELETE?
      flags[task] &= ~ACTIVE;			//     yes: just deactivate
    else
      init_task(task);				//     no:  DELETE task

}


// *always* get time through get_now()
void get_now() {			// get time, set now.time and now.overflow
  now.time = micros();

  if (now.time < last_now.time)		// manage now.overflows
    now.overflow++;

  last_now = now;			// manage last_now
}


// determine when the next event[s] will happen:
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


// check if it's time to do something and do it:
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
    Serial.println("no free tasks");
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


int setup_counted_task(void (*task_do)(int), unsigned char new_flags, struct time when, struct time new_pulse, unsigned int count) {
  int task;

  task= setup_task(task_do, new_flags|COUNTED, when, new_pulse);
  int1[task]= count;
}


// not used?
void set_new_period(int task, struct time new_pulse) {
  pulse[task].time = new_pulse.time;
  pulse[task].overflow = new_pulse.overflow;
  next[task].time = last[task].time + pulse[task].time;
  next[task].overflow = last[task].overflow + pulse[task].overflow;
  if(next[task].time < last[task].time)
    next[task].overflow++;

  fix_global_next();	// it's saver to do that from here, but could be omitted.
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
  unsigned long realtime = micros();

#ifdef LED_PIN
  digitalWrite(LED_PIN,HIGH);		// blink the LED
#endif

  Serial.print("*** TASK INFO ");
  Serial.print(task);
  Serial.print("/");
  Serial.print((unsigned int) pulse_count[task]);

  Serial.print("\ttime/ovfl ");
  Serial.print(realtime);
  Serial.print("/");
  Serial.print(now.overflow);		// cheating a tiny little bit...

  Serial.print("    \tnext/ovfl ");
  Serial.print(next[task].time);
  Serial.print("/");
  Serial.print(next[task].overflow);

  Serial.print("   \tperiod/ovfl ");
  Serial.print((unsigned int) pulse[task].time);
  Serial.print("/");
  Serial.print(pulse[task].overflow);

  Serial.print("\n\t\t");		// start next line

  Serial.print("\tlast/ovfl ");
  Serial.print((unsigned int) last[task].time);
  Serial.print("/");
  Serial.print(last[task].overflow);

  Serial.print("   \tflags ");
  serial_print_BIN(flags[task], 8);
  //  Serial.print("\t");

  Serial.print("\n\t\t");		// start next line

  // no overflow in times yet ################################
  Serial.print("\texpected seconds ");
  Serial.print((float) now.time / 1000000.0, 3);
  Serial.print("s");

  Serial.print("\treal ");
  Serial.print((float) realtime / 1000000.0, 2);
  Serial.print("s");

  Serial.print("  \t\tperiod ");
  Serial.print((float) pulse[task].time / 1000000.0, 4);
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

  Serial.println("\nPERIODICS\n");

  init_time();
  init_tasks();


  struct time when;
  struct time new_pulse;

  // setup_task(task_do, new_flags|COUNTED, when, new_pulse);

/*
  // 1 to 2 second pattern straight (for easy to read inside_task_info() output)
  // setup_task(task_do, new_flags|COUNTED, when, new_pulse);
  when=now;
  new_pulse.overflow=0;
  new_pulse.time=1000000L;
  setup_task(&inside_task_info, ACTIVE, when, new_pulse);
  new_pulse.time=2000000L;
  setup_task(&inside_task_info, ACTIVE, when, new_pulse);
*/

  // /*
  // nice 1 to 3 to 4 to 5 pattern with phase offsets
  // setup_task(task_do, new_flags|COUNTED, when, new_pulse);
  const unsigned long scaling=500000L;

  when.time=now.time+(1*scaling)/2;
  when.overflow=now.overflow;
  new_pulse.time=1*scaling;
  new_pulse.overflow=0;
  setup_task(&click, ACTIVE, when, new_pulse);

  when.time=now.time+(3*scaling)/2;
  when.overflow=now.overflow;
  new_pulse.time=3*scaling;
  new_pulse.overflow=0;
  setup_task(&click, ACTIVE, when, new_pulse);

  when.time=now.time+(4*scaling)/2;
  when.overflow=now.overflow;
  new_pulse.time=4*scaling;
  new_pulse.overflow=0;
  setup_task(&click, ACTIVE, when, new_pulse);

  when.time=now.time+(5*scaling)/2;
  when.overflow=now.overflow;
  new_pulse.time=5*scaling;
  new_pulse.overflow=0;
  setup_task(&click, ACTIVE, when, new_pulse);
  // */

/*
  // testing periods longer then overflow:
  new_pulse.time=100;
  new_pulse.overflow=1;
  setup_task(&inside_task_info, ACTIVE, now, new_pulse);
  new_pulse.time=1;
  new_pulse.overflow=2;
  setup_task(&inside_task_info, ACTIVE, now, new_pulse);
*/


  fix_global_next();	// we *must* call that here
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
    Serial.println();
  }

  check_maybe_do();
}

/* **************************************************************** */
