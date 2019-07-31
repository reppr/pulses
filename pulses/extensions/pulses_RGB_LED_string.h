/*
  pulses_RGB_LED_string.h

  see: https://github.com/MartyMacGyver/ESP32-Digital-RGB-LED-Drivers
*/

#if ! defined PULSES_RGB_LED_STRING_H

#define DEBUG_LED_STRINGS

#if ! defined MAX_LED_STRING_INTENSITY
  #define MAX_LED_STRING_INTENSITY	32
#endif

#include "../FOREIGN/ESP32-Digital-RGB-LED-Drivers/src/esp32_digital_led_lib.h"
#include "../FOREIGN/ESP32-Digital-RGB-LED-Drivers/src/esp32_digital_led_lib.cpp"

#include "../FOREIGN/ESP32-Digital-RGB-LED-Drivers/src/esp32_digital_led_funcs.h"
#include "../FOREIGN/ESP32-Digital-RGB-LED-Drivers/src/esp32_digital_led_funcs.cpp"

#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

strand_t STRANDS[] = { // Avoid using any of the strapping pins on the ESP32, anything >=32, 16, 17... not much left.
  {.rmtChannel = 0, .gpioNum = 14, .ledType = LED_WS2812B_V3, .brightLimit = 24, .numPixels = RGB_STRING_LED_CNT },
//  {.rmtChannel = 0, .gpioNum = 14, .ledType = LED_WS2812B_V2, .brightLimit = 24, .numPixels = RGB_STRING_LED_CNT },
//  {.rmtChannel = 0, .gpioNum = 14, .ledType = LED_WS2812B_V1, .brightLimit = 24, .numPixels = RGB_STRING_LED_CNT },
};

int STRANDCNT = COUNT_OF(STRANDS);	// I use only one

strand_t * strands[8];

//**************************************************************************//
boolean initStrands()
{
  /****************************************************************************
     If you have multiple strands connected, but not all are in use, the
     GPIO power-on defaults for the unused strand data lines will typically be
     high-impedance. Unless you are pulling the data lines high or low via a
     resistor, this will lead to noise on those unused but connected channels
     and unwanted driving of those unallocated strands.
     This optional gpioSetup() code helps avoid that problem programmatically.
  ****************************************************************************/

  digitalLeds_initDriver();

  for (int i = 0; i < STRANDCNT; i++) {
    gpioSetup(STRANDS[i].gpioNum, OUTPUT, LOW);
  }

  // strand_t * strands[8];
  for (int i = 0; i < STRANDCNT; i++) {
    strands[i] = &STRANDS[i];
  }
  int rc = digitalLeds_addStrands(strands, STRANDCNT);
  if (rc) {
//	    MENU.out("Init rc = ");
//	    MENU.outln(rc);
    return false;
  }

//	  for (int i = 0; i < STRANDCNT; i++) {
//	    strand_t * pStrand = strands[i];
//	    MENU.out("Strand ");
//	    MENU.out(i);
//	    MENU.out(" = ");
//	    MENU.out((uint32_t)(pStrand->pixels), HEX);
//	    MENU.outln();
//	    #if DEBUG_ESP32_DIGITAL_LED_LIB
//	      dumpDebugBuffer(-2, digitalLeds_debugBuffer);
//	    #endif
//	  }

  return true;
}

void inspect_LED_pixel(pixelColor_t pixel) {
  MENU.out(F("pixel.r = "));
  MENU.out(pixel.r);
  MENU.out(F("\tg = "));
  MENU.out(pixel.g);
  MENU.out(F("\tb = "));
  MENU.outln(pixel.b);
}

// see: https://de.wikipedia.org/wiki/HSV-Farbraum
void HSV_2_RGB_degree(pixelColor_t* pixel, float H, float S, float V) {	// TODO: test
#if defined DEBUG_LED_STRINGS
  MENU.out(F("HSV_2_RGB_degree() "));
#endif
  // assure parameter range
  int i;
  if(H != 360.0) {
    i = H / 360;	// (degree)
    H -= i*360;		// 0...360
  }
  if(S != 1.0) {
    i = S;
    S -= i;	// 0...1
  }
  if(V != 1.0) {
    i = V;
    V -= i;	// 0...1
  }

#if defined DEBUG_LED_STRINGS
  MENU.out(H);
  MENU.tab();
  MENU.out(S);
  MENU.tab();
  MENU.outln(V);
#endif
  // parameters within range now	TODO: test

  int h_i = H/60;		// (degree)
  float f = H/60 - h_i;		// (degree)
  float p = V * (1 - S);
  float q = V * (1 - S*f);
  float t = V * (1 - S*(1 - f));

#if defined DEBUG_LED_STRINGS
  MENU.out(F("f = "));
  MENU.out(f);
  MENU.out(F("\tp = "));
  MENU.out(p);
  MENU.out(F("\tq = "));
  MENU.out(q);
  MENU.out(F("\tt = "));
  MENU.outln(t);

  // double check h_i range
  MENU.out(F("h_i = "));
  MENU.out(h_i);
  if(h_i < 0 || h_i > 6) {	// catch programming errors ;)
    MENU.tab();
    MENU.error_ln(F("HSV_2_RGB_degree()\th_i"));
  } else
    MENU.ln();
#endif

  switch(h_i) {
  case 0:
  case 6:
    pixel->r = V * MAX_LED_STRING_INTENSITY;
    pixel->g = t * MAX_LED_STRING_INTENSITY;
    pixel->b = p * MAX_LED_STRING_INTENSITY;
    break;
  case 1:
    pixel->r = q * MAX_LED_STRING_INTENSITY;
    pixel->g = V * MAX_LED_STRING_INTENSITY;
    pixel->b = p * MAX_LED_STRING_INTENSITY;
    break;
  case 2:
    pixel->r = p * MAX_LED_STRING_INTENSITY;
    pixel->g = V * MAX_LED_STRING_INTENSITY;
    pixel->b = t * MAX_LED_STRING_INTENSITY;
    break;
  case 3:
    pixel->r = p * MAX_LED_STRING_INTENSITY;
    pixel->g = q * MAX_LED_STRING_INTENSITY;
    pixel->b = V * MAX_LED_STRING_INTENSITY;
    break;
  case 4:
    pixel->r = t * MAX_LED_STRING_INTENSITY;
    pixel->g = p * MAX_LED_STRING_INTENSITY;
    pixel->b = V * MAX_LED_STRING_INTENSITY;
    break;
  case 5:
    pixel->r = V * MAX_LED_STRING_INTENSITY;
    pixel->g = p * MAX_LED_STRING_INTENSITY;
    pixel->b = q * MAX_LED_STRING_INTENSITY;
    break;
  }

  // check range:
//	  if(pixel->r<0 || pixel->r>1) {
//	    MENU.out(pixel->r);
//	    MENU.tab();
//	    MENU.error_ln(F("HSV_2_RGB_degree()\tR"));
//	  }
//	  if(pixel->g<0 || pixel->g>1) {
//	    MENU.out(pixel->g);
//	    MENU.tab();
//	    MENU.error_ln(F("HSV_2_RGB_degree()\tG"));
//	  }
//	  if((*pixel).b<0 || pixel->b>1) {
//	    MENU.out(pixel->b);
//	    MENU.tab();
//	    MENU.error_ln(F("HSV_2_RGB_degree()\tB"));
//	  }
}


//	// TODO: remove, just a test	################
//	void dumpSysInfo() {
//	  esp_chip_info_t sysinfo;
//	  esp_chip_info(&sysinfo);
//	  MENU.out("Model: ");
//	  MENU.out((int)sysinfo.model);
//	  MENU.out("; Features: 0x");
//	  MENU.out((int)sysinfo.features, HEX);
//	  MENU.out("; Cores: ");
//	  MENU.out((int)sysinfo.cores);
//	  MENU.out("; Revision: r");
//	  MENU.outln((int)sysinfo.revision);
//	}

bool pulses_RGB_LED_string_init() {
  initStrands();
  digitalLeds_resetPixels(&strands[0], 1);
//	  dumpSysInfo();
}

//strand_t * pStrand = strands[8];

void random_RGB_string(uint8_t max=8) {
#if defined DEBUG_LED_STRINGS
  MENU.outln(F("random_RGB_string()"));
#endif
  strand_t * strand_p = strands[0];
  for(int i=0; i<RGB_STRING_LED_CNT; i++) {
    switch(i) {
      /*	// TODO: REMOVE: DEBUGGING: why do i always get 3 identical LEDs ???
    case 1:
      strand_p->pixels[i] = pixelFromRGB(0, 0, 0);
      break;
    case 3:
      strand_p->pixels[i] = pixelFromRGB(10, 10, 12);
      break;
    case 4:
      strand_p->pixels[i] = pixelFromRGB(10, 0, 0);
      break;
    case 5:
      strand_p->pixels[i] = pixelFromRGB(0, 10, 0);
      break;
    case 6:
      strand_p->pixels[i] = pixelFromRGB(0, 0, 10);
      break;
      */
    default:
      strand_p->pixels[i] = pixelFromRGB(random(max), random(max), random(max));
    }
#if defined DEBUG_LED_STRINGS
    inspect_LED_pixel(strand_p->pixels[i]);
#endif
    //    strands[0]->pixels[i] = (pixelColor_t) pixelFromRGB((uint8_t) random(max), (uint8_t) random(max), (uint8_t) random(max));
    //    strand->pixels[i] = (pixelColor_t) pixelFromRGB((uint8_t) random(max), (uint8_t) random(max), (uint8_t) random(max));
    //strand->pixels[i].r = (uint8_t) random(max);
    //strand->pixels[i].g = (uint8_t) random(max);
    //strand->pixels[i].b = (uint8_t) random(max);
  }

  digitalLeds_drawPixels(strands, 1);
}

void random_HSV_LED_string() {
#if defined DEBUG_LED_STRINGS
  MENU.outln(F("random_HSV_LED_string()"));
#endif

  strand_t * strand_p = strands[0];
  pixelColor_t pixel;
  int random_delta_i=1000000;	// just a random granularity
  float H, S, V;

  for(int i=0; i<RGB_STRING_LED_CNT; i++) {
    H = ((float) random(random_delta_i) / (float) random_delta_i) * 360.0;
    S = (float) random(random_delta_i) / (float) random_delta_i;
    V = (float) random(random_delta_i) / (float) random_delta_i;

    MENU.out(i);
    MENU.tab();
    MENU.out(H);
    MENU.tab();
    MENU.out(S);
    MENU.tab();
    MENU.outln(V);

    HSV_2_RGB_degree(&pixel, H, S, V);
    strand_p->pixels[i] = pixel;
#if defined DEBUG_LED_STRINGS
    inspect_LED_pixel(pixel);
    //    inspect_LED_pixel(strand_p->pixels[i]);
#endif
  }

  digitalLeds_drawPixels(strands, 1);
} // random_HSV_LED_string()


#define PULSES_RGB_LED_STRING_H
#endif
