/*
  pulses_configuration.h
*/

#include "pulses_boards.h"

#if defined ESP32				// ESP32 :)
 // DEFINE how many DACs you want to use
 #define USE_DACs		BOARD_has_DACs
#endif
