#ifndef PULSES_SYSTEMS_AND_BOARDS_H
#define PULSES_SYSTEMS_AND_BOARDS_H

#ifdef ARDUINO		// ARDUINO, ESP8266 and similar boards
  #define STREAMTYPE	Stream

  #if ARDUINO >= 100
    #include "Arduino.h"
  #else
    #include "WProgram.h"
  #endif

  #ifdef __SAM3X8E__
    const int pl_max=32;		// could be more on DUE ;)
    // *no* USE_F_MACRO on the DUE	FIXME: test	################
  #else
    #define USE_F_MACRO			// *not* on the DUE, FIXME: test	################

    #ifdef ESP8266
      const int pl_max=32;		// ESP8266: a lot of RAM
      // must be defined before including Pulses
      #define IMPLEMENT_TUNING	// needs float
    #else
      #ifdef __AVR_ATmega328P__
         const int pl_max=12;		// saving RAM on 328P
      #else
         const int pl_max=16;		// default i.e. mega boards
      #endif
    #endif
  #endif

  #ifndef CLICK_PULSES		// default number of click frequencies
    #ifdef ESP8266
      #define CLICK_PULSES	8       // default number of clicks 8
    #else
      #define CLICK_PULSES	6       // default number of click frequencies
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
