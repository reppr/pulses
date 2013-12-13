/*
 * ****************************************************************
 * inputs_menu.ino
 * ****************************************************************
*/
/* **************************************************************** */

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

#include <Inputs.h>
#include <Menu.h>


/* **************************************************************** */
/* BAUDRATE for Serial:	uncomment one of the following lines:	*/
#define BAUDRATE	115200		// works fine here
//#define BAUDRATE	57600
//#define BAUDRATE	38400
//#define BAUDRATE	19200
//#define BAUDRATE	9600		// failsafe
//#define BAUDRATE	31250		// MIDI


/* **************************************************************** */
// INPUTS:
Inputs INPUTS(8);


/* **************************************************************** */
// MENU:
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


// needed for MENU.add_page() in setup()
void inputs_display();		// defined later on
bool inputs_reaction(char token);	// defined later on


/* **************************************************************** */
// Arduino setup():

#ifdef ARDUINO
void setup() {	// ARDUINO
  Serial.begin(BAUDRATE);	// Start serial communication.
#ifdef __AVR_ATmega32U4__
  /* on ATmega32U4		Leonardo, Mini, LilyPad Arduino USB
     to be able to use Serial.print() from setup()
     we *must* do that before:
  */
  while (!Serial) { ;}		// wait for Serial to open
#endif

  // add inputs_page to MENU:
  #define HOTKEY	'I'
  #define VISABILITY_GROUP	'-'
  MENU.add_page("INPUTS", HOTKEY,	\
		&inputs_display, &inputs_reaction, VISABILITY_GROUP);

  MENU.menu_display();		// display menu at startup
  MENU.ln();

  int inp=0;
  INPUTS.setup_sample_method(inp, &analogRead_, 2, 8);		// A2, oversampling=8
  INPUTS.setup_linear(inp++, 0, 255, 1023, 0, PROPORTIONAL);	// 255*x/1023

  INPUTS.setup_sample_method(inp, &analogRead_, 3, 4);		// A3, oversampling=4
  INPUTS.setup_linear(inp++, -20, 4800, 1, 0, INVERSE);		// 4800/(x-20)

  INPUTS.setup_sample_method(inp, &analogRead_, 5, 16);		// A5, oversampling=16
  INPUTS.setup_linear(inp++, -11, 12, 16, -8, INVERSE);		// (x-11)12/16/(x-11) -8
  inputs_info();
}
#endif


/* **************************************************************** */
// Arduino loop():

#ifdef ARDUINO
// Arduino loop() template:
void loop() {	// ARDUINO
  MENU.lurk_then_do();

  // Insert your own code here.
  // Do not let your program block program flow,
  // always return to the main loop as soon as possible.
}
#endif


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


/* **************************************************************** */
/* **************************************************************** */

void inputs_display() {
  MENU.out(F("Inputs "));
  MENU.outln(INPUTS.get_inputs_allocated());
  MENU.outln(F("i=info"));
}

bool inputs_reaction(char token) {	 // just for a demo, REPLACE
  long newValue;

  switch (token) {
  case 'i':
    MENU.ln();
    inputs_info();
    MENU.ln();
    break;

  default:
    return false;	// token not found in this menu
  }
  return true;		// token found in this menu
}


const char flags_[] = "\tflags ";

void input_info(int inp) {
  MENU.space();
  MENU.space();
  if (inp < 10)
    MENU.space();
  MENU.out(inp);
  MENU.out('/');
  MENU.out(INPUTS.get_inputs_allocated());
  MENU.out(flags_);
  MENU.outBIN(INPUTS.get_flags(inp), 16);
  MENU.tab();
  MENU.out(F("i@="));
  MENU.pad(INPUTS.get_input_addr(inp), 4);
  MENU.out(F("p2="));
  MENU.pad(INPUTS.get_input_p2(inp), 4);
  MENU.out(F("smp="));
  MENU.pad(INPUTS.get_oversampling(inp),4);
  MENU.out('+');
  MENU.pad(INPUTS.get_input_offset(inp), 6);
  MENU.out('*');
  MENU.pad(INPUTS.get_mul(inp), 6);
  MENU.out('/');
  MENU.pad(INPUTS.get_div(inp), 6);
  MENU.out('+');
  MENU.pad(INPUTS.get_output_offset(inp), 6);
  MENU.out(F("o@="));
  MENU.pad(INPUTS.get_output_addr(inp), 4);
  MENU.out(F("#="));
  MENU.out(INPUTS.get_counter(inp));	// not padded

//  INPUTS.get_out_reaction(inp);

  MENU.ln();
}

void inputs_info() {
  int inputs=INPUTS.get_inputs_allocated();
  for (int inp=0; inp < inputs; inp++)
    input_info(inp);
}

int analogRead_(int pin) {	// horrible kludge, we need the type cast here...
  return analogRead(pin);
}
