/* **************************************************************** */
// periodics
/* **************************************************************** */


/* **************************************************************** */
/*
   Periodically do multiple independent tasks.
   This is a very early experimental test version
   used to test timer overflow strategies.
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
#define SERIAL_VERBOSE	0	// just bare minimum of feedbackack
// #define SERIAL_VERBOSE	1	// more info on the tasks


/* **************************************************************** */
// for testing timer overflow:
#define TIMER_TYPE	unsigned char
#define TIMER_SPEEDUP	20L
#define TIMER		(TIMER_TYPE) ((unsigned long) millis() / TIMER_SPEEDUP)

// The is-it-time-now condition:

// Well known *not* to work
// #define TIME_READY_CONDITION		next[task] <= now

//	stops at overflow
// #define TIME_READY_CONDITION		(now - last[task]) >= period[task]

//	stops at overflow
// #define TIME_READY_CONDITION		(last[task] + period[task]) <= now

// works!
// #define TIME_READY_CONDITION		((now >= last[task]) && (now >= next[task])) || ((now < last[task]) && now >= next[task])

#define TIME_READY_CONDITION		((next[task] >= last[task]) && (now >= next[task])) || ((now < last[task]) && now >= next[task])



/* **************************************************************** */
byte flags[PERIODICS];

// flag masks:
#define ACTIVE			1	// switches task on/off
#define COUNTED			2	// repeats int1 times, then vanishes
// #define P_ALTERNATE		4		// has 2 alternating periods
#define DO_NOT_DELETE	       16	// dummy to avoid being thrown out
#define CUSTOM_1	       32	// can be used by periodic_do()
#define CUSTOM_2	       64	// can be used by periodic_do()
#define CUSTOM_3	      128	// can be used by periodic_do()


unsigned int counter[PERIODICS];	// counts how many times the task woke up
unsigned int int1[PERIODICS];		// if COUNTED, gives number of executions
//					   (else free for any other internal use)
TIMER_TYPE period[PERIODICS];
TIMER_TYPE last[PERIODICS];
TIMER_TYPE next[PERIODICS];

// custom parameters[task]		//  comment/uncomment as appropriate:
					//  then *DO ADAPT init_task()* 
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



/* **************************************************************** */
#define ILLEGAL		-1

int task;
TIMER_TYPE now;


// init, reset or kill a task: 
void init_task(int task) {
  flags[task] = 0;
  periodic_do[task] = NULL;
  counter[task] = 0;
  int1[task] = 0;
  period[task] = 0;
  last[task] = 0;
  next[task] = 0;
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


void do_task(int task) {
  counter[task]++;						//      count

  if (periodic_do[task] != NULL) {				// there *is* something to do?
    (*periodic_do[task])(task);					//      do it
  } // we *did* do something
 
  // prepare future:
  last[task] = next[task];					// when it *should* have happened
  next[task] += period[task];					// when it should happen again

  if ((flags[task] & COUNTED) && (counter[task] == int1[task]))	// COUNTED task && end reached?
    if (flags[task] & DO_NOT_DELETE)				//  yes: DO_NOT_DELETE?
      flags[task] &= ~ACTIVE;					//       yes: just deactivate
    else
      init_task(task);						//       no:  delete task

  // fix_global_next();			// planed soon...

#if (SERIAL_VERBOSE > 0) 
  Serial.print("\n\t\tAFTER   "); Serial.print(task); Serial.print(" / "); Serial.print((unsigned int) counter[task]);
  Serial.print("\tlast "); Serial.print((unsigned int) last[task]);
  Serial.print("  \tnext "); Serial.println((unsigned int) next[task]);
#endif
}


int check_maybe_do() {
  now=TIMER;

  for (task=0; task<PERIODICS; task++) {	// check all tasks once
    if (flags[task] & ACTIVE) {			// task active?
      if (TIME_READY_CONDITION) {		// yes, is it time?
	do_task(task);				// yes, do this task now
      }	// not the time yet
    } // active task
  } // task loop

  return 0;
}


int setup_task(void (*task_do)(int), byte new_flags, TIMER_TYPE when, TIMER_TYPE new_period, unsigned int new_int1) {
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
  next[task] = when;				// next execution time
  period[task] = new_period;			// repetition period
  int1[task] = new_int1;;			// internal parameter

  // fix_global_next();			// planed soon...

  return task;
}


void set_new_period(int task, TIMER_TYPE new_period) {
  period[task] = new_period;
  next[task] = last[task] + period[task];
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


void inside_task_info(int task) {
#ifdef SERIAL_VERBOSE
  digitalWrite(LED_PIN,HIGH);
  #if (SERIAL_VERBOSE == 0)
    Serial.print("\ntask do "); Serial.print(task); Serial.print("/"); Serial.print((unsigned int) counter[task]);
  #else
    // Serial.print("\nat RT "); Serial.println(millis() / TIMER_SPEEDUP); 
    Serial.print("\ntime  "); Serial.print((unsigned int) now);
    Serial.print("  \tTASK DO "); Serial.print(task); Serial.print(" / "); Serial.print((unsigned int) counter[task]);
    Serial.print("\tlast "); Serial.print((unsigned int) last[task]);
    Serial.print("  \tnext "); Serial.print((unsigned int) next[task]);
    Serial.print("  \tperiod "); Serial.print((unsigned int) period[task]);
    Serial.print("  \tcounter "); Serial.print((unsigned int) counter[task]);
  #endif
  digitalWrite(LED_PIN,LOW);
#endif
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

  now=TIMER;

  /*
  // first commits test case:
  setup_task(&inside_task_info, ACTIVE, now, 5,  0);
  setup_task(&inside_task_info, ACTIVE, now, 20, 0);
  setup_task(&inside_task_info, ACTIVE, now, 40, 0);
  setup_task(&inside_task_info, ACTIVE, now, 60, 0);
  */

  /*
  // 3 to 5 pattern with phase offset:
  setup_task(&click, ACTIVE, now, 30, 0);
  setup_task(&click, ACTIVE, now+15, 50, 0);
  */

  // testing COUNTED tasks:
  setup_task(&click, ACTIVE, now, 30, 0);			// test an ordinary task
  setup_task(&inside_task_info, ACTIVE|COUNTED, now+10, 10, 3);	// test a three shot task
  setup_task(&inside_task_info, ACTIVE|COUNTED, now+25, 0, 1);	// test a one shot task

}



/* **************************************************************** */
// main loop:


// overflow detection:
TIMER_TYPE last_now=TIMER;
int overflows;


void loop() {
  now=TIMER;

  if(now < last_now) {
    overflows++;
    Serial.println("\n====> OVERFLOW <====");
    if (overflows > 5) {
      Serial.println("\nstopped");
      while (true) ;
    }
  }
  last_now = now;

  check_maybe_do();
}


/* **************************************************************** */
