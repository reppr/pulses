#error "sorry this module is temporary switched off"
/* **************************************************************** */
/*
	    http://github.com/reppr/pulses/

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

#ifdef ARDUINO
  #if ARDUINO >= 100
    #include "Arduino.h"
  #else
    #include "WProgram.h"
  #endif
#endif

#include <Menu.h>

/* BAUDRATE for Serial:	uncomment one of the following lines:	*/
#define BAUDRATE	115200		// works fine here
//#define BAUDRATE	57600
//#define BAUDRATE	38400
//#define BAUDRATE	19200
//#define BAUDRATE	9600		// failsafe
//#define BAUDRATE	31250		// MIDI

/* PAGES Maximal number of Menu pages				*/
#define PAGES	6			// allocate RAM for pages


/* **************************************************************** */
// DADA TODO:  #error "sorry this module is temporary switched off"

/*
  This version definines the menu INPUT routine int men_getchar();
  in the *program* not inside the Menu class and passes a reference.
*/
#ifdef ARDUINO	// Arduino  men_getchar();

  int men_getchar() {	// Arduino version	returns EOF 32 bit or char
    if (!Serial.available())
      return EOF;

    return Serial.read();
  }

  Menu MENU(32, PAGES, &men_getchar, Serial);	// Arduino

#else		// c++ Linux PC test version  men_getchar();

  int men_getchar() {	// c++ Linux PC test version
    return getchar();
  }

  Menu MENU(32, PAGES, &men_getchar, cout);

#endif	// Menu MENU is ready now.


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
     display function	displays the menu page,
			give infos,
			tell the user about active keys.
     action function	checks if it is responsible for a token,
			if so do its trick and return true,
			else return false.

   This example includes a small collection of pre-made Menu pages.
   Take it as example how to write your own.

   Try them out by including *some* of them in a sketch,
   but you might not have enough RAM to include too many of them.
   So if it fails start with one or just a few pages.

*/

/* **************************************************************** */
/* #include *some* of the following menu pages:			*/

#include "cpp-page.cpp"

/* **************************************************************** */
void setup_pages() {

#ifdef CPP_PAGE
  MENU.add_page(cpp_title, 'C', &cpp_info_display, &cpp_info_reaction, '+');
#endif

}

/* **************************************************************** */
#ifdef ARDUINO
/* Arduino setup() and loop():					*/

void setup() {
  Serial.begin(BAUDRATE);	// Start serial communication.

#ifdef __AVR_ATmega32U4__
  /* on ATmega32U4		Leonardo, Mini, LilyPad Arduino USB
     to be able to use Serial.print() from setup()
     we *must* do that before:
  */
  while (!Serial) { ;}		// wait for Serial to open
#endif

  MENU.outln(F("\thttp://github.com/reppr/pulses/"));
  setup_pages();

  MENU.menu_display();		// Display menu at startup.
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

void loop() {		// ARDUINO
  MENU.lurk_then_do();

  // Insert your own code here.
  // Do not let your program block program flow,
  // always return to the main loop as soon as possible.
}


/* **************************************************************** */

#else	// c++ Linux PC test version int main();

int main() {
  MENU.outln(F("cpp info\thttp://github.com/reppr/pulses/"));

  setup_pages();
  MENU.menu_display();		// Display menu at startup

  while(true)
    MENU.lurk_then_do();
}

#endif

/* **************************************************************** */
/*

README_pages

	http://github.com/reppr/pulses

 A collection of menu pages for the Menu library.

>>> >> NOT WORKING YET ON Arduino << <<<



Copyright © Robert Epprecht  www.RobertEpprecht.ch   GPLv2

	http://github.com/reppr/pulses

*/
/* **************************************************************** */
