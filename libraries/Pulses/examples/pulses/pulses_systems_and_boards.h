#ifndef PULSES_SYSTEMS_AND_BOARDS_H
#define PULSES_SYSTEMS_AND_BOARDS_H

#ifdef ARDUINO		// ARDUINO, ESP8266 and similar boards
  #define STREAMTYPE	Stream

  #if ARDUINO >= 100
    #include "Arduino.h"
  #else
    #include "WProgram.h"
  #endif


  /* **************************************************************** */
  // Arduino board specific configutation:

  #if defined(ESP8266)				// ESP8266: "a lot of RAM"
    const int pl_max=32;
    #define JIFFLE_RAM_SIZE	256*3+1
    #define RATIOS_RAM_SIZE	256*2+1
    // must be defined before including Pulses
    #define IMPLEMENT_TUNING		// needs float

  #elif defined(__AVR_ATmega2560__)		// mega2560
    const int pl_max=32;	// test with more pins than 8 ;)
    #define JIFFLE_RAM_SIZE	256*3+1
    #define RATIOS_RAM_SIZE	256*2+1
    // must be defined before including Pulses
    // #define IMPLEMENT_TUNING		// FIXME: *does not work on mega2560*	################

  #elif defined(__AVR_ATmega328P__)		// saving RAM on 328P	no recent tests
    const int pl_max=12;

  #elif defined(__SAM3X8E__)			// Arduino DUE
    const int pl_max=32;
    #define JIFFLE_RAM_SIZE	256*3+1
    #define RATIOS_RAM_SIZE	256*2+1
    // must be defined before including Pulses
    #define IMPLEMENT_TUNING		// needs float

  #else						// unknown board, defaults
    const int pl_max=16;
  #endif

  // default number of CLICK_PULSES controlling an associated hardware pin each
  #ifndef CLICK_PULSES
    #if defined(ESP8266)		// ESP8266: "a lot of RAM"
      #define CLICK_PULSES	8
    #elif defined(__AVR_ATmega2560__)
      #define CLICK_PULSES	16      // mega2560 test with 16 pins ;)
    #else
      #define CLICK_PULSES	6       // default number of click frequencies on unknown boards
    #endif
  #endif

  #define MENU_OUTSTREAM	Serial

// FIXME: why does this not work here?
//	  // for Menu
//	  int men_getchar() {
//	    if (!Serial.available())	// ARDUINO
//	      return EOF;
//
//	    return Serial.read();
//	  }

#else	// #include's for Linux PC test version	*NOT SUPPORTED*
  #include <iostream>

  const int pl_max=64;		// Linux PC test version

  #ifndef CLICK_PULSES		// default number of click frequencies
     #define CLICK_PULSES	0  // default number of click frequencies on PC, untested
  #endif

  #ifndef STREAMTYPE		// could have been #define'd in Menu.h
    using namespace std;
    ostream & Serial=cout;	// nice trick from johncc
  #endif
  #define STREAMTYPE	ostream

  #ifndef INPUT
    #define INPUT	0
  #endif
  #ifndef OUTPUT
    #define OUTPUT	1
  #endif
  #ifndef LOW
    #define LOW		0
  #endif
  #ifndef HIGH
    #define HIGH	1
  #endif

  // fakes for some Arduino functions:
  void pinMode(int p, int mode) {
    printf("Pin %d switched to mode %d\n", p, mode);
  }

  void digitalWrite(int p, int value) {
    printf("Pin %d set to %d\n", p, value);
  }

  unsigned long micros() { return 9999L; }


  // for Menu
  #define MENU_OUTSTREAM	cout

  int men_getchar() {
    return getchar();		// c++ Linux PC test version
  }

#endif	// *not* ARDUINO, c++ Linux PC test version


// use Arduino F() macro to save RAM or just a NOOP?
#ifndef USE_F_MACRO	// NOOP fake
  // For tests and on PC:  Fake Arduino F("string") macro as NOOP:
  #undef F
  #define F(s)	(s)
  #warning "using a NOOP fake F() macro."
#endif


#endif	// no PULSES_SYSTEMS_AND_BOARDS_H
