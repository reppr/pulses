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
// debugging:
// #define INPUTS_DEBUGGING_SAMPLE_REACTION



/* **************************************************************** */
// INPUTS:
Inputs INPUTS(12);

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

  inputs_info();
}
#endif


/* **************************************************************** */
// Arduino loop():

#ifdef ARDUINO
// Arduino loop():
unsigned int cnt=0;
int inp=0;
void loop() {	// ARDUINO
  MENU.lurk_then_do();

  // Insert your own code here.
  // Do not let your program block program flow,
  // always return to the main loop as soon as possible.
  inp = (cnt % INPUTS.get_inputs_allocated());	// cycle through the inputs
  INPUTS.sample_and_react(inp);
  cnt++;
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
  MENU.out(F("Inputs "));
  MENU.out(INPUTS.get_inputs_allocated());
  MENU.tab();
  MENU.out(sizeof(input_t));
  MENU.outln(F(" bytes RAM each"));


  MENU.outln(F("'i'=info"));

  how_to_select();

  if(selected_inputs) {
    MENU.outln(F("Set parameters with  A B S + * / > O Q #"));
    MENU.outln(F("!=activate s=samples t<n>=test T=tests"));
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

  case 'N':
    newValue = MENU.numeric_input(ILLEGAL);
    if ((newValue >= 0) && (newValue < INPUTS.get_inputs_allocated())) {
      selected_inputs = 1 << newValue;
    if (MENU.verbosity >= VERBOSITY_SOME)
      print_selected_inputs();
    } else MENU.OutOfRange();

    break;


  case 'A':
    if(anything_selected()) {	// if not, tell the user how to select
      newValue = MENU.numeric_input(ILLEGAL);
      if (newValue == (uint8_t) newValue) {
	for (int inp=0; inp < INPUTS.get_inputs_allocated(); inp++)
	  if (selected_inputs & ( 1 << inp))
	    INPUTS.set_inp_A(inp, newValue);
	if (MENU.verbosity >= VERBOSITY_SOME)
	  inputs_info();
      } else MENU.OutOfRange();
    }
    break;

  case 'B':
    if(anything_selected()) {	// if not, tell the user how to select
      newValue = MENU.numeric_input(ILLEGAL);
      if (newValue == (uint8_t) newValue) {
	for (int inp=0; inp < INPUTS.get_inputs_allocated(); inp++)
	  if (selected_inputs & ( 1 << inp))
	    INPUTS.set_inp_B(inp, newValue);
	if (MENU.verbosity >= VERBOSITY_SOME)
	  inputs_info();
      } else MENU.OutOfRange();
    }
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
    if(anything_selected()) {	// if not, tell the user how to select
      newValue = MENU.numeric_input(ILLEGAL);
      if (newValue == (ioP_t) newValue) {
	for (int inp=0; inp < INPUTS.get_inputs_allocated(); inp++)
	  if (selected_inputs & ( 1 << inp))
	    INPUTS.set_mul(inp, newValue);
	if (MENU.verbosity >= VERBOSITY_SOME)
	  inputs_info();
      } else MENU.OutOfRange();
    }
    break;

  case '/':
    if(anything_selected()) {	// if not, tell the user how to select
      newValue = MENU.numeric_input(ILLEGAL);
      if (newValue == (ioP_t) newValue) {
	for (int inp=0; inp < INPUTS.get_inputs_allocated(); inp++)
	  if (selected_inputs & ( 1 << inp))
	    INPUTS.set_div(inp, newValue);
	if (MENU.verbosity >= VERBOSITY_SOME)
	  inputs_info();
      } else MENU.OutOfRange();
    }
    break;

  case '>':
    if(anything_selected()) {	// if not, tell the user how to select
      newValue = MENU.numeric_input(ILLEGAL);
      if (newValue == (ioV_t) newValue) {
	for (int inp=0; inp < INPUTS.get_inputs_allocated(); inp++)
	  if (selected_inputs & ( 1 << inp))
	    INPUTS.set_out_offset(inp, newValue);
	if (MENU.verbosity >= VERBOSITY_SOME)
	  inputs_info();
      } else MENU.OutOfRange();
    }
    break;

  case 'O':
    if(anything_selected()) {	// if not, tell the user how to select
      newValue = MENU.numeric_input(ILLEGAL);
      if (newValue == (uint8_t) newValue) {
	for (int inp=0; inp < INPUTS.get_inputs_allocated(); inp++)
	  if (selected_inputs & ( 1 << inp))
	    INPUTS.set_out_A(inp, newValue);
	if (MENU.verbosity >= VERBOSITY_SOME)
	  inputs_info();
      } else MENU.OutOfRange();
    }
    break;

  case 'Q':
    if(anything_selected()) {	// if not, tell the user how to select
      newValue = MENU.numeric_input(ILLEGAL);
      if (newValue == (uint8_t) newValue) {
	for (int inp=0; inp < INPUTS.get_inputs_allocated(); inp++)
	  if (selected_inputs & ( 1 << inp))
	    INPUTS.set_out_B(inp, newValue);
	if (MENU.verbosity >= VERBOSITY_SOME)
	  inputs_info();
      } else MENU.OutOfRange();
    }
    break;

  case 'S':
    if(anything_selected()) {	// if not, tell the user how to select
      newValue = MENU.numeric_input(ILLEGAL);
      if (newValue > 0) {
	for (int inp=0; inp < INPUTS.get_inputs_allocated(); inp++)
	  if (selected_inputs & ( 1 << inp))
	    if (!INPUTS.malloc_samples(inp, newValue)) {
	      MENU.OutOfRange();
	      break;
	    }
	if (MENU.verbosity >= VERBOSITY_SOME)
	  inputs_info();
      } else MENU.OutOfRange();
    }
    break;

  case '#':
    if(anything_selected()) {	// if not, tell the user how to select
      newValue = MENU.numeric_input(ILLEGAL);
      if (newValue == (unsigned long) newValue) {
	for (int inp=0; inp < INPUTS.get_inputs_allocated(); inp++)
	  if (selected_inputs & ( 1 << inp))
	    INPUTS.set_counter(inp, newValue);
	if (MENU.verbosity >= VERBOSITY_SOME)
	  inputs_info();
      } else MENU.OutOfRange();
    }
    break;


  case 's':
    if(anything_selected()) {	// if not, tell the user how to select
      for (int inp=0; inp < INPUTS.get_inputs_allocated(); inp++)
	if (selected_inputs & ( 1 << inp))
	  show_samples(inp);
    }
    break;

  case 'i':
    inputs_info();
    MENU.ln();
    break;

  case 't':
    if(anything_selected()) {	// if not, tell the user how to select
      newValue = MENU.numeric_input(0);
      if (newValue == (int) newValue) {
	for (int inp=0; inp < INPUTS.get_inputs_allocated(); inp++)
	  if (selected_inputs & ( 1 << inp))
	    test_in2o_calculation(inp, (int) newValue);
      } else MENU.OutOfRange();
    }
    break;

  case 'T':
    if(anything_selected()) {	// if not, tell the user how to select
      for (int newValue=0; newValue<1024; newValue++)
	for (int inp=0; inp < INPUTS.get_inputs_allocated(); inp++)
	  if (selected_inputs & ( 1 << inp))
	    test_in2o_calculation(inp, (int) newValue);
    }
    break;

  case '!':
    if(anything_selected()) {	// if not, tell the user how to select
      for (int inp=0; inp < INPUTS.get_inputs_allocated(); inp++) {
	if (selected_inputs & ( 1 << inp)) {
	  INPUTS.set_flags(inp, (INPUTS.get_flags(inp) ^ INPUT_ACTIVE));
	}
      }
      if (MENU.verbosity >= VERBOSITY_SOME)
	inputs_info();
    }
    break;

  default:
    return false;	// token not found in this menu
  }

  if (was_no_selection)	// some menu items have not been displayed before
    if (selected_inputs) {
      if (MENU.verbosity >= VERBOSITY_SOME)
	MENU.menu_display();	// display full menu now
    }
  return true;		// token found in this menu
}


void how_to_select(void) {	// inform user about keys for selection
  MENU.out(F("Select with "));
  MENU.out_ticked_hexs(INPUTS.get_inputs_allocated());
  MENU.space();
  MENU.outln(F("'~'=invert  'x'=clear  'N'<number>"));
}


// Check if there's something selected.
// If nothing is selected inform the user how to select items.
// return true if there *is*  a selection.
bool anything_selected(void) {
  if (selected_inputs)
    return true;

  how_to_select();
  return false;
}


// show an info line for an (existing) input:
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
  MENU.pad(INPUTS.get_oversample(inp),6);

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


// show what in2o_calculation [inp] does with value:
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


// show all samples from an input:
void show_samples(int inp) {
  unsigned int oversample = INPUTS.get_oversample(inp);
  if(oversample == 0)	// no sample memory allocated?
    return;		//   return silently

  int average = INPUTS.oversamples_average(inp);
  int deviation, sample;
  int last = ((INPUTS.get_counter(inp) - 1) % INPUTS.get_oversample(inp));

  MENU.ln();
  for(int s=0 ; s<oversample; s++) {
    if (s == last)
      MENU.out(F("=>"));	// "=>" sign on last sample
    else {
      MENU.space();
      MENU.space();
    }

    MENU.pad(inp, 4);
    MENU.pad(s, 4);
    sample=INPUTS.get_sample(inp, s);
    MENU.out(sample);
    MENU.tab();
    deviation = (sample - average);
    if (deviation>0)
      MENU.out('+');		// '+' sign on positive deviations
    else
      if (deviation==0)
	MENU.space();
    MENU.outln(deviation);
  }

  MENU.tab();
  MENU.space();
  MENU.space();
  MENU.out(average);
  MENU.tab();
// MENU.out('±');		// plusminus sign, does not work here...
  MENU.outln(INPUTS.oversamples_deviation(inp));
}


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
