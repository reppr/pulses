/*
  old_style_experiments.h

  OBSOLETE, use presets instead

*/


#if ! defined OLD_STYLE_EXPERIMENTS_H

int selected_experiment=-1;

/* **************************************************************** */
/*
  void display_names_OBSOLETE(char** names, int count, int selected);
  display an array of strings like names of scales, jiffles, experiments
  mark the selected one with an asterisk
*/
void display_names_OBSOLETE(const char** names, int count, int selected) {	// TODO: maybe obsolete?  (only for experiment names)
  MENU.ln();
  for (int i = 0; i < count; i++) {
    if (i==selected)
      MENU.out('*');
    else
      MENU.space();
    MENU.space();

    MENU.out(i);
    MENU.tab(); MENU.outln(names[i]);
  }
  MENU.ln();
} // display_names_OBSOLETE()


void init_pentatonic(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync) {
  /*
    I was looking at pentatonic scales
    one of my favorits follows the 'd f g a c' pattern

    Let's look at the  numeric relations:
    d   f   g   a   c   d

    5 : 6
    3   :   4
    2     :     3
    1         :         2
  */
  if (inverse) {
    no_g_inverse();
    return;
  }
  unsigned long unit = multiplier * PULSES.time_unit;
  unit /= divisor;

  display_name5pars("init_pentatonic", inverse, voices, multiplier, divisor, sync);

  pulse_time_t now;
  PULSES.get_now();
  now=PULSES.now;

//  setup_click_synced(now, unit, 1, 1, sync);	// 'd'
//  setup_click_synced(now, unit, 5, 6, sync);	// 'f'
//  setup_click_synced(now, unit, 3, 4, sync);	// 'g'
//  setup_click_synced(now, unit, 2, 3, sync);	// 'a'
//  setup_click_synced(now, unit/3*2, 5, 6, sync); // 'c' is fifth from 'f'
//  setup_click_synced(now, unit, 1, 2, sync);	// 'd'

  setup_jiffle_thrower_synced(now, unit, 1, 1, sync, gling128);	// 'd'
  setup_jiffle_thrower_synced(now, unit, 5, 6, sync, gling128);	// 'f'
  setup_jiffle_thrower_synced(now, unit, 3, 4, sync, gling128);	// 'g'
  setup_jiffle_thrower_synced(now, unit, 2, 3, sync, gling128);	// 'a'
  if (voices > 4) {
    setup_jiffle_thrower_synced(now, unit/3*2, 5, 6, sync, gling128); // 'c' is fifth from 'f'
    setup_jiffle_thrower_synced(now, unit, 1, 2, sync, gling128);	// 'd'
  }
  if (voices > 6) {
    setup_jiffle_thrower_synced(now, unit, 5, 2*6, sync, gling128);	// 'f'
  }
  if (voices > 7) {
    // setup_jiffle_thrower_synced(now, unit, 3, 2*4, sync, gling128);	// 'g'
    setup_jiffle_thrower_synced(now, unit, 2, 2*3, sync, gling128);	// 'a' seems better on top with 8 voices
  }

  PULSES.fix_global_next();
} // init_pentatonic()


/* **************************************************************** */
// playing with chords:

//   init_div_123456(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync);
void init_div_123456(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync) {
//	  const unsigned long divisor=1L;
  unsigned long unit = multiplier * PULSES.time_unit;
  unit /= divisor;

  display_name5pars("init_div_123456", inverse, voices, multiplier, divisor, sync);

  pulse_time_t now;
  PULSES.get_now();
  now=PULSES.now;

  int integer;
  if (! inverse) {	// low pin number has low note
    for (integer=1; integer<=voices; integer++) {
      setup_click_synced(now, unit, 1, integer, sync);
      MENU.out(integer);
    }
  } else {	// inverse: low pin number has high frequency
    for (integer=voices; integer>0; integer--) {
      setup_click_synced(now, unit, 1, integer, sync);
      MENU.out(integer);
    }
  }
  MENU.ln();

  PULSES.fix_global_next();
} // init_div_123456()


void init_123456(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync) {
  unsigned long unit = multiplier * PULSES.time_unit;
  unit /= divisor;

  display_name5pars("init123456", inverse, voices, multiplier, divisor, sync);

  pulse_time_t now;
  PULSES.get_now();
  now=PULSES.now;

  divisor=2L*36L;
  int integer;
  if (! inverse) {	// bottom down/up click-pin mapping
    // low pin number has longest period
    for (integer=voices; integer>0; integer--) {
      setup_click_synced(now, unit, integer, divisor, sync);
      MENU.out(integer);
    }
  } else {
    // low pin number has shortest period
    for (integer=1; integer<=voices; integer++) {
      setup_click_synced(now, unit, integer, divisor, sync);
      MENU.out(integer);
    }
  }
  MENU.ln();

  PULSES.fix_global_next();
} // init_123456()

// setup for ESP8266 Frog Orchester
void setup_jiffles0(bool g_inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync) {
  if (g_inverse) {
    no_g_inverse();
    return;
  }
  unsigned long unit = multiplier * PULSES.time_unit;
  unit /= divisor;

  display_name5pars("setup_jiffles0", g_inverse, voices, multiplier, divisor, sync);

  pulse_time_t when;
  PULSES.get_now();
  when=PULSES.now;

  divisor=3;	// this was just the default for the frogs

  multiplier=2;
  setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiffletab0);

  multiplier=3;
  setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiffletab0);

  multiplier=4;
  setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiffletab0);

  multiplier=5;
  setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiffletab0);

  multiplier=6;
  setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiffletab0);

//	  multiplier=8;
//	  setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiffletab0);
//
//	  multiplier=9;
//	  setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiffletab0);
//
//	  multiplier=10;
//	  setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiffletab0);

  PULSES.fix_global_next();
} // setup_jiffles0()


void setup_jiffle128(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync) {
  /*
  multiplier and divisor are used twice:
  first to scale unit from PULSES.time_unit
  then reset to build the jiffle thrower pulses
    multiplier=1
    divisor = 1, 2, 3, 4, ...
  */
  unsigned long unit = multiplier * PULSES.time_unit;
  unit /= divisor;

  display_name5pars("setup_jiffle128", inverse, voices, multiplier, divisor, sync);

  PULSES.get_now();
  pulse_time_t when=PULSES.now;

  multiplier=1;
  if (!inverse) {
    for (int click=0; click<voices; click++) {
      divisor=click+1;
      setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, gling128);
    }
  } else {
    for (int click=voices-1; click>=0; click--) {
      divisor=click+1;
      setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, gling128);
    }
  }

  PULSES.fix_global_next();
} // setup_jiffle128()

// triplets {multiplier, divisor, count}
// multiplier==0 means end
unsigned int jiff0[] =
  {1,16,4, 1,24,6, 1,128,16, 1,1024,64, 1,2048,128, 1,4096,256, 1,2048,64, 1,4096,128, 1,32,2, 0,0}; // there *must* be a trailing zero.);

unsigned int jiff1[] =
  {1,512,8, 1,1024,16, 1,2048,32, 1,1024,16, 0,0};

unsigned int jiff2[] =
  {1,2096,4, 1,512,2, 1,128,2, 1,256,2, 1,512,8, 1,1024,32, 1,512,4, 1,256,3, 1,128,2, 1,64,1, 0,0};


void setup_jifflesNEW(bool g_inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync) {
  if (g_inverse) {
    no_g_inverse();
    return;
  }
  unsigned long unit = multiplier * PULSES.time_unit;
  unit /= divisor;

  display_name5pars("setup_jifflesNEW", g_inverse, voices, multiplier, divisor, sync);

  pulse_time_t when;
  PULSES.get_now();
  when=PULSES.now;

  multiplier=1;

  divisor=1;
  setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiff0);

  divisor=3;
  setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiff1);

  divisor=4;
  setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiff2);

  if (voices > 5) {
    divisor=6;
    setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiffletab0);
  }

  PULSES.fix_global_next();
} // setup_jifflesNEW()

// pre-defined jiffle pattern:
void setup_jiffles2345(bool g_inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync) {
  if (g_inverse) {
    no_g_inverse();
    return;
  }
  unsigned long unit = multiplier * PULSES.time_unit;
  unit /= divisor;

  display_name5pars("jiffles2345", g_inverse, voices, multiplier, divisor, sync);

  pulse_time_t when;
  PULSES.get_now();
  when=PULSES.now;

  divisor=1;

  multiplier=2;
  setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiffletab);

  multiplier=3;
  setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiffletab);

  multiplier=4;
  setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiffletab);

  multiplier=5;
  setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiffletab);

  // up to 8 voices
  if (voices > 5) {
    multiplier=6;
    setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiffletab);

    // skip multiplier 7

    if (voices > 6) {
      multiplier=8;
      setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiffletab);

      if (voices > 7) {
	multiplier=9;
	setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiffletab);
      }
    }

    // next would be i.e. 10, 12, 14, 15, 16
    // so 2 3 4 5 6 8 9 10 12 14 15 16
  }

  PULSES.fix_global_next();
} //  setup_jiffles2345()

void select_scale__UI() {	// DADA OBSOLETE?: only used inside oldstyle experiments...
  /*
    void select_scale__UI() examples:
    "a"		metric a=440 *minor* scale
    "E"		metric E=330 *major* scale
    "d5"	metric d *minor pentatonic* scale
    "C5"	metric C *european pentatonic* scale
    "F4"	*tetraCHORD* (as a scale) on metric F
  */

  unsigned int* scale_was = selected_in(SCALES);

  switch (MENU.peek()) {
  case EOF8:
    break;

    /*
      TODO:
      case '?':
      break;
    */
  case 'U':	// toggle harmonical time unit
    MENU.drop_input_token();

    if(PULSES.time_unit != TIME_UNIT)
      PULSES.time_unit=TIME_UNIT;	// switch to harmonical time unit
    else
      PULSES.time_unit=1000000;		// switch to metric time unit
    break;

  case 'S':	// use       if(UI_select_from_DB(SCALES))	// select scale
    MENU.drop_input_token();
    if(UI_select_from_DB(SCALES))	// select scale
      scale_user_selected = true;
    break;

  case 'c':	// c minor
    musicBoxConf.chromatic_pitch = 4;
    MENU.drop_input_token();
    user_selected_scale(minor_scale);
    PULSES.time_unit=1000000;	// switch to metric time unit
    musicBoxConf.pitch.divisor=262; // 261.63	// C4  ***not*** harmonical
    break;

  case 'C':	// c major
    musicBoxConf.chromatic_pitch = 4;
    MENU.drop_input_token();
    user_selected_scale(major_scale);
    PULSES.time_unit=1000000;	// switch to metric time unit
    musicBoxConf.pitch.divisor=262; // 261.63	// C4  ***not*** harmonical
    MENU.outln(" ok");
    break;

  case 'd':	// d minor scale
    musicBoxConf.chromatic_pitch = 6;
    MENU.drop_input_token();
    PULSES.time_unit=1000000;	// switch to metric time unit
    musicBoxConf.pitch.divisor = 294;		// 293.66 = D4
    // divisor = 147;		// 146.83 = D3
    user_selected_scale(minor_scale);
    break;

  case 'D':	// D major scale
    musicBoxConf.chromatic_pitch = 6;
    MENU.drop_input_token();
    PULSES.time_unit=1000000;	// switch to metric time unit
    musicBoxConf.pitch.divisor = 294;		// 293.66 = D4
    // divisor = 147;		// 146.83 = D3
    user_selected_scale(major_scale);
    break;

  case 'e':	// e minor scale
    musicBoxConf.chromatic_pitch = 8;
    MENU.drop_input_token();
    user_selected_scale(minor_scale);
    PULSES.time_unit=1000000;	// switch to metric time unit
    musicBoxConf.pitch.divisor=330; // 329.36	// e4  ***not*** harmonical
    break;

  case 'E':	// E major scale
    musicBoxConf.chromatic_pitch = 8;
    MENU.drop_input_token();
    user_selected_scale(major_scale);
    PULSES.time_unit=1000000;	// switch to metric time unit
    musicBoxConf.pitch.divisor=330; // 329.36	// e4  ***not*** harmonical
    break;

  case 'f':	// f minor
    musicBoxConf.chromatic_pitch = 9;
    MENU.drop_input_token();
    user_selected_scale(minor_scale);
    PULSES.time_unit=1000000;	// switch to metric time unit
    musicBoxConf.pitch.divisor=175; // 174.16	// F3  ***not*** harmonical
    break;

  case 'F':	// f major
    musicBoxConf.chromatic_pitch = 9;
    MENU.drop_input_token();
    user_selected_scale(major_scale);
    PULSES.time_unit=1000000;	// switch to metric time unit
    musicBoxConf.pitch.divisor=175; // 174.16	// F3  ***not*** harmonical
    break;

  case 'g':	// g minor
    musicBoxConf.chromatic_pitch = 11;
    MENU.drop_input_token();
    user_selected_scale(minor_scale);
    PULSES.time_unit=1000000;	// switch to metric time unit
    musicBoxConf.pitch.divisor=196; // 196.00	// G3  ***not*** harmonical
    break;

  case 'G':	// g major
    musicBoxConf.chromatic_pitch = 11;
    MENU.drop_input_token();
    user_selected_scale(major_scale);
    PULSES.time_unit=1000000;	// switch to metric time unit
    musicBoxConf.pitch.divisor=196; // 196.00	// G3  ***not*** harmonical
    break;

  case 'a':	// a minor scale
    musicBoxConf.chromatic_pitch = 1;
    MENU.drop_input_token();
    PULSES.time_unit=1000000;	// switch to metric time unit
    musicBoxConf.pitch.divisor = 440;
    user_selected_scale(minor_scale);
    break;

  case 'A':	// A major scale
    musicBoxConf.chromatic_pitch = 1;
    MENU.drop_input_token();
    PULSES.time_unit=1000000;	// switch to metric time unit
    musicBoxConf.pitch.divisor = 440;
    user_selected_scale(major_scale);
    break;

  case 'b':	// b minor
    musicBoxConf.chromatic_pitch = 3;
    MENU.drop_input_token();
    user_selected_scale(minor_scale);
    PULSES.time_unit=1000000;	// switch to metric time unit
    musicBoxConf.pitch.divisor=247; // 246.94	// B3  ***not*** harmonical
    break;

  case 'B':	// b major
    musicBoxConf.chromatic_pitch = 3;
    MENU.drop_input_token();
    user_selected_scale(major_scale);
    PULSES.time_unit=1000000;	// switch to metric time unit
    musicBoxConf.pitch.divisor=247; // 246.94	// B3  ***not*** harmonical
    break;
  }

  switch (MENU.peek()) {	// (second or) third letters for other scales
  case EOF8:
    break;

  case '0':	// europ_PENTAtonic ('0' just as it is free)	// shortcut europ_PENTAtonic
    MENU.drop_input_token();
    user_selected_scale(europ_PENTAtonic);
    break;
  case '9':	// pentaCHORD ('9' just as it is free)
    MENU.drop_input_token();
    user_selected_scale(pentaCHORD);
    break;
  case '8':	// major scale	// analog '7' (which is handy for morse input)
    MENU.drop_input_token();
    user_selected_scale(major_scale);
    break;
  case '7':	// minor scale	// avoid minuscules in morse: type 'E7' instead of 'e'
    MENU.drop_input_token();
    user_selected_scale(minor_scale);
    break;
  case '6':	// doric scale	// TODO: check 14
    MENU.drop_input_token();
    user_selected_scale(doric_scale);
    break;
  case '5':	// 5  pentatonic minor scale
    MENU.drop_input_token();
    user_selected_scale(pentatonic_minor);
    break;
  case '4':	// 4  tetraCHORD | tetrachord
    MENU.drop_input_token();
    if ((selected_in(SCALES) == minor_scale) || (selected_in(SCALES) == pentatonic_minor) || \
	(selected_in(SCALES) == doric_scale) || (selected_in(SCALES) == triad))
      user_selected_scale(tetrachord);
    else
      user_selected_scale(tetraCHORD);
    break;
  case '3':	// 3  octaves fourths fifths
    MENU.drop_input_token();
    user_selected_scale(octave_4th_5th);
    break;
  case '2':	// 2  octaves fifths
    MENU.drop_input_token();
    user_selected_scale(octaves_fifths);
    break;
  case '1':	// 1  octaves
    MENU.drop_input_token();
    user_selected_scale(octaves);
    break;
  }

  if(selected_in(SCALES) != scale_was)
    not_a_preset();
} // select_scale__UI()

void Press_toStart() {	// for old style 'experiment'
  MENU.outln(F("Press '!' to start"));
}


old_style_experiments_menu_reaction() {
    if (MENU.maybe_display_more()) {
      MENU.out(F("experiment "));
    }

    input_value = MENU.numeric_input(-1);	// select experiment

    if (input_value==-1)
      display_names_OBSOLETE(experiment_names, n_experiment_names, selected_experiment);

    else if (input_value>=0 ) {	// select, initialize experiment
      selected_experiment = input_value;
      switch (selected_experiment) {
	/* some old style setups just initialise defaults,  but do not start yet, press '!'
	   others do start if you add a '!' as last char
	   you can edit and then press '!' or 'n' (== synchronise to *now*)
	   sorry, it's messy!	TODO: unmess
	*/

//      case 0:	// free setup planed
//	  break;

      case 1:	// setup_jiffle128
	musicBoxConf.pitch.multiplier=2;
	musicBoxConf.pitch.divisor=1;
	musicBoxConf.sync=15;

	if (MENU.maybe_display_more()) {
	  display_name5pars("setup_jiffle128", g_inverse, voices, \
			    musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
	  Press_toStart();
	}
	break;

      case 2:	// init_div_123456
	musicBoxConf.sync=0;
	musicBoxConf.pitch.multiplier=1;
	musicBoxConf.pitch.divisor=1;

	if (MENU.maybe_display_more()) {
	  display_name5pars("init_div_123456", g_inverse, voices, \
			    musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
	  Press_toStart();
	}
	break;

      case 3:	// setup_jiffles0
	musicBoxConf.sync=1;
	musicBoxConf.pitch.multiplier=8;
	musicBoxConf.pitch.divisor=3;
#if GPIO_PINS > 0
	reverse_gpio_pins();
#endif
	if (MENU.maybe_display_more()) {
	  MENU.out(F("setup_jiffles0("));
	  MENU.out(g_inverse);
	  display_next_par(voices);
	  display_next_par(musicBoxConf.pitch.multiplier);
	  display_next_par(musicBoxConf.pitch.divisor);
	  display_next_par(musicBoxConf.sync);
	  MENU.outln(F(")  ESP8266 Frogs"));
	  Press_toStart();
	}
	break;

      case 4:	// setup_jiffles2345
	musicBoxConf.pitch.multiplier=1;
	musicBoxConf.pitch.divisor=2;
	musicBoxConf.sync=0;
	select_in(JIFFLES, jiffletab);

	if (MENU.maybe_display_more()) {
	  display_name5pars("setup_jiffles2345", g_inverse, voices, \
			    musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
	  Press_toStart();
	}
	break;

      case 5:	// init_123456
	musicBoxConf.sync=0;		// FIXME: test and select ################
	musicBoxConf.pitch.multiplier=3;
	musicBoxConf.pitch.divisor=1;

	if (MENU.maybe_display_more()) {
	  display_name5pars("init_123456", g_inverse, voices, \
			    musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
	  Press_toStart();
	}
	break;

      case 6:	// init_chord_1345689a
	musicBoxConf.sync=0;		// FIXME: test and select ################
	musicBoxConf.pitch.multiplier=1;
	musicBoxConf.pitch.divisor=1;

	if (MENU.maybe_display_more()) {
	  display_name5pars("init_chord_1345689a", g_inverse, voices, \
			    musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
	  Press_toStart();
	}
	break;

      case 7:	// init_rhythm_1
	musicBoxConf.sync=1;
	musicBoxConf.pitch.multiplier=1;
	musicBoxConf.pitch.divisor=6*7;

	if (MENU.maybe_display_more()) {
	  display_name5pars("init_rhythm_1", g_inverse, voices, \
			    musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
	  Press_toStart();
	}
	break;

      case 8:	// init_rhythm_2
	musicBoxConf.sync=5;
	musicBoxConf.pitch.multiplier=1;
	musicBoxConf.pitch.divisor=1;

	if (MENU.maybe_display_more()) {
	  display_name5pars("init_rhythm_2", g_inverse, voices, \
			    musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
	  Press_toStart();
	}
	break;

      case 9:  // init_rhythm_3
	musicBoxConf.sync=3;
	musicBoxConf.pitch.multiplier=1;
	musicBoxConf.pitch.divisor=1;

	if (MENU.maybe_display_more()) {
	  display_name5pars("init_rhythm_3", g_inverse, voices, \
			    musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
	  Press_toStart();
	}
	break;

      case 10:	// init_rhythm_4
	musicBoxConf.sync=1;
	musicBoxConf.pitch.multiplier=1;
	musicBoxConf.pitch.divisor=7L*3L;

	if (MENU.maybe_display_more()) {
	  display_name5pars("init_rhythm_4", g_inverse, voices, \
			    musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
	  Press_toStart();
	}
	break;

      case 11:	// setup_jifflesNEW
	musicBoxConf.sync=3;
	musicBoxConf.pitch.multiplier=3;
	musicBoxConf.pitch.divisor=1;

	if (MENU.maybe_display_more()) {
	  display_name5pars("setup_jifflesNEW", g_inverse, voices, \
			    musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
	  Press_toStart();
	}
	break;

      case 12:	// init_pentatonic
	musicBoxConf.pitch.multiplier=1;
	musicBoxConf.pitch.divisor=1;

	select_in(SCALES, minor_scale);		// default e minor
	select_in(JIFFLES, piip2048);

	if (voices == 0)
	  voices = GPIO_PINS;

	PULSES.select_n(voices);
	tune_2_scale(voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, selected_in(SCALES));	// TODO: OBSOLETE?

  #ifndef USE_DACs	// TODO: review and use test code
	setup_jiffle_thrower_selected(selected_actions);
  #else // *do* use dac		// TODO: not here ################
	selected_share_DACsq_intensity(DAx_max, 1);

    #if (USE_DACs == 1)
	setup_jiffle_thrower_selected(DACsq1);
    #else
	selected_DACsq_intensity_proportional(DAx_max, 2);
//	selected_share_DACsq_intensity(DAx_max, 2);

	setup_jiffle_thrower_selected(DACsq1 | DACsq2);
    #endif
  #endif

	PULSES.activate_selected_synced_now(musicBoxConf.sync);	// sync and activate;
	MENU.ln();

	if (MENU.maybe_display_more())
	  selected_or_flagged_pulses_info_lines();

	break;


      case 13:	// prepare_magnets
	musicBoxConf.sync=1;	// or: musicBoxConf.sync=0;
	musicBoxConf.pitch.multiplier=1;
	musicBoxConf.pitch.divisor=1;
	voices=8;	//just for 'The Harmonical Strings Christmas Evening Sounds'
	g_inverse=false;
	// unsigned int harmonics4 = {1,1,1024, 1,2,1024, 1,3,1024, 1,4,1024, 0,0};
	select_in(JIFFLES, harmonics4);
	PULSES.select_n(voices);
	display_name5pars("prepare_magnets", g_inverse, voices, \
			  musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
	prepare_magnets(g_inverse, voices, \
			musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);

	if (MENU.maybe_display_more()) {
	  selected_or_flagged_pulses_info_lines();
	  Press_toStart();
	}
	break;

      case 14:	// E14
	// magnets on strings, second take
	musicBoxConf.pitch.multiplier=1;
	musicBoxConf.pitch.divisor=1;
	g_inverse=false;

	select_in(SCALES, pentatonic_minor);
	PULSES.select_n(voices);
	prepare_scale(false, voices, musicBoxConf.pitch.multiplier * 1024 , musicBoxConf.pitch.divisor * 1167, musicBoxConf.sync, selected_in(SCALES));
	select_in(JIFFLES, ting1024);
	PULSES.select_n(voices);
	display_name5pars("E14", g_inverse, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);

	if (MENU.maybe_display_more())
	  selected_or_flagged_pulses_info_lines();
	break;

      case 15:	// E15
	// magnets on strings, third take
	musicBoxConf.pitch.multiplier=1;
	musicBoxConf.pitch.divisor=1;
	g_inverse=false;

	select_in(SCALES, pentatonic_minor);
	PULSES.select_n(voices);
	prepare_scale(false, voices, musicBoxConf.pitch.multiplier * 4096 , musicBoxConf.pitch.divisor * 1167, musicBoxConf.sync, selected_in(SCALES));
	select_in(JIFFLES, ting4096);
	PULSES.select_n(voices);
	display_name5pars("E15", g_inverse, voices, \
			  musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);

	if (MENU.verbosity >= VERBOSITY_SOME)
	  selected_or_flagged_pulses_info_lines();
	break;

      case 16:	// E16 europ_PENTAtonic
	// piezzos on low strings 2016-12-28
	musicBoxConf.pitch.multiplier=4096;
	musicBoxConf.pitch.divisor=256;
	g_inverse=false;

	select_in(SCALES, europ_PENTAtonic);
	PULSES.select_n(voices);
	prepare_scale(false, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync, selected_in(SCALES));
	select_in(JIFFLES, ting4096);
	// select_in(JIFFLES, arpeggio4096);
	display_name5pars("E16 PENTAtonic", g_inverse, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);

	if (MENU.verbosity >= VERBOSITY_SOME)
	  selected_or_flagged_pulses_info_lines();
	break;

      case 17:	// E17 mimic japan
	// magnets on steel strings, "japanese"
	musicBoxConf.pitch.multiplier=1;	// click
	// multiplier=4096;	// jiffle ting4096
	musicBoxConf.pitch.divisor=256*5;

	select_in(SCALES, mimic_japan_pentatonic);
	PULSES.select_n(voices);
	prepare_scale(false, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync, selected_in(SCALES));
	select_in(JIFFLES, ting4096);
	display_name5pars("E17 mimic japan", g_inverse, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);

	if (MENU.verbosity >= VERBOSITY_SOME)
	  selected_or_flagged_pulses_info_lines();

	break;

      case 18:	// E18 pentatonic minor  nylon stringed wooden box, piezzos
	select_in(SCALES, pentatonic_minor);
	musicBoxConf.pitch.multiplier=1;	// click
	// musicBoxConf.pitch.multiplier=4096;	// jiffle ting4096
	// divisor=2048;

	// string tuning on 8/9
	//   musicBoxConf.pitch.multiplier=8;
	//   divisor=9*1024;
	// musicBoxConf.pitch.multiplier=1;
	// divisor=9*128;

	// musicBoxConf.pitch.multiplier=8*4096;	// jiffle ting4096
	// divisor=9*1024;
	musicBoxConf.pitch.multiplier=32;	// reduced
	//#if defined(ESP32)	// used as test setup with 16 clicks
	// ################ FIXME: ESP32 16 click ################
	//      multiplier *= 4;
	//#endif
	musicBoxConf.pitch.divisor=9;	// reduced
	select_in(JIFFLES, ting4096);

	PULSES.select_n(voices);
	prepare_scale(false, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync, selected_in(SCALES));
	display_name5pars("E18 pentatonic minor", g_inverse, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);

	if (MENU.verbosity >= VERBOSITY_SOME)
	  selected_or_flagged_pulses_info_lines();

	break;

#ifdef IMPLEMENT_TUNING		// implies floating point
      case 19:	// TUNING: tuned sweep
	PULSES.clear_selection();
	PULSES.select_pulse(0);
	sweep_up = 1;
	PULSES.reset_and_edit_pulse(0, PULSES.time_unit);
	PULSES.divide_period(0, 1024);
	en_tuned_sweep_click(0);

	PULSES.fix_global_next();	// just in case?
	PULSES.check_maybe_do();	// maybe do it *first*

	if (MENU.maybe_display_more()) {
	  MENU.ln();
	  selected_or_flagged_pulses_info_lines();
	}
	break;
#endif

      case 20:	// macro
	// was: MENU.play_KB_macro("X E12! .a N *8 J20-.");
	select_in(JIFFLES, arpeggio4096);		// TODO: test order
	MENU.play_KB_macro("X E12! .a N *8 -");
	select_in(JIFFLES, arpeggio4096);		// FIXME: TODO: test order
	break;

      case 21:	// macro
	// was: MENU.play_KB_macro("X E12! .a N *16 J21-.");
	select_in(JIFFLES, arpeggio4096down);		// TODO: test order
	MENU.play_KB_macro("X E12! .a N *16 -.");
	break;

      case 22:	// macro
	select_in(JIFFLES, arpeggio_cont);		// TODO: test order		// :)	with pizzs
	// was: MENU.play_KB_macro("X E12! .a N *16 J22-.");
	MENU.play_KB_macro("X E12! .a N *16 -.");
	break;

      case 23:	// macro
	select_in(JIFFLES, arpeggio_and_down);		// TODO: test order		// :) :)  arpeggio down instead pizzs
	// was: MENU.play_KB_macro("X E12! .a N *16 J23-.");
	MENU.play_KB_macro("X E12! .a N *16 -.");
	break;

      case 24:	// macro
	select_in(JIFFLES, stepping_down);		// TODO: test order		// :) :)  stepping down
	// was: MENU.play_KB_macro("X E12 S=0 ! .a N *16 J24-.");
	MENU.play_KB_macro("X E12 S=0 ! .a N *16 -.");
	break;

      case 25:	// macro
	select_in(JIFFLES, back_to_ground);		// TODO: test order		// rhythm slowdown
	// was: MENU.play_KB_macro("X E12! .a N *32 J25-.");		// :)	rhythm slowdown
	MENU.play_KB_macro("X E12! .a N *32 -.");		// :)	rhythm slowdown
	break;

      case 26:	// macro
	select_in(JIFFLES, arpeggio_and_sayling);		// TODO: test order
	// was: MENU.play_KB_macro("X E12! .a N *32 J26-.");
	MENU.play_KB_macro("X E12! .a N *32 -.");
	break;

      case 27:	// macro
	select_in(JIFFLES, simple_theme);
	MENU.play_KB_macro("X E12! .a N *2 -.");
	break;

      case 28:	// macro   for tuning
	select_in(JIFFLES, peepeep4096);
	MENU.play_KB_macro("X E12! .a N *2 -.");
	break;

      case 29:	// E29 KALIMBA7 tuning
	tabula_rasa();

#if defined KALIMBA7_v2	// ESP32 version  europ_PENTAtonic
	select_in(SCALES, europ_PENTAtonic);
	voices=7;
#else
	select_in(SCALES, pentatonic_minor);	// default, including KALIMBA7_v1
#endif
#if defined  KALIMBA7_v1
	voices=7;
#endif

	musicBoxConf.pitch.multiplier=1;
	musicBoxConf.pitch.divisor=1024;
	PULSES.select_n(voices);
	prepare_scale(false, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync, selected_in(SCALES));
	display_name5pars("E29 KALIMBA7 tuning", g_inverse, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
	en_click_selected();							// for tuning ;)
	PULSES.activate_selected_synced_now(musicBoxConf.sync);	// sync and activate
	MENU.ln();

	if (MENU.verbosity >= VERBOSITY_SOME)
	  selected_or_flagged_pulses_info_lines();

	break;

      case 30:	// KALIMBA7 jiff
	select_in(SCALES, pentatonic_minor);
	voices=7;
	// voices=8;
	musicBoxConf.pitch.multiplier=4;
	musicBoxConf.pitch.divisor=1;
	// select_in(JIFFLES, peepeep4096);
	select_in(JIFFLES, ting4096);
	// select_in(JIFFLES, tingeling4096);
	PULSES.select_n(voices);
	prepare_scale(false, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync, selected_in(SCALES));
	display_name5pars("E30 KALIMBA7 jiff", g_inverse, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
	setup_jiffle_thrower_selected(selected_actions);
	PULSES.activate_selected_synced_now(musicBoxConf.sync);	// sync and activate

	MENU.ln();

	if (MENU.verbosity >= VERBOSITY_SOME)
	  selected_or_flagged_pulses_info_lines();

	break;

      case 31:	// E31 KALIMBA7 jiff
	select_in(SCALES, europ_PENTAtonic);
	voices=7;
	musicBoxConf.pitch.multiplier=4;
	musicBoxConf.pitch.divisor=1;
	select_in(JIFFLES, ting4096);
	PULSES.select_n(voices);
	prepare_scale(false, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync, selected_in(SCALES));
	display_name5pars("E31 KALIMBA7 jiff", g_inverse, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
	setup_jiffle_thrower_selected(selected_actions);
	PULSES.activate_selected_synced_now(musicBoxConf.sync);	// sync and activate;
	MENU.ln();

	if (MENU.verbosity >= VERBOSITY_SOME)
	  selected_or_flagged_pulses_info_lines();

	break;

      case 32:	// ESP32_12 ff
	select_in(SCALES, major_scale);
	voices=GPIO_PINS;
	musicBoxConf.pitch.multiplier=4;
	musicBoxConf.pitch.divisor=1;
	// select_in(JIFFLES, ting4096);
	select_in(JIFFLES, tigg_ding4096);
	PULSES.select_n(voices);
	prepare_scale(false, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync, selected_in(SCALES));
	display_name5pars("E32 ESP32_12", g_inverse, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
	setup_jiffle_thrower_selected(selected_actions);
	PULSES.activate_selected_synced_now(musicBoxConf.sync);	// sync and activate;
	MENU.ln();

	if (MENU.verbosity >= VERBOSITY_SOME)
	  selected_or_flagged_pulses_info_lines();

	break;

      case 33:	// variation
	select_in(SCALES, minor_scale);
	voices=GPIO_PINS;
	musicBoxConf.pitch.multiplier=4;
	musicBoxConf.pitch.divisor=1;
	select_in(JIFFLES, ting4096);
	PULSES.select_n(voices);
	prepare_scale(false, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync, selected_in(SCALES));
	display_name5pars("minor", g_inverse, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
	setup_jiffle_thrower_selected(selected_actions);
	PULSES.activate_selected_synced_now(musicBoxConf.sync);	// sync and activate;
	MENU.ln();

	if (MENU.verbosity >= VERBOSITY_SOME)
	  selected_or_flagged_pulses_info_lines();

	break;

      case 34:	// same, major?
	select_in(SCALES, major_scale);
	voices=GPIO_PINS;
	musicBoxConf.pitch.multiplier=4;
	musicBoxConf.pitch.divisor=1;
	select_in(JIFFLES, ting4096);
	PULSES.select_n(voices);
	prepare_scale(false, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync, selected_in(SCALES));
	display_name5pars("major", g_inverse, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
	setup_jiffle_thrower_selected(selected_actions);
	PULSES.activate_selected_synced_now(musicBoxConf.sync);	// sync and activate;
	MENU.ln();

	if (MENU.verbosity >= VERBOSITY_SOME)
	  selected_or_flagged_pulses_info_lines();

	break;

      case 35:	// tetra
	select_in(SCALES, tetraCHORD);
	voices=GPIO_PINS;
	musicBoxConf.pitch.multiplier=4;
	musicBoxConf.pitch.divisor=1;
	select_in(JIFFLES, ting4096);
	PULSES.select_n(voices);
	prepare_scale(false, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync, selected_in(SCALES));
	display_name5pars("tetra", g_inverse, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
	setup_jiffle_thrower_selected(selected_actions);
	PULSES.activate_selected_synced_now(musicBoxConf.sync);	// sync and activate;
	MENU.ln();

	if (MENU.verbosity >= VERBOSITY_SOME)
	  selected_or_flagged_pulses_info_lines();

	break;

      case 36:	// BIG major
	select_in(SCALES, pentatonic_minor);
	voices=GPIO_PINS;
	musicBoxConf.pitch.multiplier=6;
	musicBoxConf.pitch.divisor=1;
	select_in(JIFFLES, ting4096);
	PULSES.select_n(voices);
	prepare_scale(false, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync, selected_in(SCALES));
	display_name5pars("BIG major", g_inverse, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
	setup_jiffle_thrower_selected(selected_actions);
	PULSES.activate_selected_synced_now(musicBoxConf.sync);	// sync and activate;
	MENU.ln();

	if (MENU.verbosity >= VERBOSITY_SOME)
	  selected_or_flagged_pulses_info_lines();

	break;

      case 37:	// Guitar and other instruments
	tabula_rasa();
	PULSES.time_unit=1000000;

	// default tuning e
	musicBoxConf.pitch.multiplier=4096;
	// musicBoxConf.pitch.divisor=440;			// a4
	musicBoxConf.pitch.divisor=330; // 329.36		// e4  ***not*** harmonical
	// musicBoxConf.pitch.divisor=165; // 164.81		// e3  ***not*** harmonical

	select_in(SCALES, minor_scale);	// default e minor

	select_in(JIFFLES, ting4096);	// default jiffle
	//	voices = 16;			// for DAC output
	if (voices == 0)
	  voices = 15;			// default (diatonic)	// for DAC output

	select_scale__UI();	// second/third letters choose metric scales	// OBSOLETE?:

	// select_in(JIFFLES, ting4096);
	// select_in(JIFFLES, piip2048);
	// select_in(JIFFLES, tanboura); divisor *= 2;

	// ################ FIXME: remove redundant code ################
	PULSES.select_n(voices);
//	prepare_scale(false, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, 0, selected_in(SCALES));
//	display_name5pars("GUITAR", g_inverse, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
	tune_2_scale(voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, selected_in(SCALES));	// TODO: OBSOLETE?
	lower_audio_if_too_high(409600);

  #ifndef USE_DACs	// TODO: review and use test code
	setup_jiffle_thrower_selected(selected_actions);
  #else // *do* use dac		// TODO: not here ################
	selected_share_DACsq_intensity(DAx_max, 1);

    #if (USE_DACs == 1)
	setup_jiffle_thrower_selected(DACsq1);
    #else
	selected_DACsq_intensity_proportional(DAx_max, 2);
//	selected_share_DACsq_intensity(DAx_max, 2);

	setup_jiffle_thrower_selected(DACsq1 | DACsq2);
    #endif
  #endif

	PULSES.activate_selected_synced_now(musicBoxConf.sync);	// sync and activate;
	MENU.ln();

	if (MENU.verbosity >= VERBOSITY_SOME)
	  selected_or_flagged_pulses_info_lines();

	break;

      default:
	if (MENU.verbosity >= VERBOSITY_SOME)
	  MENU.outln_invalid();

	selected_experiment=0;
	break;


      case 38:	// 'E38' time machine setup
	// ESP32_15_clicks_no_display_TIME_MACHINE1

	// if (voices == 0)	// maybe, maybe not...
	voices = 32;	// init *all* primary pulses
	PULSES.select_n(voices);

	PULSES.time_unit=1000000;	// TODO:  maybe, maybe not...

	// default tuning a
	musicBoxConf.pitch.multiplier=4096;	// bass octave added and one lower...
	//	musicBoxConf.pitch.divisor=440;		// a
	//	musicBoxConf.pitch.divisor=220;		// a
	musicBoxConf.pitch.divisor=110;		// a
	// musicBoxConf.pitch.divisor=55;		// a

	// musicBoxConf.pitch.divisor=330; // 329.36		// e4  ***not*** harmonical
	// musicBoxConf.pitch.divisor=165; // 164.81		// e3  ***not*** harmonical

	select_in(SCALES, minor_scale);		// default e minor
	select_scale__UI();	// second/third letters choose metric scales	// OBSOLETE?:

	// tune *all* primary pulses
	tune_2_scale(voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, selected_in(SCALES));	// TODO: OBSOLETE?
	lower_audio_if_too_high(409600);

	// prepare primary pulse groups:
	select_in(JIFFLES, d4096_512);

	// bass on DAC1 and planed broad angle LED lamps
	// select_in(JIFFLES, d4096_512);
	PULSES.select_from_to(0,6);
	for(int pulse=0; pulse<=6; pulse++) {
	  en_jiffle_thrower(pulse, selected_in(JIFFLES), ILLEGAL8, DACsq1);	// FIXME: use inbuilt click
	}
	PULSES.select_from_to(0,7);
	// selected_DACsq_intensity_proportional(DAx_max, 1);
	selected_share_DACsq_intensity(DAx_max, 1);

	// 2 middle octaves on 15 gpios
	// select_in(JIFFLES, d4096_512);
	//	select_in(JIFFLES, d4096_256);
	PULSES.select_from_to(7,6+15);
	setup_jiffle_thrower_selected(0);	// overwrites pulse[7]
	//	setup_jiffle_thrower_selected(DACsq2);
	//	selected_share_DACsq_intensity(DAx_max, 2);

	// fix pulse[7] that belongs to both groups:
	PULSES.pulses[7].dest_action_flags |= DACsq1;

	// high octave on DAC2
	//	select_in(JIFFLES, d4096_64);
	//select_in(JIFFLES, d4096_256);

	PULSES.select_from_to(21, 31);
	for(int pulse=22; pulse<=31; pulse++) {	// pulse[21] belongs to both groups
	  en_jiffle_thrower(pulse, selected_in(JIFFLES), ILLEGAL8, DACsq2);	// FIXME: use inbuilt click
//	  PULSES.pulses[pulse].dest_action_flags |= (DACsq2);
//	  PULSES.set_payload(pulse, &do_throw_a_jiffle);
//	  PULSES.pulses[pulse].data = (unsigned int) jiffle;
	}
	// fix pulse[21] belonging to both groups
	PULSES.pulses[21].dest_action_flags |= DACsq2;
	selected_share_DACsq_intensity(DAx_max, 2);
	//	selected_DACsq_intensity_proportional(DAx_max, 2);

	PULSES.select_n(voices);	// select all primary voices

	// maybe start?
	if(MENU.check_next('!'))		// 'E38!' starts E38
	  PULSES.activate_selected_synced_now(musicBoxConf.sync);	// sync and activate;
	break; // E38

      case 39:	// 'E39' time machine 2 setup
	// #define ESP32_15_clicks_no_display_TIME_MACHINE2
	{ // local scope only right now
	  musicBoxConf.bass_pulses=14;
	  musicBoxConf.middle_pulses=15;
	  musicBoxConf.high_pulses=7;

	  voices = musicBoxConf.bass_pulses + musicBoxConf.middle_pulses + musicBoxConf.high_pulses;	// init *all* primary pulses
	  PULSES.select_n(voices);

	  PULSES.time_unit=1000000;	// default metric
	  musicBoxConf.pitch.multiplier=4096;		// uses 1/4096 jiffles
	  musicBoxConf.pitch.divisor= 294;		// 293.66 = D4	// default tuning D4
	  // musicBoxConf.pitch.divisor= 147;	// 146.83 = D3
	  // musicBoxConf.pitch.divisor=55;	// default tuning a

	  select_in(SCALES, minor_scale);		// default e minor
	  // tune *all* primary pulses
	  tune_2_scale(voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, selected_in(SCALES));	// TODO: OBSOLETE?
	  lower_audio_if_too_high(409600*2);	// 2 bass octaves

	  // prepare primary pulse groups:
	  select_in(JIFFLES, d4096_512);		// default jiffle

	  // bass on DAC1 and planed broad angle LED lamps
	  // select_in(JIFFLES, d4096_512);
	  PULSES.select_from_to(0, musicBoxConf.bass_pulses - 1);
	  for(int pulse=0; pulse<musicBoxConf.bass_pulses; pulse++)
	    en_jiffle_thrower(pulse, selected_in(JIFFLES), ILLEGAL8, DACsq1);	// TODO: FIXME: use inbuilt click
	  PULSES.select_from_to(0,musicBoxConf.bass_pulses);			// pulse[bass_pulses] belongs to both groups
	  // selected_DACsq_intensity_proportional(DAx_max, 1);
	  selected_share_DACsq_intensity(DAx_max, 1);		// bass DAC1 intensity

	  // 2 middle octaves on 15 gpios
	  // select_in(JIFFLES, d4096_512);
	  //	select_in(JIFFLES, d4096_256);
	  PULSES.select_from_to(musicBoxConf.bass_pulses, musicBoxConf.bass_pulses + musicBoxConf.middle_pulses -1);
	  setup_jiffle_thrower_selected(selected_actions=0);		// overwrites topmost bass pulse
									// TODO: 'selected_actions=...' or '|='
	  // fix topmost bass pulse pulse[bass_pulses] that belongs to both groups:
	  PULSES.pulses[musicBoxConf.bass_pulses].dest_action_flags |= DACsq1;

	  // high octave on DAC2
	  //	select_in(JIFFLES, d4096_64);
	  //select_in(JIFFLES, d4096_256);
	  PULSES.select_from_to(musicBoxConf.bass_pulses + musicBoxConf.middle_pulses -1, musicBoxConf.bass_pulses + musicBoxConf.middle_pulses + musicBoxConf.high_pulses -1);
	  for(int pulse = musicBoxConf.bass_pulses + musicBoxConf.middle_pulses; pulse<voices; pulse++) {	// pulse[21] belongs to both groups
	    en_jiffle_thrower(pulse, selected_in(JIFFLES), ILLEGAL8, DACsq2);	// FIXME: use inbuilt click
	  }
	  // fix pulse[21] belonging to both groups
	  PULSES.pulses[musicBoxConf.bass_pulses + musicBoxConf.middle_pulses - 1].dest_action_flags |= DACsq2;
	  selected_share_DACsq_intensity(DAx_max, 2);
	  //	selected_DACsq_intensity_proportional(DAx_max, 2);

	  PULSES.select_n(voices);	// select all primary voices

	  // maybe start?
	  if(MENU.check_next('!'))		// 'E39!' starts E39
	    PULSES.activate_selected_synced_now(musicBoxConf.sync);	// sync and activate, no display
	  else
	    if (DO_or_maybe_display(VERBOSITY_LOWEST))	// maybe ok for here?
	      selected_or_flagged_pulses_info_lines();
	} // case E39 { }
	break;

      case 40:	// 'E40' time machine with icode player   *adapted to musicBox*
	// #define ESP32_15_clicks_no_display_TIME_MACHINE2
	tabula_rasa();
	{ // local scope 'E40' only right now	// TODO: factor out
	  PULSES.time_unit=1000000;	// default metric
	  musicBoxConf.pitch.multiplier=4096;		// uses 1/4096 jiffles
	  musicBoxConf.pitch.multiplier *= 8;	// TODO: adjust appropriate...
	  musicBoxConf.pitch.divisor= 294;		// 293.66 = D4	// default tuning D4
	  // musicBoxConf.pitch.divisor= 147;	// 146.83 = D3
	  // musicBoxConf.pitch.divisor=55;	// default tuning a
	  if(!scale_user_selected)	// see musicBox
	    select_in(SCALES, minor_scale);		// default e minor

	  select_scale__UI();	// second/third letters choose metric scales	// OBSOLETE?:

	  if(!jiffle_user_selected)				// see musicBox
	    select_in(JIFFLES, d4096_512);		// default jiffle

	  if(!icode_user_selected) {	// see musicBox
#if defined USE_i2c
	    select_in(iCODEs, (unsigned int*) d4096_1024_i2cLED);
#else
	    select_in(iCODEs, (unsigned int*) d4096_1024_icode_jiff);
#endif
	  }

	  setup_bass_middle_high(musicBoxConf.bass_pulses, musicBoxConf.middle_pulses, musicBoxConf.high_pulses);

	  // maybe start?
	  if(MENU.check_next('!'))		// 'E40!' starts E40
	    PULSES.activate_selected_synced_now(musicBoxConf.sync);	// sync and activate, no display
	  else
	    if (DO_or_maybe_display(VERBOSITY_LOWEST))	// maybe ok for here?
	      selected_or_flagged_pulses_info_lines();
	} // case 'E40' { }
	break;


      } // switch (selected_experiment)
    } // if experiment >= 0
} // old_style_experiments_menu_reaction()


#ifndef RAM_IS_SCARE	// enough RAM?
const char * experiment_names[] = {		// FIXME: const char * experiment_names would be better
      "(invalid)",				// 0
      "setup_jiffle128",			// 1
      "init_div_123456",			// 2
      "ESP8266 Frogs",				// 3
      "setup_jiffles2345",			// 4
      "init_123456",				// 5
      "init_chord_1345689a",			// 6
      "init_rhythm_1",				// 7
      "init_rhythm_2",				// 8
      "init_rhythm_3",				// 9
      "init_rhythm_4",				// 10
      "setup_jifflesNEW",			// 11
      "init_pentatonic",			// 12
      "magnets: The Harmonical Strings Christmas Evening Sounds",  // 13
      "magnets on strings 2",			// 14
      "magnets on strings 3",			// 15
      "piezzos on low strings 2016-12-28",	// 16
      "magnets on steel strings, japanese",	// 17
      "nylon stringed wooden box, piezzos",	// 18
      "tuned sweep",				// 19
      "arpeggio4096\tpentatonic",		// 20
      "arpeggio4096down\tpentatonic",		// 21
      "arpeggio_cont\tpentatonic",		// 22
      "arpeggio_and_down\tpentatonic",		// 23
      "stepping_down\tpentatonic",		// 24
      "back_to_ground\tpentatonic rhythm slowdown", // 25
      "arpeggio_and_sayling\tpentatonic",	// 26
      "simple_theme\tpentatonic",		// 27
      "peepeep4096\tpentatonic\tfor tuning",	// 28
      "KALIMBA7 tuning",			// 29
      "KALIMBA7 jiff pent minor",		// 30
      "KALIMBA7 jiff pent euro",		// 31
      "ESP32_12 pent euro",			// 32
      "minor scale",				// 33
      "major scale",				// 34
      "tetraCHORD",				// 35
      "more voices please",			// 36
      "Guitar and other Instruments",		// 37
      "Time Machine 1",				// 38
      "Time Machine 2",				// 39
      "Time Machine iCodeplayer",		// 40
//    "(invalid)",				// over
  };

  #define n_experiment_names (sizeof (experiment_names) / sizeof (const char *))
#endif // ! RAM_IS_SCARE


void old_style_experiments_exclamation_mark_menu_reaction() {
    switch (selected_experiment) {	// setup some old style experiments:
    case -1:
    case 0:
      break;
    case 1:
      setup_jiffle128(g_inverse, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
      break;
    case 2:
      init_div_123456(g_inverse, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
      break;
    case 3:
      setup_jiffles0(g_inverse, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);    // ESP8266 Frog Orchester
      break;
    case 4:
      setup_jiffles2345(g_inverse, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
      break;
    case 5:
      init_123456(g_inverse, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
      break;
    case 6:
      init_chord_1345689a(g_inverse, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
      break;
    case 7:
      init_rhythm_1(g_inverse, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
      break;
    case 8:
      init_rhythm_2(g_inverse, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
      break;
    case 9:
      init_rhythm_3(g_inverse, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
      break;
    case 10:
      init_rhythm_4(g_inverse, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
      break;
    case 11:
      setup_jifflesNEW(g_inverse, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
      break;

    default:	// normal use case: sync and activate
      PULSES.activate_selected_synced_now(musicBoxConf.sync); // sync and activate

      if (DO_or_maybe_display(VERBOSITY_HIGH)) {
	MENU.ln();
	selected_or_flagged_pulses_info_lines();
      }
//      break;
    }
} // old_style_experiments_exclamation_mark_menu_reaction()


#define OLD_STYLE_EXPERIMENTS_H
#endif
