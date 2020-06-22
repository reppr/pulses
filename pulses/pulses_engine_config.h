/*
  pulses_engine_config.h

  pulses inner program configuration file
  do not change
*/

#if ! defined PULSES_ENGINE_CONFIG_H

#define MULTICORE_DISPLAY	// send hw display from other core

#define PRENAME_MAX_LENGTH	15		// *excluding* '\0'

#define PULSES_USE_DOUBLE_TIMES		// working fine :)

#define USE_ESP_NOW	// starts working :)	// compiles fine when switched off, but
			// TODO: watch sketch size!	check WiFI and Bt stuff

#define DO_STRESS_MANAGMENT
//#define STRESS_MONITOR_LEVEL	64*2	// TODO: menu interface	// TODO: move to another configuration file

//#define USE_BLUETOOTH_SERIAL_MENU

// #define ESP32_G15_T01	boards_layout/G15-T1-esp32_dev.h	// TODO: check, remove?
#define HARMONICAL_MUSIC_BOX

#define USE_SYSTEM_MENU		// only a start...

#define USE_INPUTS		// not used any more, but left in for compilation test ;)
/* was:
   #ifndef RAM_IS_SCARE	// ################ FIXME: USE_INPUTS default condition ################
     #if ! defined SMALL_SKETCH_SIZE_TEST
       #define USE_INPUTS
     #endif
   #endif
*/

//#define USE_LEDC	// to be written ;)

#define USE_RTC_MODULE
//#define USE_i2c_SCANNER

//#define BATTERY_LEVEL_CONTROL_PIN switched in SETUP_BRACHE
//#define USE_LEDC_AUDIO	// not written yet ;)

/*
  OLED_HALT_PIN0	// halts display in monochrome_show_program_version() by pressing boot switch on GPIO 0
  then boots to user mode, see: force_start_to_usermode
  ATTENTION: *DANGEROUS!*	could possibly block boot sequence in systems with monochrome display code
  *not* tested with GPIO00 as click or such...
*/
#define OLED_HALT_PIN0	// see: monochrome_show_program_version()

#if defined ESP32
  #define USE_NVS		// always used on ESP32
  #define USE_NVS_MENU		// to set up all the instruments...
#endif

#define PULSES_ENGINE_CONFIG_H
#endif
