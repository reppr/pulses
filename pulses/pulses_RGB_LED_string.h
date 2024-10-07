/*
  pulses_RGB_LED_string.h

  new version using <Adafruit_NeoPixel.h>
*/


#if ! defined PULSES_RGB_LED_STRING_H

//#define DEBUG_LED_STRINGS			// empty  or 'SATURATION'
//#define DEBUG_LED_STRINGS	SATURATION	// empty  or 'SATURATION'

bool update_RGB_LED_string=false;	// is the string buffer dirty?

#define RGB_LED_STRING_TASK_PRIORITY	0

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
  bgCLEAR,	// clear bg
  bgSAME,	// same as fg
  bgINVERT,	// invert rgb values
  bgHISTORY_i,	// dim by rgb_background_dim, mix with (last bg * rgb_background_history_mix)	first INTEGER version
  //  bgHISTORY_f,	// looks like we will need that...
  bgAlgoMAX	// end marker
};

char* background_algorithm_name(int index) {
  switch(index) {
  case bgDIM:
    return F("bgDIM");
    break;
  case bgCLEAR:
    return F("bgCLEAR");
    break;
  case bgSAME:
    return F("bgSAME");
    break;
  case bgINVERT:
    return F("bgINVERT");
    break;
  case bgHISTORY_i:
    return F("bgHISTORY_i");
    break;
  default:
    return F("(algo unknown)");
    break;
  }
};

/* rgb_string_config_t	*software* configuration config for RGB LED strings
			for *hardware* see pulses_hardware_conf_t	*/
typedef struct rgb_string_config_t {
  uint8_t version=1;
  uint8_t version_minor=1;
  uint8_t reserved2=0;
  uint8_t reserved3=0;

  float rgb_led_string_intensity = (float) DEFAULT_LED_STRING_INTENSITY;
  float saturation_start_value = 0.2;	// TODO: test&trimm default value ################	UI
  float saturation = saturation_start_value;

  float saturation_change_factor = 1.004;	// TODO: UI
  float saturation_reset_value = 0.35;

#if defined TRIGGERED_MUSICBOX_LILYGO_213
  //  float rgb_background_dim = 0.056;
  float rgb_background_dim = 0.055;
  float rgb_background_history_mix = 0.2;	//0.25;	// how much history included in bg color
#else
  float rgb_background_dim = 0.45;	// ok for 5V version 1m 144
  //float rgb_background_dim = 0.1;	// TEST: for "12V" version 5m 300
  float rgb_background_history_mix = 0.66;	// how much history included in bg color
#endif

//float BlueHack_factor = 2.0;	// HACK: increase blueness
  float BlueHack_factor = 1.4;	// HACK: increase blueness

  float reserve_float_36=0.0;

  uint8_t brightness = 128;

  uint8_t reserve_uint8_41=0;
  uint8_t reserve_uint8_42=0;
  uint8_t reserve_uint8_43=0;

  uint8_t hue_slice_cnt = 15;	// just a usable default  see: set_automagic_hue_slices
  uint8_t set_background_algorithm = bgDIM;
  // see: pulses_hardware_conf_t  uint8_t rgb_pattern0

#if defined HIGH_PRIORITY_RGB_LED_UPDATE
  bool rgb_leds_high_priority = true;		// run time toggle with menu 'LH'
#else
  bool rgb_leds_high_priority = false;		// run time toggle with menu 'LH'
#endif

  bool clear_rgb_background_on_ending = true;	// TODO: ################
  bool set_automagic_hue_slices = true;		// if *not* set by user

  bool rgb_strings_active = false;		// defaults to INACTIVE to avoid clicks

  uint16_t reserve_padding=0;	// sizeof(rgb_string_config_t) will be rounded % 4 anyway
} rgb_string_config_t;

rgb_string_config_t RGBstringConf;


extern pulses_hardware_conf_t HARDWARE;	// hardware of this instrument

void show_rgb_string_conf(rgb_string_config_t* RGBconf) {
  MENU.outln(F("\nRGB string parameters:"));	// see also HARDWARE for pin, offset...
  const int len=32;
  char buf[len];

  snprintf(buf, len, "version\t\t%i.%i", (*RGBconf).version, (*RGBconf).version_minor);
  MENU.outln(buf);

  snprintf(buf, len, "intensity\t%g \t'LI'", (*RGBconf).rgb_led_string_intensity);
  MENU.outln(buf);

  snprintf(buf, len, "saturation0\t%g \t'L0'", (*RGBconf).saturation_start_value);
  MENU.outln(buf);

  snprintf(buf, len, "saturation\t%.5g\t'LS'", (*RGBconf).saturation);
  MENU.outln(buf);

  snprintf(buf, len, "sat change*\t%g \t'LC'", (*RGBconf).saturation_change_factor);
  MENU.outln(buf);

  snprintf(buf, len, "sat reset to\t%g \t'LR'", (*RGBconf).saturation_reset_value);
  MENU.outln(buf);

  snprintf(buf, len, "BGdim\t\t%g \t'LD'", (*RGBconf).rgb_background_dim);
  MENU.outln(buf);

  snprintf(buf, len, "BGhistory*\t%g \t'LM'", (*RGBconf).rgb_background_history_mix);
  MENU.outln(buf);

  MENU.out(F("bg algorithm\t"));
  MENU.out(background_algorithm_name((*RGBconf).set_background_algorithm));
  MENU.outln(F("\t'LA'"));

  snprintf(buf, len, "BlueHack*\t%g \t'LB'", (*RGBconf).BlueHack_factor);
  MENU.outln(buf);

  snprintf(buf, len, "hue slices\t%i \t'LN'", (*RGBconf).hue_slice_cnt);
  MENU.outln(buf);

  snprintf(buf, len, "priority\t%i \t'LH'", (*RGBconf).rgb_leds_high_priority);
  MENU.outln(buf);

  snprintf(buf, len, "clear on ending\t%i \t'LX'", (*RGBconf).clear_rgb_background_on_ending);
  MENU.outln(buf);

  snprintf(buf, len, "hue slices auto\t%i", (*RGBconf).set_automagic_hue_slices);
  MENU.outln(buf);

  snprintf(buf, len, "active\t\t%i \t'LE' 'LT'", (*RGBconf).rgb_strings_active);
  MENU.outln(buf);
} // show_rgb_string_conf()

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

#include <Adafruit_NeoPixel.h>

//#define NUM_STRIPS		1	// this version uses only *one* strip

Adafruit_NeoPixel RGB_LEDs(HARDWARE.rgb_pixel_cnt[0], HARDWARE.rgb_pin[0], NEO_GRB + NEO_KHZ800);
//Adafruit_NeoPixel RGB_LEDs(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void show_RGB_LEDs() {	// use *this* function, specially from other files
  RGB_LEDs.show();
  update_RGB_LED_string = false;
}

void clear_RGB_LEDs() {
  RGB_LEDs.clear();
  show_RGB_LEDs();
}

void setup_RGB_LED_strip() {
  MENU.out(F("setup_RGB_LED_strip()\t leds: "));
  MENU.out(RGB_STRING_LED_CNT);
  MENU.out(F("\tpin: "));
  MENU.out(HARDWARE.rgb_pin[0]);
  MENU.out(F("\tbrightness: "));
  MENU.out(RGBstringConf.brightness);

  RGB_LEDs.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  show_RGB_LEDs();            // Turn OFF all pixels ASAP
  RGB_LEDs.setBrightness(RGBstringConf.brightness);

  MENU.ln();
} // setup_RGB_LED_strip()

#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

int selected_rgb_LED_string = 0;		// default to the first string, only one in use anyway...


uint8_t /*red=*/ get_red(uint32_t color) {	// return red value from a packed uint32_t color
  return (uint8_t) ((color >> 16) & 0xff);
}

uint8_t /*green=*/ get_green(uint32_t color) {	// return green value from a packed uint32_t color
  return (uint8_t) ((color >> 8 ) & 0xff);
}

uint8_t /*blue=*/ get_blue(uint32_t color) {	// return blue value from a packed uint32_t color
  return (uint8_t) (color & 0xff);
}

void inspect_LED_pixel(short i) {
  MENU.out(F("pixel: "));
  MENU.outln(i);

  uint32_t color = RGB_LEDs.getPixelColor(i);
  MENU.out(F("color: "));
  MENU.out_hex(color);
  MENU.out(F("\tred = "));
  MENU.out(get_red(color));
  MENU.out(F("\tgreen = "));
  MENU.out(get_green(color));
  MENU.out(F("\tblue = "));
  MENU.outln(get_blue(color));
} // inspect_LED_pixel()


// background0[]  buffer for background color history
uint8_t background0[RGB_STRING_LED_CNT*3] = {0};	// keeps last used background colour, dimmed	TODO: allocate in setup?

void clear_background_buffer0() {	// TODO: DADA: unused?
  if(RGBstringConf.rgb_strings_active && rgb_strings_available) {
    for(int i=0; i < RGB_STRING_LED_CNT*3; i++)
      background0[i] = 0;
  }
}

void set_background_color(int i, uint8_t r, uint8_t g, uint8_t b) {
  if(RGBstringConf.rgb_strings_active && rgb_strings_available) {
    if(i < RGB_STRING_LED_CNT) {
      background0[i*3 +2] =r;
      background0[i*3 +1] =g;
      background0[i*3]    =b;
    }
  }
} // set_background_color()


void pulses_RGB_LED_string_init() {
  if(rgb_strings_available) {
    RGB_LEDs.begin();	// probably needed after wake up?
    clear_RGB_LEDs();
  }
} // pulses_RGB_LED_string_init()

int pulse_2_rgb_pixel(int pulse) {
  int pixel = pulse;
  pixel -= musicBoxConf.lowest_primary;
  pixel += HARDWARE.rgb_pattern0[0];	// DADA	TODO?: multiple RGB strings
  return pixel;
} // pulse_2_rgb_pixel()

void clear_RGB_string_pixel(int pulse) {
  if(RGBstringConf.rgb_strings_active && rgb_strings_available) {
    int pix_i = pulse_2_rgb_pixel(pulse);	// TODO: use pulse internal data
    RGB_LEDs.setPixelColor(pix_i, 0);
    update_RGB_LED_string = true;
  }
} // clear_RGB_string_pixel()

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
    H *= 65536L;
    //    V = RGBstringConf.rgb_led_string_intensity / (float) 255.0;
    uint8_t S = (uint8_t) (RGBstringConf.saturation * 255.0);
    uint8_t I = (uint8_t) RGBstringConf.rgb_led_string_intensity;
    int pix_i = pulse_2_rgb_pixel(pulse);	// TODO: use pulse internal data
    RGB_LEDs.setPixelColor(pix_i, RGB_LEDs.gamma32(RGB_LEDs.ColorHSV((uint16_t) H, S, I)));	// use gamma32 correction

//    // try the BlueHack, to give *more* blue
//    int blue = (int) (((float) leds[pix_i].b * RGBstringConf.BlueHack_factor) + 0.5);
//    if(blue > 255)
//      blue = 255;
//    leds[pix_i].b = blue;

    update_RGB_LED_string=true;		// new buffer content to be displayed
  } // if(RGBstringConf.rgb_strings_active && rgb_strings_available) {
} // set_pulse_LED_pixel_from_counter(int pulse)


void rgb_led_reset_to_default() {	// reset rgb led strip management to default conditions
  if(RGBstringConf.rgb_strings_active && rgb_strings_available) {
    RGBstringConf.saturation = RGBstringConf.saturation_start_value;
    clear_background_buffer0();
  }
} // rgb_led_reset_to_default()


//#define DEBUG_RGB_STRING_BACKGROUND
void switch_rgb_pix_2_background(int pulse) {
  if(!RGBstringConf.rgb_strings_active || !rgb_strings_available)
    return;

  if(PULSES.pulses[pulse].flags & HAS_RGB_LEDs) {
    uint16_t i = PULSES.pulses[pulse].rgb_pixel_idx;
    uint32_t fg_color = RGB_LEDs.getPixelColor(i);

    switch (RGBstringConf.set_background_algorithm) {
    case bgDIM: // DIM
      {
	bool clear=false;
#if defined HARMONICAL_MUSIC_BOX && defined USE_RGB_LED_STRIP
	// bool clear_rgb_background_on_ending	// TODO: maybe	// DADA rgb_string_config
	// extern bool musicbox_is_ending();
	// clear = musicbox_is_ending();
#endif
	float x;

	x = get_red(fg_color);
	x *= RGBstringConf.rgb_background_dim;
	x += 0.5;
	if(clear)
	  x = 0;
	uint8_t r=x;

	x = get_green(fg_color);
	x *= RGBstringConf.rgb_background_dim;
	x += 0.5;
	if(clear)
	  x = 0;
	uint8_t g=x;

	x = get_blue(fg_color);
	x *= RGBstringConf.rgb_background_dim;
	x += 0.5;
	if(clear)
	  x = 0;
	uint8_t b=x;

	RGB_LEDs.setPixelColor(i, r, g, b);
	update_RGB_LED_string=true;		// new buffer content to be displayed
      }
      break;

    case bgCLEAR:
      RGB_LEDs.setPixelColor(i, 0, 0, 0);
      update_RGB_LED_string=true;		// new buffer content to be displayed
      break;

    case bgSAME:
      break;					// do not change active led, no background...

    case bgINVERT:
      {
	uint8_t R = 0xFF - get_red(fg_color);
	uint8_t G = 0xFF - get_green(fg_color);
	uint8_t B = 0xFF - get_blue(fg_color);

	// TODO: use rgb_led_string_intensity or DEFAULT_LED_STRING_INTENSITY or 0xFF when inverting?
	float intesity_factor = RGBstringConf.rgb_led_string_intensity / 255.0;	// TODO: or DEFAULT_LED_STRING_INTENSITY ?
	R *= intesity_factor;
	G *= intesity_factor;
	B *= intesity_factor;

	R += 0.5;
	G += 0.5;
	B += 0.5;

	RGB_LEDs.setPixelColor(i, (uint8_t) R, (uint8_t) G, (uint8_t) B);
	update_RGB_LED_string=true;		// new buffer content to be displayed
      }
      update_RGB_LED_string=true;		// new buffer content to be displayed
      break;

    case bgHISTORY_i: // HISTORY
      {
	// start with background history
	float R = background0[i*3 +2];
	float G = background0[i*3 +1];
	float B = background0[i*3];

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
	float fg_R = get_red(fg_color);
	float fg_G = get_green(fg_color);
	float fg_B = get_blue(fg_color);

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
	fg_R *= (1.0 - RGBstringConf.rgb_background_history_mix);
	fg_G *= (1.0 - RGBstringConf.rgb_background_history_mix);
	fg_B *= (1.0 - RGBstringConf.rgb_background_history_mix);

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

	R += 0.5;
	G += 0.5;
	B += 0.5;

	// set pixels with some amount of history mixed in
	RGB_LEDs.setPixelColor(i, (uint8_t) R, (uint8_t) G, (uint8_t) B);
	update_RGB_LED_string=true;		// new buffer content to be displayed

	set_background_color(i, (uint8_t) R, (uint8_t) G, (uint8_t) B);	// preserve history

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
	MENU.out(background0[i*3 +2]);	// bg red
	MENU.tab();
	MENU.out(background0[i*3 +1]);	// bg green
	MENU.tab();
	MENU.outln(background0[i*3]);	// bg blue
	MENU.ln();
#endif

	update_RGB_LED_string=true;		// new buffer content to be displayed
      }
      break;

    default:
      ERROR_ln(F("set_background_algorithm"));
    } // switch (RGBstringConf.set_background_algorithm)

#if defined DEBUG_RGB_STRING_BACKGROUND		// && UI was active
    MENU.out(i); MENU.tab();
    MENU.out(get_red(fg_color)); MENU.tab(); MENU.out(get_green(fg_color)); MENU.tab(); MENU.outln(get_blue(fg_color));
#endif

  } /* HAS_RGB_LEDs */ else
    ERROR_ln(F("no RGB LEDs"));
} // switch_rgb_pix_2_background(int pulse)

#if defined MULTICORE_RGB_STRING
TaskHandle_t multicore_rgb_string_draw_handle;

void multicore_rgb_string_draw_task(void* dummy) {
  show_RGB_LEDs();
  vTaskDelete(NULL);
}

void multicore_rgb_string_draw() {	// create and do one shot task
  BaseType_t err = xTaskCreatePinnedToCore(multicore_rgb_string_draw_task,	// function
					   "rgb_draw",			// name
					   4*1024,				// stack size	TODO: test
					   NULL,				// task input parameter
					   RGB_LED_STRING_TASK_PRIORITY,	// task priority
					   &multicore_rgb_string_draw_handle,	// task handle
					   0);					// core 0
  if(err != pdPASS) {
    MENU.out(err);
    MENU.space();
    ERROR_ln(F("rgb_string_draw"));
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
} // rgb_strings_info()

void RGB_led_string_UI_display() {
  MENU.outln(F("\n'L'=rgbLED  'LE'=ON 'LT'=OFF  'LI'=intensity  'LO'=offset  'LP'=pixel cnt  'L<num>'=select string"));
  MENU.outln(F("   'LA'=BG algorithm  +'D'=dim +'C'=clear +'S'=same +'I'=invert +'H'=history  'LD'=BGdim* 'LM'=BGhistMix*"));
  MENU.outln(F("   'LS'=saturation  'L0'=saturation0  'LR'=sat reset value  'LB'=blueness"));
  MENU.outln(F("   'LH'=high priority  'LV'=voltage  'LN'=hue slices  'LX'=clear when done"));
} // RGB_led_string_UI_display()

void RGB_led_string_UI() {	// starting 'L' already received
  int input_value;
  float input_value_float;
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
    show_rgb_string_conf(&RGBstringConf);
    RGB_led_string_UI_display();
    return;	// do *not* show configuration data twice
    break;

  case 'T':	// 'LT' led string off (for morse)
    MENU.drop_input_token();
    if(! MENU.check_next('E'))		// 'LTE' stop RGB activity, but *LEAVE IT ON*
      pulses_RGB_LED_string_init();	//       else 'LT' switch RGB LED string off

    RGBstringConf.rgb_strings_active = false;
    MENU.outln(F("rgb leds off"));
    break;

  case 'E':	// 'LE' led string on (for morse)
    MENU.drop_input_token();
    RGBstringConf.rgb_strings_active = true;
    MENU.outln(F("rgb leds activated"));
    break;

  case 'D':	// 'LD'	background dimming
    MENU.drop_input_token();
    RGBstringConf.rgb_background_dim = MENU.float_input(RGBstringConf.rgb_background_dim);

    if(MENU.maybe_display_more(VERBOSITY_LOWEST)) {
      MENU.out(F("background dim "));
      MENU.outln(RGBstringConf.rgb_background_dim);
    }
    break;

  case 'M':	// 'LM'	background history Mix
    MENU.drop_input_token();
    RGBstringConf.rgb_background_history_mix = MENU.float_input(RGBstringConf.rgb_background_history_mix);

    if(MENU.maybe_display_more(VERBOSITY_LOWEST)) {
      MENU.out(F("background history mix "));
      MENU.outln(RGBstringConf.rgb_background_history_mix);
    }
    break;

  case 'A':	// 'LA'	background algorithmus
    MENU.drop_input_token();
    MENU.out(F("background algorithme "));

    switch(MENU.peek()) {	// next token after 'LA'
    case EOF8: // bare 'LA' cycle algoritms
      {
	int algo = RGBstringConf.set_background_algorithm;
	if(++algo == bgAlgoMAX)
	  algo = bgDIM;
	RGBstringConf.set_background_algorithm = algo;
      }
      break;
    case '?': // 'LA?'
      MENU.drop_input_token();
      break;
    case 'D': // 'LAD'  algo bgDIM
      MENU.drop_input_token();
      RGBstringConf.set_background_algorithm = bgDIM;
      break;
    case 'C': // 'LAC'  algo bgCLEAR
      MENU.drop_input_token();
      RGBstringConf.set_background_algorithm = bgCLEAR;
      break;
    case 'S': // 'LAS'  algo bgSAME
      MENU.drop_input_token();
      RGBstringConf.set_background_algorithm = bgSAME;
      break;
    case 'I': // 'LAI'  algo bgINVERT
      MENU.drop_input_token();
      RGBstringConf.set_background_algorithm = bgINVERT;
      break;
    case 'H': // 'LAH'  algo bgHISTORY_i
      MENU.drop_input_token();
      RGBstringConf.set_background_algorithm = bgHISTORY_i;
      break;
    }
    MENU.outln(background_algorithm_name(RGBstringConf.set_background_algorithm));
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
    input_value_float = MENU.float_input(RGBstringConf.rgb_led_string_intensity);
    if(input_value_float >= 0.0 && input_value_float <= 255.0)
      RGBstringConf.rgb_led_string_intensity = input_value_float;

      MENU.out(F("LED intensity "));
      MENU.outln(RGBstringConf.rgb_led_string_intensity);
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

  case 'O':	// 'LO' offset
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

  case 'S':	// 'LS'	saturation
    MENU.drop_input_token();
    input_value_float = MENU.float_input(RGBstringConf.saturation);
    if (input_value_float >= 0.0)
      RGBstringConf.saturation = input_value_float;

    if(MENU.maybe_display_more(VERBOSITY_LOWEST)) {
      MENU.out(F("saturation "));
      MENU.outln(RGBstringConf.saturation);
    }
    break;

  case 'C':	// 'LC'	saturation_change_factor
    MENU.drop_input_token();
    RGBstringConf.saturation_change_factor = MENU.float_input(RGBstringConf.saturation_change_factor);

    if(MENU.maybe_display_more(VERBOSITY_LOWEST)) {
      MENU.out(F("saturation change "));
      MENU.outln(RGBstringConf.saturation_change_factor);
    }
    break;

  case '0':	// 'L0'	saturation_start_value  saturation0
    MENU.drop_input_token();
    RGBstringConf.saturation_start_value = MENU.float_input(RGBstringConf.saturation_start_value);

    if(MENU.maybe_display_more(VERBOSITY_LOWEST)) {
      MENU.out(F("saturation start "));
      MENU.outln(RGBstringConf.saturation_start_value);
    }
    break;

  case 'R':	// 'LR' saturation_reset_value
    MENU.drop_input_token();
    input_value_float = MENU.float_input(RGBstringConf.saturation_reset_value);
    if (input_value_float >= 0.0)
      RGBstringConf.saturation_reset_value = input_value_float;

    if(MENU.maybe_display_more(VERBOSITY_LOWEST)) {
      MENU.out(F("saturation_reset_value "));
      MENU.outln(RGBstringConf.saturation_reset_value);
    }
    break;

  case 'B':	// 'LB' blueness factor
    MENU.drop_input_token();
    RGBstringConf.BlueHack_factor = MENU.float_input(RGBstringConf.BlueHack_factor);

    if(MENU.maybe_display_more(VERBOSITY_LOWEST)) {
      MENU.out(F("blueness "));
      MENU.outln(RGBstringConf.BlueHack_factor);
    }
    break;

  case 'V':	// 'LV' voltage type
    MENU.drop_input_token();
    input_value = MENU.numeric_input(HARDWARE.rgb_led_voltage_type[selected_rgb_LED_string]);
    set_rgb_string_voltage_type(input_value, selected_rgb_LED_string);
    break;

  case 'X':	// 'LX' toggle clear on ending
    RGBstringConf.clear_rgb_background_on_ending = ! RGBstringConf.clear_rgb_background_on_ending;
    if(MENU.maybe_display_more(VERBOSITY_LOWEST)) {
      MENU.out(F("clear on ending"));
      MENU.out_ON_off(RGBstringConf.clear_rgb_background_on_ending);
    }
    break;

  default:
    return;		// nothing appropriate, quit
  } // switch second letter
  if(MENU.maybe_display_more(VERBOSITY_MORE))
    show_rgb_string_conf(&RGBstringConf);
} // RGB_led_string_UI()


void set_rgb_pix_of_parent_to_background(int pulse /*secondary*/) {	// see: beforeEXIT
//maybe:  if(! RGBstringConf.rgb_strings_active) {
//maybe:    return;
//maybe:  } // else

  if(PULSES.pulses[pulse].flags & DO_ON_EXIT) {
    int parent= PULSES.pulses[pulse].other_pulse;
    if(PULSES.pulses[parent].groups & g_PRIMARY) {		// safety net
      switch_rgb_pix_2_background(parent);
    } // else ERROR parent not g_PRIMARY
  }
} // set_rgb_pix_of_parent_to_background()


#define PULSES_RGB_LED_STRING_H
#endif
