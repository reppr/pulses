/*
  my_pulses_config.h

  this file is for configuration options likely to change often
  put your own configuration here

*/

#if ! defined MY_PULSES_CONFIG_H

#define FAMILY_NAME	SoundShipBand		// configuration family	// see: pulses_engine_config.h
// /*			0123456789abcdef	// 16 bytes	*/

#define MAGICAL_TOILET_HACK_2

#define TRIGGERED_MUSICBOX2	// #define this in my_pulses_config.h	see: pulses_project_conf.h
//#define MUSICBOX2_PIN_MAPPING	// (new pin mapping april 2021)

// some options for TRIGGERED_MUSICBOX2:
//#define HAS_ePaper290_on_PICO_KIT
//#define USE_RTC_MODULE			// DS3231
//#define USE_ESP_NOW				// possible, if you want that

#define TRIGGERED_MUSICBOX_LILYGO_213	// also #define *ONE* of ePaper213B73_BOARD_LILYGO_T5 *OR* ePaper213BN_BOARD_LILYGO_BN"
#if defined TRIGGERED_MUSICBOX_LILYGO_213	// also #define *ONE* of ePaper213B73_BOARD_LILYGO_T5 *OR* ePaper213BN_BOARD_LILYGO_BN"
  #define ePaper213BN_BOARD_LILYGO_BN		// (NEW) T5_V2.3.1    20.8.26	triggers HAS_ePaper and HAS_DISPLAY
//#define ePaper213B73_BOARD_LILYGO_T5		// (OLD) T5_V2.3_2.13 20190107	triggers HAS_ePaper and HAS_DISPLAY

  #define BATTERY_LEVELS_DOUBLED	// 12V ~ 2400
  #if defined ePaper213B73_BOARD_LILYGO_T5  ||  defined ePaper213BN_BOARD_LILYGO_BN
    #define DEBUG_ePAPER_MORSE_FEEDBACK		// TODO: remove
    // ok
  #else
    #error "#define one of ePaper213B73_BOARD_LILYGO_T5 *or* ePaper213BN_BOARD_LILYGO_BN"
  #endif
#endif

#define USE_BATTERY_LEVEL_CONTROL


//#define USE_LoRa		// needs: https://github.com/sandeepmistry/arduino-LoRa
				//	  https://github.com/khoih-prog/ESP32TimerInterrupt
#if defined USE_LoRa
  #define USE_LoRa_EXPLORING
//#define LoRa_RECEIVE_BUF_SIZE		64	// save some RAM?
  #define LoRa_RECEIVE_BUF_SIZE		256	// 256 (for chatting and debugging)	default 128
//#define LoRa_SEND_PING_ON_STARTUP
#endif

//#define FORCE_START_TO_USERMODE	// needs OLED_HALT_PIN0 to be declared


// DISPLAY HARDWARE:	activate ZERO or *ONE* of the following DISPLAYs
//
// OLED DISPLAY?
//#define BOARD_HELTEC_OLED	// Heltec OLED BOARD		// triggers HAS_OLED and HAS_DISPLAY
//#define BOARD_OLED_LIPO	// LiPo battery OLED BOARD	// triggers HAS_OLED and HAS_DISPLAY
//
// ePaper DISPLAY?
//#define HAS_ePaper290_on_PICO_KIT	// old		triggers HAS_ePaper and HAS_DISPLAY
//#define HAS_ePaper290_on_DEV_KIT	// old		triggers HAS_ePaper and HAS_DISPLAY

//#define ePaper213BN_BOARD_LILYGO_BN	// new  T5_V2.3.1     20-8-26	triggers HAS_ePaper and HAS_DISPLAY
//#define ePaper213B73_BOARD_LILYGO_T5	// old  T5_V2.3_2.13  20190107	triggers HAS_ePaper and HAS_DISPLAY

//#define ePaper_DEPG0290B_LILYGO_0290	// very first tests only...

//#define USE_MIDI		// *only* compile MIDI if you *do* intend to use it
#if defined USE_MIDI
  #define MIDI_OUT_PIN	19	// configured as Serial2 TX
  //#define MIDI_IN_PIN	39	// configured as Serial2 RX
  #define MIDI_IN_PIN	27	// configured as Serial2 RX	TODO: was: 18,	maybe use 27?
  //#define MIDI_BAUDRATE	31250	// *REAL* MIDI IO
  #define MIDI_BAUDRATE	115200	// testing faster MIDI over UART
  //#define MIDI_DOES_PITCH_BEND	// probably won't use this, so just a pp macro
#endif

//#define USE_ADS1115_AT_ADDR	0x48

#define USE_RGB_LED_STRIP
#if defined USE_RGB_LED_STRIP
  #if defined TRIGGERED_MUSICBOX_LILYGO_213
    #define RGB_LED_STRIP_DATA_PIN	27	// GPIO27  (GPIO14 in internal use)
    #define RGB_STRING_LED_CNT		30	// 50cm 5Volt string: number of RGB leds in the string
  #else
    #define RGB_LED_STRIP_DATA_PIN	14	// use GPIO14 || GPIO27
//  #define RGB_STRING_LED_CNT		144	// number of RGB leds in the string
    #define RGB_STRING_LED_CNT		150	// number of RGB leds in the string
  #endif
// DEFAULT_LED_STRING_INTENSITY
#endif

#define USE_MORSE	// touch morse code is in use :)
#if defined MUSICBOX2_PIN_MAPPING	// #define this in my_pulses_config.h
  #define MORSE_OUTPUT_PIN	12
#else
  #define MORSE_OUTPUT_PIN	PERIPHERAL_POWER_SWITCH_PIN	// *ONLY* possible if there's *NO POWER SWITCH HARDWARE*
#endif

// #define USE_MPU6050		// MPU-6050 6d accelero/gyro
#define INCLUDE_IMU_ZERO

#if defined MUSICBOX2_PIN_MAPPING	// #define this in my_pulses_config.h
  #define PERIPHERAL_POWER_SWITCH_PIN	2
#else
  #define PERIPHERAL_POWER_SWITCH_PIN	12	// == MORSE_OUTPUT_PIN	green LED in many musicBoxes
  // TODO: change PERIPHERAL_POWER_SWITCH_PIN as GPIO12 is active during boot process...
#endif

#define ESP32_DAC_ONLY

//#define USE_RTC_MODULE	// DS3231 TEST only	TODO: REMOVE: (from here) ################


#if defined ESP32
 // DEFINE how many DACs you want to use
 #define USE_DACs		BOARD_has_DACs	// TODO: FIXME: configuration sequence
#endif


// magic musicBox can produce tunings compatible to a=440 tunings only:
#define SOME_METRIC_TUNINGS_ONLY_DEFAULT	true	// fixed pitchs only like E A D G C F B  was: SOME_FIXED_TUNINGS_ONLY
//#define SOME_METRIC_TUNINGS_ONLY_DEFAULT	false;	// free pitch tuning

//#define MUSICBOX_SHOW_PROGRAM_VERSION	// start_musicBox() shows program version on OLED	*DEFAULT OFF*

// some defaults you can change:
//#define SHOW_CYCLE_PATTERN_DEFAULT		false
//#define SHOW_SUBCYCLE_POSITION_DEFAULT	false

//#define MONOCHROME_MOTION_STATE_ROW	7
//#define MONOCHROME_MOTION_MUTING_ROW	0

#define HIGH_PRIORITY_RGB_LED_UPDATE		// TODO: TEST MOVE:
#define RGB_LED_STRING_VOLTAGE_TYPE	5	// TODO: TEST MOVE:

#define MY_PULSES_CONFIG_H
#endif
