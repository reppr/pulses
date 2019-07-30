/*
  pulses_RGB_LED_string.h

  see: https://github.com/MartyMacGyver/ESP32-Digital-RGB-LED-Drivers
*/

#if ! defined PULSES_RGB_LED_STRING_H

#include "../FOREIGN/ESP32-Digital-RGB-LED-Drivers/src/esp32_digital_led_lib.h"
#include "../FOREIGN/ESP32-Digital-RGB-LED-Drivers/src/esp32_digital_led_lib.cpp"

#include "../FOREIGN/ESP32-Digital-RGB-LED-Drivers/src/esp32_digital_led_funcs.h"
#include "../FOREIGN/ESP32-Digital-RGB-LED-Drivers/src/esp32_digital_led_funcs.cpp"

#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

strand_t STRANDS[] = { // Avoid using any of the strapping pins on the ESP32, anything >=32, 16, 17... not much left.
  {.rmtChannel = 0, .gpioNum = 14, .ledType = LED_WS2812B_V3, .brightLimit = 24, .numPixels = RGB_STRING_LED_CNT },
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
    MENU.out("Init rc = ");
    MENU.outln(rc);
    return false;
  }

  for (int i = 0; i < STRANDCNT; i++) {
    strand_t * pStrand = strands[i];
    MENU.out("Strand ");
    MENU.out(i);
    MENU.out(" = ");
    MENU.out((uint32_t)(pStrand->pixels), HEX);
    MENU.outln();
    #if DEBUG_ESP32_DIGITAL_LED_LIB
      dumpDebugBuffer(-2, digitalLeds_debugBuffer);
    #endif
  }

  return true;
}


// see: https://de.wikipedia.org/wiki/HSV-Farbraum
void HSV_2_RGB_degree(pixelColor_t* pixel, float H, float S, float V) {	// TODO: test
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
  // parameters within range now	TODO: test

  int h_i = H/60;		// (degree)
  float f = H/60 - h_i;		// (degree)
  float p = V * (1 - S);
  float q = V * (1 - S*f);
  float t = V * (1 - S*(1 - f));

  // double check h_i range
  if(h_i < 0 || h_i > 6) {	// catch programming errors ;)
    MENU.out(h_i);
    MENU.tab();
    MENU.error_ln(F("HSV_2_RGB_degree()\th_i"));
  }

  switch(h_i) {
  case 0:
  case 6:
    pixel->r = V;
    pixel->g = t;
    pixel->b = p;
    break;
  case 1:
    pixel->r = q;
    pixel->g = V;
    pixel->b = p;
    break;
  case 2:
    pixel->r = p;
    pixel->g = V;
    pixel->b = t;
    break;
  case 3:
    pixel->r = p;
    pixel->g = q;
    pixel->b = V;
    break;
  case 4:
    pixel->r = t;
    pixel->g = p;
    pixel->b = V;
    break;
  case 5:
    pixel->r = V;
    pixel->g = p;
    pixel->b = q;
    break;
  }

  // check range:
  if(pixel->r<0 || pixel->r>1) {
    MENU.out(pixel->r);
    MENU.tab();
    MENU.error_ln(F("HSV_2_RGB_degree()\tR"));
  }
  if(pixel->g<0 || pixel->g>1) {
    MENU.out(pixel->g);
    MENU.tab();
    MENU.error_ln(F("HSV_2_RGB_degree()\tG"));
  }
  if((*pixel).b<0 || pixel->b>1) {
    MENU.out(pixel->b);
    MENU.tab();
    MENU.error_ln(F("HSV_2_RGB_degree()\tB"));
  }
}


// TODO: remove, just a test	################
void dumpSysInfo() {
  esp_chip_info_t sysinfo;
  esp_chip_info(&sysinfo);
  MENU.out("Model: ");
  MENU.out((int)sysinfo.model);
  MENU.out("; Features: 0x");
  MENU.out((int)sysinfo.features, HEX);
  MENU.out("; Cores: ");
  MENU.out((int)sysinfo.cores);
  MENU.out("; Revision: r");
  MENU.outln((int)sysinfo.revision);
}

bool pulses_RGB_LED_string_init() {
  initStrands();
  digitalLeds_resetPixels(&strands[0], 1);
  dumpSysInfo();
}

void random_RGB_string() {
  strand_t * strand = strands[0];
  for(int i=0; i<RGB_STRING_LED_CNT; i++)
    strand->pixels[i] = pixelFromRGB(random(255), random(255), random(255));

  digitalLeds_drawPixels(strands, 1);
}

#define PULSES_RGB_LED_STRING_H
#endif
