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
#define ILLEGAL	-1



/* **************************************************************** */
// testing timer overflow:

#define TIMER_TYPE	unsigned long
#define TIMER		micros()

// for testing timer overflow:
//	#define TIMER_TYPE	unsigned char
//	#define TIMER_SPEEDUP	20L
//	#define TIMER		(TIMER_TYPE) ((unsigned long) millis() / TIMER_SPEEDUP)


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
// CONFIGURATION:

#define PERIODICS		15	// maximal number of tasks
					// plenty of free tasks for jiffles ;)

#define LED_PIN			13

// to use serial define USE_SERIAL as serial speed: 
//  #define USE_SERIAL	115200
#define USE_SERIAL	57600
//  #define USE_SERIAL	38400

//  #define SERIAL_VERBOSE	0	// just bare minimum of feedbackack
//  #define SERIAL_VERBOSE	1	// more info on the tasks

#ifdef USE_SERIAL		// simple menus over serial line?
  // menu basics:
  #define MENU_over_serial	// do we use a serial menu?

  // simple menu to access arduino hardware:
  #define HARDWARE_menu		// menu interface to hardware configuration
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
unsigned long ulong_parameter_1[PERIODICS];	//  can be used by periodic_do()
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

  ulong_parameter_1[task] = 0L;
  // ulong_parameter_2[task] = 0L;
  // ulong_parameter_3[task] = 0L;
  // ulong_parameter_4[task] = 0L;
  char_parameter_1[task] = 0;
  char_parameter_2[task] = 0;
  // char_parameter_3[task] = 0;
  // char_parameter_4[task] = 0;

  // fix_global_next();			// planed soon...
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

void inside_task_info(int task) {
#ifdef SERIAL_VERBOSE
  digitalWrite(LED_PIN,HIGH);
  #if (SERIAL_VERBOSE == 0)
    Serial.print("\ntask do "); Serial.print(task); Serial.print("/"); Serial.print((unsigned int) counter[task]);
  #else
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
#include <avr/pgmspace.h>

const unsigned char programName[] PROGMEM = "PERIODICS";
const unsigned char programLongName[] PROGMEM = "*** Play with PERIODICS v0.0 ***";
const unsigned char version[] PROGMEM = "version 0.0";

const unsigned char tab_[] PROGMEM = "\t";


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
    Serial.print(c, BYTE);
}


void serial_println_progmem(const unsigned char *str) {
  serial_print_progmem(str);
  Serial.println("");
}
#endif


/* **************************************************************** */
// some little things to play with:


// clicks on piezzos to *hear* the result:

// *do* change 'case' statement in menu_serial_reaction() if you change this.
#ifndef CLICK_PERIODICS				// number of click frequencies
  #define CLICK_PERIODICS		5       // default number of click frequencies
#endif



void init_click_tasks() {
  for (int task=0; task<CLICK_PERIODICS; task++) {
    init_task(task);
  }
}



void click(int task) {			// can be called from a task
  digitalWrite(char_parameter_1[task], counter[task] & 1);
}


unsigned char click_task[CLICK_PERIODICS];
unsigned char click_pin[CLICK_PERIODICS]= {12, 10, 9, 8};	// configure PINs here



// playing with rhythms:
unsigned long time_unit = 100000;		// scaling timer to 10 beats/s 


// some default rhythms:
void init_rhythm_1(int sync) { 
  // By design click tasks *HAVE* to be defined *BEFORE* any other tasks:
  int task=0;
  TIMER_TYPE now = TIMER;

  init_click_tasks();

  // 2
  click_task[task] = setup_task(&click, ACTIVE, now + sync*6L*time_unit, (TIMER_TYPE) 12L*time_unit, 0);
  char_parameter_1[task] = click_pin[task];
  pinMode(click_pin[task++], OUTPUT);

  // 3
  click_task[task] = setup_task(&click, ACTIVE, now + sync*9L*time_unit, (TIMER_TYPE) 18L*time_unit, 0);
  char_parameter_1[task] = click_pin[task];
  pinMode(click_pin[task++], OUTPUT);

  // 4
  click_task[task] = setup_task(&click, ACTIVE, now + sync*12L*time_unit, (TIMER_TYPE) 24L*time_unit, 0);
  char_parameter_1[task] = click_pin[task];
  pinMode(click_pin[task++], OUTPUT);

  // 5
  click_task[task] = setup_task(&click, ACTIVE, now + sync*15L*time_unit, (TIMER_TYPE) 30L*time_unit, 0);
  char_parameter_1[task] = click_pin[task];
  pinMode(click_pin[task++], OUTPUT);

  // 2*2*3*5
  click_task[task] = setup_task(&click, ACTIVE, now + sync*6L*4L*4L*time_unit, (TIMER_TYPE) 6L*2L*2L*3L*5L*time_unit, 0);
  char_parameter_1[task] = click_pin[task];
  pinMode(click_pin[task++], OUTPUT);
}


// frequencies ratio 1, 4, 6, 8, 10
void init_rhythm_2(int sync) { 
  // By design click tasks *HAVE* to be defined *BEFORE* any other tasks:
  int task=0;
  TIMER_TYPE now = TIMER;

  unsigned long divider;
  TIMER_TYPE base=60L;

  init_click_tasks();

  for (divider=4; divider<12 ; divider += 2) {
    click_task[task] =
      setup_task(&click, ACTIVE|DO_NOT_DELETE, now + sync*base*time_unit/divider/2, (TIMER_TYPE) base*time_unit/divider, 0);
    char_parameter_1[task] = click_pin[task];
    pinMode(click_pin[task++], OUTPUT);
    task++;
  }

  //  click_task[task] = setup_task(&click, ACTIVE|DO_NOT_DELETE, now + sync*base/2*time_unit, (TIMER_TYPE) base*time_unit, 0);
  click_task[task] = setup_task(&click, ACTIVE|DO_NOT_DELETE, now, (TIMER_TYPE) base*time_unit, 0);		// slowest *not* synced
  char_parameter_1[task] = click_pin[task];
  pinMode(click_pin[task++], OUTPUT);
}



/* **************************************************************** */
// functions to deal with clicks:

// destination of menu functions '*' '/' '=' and 's'
unsigned char selected_destination=~0;
// destinations codes (other then 0, 1, 2, ... for individual clicker tasks):
#define ALL_PERIODICS	PERIODICS
#define TIME_UNIT	(PERIODICS + 1)



#ifdef USE_SERIAL
const unsigned char mutedAllPeriodics[] PROGMEM = "muted all periodics";
#endif

void mute_all_clicks () {
  for (int task=0; task<CLICK_PERIODICS; task++)
    flags[task] &= ~ACTIVE;

  // fix_global_next();			// planed soon...

#ifdef USE_SERIAL
  serial_println_progmem(mutedAllPeriodics);
#endif
}



const unsigned char period_[] PROGMEM = " period = ";
const unsigned char timeUnits_[] PROGMEM = " time units";

void print_period_in_time_units(int task) {
  float time_units, scratch;

  serial_print_progmem(period_);
  time_units = ((float) period[task] / (float) time_unit);
  scratch = 1000.0;
  while (scratch > time_units) {
    Serial.print(" ");
    scratch /= 10.0;
  }
  Serial.print((float) time_units, 3);
  serial_print_progmem(timeUnits_);
}

const unsigned char pin_[] PROGMEM = "pin ";	// HARDWARE_menu uses this too

const unsigned char periodic_[] PROGMEM = "periodic ";
const unsigned char timeUnits[] PROGMEM = " time units";
const unsigned char active_[] PROGMEM = "\tactive";
const unsigned char counter_[] PROGMEM = "\tcounter ";

void periodic_info(int task) {
  serial_print_progmem(periodic_);
  Serial.print((int) task);
  print_period_in_time_units(task);

  if(flags[task] & ACTIVE)
    serial_print_progmem(active_);
  else
    serial_print_progmem(tab_);

  serial_print_progmem(tab_);
  serial_print_progmem(pin_);
  Serial.print((int) click_pin[task]);

  serial_print_progmem(counter_);
  Serial.print(counter[task]);

  Serial.println("");
}


void periodics_info() {
  for (char task=0; task<CLICK_PERIODICS; task++) {
    if ((ALL_PERIODICS == selected_destination) || (task == selected_destination))
      Serial.print("*");
    else
      Serial.print(" ");

    periodic_info(task);
  }
  Serial.println("");
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

  Serial.println("");
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


//	// print binary numbers with leading zeroes and a space
//	void serial_print_BIN(unsigned long value, int bits) {
//	  int i;
//	  unsigned long mask=0;
//	
//	  for (i = bits - 1; i >= 0; i--) {
//	    mask = (1 << i);
//	      if (value & mask)
//		Serial.print(1);
//	      else
//		Serial.print(0);
//	  }
//	  Serial.print(" ");
//	}


#ifdef HARDWARE_menu	// inside MENU_over_serial
const unsigned char analog_reads_title[] PROGMEM = "\npin\tvalue\t|\t\t\t\t|\t\t\t\t|";

void display_analog_reads() {
  int i, value;

  serial_println_progmem(analog_reads_title);

  for (i=0; i<ANALOG_INPUTs; i++) {
    value = analogRead(i);
    Serial.print(i); serial_print_progmem(tab_); bar_graph(value);
  }

  Serial.println("");
}



const unsigned char watchingINpin[] PROGMEM = "watching digital input pin ";
const unsigned char anyStop[] PROGMEM = "\t\t(send any byte to stop)";
// const unsigned char pin_[] PROGMEM = "pin ";		// defined already
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
  Serial.println("");

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

const unsigned char switchPeriodic[] PROGMEM = "s=switch periodic on/off";
const unsigned char freeRAM[] PROGMEM = "free RAM: ";
const unsigned char pPin[] PROGMEM = "p=pin (";

const unsigned char none_[] PROGMEM = "(none)";
const unsigned char selectDestinationInfo[] PROGMEM = "SELECT DESTINATION for '=' '*' '/' and 's' to work on:";
const unsigned char selectPeriodicWith[] PROGMEM = "Select periodic with ";
const unsigned char all_[] PROGMEM = "(ALL)";
const unsigned char selectAllPeriodics[] PROGMEM = "A=select *all* periodics";
const unsigned char selectTimeUnits[] PROGMEM = "t=select time units";
const unsigned char selected_[] PROGMEM = "\t(selected)";

void info_select_destination_with(boolean extended_destinations) {
  serial_println_progmem(selectDestinationInfo);
  serial_print_progmem(selectPeriodicWith);
  for (int task=0; task<CLICK_PERIODICS; task++ ) {
    Serial.print(task); Serial.print("  ");
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

  serial_println_progmem(selectAllPeriodics);


  if(extended_destinations) {
    serial_print_progmem(selectTimeUnits);
    if(selected_destination == TIME_UNIT) {
    serial_println_progmem(selected_);
    } else
      Serial.println("");
    Serial.println("");
  }
}



const unsigned char pressm[] PROGMEM = "\nPress 'm' or '?' for menu.\n\n";

const unsigned char microSeconds[] PROGMEM = " microseconds";
const unsigned char Mute_Info[] PROGMEM = "M=mute all\ti=periodics info";
const unsigned char perSecond_[] PROGMEM = " per second)";
const unsigned char equals_[] PROGMEM = " = ";

void display_serial_menu() {

  serial_println_progmem(programLongName);

  Serial.println("");
  info_select_destination_with(false);

  serial_print_progmem(selectTimeUnits);
  Serial.print("  (");
  Serial.print(time_unit);
  serial_print_progmem(microSeconds);
  serial_print_progmem(equals_);
  Serial.print((float) (1000000.0 / (float) time_unit),3);
  serial_println_progmem(perSecond_);

  Serial.println("");
  serial_println_progmem(switchPeriodic);
  serial_println_progmem(Mute_Info);

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
      serial_print_progmem(pin_); Serial.print((int) hw_PIN); serial_println_progmem(setToHigh);
    }
    break;

  case 'L':
    if (hw_PIN == ILLEGAL)
      please_select_pin();
    else {
      pinMode(hw_PIN, OUTPUT);
      digitalWrite(hw_PIN, LOW);
      serial_print_progmem(pin_); Serial.print((int) hw_PIN); serial_println_progmem(setToLow);
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
      serial_print_progmem(analogValueOnPin); Serial.print((int) hw_PIN); serial_print_progmem(is_);
      Serial.println(analogRead(hw_PIN));
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

const unsigned char multipliedPeriod_[] PROGMEM = "Multiplied period[";

void multiply_period_and_inform(int task, unsigned long factor) {
  set_new_period(task, (TIMER_TYPE) period[task] * factor);

  serial_print_progmem(multipliedPeriod_);
  Serial.print((int) task);
  Serial.print("] by ");
  Serial.print(factor);
  serial_print_progmem(tab_);
  periodic_info(task);
}


const unsigned char dividedPeriod_[] PROGMEM = "Divided period[";

void divide_period_and_inform(int task, unsigned long divisor) {
  set_new_period(task, (TIMER_TYPE) period[task] / divisor);

  serial_print_progmem(dividedPeriod_);
  Serial.print((int) task);
  Serial.print("] by ");
  Serial.print(divisor);
  serial_print_progmem(tab_);
  periodic_info(task);
}


const unsigned char setPeriod_[] PROGMEM = "Set period[";

void set_new_period_and_inform(int task, unsigned long value) {
  set_new_period(task, value);

  serial_print_progmem(setPeriod_);
  Serial.print((int) task);
  Serial.print("] to ");
  print_period_in_time_units(task);
  //  Serial.print(value);
  serial_print_progmem(tab_);
  serial_print_progmem(tab_);
  periodic_info(task);
}


const unsigned char setTimeUnit_[] PROGMEM = "Set time unit to ";

void set_time_unit_and_inform(unsigned long newValue) {
  time_unit = newValue;
  serial_print_progmem(setTimeUnit_);
  Serial.print(time_unit);
  serial_println_progmem(microSeconds);
}


const unsigned char switchedPeriodic_[] PROGMEM = "Switched periodic ";

void switch_periodic_and_inform(int task) {
  flags[task] ^= ACTIVE;

  serial_print_progmem(switchedPeriodic_);
  Serial.print((int)  task);
  if (flags[task] & ACTIVE) {
    next[task] = TIMER;
    last[task] = next[task];	// for overflow logic

    Serial.println(" on/t");
    periodic_info(task);
  } else
    Serial.println (" off");

  // fix_global_next();			// planed soon...
}



/* **************************************************************** */
// menu reaction:

const unsigned char unknownMenuInput[] PROGMEM = "unknown menu input: ";

const unsigned char selectedPeriodic_[] PROGMEM = "Selected periodic ";
const unsigned char selectedAllPeriodics[] PROGMEM = "Selected *all* periodics";
const unsigned char selectedTimeUnit[] PROGMEM = "Selected time unit";


void menu_serial_reaction() {
  char menu_input;
  long newValue;

  while(!char_available())
    ;

  if (char_available()) {
    while (char_available()) {
      newValue=0;

      switch (menu_input = get_char()) {
      case ' ': case '\t':		// skip white chars
	break;

      case 'm': case '?':
	display_serial_menu();
	break;

      // *do* change this line if you change CLICK_PERIODICS
      case '0': case '1': case '2': case '3': case '4':
	// display(menu_input - '0');
	selected_destination = menu_input - '0';
	serial_print_progmem(selectedPeriodic_);
	Serial.println((int)  menu_input - '0');
	break;

      case 't':
	selected_destination = TIME_UNIT;
	serial_println_progmem(selectedTimeUnit);
	break;

      case 'A':
	selected_destination = ALL_PERIODICS;
	serial_println_progmem(selectedAllPeriodics);
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
	    Serial.println("");
	    break;

	  default:
	    info_select_destination_with(false);
	  }
	break;

      case 'i':
	periodics_info();
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

	} else
	  newValue = numericInput(1);

	  switch (selected_destination) {
	  case ALL_PERIODICS:
	    for (int task=0; task<CLICK_PERIODICS; task++ ) {
	      multiply_period_and_inform(task, newValue);
	    }
	    Serial.println("");
	    break;

	  case TIME_UNIT:
	    set_time_unit_and_inform(time_unit*newValue);
	    break;

	  default:
	      info_select_destination_with(true);
	  }
	break;

      case '/':
	if (selected_destination < CLICK_PERIODICS) {
	  newValue = numericInput(1);
	  if (newValue>0) {
	    divide_period_and_inform(selected_destination, newValue);
	  } else
	    serial_println_progmem(invalid);

	} else
	  newValue = numericInput(1);

	  switch (selected_destination) {
	  case ALL_PERIODICS:
	    for (int task=0; task<CLICK_PERIODICS; task++ ) {
	      divide_period_and_inform(task, newValue);
	    }
	    Serial.println("");
	    break;

	  case TIME_UNIT:
	    set_time_unit_and_inform(time_unit/newValue);
	    break;

	  default:
	    info_select_destination_with(true);
	  }
	break;

      case '=':
	if (selected_destination < CLICK_PERIODICS) {		// periodiccs
	newValue = numericInput(period[selected_destination] / time_unit);
	if (newValue>=0) {
	  set_new_period_and_inform(selected_destination, newValue * time_unit);
	} else
	  serial_println_progmem(invalid);

	} else {
	  newValue = numericInput(1);

	  switch (selected_destination) {
	  case ALL_PERIODICS:
	    for (int task=0; task<CLICK_PERIODICS; task++ ) {
	      set_new_period_and_inform(task, newValue * time_unit);
	    }
	    Serial.println("");
	    break;

	  case TIME_UNIT:			// time_unit
	    set_time_unit_and_inform(newValue);
	    break;

	  default:
	    info_select_destination_with(true);
	  }
	}
	break;



//    case 'd':				// hook for debugging
//	Serial.println("DEBUGGING");
//	init_rhythm_2(1);
//	break;

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
	  Serial.println("");
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


void do_jiffle0 (int task) {	// to be called by task_do
  // char_parameter_1[task]	click pin
  // char_parameter_2[task]	jiffletab index
  // parameter_1[task]		count down
  // parameter_2[task]		jiffletab[] pointer
  // ulong_parameter_1[task]	base period = period of starting task

  digitalWrite(char_parameter_1[task], counter[task] & 1);	// click

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
  period[task] = ulong_parameter_1[task] * jiffletab[base_index] / jiffletab[base_index+1];
  parameter_1[task] = jiffletab[base_index+2];			// count of next phase
  // fix_global_next();
}


int init_jiffle(unsigned int *jiffletab, TIMER_TYPE when, TIMER_TYPE jiffle_base_period, int origin_task) {
  TIMER_TYPE jiffle_period = jiffle_base_period * jiffletab[0] / jiffletab[1];

  int jiffle_task = setup_task(&do_jiffle0, ACTIVE, when, jiffle_period , 0);
  if (jiffle_task != ILLEGAL) {
    char_parameter_1[jiffle_task] = click_pin[origin_task];	// set pin
    // pinMode(click_pin[task++], OUTPUT);			// should be ok already
    char_parameter_2[jiffle_task] = 0;				// init phase 0
    parameter_1[jiffle_task] = jiffletab[2];			// count of first phase
    parameter_2[jiffle_task] = (unsigned int) jiffletab;
    ulong_parameter_1[jiffle_task] = jiffle_base_period;
  } 
#ifdef USE_SERIAL
  else {
    Serial.print("ERROR: no task free to start jiffle from task ");
    Serial.println(origin_task);
  }
#endif
}


void do_throw_a_jiffle(int task) {		// for task_do
  // parameter_2[task]	= (unsigned int) jiffletab;
  // we *could* also do     ulong_parameter_1[task] = jiffle_base_period;

  // start a new jiffling task now (next [task] is not yet updated):
  init_jiffle((unsigned int *) parameter_2[task], next[task], period[task], task);
}

void setup_jiffle_thrower(unsigned int *jiffletab, unsigned char new_flags, TIMER_TYPE when, TIMER_TYPE new_period, unsigned int new_int1) {
  int jiffle_task = setup_task(&do_throw_a_jiffle, new_flags, when, new_period, new_int1);
  if (jiffle_task != ILLEGAL)
    parameter_2[jiffle_task] = (unsigned int) jiffletab;
}


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
  #ifndef MENU_over_serial
   Serial.println("");
   serial_println_progmem(programLongName);
  #endif
#endif

#ifdef MENU_over_serial	// show message about menu
  display_serial_menu();
#endif

//	#ifdef USE_LCD
//	  delay(3000);
//	  LCD.clear();
//	  LCD_print_line_progmem(TOP, programName);
//	#endif

  click_pin[0]= 2;		// configure PINs here
  click_pin[1]= 3;		// configure PINs here
  click_pin[2]= 4;		// configure PINs here
  click_pin[3]= 5;		// configure PINs here
  click_pin[4]= 6;		// configure PINs here


  for (int task=0; task<CLICK_PERIODICS; task++) {
    pinMode(click_pin[task], OUTPUT);
  }

#ifdef LED_PIN
  pinMode(LED_PIN, OUTPUT);
#endif

  init_tasks();

  // By design click tasks *HAVE* to be defined *BEFORE* any other tasks:
  // init_rhythm_1(1);
  // init_rhythm_2(1);

//  setup_jiffle_thrower(jiffletab0, ACTIVE|DO_NOT_DELETE, TIMER+5*time_unit, 10*time_unit, 0);
//  setup_jiffle_thrower(jiffletab0, ACTIVE|DO_NOT_DELETE, TIMER+10*time_unit, 15*time_unit, 0);
//  setup_jiffle_thrower(jiffletab0, ACTIVE|DO_NOT_DELETE, TIMER+15*time_unit, 20*time_unit, 0);
//  setup_jiffle_thrower(jiffletab0, ACTIVE|DO_NOT_DELETE, TIMER,              25*time_unit, 0);
//  setup_jiffle_thrower(jiffletab0, ACTIVE|DO_NOT_DELETE, TIMER+20*time_unit, 30*time_unit, 0);

  now=TIMER;
  int scale=18;
  setup_jiffle_thrower(jiffletab0, ACTIVE|DO_NOT_DELETE, now +  scale*2/2*time_unit,   scale*2*time_unit, 0);	// 2
  setup_jiffle_thrower(jiffletab0, ACTIVE|DO_NOT_DELETE, now +  scale*3/2*time_unit,   scale*3*time_unit, 0);	// 3
  setup_jiffle_thrower(jiffletab0, ACTIVE|DO_NOT_DELETE, now +  scale*4/2*time_unit,   scale*4*time_unit, 0);	// 4
  setup_jiffle_thrower(jiffletab0, ACTIVE|DO_NOT_DELETE, now +  scale*5/2*time_unit,   scale*5*time_unit, 0);	// 5

  // 2*3*2*5	(the 4 needs only another factor of 2)
  setup_jiffle_thrower(jiffletab0, ACTIVE|DO_NOT_DELETE, now + scale*2*3*2*5/2*time_unit, scale*2*3*2*5*time_unit, 0);

#ifdef MENU_over_serial
  periodics_info();
#endif
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

#ifdef MENU_over_serial
  if (char_available())
    menu_serial_reaction();
#endif

}


/* **************************************************************** */
