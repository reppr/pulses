/*
  pulses_RGB_LED_string.h

  see: https://github.com/MartyMacGyver/ESP32-Digital-RGB-LED-Drivers
*/

#if ! defined PULSES_RGB_LED_STRING_H

//#define DEBUG_LED_STRINGS			// empty  or 'SATURATION'
//#define DEBUG_LED_STRINGS	SATURATION	// empty  or 'SATURATION'

// TODO: bool rgb_strings_available	MOVE: to HARDWARE
bool rgb_strings_available = true;	// default, can be switched off i.e. from nvs

// TODO: bool rgb_strings_active	MOVE: to RGBstringConf
bool rgb_strings_active = false;	// new default *INACTIVE* to avoid audible clicks
					//  programm can switch all rgb string activity ON|off

#if ! defined DEFAULT_LED_STRING_INTENSITY
  #define DEFAULT_LED_STRING_INTENSITY	48	// 72 on 'placeholder'	DADA
#endif

#if ! defined DEFAULT_LED_STRING_INTENSITY_12V
  #define DEFAULT_LED_STRING_INTENSITY_12V	192	// bright for festival opening ceremony ;)
#endif

enum background_algorithms {
  bgDIM = 1,	// dim by rgb_background_dim
  bgHISTORY_i,	// dim by rgb_background_dim, mix with (last bg * rgb_background_history_mix)	first INTEGER version
  bgHISTORY_f,	// looks like we will need that...
};

/* rgb_string_config_t	*software* configuration config for RGB LED strings
			for *hardware* see pulses_hardware_conf_t	*/
typedef struct rgb_string_config_t {
  float rgb_led_string_intensity = DEFAULT_LED_STRING_INTENSITY;
  float saturation_start_value = 0.2;	// TODO: test&trimm default value ################	UI
  float saturation = saturation_start_value;

  float saturation_change_factor = 1.004;	// TODO: UI
  float saturation_reset_value = 0.35;
  float rgb_background_dim = 0.45;	// ok for 5V version 1m 144
  //float rgb_background_dim = 0.1;	// TEST: for "12V" version 5m 300
  float rgb_background_history_mix = 0.66;	// how much history included in bg color

//float BlueHack_factor = 2.0;	// HACK: increase blueness
  float BlueHack_factor = 1.4;	// HACK: increase blueness

  uint8_t hue_slice_cnt = 15;	// just a usable default  see: set_automagic_hue_slices
  uint8_t set_background_algorithm = bgDIM;
  //uint8_t set_background_algorithm = bgHISTORY_i;	// TODO: DADA broken
  // see: pulses_hardware_conf_t  uint8_t rgb_pattern0

  uint8_t version = 0;	// 0 means currently under development

#if defined HIGH_PRIORITY_RGB_LED_UPDATE
  bool rgb_leds_high_priority = true;		// run time toggle with menu 'LH'
#else
  bool rgb_leds_high_priority = false;		// run time toggle with menu 'LH'
#endif

  bool rgb_strings_active = false;		// defaults to INACTIVE to avoid clicks

  bool clear_rgb_background_on_ending = true;	// TODO: ################
  bool set_automagic_hue_slices = true;		// if *not* set by user

} rgb_string_config_t;

rgb_string_config_t RGBstringConf;


void set_rgb_string_voltage_type(int voltage, int string) {
  HARDWARE.rgb_led_voltage_type[string] = voltage;	// voltage goes to *HARDWARE*

  if(voltage < 6) {
    // RGBstringConf.rgb_background_dim = 0.45;	// ok for 5V version (1m 144)
    RGBstringConf.rgb_background_dim = 0.08;	// TEST
    RGBstringConf.rgb_led_string_intensity = DEFAULT_LED_STRING_INTENSITY;
  } else {	// 6V and more "12V" type
    RGBstringConf.rgb_background_dim = 0.1;	// TEST: for "12V" version 5m 300
    RGBstringConf.rgb_led_string_intensity = DEFAULT_LED_STRING_INTENSITY_12V;
  }
  MENU.out(F("configured voltage adaptions on string "));
  MENU.out(string);
  MENU.out(F("  for voltage "));
  MENU.out(voltage);
  MENU.outln('V');
}


#include "FOREIGN/ESP32-Digital-RGB-LED-Drivers/src/esp32_digital_led_lib.h"
#include "FOREIGN/ESP32-Digital-RGB-LED-Drivers/src/esp32_digital_led_lib.cpp"

#include "FOREIGN/ESP32-Digital-RGB-LED-Drivers/src/esp32_digital_led_funcs.h"
#include "FOREIGN/ESP32-Digital-RGB-LED-Drivers/src/esp32_digital_led_funcs.cpp"

#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

int selected_rgb_LED_string = 0;	// default to the first string

strand_t STRANDS[] = { // Avoid using any of the strapping pins on the ESP32, anything >=32, 16, 17... not much left.
  {.rmtChannel = 0, .gpioNum = HARDWARE.rgb_pin[0], .ledType = LED_WS2812B_V3, .brightLimit = 24, .numPixels = RGB_STRING_LED_CNT },
//  {.rmtChannel = 0, .gpioNum = 14, .ledType = LED_WS2812B_V2, .brightLimit = 24, .numPixels = RGB_STRING_LED_CNT },
//  {.rmtChannel = 0, .gpioNum = 14, .ledType = LED_WS2812B_V1, .brightLimit = 24, .numPixels = RGB_STRING_LED_CNT },
};

int STRANDCNT = COUNT_OF(STRANDS);	// I use only one

strand_t * strands[RGB_STRINGS_MAX];

//**************************************************************************//
boolean initStrands()
{
  if(!rgb_strings_available)
    return true;

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
    gpioSetup(HARDWARE.rgb_pin[i], OUTPUT, LOW);
  }

  // strand_t * strands[RGB_STRINGS_MAX];
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


// background0[]  buffer for background colour history
pixelColor_t background0[RGB_STRING_LED_CNT] = {0};	// keeps last used background colour, dimmed

void clear_background_buffer0() {
  if(RGBstringConf.rgb_strings_active && rgb_strings_available) {
    for(int i=0; i < RGB_STRING_LED_CNT; i++)
      background0[i].r = background0[i].g = background0[i].b = 0;
  }
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
      ERROR_ln(F("HSV_2_RGB_degree()\th_i"));
    } else
      MENU.ln();
  #endif
#endif

  float rgb_led_string_intensity = RGBstringConf.rgb_led_string_intensity;
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
//	    ERROR_ln(F("HSV_2_RGB_degree()\tR"));
//	  }
//	  if(pixel->g<0 || pixel->g>1) {
//	    MENU.out(pixel->g);
//	    MENU.tab();
//	    ERROR_ln(F("HSV_2_RGB_degree()\tG"));
//	  }
//	  if((*pixel).b<0 || pixel->b>1) {
//	    MENU.out(pixel->b);
//	    MENU.tab();
//	    ERROR_ln(F("HSV_2_RGB_degree()\tB"));
//	  }
} // HSV_2_RGB_degree()

void pulses_RGB_LED_string_init() {
  if(rgb_strings_available) {
    initStrands();
    digitalLeds_resetPixels(&strands[0], 1);	// DADA	TODO: multiple RGB strings
  }
}

void random_RGB_string(uint8_t max=8) {
#if defined DEBUG_LED_STRINGS
  #if DEBUG_LED_STRINGS != SATURATION
    MENU.outln(F("random_RGB_string()"));
  #endif
#endif

  strand_t * strand_p = strands[0];	// DADA	TODO: multiple RGB strings
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

  strand_t * strand_p = strands[0];	// DADA	TODO: multiple RGB strings
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


int pulse_2_rgb_pixel(int pulse) {
  int pixel = pulse;
  pixel -= musicBoxConf.lowest_primary;
  pixel += HARDWARE.rgb_pattern0[0];	// DADA	TODO: multiple RGB strings
  return pixel;
}

void clear_RGB_string_pixel(int pulse) {
  if(RGBstringConf.rgb_strings_active && rgb_strings_available) {
    strand_t * strand_p = strands[selected_rgb_LED_string];		// DADA	TODO: multiple RGB strings
    int pix_i = pulse_2_rgb_pixel(pulse);	// TODO: use pulse internal data
    pixelColor_t pixel;
    pixel.r = 0;
    pixel.g = 0;
    pixel.b = 0;
    strand_p->pixels[pix_i] = pixel;

    update_RGB_LED_string = true;
  }
}


void set_pulse_LED_pixel_from_counter(int pulse) {
  if(RGBstringConf.rgb_strings_active && rgb_strings_available) {
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
    V = RGBstringConf.rgb_led_string_intensity / (float) 255;

    strand_t * strand_p = strands[selected_rgb_LED_string];		// DADA	TODO: multiple RGB strings
    int pix_i = pulse_2_rgb_pixel(pulse);	// TODO: use pulse intenal data
    pixelColor_t pixel;

    HSV_2_RGB_degree(&pixel, (H * 360.0), RGBstringConf.saturation, V);
    strand_p->pixels[pix_i] = pixel;

    // try the BlueHack, to give *more* blue
    int blue = (int) (((float) strand_p->pixels[pix_i].b * RGBstringConf.BlueHack_factor) + 0.5);
    if(blue > 255)
      blue = 255;
    strand_p->pixels[pix_i].b = blue;

    update_RGB_LED_string=true;		// new buffer content to be displayed
  } // if(RGBstringConf.rgb_strings_active && rgb_strings_available) {
} // set_pulse_LED_pixel_from_counter(int pulse)


void rgb_led_reset_to_default() {	// reset rgb led strip management to default conditions
  if(RGBstringConf.rgb_strings_active && rgb_strings_available) {
    RGBstringConf.saturation = RGBstringConf.saturation_start_value;
    clear_background_buffer0();
  }
}


//#define DEBUG_RGB_STRING_BACKGROUND
void set_rgb_led_background(int pulse) {
  if(!RGBstringConf.rgb_strings_active || !rgb_strings_available)
    return;

  if(PULSES.pulses[pulse].flags & HAS_RGB_LEDs) {
    strand_t * strand_p = strands[PULSES.pulses[pulse].rgb_string_idx];

//  pixelColor_t* pixel_p = strand_p->pixels[PULSES.pulses[pulse].rgb_pixel_idx];	DADA

    switch (RGBstringConf.set_background_algorithm) {
    case 1: // DIM
      {
	bool clear=false;
#if defined HARMONICAL_MUSIC_BOX && defined USE_RGB_LED_STRIP
	// bool clear_rgb_background_on_ending	// TODO: maybe	// DADA rgb_string_config
	// extern bool musicbox_is_ending();
	// DADA
	// clear = musicbox_is_ending();
#endif
	float x = strand_p->pixels[PULSES.pulses[pulse].rgb_pixel_idx].r;
	x *= RGBstringConf.rgb_background_dim;
	strand_p->pixels[PULSES.pulses[pulse].rgb_pixel_idx].r = (x + 0.5);
	if(clear)
	  strand_p->pixels[PULSES.pulses[pulse].rgb_pixel_idx].r = 0;

	x = strand_p->pixels[PULSES.pulses[pulse].rgb_pixel_idx].g;
	x *= RGBstringConf.rgb_background_dim;
	strand_p->pixels[PULSES.pulses[pulse].rgb_pixel_idx].g = (x + 0.5);
	if(clear)
	  strand_p->pixels[PULSES.pulses[pulse].rgb_pixel_idx].g = 0;

	x = strand_p->pixels[PULSES.pulses[pulse].rgb_pixel_idx].b;
	x *= RGBstringConf.rgb_background_dim;
	strand_p->pixels[PULSES.pulses[pulse].rgb_pixel_idx].b = (x + 0.5);
	if(clear)
	  strand_p->pixels[PULSES.pulses[pulse].rgb_pixel_idx].b = 0;

	update_RGB_LED_string=true;		// new buffer content to be displayed
      }
      break;

    case 2: // HISTORY
      {
	int i = PULSES.pulses[pulse].rgb_pixel_idx;
	// start with background history
	float R = background0[i].r;
	float G = background0[i].g;
	float B = background0[i].b;

#if defined DEBUG_RGB_STRING_BACKGROUND
	MENU.out(F("bg pixel  "));
	MENU.outln(i);

	MENU.out(F("last BG \t"));
	MENU.out(R);
	MENU.tab();
	MENU.out(G);
	MENU.tab();
	MENU.out(B);
	MENU.tab();
	MENU.out('*');
	MENU.outln(RGBstringConf.rgb_background_history_mix);
#endif

	// history share scaling
	R *= RGBstringConf.rgb_background_history_mix;
	G *= RGBstringConf.rgb_background_history_mix;
	B *= RGBstringConf.rgb_background_history_mix;

#if defined DEBUG_RGB_STRING_BACKGROUND
	MENU.out(F("history share\t"));
	MENU.out(R);
	MENU.tab();
	MENU.out(G);
	MENU.tab();
	MENU.out(B);
	MENU.tab();
	MENU.outln(F("now get FG"));
#endif

	// new (float) FG colors
	float fg_R = strand_p->pixels[i].r;
	float fg_G = strand_p->pixels[i].g;
	float fg_B = strand_p->pixels[i].b;

#if defined DEBUG_RGB_STRING_BACKGROUND
	MENU.out(F("new FG\t\t"));
	MENU.out(fg_R);
	MENU.tab();
	MENU.out(fg_G);
	MENU.tab();
	MENU.out(fg_B);
	MENU.tab();
	MENU.out('*');
	MENU.outln(RGBstringConf.rgb_background_dim);
	//	MENU.outln(1.0 - RGBstringConf.rgb_background_history_mix);
#endif

	// dim
	fg_R *= RGBstringConf.rgb_background_dim;
	fg_G *= RGBstringConf.rgb_background_dim;
	fg_B *= RGBstringConf.rgb_background_dim;

#if defined DEBUG_RGB_STRING_BACKGROUND
	MENU.out(F("dimmed fg\t"));
	MENU.out(fg_R);
	MENU.tab();
	MENU.out(fg_G);
	MENU.tab();
	MENU.out(fg_B);
	MENU.tab();
	MENU.out('*');
	MENU.outln(1 - RGBstringConf.rgb_background_history_mix);
	//	MENU.outln(1.0 - RGBstringConf.rgb_background_history_mix);
#endif

	// new color share
	fg_R *= (1 - RGBstringConf.rgb_background_history_mix);
	fg_G *= (1 - RGBstringConf.rgb_background_history_mix);
	fg_B *= (1 - RGBstringConf.rgb_background_history_mix);

#if defined DEBUG_RGB_STRING_BACKGROUND
	MENU.out(F("share fg \t"));
	MENU.out(fg_R);
	MENU.tab();
	MENU.out(fg_G);
	MENU.tab();
	MENU.out(fg_B);
	MENU.tab();
	MENU.outln(F("sum up"));
	//	MENU.outln(1.0 - RGBstringConf.rgb_background_history_mix);
#endif

	// sum up
	R += fg_R;
	G += fg_G;
	B += fg_B;
	// set and preserve history
	strand_p->pixels[i].r = background0[i].r = (R + 0.5);
	strand_p->pixels[i].g = background0[i].g = (G + 0.5);
	strand_p->pixels[i].b = background0[i].b = (B + 0.5);

#if defined DEBUG_RGB_STRING_BACKGROUND
	MENU.out(F("BG float\t"));
	MENU.out(R);
	MENU.tab();
	MENU.out(G);
	MENU.tab();
	MENU.out(B);
	MENU.tab();
	MENU.outln(F("as int"));

	MENU.out(F("BACKGOUND\t"));
	MENU.out(background0[i].r);
	MENU.tab();
	MENU.out(background0[i].g);
	MENU.tab();
	MENU.outln(background0[i].b);
	MENU.ln();
#endif

	update_RGB_LED_string=true;		// new buffer content to be displayed
      }
      break;

    default:
      ERROR_ln(F("set_background_algorithm"));
    } // switch (RGBstringConf.set_background_algorithm)
  } /* HAS_RGB_LEDs */ else
    ERROR_ln(F("no RGB LEDs"));
} // set_rgb_led_background(int pulse)

#if defined MULTICORE_RGB_STRING
TaskHandle_t multicore_rgb_string_draw_handle;

void multicore_rgb_string_draw_task(void* dummy) {
  digitalLeds_drawPixels(strands, 1);
  vTaskDelete(NULL);
}

void multicore_rgb_string_draw() {	// create and do one shot task
  BaseType_t err = xTaskCreatePinnedToCore(multicore_rgb_string_draw_task,	// function
					   "rgb_draw",			// name
					   2000,				// stack size
					   NULL,				// task input parameter
					   0,					// task priority
					   &multicore_rgb_string_draw_handle,	// task handle
					   0);					// core 0
  if(err != pdPASS) {
    MENU.out(err);
    MENU.space();
    MENU.error_ln(F("rgb_string_draw"));
  }
}
#endif // MULTICORE_RGB_STRING

void rgb_strings_info() {
  MENU.out(F("LED strings "));
  MENU.out(HARDWARE.rgb_strings);
  rgb_strings_available = (HARDWARE.rgb_strings > 0); // false, if there's no string
  MENU.out('\t');
  if(!rgb_strings_available)
    MENU.out(F("*not* "));
  MENU.out(F("available\tswitched"));
  MENU.out_ON_off(RGBstringConf.rgb_strings_active);

  MENU.out(F("\thigh priority"));
  MENU.out_ON_off(RGBstringConf.rgb_leds_high_priority);
  MENU.ln();

  for(int i=0; i<RGB_STRINGS_MAX; i++) {	// TODO: multiple rgb led strings
    if(i==selected_rgb_LED_string)
      MENU.out('*');
    else
      MENU.space();
    MENU.out(F(" string["));
    MENU.out(i);
    MENU.out(F("]\tpin "));
    MENU.out(HARDWARE.rgb_pin[i]);
    MENU.out(F("\t\tpixel "));
    MENU.out(HARDWARE.rgb_pixel_cnt[i]);
    MENU.out(F("\tvoltage "));
    MENU.out(HARDWARE.rgb_led_voltage_type[i]);
    MENU.out(F("\toffset "));
    MENU.outln(HARDWARE.rgb_pattern0[i]);
  }
}

void rgb_led_string_UI_display() {
  MENU.outln(F("\n'L'=rgbLED 'L<num>'=select string  'L+''LE'=ON  'L-''LT'=OFF 'LI'=intensity 'LO'='L0'offset 'LP'=pixel cnt"));
  MENU.outln(F("   'LH'=high priority 'LV'=voltage 'LB'=BGdim 'LS'=saturation0 'LR'=sat reset value 'LN'=hue slices"));
}

void rgb_led_string_UI() {	// starting 'L' already received
  int input_value;

  /*
    // TODO: unused (only one string) clashes with 'L0'=='LO' offset
  if(MENU.is_numeric()) {	// 'L<num>' select string
    input_value = MENU.numeric_input(selected_rgb_LED_string);
    if(input_value >= 0 && input_value < RGB_STRINGS_MAX) {
      selected_rgb_LED_string = input_value;
      MENU.out(F("selected RGB LED string "));
      MENU.outln(selected_rgb_LED_string);
    }
  }
  */

  switch (MENU.peek()) {	// second letter after 'L...'
  case '?':	// 'L?' (and bare 'L') led string infos
    MENU.drop_input_token();
  case EOF8:	// bare 'L' led string info
    rgb_strings_info();
    rgb_led_string_UI_display();
    break;

  case '-':	// 'L-' == 'LT' led string off
  case 'T':	// 'LT' for morse
    MENU.drop_input_token();
    if(! MENU.check_next('E'))		// 'LTE' stop RGB activity, but *LEAVE IT ON*
      pulses_RGB_LED_string_init();	//       else 'LT' switch RGB LED string off

    RGBstringConf.rgb_strings_active = false;
    MENU.outln(F("rgb leds off"));
    break;

  case '+':	// 'L+' == 'LE' led string on
  case 'E':	// 'LE' for morse
    MENU.drop_input_token();
    RGBstringConf.rgb_strings_active = true;
    MENU.outln(F("rgb leds activated"));
    break;

  case 'B':	// 'LB'	background dimming
    MENU.drop_input_token();
    input_value = MENU.numeric_input(0);
    if (input_value > 0) {
      RGBstringConf.rgb_background_dim = 1.0 / ((float) input_value);
    }

    if(MENU.maybe_display_more(VERBOSITY_LOWEST)) {
      MENU.out(F("background dim "));
      MENU.outln(RGBstringConf.rgb_background_dim);
    }
    break;

  case 'H': // 'LH' toggle high priority
    MENU.drop_input_token();
    RGBstringConf.rgb_leds_high_priority = !RGBstringConf.rgb_leds_high_priority;

    MENU.out(F("rgb_leds_high_priority "));
    MENU.out_ON_off(RGBstringConf.rgb_leds_high_priority);
    MENU.ln();
    break;

  case 'I':	// 'LI' led intenity
    MENU.drop_input_token();
    {
      float float_input_value = (float) MENU.numeric_input(RGBstringConf.rgb_led_string_intensity);
      if(float_input_value >= 0 && float_input_value <= 255)
	RGBstringConf.rgb_led_string_intensity = float_input_value;

      MENU.out(F("LED intensity "));
      MENU.outln(RGBstringConf.rgb_led_string_intensity);
    }
    break;

//case 'N':	// 'LN' number of strings	CLASH hue slices
//  MENU.drop_input_token();
//  input_value = MENU.numeric_input(HARDWARE.rgb_strings);
//  if((input_value >= 0) && (input_value < RGB_STRINGS_MAX))
//    HARDWARE.rgb_strings = input_value;
//  rgb_strings_info();
//  break;

  case 'N':	// 'LN' hue slices	CLASH number of strings
    MENU.drop_input_token();
    input_value = MENU.numeric_input(RGBstringConf.hue_slice_cnt);
    if((input_value >= 0) && (input_value < 256))
      HARDWARE.rgb_strings = input_value;
    rgb_strings_info();
    break;

  case 'O':	// 'LO'='L0' offset
  case '0':	// 'LO'='L0' offset
    MENU.drop_input_token();
    input_value = (uint8_t) MENU.numeric_input(HARDWARE.rgb_pattern0[selected_rgb_LED_string]);
    if(input_value >=0 && input_value < ILLEGAL8)
      HARDWARE.rgb_pattern0[selected_rgb_LED_string] = input_value;
    MENU.out(F("led offset "));
    MENU.outln(HARDWARE.rgb_pattern0[selected_rgb_LED_string]);
    break;

  case 'P':	// 'LP' led pixel_cnt
    MENU.drop_input_token();
    input_value = MENU.numeric_input(HARDWARE.rgb_pixel_cnt[selected_rgb_LED_string]);
    if(input_value >= 0)
      HARDWARE.rgb_pixel_cnt[selected_rgb_LED_string] = input_value;

    MENU.out(F("LED pixels "));
    MENU.outln(HARDWARE.rgb_pixel_cnt[selected_rgb_LED_string]);
    break;

  case 'R':	// 'LR' saturation_reset_value
    MENU.drop_input_token();
    input_value = MENU.numeric_input(0);
    if (input_value > 0) {
      RGBstringConf.saturation_reset_value = 1.0 / ((float) input_value);
    }

    if(MENU.maybe_display_more(VERBOSITY_LOWEST)) {
      MENU.out(F("saturation_reset_value "));
      MENU.outln(RGBstringConf.saturation_reset_value);
    }
    break;

  case 'S':	// 'LS'	saturation start value
    MENU.drop_input_token();
    input_value = MENU.numeric_input(0);
    if (input_value > 0) {
      RGBstringConf.saturation_start_value = 1.0 / ((float) input_value);
    }

    if(MENU.maybe_display_more(VERBOSITY_LOWEST)) {
      MENU.out(F("saturation start "));
      MENU.outln(RGBstringConf.saturation_start_value);
    }
    break;

  case 'V':	// 'LV' voltage type
    MENU.drop_input_token();
    input_value = MENU.numeric_input(HARDWARE.rgb_led_voltage_type[selected_rgb_LED_string]);
    set_rgb_string_voltage_type(input_value, selected_rgb_LED_string);
    break;

  } // switch second letter
} // rgb_led_string_UI()


#define PULSES_RGB_LED_STRING_H
#endif
