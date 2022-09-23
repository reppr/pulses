/*
  pulses_engine_config.h

  pulses inner program configuration file
  do not change
*/

#if ! defined PULSES_ENGINE_CONFIG_H

//#define NEW_LOOP_TASK_STACK_SIZE	16*1024		// TODO: experimental

#define DO_ON_OTHER_CORE_PRIORITY	1		// was: 0  new default: 1
#define MONOCHROME_PRIORITY		0		// was: 0  seemed best	TODO: test&trimm

// LEDC AUDIO TESTS DEACTIVATED:
//   #define USE_LEDC_AUDIO		// LEDC tests, experimental
//   //#define try_ARDUINO_LEDC_version	// prefer arduino ledc code over esp ide code? (no)
//   #define LEDC_INSTEAD_OF_DACs	// TESTING LEDC audio on hardware that was built for DAC audio on pins 25 & 26

#define ICODE_INSTEAD_OF_JIFFLES	// TESTING using all jiffles as icodes	TODO: REMOVE: after a while
					//   test speed, bugs
//	#if defined ICODE_INSTEAD_OF_JIFFLES	/*TODO:*/
//	  #define USED_DB	iCODEs		/*TODO:*/
//	#else					/*TODO:*/
//	  #define USED_DB	JIFFLES		/*TODO:*/
//	#endif					/*TODO:*/

#define MULTICORE_DISPLAY	// send hw display from other core (core 0)
				// to avoid audible clicks and dropouts

//#define MC_DISPLAY_STACK_SIZE		4*1024	// (default)
//#define MC_SAMPLE_MCU_STACK_SIZE	4*1024	// (default)

//#define DO_ON_OTHER_CORE_STACK_SIZE	8*1024	// (default)	old	TODO: test&trimm

// MULTICORE_RGB_STRING	does *not* avoid clicks, so deactivated
//#define MULTICORE_RGB_STRING	// avoid clicks, see: multicore_rgb_string_draw()

//#define MULTICORE_MPU_SAMPLING	// sample MPU from core 0	CRASHES VERY OFTEN

#define HAS_SOFTBOARD_MENU		// inlude Hardware Softboard menu

#define PRENAME_MAX_LENGTH	15	// *excluding* '\0'

#define PULSES_USE_DOUBLE_TIMES		// working fine :)

#if ! defined TRIGGERED_MUSICBOX2
  #define USE_ESP_NOW
//#define ESP_NOW_CHANNEL		11		// works fine TODO: UI
#endif

#define DO_STRESS_MANAGMENT
//#define STRESS_MONITOR_LEVEL	64*2	// TODO: menu interface	// TODO: move to another configuration file

#define USE_BLUETOOTH_SERIAL_MENU	// needs new git versions
/*
  see: https://github.com/pjalocha/esp32-ogn-tracker/issues/6
    esp_bt_gap_set_scan_mode(ESP_BT_SCAN_MODE_CONNECTABLE_DISCOVERABLE); // for older ESP-IDF
    esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE); // for newer ESP-IDF
*/
//#define BLUETOOTH_ENABLE_PIN	35	// used by some old instruments, probably obsolete

// #define ESP32_G15_T01	boards_layout/G15-T1-esp32_dev.h	// TODO: check, remove?
#define HARMONICAL_MUSIC_BOX
//#define SOFT_END_AFTER_N_CYCLE_SLICES		// BUGFIX: (safety net) make sure soft ending *IS* triggered

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

//#define USE_LEDC_AUDIO

/*
  OLED_HALT_PIN0	// halts display in monochrome_show_program_version() by pressing boot switch on GPIO 0
  then boots to user mode, see: force_start_to_usermode
  ATTENTION: *DANGEROUS!*	could possibly block boot sequence in systems with monochrome display code
  *not* tested with GPIO00 as click or such...
*/
#define OLED_HALT_PIN0	// see: monochrome_show_program_version()	// TODO: test with LilyGo T5_2.13

#if defined ESP32
  #define USE_NVS		// always used on ESP32
  #define USE_NVS_MENU		// to set up all the instruments...
#endif

#define PULSES_ENGINE_CONFIG_H
#endif
