/* **************************************************************** */
/*
  pulses_project_conf.h

  prepare compiling for special projects like the kalimbas,
  dac only minimal boards, dac with OLED, etc.

  (for default just leave this file untouched)

*/
/* **************************************************************** */
#ifndef PULSES_PROJECT_CONF_H

/*  TIME_MACHINE did *not* use i2c	*i2c DEACTIVATED* see below */


/* **************************************************************** */
// You may want to configure special setups here:

/*
  You *can* setup and autostart some instrument setups
  by uncommenting *one* of the following lines

  in most cases you can let all these lines inactive, commented out
*/

#if defined TRIGGERED_MUSICBOX_LILYGO_213		// define this in my_pulses_config.h
  #if defined ePaper213B73_BOARD_LILYGO_T5  ||  defined ePaper213BN_BOARD_LILYGO_BN
    #define ePAPER_SMALL_213
    #if defined ePaper213B73_BOARD_LILYGO_T5  &&  defined ePaper213BN_BOARD_LILYGO_BN
      #error "define *ONE* of ePaper213B73_BOARD_LILYGO_T5 *OR* ePaper213BN_BOARD_LILYGO_BN"
    #endif

    #define ePAPER_SHOW_CYCLE_bar	// while running do show a cycle progress bar over musicBox_parameters screen

    //#define USE_SD_CARD		// #undef that to SAVE RAM?
    #if defined USE_SD_CARD
      #define USE_LOGGING
      #define LOG_PLAY_DEFAULT
      #define LOG_BATTERY_DEFAULT
    #endif

    #undef RGB_STRING_LED_CNT			// WAS: 150
    #define RGB_STRING_LED_CNT		30	// NEW: 30 (50cm 5V string)
  #else
    #error "#define ePaper213B73_BOARD_LILYGO_T5 *or* ePaper213BN_BOARD_LILYGO_BN"
  #endif

  #if ! defined ESP32
    #error  TRIGGERED_MUSICBOX_LILYGO_213 *only* on ESP32
  #endif

  #define TRIGGERED_MUSICBOX2
  #undef MUSICBOX2_PIN_MAPPING

  #define NO_GPIO_PINS				// DAC only
  #undef OLED_HALT_PIN0

  //#undef PERIPHERAL_POWER_SWITCH_PIN		// was: 12	maybe let the warning appear?
  #define PERIPHERAL_POWER_SWITCH_PIN	32	// <<< NEW >>> LilyGo 2.13

  //#undef MORSE_OUTPUT_PIN			// was: 12	maybe let the warning appear?
  #if defined USE_MORSE
    #define MORSE_OUTPUT_PIN		0	// <<< NEW >>> LilyGo 2.13
    #define MORSE_TOUCH_INPUT_PIN	33	// MORSE TOUCH INPUT
  #endif

  #define RGB_LED_STRIP_DATA_PIN	27	// testing 27, 14
  #define MUSICBOX_TRIGGER_PIN		34	// activates trigger pin, needs pulldown (i.e. 470k, 100k ok)
  #define BATTERY_LEVEL_CONTROL_PIN	36	// (35 is used for internal LiPo battery level?)

  #define SHOW_ePAPER_UPDATE_ON_END		// ePaper display update on hard END

  // some options:
  //#define MIDI_OUT_PIN		19
  //#define USE_RTC_MODULE
#endif // TRIGGERED_MUSICBOX_LILYGO_213


#if defined TRIGGERED_MUSICBOX2		// also for TRIGGERED_MUSICBOX_LILYGO_213	// define in my_pulses_config.h
  #if ! defined ESP32
    #error TRIGGERED_MUSICBOX2 *only* on ESP32
  #endif

  #if ! defined TRIGGERED_MUSICBOX_LILYGO_213
    #define MUSICBOX2_PIN_MAPPING		// new pin mapping april 2021	(see below)
  #endif

//#define PROGRAM_SUB_VERSION	BrachenSound
  #define MAGICAL_TOILET_HACK_2	// continue using (parts of) setup_bass_middle_high() to setup musicbox
  #undef AUTOSTART
  #define AUTOSTART	play_random_preset();		// in pulses_project_conf.h
  #undef MUSICBOX_WHEN_DONE_FUNCTION_DEFAULT
  #define MUSICBOX_WHEN_DONE_FUNCTION_DEFAULT	&light_sleep	// do test for dac noise...
  #define SOFT_END_AFTER_N_CYCLE_SLICES		// BUGFIX: (safety net) make sure soft ending *IS* triggered

  // some options:
  #define USE_BATTERY_LEVEL_CONTROL
//#define HAS_ePaper290_on_PICO_KIT		// triggers HAS_ePaper and HAS_DISPLAY
//#define BOARD_HELTEC_OLED			// either OLED or ePaper, not both
//#define USE_RTC_MODULE			// DS3231
//#undef USE_ESP_NOW				// default is ON
//#define USE_BLUETOOTH_SERIAL_MENU		// DADA: *does* compile on ESP32-Arduino 2.0.8  *but* RAM goes very low...
//#define BLUETOOTH_ENABLE_PIN	35		// used by some OLD instruments, probably OBSOLETE

  #undef AUTOSTART	// DADA	*NO*, *PORTABLE* not triggered
/* do these 2 lines make a difference? */
  #define AUTOSTART	play_random_preset(); musicBox_when_done=&user;		// same as pulses_project_conf.h
//#define AUTOSTART	play_random_preset();musicBox_when_done=&user;		// same as pulses_project_conf.h
#endif	// TRIGGERED_MUSICBOX2


#if defined MUSICBOX2_PIN_MAPPING	// new pin mapping april 2021
  #if ! defined ESP32
    #error MUSICBOX2_PIN_MAPPING *only* on ESP32
  #endif

  #define NO_GPIO_PINS				// DAC only
//#undef PERIPHERAL_POWER_SWITCH_PIN		// maybe let the warning appear?
  #define PERIPHERAL_POWER_SWITCH_PIN	2	// <<< NEW >>>
  #if defined USE_MORSE
    #define MORSE_OUTPUT_PIN		12
    #define MORSE_TOUCH_INPUT_PIN	13	// ok
  #endif
  #define RGB_LED_STRIP_DATA_PIN	14	// testing 27, 14
//#define RGB_LED_STRIP_DATA_PIN	27	// testing 27, 14
  #define MUSICBOX_TRIGGER_PIN		34	// activates trigger pin, needs pulldown (i.e. 470k, 100k ok)
  #define BATTERY_LEVEL_CONTROL_PIN	35	// NEW default, (was: 36)
//#define HAS_ePaper290_on_PICO_KIT		// (reserve these pins for optional ePaper display)
#endif // MUSICBOX2_PIN_MAPPING



//#define ESP32_15_clicks_no_display_TIME_MACHINE2	// ESP32 new default

//#define ESP32_DAC_ONLY			// minimal setup to play on DACs only
//#define ESP32_DAC_ONLY_OLED			// minimal setup to play on DACs only with OLED display

// #define ESP32_15_clicks_no_display_TIME_MACHINE1	// ESP32 (prior default)

// #define ESP32_15_clicks_no_display
// #define GUITAR_v0	// autostart guitar E37  on ESP32 DAC	|  // Santur in G
// #define KALIMBA7_v2	// autostart KALIMBA ESP32 version
// #define ESP32_12	// autostart 12 click ESP32 mode

/* next one is default on ESP8266	*/
// #define KALIMBA7_v1	// autostart ESP8266 nodemcu version


/*
  uncomment following line to *switch off autostart*
  of your configured instrument above

  this has *no* influence on autostarting WiFi
*/
//#define NO_AUTOSTART_INSTRUMENT


// WiFi autostart on ESP8266 and ESP32?
#if defined ESP8266 || defined ESP32
  // uncomment the following line to autostart WiFi on ESP devices:
  //#define AUTOSTART_WIFI	setup_wifi_telnet();
#endif

// end of instrument configuration
/* **************************************************************** */


/* **************************************************************** */
// do not change the following code

#if defined ESP32
  /* OLED boards	*/
  #if defined BOARD_OLED_LIPO || defined BOARD_HELTEC_OLED
    #if ! defined USE_MONOCHROME_DISPLAY
      #define USE_MONOCHROME_DISPLAY
    #endif
    #define ESP32_DAC_ONLY_OLED		// not very flexible, but ok for now???

    #undef MORSE_OUTPUT_PIN			// was: 12	let the warning appear?
    #define MORSE_OUTPUT_PIN		0	// <<< NEW >>> try also on BOARD_HELTEC_OLED
  #endif // _OLED_BOARD

  #if defined ESP32_DAC_ONLY
    #define PROGRAM_SUB_VERSION			SoundShip1

//  #if defined USE_MONOCHROME_DISPLAY		// I think we do not want this?
//    #warning *NOT* using monochrome display
//    #undef USE_MONOCHROME_DISPLAY
//  #endif

    #if ! defined MUSICBOX_WHEN_DONE_FUNCTION_DEFAULT
      #define MUSICBOX_WHEN_DONE_FUNCTION_DEFAULT	&user	// a possible snoring workaround on usb dac only models
    #endif

  #elif defined ESP32_DAC_ONLY_OLED
    #define PROGRAM_SUB_VERSION			DAC_only_OLED

    #define MAX_SUBCYCLE_SECONDS		21*60	// *max seconds*, produce sample pieces	ESP32_usb_DAC_only

    #if ! defined NO_GPIO_PINS
      #define NO_GPIO_PINS
    #endif

    #if ! defined USE_MONOCHROME_DISPLAY
      #define USE_MONOCHROME_DISPLAY
    #endif

    #define MUSICBOX_WHEN_DONE_FUNCTION_DEFAULT		&user	// a possible snoring workaround on usb dac only models
  #endif

  #if defined ESP32_15_clicks_no_display_TIME_MACHINE2 || defined ESP32_DAC_ONLY || defined ESP32_DAC_ONLY_OLED
    #if defined HARMONICAL_MUSIC_BOX
      #if ! defined AUTOSTART
        #define AUTOSTART	start_musicBox();
      #endif
//    #define AUTOSTART	start_musicBox(); magic_trigger_ON();
    #else
      #define AUTOSTART	MENU.play_KB_macro("-E40a5 *2 n"); selected_experiment=-1;	// Lichterfest	a5 == TIME MACHINE
	// reused for LIVING ROOM 2018	"TIME MACHINE"  Robert Epprecht / Jürg Baruffol

	// #define AUTOSTART	MENU.play_KB_macro("-E40F5 *2 n"); selected_experiment=-1;	// Lichterfest	F5
	// #define AUTOSTART	MENU.play_KB_macro("-E40D *2S0 n"); selected_experiment=-1;	// Lichterfest	D BIG BANG
	// #define AUTOSTART	MENU.play_KB_macro("-E40 *2 n"); selected_experiment=-1;	// Lichterfest	a	default
    #endif
//    #define AUTOSTART	MENU.play_KB_macro("-E40 S0 n"); selected_experiment=-1;	// the big bang

    #undef  PL_MAX		// was: 128  *deactivates*  #define in pulses_boards.h
    #define PL_MAX	96	// *deactivates*  #define in pulses_boards.h

/*  TIME_MACHINE did not use i2c	*i2c DEACTIVATED*
    #define USE_i2c
    #define USE_MCP23017	Adafruit
*/

    #if defined ESP32_DAC_ONLY || defined ESP32_DAC_ONLY_OLED
      #define NO_GPIO_PINS
    #endif

    #if defined ESP32_DAC_ONLY_OLED
      #define USE_MONOCHROME_DISPLAY
    #endif

  #elif defined ESP32_15_clicks_no_display_TIME_MACHINE1
    #define PROGRAM_SUB_VERSION	TIME_MACHINE1
    #define AUTOSTART	MENU.play_KB_macro("E38!"); selected_experiment=-1;
    #define PL_MAX	64	// *deactivates*  #define in pulses_boards.h
  #elif defined ESP32_15_clicks_no_display
    #define PROGRAM_SUB_VERSION	15_clicks_no_dis
    #define AUTOSTART	MENU.play_KB_macro("V15 - E37D"); selected_experiment=-1;	// about 5'34"
    //#define AUTOSTART	MENU.play_KB_macro("V15 - E37A"); selected_experiment=-1;	// about 3'44"
    //#define AUTOSTART	MENU.play_KB_macro("V15 - E37E"); selected_experiment=-1;	// about 4'57"
  #elif defined ESP32_13_clicks_v0
    #define PROGRAM_SUB_VERSION	13_clicks_v0
    #define AUTOSTART	MENU.play_KB_macro("V13 - E37A"); selected_experiment=-1;
  #elif defined GUITAR_v0	// GUITAR with surface vibration speaker on ESP32 DACs
    //#define AUTOSTART	MENU.play_KB_macro("- E37e5 *2"); selected_experiment=-1; // Guitar
    //#define AUTOSTART	MENU.play_KB_macro("- E37d"); selected_experiment=-1; // Santur in d
    //#define AUTOSTART	MENU.play_KB_macro("- E37G"); selected_experiment=-1; // Santur in G
    #define AUTOSTART	MENU.play_KB_macro("- E37f"); selected_experiment=-1; // Santur in f	// GUITAR default is SANTUR ;)
  #elif defined ESP32_12	// more voices, please
    #define PROGRAM_SUB_VERSION	ESP32_12_old
    #define AUTOSTART	MENU.play_KB_macro("E32 -"); selected_experiment=-1;
  #elif defined KALIMBA7_v2	// KALIMBA ESP32 version
    #define PROGRAM_SUB_VERSION	ESP32KALIMBA7_v2
    #define AUTOSTART	MENU.play_KB_macro("E31 -"); selected_experiment=-1;
    // #define MORSE_TOUCH_INPUT_PIN	13
#endif

#elif defined ESP8266	// DEFAULT ON ESP8266  KALIMBA7_v1 ESP8266 nodemcu version
  #define KALIMBA7_v1	// KALIMBA7_v1 ESP8266 nodemcu version
  #define PROGRAM_SUB_VERSION	E8266KALIMBA7_v1
  #define AUTOSTART	MENU.play_KB_macro("E30 -"); selected_experiment=-1;
#endif

#if defined RANDOM_PRESET_LOOP
  #undef AUTOSTART
  #define AUTOSTART	  MENU.play_KB_macro(F(":M y0"));		// start random preset
#endif

#ifdef NO_AUTOSTART_INSTRUMENT
  #undef AUTOSTART
#endif

#if defined SETUP_BRACHE_TRIGGERED_PRESETs	// same as musicBox.h
  #undef AUTOSTART
  #define AUTOSTART	play_random_preset();	// same as musicBox.h
#endif

#if defined HELTEC_OLED_PORTABLE22
  #define MUSICBOX_SHOW_PROGRAM_VERSION		// TODO: test compatibility with ESP-NOW synchronisation!

//  #if defined USE_BATTERY_LEVEL_CONTROL	// without USE_BATTERY_LEVEL_CONTROL RGB LEDS to *not* work, very strange!
//    #warning 'pulses_project_conf.h switched USE_BATTERY_LEVEL_CONTROL off'
//    #undef USE_BATTERY_LEVEL_CONTROL
//    #define BATTERY_LEVEL_CONTROL_PIN	255
//  #endif
#endif

/* **************************************************************** */
#define PULSES_PROJECT_CONF_H
#endif
