/* pulses_CLICK_PIN_configuration.h	*/

#ifndef CLICK_PIN_CONFIGURATION.H
#define CLICK_PIN_CONFIGURATION.H

#if defined(CLICK_PULSES)

// which board?
#if defined ESP32 || defined ESP8266			// ESPxx boards

// KALIMBA ESPxx versions:
  #ifdef KALIMBA7_v2		// KALIMBA version 2 on ESP32
    #undef CLICK_PULSES
      #define CLICK_PULSES	8	// 7 used for kalimba magnets

    // ESP32s module 2x19 pins
    uint8_t click_pin[CLICK_PULSES] = { 32, 33, 14, 13, 23, 5, 17, 16};	//  ESP32  KALIMBA7_v2  8 clicks, 7 used

  #elif defined KALIMBA7_v1	// KALIMBA version 1 on ESP8266
    #undef CLICK_PULSES
    #define CLICK_PULSES	7	// number 8 unused, so far

    uint8_t click_pin[CLICK_PULSES] = {5,  4,  0,  2,  14, 13, 15 };	// ESP8266  KALIMBA7_v1  7 clicks, 3rd try
//  uint8_t click_pin[CLICK_PULSES] = {D1, D2, D3, D4, D5, D7, D8 };	// 7 clicks, 3rd try

  #elif defined ESP8266	// configure PINs on ESP8266	general case
   /*
     // on NodeMCU ESP8266 board Arduino defines digital pins
     static const uint8_t D0   = 16;
     static const uint8_t D1   = 5;
     static const uint8_t D2   = 4;
     static const uint8_t D3   = 0;
     static const uint8_t D4   = 2;
     static const uint8_t D5   = 14;
     static const uint8_t D6   = 12;
     static const uint8_t D7   = 13;
     static const uint8_t D8   = 15;
     static const uint8_t D9   = 3;
     static const uint8_t D10  = 1;
   */

   uint8_t click_pin[CLICK_PULSES]={D1, D2, D3, D4, D5, D6, D7, D8};		// 8 clicks

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
    #define CLICK_PULSES	15
    // ################	FIXME: do not use GPIO12 as click pin	################
    /*
      using GPIO12 as click pin gives error
      rst:0x10 (RTCWDT_RTC_RESET),boot:0x33 (SPI_FAST_FLASH_BOOT)
      flash read err, 1000
    */

    // TODO: ################ FIXME: check pins on ESP32 ################
    /*
      uint8_t click_pin[CLICK_PULSES] =
      {26, 27, 14, 12, 13, 21, 19, 18, 5, 17, 16, 4, 0, 15};	// 16 clicks
    */

   uint8_t click_pin[CLICK_PULSES] =
     {26, 27, 14, 13, 23, 22, 21, 19, 18, 5, 17, 16, 4, 0, 15};	// 15 clicks

   #endif
// done with ESPxx versions

#elif defined(__SAM3X8E__)			// Arduino DUE
  uint8_t click_pin[CLICK_PULSES] =		// 32 clicks ;)
    {22, 23, 24, 25, 26, 27, 28, 29,
     30, 31, 32, 33, 34, 35, 36, 37,
     38, 39, 40, 41, 42, 43, 44, 45,
     46, 47, 48, 49, 50, 51, 52, 53};		// 32 clicks

#elif defined(__AVR_ATmega2560__)		// Mega2560
  uint8_t click_pin[CLICK_PULSES] =		// 16 clicks
    {38, 39, 40, 41, 42, 43, 44, 45,
     46, 47, 48, 49, 50, 51, 52, 53};		// 16 clicks

#else // other boards, default:
  uint8_t click_pin[CLICK_PULSES] =		// 16 clicks
    {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17};

#endif // which board?


#endif // CLICK_PULSES
#endif // CLICK_PIN_CONFIGURATION.H
