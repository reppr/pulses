/* **************************************************************** */
/*
			cpp-page

            http://github.com/reppr/pulses/
        Given as an example for the Menu library.


Copyright © Robert Epprecht  www.RobertEpprecht.ch   GPLv2



    Please do read README_cpp_page at the end of this file
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
#ifndef CPP_PAGE
#define CPP_PAGE

#include <stdio.h>
#include <stdlib.h>

#ifdef ARDUINO
  #if ARDUINO >= 100
    #include "Arduino.h"
  #else
    #include "WProgram.h"
  #endif
#else
  #include <iostream>
  #include <Menu.cpp>		// why that?
#endif

#include <Menu.h>


/* **************************************************************** */
/* define some things early: */

#ifndef ILLEGAL
  #define ILLEGAL	-1
#endif

/* **************************************************************** */

//	void cpp_info_display();		// defined later on
//	bool cpp_info_reaction(char token);	// defined later on
//
//	/* **************************************************************** */
//	#ifdef ARDUINO
//	/* Arduino setup() and loop():					*/
//
//	void setup() {
//	  Serial.begin(BAUDRATE);	// Start serial communication.
//
//	  MENU.add_page("cpp info", 'C', \
//			&cpp_info_display, &cpp_info_reaction, '+');
//
//	  MENU.outln(F("cpp info\thttp://github.com/reppr/pulses/\n"));
//
//	  MENU.menu_display();		// display menu at startup
//	}
//
//
//	/*
//	  All you have to from your Arduino sketch loop() is to call:
//	  MENU.lurk_then_do();
//	  This will *not* block the loop.
//
//	  It will lurk on menu input, but return immediately if there's none.
//	  On input it will accumulate one next input byte and return until it
//	  will see an 'END token'.
//
//	  When seeing one of the usual line endings it takes it as 'END token' \0.
//
//	  On receiving an 'END token' it will interpret the accumulated tokens and return.
//	*/
//
//	void loop() {	// ARDUINO
//	  MENU.lurk_then_do();
//
//	  // Insert your own code here.
//	  // Do not let your program block program flow,
//	  // always return to the main loop as soon as possible.
//	}
//
//	#else
//
//	int main() {
//	  MENU.add_page("cpp info", 'C', \
//			&cpp_info_display, &cpp_info_reaction, '+');
//
//	  MENU.outln(F("cpp info\thttp://github.com/reppr/pulses/\n"));
//
//	  MENU.menu_display();		// Display menu at startup
//
//	  while(true)
//	    MENU.lurk_then_do();
//	}
//
//	#endif
//
//	/* **************************************************************** */
//	/* define some things early: */
//
//	#define ILLEGAL		-1
//
//
//	/* **************************************************************** */
//	/*
//	  serial_print_BIN(unsigned long value, int bits)
//	  print binary numbers with leading zeroes and a trailing space
//	*/
//	void print_BIN(unsigned long value, int bits) {
//	  int i;
//	  unsigned long mask=0;
//
//	  for (i = bits - 1; i >= 0; i--) {
//	    mask = (1 << i);
//	      if (value & mask)
//		MENU.out('1');
//	      else
//		MENU.out('0');
//	  }
//	  MENU.space();
//	}


/* **************************************************************** */
/*
  Determine RAM usage:
  int get_free_RAM() {
*/

#ifdef ARDUINO
  extern int __bss_end;
  extern void *__brkval;

  int get_free_RAM() {
    int free;

    if ((int) __brkval == 0)
      return ((int) &free) - ((int) &__bss_end);
    else
      return ((int) &free) - ((int) __brkval);
  }
#else			// not used yet on PC ;(	################
  int get_free_RAM() { return ILLEGAL; }
#endif


/* **************************************************************** */
/*
   Menu cpp_info display and reaction:
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


/* ****************  arduino_info():  **************** */
#ifdef ARDUINO
void arduino_info() {	// Display some Arduino specific informations.
  CPP_INFO.out(F("\nArduino software version\t"));
  CPP_INFO.outln(ARDUINO);

  /* ARDUINO BOARD SPECIFIC THINGS:				*/

  // Board type:
  #if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) // mega boards
    CPP_INFO.out(F("Mega board"));
  #endif

  // ..., UNO, DUE, TRE ;)
  CPP_INFO.ln();	// end board type

  // Processor:
  CPP_INFO.out(F("Processor\t"));
  #if defined(__AVR_ATmega328P__)
    CPP_INFO.outln(F("__AVR_ATmega328P__"));
  #else
  #if defined(__AVR_ATmega2560__)
    CPP_INFO.outln(F("__AVR_ATmega2560__"));
  #else
  #if defined(__AVR_ATmega1280__)
    CPP_INFO.outln(F("__AVR_ATmega1280__"));

  #else
    CPP_INFO.out(F("NOT IMPLEMENTED YET"));
  #endif
  #endif
  #endif // processor
  CPP_INFO.ln(); // end processor

  // Pins:
  #ifdef NUM_DIGITAL_PINS
    CPP_INFO.out(F("NUM_DIGITAL_PINS\t"));
    CPP_INFO.outln(NUM_DIGITAL_PINS);
  #else
    CPP_INFO.outln(F("NUM_DIGITAL_PINS\tunknown"));
  #endif

  #ifdef NUM_ANALOG_INPUTS
    CPP_INFO.out(F("NUM_ANALOG_INPUTS\t"));
    CPP_INFO.outln(NUM_ANALOG_INPUTS);
  #else
    CPP_INFO.outln(F("NUM_ANALOG_INPUTS\tunknown"));
  #endif

  /*
    DIGITAL_ONLY_PINS
    number of arduino pins configured for digital I/O
    *not* counting analog inputs:
  */
  #if defined(NUM_DIGITAL_PINS) && defined(NUM_ANALOG_INPUTS)
    #define DIGITAL_ONLY_PINS	(NUM_DIGITAL_PINS - NUM_ANALOG_INPUTS)

    CPP_INFO.out(F("Digital only pins\t"));
    CPP_INFO.outln(DIGITAL_ONLY_PINS);
  #endif
}
#endif


/* ****************  cpp_info menu display:  **************** */

void cpp_info_display() {
  MENU.out(F("free RAM="));
  MENU.outln(get_free_RAM());

  MENU.ln();

  MENU.out(F("sizeof(char)\t"));		MENU.outln(sizeof(char));
  MENU.out(F("sizeof(int)\t"));		MENU.outln(sizeof(int));
  MENU.out(F("sizeof(long)\t"));		MENU.outln(sizeof(long));
  MENU.out(F("sizeof(int*)\t"));		MENU.outln(sizeof(int*));
  MENU.out(F("sizeof(void*)\t"));		MENU.outln(sizeof(void*));

#ifdef ARDUINO
  arduino_info();	// Display some Arduino specific informations.
#endif
}


// bool cpp_info_reaction(char token)
// Try to react on token, return success flag.
// const char noHw_[] = "no hardware ";	################

bool cpp_info_reaction(char token) {
  switch (token) {

  default:
    return 0;		// token not found in this menu
  }
  return 1;		// token found in this menu
}

const char cpp_title[] = "cpp info";

/* **************************************************************** */
/* README_cpp-page

*/
/* **************************************************************** */
#endif
