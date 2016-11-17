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

#define USE_FLOAT_TYPES		// you might want switch that off

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
  #include <Menu.h>
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

  Menu MENU(32, 5, &men_getchar, Serial);

#else

  int men_getchar() {
    return getchar();		// c++ Linux PC test version
  }

  Menu MENU(32, 5, &men_getchar, cout);

#endif


void cpp_info_display();		// defined later on
bool cpp_info_reaction(char token);	// defined later on

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

  MENU.add_page("cpp info", 'C', \
		&cpp_info_display, &cpp_info_reaction, '+');

  MENU.outln(F("cpp info\thttp://github.com/reppr/pulses/"));

  MENU.menu_display();		// display menu at startup
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

void loop() {	// ARDUINO
  MENU.lurk_then_do();

  // Insert your own code here.
  // Do not let your program block program flow,
  // always return to the main loop as soon as possible.
}

#else

int main() {
  MENU.add_page("cpp info", 'C', \
		&cpp_info_display, &cpp_info_reaction, '+');

  MENU.outln(F("cpp info\thttp://github.com/reppr/pulses/"));

  MENU.menu_display();		// Display menu at startup

  while(true)
    MENU.lurk_then_do();
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

// helper function:  maybe_display_An(uint8_t pin);
int maybe_display_An(uint8_t pin) {
  int analog_input;

  switch (pin) {	// there can be *only one* of them
  case A0:		// so we *can* 'switch' here
    analog_input=0;
    break;
#if (NUM_ANALOG_INPUTS > 1)
  case A1:
    analog_input=1;
    break;
  case A2:
    analog_input=2;
    break;
  case A3:
    analog_input=3;
    break;
  case A4:
    analog_input=4;
    break;
  case A5:
    analog_input=5;
    break;
#if (NUM_ANALOG_INPUTS > 6)
  case A6:
    analog_input=6;
    break;
  case A7:
    analog_input=7;
    break;
#if (NUM_ANALOG_INPUTS > 8)
  case A8:
    analog_input=8;
    break;
  case A9:
    analog_input=9;
    break;
  case A10:
    analog_input=10;
    break;
  case A11:
    analog_input=11;
    break;
#if (NUM_ANALOG_INPUTS > 12)
  case A12:
    analog_input=12;
    break;
  case A13:
    analog_input=13;
    break;
  case A14:
    analog_input=14;
    break;
  case A15:
    analog_input=15;
    break;
#endif
#endif
#endif
#endif

  default:
    return false;	// nothing done, nothing to do
  }

  // found analog input, display:
  MENU.out('A');
  MENU.out(analog_input);
  MENU.space();
  MENU.space();
  return true;
}


void  arduino_pins_info() {
  MENU.ln();

#ifdef ESP8266
  MENU.outln("COMPLETELY UNTESTED ON THIS BOARD");
#endif

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
  bool spi, i2c, led, analog_in;

  for(int pin=0; pin<CHECK_PINS; pin++) {

    // pin number:
    MENU.out(pin_);
    MENU.out(pin);
    MENU.tab();

    // LED?
    led=false;
#ifdef LED_BUILTIN
    if (pin == LED_BUILTIN)
      led=true;
#endif
#ifdef PIN_LED_RXL	// see: <variant.h>
    if(pin == PIN_LED_RXL)
      led=true;
#endif
#ifdef PIN_LED_TXL
    if(pin == PIN_LED_TXL)
      led=true;
#endif

    if(led)
      MENU.out('*');
    
    // PWM?
    #ifdef digitalPinHasPWM
      if(digitalPinHasPWM(pin))
	MENU.out('~');
      else
	MENU.space();
    #else
      MENU.out('?');
    #endif
    MENU.space();

    // analog input?
    analog_in = maybe_display_An(pin);
    MENU.tab();

    // timer?
    #ifdef digitalPinToTimer
      #ifdef __SAM3X8E__	// FIXME: ################
         #warning "no timer info on Arduino Due yet. ################"
      #else
        timer= digitalPinToTimer(pin);
	if(timer) {
	  // FIXME: ################
	  MENU.out(TIMER_);
	  MENU.out(timer);
	  MENU.space();
	  MENU.out(Question3_); // don't know what the numbers mean!
	} else {
	  // FIXME: ################
	  MENU.tab();
	  MENU.tab();
	}
      #endif
    #else
      MENU.out(TIMER_);
      MENU.out(Question3_);
    #endif


    // SPI/I2C: (not expected on same pin, but who knows?)
    // SPI?
    spi=true;
    switch (pin) {
    case SS:
      MENU.out(F("SS"));
      break;
#ifdef PIN_SPI_SS1;	// see: <variant.h>
    case SS1:
      MENU.out(F("SS1"));
      break;
#endif
#ifdef PIN_SPI_SS2;	// see: <variant.h>
    case SS2:
      MENU.out(F("SS2"));
      break;
#endif
#ifdef PIN_SPI_SS3;	// see: <variant.h>
    case SS3:
      MENU.out(F("SS3"));
      break;
#endif
    case MOSI:
      MENU.out(F("MOSI"));
      break;

    case MISO:
      MENU.out(F("MISO"));
      break;

    case SCK:
      MENU.out(F("SCK"));
      break;

    default:
      spi=false;
    }
    if (spi)	// separate from i2c, just in case...
      MENU.space();

    // I2C?
    i2c=true;
    switch (pin) {
    case SDA:
      MENU.out(F("SDA"));
      break;

    case SCL:
      MENU.out(F("SCL"));
      break;

#ifdef PIN_WIRE1_SDA
    case SDA1:
      MENU.out(F("SDA1"));
      break;

    case SCL1:
      MENU.out(F("SCL1"));
      break;
#endif

    default:
      i2c=false;
    }
    MENU.tab();

    // port and mask:
    MENU.out(port_);
    #ifdef digitalPinToPort
      #ifdef __SAM3X8E__	// FIXME: ################
	MENU.out(int digitalPinToPort(pin));
      #else
        MENU.out(uint8_t digitalPinToPort(pin));
      #endif
    #else
      MENU.out('?');
    #endif
    MENU.space(); MENU.space();

    #ifdef digitalPinToBitMask
      #ifdef ESP8266
        MENU.outBIN(digitalPinToBitMask(pin), 16);
      #else
        MENU.outBIN(digitalPinToBitMask(pin), 8);
      #endif
    #else
      MENU.out(Question3_);
    #endif

    // Summarize special abilities:
    MENU.tab();
    if (spi)
      MENU.out(F("SPI  "));
    if (i2c)
      MENU.out(F("I2C  "));
#ifdef LED_BUILTIN
    if(pin == LED_BUILTIN)
      MENU.out(F("LED  "));
#endif
#ifdef digitalPinHasPWM
    if(digitalPinHasPWM(pin))
      MENU.out(F("PWM  "));
#endif
#ifdef PIN_LED_RXL	// see: <variant.h>
    if(pin == PIN_LED_RXL)
      MENU.out(F("LEDRx"));
#endif
#ifdef PIN_LED_TXL
    if(pin == PIN_LED_TXL)
      MENU.out(F("LEDTx"));
#endif
    if (analog_in)
      MENU.out(F("analog input  "));


// TODO: leftovers unfinished	// FIXME: ################
#ifdef DACC_INTERFACE
    if(pin == DAC0) {
      MENU.out(F("DAC0  "));
  #ifdef DAC0_RESOLUTION	// FIXME: ################
      MENU.out(F("DAC0 resolution:"));
      MENU.out((int) DAC0_RESOLUTION);
      MENU.out(F("bit  "));
  #else
    #warning "DAC0_RESOLUTION undefined"
  #endif
    }

    if(pin == DAC1) {
      MENU.out(F("DAC1  "));
  #ifdef DAC1_RESOLUTION	// FIXME: ################
      MENU.out(F("DAC1 resolution:"));
      MENU.out((int) DAC1_RESOLUTION);
      MENU.out(F("bit  "));
  #else
    #warning "DAC1_RESOLUTION undefined"
  #endif
    }
#endif

// FIXME: does not work ################
#ifdef PINS_CAN0
    if(pin == CAN0RX)
      MENU.out(F("CAN0RX  "));
#endif
#ifdef PINS_CAN1
    if(pin == CAN1RX)
      MENU.out(F("CAN1RX  "));
#endif

    MENU.ln();
  } // pin loop
  #endif
}


void arduino_info() {	// Display some Arduino specific informations.

  MENU.out(F("\nArduino software version\t"));
  MENU.outln(ARDUINO);

  /* ARDUINO BOARD SPECIFIC THINGS:				*/

  // Board type:
  MENU.out(F("\nBoard\t"));
  #if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) // mega boards
    MENU.out(F("Mega board"));
  #endif

  #ifdef ARDUINO_DUE	// see: boards.txt
    MENU.outln(F("ARDUINO_DUE"));
  #endif

  #if defined(_VARIANT_ARDUINO_DUE_X_)
    MENU.outln(F("\t_VARIANT_ARDUINO_DUE_X_"));
  #endif

  #if defined(ESP8266)
    MENU.outln(F("ESP8266"));
  #endif

  #ifdef ARDUINO_ESP8266_NODEMCU
    MENU.outln(F("ARDUINO_ESP8266_NODEMCU"));
  #endif

  #if defined(VARIANT_NAME)		// /** Name of the board */
    MENU.out(F("VARIANT_NAME\t"));
    MENU.outln(F(VARIANT_NAME));
  #endif
  // ..., UNO, DUE, TRE ;)


  // Processor:
  MENU.out(F("\nProcessor\t"));
  #if defined(__AVR_ATmega328P__)
    MENU.outln(F("__AVR_ATmega328P__"));
  #elif defined(__AVR_ATmega2560__)
    MENU.outln(F("__AVR_ATmega2560__"));
  #elif defined(__AVR_ATmega1280__)
    MENU.outln(F("__AVR_ATmega1280__"));
  #elif defined(__SAM3X8E__)
    MENU.outln(F("__SAM3X8E__"));
  #elif defined(__AVR_ATmega32U4__)
    MENU.outln(F("__AVR_ATmega32U4__"));
  #elif defined(ESP8266)
    MENU.outln(F("ESP8266"));

  #else
    MENU.outln(F("NOT IMPLEMENTED YET"));
  #endif // processor


  // oscillator speed:
  MENU.outln(F("\nOscillator speed"));
  // VARIANT_MAINOSC		/** Frequency of the board main oscillator */
  #ifdef VARIANT_MAINOSC
    MENU.out(F("VARIANT_MAINOSC\t"));
    MENU.outln(VARIANT_MAINOSC);
  #else
    MENU.outln(F("VARIANT_MAINOSC\tunknown"));
  #endif


  // VARIANT_MCK		/** Master clock frequency */
  #ifdef VARIANT_MCK
    MENU.out(F("VARIANT_MCK\t"));
    MENU.outln(VARIANT_MCK);
  #else
    MENU.outln(F("VARIANT_MCK\tunknown"));
  #endif


  #if defined(ARDUINO_MODEL_PID)
    #if (ARDUINO_MODEL_PID == ARDUINO_UNO_PID)
      MENU.outln(F("ARDUINO_UNO_PID"));
    #elif (ARDUINO_MODEL_PID == ARDUINO_DUE_PID)
      MENU.outln(F("ARDUINO_DUE_PID"));
    #elif (ARDUINO_MODEL_PID == ARDUINO_MEGA2560_PID)
      MENU.outln(F("ARDUINO_MEGA2560_PID"));
    #endif
  #else
      // MENU.outln(F("ARDUINO_MODEL_PID is not defined.\n"));
  #endif


  // PINS_COUNT
  MENU.outln(F("\nPin counts"));
  #ifdef PINS_COUNT
    MENU.out(F("PINS_COUNT\t\t"));
    MENU.outln(PINS_COUNT);
  #else
    MENU.outln(F("PINS_COUNT\tnot given"));
  #endif


  // Pins:
  // see: <pins_arduino.h>
  #ifdef NUM_DIGITAL_PINS
    MENU.out(F("NUM_DIGITAL_PINS\t"));
    MENU.outln(NUM_DIGITAL_PINS);
  #else
    MENU.outln(F("NUM_DIGITAL_PINS\tunknown"));
  #endif

  #ifdef NUM_ANALOG_INPUTS
    MENU.out(F("NUM_ANALOG_INPUTS\t"));
    MENU.outln(NUM_ANALOG_INPUTS);
  #else
    MENU.outln(F("NUM_ANALOG_INPUTS\tunknown"));
  #endif

  #ifdef ADC_CHANNEL_NUMBER_NONE
    MENU.outln(F("ADC_CHANNEL_NUMBER_NONE"));
  #endif


  // check for macros
  MENU.outln(F("\nChecking for some macros"));

  MENU.out(F("digitalPinHasPWM\t"));
  #ifdef digitalPinHasPWM
    MENU.outln(YES_);
  #else
    MENU.outln(no_);
  #endif

  MENU.out(F("analogInputToDigitalPin\t"));
  #ifdef analogInputToDigitalPin
    MENU.outln(YES_);
  #else
    MENU.outln(no_);
  #endif

  MENU.out(F("digitalPinToPort\t"));
  #ifdef digitalPinToPort
    MENU.outln(YES_);
  #else
    MENU.outln(no_);
  #endif

  MENU.out(F("digitalPinToBitMask\t"));
  #ifdef digitalPinToBitMask
    MENU.outln(YES_);
  #else
    MENU.outln(no_);
  #endif

  MENU.out(F("digitalPinToTimer\t"));
  #ifdef digitalPinToTimer
    MENU.outln(YES_);
  #else
    MENU.outln(no_);
  #endif

  MENU.outln(F("\nPress 'p' for info about pins."));
}
#endif


/* ****************  cpp_info menu display:  **************** */

void cpp_info_display() {
  bool dummybool[8];

  MENU.outln(F("\nSize of some objects:"));
  MENU.out(F("sizeof(char)\t"));	MENU.outln(sizeof(char));
  MENU.out(F("sizeof(int)\t"));		MENU.outln(sizeof(int));
  MENU.out(F("sizeof(short)\t"));	MENU.outln(sizeof(short));
  MENU.out(F("sizeof(long)\t"));	MENU.outln(sizeof(long));
  MENU.out(F("sizeof(int*)\t"));	MENU.outln(sizeof(int*));
  MENU.out(F("sizeof(void*)\t"));	MENU.outln(sizeof(void*));
  MENU.out(F("sizeof(bool[8])\t"));	MENU.outln(sizeof(dummybool));

#ifdef USE_FLOAT_TYPES
  MENU.ln();
  MENU.out(F("sizeof(float)\t"));	MENU.outln(sizeof(float));
  MENU.out(F("sizeof(double)\t"));	MENU.outln(sizeof(double));
  MENU.out(F("sizeof(long double)\t"));	MENU.outln(sizeof(long double));
#endif

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
