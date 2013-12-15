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

unsigned long selected_inputs=0;

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
void inputs_display();			// defined later on
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
  INPUTS.setup_sample_method(inp, &analogRead_, 2, 8);		// A2, oversample=8
  INPUTS.setup_linear(inp++, 0, 255, 1023, 0, PROPORTIONAL);	// 255*x/1023

  INPUTS.setup_sample_method(inp, &analogRead_, 3, 4);		// A3, oversample=4
  INPUTS.setup_linear(inp++, -20, 4800, 1, 0, INVERSE);		// 4800/(x-20)

  INPUTS.setup_sample_method(inp, &analogRead_, 5, 16);		// A5, oversample=16
  INPUTS.setup_linear(inp++, -11, 12, 16, -8, INVERSE);		// (x-11)12/16/(x-11) -8

  /* test Inputs::set_xxx() functions:
  // ATTENTION: nonsence settings!
  inp=6;
  INPUTS.set_counter(inp, 11);
  INPUTS.set_flags(inp, 255);		// no no
  INPUTS.set_inp_A(inp, 33);
  INPUTS.set_inp_B(inp, 44);
//  INPUTS.set_oversample(inp, 55);	// outch ;)  Do not do this...
  INPUTS.set_in_offset(inp, 66);
  INPUTS.set_mul(inp, -77);
  INPUTS.set_div(inp, 88);
  INPUTS.set_out_offset(inp, 9999);
  INPUTS.set_out_A(inp, 10);
  INPUTS.set_out_B(inp, 111);
  */

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


// display function for the inputs_menu page:
void inputs_display() {
  int max = INPUTS.get_inputs_allocated();

  MENU.out(F("Inputs "));
  MENU.out(INPUTS.get_inputs_allocated());
  MENU.tab();
  MENU.out(sizeof(input_t));
  MENU.outln(F(" bytes RAM each"));


  MENU.outln(F("'i'=info"));

  MENU.out(F("Select with "));
  for (int inp=0; inp < INPUTS.get_inputs_allocated(); inp++) {
    if (inp == 16)	// not a hex chiffre any more
      break;

    MENU.out_hex_chiffre(inp);
    MENU.space();
  }
  MENU.tab();
  MENU.outln(F("'~'=invert  'x'=clear"));

  if(selected_inputs) {
    MENU.outln(F("Set parameters with  A B + * / > O Q #"));
    MENU.outln(F("t<n>=test T=tests"));
  }
}


// reaction function for the inputs_menu page:
bool inputs_reaction(char token) {
  bool was_no_selection = (selected_inputs == 0);
  long newValue;
  unsigned long bitmask;
  int bit;

  switch (token) {
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
    bit = token -'0';
    if (bit >= INPUTS.get_inputs_allocated())
      return false;	// *not* responsible

    // existing input:
    selected_inputs ^= (1 << bit);
    if (MENU.verbosity >= VERBOSITY_SOME)
      print_selected_inputs();
    break;

  case 'a':
  case 'b':
  case 'c':
  case 'd':
  case 'e':
  case 'f':
    bit = token -'a' + 10;
    if (bit >= INPUTS.get_inputs_allocated())
      return false;	// *not* responsible

    // existing input:
    selected_inputs ^= (1 << bit);
    if (MENU.verbosity >= VERBOSITY_SOME)
      print_selected_inputs();
    break;

  case '~':
    selected_inputs = ~selected_inputs;
    bitmask=0;
    for (int inp=0; inp < INPUTS.get_inputs_allocated(); inp++)
      bitmask |= (1 << inp);
    selected_inputs &= bitmask;
    if (MENU.verbosity >= VERBOSITY_SOME)
      print_selected_inputs();
    break;

  case 'x':
    selected_inputs = 0;
    if (MENU.verbosity >= VERBOSITY_SOME)
      print_selected_inputs();
    break;

  case 'A':
    newValue = MENU.numeric_input(ILLEGAL);
    if (newValue == (uint8_t) newValue) {
      for (int inp=0; inp < INPUTS.get_inputs_allocated(); inp++)
	if (selected_inputs & ( 1 << inp))
	  INPUTS.set_inp_A(inp, newValue);
      if (MENU.verbosity >= VERBOSITY_SOME)
	inputs_info();
    } else MENU.OutOfRange();
    break;

  case 'B':
    newValue = MENU.numeric_input(ILLEGAL);
    if (newValue == (uint8_t) newValue) {
      for (int inp=0; inp < INPUTS.get_inputs_allocated(); inp++)
	if (selected_inputs & ( 1 << inp))
	  INPUTS.set_inp_B(inp, newValue);
      if (MENU.verbosity >= VERBOSITY_SOME)
	inputs_info();
    } else MENU.OutOfRange();
    break;

  case '+':
    if (!selected_inputs)
      return false;			// unhide verbosity '+'

    newValue = MENU.numeric_input(ILLEGAL);
    if (newValue == (ioP_t) newValue) {
      for (int inp=0; inp < INPUTS.get_inputs_allocated(); inp++)
	if (selected_inputs & ( 1 << inp))
	  INPUTS.set_in_offset(inp, newValue);
      if (MENU.verbosity >= VERBOSITY_SOME)
	inputs_info();
    } else MENU.OutOfRange();
    break;

  case '*':
    newValue = MENU.numeric_input(ILLEGAL);
    if (newValue == (ioP_t) newValue) {
      for (int inp=0; inp < INPUTS.get_inputs_allocated(); inp++)
	if (selected_inputs & ( 1 << inp))
	  INPUTS.set_mul(inp, newValue);
      if (MENU.verbosity >= VERBOSITY_SOME)
	inputs_info();
    } else MENU.OutOfRange();
    break;

  case '/':
    newValue = MENU.numeric_input(ILLEGAL);
    if (newValue == (ioP_t) newValue) {
      for (int inp=0; inp < INPUTS.get_inputs_allocated(); inp++)
	if (selected_inputs & ( 1 << inp))
	  INPUTS.set_div(inp, newValue);
      if (MENU.verbosity >= VERBOSITY_SOME)
	inputs_info();
    } else MENU.OutOfRange();
    break;

  case '>':
    newValue = MENU.numeric_input(ILLEGAL);
    if (newValue == (ioV_t) newValue) {
      for (int inp=0; inp < INPUTS.get_inputs_allocated(); inp++)
	if (selected_inputs & ( 1 << inp))
	  INPUTS.set_out_offset(inp, newValue);
      if (MENU.verbosity >= VERBOSITY_SOME)
	inputs_info();
    } else MENU.OutOfRange();
    break;

  case 'O':
    newValue = MENU.numeric_input(ILLEGAL);
    if (newValue == (uint8_t) newValue) {
      for (int inp=0; inp < INPUTS.get_inputs_allocated(); inp++)
	if (selected_inputs & ( 1 << inp))
	  INPUTS.set_out_A(inp, newValue);
      if (MENU.verbosity >= VERBOSITY_SOME)
	inputs_info();
    } else MENU.OutOfRange();
    break;

  case 'Q':
    newValue = MENU.numeric_input(ILLEGAL);
    if (newValue == (uint8_t) newValue) {
      for (int inp=0; inp < INPUTS.get_inputs_allocated(); inp++)
	if (selected_inputs & ( 1 << inp))
	  INPUTS.set_out_B(inp, newValue);
      if (MENU.verbosity >= VERBOSITY_SOME)
	inputs_info();
    } else MENU.OutOfRange();
    break;

  case '#':
    newValue = MENU.numeric_input(ILLEGAL);
    if (newValue == (unsigned long) newValue) {
      for (int inp=0; inp < INPUTS.get_inputs_allocated(); inp++)
	if (selected_inputs & ( 1 << inp))
	  INPUTS.set_counter(inp, newValue);
      if (MENU.verbosity >= VERBOSITY_SOME)
	inputs_info();
    } else MENU.OutOfRange();
    break;


  case 'i':
    inputs_info();
    MENU.ln();
    break;

  case 't':
    newValue = MENU.numeric_input(0);
    if (newValue == (int) newValue) {
      for (int inp=0; inp < INPUTS.get_inputs_allocated(); inp++)
	if (selected_inputs & ( 1 << inp))
	  test_in2o_calculation(inp, (int) newValue);
    } else MENU.OutOfRange();
    break;

  case 'T':
    for (int newValue=0; newValue<1024; newValue++)
      for (int inp=0; inp < INPUTS.get_inputs_allocated(); inp++)
	if (selected_inputs & ( 1 << inp))
	  test_in2o_calculation(inp, (int) newValue);
    break;


  default:
    return false;	// token not found in this menu
  }
  if (was_no_selection)	// some menu items have not been displayed before
    if (selected_inputs) {
      if (MENU.verbosity >= VERBOSITY_SOME) {
	inputs_info();		// nice to see
	MENU.menu_display();	// display full menu now
      }
    }
  return true;		// token found in this menu
}


// show an info line for each existing input:
void input_info(int inp) {
  MENU.space();
  if (selected_inputs & (1 << inp))
    MENU.out('*');			// '*' means selected
  else
    MENU.space();

  if (inp < 10)				// left side padding
    MENU.space();
  MENU.out(inp);
  MENU.out('/');
  MENU.out(INPUTS.get_inputs_allocated());

  MENU.out_flags_();
  MENU.outBIN(INPUTS.get_flags(inp), 16);
  MENU.tab();

  MENU.out(F("iA="));
  MENU.pad(INPUTS.get_inp_A(inp), 4);

  MENU.out(F("iB="));
  MENU.pad(INPUTS.get_inp_B(inp), 4);

  MENU.out(F("smp="));
  MENU.pad(INPUTS.get_oversample(inp),4);

  MENU.out('+');
  MENU.pad(INPUTS.get_in_offset(inp), 6);

  MENU.out('*');
  MENU.pad(INPUTS.get_mul(inp), 6);

  MENU.out('/');
  MENU.pad(INPUTS.get_div(inp), 6);

  MENU.out(F(">+"));
  MENU.pad(INPUTS.get_out_offset(inp), 6);

  MENU.out(F("O="));
  MENU.pad(INPUTS.get_out_A(inp), 4);

  MENU.out(F("Q="));
  MENU.pad(INPUTS.get_out_B(inp), 4);

  MENU.out('#');
  MENU.out(INPUTS.get_counter(inp));	// not padded

//  INPUTS.get_out_reaction(inp);

  MENU.ln();
}


// show info lines of all existing inputs:
void inputs_info() {
  MENU.ln();
  int inputs=INPUTS.get_inputs_allocated();
  for (int inp=0; inp < inputs; inp++)
    input_info(inp);
}


int analogRead_(int pin) {	// horrible kludge, we need the type cast here...
  return analogRead(pin);
}


// show which inputs are selected in the menu:
void print_selected_inputs() {
  const int inputs=INPUTS.get_inputs_allocated();
  const int hex_inputs=min(inputs, 16);	// displayed as hex chiffres
  MENU.out_selected_();
  for (int inp=0; inp < hex_inputs; inp++) {
    if (selected_inputs & (1 << inp))
      MENU.out_hex_chiffre(inp);
    else
      MENU.out('.');
  }

  MENU.space();
  for (int inp=16; inp < inputs; inp++) {
    if (selected_inputs & (1 << inp))
      MENU.out('+');
    else
      MENU.out('.');
  }
  MENU.ln();
}


// show what in2o_calculation on inp does with value:
void test_in2o_calculation(int inp, int value) {
  MENU.pad(inp, 4);
  MENU.out(F("in="));
  MENU.out(value);
  MENU.out(F("\tout="));
#ifdef I2O_PARAMETER_IS_FLOAT
      Serial.println(INPUTS.in2o_calculation(inp, value), 4);
#else
      Serial.println(INPUTS.in2o_calculation(inp, value));
#endif
}
