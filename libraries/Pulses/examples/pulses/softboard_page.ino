/*
 * ****************************************************************
 * included from softboard_page.ino
 * ****************************************************************
 */


/* **************************************************************** */
/* CODE TO INSERT INTO YOUR PROGRAM setup() follows: ************** */
/* Arduino setup()						*/

/*
  >>>>>>>>>>>>>>>> PUT this *BEFORE*  Arduino setup() <<<<<<<<<<<<<<<<
  // needed for MENU.add_page();
  void softboard_display();		// defined later on
  bool softboard_reaction(char token);	// defined later on
  >>>>>>>>>>>>>>>> PUT THESE BEFORE  Arduino setup() <<<<<<<<<<<<<<<<
*/

/*
  >>>>>>>>>>>>>>>> PUT this *IN*  Arduino setup() <<<<<<<<<<<<<<<<
  // add softboard to MENU:
  MENU.add_page("Arduino Softboard", 'H',	\
		&softboard_display, &softboard_reaction, 'H');

  >>>>>>>>>>>>>>>> PUT THIS IN Arduino setup() <<<<<<<<<<<<<<<<
*/

/* CODE TO INSERT INTO YOUR PROGRAM setup() ends. ***************** */
/* **************************************************************** */



/* **************************************************************** */
/* CODE TO INSERT INTO YOUR PROGRAM loop() follows: *************** */
/* Arduino loop()						*/

/*
  >>>>>>>>>>>>>>>> PUT this *BEFORE*  Arduino loop() <<<<<<<<<<<<<<<<
  void maybe_run_continuous();	// defined later on
  >>>>>>>>>>>>>>>> PUT this *BEFORE*  Arduino loop() <<<<<<<<<<<<<<<<
*/

/* Arduino loop() template:
//	void loop() {	// ARDUINO
//	  MENU.lurk_then_do();
//	  maybe_run_continuous();	// maybe display input pin state changes
//
//	  // Insert your own code here.
//	  // Do not let your program block program flow,
//	  // always return to the main loop as soon as possible.
//	}
*/

/* CODE TO INSERT INTO YOUR PROGRAM loop() ends. ****************** */
/* **************************************************************** */



/* **************************************************************** */
/* ARDUINO BOARD SPECIFIC THINGS  try to use ARDUINO MACROS: */

#ifndef NUM_DIGITAL_PINS		// try harder... ?
  #ifndef NUM_DIGITAL_PINS
    #error #define NUM_DIGITAL_PINS
  #endif

  #if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) // mega boards
    #define NUM_DIGITAL_PINS	70
  #else								 // 168/328
    #define NUM_DIGITAL_PINS	20
  #endif

  #ifndef NUM_DIGITAL_PINS
    #error #define NUM_DIGITAL_PINS
  #endif
#endif


#ifndef NUM_ANALOG_INPUTS		// try harder... ?
  #ifndef NUM_ANALOG_INPUTS
    #error #define NUM_ANALOG_INPUTS
  #endif

  #if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) // mega boards
    #define NUM_ANALOG_INPUTS	16
  #else								// 168/328 boards
    #define NUM_ANALOG_INPUTS	6
  #endif

  #ifndef NUM_ANALOG_INPUTS
    #error #define NUM_ANALOG_INPUTS
  #endif
#endif


// check for possible unknown eightanaloginputs variant
#if (NUM_ANALOG_INPUTS == 8) && (NUM_DIGITAL_PINS != 20)
  #error "UNKNOWN eightanaloginputs variant, please adapt source or ignore."
#endif


#ifndef digitalPinHasPWM	// ################
  #ifndef __SAM3X8E__
    #error #define digitalPinHasPWM
  #else
    #define digitalPinHasPWM(p)         ((p) >= 2 && (p) <= 13)
  #endif
#endif




/* **************************************************************** */
/* define some things early: */

// #define ILLEGAL	-1	// probably already defined above

char PIN_digital = ILLEGAL_PIN;	// would be dangerous to default to zero
char PIN_analog = 0;		// 0 is save as default for analog pins


/* extra_switch:
 * toggle extra range (and maybe functionality) on digital pins:
 * chage default here by un/commenting
 * #define START_WITH_EXTRA_SWITCH_ON				*/
#define START_WITH_EXTRA_SWITCH_ON

#ifdef START_WITH_EXTRA_SWITCH_ON

  bool extra_switch=true;	// extra functionality default *on*

  #if (NUM_ANALOG_INPUTS != 8)
    int visible_digital_pins=NUM_DIGITAL_PINS;
  #else
    int visible_digital_pins=22;

  #endif
#else	// extra_switch defaults to off

  bool extra_switch=false;	// extra functionality default *off*

  #if (NUM_ANALOG_INPUTS != 8)	// standard boards
    int visible_digital_pins=NUM_DIGITAL_PINS - NUM_ANALOG_INPUTS;
  #else				// variant eightanaloginputs
    int visible_digital_pins=14;		// FIXME: test ################
  #endif
#endif

bool echo_switch=true;		// serial echo switch


/* ****************  info on DIGITAL pins:  **************** */

#ifdef __SAM3X8E__
  #ifndef portModeRegister
    #warning "#define *MISSING* portModeRegister(P)."
    #define portModeRegister(P) ( (volatile RwReg*)( pgm_read_word( port_to_mode_PGM + (P))) )
  #endif
#endif

/*
  pin_info_digital()
  display configuration and state of a pin:
*/

// const char pin_[] = "pin ";		// pulses.ino has already defined it?
const char high_[] = "HIGH";
const char low_[] = "LOW";

void pin_info_digital(uint8_t pin) {
  uint8_t mask = digitalPinToBitMask(pin);
#ifndef __SAM3X8E__	// FIXME: ################
  uint8_t port = digitalPinToPort(pin);
#else
  #warning "softboard does not run on Arduino Due yet! ################"
  Pio* const port = digitalPinToPort(pin);
#endif

#ifdef __SAM3X8E__	// FIXME: ################
  volatile RwReg* reg;

  if (port == NULL) return;

#else
  volatile uint8_t *reg;

  if (port == NOT_A_PIN) return;
#endif

  // selected sign * and pin:
  if (pin == PIN_digital )
    MENU.out('*');
  else
    MENU.space();
  MENU.out(pin_);
  MENU.out((int) pin);
  MENU.tab();

#ifdef __SAM3X8E__
  #warning "I/O pin configuration info *not implemented on Arduino DUE yet*."
  MENU.out(F("??? (DUE not implemented)"));
#else
  // see: <Arduino.h>

  // input or output?
  reg = portModeRegister(port);
  // uint8_t oldSREG = SREG;	// let interrupt ACTIVE ;)
  // cli();
  if (*reg & mask) {		// digital OUTPUTS
    // SREG = oldSREG;
    MENU.out(F("O  "));

    // high or low?
    reg = portOutputRegister(port);
    // oldSREG = SREG;		// let interrupt ACTIVE ;)
    // cli();
    if (*reg & mask) {		    // HIGH
      // SREG = oldSREG;
      MENU.out(high_);
    } else {			    // LOW
      // SREG = oldSREG;
      MENU.out(low_);
    }
  } else {			// digital INPUTS
    // SREG = oldSREG;
    MENU.out(F("I  "));

    // pullup, tristate?
    reg = portOutputRegister(port);
    // oldSREG = SREG;		// let interrupt ACTIVE ;)
    // cli();
    if (*reg & mask) {		    // pull up resistor
      // SREG = oldSREG;
      MENU.out(F("pullup"));
    } else {			    // tri state high-Z
      // SREG = oldSREG;
      MENU.out(F("floating"));
    }
  }
#endif
  MENU.ln();
}


// display configuration and state of all digital pins:
void pins_info_digital() {
  for (uint8_t pin=0; pin<visible_digital_pins; pin++)
    pin_info_digital(pin);
}


/* continuously watch a digital pins readings: */
/*
  watch_digital_input(pin)
  continuously display digital input readings,
  whenever the input changes:
*/

#define IMPOSSIBLE	-9785	// just a value not possible on analog input

int watch_seen=IMPOSSIBLE;
void watch_digital_start(uint8_t pin) {
  watch_seen=IMPOSSIBLE;

  MENU.out(F("watching pin D"));
  MENU.out((int) pin);
  MENU.out(F("\t\tr=stop"));
}


void watch_digital_input(int pin) {
  int value=digitalRead(PIN_digital);

  if (value != watch_seen) {
    watch_seen = value;
    MENU.out(F("*D"));  MENU.out((int) pin);
    MENU.tab();
    if (value)
      MENU.outln(high_);
    else
      MENU.outln(low_);
  }
}


bool run_watch_dI=false;

// toggle_watch()  toggle continuous digital watch display on/off.
void toggle_watch() {
  run_watch_dI = !run_watch_dI;
  if (run_watch_dI)
    watch_digital_start(PIN_digital);
  else
    MENU.ln();
}


/* ****************  info on ANALOG pins:  **************** */
/*
  bar_graph(value)
  print one value & bar graph line:
*/
const char value_[] = "value ";

void bar_graph(int value) {
  int i, length=64, scale=1023;
  int stars = ((long) value * (long) length) / scale + 1 ;


  if (value >=0 && value <= 1024) {
    MENU.out(value); MENU.tab();
    for (i=0; i<stars; i++) {
      if (i == 0 && value == 0)		// zero
	MENU.out('0');
					// middle or top
      else if \
	((i == length/2 && value == 512) || (i == length && value == scale))
	MENU.out('|');
      else
	MENU.out('*');
    }
  } else {
    MENU.out(F("value "));
    MENU.out(value);
    MENU.OutOfRange();
  }

  MENU.ln();
}


// Display analog pin name and value as number and bar graph:
void pin_info_analog(uint8_t pin) {
  if (pin == PIN_analog)
    MENU.out(F("*A"));
  else
    MENU.out(F(" A"));
  MENU.out((int) pin);
  MENU.tab();
  bar_graph(analogRead(pin));
}


/*
  pins_info_analog()
  Display analog snapshot read values and bar graphs, a line each analog input:
*/
const char analog_reads_title[] =	\
  "\npin\tvalue\t|\t\t\t\t|\t\t\t\t|";

void pins_info_analog() {
  int i, value;

  MENU.outln(analog_reads_title);

  for (i=0; i<NUM_ANALOG_INPUTS; i++)
    pin_info_analog(i);

  MENU.ln();
}


/* **************************************************************** */
/*
  Running 'continuous' display types or similar.
  Implemented without waiting allows 'run-through' programming.

  bar_graph_VU(pin):
  Continuous display changes on an analogue input.
  Display a scrolling bar graph over the readings.
  A new line is displayed as soon as the reading changes for more
  than +/- tolerance.

  tolerance can be changed by sending '+' or '-'

  asynchronous run-through, don't wait too long...
*/
bool run_VU=false;

void stop_continuous() {
  run_VU=false;
  run_watch_dI=false;
}

int VU_last=IMPOSSIBLE;

// tolerance default 0. Let the user *see* the noise...
int bar_graph_tolerance=0;

void feedback_tolerance() {
  MENU.out(F("tolerance "));
  MENU.outln(bar_graph_tolerance);
}


void VU_init(int pin) {
  VU_last=IMPOSSIBLE;	// just an impossible value

  MENU.out(F("pin\tval\t+/- set "));
  feedback_tolerance();
}


// toggle_VU()  toggle continuous VU display on/off.
void toggle_VU() {
  run_VU = !run_VU;
  if (run_VU)
    VU_init(PIN_analog);
  else
    MENU.ln();
}


/*
  bar_graph_VU(pin):
  Continuous display changes exceeding a tolerance on an analogue input.
  Display a scrolling bar graph over the readings.
  A new line is displayed as soon as the reading changes for more
  than +/- tolerance.

  tolerance can be changed by sending '+' or '-'

  asynchronous run-through, don't wait too long...
*/
void bar_graph_VU(int pin) {
  int value;

  value =  analogRead(pin);
  if (abs(value - VU_last) > bar_graph_tolerance) {
    MENU.out(F("*A"));
    MENU.out((int) pin);
    MENU.tab();
    bar_graph(value);
    VU_last = value;
  }
}


/*
  void maybe_run_continuous():
  Check if to continuously show analog/digital input changes:
*/
void maybe_run_continuous() {
  if (softboard_page == MENU.men_selected) {	  // only active on softboard page
    if (run_VU)
      bar_graph_VU(PIN_analog);

    if (run_watch_dI)
      watch_digital_input(PIN_digital);
  }
}


/* **************************************************************** */
/*
   Menu softboard display and reaction:
*/

/* **************************************************************** */
/* Building menu pages:

   The program defines what the menu pages display and how they react.

   The menu can have multiple pages, the selected one gets displayed.
   On start up the first one added will be selected.

   The menu pages define keys ("tokens") and the corresponding actions.

   Below the tokens of the selected/displayed page tokens of other pages
   can be configured to remain visible in groups of related pages.

   For each menu page we will need:

     hotkey		selects this menu page.
     title		identifies page to the user.
     display function	displays the menu page.
     action function	checks if it is responsible for a token,
     	    		if so do its trick and return true,
			else return false.
*/


/* ****************  softboard menu display:  **************** */
const char select_[] = "select ";
const char pinFor[] = " pin for ";
const char toWork_[] = " to work on:\t";
const char pin__[] = "pin (";


// Factored out display functions:

void _select_digital(bool key) {
  if (key)
    MENU.out(F("D="));
  MENU.out(select_);
  MENU.out(F("digital"));
  MENU.out(pinFor);
  MENU.out(F("'d, r, I, O, H, L, W'"));
}


void _select_analog(bool key) {
  if (key)
    MENU.out(F("\nA="));
  MENU.out(select_);
  MENU.out(F("analog"));
  MENU.out(pinFor);
  MENU.out(F("'a, v'"));
}


void softboard_display() {
  _select_digital(true);
  MENU.out(toWork_);
  MENU.out(pin__);
  if (PIN_digital == ILLEGAL_PIN)
    MENU.out(F("no"));
  else
    MENU.out((int) PIN_digital);
  MENU.outln(')');
  MENU.out(F("O=OUTPUT\tI=INPUT\t\tH=HIGH\tL=LOW\tPWM: W=WRITE\td=pin info"));
  MENU.ln();

  _select_analog(true);
  MENU.out(toWork_);
  MENU.out(pin__);
  MENU.out((int) PIN_analog);
  MENU.outln(')');
  MENU.out(F("watch over time:\tv=VU bar\tr=read"));
  MENU.ln();

  MENU.outln(F("\n.=all digital\t,=all analog\t;=both\tx=extra"));
}


/* ****************  softboard reactions:  **************** */

/*
  Toggle extra functionality
  * Visability of analog inputs as general digital I/O pins
    (only this one item implemented)
*/

/* Variant eightanaloginputs is already tested above.
 * adapt code here if the test fails.				*/
#if (NUM_ANALOG_INPUTS != 8)	// *no* variant eightanaloginputs
  void toggle_extra() {		// *no* variant eightanaloginputs
    extra_switch = !extra_switch;
    if (extra_switch) {
      visible_digital_pins=NUM_DIGITAL_PINS;
    else
      visible_digital_pins=NUM_DIGITAL_PINS - NUM_ANALOG_INPUTS;
    }
  }
#else				// VARIANT: eightanaloginputs
  void toggle_extra() {		// VARIANT: eightanaloginputs
    extra_switch = !extra_switch;
    if (extra_switch)
      visible_digital_pins=22;		// FIXME: test ################
    else
      visible_digital_pins=14;		// FIXME: test ################
  }
#endif

// Factored out helper function
// digital_pin_ok()  Checks if PIN_digital has been set
bool digital_pin_ok() {
  // testing on ILLEGAL_PIN is enough in this context
  if (PIN_digital == ILLEGAL_PIN) {
    _select_digital(true);
    MENU.ln();
    return false;
  } else
    return true;
}


// bool softboard_reaction(char token)
// Try to react on token, return success flag.
const char pwm_[] = "PWM ";
const char noHw_[] = "no hardware ";
const char analogWrite_[] = "\tanalogWrite(";

bool softboard_reaction(char token) {
  long newValue;

  switch (token) {
  case 'A':
    _select_analog(false);
    MENU.tab();

    newValue = MENU.numeric_input(PIN_analog);
    if (newValue>=0 && newValue<NUM_ANALOG_INPUTS)
      PIN_analog = newValue;
    else {
      MENU.OutOfRange();
      MENU.ln();
    }

    MENU.out(pin_);
    MENU.outln((int) PIN_analog);
    break;

  case 'D':
    _select_digital(false);
    MENU.tab();

    newValue = MENU.numeric_input(PIN_digital);
    if (newValue>=0 && newValue<visible_digital_pins) {
      PIN_digital = newValue;
      pin_info_digital((int) PIN_digital);
    } else
      if (newValue != ILLEGAL_PIN) {
	MENU.OutOfRange();
	MENU.ln();
      }
    break;

  case 'O':
    if (digital_pin_ok()) {
      pinMode(PIN_digital, OUTPUT);
      pin_info_digital(PIN_digital);
    }
    break;

  case 'I':
    if (digital_pin_ok()) {
      pinMode(PIN_digital, INPUT);
      pin_info_digital(PIN_digital);
    }
    break;

  case 'H':  // no problem to use H here, same as the menu code.
    if (digital_pin_ok()) {
      digitalWrite(PIN_digital, HIGH);
      pin_info_digital(PIN_digital);
    }
    break;

  case 'L':
    if (digital_pin_ok()) {
      digitalWrite(PIN_digital, LOW);
      pin_info_digital(PIN_digital);
    }
    break;

  case 'W':
    if (digital_pin_ok()) {

      // can the pin do hardware PWM?
      if (!digitalPinHasPWM(PIN_digital)) {	// *no PWM* on this pin
	MENU.out(noHw_); MENU.out(pwm_);
	MENU.out(F("on ")); MENU.out(pin_);
	MENU.outln((int) PIN_digital);

	MENU.skip_numeric_input();
      } else {					// pin *can* do PWM
        MENU.out(pin_); MENU.out((int) PIN_digital);
	MENU.tab();
	MENU.out(pwm_); MENU.out(F("write "));
	newValue = MENU.numeric_input(ILLEGAL_PIN);
	if (newValue>=0 && newValue<=255) {
	  MENU.out(newValue);

	  analogWrite(PIN_digital, newValue);
	  MENU.out(F("\tanalogWrite(")); MENU.out((int) PIN_digital);
	  MENU.out(F(", ")); MENU.out(newValue);
	  MENU.outln(')');
	} else {
	  MENU.OutOfRange();
	  MENU.ln();
	}
      }
    }
    break;

  case 'a':
    MENU.outln(analog_reads_title);
    pin_info_analog(PIN_analog);
    break;

  case 'd':
    if (digital_pin_ok())
      pin_info_digital(PIN_digital);
    break;

  case 'v':
    toggle_VU();
    break;

  case '+':
    if (! run_VU)
      return false;    // *only* responsible if (run_VU)

    bar_graph_tolerance++;
    feedback_tolerance();
    break;

  case '-':
    if (! run_VU)
      return false;    // *only* responsible if (run_VU)

    if (bar_graph_tolerance)
      bar_graph_tolerance--;
    feedback_tolerance();
    break;

  case 'r':
    if (digital_pin_ok()) {
      toggle_watch();
      /*
	I do *not* call	pinMode(PIN_digital, INPUT) from here.
	Looks more flexible to me to let the user do that,
	if she wants.
      */
    }
    break;

  case '.':	// all digital
    MENU.ln();
    pins_info_digital();
    MENU.ln();
    break;

  case ',':	// all analog
    pins_info_analog();
    break;

  case ';':	// both ;)
    MENU.ln();
    pins_info_analog();
    pins_info_digital();
    MENU.ln();
    break;

    case 'x':	// toggle extended
      toggle_extra();
      break;

  default:
    return 0;		// token not found in this menu
  }
  return 1;		// token found in this menu
}


/* **************************************************************** */
/* README

README softboard [snipped]

*/
