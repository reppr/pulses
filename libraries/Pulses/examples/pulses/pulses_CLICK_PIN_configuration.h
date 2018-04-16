/* pulses_CLICK_PIN_configuration.h	*/

#ifndef CLICK_PIN_CONFIGURATION.H
#define CLICK_PIN_CONFIGURATION.H

#if defined(CLICK_PULSES)

// which board?
#if defined ESP32 || defined ESP8266			// ESPxx boards


  #if defined ESP32_13_clicks		//  ESP32_13_clicks	TESTED
    #undef CLICK_PULSES
    #define CLICK_PULSES	13
    gpio_pin_t click_pin[CLICK_PULSES] =
      {0, 2, 4, 5, 13, 14, 15, 16, 17, 23, 27, 32, 33, };	//  ESP32_13_clicks	TESTED
//    ok,ok,ok,ok. ok, ok, ok, ok, ok, ok, ok, ok, ok,
  #elif defined ESP32_15_clicks_no_display	//  ESP32_15_clicks_no_display, 15 clicks TESTED
    #undef CLICK_PULSES
    #define CLICK_PULSES	15
    gpio_pin_t click_pin[CLICK_PULSES] =				//  ESP32_15_clicks_no_display	15 clicks TESTED
      {0, 2, 4, 5, 13, 14, 15, 16, 17, 23, 27, 32, 33, 18, 19, };

  #elif defined ESP32_12_v0
  // testing ESP32_12 version
    #undef CLICK_PULSES
      #define CLICK_PULSES	12

  // pin  layout:  ESP32-WROOM-32
  //               also for heltec WIFI_Kit_32 (with OLED) https://robotzero.one/heltec-wifi-kit-32/
    gpio_pin_t click_pin[CLICK_PULSES] =
      {23, 5, 17, 16, 4, 2, 15,		32, 33, 27, 14, 13, };	//  ESP32_12_v0  12 clicks
  //                     ?               ?   ?       ?          // these pins need configuration FIXME: ################

/* layout ESP-WROOM-32	heltec WIFI_Kit_32 (OLED)
   GND 1,  2, ...	    6     |     7               12
   WIFI                      USB  |  WIFI		    USB  |
  */

  // KALIMBA ESPxx versions:
  #elif defined KALIMBA7_v2		// KALIMBA version 2 on ESP32
    #undef CLICK_PULSES
      #define CLICK_PULSES	8	// 7 used for kalimba magnets

    // ESP32s module 2x19 pins
    gpio_pin_t click_pin[CLICK_PULSES] = { 32, 33, 14, 13, 23, 5, 17, 16};	//  ESP32  KALIMBA7_v2  8 clicks, 7 used

  #elif defined KALIMBA7_v1	// KALIMBA version 1 on ESP8266
    #undef CLICK_PULSES
    #define CLICK_PULSES	7	// number 8 unused, so far

    gpio_pin_t click_pin[CLICK_PULSES] = {5,  4,  0,  2,  14, 13, 15 };	// ESP8266  KALIMBA7_v1  7 clicks, 3rd try
//  gpio_pin_t click_pin[CLICK_PULSES] = {D1, D2, D3, D4, D5, D7, D8 };	// 7 clicks, 3rd try

  #elif defined GUITAR_v0
    #undef CLICK_PULSES
    #define CLICK_PULSES	16	// fake for DAC output ;)

    // ESP32s module 2x19 pins
    gpio_pin_t click_pin[CLICK_PULSES] = { 32, 33, 14, 13, 23, 5, 17, 16};	//  GUITAR_v0 == KALIMBA7_v2  8 clicks

  #elif defined ESP32	// DEFAULT on ESP32
    #define ESP32_15_clicks_no_display	// why not?

    #undef CLICK_PULSES
    #define CLICK_PULSES	15
    gpio_pin_t click_pin[CLICK_PULSES] =				//  ESP32_15_clicks_no_display	15 clicks
      {0, 2, 4, 5, 13, 14, 15, 16, 17, 23, 27, 32, 33, 18, 19, };

  #elif defined ESP8266	// configure PINs on ESP8266	general case
   /*
     // on NodeMCU ESP8266 board Arduino defines digital pins
     static const gpio_pin_t D0   = 16;
     static const gpio_pin_t D1   = 5;
     static const gpio_pin_t D2   = 4;
     static const gpio_pin_t D3   = 0;
     static const gpio_pin_t D4   = 2;
     static const gpio_pin_t D5   = 14;
     static const gpio_pin_t D6   = 12;
     static const gpio_pin_t D7   = 13;
     static const gpio_pin_t D8   = 15;
     static const gpio_pin_t D9   = 3;
     static const gpio_pin_t D10  = 1;
   */

   gpio_pin_t click_pin[CLICK_PULSES]={D1, D2, D3, D4, D5, D6, D7, D8};		// 8 clicks

   /* 8 clicks, using D1 to D8:
   // click_pin[0] = D1;	// D1 = 5
   // click_pin[1] = D2;	// D2 = 4
   // click_pin[2] = D3;	// D3 = 0
   // click_pin[3] = D4;	// D4 = 2	must be HIGH on boot	board blue led on LOW
   // click_pin[4] = D5;	// D5 = 14
   // click_pin[5] = D6;	// D6 = 12
   // click_pin[6] = D7;	// D7 = 13	LED?
   // click_pin[7] = D8;	// D8 = 15
   */

  #elif defined ESP32	// configure PINs on ESP32	general case
    #undef CLICK_PULSES
    #define CLICK_PULSES	12

  /* using ESP32_12 as default for now */
  // pin  layout:  ESP32-WROOM-32
  //               also for heltec WIFI_Kit_32 (with OLED) https://robotzero.one/heltec-wifi-kit-32/
    gpio_pin_t click_pin[CLICK_PULSES] =
      {23, 5, 17, 16, 4, 2, 15,		32, 33, 27, 14, 13, };	//  ESP32_12_v0  12 clicks
  /* layout ESP-WROOM-32	heltec WIFI_Kit_32 (OLED)
   GND 1,  2, ...	    6     |     7               12
   WIFI                      USB  |  WIFI		    USB  |
  */

  // ################	FIXME: do not use GPIO12 as click pin	################
  /*
    using GPIO12 as click pin gives error
    rst:0x10 (RTCWDT_RTC_RESET),boot:0x33 (SPI_FAST_FLASH_BOOT)
    flash read err, 1000
  */

  // check this list for more pins you do not need otherwise
  // gpio_pin_t click_pin[CLICK_PULSES] =
  //   {5, 4, /* 0,*/ 2, ,14, /* 12 */ 13, 15, /* 25, 26, */ 27, 23, /* 22, 21, *//* 19, 18, */ 17, 16,   32, 33, }; // 12 (to 20) clicks
  //   {D1 D2  D3?    D4  D5     D6    D7  D8                            i2c
  //                  LED      error               AUDIO               acc/gyro      e-paper

  #endif	// done with ESPxx versions now

#elif defined(__SAM3X8E__)			// Arduino DUE
  gpio_pin_t click_pin[CLICK_PULSES] =		// 32 clicks ;)
    {22, 23, 24, 25, 26, 27, 28, 29,
     30, 31, 32, 33, 34, 35, 36, 37,
     38, 39, 40, 41, 42, 43, 44, 45,
     46, 47, 48, 49, 50, 51, 52, 53};		// 32 clicks

#elif defined(__AVR_ATmega2560__)		// Mega2560
  gpio_pin_t click_pin[CLICK_PULSES] =		// 16 clicks
    {38, 39, 40, 41, 42, 43, 44, 45,
     46, 47, 48, 49, 50, 51, 52, 53};		// 16 clicks

#else // other boards, default:
  gpio_pin_t click_pin[CLICK_PULSES] =		// 16 clicks
    {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17};

#endif // which board?


#endif // CLICK_PULSES
#endif // CLICK_PIN_CONFIGURATION.H
