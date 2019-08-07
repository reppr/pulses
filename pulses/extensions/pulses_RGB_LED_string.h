/*
  pulses_RGB_LED_string.h

  see: https://github.com/MartyMacGyver/ESP32-Digital-RGB-LED-Drivers
*/

#if ! defined PULSES_RGB_LED_STRING_H

//#define DEBUG_LED_STRINGS			// empty  or 'SATURATION'
//#define DEBUG_LED_STRINGS	SATURATION	// empty  or 'SATURATION'

#if ! defined DEFAULT_LED_STRING_INTENSITY
  #define DEFAULT_LED_STRING_INTENSITY	48	// 72 on 'placeholder'	DADA
#endif


typedef struct rgb_string_config_t {
  //short hue_slice_cnt = 8;	// just a usable default	// TODO: UI
  short hue_slice_cnt = 12;	// just another usable default	// TODO: UI	DADA

//float saturation_start_value = 0.12;
//float saturation_start_value = 0.3;
  float saturation_start_value = 0.2;	// TODO: test&trimm default value ################	UI
  float saturation = saturation_start_value;

  float saturation_change_factor = 1.004;	// TODO: UI
  float saturation_reset_value = 0.35;

  float BlueHack_factor = 2.0;	// HACK: increase blueness

  uint8_t pixel_cnt=150;
  uint8_t rgb_led_string_intensity = DEFAULT_LED_STRING_INTENSITY;
  uint8_t version = 0;	// 0 means currently under development

  bool clear_rgb_background_on_ending = true;	// TODO: ################

} rgb_string_config_t;

rgb_string_config_t RGBstringConf;


#include "FOREIGN/ESP32-Digital-RGB-LED-Drivers/src/esp32_digital_led_lib.h"
#include "FOREIGN/ESP32-Digital-RGB-LED-Drivers/src/esp32_digital_led_lib.cpp"

#include "FOREIGN/ESP32-Digital-RGB-LED-Drivers/src/esp32_digital_led_funcs.h"
#include "FOREIGN/ESP32-Digital-RGB-LED-Drivers/src/esp32_digital_led_funcs.cpp"

#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

strand_t STRANDS[] = { // Avoid using any of the strapping pins on the ESP32, anything >=32, 16, 17... not much left.
  {.rmtChannel = 0, .gpioNum = RGB_LED_STRIP_DATA_PIN, .ledType = LED_WS2812B_V3, .brightLimit = 24, .numPixels = RGB_STRING_LED_CNT },
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
  #if DEBUG_LED_STRINGS != SATURATION
    MENU.out(F("H= "));
    MENU.out(H);
    MENU.out(F("\tS= "));
    MENU.out(S);
    MENU.out(F("\tV= "));
    MENU.outln(V);
  #else	//    DEBUG_LED_STRINGS == SATURATION
    MENU.out(F("\tS= "));
    MENU.outln(S);
  #endif
#endif
  // parameters within range now	TODO: test

  int h_i = H/60;		// (degree)
  float f = H/60 - h_i;		// (degree)
  float p = V * (1 - S);
  float q = V * (1 - S*f);
  float t = V * (1 - S*(1 - f));

#if defined DEBUG_LED_STRINGS
  #if DEBUG_LED_STRINGS != SATURATION
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
#endif

  uint8_t rgb_led_string_intensity = RGBstringConf.rgb_led_string_intensity;
  switch(h_i) {
  case 0:
  case 6:
    pixel->r = V * rgb_led_string_intensity;
    pixel->g = t * rgb_led_string_intensity;
    pixel->b = p * rgb_led_string_intensity;
    break;
  case 1:
    pixel->r = q * rgb_led_string_intensity;
    pixel->g = V * rgb_led_string_intensity;
    pixel->b = p * rgb_led_string_intensity;
    break;
  case 2:
    pixel->r = p * rgb_led_string_intensity;
    pixel->g = V * rgb_led_string_intensity;
    pixel->b = t * rgb_led_string_intensity;
    break;
  case 3:
    pixel->r = p * rgb_led_string_intensity;
    pixel->g = q * rgb_led_string_intensity;
    pixel->b = V * rgb_led_string_intensity;
    break;
  case 4:
    pixel->r = t * rgb_led_string_intensity;
    pixel->g = p * rgb_led_string_intensity;
    pixel->b = V * rgb_led_string_intensity;
    break;
  case 5:
    pixel->r = V * rgb_led_string_intensity;
    pixel->g = p * rgb_led_string_intensity;
    pixel->b = q * rgb_led_string_intensity;
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
}

void random_RGB_string(uint8_t max=8) {
#if defined DEBUG_LED_STRINGS
  #if DEBUG_LED_STRINGS != SATURATION
    MENU.outln(F("random_RGB_string()"));
  #endif
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
  #if DEBUG_LED_STRINGS != SATURATION
    inspect_LED_pixel(strand_p->pixels[i]);
  #endif
#endif
  }

  digitalLeds_drawPixels(strands, 1);
} // random_RGB_string(uint8_t max=8)

void random_HSV_LED_string() {
#if DEBUG_LED_STRINGS == true
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

#if DEBUG_LED_STRINGS == true
    MENU.out(i);
    MENU.tab();
    MENU.out(H);
    MENU.tab();
    MENU.out(S);
    MENU.tab();
    MENU.outln(V);
#endif

    HSV_2_RGB_degree(&pixel, H, S, V);
    strand_p->pixels[i] = pixel;

#if DEBUG_LED_STRINGS == true
    inspect_LED_pixel(pixel);
#endif
  }

  digitalLeds_drawPixels(strands, 1);
} // random_HSV_LED_string()


bool update_RGB_LED_string=false;	// is the string buffer dirty?

#define PULSE_2_RGB_LED_STRING	  pulse - lowest_primary	// find the corresponding led sting

void clear_RGB_string_pixel(int pulse) {
  strand_t * strand_p = strands[0];
  int pix_i = PULSE_2_RGB_LED_STRING;		// TODO: use pulse intenal data
  pixelColor_t pixel;
  pixel.r = 0;
  pixel.g = 0;
  pixel.b = 0;
  strand_p->pixels[pix_i] = pixel;

  update_RGB_LED_string = true;
}


void set_pulse_LED_pixel_from_counter(int pulse) {
  float H, V;

  bool do_display = MENU.maybe_display_more(VERBOSITY_SOME);
#if defined DEBUG_LED_STRINGS
  do_display = true;
#endif

  RGBstringConf.saturation *= RGBstringConf.saturation_change_factor;	// slowly increase saturation ;)
  if(RGBstringConf.saturation > 1.0) {			// sudden reset on overflow to a low saturation level
    RGBstringConf.saturation = RGBstringConf.saturation_reset_value;
    if(do_display) {
      MENU.out(F("RESET saturation to "));
      MENU.outln(RGBstringConf.saturation);
    }
  }

  H = (float) (PULSES.pulses[pulse].counter % RGBstringConf.hue_slice_cnt) / (float) RGBstringConf.hue_slice_cnt;
  V = (float) RGBstringConf.rgb_led_string_intensity / (float) 255;

  strand_t * strand_p = strands[0];
  int pix_i = PULSE_2_RGB_LED_STRING;		// TODO: use pulse intenal data
  pixelColor_t pixel;

  HSV_2_RGB_degree(&pixel, (H * 360.0), RGBstringConf.saturation, V);
  strand_p->pixels[pix_i] = pixel;

  // try the BlueHack, to give *more* blue
  int blue = (int) (((float) strand_p->pixels[pix_i].b * RGBstringConf.BlueHack_factor) + 0.5);
  if(blue > 255)
    blue = 255;
  strand_p->pixels[pix_i].b = blue;

  update_RGB_LED_string=true;		// new buffer content to be displayed
} // set_pulse_LED_pixel_from_counter(int pulse)


void rgb_led_reset_to_default() {	// reset rgb led strip management to default conditions
  RGBstringConf.saturation = RGBstringConf.saturation_start_value;
}

float rgb_background_dim = 0.45;	// ok for 5V version 1m 144
//float rgb_background_dim = 0.1;		// TEST: for "12V" version 5m 300	DADA placeholder

void set_rgb_led_background(int pulse) {
  if(PULSES.pulses[pulse].flags & HAS_RGB_LEDs) {
    strand_t * strand_p = strands[PULSES.pulses[pulse].rgb_string_idx];

//    pixelColor_t pixel;
//    float H, S, V;
//    H = 0.0;		// TODO: this version just switches OFF
//    S = 0.0;
//    V = 0.0;
//    HSV_2_RGB_degree(&pixel, (H * 360.0), saturation, V);
//    strand_p->pixels[PULSES.pulses[pulse].rgb_pixel_idx] = pixel;

    bool clear=false;
#if defined HARMONICAL_MUSIC_BOX && defined USE_RGB_LED_STRIP
    // bool clear_rgb_background_on_ending	// TODO: maybe	// DADA rgb_string_config
    // extern bool musicbox_is_ending();
    // DADA
    // clear = musicbox_is_ending();
#endif

    float x = strand_p->pixels[PULSES.pulses[pulse].rgb_pixel_idx].r;
    x *= rgb_background_dim;
    strand_p->pixels[PULSES.pulses[pulse].rgb_pixel_idx].r = (x + 0.5);
    if(clear)
      strand_p->pixels[PULSES.pulses[pulse].rgb_pixel_idx].r = 0;

    x = strand_p->pixels[PULSES.pulses[pulse].rgb_pixel_idx].g;
    x *= rgb_background_dim;
    strand_p->pixels[PULSES.pulses[pulse].rgb_pixel_idx].g = (x + 0.5);
    if(clear)
      strand_p->pixels[PULSES.pulses[pulse].rgb_pixel_idx].g = 0;

    x = strand_p->pixels[PULSES.pulses[pulse].rgb_pixel_idx].b;
    x *= rgb_background_dim;
    strand_p->pixels[PULSES.pulses[pulse].rgb_pixel_idx].b = (x + 0.5);
    if(clear)
      strand_p->pixels[PULSES.pulses[pulse].rgb_pixel_idx].b = 0;

    update_RGB_LED_string=true;		// new buffer content to be displayed
  } else
    MENU.error_ln(F("no RGB LED"));
}

#define PULSES_RGB_LED_STRING_H
#endif
