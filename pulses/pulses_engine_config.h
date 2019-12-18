/*
  pulses_engine_config.h

  pulses inner program configuration file
  do not change
*/

#if ! defined PULSES_ENGINE_CONFIG_H

#define PRENAME_MAX_LENGTH	15		// *excluding* '\0'


#define PULSES_USE_DOUBLE_TIMES		// working fine :)

#define USE_ESP_NOW	// starts working :)	// compiles fine when switched off, but
			// TODO: watch sketch size!	check WiFI and Bt stuff

/* CRASH!
  after #define PULSES_USE_DOUBLE_TIMES
  and   #define USE_ESP_NOW

working well for a while, then:
MusicBox is playing	ID not sent
---------  * * *  *                 '  S(14)	0.727778    131/180	w=1	now=7' 44"
---------** * *  *   *      *       '  S(17)	0.729630    197/270	w=1	now=7' 45"
---------* * *  *   *               '  S(16)	0.731481     79/108	w=1	now=7' 46"
--------- * *  *   *  *   *  *   *  '  S(16)	0.733333     11/15	w=2	now=7' 47"
---------* *  *   *      *          '  S(17)	0.735185    397/540	w=0	now=7' 48"
--------- *  *   *      *           '  S(15)	0.737037    199/270	w=1	now=7' 49"
---------* **  **      *            '  S(15)	0.738889    133/180	w=0	now=7' 51"
---------**  **     **     *      * '  S(17)	0.740741     20/27	w=0	now=7' 52"
---------  **     **                '  S(15)	0.742593    401/540	w=0	now=7' 53"
---------**    ***    *             '  S(15)	0.744444     67/90	w=1	now=7' 54"
---------  ****                     '  S(13)	0.746296    403/540	w=0	now=7' 55"
---------**                         '  S(8)	0.748148    101/135	w=1	now=7' 56"
Guru Meditation Error: Core  1 panic'ed (InstrFetchProhibited). Exception was unhandled.
Guru Meditation Error: Core  1 panic'ed (Interrupt wdt timeout on CPU1)
*/



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
  ATTENTION: *DANGEROUS!*	could possibly block boot sequence in systems with monochrome display code
  *not* tested with GPIO00 as click or such...
*/
//#define OLED_HALT_PIN0	// see: monochrome_show_program_version()

#if defined ESP32
  #define USE_NVS		// always used on ESP32
  #define USE_NVS_MENU		// to set up all the instruments...
#endif

#define PULSES_ENGINE_CONFIG_H
#endif
