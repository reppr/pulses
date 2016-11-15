/*
 * ****************************************************************
 * included from softboard_page.ino
 * ****************************************************************
 */


/* **************************************************************** */
/* ****  CODE TO INSERT TO THE START OF YOUR PROGRAM follows:  **** */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#ifdef ARDUINO
  #if ARDUINO >= 100
    #include "Arduino.h"
  #else
    #include "WProgram.h"
  #endif
#else	// #include's for Linux PC test version
  #include <iostream>
#endif

/* ****   CODE TO INSERT TO THE START OF YOUR PROGRAM ends:    **** */
/* **************************************************************** */



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
#include <pins_arduino.h>


#ifndef NUM_DIGITAL_PINS		// try harder... ?
  #warning "#define NUM_DIGITAL_PINS	// FIXME: ################"

  #if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) // mega boards
    #define NUM_DIGITAL_PINS	70
  #elif defined(__SAM3X8E__)
    #ifdef PINS_COUNT	// on Arduino DUE	// FIXME: ################
      #define NUM_DIGITAL_PINS	PINS_COUNT	// FIXME: ################
    #else
      #warning "#define MISING NUM_DIGITAL_PINS	// FIXME: ################"
      #warning "#define NUM_DIGITAL_PINS 79	// FIXME: ################"
      #define NUM_DIGITAL_PINS	79 		// FIXME: DUE ################
    #endif
  #else						// FIXME: 168/328 boards ???
    #define NUM_DIGITAL_PINS	20
  #endif

  #ifndef NUM_DIGITAL_PINS
    #error "#define NUM_DIGITAL_PINS		// FIXME: ################"
  #endif
#endif


#ifndef NUM_ANALOG_INPUTS		// try harder... ?
  #warning "#define NUM_ANALOG_INPUTS	// FIXME: ################"

  #if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) // mega boards
    #define NUM_ANALOG_INPUTS	16
  #elif defined(__SAM3X8E__)
     #define NUM_ANALOG_INPUTS	16		// FIXME: DUE ################
  #else 					// FIXME: 168/328 boards ???
    #define NUM_ANALOG_INPUTS	6
  #endif

  #ifndef NUM_ANALOG_INPUTS
    #error "#define NUM_ANALOG_INPUTS		// FIXME: ################"
  #endif
#endif


#ifndef digitalPinHasPWM	// ################
  #ifdef __SAM3X8E__		// FIXME: ################
    #warning "#define MISSING digitalPinHasPWM(p)"
    #define digitalPinHasPWM(p)         ((p) >= 2 && (p) <= 13)
  #else
    #error #define digitalPinHasPWM
  #endif
#endif


/*
  DIGITAL_IOs
  number of arduino pins configured for digital I/O
  *not* counting analog inputs:
*/

#define DIGITAL_IOs	(NUM_DIGITAL_PINS - NUM_ANALOG_INPUTS)


/* **************************************************************** */
/* define some things early: */

#define ILLEGAL		-1

char PIN_digital = ILLEGAL;	// would be dangerous to default to zero
char PIN_analog = 0;		// 0 is save as default for analog pins

// Comment next line out if you do not want the analog2tone functionality:
#define has_PIEZZO_SOUND

#ifdef has_PIEZZO_SOUND
char PIN_tone = ILLEGAL;	// pin for tone output on a piezzo
#endif

// The following defaults *must* match with each other, choose one pair.
// either:
  bool extra_switch=false;	// extra functionality on digital pins
  int visible_digital_pins=DIGITAL_IOs;		// extra_switch==false
// or:
  // bool extra_switch=false;	// extra functionality on digital pins
  // int visible_digital_pins=NUM_DIGITAL_PINS;	// extra_switch==true



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
const char pin_[] = "pin ";
const char high_[] = "HIGH";
const char low_[] = "LOW";

void pin_info_digital(uint8_t pin) {
  uint8_t mask = digitalPinToBitMask(pin);
#ifdef __SAM3X8E__	// FIXME: ################
  #warning "softboard does not run on Arduino Due yet! ################"
  Pio* const port = digitalPinToPort(pin);
#else
  uint8_t port = digitalPinToPort(pin);
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

#ifdef __SAM3X8E__	// FIXME: !!! ################
  #warning "I/O pin configuration info *not implemented on Arduino DUE yet*."
  MENU.out(F("(pin_info_digital() not implemented on DUE yet)"));
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
  MENU.outln(F("\t\tr=stop"));
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
    MENU.ln();
  } else {
    MENU.out(F("value "));
    MENU.out(value);
    MENU.OutOfRange();
  }
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
  int i;

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

  run-through, don't wait...
*/
bool run_VU=false;


int VU_last=IMPOSSIBLE;

// tolerance default 0. Let the user *see* the noise...
int bar_graph_tolerance=0;

void feedback_tolerance(unsigned int tolerance) {
  MENU.out(F("tolerance "));
  MENU.outln(tolerance);
}


void VU_init(int pin) {
  VU_last=IMPOSSIBLE;	// just an impossible value

  MENU.out(F("pin\tval\t+/- set "));
  feedback_tolerance(bar_graph_tolerance);
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

  run-through, don't wait...
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


#ifdef has_PIEZZO_SOUND
/*
  analog2tone(int analogPIN, int PIN_tone):
  Simple acoustical feedback of analog readings using arduino tone()

  This very simple version is probably not compatible with pulses.

  Arduino tone() gives me some garbage below 32hz and is unusable below 16hz
  So analog2tone switches tone off for analog reading 0 and
  adds an offset of 31hz starting from analog reading 1.
*/
bool run_analog2tone = false;

//unsigned int analog2tone_tolerance = 1;		// tolerance regarding analog reading
unsigned int analog2tone_tolerance = 0;		// tolerance regarding analog reading
unsigned long analog2tone_timeInterval = 36;	// minimal time between measurements
int lastToneReading = -1;

const int unusable_tones = 31;		// tone() plays some garbage below 32hz :(
// const int unusable_tones = 15;	// tone() plays garbage below 16hz :(

void analog2tone(int analogPIN, int PIN_tone) {
  if (PIN_tone == ILLEGAL)
    return;

  static unsigned long lastToneTime = 0;
  unsigned long now = millis();

  if (now - lastToneTime >= analog2tone_timeInterval) {
    lastToneTime = now;

    int ToneReading=analogRead(analogPIN);
    if (abs(ToneReading - lastToneReading) > analog2tone_tolerance) {
      lastToneReading = ToneReading;

      // unsigned int hz = ToneReading*1;	// arbitrary factor
      unsigned int hz = ToneReading;		// arbitrary factor==1
      // arduino tone() delivers garbage below 16hz,
      // so we mend that a bit:
      if(hz) {
	hz += unusable_tones;	// start at lowest ok tone=16hz)
	tone(PIN_tone, hz);
      } else			// switch tone off for zero
	noTone(PIN_tone);
      if(MENU.verbosity >= VERBOSITY_CHATTY) {
	MENU.out(F("tone hz="));
	MENU.outln(hz);
      }
    }
  }
}


// toggle_tone()  toggle continuous sound pitch follower on/off.
void toggle_tone() {
  run_analog2tone = !run_analog2tone;
  if (run_analog2tone)
    lastToneReading = -1;
  else
    noTone(PIN_tone);

  if(MENU.verbosity >= VERBOSITY_SOME) {
    MENU.out(F("tone "));
    if (run_analog2tone)
      MENU.outln(F("ON"));
    else
      MENU.outln(F("off"));
  }
}
#endif


/*
  void maybe_run_continuous():
  Check if to continuously show/sound analog/digital input changes:
*/
void maybe_run_continuous() {
  if (run_VU)
    bar_graph_VU(PIN_analog);

#ifdef has_PIEZZO_SOUND
  if(run_analog2tone)
    analog2tone(PIN_analog, PIN_tone);
#endif

  if (run_watch_dI)
    watch_digital_input(PIN_digital);
}


void stop_continuous() {	// unused
  run_VU=false;
  run_watch_dI=false;
#ifdef has_PIEZZO_SOUND
  if(run_analog2tone)
    toggle_tone();
#endif
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
#ifdef has_PIEZZO_SOUND
  MENU.out(F("'a, v, t'"));
#else
  MENU.out(F("'a, v'"));
#endif
}


void softboard_display() {
  _select_digital(true);
  MENU.out(toWork_);
  MENU.out(pin__);
  if (PIN_digital == ILLEGAL)
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

#ifdef has_PIEZZO_SOUND
  MENU.out(F("watch over time:\tv=VU bar  r=read\tt=tone\tT=TonePIN ("));
  MENU.out(( int) PIN_tone);
  MENU.outln(F(")"));
#else
  MENU.outln(F("watch over time:\tv=VU bar\tr=read"));
#endif

  MENU.outln(F("\n.=all digital\t,=all analog\t;=both\tx=extra"));
}


/* ****************  softboard reactions:  **************** */

/*
  Toggle extra functionality
  * Visability of analog inputs as general digital I/O pins
    (only this one item implemented)
*/
void toggle_extra() {
  extra_switch = !extra_switch;
  if (extra_switch)
    visible_digital_pins=NUM_DIGITAL_PINS;
  else
    visible_digital_pins=DIGITAL_IOs;
}


// Factored out helper function
// digital_pin_ok()  Checks if PIN_digital has been set
bool digital_pin_ok() {
  // testing on ILLEGAL is enough in this context
  if (PIN_digital == ILLEGAL) {
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
    else
      MENU.OutOfRange();

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
      if (newValue != ILLEGAL)
	MENU.OutOfRange();
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
	newValue = MENU.numeric_input(ILLEGAL);
	if (newValue>=0 && newValue<=255) {
	  MENU.out(newValue);

	  analogWrite(PIN_digital, newValue);
	  MENU.out(F("\tanalogWrite(")); MENU.out((int) PIN_digital);
	  MENU.out(F(", ")); MENU.out(newValue);
	  MENU.outln(')');
	} else
	  MENU.OutOfRange();
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
#ifdef has_PIEZZO_SOUND
    if (!run_VU && !run_analog2tone)
      return false;    // *only* responsible if (run_VU || run_analog2tone)

    if (run_VU)
      feedback_tolerance(++bar_graph_tolerance);
    if (run_analog2tone)
      feedback_tolerance(++analog2tone_tolerance);
#else
    if (!run_VU)
      return false;    // *only* responsible if (run_VU)

    feedback_tolerance(++bar_graph_tolerance);
#endif
    break;

  case '-':
#ifdef has_PIEZZO_SOUND
    if (!run_VU && !run_analog2tone)
      return false;    // *only* responsible if (run_VU || run_analog2tone)

    if (run_VU)
      if (bar_graph_tolerance)		// only if not already zero
	feedback_tolerance(--bar_graph_tolerance);
    if (run_analog2tone)
      if (analog2tone_tolerance)	// only if not already zero
	feedback_tolerance(--analog2tone_tolerance);
#else
    if (! run_VU)
      return false;    // *only* responsible if (run_VU)

    if (bar_graph_tolerance)		// only if not already zero
      feedback_tolerance(--bar_graph_tolerance);
#endif

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

#ifdef has_PIEZZO_SOUND
  case 't':
    toggle_tone();
    break;

  case 'T':
    if(run_analog2tone)
      toggle_tone();	// switch old tone off

    newValue = MENU.numeric_input(PIN_tone);
    if (newValue>=0 && newValue<NUM_DIGITAL_PINS) {
      PIN_tone = newValue;
      MENU.out(pin_);
      MENU.outln((int) PIN_tone);
      toggle_tone();	// switch tone ON
    } else {
      MENU.OutOfRange();
    }
    break;
#endif

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
    if(MENU.verbosity >= VERBOSITY_SOME) {
      MENU.out(F("Show pins: "));
      MENU.outln(visible_digital_pins);
    }
    break;

  default:
    return 0;		// token not found in this menu
  }
  return 1;		// token found in this menu
}


/* **************************************************************** */
/* README

README softboard

   softboard  http://github.com/reppr/pulses

   Arduino hardware/software developing/testing tool.

   This version is implemented as an example for the Menu library.
   Using a library makes it easy to include in your own sketches.
   You can add more menu pages with other functionality to your sketch.


Description

   Softboard is a kind of an Arduino software breadboard.

   Simple Arduino hardware Menu interface over a serial line like USBserial.
   Softboard gives you an interface to the Arduino hardware
   by sending simple commands:
   * show infos about arduino pins and RAM usage
   * set pin mode
   * write pin states
   * read analog and digital inputs
   * write PWM values
   * continuously display changes on analog or digital inputs as bar graph
   * play analog readings back as the pitch of a tone


   Use it to test hardware like sensors, motors, things you want
   to run quick tests after setting something up on a (real) breadboard.

   Then you can use it to test software parts of your program while
   you write it and fit parts together.

   Later, when everything is working you can throw out the Menu code
   from your sketch, or keep it as an interface to your program.


   Send a sequence of one letter commands and numeric chiffres
   followed by a linebreak over a serial line (say from your computer)
   to the arduino to trigger menu actions and get infos.

   The arduino will buffer serial input byte by byte (without waiting)
   until a terminating linefeed is received as an end token.
   Any sequence of one or more '\n'  '\c'  '\0' is accepted as end token.


   Commands can read and set I/O pin configuration and states,
   switch pins on/off, read and write digital and analog values,
   switch pullup/high-z, continuously watch changing inputs over time.
   You can also connect a piezzo to a spare digital pin and listen
   to the changes on an analog input coded as pitch of a tone.


   Send '?' (and a linefeed) over serial line to see the menu.
   It displays some basic infos and shows some one-letter commands.


'run-through' cpu time friendly implementation:

   Softboard tries not to block the processor longer then needed and
   to let as much cpu time as possible to any other code possibly running.
   Menu will just run through it's code and return immediately.


Integrate your own code:

   It is easy to define Menu pages as an interface to your own code.
   Please have a look at the Menu/examples/ to see how to do this.

   You can define multiple Menu pages and define visability groups
   for pages sharing the same commands.

   Softboard is just a definition of a Menu page, you can add pages
   or delete them when compiling your program as you like.


Installation:

   Get it from http://github.com/reppr/pulses
   Put contents of the pulses/libraries/ folder into sketchbook/libraries/

   Softboard is implemented as an example of libraries/Menu/
   After a restart the Arduino GUI shows softboard under
   File >> Sketchbook >> libraries >> Menu >> softboard

	Arduino versions older than 1.0 need a fix:
	  see this thread:
	  http://code.google.com/p/arduino/issues/detail?id=604&start=200

	    short version (adapt to your arduino version)
	      edit file arduino-0022/hardware/arduino/cores/arduino/wiring.h
	      comment out line 79 (round macro)
	      #define round(x)     ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))
	      tested on arduino-0023

	For older arduino versions you also have to rename the sketch file
	from softboard.ino to softboard.pde


How it works:

   You communicate with the arduino over a serial connection
   that could be a real serial line or an usb cable.
   The library should be able to deal with other stream interfaces, btw.

   The minimalistic menu shows you one letter commands and listens to
   your input. Serial inputs are buffered until you send a linefeed.
   Then your inputs (commands and numbers as sequences of chiffres)
   will be read and acted upon.


Configure your terminal program:

   Do configure your terminal emulation program to send some sort of
   line ending code, usual culprits should work.

   Set arduino baud rate by editing the line starting with
   #define BAUDRATE in softboard.ino
   and set it to the same value in your terminal software.

   So if for example you use the Arduino 'Serial Monitor' window
   check that it *does* send 'Newline' (in the bottom window frame)
   and set baud rate to the same value as BAUDRATE on the arduino.


   Send '?' (and a linefeed) over serial line to see the menu.
   It displays some basic infos and shows some one-letter commands.

   'e' toggle serial echo.
   'x' toggle visibility of analog inputs as extra digital I/O pins.



Some examples:

       Always let your terminal send a linefeed after the example string
       to trigger the menu actions.

       Spaces in the strings can be omitted.



   Example 1:  'D13 OH' switch LED on   (D select pin, O output, H high)
               'L'      off again       (L low)


   Example 2:  writing high to an input activates internal pullup resistor

	       'D13 IH' pullup LED pin  (D select pin, I input, H high)

                                        LED glows at low level
               'O'      LED as OUTPUT   now the LED is on


   Example 3:  Watch an analog input like a VU meter, changing over time.
               See electric noise on unconnected floating A0 input
               scrolling over your serial terminal.
               (Touch the input if there is no visible signal.)

               Or connect a sensor to the input and explore its data...

               'A0 v'      A=select pin (both analog or digital)
                           v=display analog read values and bar graphs

			   Stop it by pressing 'v' again.


*A0	77	*****
*A0	63	****
*A0	74	*****
*A0	84	******
*A0	115	********
*A0	165	***********
*A0	237	***************
*A0	289	*******************
*A0	285	******************
*A0	241	****************
*A0	159	**********
*A0	97	*******
*A0	70	*****
*A0	63	****
*A0	76	*****
*A0	86	******
*A0	123	********
*A0	171	***********
*A0	248	****************
*A0	294	*******************
*A0	280	******************
*A0	227	***************
*A0	144	**********
*A0	87	******
*A0	66	*****



   Example 4:  Listen to an analog input value changing the pitch of a piezzo tone.
   	       This example does a similar thing as #3 playing an analog value back
	       as the pitch of a sound on a Piezzo connected from pin 8 to ground:

   	       'A0 T8'	Select analog input and the pin for the piezzo.
	       't'	toggles the tone on and off.

	       You will hear a sound on the piezzo now. The pitch will change
	       according to the reading on the analog pin.


   Example 5:  ','  Display snapshot values of all analog inputs.

                        [fixed font only]

pin     value   |                               |                               |
*A0     609     ***************************************
 A1     504     ********************************
 A2     451     *****************************
 A3     398     *************************
 A4     383     ************************
 A5     409     **************************



   Example 6:  '.'  Info on all digital pin's I/O configuration and state.

 pin 0	I  floating
 pin 1	I  floating
 pin 2	O  LOW
 pin 3	O  HIGH
 pin 4	I  floating
*pin 5	I  pullup       // * indicates the selected pin
 pin 6	I  floating
 pin 7	I  floating
 pin 8	I  floating
 pin 9	I  floating
 pin 10	I  floating
 pin 11	I  floating
 pin 12	I  floating
 pin 13	I  floating

   btw: The example output was generated after 'D2OLD3OHD5IH'.
        'D2OL D3OH D5IH' is the same, but easier to read.



   Copyright © Robert Epprecht  www.RobertEpprecht.ch   GPLv2

   http://github.com/reppr/pulses

*/
/* **************************************************************** */
