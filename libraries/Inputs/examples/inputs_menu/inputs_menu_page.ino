/*
  inputs_menu_page.ino
*/

/* **************************************************************** */
#ifndef INPUTS_MENU_PAGE_INO
#ifdef USE_INPUTS

/*
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
*/

/* **************************************************************** */
#define ILLEGAL		-1
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

  if(INPUTS.selected_inputs) {
    MENU.outln(F("Set parameters with  A B S + * / > O Q #"));
    MENU.outln(F("!=activate s=samples t<n>=test T=tests"));
  }
}


// reaction function for the inputs_menu page:
bool inputs_reaction(char token) {
  bool was_no_selection = (INPUTS.selected_inputs == 0);
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
  case '9':	// togle selection 0-9
    bit = token -'0';
    if (bit >= INPUTS.get_inputs_allocated())
      return false;	// *not* responsible

    // existing input:
    INPUTS.selected_inputs ^= (1 << bit);
    if (MENU.verbosity >= VERBOSITY_SOME)
      print_selected_inputs();
    break;

  case 'a':
  case 'b':
  case 'c':
  case 'd':
  case 'e':
  case 'f':	// togle selection a-f
    bit = token -'a' + 10;
    if (bit >= INPUTS.get_inputs_allocated())
      return false;	// *not* responsible

    // existing input:
    INPUTS.selected_inputs ^= (1 << bit);
    if (MENU.verbosity >= VERBOSITY_SOME)
      print_selected_inputs();
    break;

  case '~':	// invert selection
    INPUTS.selected_inputs = ~INPUTS.selected_inputs;
    bitmask=0;
    for (int inp=0; inp < INPUTS.get_inputs_allocated(); inp++)
      bitmask |= (1 << inp);
    INPUTS.selected_inputs &= bitmask;
    if (MENU.verbosity >= VERBOSITY_SOME)
      print_selected_inputs();
    break;

  case 'x':	// clear selection
    INPUTS.selected_inputs = 0;
    if (MENU.verbosity >= VERBOSITY_SOME)
      print_selected_inputs();
    break;

  case 'N':	// set selection	do i want that at all?
    newValue = MENU.numeric_input(ILLEGAL);
    if ((newValue >= 0) && (newValue < INPUTS.get_inputs_allocated())) {
      INPUTS.selected_inputs = 1 << newValue;
    if (MENU.verbosity >= VERBOSITY_SOME)
      print_selected_inputs();
    } else MENU.OutOfRange();

    break;


  case 'A':	// set_inp A
    if(anything_selected()) {	// if not, tell the user how to select
      newValue = MENU.numeric_input(ILLEGAL);
      if (newValue == (uint8_t) newValue) {
	for (int inp=0; inp < INPUTS.get_inputs_allocated(); inp++)
	  if (INPUTS.selected_inputs & ( 1 << inp))
	    INPUTS.set_inp_A(inp, newValue);
	if (MENU.verbosity >= VERBOSITY_SOME)
	  inputs_info();
      } else MENU.OutOfRange();
    }
    break;

  case 'B':	// set_inp B
    if(anything_selected()) {	// if not, tell the user how to select
      newValue = MENU.numeric_input(ILLEGAL);
      if (newValue == (uint8_t) newValue) {
	for (int inp=0; inp < INPUTS.get_inputs_allocated(); inp++)
	  if (INPUTS.selected_inputs & ( 1 << inp))
	    INPUTS.set_inp_B(inp, newValue);
	if (MENU.verbosity >= VERBOSITY_SOME)
	  inputs_info();
      } else MENU.OutOfRange();
    }
    break;

  case '+':	// set-in_offset
    if (!INPUTS.selected_inputs)
      return false;			// unhide verbosity '+'

    newValue = MENU.numeric_input(ILLEGAL);
    if (newValue == (ioP_t) newValue) {
      for (int inp=0; inp < INPUTS.get_inputs_allocated(); inp++)
	if (INPUTS.selected_inputs & ( 1 << inp))
	  INPUTS.set_in_offset(inp, newValue);
      if (MENU.verbosity >= VERBOSITY_SOME)
	inputs_info();
    } else MENU.OutOfRange();
    break;

  case '*':	// set_mul
    if(anything_selected()) {	// if not, tell the user how to select
      newValue = MENU.numeric_input(ILLEGAL);
      if (newValue == (ioP_t) newValue) {
	for (int inp=0; inp < INPUTS.get_inputs_allocated(); inp++)
	  if (INPUTS.selected_inputs & ( 1 << inp))
	    INPUTS.set_mul(inp, newValue);
	if (MENU.verbosity >= VERBOSITY_SOME)
	  inputs_info();
      } else MENU.OutOfRange();
    }
    break;

  case '/':	// set_div
    if(anything_selected()) {	// if not, tell the user how to select
      newValue = MENU.numeric_input(ILLEGAL);
      if (newValue == (ioP_t) newValue) {
	for (int inp=0; inp < INPUTS.get_inputs_allocated(); inp++)
	  if (INPUTS.selected_inputs & ( 1 << inp))
	    INPUTS.set_div(inp, newValue);
	if (MENU.verbosity >= VERBOSITY_SOME)
	  inputs_info();
      } else MENU.OutOfRange();
    }
    break;

  case '>':	// set_out_offset
    if(anything_selected()) {	// if not, tell the user how to select
      newValue = MENU.numeric_input(ILLEGAL);
      if (newValue == (ioV_t) newValue) {
	for (int inp=0; inp < INPUTS.get_inputs_allocated(); inp++)
	  if (INPUTS.selected_inputs & ( 1 << inp))
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
	  if (INPUTS.selected_inputs & ( 1 << inp))
	    INPUTS.set_out_A(inp, newValue);
	if (MENU.verbosity >= VERBOSITY_SOME)
	  inputs_info();
      } else MENU.OutOfRange();
    }
    break;

  case 'Q':	 // set_out_B
    if(anything_selected()) {	// if not, tell the user how to select
      newValue = MENU.numeric_input(ILLEGAL);
      if (newValue == (uint8_t) newValue) {
	for (int inp=0; inp < INPUTS.get_inputs_allocated(); inp++)
	  if (INPUTS.selected_inputs & ( 1 << inp))
	    INPUTS.set_out_B(inp, newValue);
	if (MENU.verbosity >= VERBOSITY_SOME)
	  inputs_info();
      } else MENU.OutOfRange();
    }
    break;

  case 'S':	// malloc_samples
    if(anything_selected()) {	// if not, tell the user how to select
      newValue = MENU.numeric_input(ILLEGAL);
      if (newValue > 0) {
	for (int inp=0; inp < INPUTS.get_inputs_allocated(); inp++)
	  if (INPUTS.selected_inputs & ( 1 << inp))
	    if (!INPUTS.malloc_samples(inp, newValue)) {
	      MENU.OutOfRange();
	      break;
	    }
	if (MENU.verbosity >= VERBOSITY_SOME)
	  inputs_info();
      } else MENU.OutOfRange();
    }
    break;

  case '#':	// set_counter	do i want that at all?
    if(anything_selected()) {	// if not, tell the user how to select
      newValue = MENU.numeric_input(ILLEGAL);
      if (newValue == (unsigned long) newValue) {
	for (int inp=0; inp < INPUTS.get_inputs_allocated(); inp++)
	  if (INPUTS.selected_inputs & ( 1 << inp))
	    INPUTS.set_counter(inp, newValue);
	if (MENU.verbosity >= VERBOSITY_SOME)
	  inputs_info();
      } else MENU.OutOfRange();
    }
    break;


  case 's':	// show_samples
    if(anything_selected()) {	// if not, tell the user how to select
      for (int inp=0; inp < INPUTS.get_inputs_allocated(); inp++)
	if (INPUTS.selected_inputs & ( 1 << inp))
	  show_samples(inp);
    }
    break;

  case 'i': case '.':	// inputs_info
    inputs_info();
    MENU.ln();
    break;

  case 't':	// test_in2o_calculation
    if(anything_selected()) {	// if not, tell the user how to select
      newValue = MENU.numeric_input(0);
      if (newValue == (int) newValue) {
	for (int inp=0; inp < INPUTS.get_inputs_allocated(); inp++)
	  if (INPUTS.selected_inputs & ( 1 << inp))
	    test_in2o_calculation(inp, (int) newValue);
      } else MENU.OutOfRange();
    }
    break;

  case 'T':	// test_in2o_calculation range
    if(anything_selected()) {	// if not, tell the user how to select
      for (int newValue=0; newValue<1024; newValue++)
	for (int inp=0; inp < INPUTS.get_inputs_allocated(); inp++)
	  if (INPUTS.selected_inputs & ( 1 << inp))
	    test_in2o_calculation(inp, (int) newValue);
    }
    break;

  case '!':	// toggle activity
    if(anything_selected()) {	// if not, tell the user how to select
      for (int inp=0; inp < INPUTS.get_inputs_allocated(); inp++) {
	if (INPUTS.selected_inputs & ( 1 << inp)) {
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
    if (INPUTS.selected_inputs) {
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
  if (INPUTS.selected_inputs)
    return true;

  how_to_select();
  return false;
}

// FIXME: move to Menu::out_flag_mnemonics(int inp, unsigned int bits)
void show_flag_mnemonics(unsigned int flags, unsigned int bits) {
  for(int i=0; i<bits; i++)
    if (flags & (1 << i))
      MENU.out(INPUTS.flag_mnemonics_on[i]);
    else
      MENU.out(INPUTS.flag_mnemonics_off[i]);
}


// show an info line for an (existing) input:
void input_info(int inp) {
  MENU.space();
  if (INPUTS.selected_inputs & (1 << inp))
    MENU.out('*');			// '*' means selected
  else
    MENU.space();

  if (inp < 10)				// left side padding
    MENU.space();
  MENU.out(inp);
  MENU.out('/');
  MENU.out(INPUTS.get_inputs_allocated());

  MENU.out_flags_();
  show_flag_mnemonics(INPUTS.get_flags(inp), 16);
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

#ifdef DEBUG_INPUTS
  MENU.out('#');
  MENU.out(INPUTS.get_counter(inp));	// not padded
#endif

  MENU.tab();
  MENU.out(F("=> "));
  MENU.out(INPUTS.get_last_output(inp));

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
  const int hex_inputs=_min(inputs, 16);	// displayed as hex chiffres
  MENU.out_selected_();
  for (int inp=0; inp < hex_inputs; inp++) {
    if (INPUTS.selected_inputs & (1 << inp))
      MENU.out_hex_chiffre(inp);
    else
      MENU.out('.');
  }

  MENU.space();
  for (int inp=16; inp < inputs; inp++) {
    if (INPUTS.selected_inputs & (1 << inp))
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

#endif // USE_INPUTS

#define INPUTS_MENU_PAGE_INO
#endif // ! INPUTS_MENU_PAGE_INO
/* **************************************************************** */

