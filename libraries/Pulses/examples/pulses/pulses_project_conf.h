/* **************************************************************** */
/*
  project_configuration.ino

  prepare compiling for special projects like the kalimbas
  (for default just leave this file untouched)
*/
/* **************************************************************** */
#ifndef PROJECT_CONFIGURATION_H

#if defined HACK_11_11_11_11	// hacked a setup for MagicalGuitar 11.11.2018 Zürich ;)
  #if ! defined USE_MORSE
    #define USE_MORSE
  #endif

  #if ! defined SOME_FIXED_TUNINGS_ONLY	// for jam sessions with metric tuned instruments
    #define SOME_FIXED_TUNINGS_ONLY
  #endif

  #undef MORSE_OUTPUT_PIN
  #define MORSE_OUTPUT_PIN	12
  #undef PERIPHERAL_POWER_SWITCH_PIN	// unused, was 12

#endif

/*  TIME_MACHINE did *not* use i2c	*i2c DEACTIVATED* see below */

/* **************************************************************** */
// You may want to configure special setups here:

/*
  You *can* setup and autostart some instrument setups
  by uncommenting *one* of the following lines

  in most cases you can let all these lines inactive, commented out
*/

#define ESP32_15_clicks_no_display_TIME_MACHINE2	// ESP32 new default
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
  #if defined ESP32_15_clicks_no_display_TIME_MACHINE2
// planed:  #define AUTOSTART	MENU.play_KB_macro("-E40U5 *64 n"); selected_experiment=-1;	// rhythm! PENTATONIC
  #if defined MAGICAL_MUSIC_BOX
    #define AUTOSTART	start_musicbox();
//    #define AUTOSTART	start_musicbox(); magic_trigger_ON();
  #else
// #define AUTOSTART	MENU.play_KB_macro("E40a5 S3 *2 J64j -n"); selected_experiment=-1;	// furzification test ;)
    #define AUTOSTART	MENU.play_KB_macro("-E40a5 *2 n"); selected_experiment=-1;	// Lichterfest	a5 == TIME MACHINE
// reused for LIVING ROOM 2018	"TIME MACHINE"  Robert Epprecht / Jürg Baruffol

// #define AUTOSTART	MENU.play_KB_macro("-E40F5 *2 n"); selected_experiment=-1;	// Lichterfest	F5
// #define AUTOSTART	MENU.play_KB_macro("-E40D *2S0 n"); selected_experiment=-1;	// Lichterfest	D BIG BANG
// #define AUTOSTART	MENU.play_KB_macro("-E40 *2 n"); selected_experiment=-1;	// Lichterfest	a	default
  #endif
//    #define AUTOSTART	MENU.play_KB_macro("-E40 S0 n"); selected_experiment=-1;	// the big bang
    #define PL_MAX	96	// *deactivates*  #define in pulses_boards.h

/*  TIME_MACHINE did not use i2c	*i2c DEACTIVATED*
    #define USE_i2c
    #define USE_MCP23017	Adafruit
*/

  #elif defined ESP32_15_clicks_no_display_TIME_MACHINE1
    #define AUTOSTART	MENU.play_KB_macro("E38!"); selected_experiment=-1;
    #define PL_MAX	64	// *deactivates*  #define in pulses_boards.h
  #elif defined ESP32_15_clicks_no_display
    #define AUTOSTART	MENU.play_KB_macro("V15 - E37D"); selected_experiment=-1;	// about 5'34"
    //#define AUTOSTART	MENU.play_KB_macro("V15 - E37A"); selected_experiment=-1;	// about 3'44"
    //#define AUTOSTART	MENU.play_KB_macro("V15 - E37E"); selected_experiment=-1;	// about 4'57"
  #elif defined ESP32_13_clicks_v0
    #define AUTOSTART	MENU.play_KB_macro("V13 - E37A"); selected_experiment=-1;
  #elif defined GUITAR_v0	// GUITAR with surface vibration speaker on ESP32 DACs
    //#define AUTOSTART	MENU.play_KB_macro("- E37e5 *2"); selected_experiment=-1; // Guitar
    //#define AUTOSTART	MENU.play_KB_macro("- E37d"); selected_experiment=-1; // Santur in d
    //#define AUTOSTART	MENU.play_KB_macro("- E37G"); selected_experiment=-1; // Santur in G
    #define AUTOSTART	MENU.play_KB_macro("- E37f"); selected_experiment=-1; // Santur in f	// GUITAR default is SANTUR ;)
  #elif defined ESP32_12	// more voices, please
    #define AUTOSTART	MENU.play_KB_macro("E32 -"); selected_experiment=-1;
  #elif defined KALIMBA7_v2	// KALIMBA ESP32 version
    #define AUTOSTART	MENU.play_KB_macro("E31 -"); selected_experiment=-1;
    // #define MORSE_TOUCH_INPUT_PIN	13
#endif

#elif defined ESP8266	// DEFAULT ON ESP8266  KALIMBA7_v1 ESP8266 nodemcu version
  #define KALIMBA7_v1	// KALIMBA7_v1 ESP8266 nodemcu version
  #define AUTOSTART	MENU.play_KB_macro("E30 -"); selected_experiment=-1;
#endif

#ifdef NO_AUTOSTART_INSTRUMENT
  #undef AUTOSTART
#endif

/* **************************************************************** */
#define PROJECT_CONFIGURATION_H
#endif
