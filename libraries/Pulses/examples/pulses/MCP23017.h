/*
  MCP23017.h

  very simple minded implementation using just one chip on Adafruit_MCP23017 library
*/
#ifndef MCP23017_H

#include <Wire.h>
  #include "Adafruit_MCP23017.h"
  Adafruit_MCP23017 MCP23017;

  void MCP23017_OUT_LOW() {
    for (int output=0; output<16; output++) {
      MCP23017.pinMode(output, OUTPUT);
      MCP23017.digitalWrite(output, LOW);
    }
  }

#define MCP23017_H
#endif
