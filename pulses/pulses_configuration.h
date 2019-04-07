/*
  pulses_configuration.h
*/

// TODO: FIXME: configuration
#include "pulses_boards.h"

#if defined ESP32				// ESP32 :)
 // DEFINE how many DACs you want to use
 #define USE_DACs		BOARD_has_DACs	// TODO: FIXME: configuration
#endif
