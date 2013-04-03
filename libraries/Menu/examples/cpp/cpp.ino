/* **************************************************************** */
/*
			cpp test

            http://github.com/reppr/pulses/
        Given as an example for the Menu library.


Copyright © Robert Epprecht  www.RobertEpprecht.ch   GPLv2



    Please do read README_cpp at the end of this file
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
#else
  #include <iostream>
  #include <Menu.cpp>		// why that?
#endif

#ifdef __SAM3X8E__	// FIXME: ################
  #warning 'loading hack for Arduino Due "support"'
  #define LED_BUILTIN	PIN_LED
  #define SDA	PIN_WIRE_SDA
  #define SCL	PIN_WIRE_SCL
  #define SDA1  PIN_WIRE1_SDA
  #define SCL1	PIN_WIRE1_SCL
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
/*
  This version definines the menu INPUT routine int men_getchar();
  in the *program* not inside the Menu class.
*/
#ifdef ARDUINO

  int men_getchar() {
    if (!Serial.available())	// ARDUINO
      return EOF;

    return Serial.read();
  }

  Menu CPP_INFO(32, 5, &men_getchar, Serial);

#else

  int men_getchar() {
    return getchar();		// c++ Linux PC test version
  }

  Menu CPP_INFO(32, 5, &men_getchar, cout);

#endif


void cpp_info_display();		// defined later on
bool cpp_info_reaction(char token);	// defined later on

/* **************************************************************** */
#ifdef ARDUINO
/* Arduino setup() and loop():					*/

void setup() {
  Serial.begin(BAUDRATE);	// Start serial communication.

  CPP_INFO.add_page("cpp info", 'C', \
		&cpp_info_display, &cpp_info_reaction, '+');

  CPP_INFO.outln(F("cpp info\thttp://github.com/reppr/pulses/"));

  CPP_INFO.menu_display();		// display menu at startup
}


/*
  All you have to from your Arduino sketch loop() is to call:
  CPP_INFO.lurk_then_do();
  This will *not* block the loop.

  It will lurk on menu input, but return immediately if there's none.
  On input it will accumulate one next input byte and return until it
  will see an 'END token'.

  When seeing one of the usual line endings it takes it as 'END token' \0.

  On receiving an 'END token' it will interpret the accumulated tokens and return.
*/

void loop() {	// ARDUINO
  CPP_INFO.lurk_then_do();

  // Insert your own code here.
  // Do not let your program block program flow,
  // always return to the main loop as soon as possible.
}

#else

int main() {
  CPP_INFO.add_page("cpp info", 'C', \
		&cpp_info_display, &cpp_info_reaction, '+');

  CPP_INFO.outln(F("cpp info\thttp://github.com/reppr/pulses/"));

  CPP_INFO.menu_display();		// Display menu at startup

  while(true)
    CPP_INFO.lurk_then_do();
}

#endif

/* **************************************************************** */
/* define some things early: */

#define ILLEGAL		-1



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

#include <pins_arduino.h>	// not needed, but source of inspiration...

  // see: <Arduino.h>
      /*
#define clockCyclesPerMicrosecond() ( SystemCoreClock / 1000000L )
#define clockCyclesToMicroseconds(a) ( ((a) * 1000L) / (SystemCoreClock / 1000L) )
#define microsecondsToClockCycles(a) ( (a) * (SystemCoreClock / 1000000L) )

// Get the bit location within the hardware port of the given virtual pin.
// This comes from the pins_*.c file for the active board configuration.
//
#define digitalPinToPort(P)        ( g_APinDescription[P].pPort )
#define digitalPinToBitMask(P)     ( g_APinDescription[P].ulPin )
#define digitalPinToTimer(P)       (  )
//#define analogInPinToBit(P)        ( )
#define portOutputRegister(port)   ( &(port->PIO_ODSR) )
#define portInputRegister(port)    ( &(port->PIO_PDSR) )
//#define portModeRegister(P)        (  )

#define ADC_CHANNEL_NUMBER_NONE 0xffffffff

      */


const char pin_[] = "pin ";
const char port_[] = "port ";
const char TIMER_[] = "TIMER ";
const char YES_[] = "YES";
const char no_[] = "no ";
const char Question3_[] = "???\t";


void  arduino_pins_info() {
  CPP_INFO.ln();

#ifdef PINS_COUNT		// #defined on Arduino Due
  #define CHECK_PINS	PINS_COUNT
#else
  #ifdef NUM_DIGITAL_PINS
    #define CHECK_PINS	NUM_DIGITAL_PINS
  #else
    #undef CHECK_PINS
  #endif
#endif

#ifdef CHECK_PINS
  int timer;

  for(int pin=0; pin<CHECK_PINS; pin++) {
    CPP_INFO.out(pin_);
    CPP_INFO.out(pin);
    CPP_INFO.tab();

    #ifdef digitalPinHasPWM
      if(digitalPinHasPWM(pin))
	CPP_INFO.out(F("PWM\t"));
      else
	CPP_INFO.tab();
    #else
      CPP_INFO.out(Question3_);
    #endif

    CPP_INFO.out(port_);
    #ifdef digitalPinToPort
      #ifndef __SAM3X8E__
        CPP_INFO.out(uint8_t digitalPinToPort(pin));
      #else
	CPP_INFO.out(int digitalPinToPort(pin));
      #endif
    #else
      CPP_INFO.out('?');
    #endif
    CPP_INFO.space(); CPP_INFO.space();

    #ifdef digitalPinToBitMask
      CPP_INFO.outBIN(digitalPinToBitMask(pin), 8);
      CPP_INFO.tab();
    #else
      CPP_INFO.out(Question3_);
    #endif

    CPP_INFO.tab();
    switch (pin) {	// there can be *only one* of them
    case A0:		// so we *can* 'switch' here
      CPP_INFO.out(F("A0  "));
      break;
    case A1:
      CPP_INFO.out(F("A1  "));
      break;
    case A2:
      CPP_INFO.out(F("A2  "));
      break;
    case A3:
      CPP_INFO.out(F("A3  "));
      break;
    case A4:
      CPP_INFO.out(F("A4  "));
      break;
    case A5:
      CPP_INFO.out(F("A5  "));
      break;
#if (NUM_ANALOG_INPUTS > 6)
    case A6:
      CPP_INFO.out(F("A6  "));
      break;
    case A7:
      CPP_INFO.out(F("A7  "));
      break;
  #if (NUM_ANALOG_INPUTS > 8)
    case A8:
      CPP_INFO.out(F("A8  "));
      break;
    case A9:
      CPP_INFO.out(F("A9  "));
      break;
    case A10:
      CPP_INFO.out(F("A10  "));
      break;
    case A11:
      CPP_INFO.out(F("A11  "));
      break;
    case A12:
      CPP_INFO.out(F("A12  "));
      break;
    case A13:
      CPP_INFO.out(F("A13  "));
      break;
    case A14:
      CPP_INFO.out(F("A14  "));
      break;
    case A15:
      CPP_INFO.out(F("A15  "));
      break;

#ifdef DACC_INTERFACE
    case DAC0:
      CPP_INFO.out(F("DAC0 resolution:"));
      CPP_INFO.out((int) DAC0_RESOLUTION);
      CPP_INFO.out(F("bit  "));
      break;
    case DAC1:
      CPP_INFO.out(F("DAC1 resolution:"));
      CPP_INFO.out((int) DAC1_RESOLUTION);
      CPP_INFO.out(F("bit  "));
      break;
#endif

  #endif
#endif
    }

// FIXME: does not work ################
#ifdef PINS_CAN0
    if(pin == CAN0RX)
      CPP_INFO.out(F("CAN0RX  "));
#endif
#ifdef PINS_CAN1
    if(pin == CAN1RX)
      CPP_INFO.out(F("CAN1RX  "));
#endif

    if(pin == LED_BUILTIN)
      CPP_INFO.out(F("LED  "));

    if(pin == SS)
      CPP_INFO.out(F("SPI SS  "));

#ifdef PIN_SPI_SS1;	// see: <variant.h>
    if(pin == SS1)
      CPP_INFO.out(F("SPI SS1  "));
#endif
#ifdef PIN_SPI_SS2;	// see: <variant.h>
    if(pin == SS2)
      CPP_INFO.out(F("SPI SS2  "));
#endif
#ifdef PIN_SPI_SS3;	// see: <variant.h>
    if(pin == SS3)
      CPP_INFO.out(F("SPI SS3  "));
#endif

    if(pin == MOSI)
      CPP_INFO.out(F("SPI MOSI  "));

    if(pin == MISO)
      CPP_INFO.out(F("SPI MISO  "));

    if(pin == SCK)
      CPP_INFO.out(F("SPI SCK  "));

    if(pin == SDA)
      CPP_INFO.out(F("I2C SDA  "));

    if(pin == SCL)
      CPP_INFO.out(F("I2C SCL  "));

#ifdef PIN_LED_RXL	// see: <variant.h>
    if(pin == PIN_LED_RXL)
      CPP_INFO.out(F("LED Rx  "));
#endif
#ifdef PIN_LED_TXL
    if(pin == PIN_LED_TXL)
      CPP_INFO.out(F("LED Tx  "));
#endif

#ifdef PIN_WIRE1_SDA
    if(pin == SDA1)
      CPP_INFO.out(F("I2C SDA1  "));

    if(pin == SCL1)
      CPP_INFO.out(F("I2C SCL1  "));
#endif

    #ifdef digitalPinToTimer
      #ifdef __SAM3X8E__	// FIXME: ################
         #warning "no timer info on Arduino Due yet. ################"
      #else
        timer= digitalPinToTimer(pin);
	if(timer) {
	  CPP_INFO.out(TIMER_);
	  CPP_INFO.out(timer);
	  // FIXME: ################
	  CPP_INFO.space();
	  CPP_INFO.out(Question3_); // don't know what the numbers mean!
	}
      #endif
    #else
      CPP_INFO.out(TIMER_);
      CPP_INFO.out(Question3_);
    #endif

    CPP_INFO.ln();
  }
  #endif
}


void arduino_info() {	// Display some Arduino specific informations.

  CPP_INFO.out(F("\nArduino software version\t"));
  CPP_INFO.outln(ARDUINO);

  /* ARDUINO BOARD SPECIFIC THINGS:				*/

  // Board type:
  #if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) // mega boards
    CPP_INFO.out(F("Mega board"));
  #endif

  #ifdef ARDUINO_DUE	// see: boards.txt
    CPP_INFO.outln(F("ARDUINO_DUE"));
  #endif

  #if defined(_VARIANT_ARDUINO_DUE_X_)
    CPP_INFO.outln(F("_VARIANT_ARDUINO_DUE_X_"));
  #endif

  #if defined(VARIANT_NAME)		// /** Name of the board */
    CPP_INFO.out(F("VARIANT_NAME\t"));
    CPP_INFO.outln(F(VARIANT_NAME));
  #endif

  // ..., UNO, DUE, TRE ;)
  CPP_INFO.ln();	// end board type

  // Processor:
  CPP_INFO.out(F("Processor\t"));
  #if defined(__AVR_ATmega328P__)
    CPP_INFO.outln(F("__AVR_ATmega328P__"));
  #elif defined(__AVR_ATmega2560__)
    CPP_INFO.outln(F("__AVR_ATmega2560__"));
  #elif defined(__AVR_ATmega1280__)
    CPP_INFO.outln(F("__AVR_ATmega1280__"));
  #elif defined(__SAM3X8E__)
    CPP_INFO.outln(F("__SAM3X8E__"));

  #else
    CPP_INFO.outln(F("NOT IMPLEMENTED YET"));
  #endif // processor
  CPP_INFO.ln(); // end processor

  //
  // VARIANT_MAINOSC		/** Frequency of the board main oscillator */
  #ifdef VARIANT_MAINOSC
    CPP_INFO.out(F("VARIANT_MAINOSC\t"));
    CPP_INFO.outln(VARIANT_MAINOSC);
  #else
    CPP_INFO.outln(F("VARIANT_MAINOSC\tunknown"));
  #endif

  // VARIANT_MCK		/** Master clock frequency */
  #ifdef VARIANT_MCK
    CPP_INFO.out(F("VARIANT_MCK\t"));
    CPP_INFO.outln(VARIANT_MCK);
  #else
    CPP_INFO.outln(F("VARIANT_MCK\tunknown"));
  #endif

  // PINS_COUNT
  #ifdef PINS_COUNT
    CPP_INFO.out(F("PINS_COUNT\t"));
    CPP_INFO.outln(PINS_COUNT);
  #else
    CPP_INFO.outln(F("PINS_COUNT\tunknown"));
  #endif

  #if defined(ARDUINO_MODEL_PID)
    #if (ARDUINO_MODEL_PID == ARDUINO_UNO_PID)
      CPP_INFO.outln(F("ARDUINO_UNO_PID"));
    #elif (ARDUINO_MODEL_PID == ARDUINO_DUE_PID)
      CPP_INFO.outln(F("ARDUINO_DUE_PID"));
    #elif (ARDUINO_MODEL_PID == ARDUINO_MEGA2560_PID)
      CPP_INFO.outln(F("ARDUINO_MEGA2560_PID"));
    #endif
  #else
      // CPP_INFO.outln(F("ARDUINO_MODEL_PID is not defined.\n"));
  #endif

  // Pins:
  // see: <pins_arduino.h>
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

  #ifdef ADC_CHANNEL_NUMBER_NONE
    CPP_INFO.outln(F("ADC_CHANNEL_NUMBER_NONE"));
  #endif

  CPP_INFO.outln(F("\nchecking for some macros:"));

  CPP_INFO.out(F("digitalPinHasPWM\t"));
  #ifdef digitalPinHasPWM
    CPP_INFO.outln(YES_);
  #else
    CPP_INFO.outln(no_);
  #endif

  CPP_INFO.out(F("analogInputToDigitalPin\t"));
  #ifdef analogInputToDigitalPin
    CPP_INFO.outln(YES_);
  #else
    CPP_INFO.outln(no_);
  #endif

  CPP_INFO.out(F("digitalPinToPort\t"));
  #ifdef digitalPinToPort
    CPP_INFO.outln(YES_);
  #else
    CPP_INFO.outln(no_);
  #endif

  CPP_INFO.out(F("digitalPinToBitMask\t"));
  #ifdef digitalPinToBitMask
    CPP_INFO.outln(YES_);
  #else
    CPP_INFO.outln(no_);
  #endif

  CPP_INFO.out(F("digitalPinToTimer\t"));
  #ifdef digitalPinToTimer
    CPP_INFO.outln(YES_);
  #else
    CPP_INFO.outln(no_);
  #endif

  CPP_INFO.outln(F("\nPress 'p' for info about pins."));
}
#endif


/* ****************  cpp_info menu display:  **************** */

void cpp_info_display() {
  bool dummybool[8];

  CPP_INFO.out(F("sizeof(char)\t"));		CPP_INFO.outln(sizeof(char));
  CPP_INFO.out(F("sizeof(int)\t"));		CPP_INFO.outln(sizeof(int));
  CPP_INFO.out(F("sizeof(short)\t"));		CPP_INFO.outln(sizeof(short));
  CPP_INFO.out(F("sizeof(long)\t"));		CPP_INFO.outln(sizeof(long));
  CPP_INFO.out(F("sizeof(int*)\t"));		CPP_INFO.outln(sizeof(int*));
  CPP_INFO.out(F("sizeof(void*)\t"));		CPP_INFO.outln(sizeof(void*));
  CPP_INFO.out(F("sizeof(bool[8])\t"));		CPP_INFO.outln(sizeof(dummybool));

#ifdef ARDUINO
  arduino_info();	// Display some Arduino specific informations.
#endif
}


// bool cpp_info_reaction(char token)
// Try to react on token, return success flag.
// const char noHw_[] = "no hardware ";	################

bool cpp_info_reaction(char token) {
  switch (token) {
  case 'p':
    arduino_pins_info();
    break;

  default:
    return 0;		// token not found in this menu
  }
  return 1;		// token found in this menu
}


/* **************************************************************** */
/* README_cpp

README cpp_info

   cpp_info  http://github.com/reppr/pulses


   cpp_info, arduino_info temporally implemented under libraries/Menu/examples/cpp.

   This is just a very simple menupage giving informations about
   Arduino software and board. Mainly just showing some macro values.
   Much overkill for the simple task, I know ;)

   It is not really worth an entry under libraries/Menu/examples/,
   planed to move as a menupage under libraries/Menu/examples/menupages/ soon.

   Just for a quick test (aiming at the Arduino DUE, btw).

   Not finished at all,
   barely usable but starts working.


Description

   Show some compiler and system specific infos on a menu page.

   This version is implemented as an example for the Menu library.



Installation:

   Get it from http://github.com/reppr/pulses
   Put contents of the pulses/libraries/ folder into sketchbook/libraries/
   Cpp_Info is implemented as an example of libraries/Menu/
   After a restart the Arduino GUI shows cpp_info under
   File >> Sketchbook >> libraries >> Menu >> cpp_info

	Arduino versions older than 1.0 need a fix:
	  see this thread:
	  http://code.google.com/p/arduino/issues/detail?id=604&start=200

	    short version (adapt to your arduino version)
	      edit file arduino-0022/hardware/arduino/cores/arduino/wiring.h
	      comment out line 79 (round macro)
	      #define round(x)     ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))
	      tested on arduino-0023

	For older arduino versions you also have to rename the sketch file
	from cpp_info.ino to cpp_info.pde


How it works:

   You communicate with the arduino over a serial connection
   that could be a real serial line or an usb cable.


Configure your terminal program:

   Do configure your terminal emulation program to send some sort of
   line ending code, usual culprits should work.

   Set arduino baud rate by editing the line starting with
   #define BAUDRATE in cpp.ino
   and set it to the same value in your terminal software.

   So if for example you use the Arduino 'Serial Monitor' window
   check that it *does* send 'Newline' (in the bottom window frame)
   and set baud rate to the same value as BAUDRATE on the arduino.


   Send '?' (and a linefeed) over serial line to see the menu.
   It displays some basic infos and shows some one-letter commands.




   Copyright © Robert Epprecht  www.RobertEpprecht.ch   GPLv2

   http://github.com/reppr/pulses

*/
/* **************************************************************** */
