/* **************************************************************** */
/*
  Harmonical.cpp
*/
/* **************************************************************** */

#include <stdio.h>
#include <stdlib.h>

#ifdef ARDUINO

  /* Keep ARDUINO GUI happy ;(		*/
  #if ARDUINO >= 100
    #include "Arduino.h"
  #else
    #include "WProgram.h"
  #endif

  #include <Menu.h>
  #include <Pulses.h>

#else	// PC version *NOT SUPPORTED*
  #include <iostream>

  #include <Menu/Menu.h>
  #include <Pulses/Pulses.h>
#endif


/* **************************************************************** */
// Constructor/Destructor:

//	Harmonical::Harmonical(): {
//	}
//	
//	Harmonical::~Harmonical() {
//	}

/* **************************************************************** */
