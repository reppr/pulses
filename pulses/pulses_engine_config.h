/*
  pulses_engine_config.h

  pulses inner program configuration file
  do not change
*/

#define DO_STRESS_MANAGMENT
//#define STRESS_MONITOR_LEVEL	64*2	// TODO: menu interface	// TODO: move to another configuration file

#define USE_BLUETOOTH_SERIAL_MENU

// #define ESP32_G15_T01	boards_layout/G15-T1-esp32_dev.h	// TODO: check, remove?
#define HARMONICAL_MUSIC_BOX

//#define USE_MORSE	// incomplete	DEACTIVATED MORSE OUTPUT, (== PERIPHERAL_POWER_SWITCH_PIN ;)
//#define USE_INPUTS
//#define USE_LEDC	// to be written ;)

#define USE_RTC_MODULE
//#define USE_i2c_SCANNER

// #define USE_BATTERY_CONTROL switched on in SETUP_BRACHE
//#define USE_LEDC_AUDIO	// not written yet ;)

/*
  OLED_HALT_PIN0	// halts display in display_program_version() by pressing boot switch on GPIO 0
  ATTENTION: *DANGEROUS!*	could possibly block boot sequence in systems with monochrome display code
  *not* tested with GPIO00 as click or such...
*/
#define OLED_HALT_PIN0	// see: display_program_version()
