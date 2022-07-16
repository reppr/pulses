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

#include <Menu.h>


/* BAUDRATE for Serial:	uncomment one of the following lines:	*/
#define BAUDRATE	115200		// works fine here
//#define BAUDRATE	57600
//#define BAUDRATE	38400
//#define BAUDRATE	19200
//#define BAUDRATE	9600		// failsafe
//#define BAUDRATE	31250		// MIDI


/* **************************************************************** */
#define USE_WIFI_telnet_menu	// DADA: really?	################ FIXME: always active WiFi
#include "menu_IO_configuration.h"
#include <Menu.h>
Menu MENU(32, 1, &men_getchar, Serial, MENU_OUTSTREAM2);


/* **************************************************************** */
/* Arduiono setup() and loop():					*/

void setup() {
  Serial.begin(BAUDRATE);	// Start serial communication.

#if defined(__AVR_ATmega32U4__) || defined(ESP8266)
  /* on ATmega32U4		Leonardo, Mini, LilyPad Arduino USB
     to be able to use Serial.print() from setup()
     we *must* do that before:
  */
  while (!Serial) { ;}		// wait for Serial to open
  // delay(1000);  // still no Serial in setup
#endif

  MENU.add_page("Arduino Softboard", 'H', \
		&softboard_display, &softboard_reaction, '+');

  MENU.outln(F("Arduino Softboard\thttp://github.com/reppr/pulses/\n"));

  pins_info_digital(); MENU.ln();
  pins_info_analog();

  MENU.menu_display();		// display menu at startup
}


/* **************************************************************** */
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
  maybe_run_continuous();	// maybe display input pin state changes

  // Insert your own code here.
  // Do not let your program block program flow,
  // always return to the main loop as soon as possible.
}

/* **************************************************************** */
/* **************************************************************** */


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
