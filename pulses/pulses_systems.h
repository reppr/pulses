#ifndef PULSES_SYSTEMS_H

#ifdef ARDUINO		// ARDUINO, ESP32, ESP8266 and similar boards
  #define STREAMTYPE	Stream

  #if ARDUINO >= 100
    #include "Arduino.h"
  #else
    #include "WProgram.h"
  #endif

  #define MENU_OUTSTREAM	Serial

#else	// #include's for Linux PC test version	*NOT SUPPORTED*

  #include <iostream>
  #define STREAMTYPE	ostream

#endif	// *not* ARDUINO, c++ Linux PC test version
/* **************************************************************** */

#define PULSES_SYSTEMS_H
#endif	// ! PULSES_SYSTEMS_H
