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
// #define SERIAL_VERBOSE	0	// just bare minimum of feedbackack
#define SERIAL_VERBOSE	1	// more info on the tasks

// for testing timer overflow:
#define TIMER_TYPE	unsigned char
#define TIMER_SPEEDUP	50L
#define TIMER		(TIMER_TYPE) ((unsigned long) millis() / TIMER_SPEEDUP)

// is-it-time-now condition:

// well known *not* to work
// #define TASK_READY_CONDITION		next[task] <= now

//	stops at overflow
// #define TASK_READY_CONDITION		(now - last[task]) >= period[task]

//	stops at overflow
// #define TASK_READY_CONDITION		(last[task] + period[task]) <= now

// works!
// #define TASK_READY_CONDITION		((now >= last[task]) && (now >= next[task])) || ((now < last[task]) && now >= next[task])

#define TASK_READY_CONDITION		((next[task] >= last[task]) && (now >= next[task])) || ((now < last[task]) && now >= next[task])

/* **************************************************************** */
// CONFIGURATION:

#define USE_SERIAL		57600

#define PERIODICS		4	// maximal number of tasks

#define LED_PIN			13
/* **************************************************************** */
byte flags[PERIODICS];

// flag masks:
#define ACTIVE			1	// switches task on/off
// #define P_REPETIVE		2		// task is repetive
// #define P_ALTERNATE		4		// has 2 alternating periods
// #define P_ONE_SHOT		8		// do once, then vanish

unsigned int counter[PERIODICS];
TIMER_TYPE period[PERIODICS];
TIMER_TYPE last[PERIODICS];
TIMER_TYPE next[PERIODICS];

// pointers on  void something(int task)  functions:
void (*periodic_do[PERIODICS])(int);


/* **************************************************************** */
#define ILLEGAL		-1

int task;
TIMER_TYPE now;


void init_tasks() {
  for (int task=0; task<PERIODICS; task++) {
    flags[task] = 0;
    periodic_do[task] = NULL;
  }
}


void do_task(int task) {
  counter[task]++;				//      count

  if (periodic_do[task] != NULL) {		// there *is* something to do?
    (*periodic_do[task])(task);			//      do it
  } // we *did* do something
 
  // prepare future:
  last[task] = next[task];			// when it *should* have happened
  next[task] += period[task];			// when it should happen again

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
      if (TASK_READY_CONDITION) {		// yes, is it time?
	do_task(task);				// yes, do this task now
      }	// not the time yet
    } // active task
  } // task loop

  return 0;
}


int setup_task(void (*task_do)(int), byte new_flags, TIMER_TYPE when, TIMER_TYPE new_period) {
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
  flags[task] = new_flags;			//      initialize task
  period[task] = new_period;
  periodic_do[task] = task_do;			// payload
  next[task] = when;

  return task;
}

/* **************************************************************** */
// debugging:

/* **************************************************************** */
// click un a piezzo to hear result:
#define CLICK_PIN	12			// pin with a piezzo
void click(int task) {
  digitalWrite(CLICK_PIN, counter[task] & 1);
}

/* **************************************************************** */
void inside_task_info(int task) {
  if (task == 0)
    click(task);

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

  delay(2000);
  Serial.println("\nPERIODICS\n");

  now=TIMER;
  setup_task(&inside_task_info, ACTIVE, now, 5);

  setup_task(&inside_task_info, ACTIVE, now, 20);
  setup_task(&inside_task_info, ACTIVE, now, 40);
  setup_task(&inside_task_info, ACTIVE, now, 60);
}


/* **************************************************************** */
// overflow detection:
TIMER_TYPE last_now=TIMER;
int overflows;

void loop() {
  now=TIMER;

  if(now < last_now) {
    overflows++;
    Serial.println("\n====> OVERFLOW <====");
    if (overflows > 1) {
      Serial.println("stopped");
      while (true) ;
    }
  }
  last_now = now;

  check_maybe_do();
}

/* **************************************************************** */
