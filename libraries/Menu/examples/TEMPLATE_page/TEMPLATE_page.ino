#error "sorry this module is temporary switched off"
// REPLACE *ALL* STRINGS 'TEMPLATE' with the NAME OF YOUR MENU PAGE!
/*
 * ****************************************************************
 * TEMPLATE_page.ino
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
Inputs INPUTS(8);


/* **************************************************************** */
// DADA TODO:  #error "sorry this module is temporary switched off"

/*
  This version definines the menu INPUT routine int men_getchar();
  in the *program* not inside the Menu class.
*/
int men_getchar() {	// returns EOF32
  if (!Serial.available())	// ARDUINO
    return EOF32;

  return Serial.read();
}

Menu MENU(32, 1, &men_getchar, Serial);
/* **************************************************************** */


// needed for MENU.add_page() in setup()
void TEMPLATE_display();		// defined later on
bool TEMPLATE_reaction(char token);	// defined later on


/* ****   CODE TO INSERT TO THE START OF YOUR PROGRAM ends:    **** */
/* **************************************************************** */



/* **************************************************************** */
/* CODE TO INSERT INTO YOUR PROGRAM setup() follows: ************** */
/* Arduino setup()						*/

#ifdef ARDUINO
// Arduino setup() template:
void setup() {	// ARDUINO
  Serial.begin(BAUDRATE);	// Start serial communication.
#ifdef __AVR_ATmega32U4__
  /* on ATmega32U4		Leonardo, Mini, LilyPad Arduino USB
     to be able to use Serial.print() from setup()
     we *must* do that before:
  */
  while (!Serial) { ;}		// wait for Serial to open
#endif

  // add TEMPLATE_page to MENU:
  #define HOTKEY	'M'
  #define VISABILITY_GROUP	'M'
  MENU.add_page("TEMPLATE", HOTKEY,	\
		&TEMPLATE_display, &TEMPLATE_reaction, VISABILITY_GROUP);

  MENU.menu_display();		// display menu at startup
}
#endif

/* CODE TO INSERT INTO YOUR PROGRAM setup() ends. ***************** */
/* **************************************************************** */


/* **************************************************************** */
/* CODE TO INSERT INTO YOUR PROGRAM loop() follows: *************** */
/* Arduino loop()						*/

#ifdef ARDUINO
// Arduino loop() template:
void loop() {	// ARDUINO
  MENU.lurk_then_do();

  // Insert your own code here.
  // Do not let your program block program flow,
  // always return to the main loop as soon as possible.
}
#endif

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
      #define NUM_DIGITAL_PINS	79		// FIXME: DUE ################
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
  #else						// FIXME: 168/328 boards ???
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
/* ****  CODE TO WRITE YOURSELF follows:  **** */

long a_program_variable=42;	// just for a demo, REPLACE

void TEMPLATE_display() {	// just for a demo, REPLACE
  MENU.outln(F("TEMPLATE_display INSERT *YOUR* CODE HERE..."));
  MENU.outln(F("show possible actions and the key to trigger it"));
  MENU.ln();
  MENU.out(F("'X' to do action X\tV<number> to set a program variable, currently="));
  MENU.outln(a_program_variable);
}

bool TEMPLATE_reaction(char token) {	 // just for a demo, REPLACE
  long newValue;

  switch (token) {
  case 'X':	// EXAMPLE: code to do the action you want on pressing 'X'
    MENU.outln(F("you selected item X"));
    break;

  case 'V':	// EXAMPLE: say V66 to set a variable to 66
    newValue = MENU.numeric_input(a_program_variable);
    a_program_variable=newValue;
    MENU.out(F("Set variable to "));
    MENU.out(a_program_variable);
    MENU.menu_display();	// show changes
    break;

  default:
    return false;	// token not found in this menu
  }
  return true;		// token found in this menu
}
