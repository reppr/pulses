/* pulses_CLICK_PIN_configuration.h	*/

#ifndef CLICK_PIN_CONFIGURATION.H
#define CLICK_PIN_CONFIGURATION.H

#if defined(CLICK_PULSES)

#if defined(ESP32)				// which board?
/*
  uint8_t click_pin[CLICK_PULSES] =
    {26, 27, 14, 12, 13, 21, 19, 18, 5, 17, 16, 4, 0, 15};	// 16 clicks
 */

//  uint8_t click_pin[CLICK_PULSES] =
//    {26, 27, 14, 12, 13, 23, 22, 21, 19, 18, 5, 17, 16, 4, 0, 15};	// 16 clicks

// ################ FIXME: more click pins on ESP32 ################
  uint8_t click_pin[CLICK_PULSES] =
    {26, 27, 14, 12, 13, 23, 22, 21};	// 8 clicks, first try
//    {26, 27, 14, 12, 13, 23, 22, 21, 19, 18, 5, 17, 16, 4, 0, 15};	// 16 clicks

//	// FIXME: test only ################
//	    #undef CLICK_PULSES		// FIXME: test only ################
//	    #define CLICK_PULSES	8	// test like ESP8266 ################
//	//uint8_t click_pin[CLICK_PULSES] =		// FIXME: test only ################
//	//   {26, 27, 14, 12, 13, 23, 22, 21};		// 8 lower clicks		// FIXME: test only ################
//	  uint8_t click_pin[CLICK_PULSES] =		// FIXME: test only 8 ################
//	    {19, 18, 5, 17, 16, 4, 0, 15};			// FIXME: test only 8 higher ################

#elif defined(ESP8266)	// configure PINs on ESP8266
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

#elif defined(__SAM3X8E__)			// Arduino DUE
  uint8_t click_pin[CLICK_PULSES] =		// 32 clicks ;)
    {22, 23, 24, 25, 26, 27, 28, 29,
     30, 31, 32, 33, 34, 35, 36, 37,
     38, 39, 40, 41, 42, 43, 44, 45.
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
