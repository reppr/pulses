/* **************************************************************** */
/*
			Softboard

            Software breadboard for the Arduino.

            http://github.com/reppr/pulses/
        Given as an example for the Menu library.


 (New library version of http://github.com/reppr/softboard)


    Please do read the README, at the end of this file



Copyright © Robert Epprecht  www.RobertEpprecht.ch   GPLv2

*/
/* **************************************************************** */


/*  GNU General Public License GPLv2

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
// SOURCE CODE STARTS HERE:
/* **************************************************************** */

#include <stdio.h>
#include <stdlib.h>

#ifndef ARDUINO
  #error Arduino only
#endif

/* Keep ARDUINO GUI happy ;(		*/
#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#undef SHOW_FREE_RAM		// softboard does this by default
#include <Menu.h>

/* BAUDRATE for Serial:	uncomment one of the following lines:	*/
#define BAUDRATE	115200		// works fine here
//#define BAUDRATE	57600
//#define BAUDRATE	38400
//#define BAUDRATE	19200
//#define BAUDRATE	9600		// failsafe
//#define BAUDRATE	31250		// MIDI


/* **************************************************************** */
/*
  This version definines the menu INPUT routine int men_getchar();
  in the *program* not inside the Menu class.
*/
int men_getchar() {
  if (!Serial.available())	// ARDUINO
    return EOF;

  return Serial.read();
}

Menu MENU(32, 1, &men_getchar, Serial);


/* **************************************************************** */
/* Arduiono setup() and loop():					*/


void setup() {
  Serial.begin(BAUDRATE);	// Start serial communication.

  MENU.add_page("Arduino Hardware", 'H', \
		&softboard_display, &softboard_reaction, '+');

  MENU.outln(F("Arduino Softboard\nhttp://github.com/reppr/pulses/\n"));

  pins_info_digital(); MENU.ln();
  pins_info_analog();  MENU.ln();

  MENU.menu_display();		// display menu at startup
}


/*
  All you have to from your Arduino sketch loop() is to call:
  MENU.lurk_then_do();
  This will *not* block the loop.

  It will lurk on menu input, but return immediately if there's none.
  On input it will accumulate one next input byte and return until it
  will see an 'END token'.

  When seeing one of the usual line endings it takes it as 'END token' \0.

  On receiving an 'END token' it will interpret the accumulated tokens and return.
*/

void loop() {	// ARDUINO
  MENU.lurk_then_do();

  // Insert your own code here.
  // Do not let your program block program flow,
  // always return to the main loop soon.
}


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

/* **************************************************************** */
/* ARDUINO BOARD SPECIFIC THINGS  try to use ARDUINO MACROS: */

#ifndef NUM_DIGITAL_PINS		// try harder... ?
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
  #if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) // mega boards
    #define NUM_ANALOG_INPUTS	16
  #else								// 168/328 boards
    #define NUM_ANALOG_INPUTS	6
  #endif

  #ifndef NUM_ANALOG_INPUTS
    #error #define NUM_ANALOG_INPUTS
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


// The following defaults *must* match with each other, choose one pair.
// either:
  bool extra_switch=false;	// extra functionality on digital pins
  int visible_digital_pins=DIGITAL_IOs;		// extra_switch==false
// or:
  // bool extra_switch=false;	// extra functionality on digital pins
  // int visible_digital_pins=NUM_DIGITAL_PINS;	// extra_switch==true

bool echo_switch=true;		// serial echo switch


/* **************************************************************** */
/*
  serial_print_BIN(unsigned long value, int bits)
  print binary numbers with leading zeroes and a trailing space
*/
void serial_print_BIN(unsigned long value, int bits) {
  int i;
  unsigned long mask=0;

  for (i = bits - 1; i >= 0; i--) {
    mask = (1 << i);
      if (value & mask)
	MENU.out('1');
      else
	MENU.out('0');
  }
  MENU.space();
}


/* ****************  info on DIGITAL pins:  **************** */
/*
  pin_info_digital()
  display configuration and state of a pin:
*/
const char pin_[] = "pin ";
const char high_[] = "HIGH";
const char low_[] = "LOW";

void pin_info_digital(uint8_t pin) {
  uint8_t mask = digitalPinToBitMask(pin);
  uint8_t port = digitalPinToPort(pin);
  volatile uint8_t *reg;

  if (port == NOT_A_PIN) return;

  // selected sign * and pin:
  if (pin == PIN_digital )
    MENU.out('*');
  else
    MENU.space();
  MENU.out(pin_);
  MENU.out((int) pin);
  MENU.tab();

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
      MENU.out(high_);
    else
      MENU.out(low_);
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



// #ifdef HARDWARE_menu	// inside SERIAL_MENU
/* ****************  info on ANALOG pins:  **************** */

// display analog pin name and value as number and bar graph: 
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
  display analog snapshot read values and bar graphs, a line each analog input:
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


// #ifdef HARDWARE_menu	 // inside  #ifdef SERIAL_MENU
/* **************************************************************** */
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



// #ifdef HARDWARE_menu	 // inside  #ifdef SERIAL_MENU
/* **************************************************************** */
/*
  running 'continuous' display types or similar
  implemented without waiting allows 'run-through' programming.
*/
bool run_VU=false;


void stop_continuous() {
  run_VU=false;
  run_watch_dI=false;
}


void maybe_run_continuous() {
  if (run_VU)
    bar_graph_VU(PIN_analog);

  if (run_watch_dI)
    watch_digital_input(PIN_digital);
}



// #ifdef HARDWARE_menu	// inside SERIAL_MENU
/* **************************************************************** */
/*
  bar_graph_VU(pin)   continuous display of changes on an analogue input.
  display a scrolling bar graph over the readings
  a new line is displayed as soon as the reading changes for more
  then +/- tolerance.

  tolerance can be changed by sending '+' or '-'

  run-through, don´t wait...
*/


int VU_last=IMPOSSIBLE;


// tolerance default 0. Let the user *see* the noise...
int bar_graph_tolerance=0;

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


// bar_graph_feedback(pin), info line for bar graph:


void feedback_tolerance() {
  MENU.out(F("tolerance "));
  MENU.outln(bar_graph_tolerance);
}


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


// #ifdef HARDWARE_menu	// inside SERIAL_MENU
/* **************************************************************** */
/*
  determine RAM usage:
  int get_free_RAM() {

  wee use this here below
*/
extern int __bss_end;
extern void *__brkval;

int get_free_RAM() {
  int free;

  if ((int) __brkval == 0)
    return ((int) &free) - ((int) &__bss_end);
  else
    return ((int) &free) - ((int) __brkval);
}



// #ifdef HARDWARE_menu	// inside SERIAL_MENU
// ****************************************************************
// menu hardware display and reaction:


/* ****************  hardware menu display:  **************** */
const char hwMenuTitle[] = \
  "\n*** HARDWARE MENU ***\t\tfree RAM=";
const char select_[] = "select ";
const char pinFor[] = " pin for ";
const char toWork_[] = " to work on:\t";
const char pin__[] = "pin (";
const char close_[] = ")";


// factored out display functions:

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
    MENU.out(F("A="));
  MENU.out(select_);
  MENU.out(F("analog"));
  MENU.out(pinFor);
  MENU.out(F("'a, v'"));
}


void softboard_display() {
  MENU.out(hwMenuTitle);
  // free RAM=
  MENU.outln(get_free_RAM());
  MENU.ln();

  _select_digital(true);
  MENU.out(toWork_);
  MENU.out(pin__);
  if (PIN_digital == ILLEGAL)
    MENU.out(F("no"));
  else
    MENU.out((int) PIN_digital);
  MENU.outln(close_);
  MENU.out(F("O=OUTPUT\tI=INPUT\t\tH=HIGH\tL=LOW\tPWM: W=WRITE\td=pin info"));
  MENU.ln();

  _select_analog(true);
  MENU.out(toWork_);
  MENU.out(pin__);
  MENU.out((int) PIN_analog);
  MENU.outln(close_);
  MENU.out(F("watch over time:\tv=VU bar\tr=read"));
  MENU.ln();

  MENU.outln(F(".=all digital\t,=all analog\t;=both\tx=extra"));
}



// #ifdef HARDWARE_menu	// inside SERIAL_MENU
/* ****************  menu hardware reactions:  **************** */

/*
  toggle extra functionality
  * visability of analog inputs as general digital I/O pins
    (only this one item implemented)
*/
void toggle_extra() {
  extra_switch = !extra_switch;
  if (extra_switch)
    visible_digital_pins=NUM_DIGITAL_PINS;
  else
    visible_digital_pins=DIGITAL_IOs;
}


// factored out helper function
// digital_pin_ok()  checks if PIN_digital has been set
bool digital_pin_ok() {
  // testing on ILLEGAL is enough in this context
  if (PIN_digital == ILLEGAL) {
    _select_digital(true);
    MENU.ln();
    return false;
  } else
    return true;
}

// bool menu_hardware_reaction(menu_input)
// try to react on menu_input, return success flag
const char pwm_[] = "PWM ";
const char noHw_[] = "no hardware ";
const char analogWrite_[] = "\tanalogWrite(";

bool softboard_reaction(char menu_input) {
  long newValue;
  bool do_it;	// sorry for the hack...

  switch (menu_input) {
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
      do_it=true;	// hack to get through the #ifdef

#ifndef digitalPinHasPWM
  #ifdef MACROS_STRICT
    #error digitalPinHasPWM undefined
  #endif
#else

      if (!digitalPinHasPWM(PIN_digital)) {
	do_it=false;	// hack to get through the #ifdef
	MENU.out(noHw_); MENU.out(pwm_);
	MENU.ln();
	MENU.skip_numeric_input();
      }

#endif

      if (do_it) {	// hack to get through the #ifdef
        MENU.out(pin_); MENU.out((int) PIN_digital);
	MENU.tab();
	MENU.out(pwm_); MENU.out(F("write "));
	newValue = MENU.numeric_input(ILLEGAL);
	if (newValue>=0 && newValue<=255) {
	  MENU.out(newValue);

	  analogWrite(PIN_digital, newValue);
	  MENU.out(F("\tanalogWrite(")); MENU.out((int) PIN_digital);
	  MENU.out(F(", ")); MENU.out(newValue);
	  MENU.outln(close_);
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

  case '+':	// we *could* do  'if (run_VU)' here
    bar_graph_tolerance++;
    feedback_tolerance();
    break;

  case '-':	// we *could* do  'if (run_VU)' here
    if (bar_graph_tolerance)
      bar_graph_tolerance--;
    feedback_tolerance();
    break;

  case 'r':
    if (digital_pin_ok()) {
      toggle_watch();
      // pinMode(PIN_digital, INPUT);	// ################
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
    MENU.ln(); MENU.outln(close_);
    break;

    case 'x':	// toggle extended
      toggle_extra();
      break;

  default:
    return 0;		// menu_input not found in this menu
  }
  return 1;		// menu_input found in this menu
}



/* **************************************************************** */
/* README

README softboard

   softboard  http://github.com/reppr/pulses

   Arduino hardware/software developing/testing tool.

Description

   Simple hardware menu interface over serial line
   as a kind of arduino software breadboard.

   Send a sequence of one letter commands and numeric chiffres
   followed by a linebreak over a serial line (say from your computer)
   to the arduino to trigger menu actions and get infos.

   The arduino will buffer serial input (without waiting for it) until
   a terminating linefeed is received as an end token.
   Any sequence of one or more '\n'  '\c'  '\0' accepted as end token.


   Commands can read and set I/O pin configuration and states,
   switch pins on/off, read and write digital and analog values,
   switch pullup/high-z, or continuously watch changing inputs over time.

   Use it to test hardware like sensors, motors, things you want
   to run quick tests after setting something up on a (real) breadboard.

   Then you can use it to test software parts of your program while
   you write it and fit parts together. 


   Send 'm' or '?' (and a linefeed) over serial line to see the menu.
   It displays some basic infos and shows some one-letter commands.


'run-through' cpu time friendly implementation:

   Softboard tries not to block the processor longer then needed and
   to let as much cpu time as possible to any other code possibly running.
   serial_menu() will just run through it's code and return immediately.


Integrate your own code:

   It is easy to write a program menu as interface to your own code.

   Grep the source for ´dance´ or ´yodel´ and you might find hints how to
   setup a program menu to tell the arduino sketch to dance on ´d´,
   to yodel on ´y´  and on ´s´ to do something special ;)

   Just do #define PROGRAM_menu by uncommenting the line further down
   and you will get an example program menu to build upon.

   Or look in the hardware menu as example how to use serial menu.


Installation:

   Get it from http://github.com/reppr/softboard
   Move the 'softboard' folder with all files
   to your arduino sketchbook/ folder.
   Load the sketch from arduino menu Files>Sketchbook>softboard.

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

   The minimalistic menu shows you one letter commands and listens to
   your input. Serial inputs are buffered until you send a linefeed.
   Then your inputs (commands and numbers as sequences of chiffres)
   will be read and acted upon.


Configure your terminal program:

   Do configure your terminal emulation program to send some sort of
   line ending code, usual culprits should work.

   Set arduino baud rate by editing the line starting with
   #define USE_SERIAL_BAUD
   and set it to the same value in your terminal software.

   So if for example you use the Arduino 'Serial Monitor' window
   check that it *does* send 'Newline' (in the bottom window frame)
   and set baud rate to the same value as USE_SERIAL_BAUD on the arduino.


   Send 'm' or '?' (and a linefeed) over serial line to see the menu.
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



   Example 4:  ','  Display snapshot values of all analog inputs.

                        [fixed font only]

pin     value   |                               |                               |
*A0     609     ***************************************
 A1     504     ********************************
 A2     451     *****************************
 A3     398     *************************
 A4     383     ************************
 A5     409     **************************



   Example 5:  '.'  Info on all digital pin's I/O configuration and state.

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
