/* ****************************************************************

 * ****************************************************************
 *   inputs_menu.ino
 * ****************************************************************

 * **************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

/* **************************************************************** */
// INPUTS:
// debugging:
// #define INPUTS_DEBUGGING_SAMPLE_REACTION
/* **************************************************************** */
#include <Inputs.h>
Inputs INPUTS(12);

/* **************************************************************** */


#define USE_WIFI_telnet_menu	// ################ FIXME: always active WiFi
#include "menu_IO_configuration.h"
/*
  This version definines the menu INPUT routine int men_getchar();
  and the menu OUTPUT streams
  from the *program*
  not inside the Menu class
*/

#include <Menu.h>
Menu MENU(32, 1, &men_getchar, Serial, MENU_OUTSTREAM2);
/* **************************************************************** */
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
  INPUTS.setup_analog_read(inp, 0, 16);				// A0, oversample=16 new internal
  INPUTS.setup_raw(inp);					// no calculations

  inp++;
  INPUTS.setup_analog_read(inp, 1, 16);				// A1, oversample=16
  INPUTS.setup_raw(inp);					// no calculations

  inp++;
  INPUTS.setup_analog_read(inp, 2, 8);				// A2, oversample=8
  INPUTS.setup_linear(inp, 0, 255, 1023, 0, PROPORTIONAL);	// 255*x/1023
#ifdef INPUTS_DEBUGGING_SAMPLE_REACTION
  INPUTS.setup_raw(inp);
  INPUTS.setup_io_reaction(inp, &bar_graph_);	// obsolete
#endif

  inp++;							// Sharp HC-SR04
  INPUTS.setup_analog_read(inp, 3, 4);				// A3, oversample=4
  INPUTS.setup_linear(inp, -20, 4800, 1, 0, INVERSE);		// 4800/(x-20)	[formula often cited]

  /*
  inp++;
  INPUTS.setup_analog_read(inp, 4, 2);				// A4, oversample=2
  INPUTS.setup_raw(inp);					// no calculations

  inp++;
  INPUTS.setup_analog_read(inp, 5, 1);				// A5, oversample=1
  INPUTS.setup_raw(inp);					// no calculations
  */

  inp++;
  INPUTS.setup_analog_read(inp, 6, 32);				// A5, oversample=32
  // ################		FIXME: add analogWriteResolution(12) on the DUE
  INPUTS.setup_linear(inp, 0, 255, 1023, 0, PROPORTIONAL);
  INPUTS.setup_PWM(inp, 13);

  inputs_info();
}
#endif


/* **************************************************************** */
// Arduino loop():

#ifdef ARDUINO
// Arduino loop():
void loop() {	// ARDUINO
  MENU.lurk_then_do();
  INPUTS.do_next_input();

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


#ifdef INPUTS_DEBUGGING_SAMPLE_REACTION	// obsolete

/* **************************************************************** */
/*
  bar_graph_(int inp, ioV_t value);	// obsolete
  testing io_reaction:
  print one value & bar graph line
*/
// bar_graph_(inp, value)
void bar_graph_(int inp, ioV_t value) {	// obsolete
  MENU.pad(inp, 4);
  MENU.bar_graph(value, 1023, '*');
}

#endif // INPUTS_DEBUGGING_SAMPLE_REACTION
