/*
  pulses_boards.h
  initialize Pulses depending the board it is running on
*/

#ifndef PULSES_BOARDS_H
  /* **************************************************************** */
  // Arduino board specific Pulses configuration:

#ifdef ARDUINO
  #if defined(ESP32)				// ESP32 :)
  /* OLED boards	*/
    #if defined BOARD_OLED_LIPO || defined BOARD_HELTEC_OLED
      #if ! defined HAS_OLED
	#define HAS_OLED
      #endif
      #define ESP32_DAC_ONLY_OLED	// not very flexible, but ok for now
    #else
      #if defined HAS_OLED
	#undef HAS_OLED
	#warning '*NOT* using monochrome display, type is unlnown'
	//#error 'MONOCHROME OLED DISPLAY BOARD TYPE UNKNOWN'
      #endif
    #endif

   #if defined ePaper213B73_BOARD_LILYGO_T5  ||  defined ePaper213BN_BOARD_LILYGO_BN
      #undef MORSE_TOUCH_INPUT_PIN
      #define MORSE_TOUCH_INPUT_PIN	33

      // #undef RGB_LED_STRIP_DATA_PIN		// *let* the warning appear
      #define RGB_LED_STRIP_DATA_PIN	27
   #endif // ePaper213B73_BOARD_LILYGO_T5 || ePaper213BN_BOARD_LILYGO_BN

   /* ESP32 DAC
      #define USE_DACs in your configuration files to use ESP32 DAC output	*/
#if CONFIG_IDF_TARGET_ESP32S3		// no DAC on ESP32s3
   #undef USE_DACs
   #undef BOARD_has_DACs
#else
    #define BOARD_has_DACs		2
    #define BOARD_DAC_RESOLUTION	8
    #define BOARD_DAC1			25	// DAC1 gpio
    #define BOARD_DAC2			26	// DAC2 gpio

    #if true	// change that if you do not want to use the dacs
       #define USE_DACs		BOARD_has_DACs
    #endif
#endif

   /* ESP32 touch sensors
   #define USE_TOUCH in your configuration files to use ESP32 touch sensors	*/

    #define BOARD_has_TOUCH		10

    #if true	// change that if you do not want to use touch interfaces
      //#define USE_TOUCH	BOARD_has_TOUCH		// to use all touch pads
      #define USE_TOUCH		1			// actually used
    #endif


#if ! defined PL_MAX	// see: pulses_project_conf.h
  #define PL_MAX		128	// often redefined in pulses_project_conf.h
#endif

    #define CODE_RAM_SIZE	256*3+2
    #define SCALES_RAM_SIZE	256*2+2
    // must be defined before including Pulses
    #define IMPLEMENT_TUNING		// needs float
    #undef USE_F_MACRO			// F() does not work here, FIXME: see also Menu.h  TODO: test again	################

    //#define USE_WIFI_telnet_menu	// use WIFI as menu over telnet?
    //#define AUTOSTART_WIFI	// start wifi on booting?  see: WiFi_stuff.ino

  #elif defined(ESP8266)			// ESP8266: "a lot of RAM"
    #define PL_MAX	32
    #define CODE_RAM_SIZE	256*3+2
    #define SCALES_RAM_SIZE	256*2+2
    // must be defined before including Pulses
    #define IMPLEMENT_TUNING		// needs float

    //#define USE_WIFI_telnet_menu	// use WIFI as menu over telnet?
    //#define AUTOSTART_WIFI		// start wifi on booting?  see: WiFi_stuff.ino

  #elif defined(__SAM3X8E__)			// Arduino DUE
    #define PL_MAX	64
    #define CODE_RAM_SIZE	256*3+2
    #define SCALES_RAM_SIZE	256*2+2
    // must be defined before including Pulses
    #define IMPLEMENT_TUNING		// needs float

  #elif defined(__AVR_ATmega2560__)		// mega2560
    #define PL_MAX	32	// test with more pins than 8 ;)
    #define CODE_RAM_SIZE	256*3+2
    #define SCALES_RAM_SIZE	256*2+2
//  #define RAM_IS_SCARE	// ################ FIXME: RAM_IS_SCARE ??? mega2560 ################

    // must be defined before including Pulses:
    // #define IMPLEMENT_TUNING		// *does not work on mega2560*
    // problem:
    //  double detune_number = PULSES.ticks_per_octave / PULSES.pulses[pulse].period.time;
    //  double detune = pow(2.0, 1.0/detune_number);	// crashes Arduino Mega2560

  #elif defined(__AVR_ATmega328P__)		// saving RAM on 328P	no recent tests
    #define RAM_IS_SCARE
    #define PL_MAX	12

  #else						// unknown board, defaults
    #warning unknown Arduino board
//  #define RAM_IS_SCARE	// ################ FIXME: RAM_IS_SCARE (other boards) ################
    #define PL_MAX	16
  #endif


// default number of GPIO_PINS controlling an associated hardware pin each
  #if ! defined GPIO_PINS && ! defined NO_GPIO_PINS
    #if defined(ESP32)			// ESP32: many pins, a lot of RAM :)
      #if defined(KALIMBA7_v2)
	#define GPIO_PINS	8	// only 7 used
      #else	// ESP32 generic
	#define GPIO_PINS	8
//      #define GPIO_PINS	16	//  ################ FIXME: GPIO_PINS  16 pulses on ESP32 broken
      #endif
    #elif defined(ESP8266)		// ESP8266: "a lot of RAM", 8 pins
      #define GPIO_PINS	8
    #elif defined(__AVR_ATmega2560__)
      #define GPIO_PINS	16      // mega2560 test with 16 pins ;)
    #elif defined(__SAM3X8E__)
      #define GPIO_PINS	32      // Arduino DUE test with 32 pins ;)
    #else
      #warning unknown Arduino board, edit  __FILE__:__LINE__
      #define GPIO_PINS	8       // default number of click frequencies on unknown boards
    #endif
  #endif

  #define MENU_OUTSTREAM	Serial

  #ifndef PWMRANGE	// old style arduino hardware
    #define PWMRANGE	255
  #endif

#else	// not ARDUINO, PC
  #warning: "PC version not supported, out of date"

  #define PL_MAX	64		// Linux PC test version

  #ifndef GPIO_PINS		// default number of click frequencies
     #define GPIO_PINS	0  // default number of click frequencies on PC, untested
  #endif

  #ifndef INPUT
    #define INPUT	0
  #endif
  #ifndef OUTPUT
    #define OUTPUT	1
  #endif
  #ifndef LOW
    #define LOW		0
  #endif
  #ifndef HIGH
    #define HIGH	1
  #endif

  // fakes for some Arduino functions:
  void pinMode(int p, int mode) {
    printf("Pin %d switched to mode %d\n", p, mode);
  }

  void digitalWrite(int p, int value) {
    printf("Pin %d set to %d\n", p, value);
  }

  unsigned long micros() { return 9999L; }


  // for Menu
  #define MENU_OUTSTREAM	cout

  int men_getchar() {
    return getchar();		// c++ Linux PC test version
  }

#endif	// *not* ARDUINO, c++ Linux PC test version


/* **************************************************************** */
// use Arduino F() macro to save RAM or just a NOOP?
#ifndef USE_F_MACRO	// NOOP fake
  // For tests and on PC:  Fake Arduino F("string") macro as NOOP:
  #if ! defined NOOP_FAKE_F_MACRO_INSTALLED	// probably obsolete, leave it in to make it more robust
    #define NOOP_FAKE_F_MACRO_INSTALLED
    #warning "using a NOOP fake F() macro."
    #undef F
    #define F(s)	((char*) (s))
  #endif
#endif

/* **************************************************************** */

#define PULSES_BOARDS_H
#endif	// no PULSES_BOARDS_H
