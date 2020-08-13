/*
  musicBox.h
*/

#include "musicBox_config.h"	// included in the configuration sequence
#include "my_pulses_config.h"

/* **************************************************************** */
// some DEFAULTs, setups might change them

#if ! defined MUSICBOX_WHEN_DONE_FUNCTION_DEFAULT
  //#define  MUSICBOX_WHEN_DONE_FUNCTION_DEFAULT	&deep_sleep	// test for dac noise...  BT checks BLUETOOTH_ENABLE_PIN on boot
  //#define  MUSICBOX_WHEN_DONE_FUNCTION_DEFAULT	&light_sleep	// fine as default for triggered musicBox  bluetooth does *not* wake up
  //#define  MUSICBOX_WHEN_DONE_FUNCTION_DEFAULT	&restart	// endless loop
  //#define  MUSICBOX_WHEN_DONE_FUNCTION_DEFAULT	&ESP.restart	// works fine
  //#define  MUSICBOX_WHEN_DONE_FUNCTION_DEFAULT	&hibernate	// wakes up after musicBox_pause_seconds  BT should work, test
  #define  MUSICBOX_WHEN_DONE_FUNCTION_DEFAULT		&user		// works fine, a possible snoring workaround on usb dac only models
  //#define  MUSICBOX_WHEN_DONE_FUNCTION_DEFAULT	&random_preset	// muzak forever? >>>RANDOM_PRESET_LOOP sets this automagically <<<
#endif

/* **************************************************************** */
// pre defined SETUPS:

#if defined SETUP_PORTABLE_DAC_ONLY

  #define PERIPHERAL_POWER_SWITCH_PIN		12	// *pseudo* for green LED,  switch power, often green LED
  #define PROGRAM_SUB_VERSION			portable 3D	// with morse and 3D accGyro UI
  #if ! defined MAX_SUBCYCLE_SECONDS
    #define MAX_SUBCYCLE_SECONDS		60*18	// *max seconds*, produces short PRESET PIECES	   portable instruments 2019-06
  #endif
//#define MUSICBOX_HARD_END_SECONDS		60*100	// SAVETY NET shut down after 100'	***DEACTIVATED***
  #define MUSICBOX_TRIGGER_BLOCK_SECONDS	3600*12	// *DEACTIVATED*    TODO: should go to musicBoxConf or HARDWARE
  #define SOFT_END_DAYS_TO_LIVE_DEFAULT		1	// quite fast ending
  #undef RANDOM_PRESET_LOOP				// just in case, does not work well together	TODO: TEST: ################
  #define MUSICBOX_WHEN_DONE_FUNCTION_DEFAULT	&user	// new default for *portable* instruments
//  #define MUSICBOX_WHEN_DONE_FUNCTION_DEFAULT	&random_preset
//  #define MUSICBOX_TRIGGER_PIN			34	// activates trigger pin, needs pulldown (i.e. 470k)
  #define MAGICAL_TOILET_HACK_2	// continue using (parts of) setup_bass_middle_high() to setup musicbox
  #undef AUTOSTART
  #define AUTOSTART	play_random_preset();		// same as pulses_project_conf.h

#elif defined SETUP_BRACHE_TRIGGERED_PRESETs
//#define BATTERY_LEVEL_CONTROL_PIN		36	// triggers battery control
  #define PERIPHERAL_POWER_SWITCH_PIN		12	// switch power, often green LED
  #define PROGRAM_SUB_VERSION			TRIGGERED_PLAYER
  #define MAX_SUBCYCLE_SECONDS			120	// *max seconds*, PRODUCES *VERY SHORT PRESET PIECES*	BRACHE 2019-04
  #define MUSICBOX_HARD_END_SECONDS		5*60	// SAVETY NET
//#define MUSICBOX_TRIGGER_BLOCK_SECONDS	30	// *DEACTIVATED* BRACHE 2019-01 FIXME: retriggering while playing is buggy
  #define MUSICBOX_TRIGGER_BLOCK_SECONDS	3600	// *DEACTIVATED* BRACHE 2019-04 FIXME: retriggering while playing is buggy
  #define SOFT_END_DAYS_TO_LIVE_DEFAULT		1	// quite fast ending
  #undef RANDOM_PRESET_LOOP				// just in case, does not work well together
  #undef MUSICBOX_WHEN_DONE_FUNCTION_DEFAULT
  #define MUSICBOX_WHEN_DONE_FUNCTION_DEFAULT	&deep_sleep	// do test for dac noise...	BT checks BLUETOOTH_ENABLE_PIN on boot
  #define MUSICBOX_TRIGGER_PIN			34	// activates trigger pin, needs pulldown (i.e. 470k)
//#define MAGICAL_TOILET_HACKS	// some quick dirty hacks around fake triggering
  #define MAGICAL_TOILET_HACK_2	// continue using (parts of) setup_bass_middle_high() to setup musicbox
  #undef AUTOSTART
  #define AUTOSTART	play_random_preset();		// same as pulses_project_conf.h

#elif defined SETUP_BRACHE
//#define BATTERY_LEVEL_CONTROL_PIN		36	// triggers battery control
  #define PERIPHERAL_POWER_SWITCH_PIN		12	// switch power, often green LED
  #define PROGRAM_SUB_VERSION			SETUP_BRACHE
  #define MAX_SUBCYCLE_SECONDS	5*60		// *max seconds*, produce short sample pieces	BRACHE 2019-01
  #define MUSICBOX_HARD_END_SECONDS		10*60	// SAVETY NET, we're low on energy...
  #define MUSICBOX_TRIGGER_BLOCK_SECONDS	30	// BRACHE 2019-01
  #define SOFT_END_DAYS_TO_LIVE_DEFAULT		0	// fast ending, as there are more people now that it get's warmer
  #if ! defined MUSICBOX_WHEN_DONE_FUNCTION_DEFAULT
    #define MUSICBOX_WHEN_DONE_FUNCTION_DEFAULT	&deep_sleep	// do test for dac noise...	BT checks BLUETOOTH_ENABLE_PIN on boot
  #endif
  #define MUSICBOX_TRIGGER_PIN			34	// activates trigger pin, needs pulldown (i.e. 470k)
  #define MUSICBOX_TRIGGERED_FUNCTION		start_musicBox();
  #define MAGICAL_TOILET_HACKS	// some quick dirty hacks around fake triggering

#elif defined SETUP_BAHNPARKPLATZ
  #define PROGRAM_SUB_VERSION			SETUP_BAHNPARKPLATZ
  #define MAX_SUBCYCLE_SECONDS	12*60		// *max seconds*, produce sample pieces		BahnParkPlatz 18
  #define MUSICBOX_TRIGGER_BLOCK_SECONDS	13	// BahnParkPlatz
  // #define MUSICBOX_TRIGGER_PIN			34	// activates trigger pin, needs pulldown (i.e. 470k)

#elif defined SETUP_CHAMBER_ORCHESTRA
  #define PROGRAM_SUB_VERSION			SETUP_CHAMBER_ORCHESTRA

//#define BATTERY_LEVEL_CONTROL_PIN		36	// triggers battery control	maybe?

  //#define MAX_SUBCYCLE_SECONDS	21*60		// *max seconds*, produce sample pieces   The Harmonical Chambre Orchestra
  #define MAX_SUBCYCLE_SECONDS		12*60		// *max seconds*, produce sample pieces   The Harmonical Chambre Orchestra
  //#define MAX_SUBCYCLE_SECONDS	7*60		// DEBUG *max seconds*, produce sample pieces   The Harmonical Chambre Orchestra

  #define MUSICBOX_TRIGGER_PIN			34	// activates trigger pin, needs pulldown (i.e. 470k)
  #define MUSICBOX_TRIGGER_BLOCK_SECONDS	1	// debounce only

#endif	// (pre defined setups)

// additional feature	// TODO: MOVE: where appropriate
#if defined RANDOM_PRESET_LOOP
  #undef  MUSICBOX_WHEN_DONE_FUNCTION_DEFAULT
  #define  MUSICBOX_WHEN_DONE_FUNCTION_DEFAULT	&random_preset	// muzak forever?
#endif

#if ! defined MAX_SUBCYCLE_SECONDS
  #define MAX_SUBCYCLE_SECONDS		7*60	// *max seconds*, produce short sample pieces	BRACHE 2019-01
  //#define MAX_SUBCYCLE_SECONDS	12*60	// *max seconds*, produce sample pieces		BahnParkPlatz 18
  //#define MAX_SUBCYCLE_SECONDS	18*60	// *max seconds*, produce moderate length sample pieces  DEFAULT
  //#define MAX_SUBCYCLE_SECONDS	65*60	// *max seconds*, sets performance timing based on cycle
#endif


#include <esp_sleep.h>
// #include "rom/gpio.h"
// #include "driver/gpio.h"
// #include <bt/bluedroid/api/include/api/esp_bt_main.h>
#include <esp_bt_main.h>
#include <esp_bt.h>
#include "esp_wifi.h"
#include "WiFi.h"
#include "driver/rtc_io.h"
#include "driver/dac.h"

#include "random_entropy.h"

#if defined BATTERY_LEVEL_CONTROL_PIN
  #include "battery_control.h"
#endif


typedef struct magical_conf_t {
  void (*musicBox_when_done)(void)=NULL;
  int octave_shift=0;
  int soft_cleanup_minimal_fraction_weighting=1;	// TODO: adjust default
  unsigned long soft_end_cleanup_wait=60*1000000L;	// default 60"	// TODO: rethink ################
  int autostack_S0=AUTOSTACK_S0_DEFAULT;

#if ! defined SOFT_END_DAYS_TO_LIVE_DEFAULT
  #define SOFT_END_DAYS_TO_LIVE_DEFAULT		1	// default
#endif
  unsigned short soft_end_days_to_live=SOFT_END_DAYS_TO_LIVE_DEFAULT;

  unsigned short soft_end_survive_level = 4;		// the level a pulse must have reached to survive soft end

  bool magic_autochanges=true;		// TODO: switch that logic
  bool some_metric_tunings_only=false;	// free or fixed pitchs (like E A D G C F B) *only*

  bool autoskip_pause = false;		// TODO: UI
  bool no_octave_shift = false;

} magical_conf_t;

magical_conf_t MagicConf;


typedef struct ui_conf_t {
  uint8_t version = 0;	// 0 means currently under development
  // voices=0;	// DADA: include voices?	maybe not?
  // TODO: include selection ???	I think YES	DADA

  bool show_cycle_pattern=false;
  bool show_cycle_pattern_intervals=false;
  bool show_subcycle_position=false;

  bool subcycle_user_selected=false;

  bool scale_user_selected = false;
  bool sync_user_selected = false;
  bool stack_sync_user_selected = false;
  bool jiffle_user_selected = false;
  bool pitch_user_selected = false;	// TODO: ################

} ui_conf_t;

ui_conf_t uiConf;


#if defined USE_MONOCHROME_DISPLAY
  #include "monochrome_display.h"
#endif

#if ! defined MAX_SUBCYCLE_SECONDS
  #define MAX_SUBCYCLE_SECONDS	0	// default: off, use whole harmonical cycle
#endif
int max_subcycle_seconds=MAX_SUBCYCLE_SECONDS;

#if defined PERIPHERAL_POWER_SWITCH_PIN
  #include "peripheral_power_switch.h"
#endif

#if defined MUSICBOX_TRIGGER_PIN
  #if ! defined MUSICBOX_TRIGGER_BLOCK_SECONDS
    #define MUSICBOX_TRIGGER_BLOCK_SECONDS	3	// just playing, DEBUGGING	// TODO: not flexible enough
  #endif
#endif

// TODO: FIXME: MUSICBOX_HARD_END_SECONDS		// savety net	################
#if ! defined MUSICBOX_HARD_END_SECONDS		// savety net
  #if defined MAX_SUBCYCLE_SECONDS && MAX_SUBCYCLE_SECONDS > 0
    // should be ok
    // or try	#define  MUSICBOX_HARD_END_SECONDS	(MAX_SUBCYCLE_SECONDS*2)
  #else
    #warning *no* MUSICBOX_HARD_END_SECONDS
  #endif
#endif


//int musicBox_pause_seconds=10;	// SEE: hibernate(), restart(), random_preset()
int musicBox_pause_seconds=18;		// DADA changed that ################
void delay_pause() {	// TODO; FIXME: (i.e. morse input...)
  if(musicBox_pause_seconds) {
    if(MENU.verbosity >= VERBOSITY_LOWEST); {
      MENU.out(F("restarting after "));
      MENU.out(musicBox_pause_seconds);
      MENU.outln(F(" seconds"));
    }
    delay(musicBox_pause_seconds*1000);	// *not* meant for any background activity to go on	// FIXME: (i.e. morse input...)
  }
}


void start_musicBox();		// forward declaration
void play_random_preset();	// pre declaration	see: AUTOSTART

// functions to call when musicBox has reached the end:
void deep_sleep();		// pre declaration
void light_sleep();		// pre declaration

void restart() {	// alias, pause and loop, endlessly
  delay_pause();
}

void user() {	// manual musicBox interaction
  MENU.outln(F("menu interaction"));
}

void random_preset() {	// TODO: sets preset, how to unset?
  delay_pause();
  MENU.play_KB_macro(F("y0"));		// start random preset
}

void hibernate() {	// see: https://esp32.com/viewtopic.php?t=3083
  if(MENU.verbosity >= VERBOSITY_LOWEST) {
    MENU.outln(F("hibernate()"));
    MENU.out(F("restarting after "));
    MENU.out(musicBox_pause_seconds);
    MENU.outln(F(" seconds"));
  }

  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
  esp_sleep_enable_timer_wakeup(1000000 * musicBox_pause_seconds);
  esp_deep_sleep_start();
}


// void (*musicBox_when_done)(void)=&deep_sleep;	// function* called when musicBox ends
void (*musicBox_when_done)(void)=MUSICBOX_WHEN_DONE_FUNCTION_DEFAULT;	// function* called when musicBox ends

void show_when_done_function() {
  MENU.out(F("when done do: "));

  if(musicBox_when_done == &deep_sleep)
    MENU.out("deep_sleep");
  else if(musicBox_when_done == &light_sleep)
    MENU.out("light_sleep");
  else if(musicBox_when_done == &hibernate) {
    MENU.out("pause(");
    MENU.out(musicBox_pause_seconds);
    MENU.out("); hibernate");
  } else if(musicBox_when_done == &restart) {
    MENU.out("pause(");
    MENU.out(musicBox_pause_seconds);
    MENU.out("); restart");
  } else if(musicBox_when_done == &user) {
    MENU.out("user");
  } else if(musicBox_when_done == & random_preset) {
    MENU.out("pause(");
    MENU.out(musicBox_pause_seconds);
    MENU.out("); random_preset");
  } else
    MENU.out("(unknown)");

  MENU.out(F("();"));
}


// TODO: musicBox_runtime_data_t block?
pulse_time_t musicBox_start_time;
pulse_time_t musicBox_hard_end_time;

/*
  select something like
   unsigned short cycle_slices = 72;	// test&adapt   classical aesthetics?
   unsigned short cycle_slices = 120;	// test&adapt   classical aesthetics?
   unsigned short cycle_slices = 90*3;	// test&adapt   simplified keeping important details?
   unsigned short cycle_slices = 180*3;	// test&adapt	interesting lot of detail
   unsigned short cycle_slices = 180;	// test&adapt	sometimes less is more
   unsigned short cycle_slices = 360;	// test&adapt   classical aesthetics?
*/
unsigned short cycle_slices = 540;	// *DO NOT CHANGE DIRECTLY* use set_cycle_slice_number(n);
//unsigned short cycle_slices = 540*2;	// *DO NOT CHANGE DIRECTLY* use set_cycle_slice_number(n);
//unsigned short cycle_slices = 540*3;	// *DO NOT CHANGE DIRECTLY* use set_cycle_slice_number(n);

pulse_time_t slice_tick_period;	// *DO NOT SET DIRECTLY* use set_cycle_slice_number(n);

void set_cycle_slice_number(short ticks_a_cycle) {
  cycle_slices = ticks_a_cycle;
  slice_tick_period = CyclesConf.used_subcycle;
  PULSES.div_time(&slice_tick_period, cycle_slices);
}

// remember pulse index of the butler, so we can call him, if we need him ;)
int musicBox_butler_i=ILLEGAL32;	// pulse index of musicBox_butler(p)

// some pre declarations:
void musicBox_butler(int);


// MusicBoxState
enum musicbox_state_t {OFF=0, ENDING, SLEEPING, SNORING, AWAKE, FADE};
musicbox_state_t MusicBoxState=OFF;
char * MusicBoxState_name;
void set_MusicBoxState(musicbox_state_t state) {	// sets the state unconditionally
  int butler_survivors=0;	// checks for stray butler pulses when state switched to OFF

  switch (state) {			// initializes state if necessary
  case OFF:
    MusicBoxState_name = F("OFF");

#if defined USE_RGB_LED_STRIP	// TODO: more flexibility, like fading	DADA
    digitalLeds_resetPixels(&strands[0], 1);
#endif

    // control if the butler is still running || musicBox_butler_i != ILLEGAL32
    if(musicBox_butler_i != ILLEGAL32) {	// musicBox_butler(p) seems running?
      if(PULSES.pulses[musicBox_butler_i].payload == &musicBox_butler) {
	PULSES.init_pulse(musicBox_butler_i);
	if(MENU.maybe_display_more(VERBOSITY_MORE))
	  MENU.outln(F("set_MusicBoxState: butler removed"));	// butler still running on musicBox_butler_i
      } else {
	if(MENU.maybe_display_more(VERBOSITY_SOME)) {
	  MENU.out(F("butler not at id "));			// musicBox_butler_i was set, but no butler there
	  MENU.outln(musicBox_butler_i);
	}
      }

      musicBox_butler_i = ILLEGAL32;				// invalidate pulse index of musicBox_butler(p)
    }

    // test if there is really no butler left:
    for(int p=0; p<PL_MAX; p++) {
      if(PULSES.pulses[musicBox_butler_i].payload == &musicBox_butler)
	butler_survivors++;	// checks for stray butler pulses when state switched to OFF
    }
    if(butler_survivors) {
      if(MENU.maybe_display_more(VERBOSITY_LOWEST)) {
	MENU.out(F("set_MusicBoxState: surviving butlers: "));	// strange, made by user interaction?
	MENU.outln(butler_survivors);
      }
    }

    break; // OFF

  case ENDING:
    MusicBoxState_name = F("ENDING");
    break;
  case SLEEPING:
    MusicBoxState_name = F("SLEEPING");
    break;
  case SNORING:
    MusicBoxState_name = F("SNORING");
    break;
  case AWAKE:
    MusicBoxState_name = F("AWAKE");
    break;
  case FADE:
    MusicBoxState_name = F("FADE");
    break;
  default:
    MusicBoxState_name = F("(unknown)");
    MENU.outln(F("unknown MusicBoxState"));	// should not happen ;)
  }
  MusicBoxState = state;		// OK

  if(MENU.maybe_display_more(VERBOSITY_SOME)) {
    MENU.out(F("MusicBoxState "));
    MENU.outln(MusicBoxState_name);
  }
}

bool musicbox_is_idle() {	// makes it easier to pre declare that from outside
  return MusicBoxState == OFF;
}

bool musicbox_is_ending() {	// makes it easier to pre declare that from outside
  return MusicBoxState == ENDING;
}

bool musicbox_is_awake() {	// makes it easier to pre declare that from outside
  return MusicBoxState == AWAKE;
}


void tabula_rasa() {
  if (MENU.verbosity > VERBOSITY_LOWEST) {
    MENU.out(F("tabula rasa\t"));
    MENU.print_free_RAM();
    MENU.tab();
  }

  reset_all_flagged_pulses_GPIO_OFF();
  if(MusicBoxState != OFF)	// avoid possible side effects
    set_MusicBoxState(OFF);

  musicBoxConf.date=NULL;	// TODO: TEST: hmm?
  if (MENU.verbosity > VERBOSITY_LOWEST) {
    MENU.print_free_RAM();
    MENU.ln();
  }

  //reset_timer64();  // restart timer64?
  //PULSES.get_now();
} // tabula_rasa()


// TODO: magic_autochanges default?
bool magic_autochanges=true;	// TODO: bitvector loweraudio, subcycle, .... metric

unsigned long primary_counters[PL_MAX] = { 0 };	// preserve last seen counters

void init_primary_counters() {
  memset(&primary_counters, 0, sizeof(primary_counters));
}

#if ! defined SHOW_CYCLE_PATTERN_DEFAULT
  #define SHOW_CYCLE_PATTERN_DEFAULT	false
#endif

bool show_cycle_pattern=false;
bool show_cycle_pattern_intervals=false;

short primary_count=0;	// TODO: use musicBoxConf.primary_count in next version

void set_primary_block_bounds() {	// remember where the primary block starts and stops
  musicBoxConf.lowest_primary=ILLEGAL16;
  musicBoxConf.highest_primary=ILLEGAL16;
  primary_count=0;	// TODO: use musicBoxConf.primary_count in next version

  for(int pulse=0; pulse<PL_MAX; pulse++) {
    if(PULSES.pulses[pulse].groups & g_PRIMARY) {
      primary_count++;

      if(musicBoxConf.lowest_primary == ILLEGAL16)
	musicBoxConf.lowest_primary = pulse;		// first primary pulse seen
      else
	musicBoxConf.highest_primary = pulse;	// last primary pulse seen
    }
  }

 if(MENU.verbosity >= VERBOSITY_MORE) {
    MENU.out(primary_count);	// TODO: use musicBoxConf.primary_count in next version
    if(primary_count) {
      MENU.out(" primaries from ");
      MENU.out(musicBoxConf.lowest_primary);
      MENU.out(" to ");
      MENU.outln(musicBoxConf.highest_primary);
    } else
      MENU.outln(F("no primary pulses"));
  }
} // set_primary_block_bounds()


int highest_primary_at_start=ILLEGAL32;	// needed for primary pattern display in watch_primary_pulses()

void watch_primary_pulses() {
  long diff;
  int primary_cnt=0;	// keep track (for display only)

  for(int pulse=highest_primary_at_start; pulse>=musicBoxConf.lowest_primary; pulse--) {
    if(PULSES.pulses[pulse].groups & g_PRIMARY) {
      primary_cnt++;
      diff = PULSES.pulses[pulse].counter - primary_counters[pulse];	// has the counter changed?
      primary_counters[pulse]=PULSES.pulses[pulse].counter;		// update to new counter

      if(show_cycle_pattern) {
	if(diff == 0) {		// no change
	  if(PULSES.pulses[pulse].flags & COUNTED)
	    MENU.out(PULSES.pulses[pulse].remaining);	// counted pulse waiting for it's turn	 no change
	  else
	    MENU.space();	// no change
	} else {		// change (any)
	  if(diff == 1) {
	    if(show_cycle_pattern_intervals) {		// show interval symbols?
	      if(PULSES.pulses[pulse].groups & g_OCTAVE)
		MENU.out('^');	// '^' OCTAVE was alive *once*
	      else if(PULSES.pulses[pulse].groups & g_FOURTH)
		MENU.out('{');	// '{' FOURTH was alive *once*
	      else if(PULSES.pulses[pulse].groups & g_FIFTH)
		MENU.out('}');	// '}' FIFTH was alive *once*
	      else
		MENU.out('*');	// '*' was alive *once*
	    } else {					// simple all stars pattern :)
	      if(PULSES.pulses[pulse].action_flags & noACTION)
		MENU.out('.');	// '.' action is muted, was alive *once*
	      else
		MENU.out('*');	// '*' was alive *once*
	    }
	  } else if(diff > 1) {
	    if (diff < 16)
	      MENU.out_hex_chiffre(diff);
	    else	// slice too long to display individual wakeup counts
	      MENU.out('#');
	  } else		// diff negative??? (could possibly happen after some reset?)
	    MENU.out(diff);	// DEBUGGING: catch that, if it ever happens...
	} // counter changed
      } // show pattern

    } else	// is *not* g_PRIMARY
      MENU.out('-');	// was removed, replaced or something
  } // primary pulse loop (high to low)
  MENU.out('\'');	// mark end of primary pulses by "'  "
  MENU.space(2);

  // near the end  show secondary_cnt
  if((primary_cnt <= 4) || (MENU.verbosity >= VERBOSITY_SOME)) {	// test&trimm: 4
    int secondary_cnt=0;

    for(int pulse=0; pulse < PL_MAX; pulse++) {
      if(PULSES.pulses[pulse].groups & g_SECONDARY)
	secondary_cnt++;
    }

    MENU.out(F("S("));
    MENU.out(secondary_cnt);
    MENU.out(F(")\t"));
  }
} // watch_primary_pulses()

void show_cycles_1line() {	// no scale, no cycle
  if(selected_in(SCALES)==NULL) {
    MENU.outln(F("no scale, no cycle"));
    return;
  } // else

  // this version assumes harmonical_CYCLE and CyclesConf.used_subcycle *are* set
  MENU.out(F("harmonical CYCLE: "));
  PULSES.display_time_human(CyclesConf.harmonical_CYCLE);
  MENU.out(F("\t2^"));
  MENU.out(CyclesConf.subcycle_octave);
  MENU.out(F(" SUBCYCLE: | "));
  PULSES.display_time_human(CyclesConf.used_subcycle);
  MENU.outln('|');

  // TODO: show current position (float)
  return;
}

void show_cycle(pulse_time_t cycle) {
  MENU.out(F("\nharmonical cycle:  "));
  if(selected_in(SCALES)==NULL) {
    MENU.outln(F("no scale, no cycle"));
    return;
  }

  set_primary_block_bounds();	// remember where the primary block starts and stops

  PULSES.display_time_human(cycle);
  MENU.ln();

  pulse_time_t period_highest = PULSES.pulses[musicBoxConf.highest_primary].period;
  pulse_time_t shortest = scale2harmonical_cycle(selected_in(SCALES), &period_highest);

  if(MENU.verbosity >= VERBOSITY_MORE) {
    MENU.out(F("shortest pulse's harmonical cycle: "));
    PULSES.display_time_human(shortest);
  }
  MENU.ln();

#if defined PULSES_USE_DOUBLE_TIMES
  MENU.outln(F("todo: maybe give subcycle infos?"));	// TODO:

#else // old int overflow style
  int i=0;
  //                                   !!!  a tolerance of 128 seemed *not* to be enough
  while(cycle.time >= (CyclesConf.used_subcycle.time - 256/*tolerance*/) || \
	cycle.time >= (shortest.time - 256/*tolerance*/) || \
	cycle.overflow)		// display cycle and relevant octaves
    {
      if(cycle.time == CyclesConf.used_subcycle.time)
	MENU.out('|');

      MENU.out(F("(2^"));
      MENU.out(i--);
      MENU.out(F(") "));
      PULSES.display_time_human(cycle);

      if(cycle.time == CyclesConf.used_subcycle.time)
	MENU.out('|');

      if(i % 4)	// 4 items a line
	MENU.tab();
      else
	MENU.ln();

      PULSES.div_time(&cycle, 2);
    }

  if(i % 4)	// 4 items a line
    MENU.ln();
#endif // old int overflow style
} // show_cycle()


int slice_weighting(Harmonical::fraction_t F) {
  int weighting=0;

  if(F.multiplier<9)
    if(F.divisor<=128)
      weighting += 9 - F.multiplier;

  if(F.divisor<9)
    weighting += 9 - F.divisor;

  switch(F.divisor) {	// octaves get higher ratings
  case 1:
    weighting++;
  case 2:
    weighting++;
  case 4:
    weighting++;
  case 8:
    weighting++;
  }

  switch(F.divisor) {	// some divisors get higher ratings
  case 6:
    weighting++;
    break;
  case 9:
    weighting++;
    break;
  case 10:
    weighting++;
    break;
  case 12:
    weighting++;
    weighting++;
    break;
  case 15:
    weighting++;
    break;
  case 20:
    weighting++;
    break;
  case 24:
    weighting++;
    break;
  case 30:
    weighting++;
    break;
  case 36:
    weighting++;
    break;
  case 45:
    weighting++;
    break;
  }

  if(F.multiplier + 1 == F.divisor)
    if(F.divisor <= 128)
      weighting++;

  switch(F.multiplier) {	// octaves get higher ranking
  case 1: weighting++;
    if(F.divisor>128) {		// but less so, if divisor is too large
      break;
    }
  case 2: weighting++;
    if(F.divisor>64) {		// ...
      break;
    }
  case 4: weighting++;
    if(F.divisor>32) {
      break;
    }
  case 8: weighting++;
  case 16: weighting++;
  case 32: weighting++;
  case 64: weighting++;
  }

  if((*HARMONICAL).is_small_prime(F.multiplier))
    weighting++;

  return weighting;
}


// cycle_monitor(p)  payload to give infos where in the cycle we are
int cycle_monitor_i=ILLEGAL32;	// pulse index of cycle_monitor(p)

#if ! defined SHOW_SUBCYCLE_POSITION_DEFAULT
  #define SHOW_SUBCYCLE_POSITION_DEFAULT	false
#endif
bool show_subcycle_position=SHOW_SUBCYCLE_POSITION_DEFAULT;

void cycle_monitor(int pulse) {	// show markers at important cycle divisions
  static unsigned short cycle_monitor_last_seen_division=0;
  if(PULSES.pulses[pulse].counter == 1)
    cycle_monitor_last_seen_division =  0;

  pulse_time_t this_time = PULSES.get_now();
  PULSES.sub_time(&PULSES.pulses[pulse].last, &this_time);	// so long inside this cycle
  /* TESTED: works fine disregarding overflow :)
  if(this_time.overflow != PULSES.pulses[pulse].last.overflow)
    MENU.outln(F("over"));
  */
  // Harmonical::fraction_t phase = {this_time.time, PULSES.pulses[pulse].period.time};
  // float float_phase = this_time.time / PULSES.pulses[pulse].period.time;	// not used
  Harmonical::fraction_t this_division = {cycle_monitor_last_seen_division, cycle_slices};
  (*HARMONICAL).reduce_fraction(&this_division);

  if(show_subcycle_position /*&& slice_weighting(this_division) > 0*/) {	// weighting influence switched off
    MENU.out((float) this_division.multiplier/this_division.divisor, 6);
    MENU.space(2);

    if(this_division.multiplier<10000)
      MENU.space();
    if(this_division.multiplier<1000)
      MENU.space();
    if(this_division.multiplier<100)
      MENU.space();
    if(this_division.multiplier<10)
      MENU.space();
    MENU.out(this_division.multiplier);
    MENU.out('/');
    MENU.out(this_division.divisor);
    if(this_division.divisor<10)
      MENU.space();
    if(this_division.divisor<100)
      MENU.space();
    if(this_division.divisor<1000)
      MENU.space();
    if(this_division.divisor<10000)
      MENU.space();
    MENU.space();

    if(MENU.verbosity >= VERBOSITY_SOME) {
      MENU.out(F("\tw="));
      MENU.out(slice_weighting(this_division));
      MENU.out(F("\tnow="));
      PULSES.display_time_human(PULSES.now);
    }
    if(PULSES.tuning != 1.0) {
      MENU.tab();
      MENU.out(PULSES.tuning,6);
    }
  }

  cycle_monitor_last_seen_division++;
  cycle_monitor_last_seen_division %= cycle_slices;
} // cycle_monitor()


unsigned int kill_secondary() {	// kill all secondary
  unsigned int cnt=0;

  for(int pulse=0; pulse < PL_MAX; pulse++) {
    if(PULSES.pulses[pulse].groups & g_SECONDARY) {
      if(PULSES.pulses[pulse].flags & HAS_GPIO)	{	// maybe set GPIO low?
	if(PULSES.pulses[pulse].gpio != ILLEGAL8)
	  digitalWrite(PULSES.pulses[pulse].gpio, LOW);
      }
      PULSES.init_pulse(pulse);				// remove all secondary pulses
      cnt++;
    }
  }
  return cnt;
}


unsigned int kill_primary() {
  unsigned int cnt=0;

  for(int pulse=0; pulse < PL_MAX; pulse++) {
    if(PULSES.pulses[pulse].groups & g_PRIMARY) {
      PULSES.init_pulse(pulse);				// remove all secondary pulses
      cnt++;
    }
  }
  return cnt;
}


bool do_pause_musicBox=false;	// triggers MUSICBOX_ENDING_FUNCTION;	// sleep, restart or somesuch

pulse_time_t soft_end_start_time;
unsigned long soft_end_cleanup_wait=60*1000000L;	// default 60"
void start_soft_ending(int days_to_live, int survive_level) {	// initiate soft ending of musicBox
/*
  void start_soft_ending(int days_to_live, int survive_level);

  (primary pulses are selected)
  if(MusicBoxState)
    if(MusicBoxState > ENDING) {		// in a play mode: initiate end
      kill all selected pulses that have not been used yet
      also kill all selected p that have not been alive yet for least survive_level times

      set survivers to be COUNTED pulses with 'days_to_live' repeats
      set_MusicBoxState(ENDING);
    } else
    if(MusicBoxState == ENDING) {		// in a play mode: initiate end
      check for survivors
      when no one is left, then *SWITCH OFF* musicbox
*/

#if defined SOFT_END_STOPPS_MOTION_UI	// TODO: integrate
  #if defined USE_MPU6050		// MPU-6050 6d accelero/gyro	// TODO: RETHINK: HACK to avoid problems between 6050 and soft end
  extern bool accGyro_is_active;
  accGyro_is_active = false;	// CANCEL accGyro_is_active	// TODO: RETHINK: HACK to avoid problems between 6050 and soft end
  #endif
#endif

  /*	*no* let it always *try* to work...
  if(MusicBoxState == OFF)
    return;
  */
  if(MusicBoxState > ENDING) {		// initiate end
    soft_end_start_time = PULSES.get_now();
    set_MusicBoxState(ENDING);

    if(MENU.verbosity >= VERBOSITY_LOWEST) {
      MENU.out(F("start_soft_ending("));		// info
      MENU.out(MagicConf.soft_end_days_to_live);
      MENU.out_comma_();
      MENU.out(MagicConf.soft_end_survive_level);
      MENU.out(F(")   main part "));
      pulse_time_t main_part_duration = soft_end_start_time;
      PULSES.sub_time(&musicBox_start_time, &main_part_duration);
      PULSES.display_time_human(main_part_duration);
      MENU.ln();
    }

#if defined USE_MONOCHROME_DISPLAY
    extern char run_state_symbol();
    char s[] = {run_state_symbol(),0};
    MC_print2x2(0, 0, s);
#endif

    for (int pulse=0; pulse<PL_MAX; pulse++) {	// make days_to_live COUNTED generating pulses
      if(PULSES.pulses[pulse].groups & g_PRIMARY) {
	//         pulse was already awake long enough?   && days_to_live positive?
	if((PULSES.pulses[pulse].counter > survive_level) && days_to_live > 0) {
	  PULSES.pulses[pulse].remaining = days_to_live;  // repeat, then vanish
	  PULSES.pulses[pulse].flags |= COUNTED;
	} else {
#if defined USE_RGB_LED_STRIP
	  clear_RGB_string_pixel(pulse);
#endif
	  PULSES.init_pulse(pulse);	// unborn pulse or too young, or days==0	just remove
	}
      }
    }
    stress_event_cnt = -1;	// stress event expected after switching to ENDING
  } else {
    if(MusicBoxState == ENDING) {		// ENDING
      for (int pulse=0; pulse<PL_MAX; pulse++) {	// check for any active pulses
	if (PULSES.pulses[pulse].flags & ACTIVE)	//   still something active?
	  return;
      }
      // no activity remaining

      set_MusicBoxState(OFF);
      if(MENU.verbosity) {
	MENU.out(F("playing ended "));
	pulse_time_t play_time = PULSES.get_now();
	PULSES.sub_time(&musicBox_start_time, &play_time);
	PULSES.display_time_human(play_time);
	MENU.ln();
      }

#if defined PERIPHERAL_POWER_SWITCH_PIN
      MENU.out(F("peripheral POWER OFF "));
      MENU.outln(PERIPHERAL_POWER_SWITCH_PIN);
      peripheral_power_switch_OFF();
      delay(600);	// let power go down softly
#endif

      tabula_rasa();
      do_pause_musicBox = true;	// triggers MUSICBOX_ENDING_FUNCTION;	// sleep, restart or somesuch	*ENDED*
      // MUSICBOX_WHEN_DONE_FUNCTION_DEFAULT
    }
  }
} // start_soft_ending()


void parameters_by_user(bool no_output = false) {
  if(!no_output)
    MENU.outln(F("manual mode"));
  scale_user_selected = true;
  sync_user_selected = true;
  stack_sync_user_selected = true;
  jiffle_user_selected = true;
  pitch_user_selected = true;	// TODO: ################################################################
  // subcycle_user_selected=true;	// TODO: ################################################################
}


void parameters_get_randomised() {	// TODO: factor out randomisation
  MENU.outln(F("random mode"));
  scale_user_selected = false;
  sync_user_selected = false;
  stack_sync_user_selected = false;
  jiffle_user_selected = false;
  pitch_user_selected = false;
  // subcycle_user_selected=false;	// TODO: ################################################################
}

void tag_randomness(bool user_selected) {
  if(user_selected)
    MENU.out('!');
  else
    MENU.out('~');
  MENU.space();
}


pulse_time_t butler_soft_end_time;	// used by musicBox_butler(int pulse), toggle_magic_autochanges()
void toggle_magic_autochanges() {
  if(magic_autochanges = !magic_autochanges) {	// if magic_autochanges got *SWITCHED ON*
    if(musicBox_butler_i != ILLEGAL32) {	// deal with soft_end_time
      pulse_time_t thisNow = PULSES.get_now();
      pulse_time_t soft_end_time;	// scratch
      int cnt=0;
      while (true)
	{
	  soft_end_time = butler_soft_end_time;
	  if(PULSES.time_reached(soft_end_time)) {
	    PULSES.add_time(&CyclesConf.used_subcycle, &butler_soft_end_time);
	    cnt++;
	  } else
	    break;
	}
      if(MENU.verbosity >= VERBOSITY_LOWEST) {
	MENU.out(cnt);
	MENU.outln(F(" subcycles added 'til soft_end"));
      }
    }
#if defined MUSICBOX_HARD_END_SECONDS
    musicBox_hard_end_time = PULSES.get_now(); // setup *savety net*  resceduled maximal performance duration
    PULSES.add_time(MUSICBOX_HARD_END_SECONDS*1000000, &musicBox_hard_end_time);
#endif
  } // else (switched *off*) nothing needs mending
} // toggle_magic_autochanges()


char* metric_mnemonic = "::";	// set by set_metric_pitch() only	 default "::" == not metric

enum metric_pitch_t {
		     mp_free = 0,
		     mp_A,
		     mp_Bb,
		     mp_B,
		     mp_C,
		     mp_Cis,
		     mp_D,
		     mp_Dis,
		     mp_E,
		     mp_F,
		     mp_Fis,
		     mp_G,
		     mp_Gis,
};

// float hertz = 0.0;	// TODO: maybe

void set_metric_pitch(int metric_pitch) {	// SETS PARAMETERS ONLY, does *not* tune
  if(MENU.maybe_display_more(VERBOSITY_LOWEST)) {
    MENU.out(F("set_metric_pitch("));
    MENU.out(metric_pitch);
    MENU.outln(')');
  }

// was: deactivated, gave wrong ';;' et al mnemonics
// REACTIVATED as I do *not* touch musicBoxConf.chromatic_pitch any more, always using set_metric_pitch() now
// TODO: TEST:
  if(musicBoxConf.chromatic_pitch == metric_pitch) {	// do *not* touch parameters for same tonica
    if(MENU.maybe_display_more(VERBOSITY_LOWEST)) {	//    else we might loose octave setup
      MENU.outln(F("metric pitch not changed"));
    }
    return;
  }

  int metric_pitch_was = musicBoxConf.chromatic_pitch = (uint8_t) metric_pitch;	// check follows

  unsigned long time_unit_was = PULSES.time_unit;
  PULSES.time_unit=1000000;	// switch to metric time unit

  // TODO:  should we keep pitch_was in set_metric_pitch()?

  switch(metric_pitch) {
  case 1: // A
    metric_mnemonic = "A ";
    musicBoxConf.pitch = {1, 220};	// A  220.0
    break;

  case 2: // Bb
    metric_mnemonic = "Bb";
    musicBoxConf.pitch = {1, 233};	// Bb 233.08
    break;

  case 3: // B == H
    metric_mnemonic = "B "; // B == H
    musicBoxConf.pitch = {1, 247};	// B=H 246.94
    break;

  case 4: // C
    metric_mnemonic = "C ";
    musicBoxConf.pitch = {1, 262};	// C  261.63
    break;

  case 5: // C#
    metric_mnemonic = "C#";
    musicBoxConf.pitch = {1, 277};	// C# 277.18
    break;

  case 6: // D
    metric_mnemonic = "D ";
    musicBoxConf.pitch = {1, 294};	// D  293.66
    break;

  case 7: // D#
    metric_mnemonic = "D#";
    musicBoxConf.pitch = {1, 311};	// D# 311.12
    break;

  case 8: // E
    metric_mnemonic = "E ";
    musicBoxConf.pitch = {1, 330};	// E  329.36
    break;

  case 9: // F
    metric_mnemonic = "F ";
    musicBoxConf.pitch = {1, 175};	// F  349.23 / 2
    break;

  case 10: // F#
    metric_mnemonic = "F#";
    musicBoxConf.pitch = {1, 185};	// F# 369.99 / 2
    break;

  case 11: // G
    metric_mnemonic = "G ";
    musicBoxConf.pitch = {1, 196};	// G  392.00 / 2
    break;

  case 12: // G#
    metric_mnemonic = "G#";
    musicBoxConf.pitch = {1, 208};	// G# 415.30 / 2
    break;

  case 0:	// set not metric
    metric_mnemonic = "::";
    musicBoxConf.chromatic_pitch = 0;	// not metric
    break;

  default:	// invalid
    MENU.out_Error_();
    MENU.out(F("invalid metric pitch "));
    MENU.outln(metric_pitch);

    metric_mnemonic = "??";		// "??" like "::" but flags an ERROR, result undefined
    musicBoxConf.chromatic_pitch = metric_pitch_was;
    PULSES.time_unit = time_unit_was;
  }

  if(MENU.maybe_display_more(VERBOSITY_LOWEST))
    MENU.outln(metric_mnemonic);		// TODO: do i want that?
} // set_metric_pitch()


void show_metric_cents_list(double base_note=220.0) {	// helper function, not needed
  /*
    sample output:

    semitone	1.059463143348693847	2.000000000000000000
    cent	1.000577807426452636	2.000000000000000000


    1.000000000000000000	220.00	1	A
    1.059463143348693847	233.08	2	Bb
    1.122462034225463867	246.94	3	B
    1.189207077026367187	261.63	4	C
    1.259921073913574218	277.18	5	C#
    1.334839820861816406	293.66	6	D
    1.414213538169860839	311.13	7	D#
    1.498307108879089355	329.63	8	E
    1.587401032447814941	349.23	9	F
    1.681792855262756347	369.99	10	F#
    1.781797409057617187	392.00	11	G
    1.887748599052429199	415.30	12	G#
  */

  int metric_pitch_was = musicBoxConf.chromatic_pitch;

  double semitone = pow(2.0, (1.0 / 12.0));
  double cent = pow(2.0, (1.0 / 1200.0));

  MENU.out(F("semitone "));
  MENU.out(semitone, 18);
  MENU.tab();
  MENU.out(pow(semitone, 12), 18);	// octave test
  MENU.ln();

  MENU.out(F("cent\t "));
  MENU.out(cent, 18);
  MENU.tab();
  MENU.out(pow(cent, 1200), 18);	// octave test
  MENU.ln(2);

  double b;
  for(int i=0; i<12; i++) {
    b = pow(semitone, i);
    MENU.out(b, 18);
    MENU.tab();
    MENU.out(b * base_note);
    MENU.tab();
    MENU.out(i+1);
    MENU.tab();
    set_metric_pitch(i+1);
    MENU.outln(metric_mnemonic);
  }

  set_metric_pitch(metric_pitch_was);	// restore
} // show_metric_cents_list()


//bool metric_alternative_tuning = false;	// force metric autotuning
bool metric_alternative_tuning = true;	// TODO: active only for TESTING...


void entune_basic_musicbox_pulses() {
  MENU.outln(F("entune_basic_musicbox_pulses()"));

  for (int pulse=0; pulse<PULSES.get_pl_max(); pulse++)
    if (PULSES.pulse_is_selected(pulse) || PULSES.pulses[pulse].groups & g_PRIMARY)
      PULSES.activate_tuning(pulse);	// en tune *all* primary or selected pulses

  if(musicBox_butler_i != ILLEGAL32)
    PULSES.activate_tuning(musicBox_butler_i);	// entune butler

  if(cycle_monitor_i != ILLEGAL32)
    PULSES.activate_tuning(cycle_monitor_i);	// entune cycle_monitor
} // entune_basic_musicbox_pulses()

void tuning_pitch_and_scale_UI_display() {	// TODO: update
  MENU.out(F("'T'=tuning 'T?'=?\t'TM'=toggle metric"));
  MENU.out_ON_off(metric_alternative_tuning);
  MENU.out(F("\t'TZ<num>'=(int)hertz\t'T!'=tune"));
  MENU.outln(F("\t'T<num>'=select scale"));

  MENU.out(F("'T[ABHCDEFG]'=select metric key ("));
  MENU.out(metric_mnemonic);

  MENU.outln(F(")\t'T+' 'T-'=chromatic up|down tuning\n"));
} // tuning_pitch_and_scale_UI_display()

void display_pitch() {
  MENU.out(F("PITCH: "));
  MENU.out(musicBoxConf.pitch.multiplier);
  MENU.slash();
  MENU.out(musicBoxConf.pitch.divisor);
  if(musicBoxConf.chromatic_pitch) {
    MENU.out(F(" metric "));
    MENU.out(metric_mnemonic);
  }
}

void display_scale_name() {
  MENU.out(F("SCALE: "));
  MENU.out(array2name(SCALES, selected_in(SCALES)));
}

bool tuning_pitch_and_scale_UI() {
  uint8_t first_token = MENU.peek();
  switch(first_token) {
  case '?':	// 'T?'
    MENU.drop_input_token();
    display_names(SCALES);	// display SCALES list
    MENU.ln(2);			//  then like bare 'T'
  case EOF8:	// bare 'T'?
    display_pitch();
    MENU.tab();
    display_scale_name();
    MENU.ln();
    tuning_pitch_and_scale_UI_display();
    MENU.ln();
    extern void musicBox_short_info();
    musicBox_short_info();
#if defined  USE_MONOCHROME_DISPLAY
    MC_show_tuning();
#endif
    return true;
    break;

  case 'M':	// 'TM' toggle metric_alternative_tuning	// TODO: check that...
    MENU.drop_input_token();
    metric_alternative_tuning = ! metric_alternative_tuning;
    MENU.out(F("metric alternative tuning"));
    MENU.out_ON_off(metric_alternative_tuning);
    MENU.ln();
    return true;
    break;

//case 'I':	// 'TIG' start GUITAR tuning
//    // .. G 2drop
//    MENU.drop_input_token();
//    MENU.drop_input_token();
//    // sound E A D G B E on metric pitch
//
//    MENU.out(F("GUITAR TUNING"));
//    MENU.ln();
//    break;

  case 'Z':	// 'TZ<n>'	tune to hertZ	integer only hertz input
    // global  float hertz = 0.0;	// TODO: maybe...

    MENU.drop_input_token();
    MENU.out(F("set tuning to frequency in hertz "));
    {
      int hz = 0;
      if(hz = MENU.numeric_input(0)) {
	musicBoxConf.pitch = {1, hz};
	pitch_user_selected = true;
	MENU.out(hz);
	MENU.tab();
	set_metric_pitch(0);	// assumed to be *not* metric
      }
    }
    MENU.ln();
#if defined  USE_MONOCHROME_DISPLAY
    MC_show_tuning();
#endif
    return true;
    break;
  } // switch(first_token)

  {				// more input?
    bool done=false;
    while(!done) {	// sequential input loop
      switch(MENU.peek()) {
      // check for ending tokens first:
      case '!':	// 'Txyz!'	trailing '!': *do* tune and quit
	tune_selected_2_scale_limited(musicBoxConf.pitch, selected_in(SCALES), 409600*2L);	// 2 bass octaves
      case ' ':	// a space ends a 'T... ' input sequence, quit
	MENU.drop_input_token();
      case EOF8:	// EOF done		quit
	done = true;
	break;

      default:	// numbers or known letters?
	if(MENU.is_numeric()) {
	  if(UI_select_from_DB(SCALES))	// select scale
	    scale_user_selected = true;
	} else {
	  // not numeric
	  switch(MENU.peek()) {	// test for known letters
	  case 'C':
	    MENU.drop_input_token();
	    set_metric_pitch(mp_C);
	    pitch_user_selected = true;
	    break;
	  case 'D':
	    MENU.drop_input_token();
	    set_metric_pitch(mp_D);
	    pitch_user_selected = true;
	    break;
	  case 'E':
	    MENU.drop_input_token();
	    set_metric_pitch(mp_E);
	    pitch_user_selected = true;
	    break;
	  case 'F':
	    MENU.drop_input_token();
	    set_metric_pitch(mp_F);
	    pitch_user_selected = true;
	    break;
	  case 'G':
	    MENU.drop_input_token();
	    set_metric_pitch(mp_G);
	    pitch_user_selected = true;
	    break;
	  case 'A':
	    MENU.drop_input_token();
	    set_metric_pitch(mp_A);
	    pitch_user_selected = true;
	    break;
	  case 'B':
	    MENU.drop_input_token();
	    set_metric_pitch(mp_Bb);
	    pitch_user_selected = true;
	  case 'H':
	    MENU.drop_input_token();
	    set_metric_pitch(mp_B);
	    pitch_user_selected = true;
	    break;
	  case '-': // 'T-' if metric: chromatic step down
	    MENU.drop_input_token();
	    if(musicBoxConf.chromatic_pitch) {
	      int pitch = musicBoxConf.chromatic_pitch - 1;
	      if(pitch < 1)
		pitch = 11;	// wrap

	      set_metric_pitch(pitch);
	      MENU.out(F("tuned to "));
	      MENU.outln(metric_mnemonic);
	      pitch_user_selected = true;
	    }
	    break;
	  case '+': // 'T+' chromatic step up
	    MENU.drop_input_token();
	    {
	      int pitch = musicBoxConf.chromatic_pitch + 1;
	      if(pitch >11)
		pitch = 1;	// wrap

	      set_metric_pitch(pitch);
	      MENU.out(F("tuned to "));
	      MENU.outln(metric_mnemonic);
	      pitch_user_selected = true;
	    }
	    break;

	  default:	//	unknown input, not for the 'T' interface
	    done=true;
	  } // known letters?
	} // not numeric
      } // treat input following 'T......'
    }	// // sequential input loop		'Tx'
  }
#if defined  USE_MONOCHROME_DISPLAY
    MC_show_tuning();
#endif
} // tuning_pitch_and_scale_UI()


void noAction_flags_line() {	// show a line with primary noACTION flag signs
  MENU.ln();
  for (int pulse=musicBoxConf.highest_primary; pulse >= musicBoxConf.lowest_primary; pulse--) {
    if(PULSES.pulses[pulse].flags) { // is there a pulse?
      if(PULSES.pulses[pulse].action_flags & noACTION)
	MENU.out('x');	// actions are blocked
      else
	MENU.out('A');	// actions on
    } else
      MENU.out('.');	// nothing there
  }

  MENU.outln(F("' action muting"));
}

void muting_actions_UI_line() {
  MENU.outln(F("'M'=muting actions: 'MH''MM''MB''ML'=toggle HIGH,MELODY,BASS,LOW 'M<n>'=~notes 'MX'=~selected 'MT'='M0'=mute all 'ME'='MA'=all on"));
}

void muting_actions_UI() {	// 'M' already received   action muting UI
  char next_letter;

  while (EOF8 != (next_letter = MENU.peek())) {
    magic_autochanges = false;	// brute force...
    // TODO: switch ui menu input mode: chiffres are note position toggle *not* presets
    switch(next_letter) {
    case 'T':	// 'MT' (morse)	all MUTED
    case '0':	// 'M0' == 'MT' all MUTED
      MENU.drop_input_token();
      for(int pulse=musicBoxConf.lowest_primary; pulse <= musicBoxConf.highest_primary; pulse++) {	// tonic only
	PULSES.pulses[pulse].action_flags |= noACTION;	// MUTE ALL
      }
      MENU.outln(F("all MUTED"));
      break;

    case 'E':	// 'ME' (morse)	all on
    case 'A':	// 'MA' == 'ME'	all on
      MENU.drop_input_token();
      for(int pulse=musicBoxConf.lowest_primary; pulse <= musicBoxConf.highest_primary; pulse++)	// ALL ON
	PULSES.pulses[pulse].action_flags &= ~noACTION;	// UNmute all,  all ON
      MENU.outln(F("all unmuted"));
      break;

    case '1':	// 'M1'		// note position	1 == tonic     toggle
    case '2':	// 'M2'		// switch individual note positions on and off
    case '3':	// 'M3'
    case '4':	// 'M4'
    case '5':	// 'M5'
    case '6':	// 'M6'
    case '7':	// 'M7'
    case '8':	// 'M8'
    case '9':	// 'M9'
      MENU.drop_input_token();

      for(int pulse=musicBoxConf.lowest_primary; pulse <= musicBoxConf.highest_primary; pulse++)	// toggle one
	if (PULSES.pulses[pulse].note_position == next_letter - '0')
	  PULSES.pulses[pulse].action_flags ^= noACTION;
      MENU.out(F("mute toggle "));
      MENU.outln(next_letter);
      break;

      // TODO: staff pitch groups L B M H
    case 'H': // 'MH'	toggle high end
      MENU.drop_input_token();
      for(int pulse = musicBoxConf.highest_primary - (primary_count/4) +1; pulse <= musicBoxConf.highest_primary; pulse++)
	PULSES.pulses[pulse].action_flags ^= noACTION;	// toggle mute high quarter
      MENU.outln(F("mute toggle HIGH"));
      break;

    case 'M': // 'MM'	toggle melody
      MENU.drop_input_token();
      for(int pulse = musicBoxConf.lowest_primary + (primary_count/2)+1;
	  pulse <= musicBoxConf.highest_primary  - (primary_count/4);
	  pulse++)
	PULSES.pulses[pulse].action_flags ^= noACTION;	// toggle mute high quarter
      MENU.outln(F("mute toggle MELODY"));
      break;

    case 'B': // 'MB'	toggle  bass
      MENU.drop_input_token();
      for(int pulse = musicBoxConf.lowest_primary + (primary_count/4)+1;
	  pulse <= musicBoxConf.lowest_primary  + (primary_count/2);
	  pulse++)
	PULSES.pulses[pulse].action_flags ^= noACTION;	// toggle mute high quarter
      MENU.outln(F("mute toggle BASS"));
      break;

    case 'L': // 'ML'	toggle low end
      MENU.drop_input_token();
      for(int pulse=musicBoxConf.lowest_primary; pulse <= musicBoxConf.lowest_primary + (primary_count/4); pulse++)
	PULSES.pulses[pulse].action_flags ^= noACTION;	// toggle mute low quarter
      MENU.outln(F("mute toggle LOW"));
      break;

    case 'X':	// 'MX'  muting toggle selected
      MENU.drop_input_token();
      PULSES.selected_toggle_no_actions();

      if (DO_or_maybe_display(VERBOSITY_LOWEST))
	MENU.outln(F("toggle action muting"));
      break;

      //	    case 'O':	// octave managnent
      //	    case '+';
      //	    case '-';
      //	      MENU.drop_input_token();
      //	      break;
    }
  } // next letter loop
  noAction_flags_line();
} // muting_actions_UI()


void show_basic_musicBox_parameters() {		// similar show_UI_basic_setup()
  MENU.out_IstrI(my_IDENTITY.preName);
  MENU.tab();
  if(musicBoxConf.preset) {
    MENU.out(F("PRESET: "));
    MENU.out(musicBoxConf.preset);
    if(musicBoxConf.name) {
      MENU.tab();
      MENU.out(musicBoxConf.name);
    }
    MENU.ln();
  }

  tag_randomness(scale_user_selected);
  display_scale_name();
  MENU.space(3);

  tag_randomness(sync_user_selected);
  MENU.out(F("SYNC: "));
  MENU.out(musicBoxConf.sync);
  if(musicBoxConf.stack_sync_slices) {	// stack_sync_slices?
    MENU.out(F(" p["));
    MENU.out(musicBoxConf.base_pulse);
    MENU.out(F("]|"));
    MENU.out(musicBoxConf.stack_sync_slices);
    MENU.space();
    tag_randomness(stack_sync_user_selected);
  }
  MENU.space(2);

  tag_randomness(jiffle_user_selected);
  MENU.out(F("JIFFLE: "));
  MENU.out(array2name(JIFFLES, selected_in(JIFFLES)));
  MENU.space(2);

  tag_randomness(pitch_user_selected);
  display_pitch();
  MENU.ln();
} // show_basic_musicBox_parameters()


double pitch_normalised=0.0;	// maybe MOVE?: to musicBoxConf?
short normalised_octave=0;	// maybe MOVE?: to musicBoxConf?
double get_normalised_pitch() {
  if(musicBoxConf.pitch.divisor==0)	// savety net
    MENU.error_ln(F("pitch.divisor==0"));
  else {
    pitch_normalised = musicBoxConf.pitch.multiplier;
    pitch_normalised /= (double) musicBoxConf.pitch.divisor;
    normalised_octave=0;

    if(pitch_normalised < 0.0)		// savety net
      MENU.error_ln(F("pitch_normalised *negative*"));
    else {				// everything looks ok
      while(pitch_normalised < 1.0) {	// below 1.0
	normalised_octave++;
	pitch_normalised *= 2.0;
      }

      while(pitch_normalised >= 2.0) {	// 2.0 and above
	normalised_octave--;
	pitch_normalised /= 2.0;
      }
    }
  }

  return pitch_normalised;
} // get_normalised_pitch()


void musicBox_short_info() {
  show_basic_musicBox_parameters();
  MENU.space(2);  // indent
  MENU.out(F("normalised pitch "));
  MENU.out(get_normalised_pitch(), 6);
  MENU.tab();
  show_cycles_1line();
}

void show_configuration_code() {	// show code, similar show_UI_basic_setup()
  MENU.out(F("name = F(\""));
  if(musicBoxConf.name)
    MENU.out(musicBoxConf.name);
  MENU.outln(F("\");"));

  MENU.out(F("date = F(\""));
  if(musicBoxConf.date)
    MENU.out(musicBoxConf.date);
  MENU.outln(F("\");"));

  MENU.out(F("select_in(SCALES, "));
  MENU.out(array2name(SCALES, selected_in(SCALES)));
  MENU.outln(F(");"));

  MENU.out(F("select_in(JIFFLES, "));	// TODO: iCode
  MENU.out(array2name(JIFFLES, selected_in(JIFFLES)));
  MENU.outln(F(");"));

  MENU.out(F("sync = "));
  MENU.out(musicBoxConf.sync);
  MENU.outln(';');

  MENU.out(F("stack_sync_slices = "));
  MENU.out(musicBoxConf.stack_sync_slices);
  MENU.outln(';');

  MENU.out(F("// base_pulse = "));	// commented out, must rethink that
  MENU.outln(musicBoxConf.base_pulse);

  MENU.out(F("pitch = {"));
  MENU.out(musicBoxConf.pitch.multiplier);
  MENU.out(F(", "));
  MENU.out(musicBoxConf.pitch.divisor);
  MENU.outln(F("};"));

  MENU.out(F("chromatic_pitch = "));
  MENU.out(musicBoxConf.chromatic_pitch);
  MENU.out(F(";\t// "));
  MENU.out(metric_mnemonic);
  MENU.ln();

  MENU.out(F("// subcycle_octave = "));	// commented out, must rethink that
  MENU.outln(CyclesConf.subcycle_octave);

  MENU.out(F("// cycle "));	// commented out, redundant info
  PULSES.display_time_human(CyclesConf.harmonical_CYCLE);
  MENU.out(F(" subcycle | "));
  PULSES.display_time_human(CyclesConf.used_subcycle);
  MENU.outln('|');
} // show_configuration_code()


// TODO: unused
void show_configuration_as_string() {	// file 1line representation, similar show_configuration_code()
  MENU.out(F("name:"));
  if(musicBoxConf.name)
    MENU.out(musicBoxConf.name);
  MENU.out('\t');

  MENU.out(F("date:"));
  if(musicBoxConf.date)
    MENU.out(musicBoxConf.date);
  MENU.out('\t');

  MENU.out(F("SCALE:"));
  MENU.out(array2name(SCALES, selected_in(SCALES)));
  MENU.out('\t');

  MENU.out(F("JIFF:"));
  MENU.out(array2name(JIFFLES, selected_in(JIFFLES)));
  MENU.out('\t');

  MENU.out(F("iCODE:"));
  MENU.out(array2name(iCODEs, selected_in(iCODEs)));
  MENU.out('\t');

  MENU.out(F("SYNC:"));
  MENU.out(musicBoxConf.sync);
  MENU.out('\t');

  MENU.out(F("synS:"));
  MENU.out(musicBoxConf.stack_sync_slices);
  MENU.out('\t');

  MENU.out(F("pul*:"));
  MENU.out(musicBoxConf.base_pulse);
  MENU.out('\t');

  MENU.out(F("PITCH:"));
  MENU.out(musicBoxConf.pitch.multiplier);
  MENU.out(',');
  MENU.out(musicBoxConf.pitch.divisor);
  MENU.out('\t');

  MENU.out(F("chrom:"));
  MENU.out(musicBoxConf.chromatic_pitch);
  MENU.out('\t');

  /* metric mnemonic is shown, but *ignored* when reading */
  MENU.out(F("chmnm:"));
  MENU.out(metric_mnemonic);	// TODO: not set?
  MENU.out('\t');

  /* decide on loading to respekt that or not */
  MENU.out(F("cyc_o:"));
  MENU.out(CyclesConf.subcycle_octave);
  MENU.out('\t');

  MENU.ln();
} // show_configuration_as_string()


void HARD_END_playing(bool with_title) {	// switch off peripheral power and hard end playing
  if(with_title)	// TODO: maybe use MENU.verbosity, but see also 'o'
    MENU.out(F("HARD_END_playing()\t"));

  if(MusicBoxState != OFF)
    set_MusicBoxState(OFF);

  if (MENU.verbosity){
    pulse_time_t play_time = PULSES.get_now();
    PULSES.sub_time(&musicBox_start_time, &play_time);
    MENU.out(F("played "));
    PULSES.display_time_human(play_time);
    MENU.ln(2);
    musicBox_short_info();
    MENU.ln();
    show_configuration_code();
    MENU.ln();
  }

#if defined USE_MONOCHROME_DISPLAY
  MC_show_musicBox_parameters();
#endif

#if defined USE_ESP_NOW
  esp_now_prepare_N_ID(broadcast_mac);
#endif

  delay(3200); // aesthetics	DADA

#if defined PERIPHERAL_POWER_SWITCH_PIN && defined BATTERY_LEVEL_CONTROL_PIN
  MENU.out(F("peripheral POWER OFF "));
  MENU.out(PERIPHERAL_POWER_SWITCH_PIN);
  MENU.tab();
  MENU.outln(read_battery_level());
  peripheral_power_switch_OFF();
  delay(3000);	// let power go down softly, do *not* shorten that without testing...
#endif

  tabula_rasa();
  MENU.ln();

  do_pause_musicBox = true;	//  triggers MUSICBOX_ENDING_FUNCTION;	sleep, restart or somesuch	// *ENDED*
  // MUSICBOX_WHEN_DONE_FUNCTION_DEFAULT
}

portMUX_TYPE musicBox_trigger_MUX = portMUX_INITIALIZER_UNLOCKED;

void musicBox_trigger_ON();	// forward declaration
bool musicBox_trigger_enabled=false;
bool blocked_trigger_shown=false;	// show only once a run

void activate_musicBox_trigger(int dummy_p=ILLEGAL32) {
#if defined MUSICBOX_TRIGGER_PIN	// trigger pin?
  musicBox_trigger_enabled = true;
  if(MENU.verbosity >= VERBOSITY_LOWEST) {
    MENU.out(F("trigger enabled "));
    MENU.outln(MUSICBOX_TRIGGER_PIN);
  }
#else					// else noop
  ;
#endif
}


unsigned int musicbox_trigger_cnt=0;
void musicBox_trigger_OFF();

void musicBox_trigger_ISR() {	// can also be used on the non interrupt version :)
  portENTER_CRITICAL_ISR(&musicBox_trigger_MUX);
  static pulse_time_t triggered_at=PULSES.get_now();	// preserves last seen fresh trigger time

  pulse_time_t new_trigger = PULSES.get_now();		// new trigger time
  pulse_time_t duration = new_trigger;			// remember

  bool triggered=false;
  switch (MusicBoxState) {
  case OFF:
  case ENDING:
  case SNORING:
    triggered = true;			// always accept trigger in these states
    break;
  case AWAKE:
    if(musicBox_trigger_enabled)		// enabled?  was it awake long enough for triggering again?
      triggered=true;			//   accept
    else if(!blocked_trigger_shown) {
      MENU.outln(F("trigger blocked"));	// FIXME: ################
      blocked_trigger_shown = true;
    }
    break;
  default:
    MENU.outln(F("musicBox_trigger_ISR unknown state"));	// should not happen
    triggered = true;	// not save... but
  }

  if(triggered) {
    musicBox_trigger_enabled = false;
    blocked_trigger_shown = false;
    triggered_at = new_trigger;
    musicbox_trigger_cnt++;
  }

  portEXIT_CRITICAL_ISR(&musicBox_trigger_MUX);
}

void musicBox_trigger_ON() {
#if ! defined MAGICAL_TOILET_HACKS	// some quick dirty hacks *NOT* using the interrupt
  MENU.out(F("MUSICBOX_TRIGGER ON\t"));
  #if defined MUSICBOX_TRIGGER_PIN
    pinMode(MUSICBOX_TRIGGER_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(MUSICBOX_TRIGGER_PIN), musicBox_trigger_ISR, RISING);
    MENU.out(MUSICBOX_TRIGGER_PIN);
    MENU.tab();
    MENU.outln(musicbox_trigger_cnt);
  #else
    MENU.ln();
  #endif
#else
  ;
#endif
}

// TODO: ################  DOES NOT WORK: isr is *not* called any more BUT THE SYSTEM BECOMES STRESSED!
void musicBox_trigger_OFF() {
#if ! defined MAGICAL_TOILET_HACKS	// some quick dirty hacks *NOT* using the interrupt
  MENU.outln(F("musicBox_trigger_OFF\t"));
  #if defined MUSICBOX_TRIGGER_PIN
    detachInterrupt(digitalPinToInterrupt(MUSICBOX_TRIGGER_PIN));
  //  esp_intr_free(digitalPinToInterrupt(MUSICBOX_TRIGGER_PIN));
  #endif
#endif
  musicBox_trigger_enabled=false;
}


#if defined MAGICAL_TOILET_HACKS	// some quick dirty hacks

#if ! defined MUSICBOX_TRIGGERED_FUNCTION
  #define MUSICBOX_TRIGGERED_FUNCTION	// dummy, noop	was: start_musicBox();
#endif
void musicBox_trigger_got_hot() {	// must be called when magical trigger was detected high
  if(musicBox_trigger_enabled) {
    musicBox_trigger_ISR();	// *not* as ISR
    MENU.outln(F("\nTRIGGERED!"));

    if(MusicBoxState != OFF)
      tabula_rasa();

    MUSICBOX_TRIGGERED_FUNCTION		// start MUSICBOX_TRIGGERED_FUNCTION

#if defined PERIPHERAL_POWER_SWITCH_PIN
    peripheral_power_switch_ON();
#endif

    musicBox_trigger_enabled=false;
  }
}
#endif


// simple (not absolutely bullet proof) inactivity detection:
int last_counter_sum=0;	// sum of counters of all relevant pulses
#define INACTIVITY_LIMIT_TIME	6*1000000	// seconds default inactivity limit	// TODO: trimm

//#define DEBUG_CLEANUP  TODO: maybe remove debug code, but can give interesting insights...

void magical_cleanup(int p) {	// deselect unused primary pulses, check if playing has ended
  static pulse_time_t inactivity_limit_time=PULSES.simple_time(0);

  if(!magic_autochanges)	// completely switched off by magic_autochanges==false
    return;			// noop

#if ! defined DEBUG_CLEANUP
  if(MENU.verbosity >= VERBOSITY_MORE)
#endif
    MENU.out(F("magical_cleanup() "));

  unsigned int deselected = PULSES.deselect_zombie_primaries();	// deselect deleted primary pulses
  bool do_display = MENU.maybe_display_more(VERBOSITY_MORE);
  int skipped=0;
  int flagged=0;
  int counter_sum=0;
  for(int pulse=0; pulse<PL_MAX; pulse++) {
    if(pulse == p)	// it's always good to exclude the host pulse...
      continue;

    if(PULSES.pulses[pulse].flags) {	// check if playing has ended  activity?
      if(PULSES.pulses[pulse].payload == &musicBox_butler) {
	if(do_display) {
	  MENU.out('B');
	}
	skipped++;
      } else if(PULSES.pulses[pulse].payload == &magical_cleanup) {
	if(do_display) {
	  MENU.out('C');
	}
	skipped++;
      } else if(PULSES.pulses[pulse].payload == &cycle_monitor) {
	if(do_display) {
	  MENU.out('M');
	}
	skipped++;
      } else {
	counter_sum += PULSES.pulses[pulse].counter;	// sums up all relevant counters to detect inactivity

	if(do_display) {	// show (only) most important group	TODO: make group mnemonics r/w and use here
	  if(PULSES.pulses[pulse].groups & g_MASTER)
	    MENU.out('0');
	  else if(PULSES.pulses[pulse].groups & g_PRIMARY)
	    MENU.out('1');
	  else if(PULSES.pulses[pulse].groups & g_SECONDARY)
	    MENU.out('2');
	  else	// TODO: keep that always up to date ;)	or better:	TODO: make group mnemonics r/w and use here
	    MENU.out('.');
	}
	flagged++;
      }
    } // flags?
  } // pulse

  if(do_display) {
    MENU.out(F(" = "));
    MENU.outln(flagged);
  }

  if(flagged) {
    if(counter_sum == last_counter_sum) {	// inactivity detected
      // long enough?
      if(PULSES.time_reached(inactivity_limit_time)) {
	MENU.out(F("inactivity stop\t"));
	HARD_END_playing(true);			// END
      }
      else
	if(MENU.verbosity >= VERBOSITY_SOME)
	  MENU.out(F("waiting "));

      if(MENU.verbosity >= VERBOSITY_SOME) {
	MENU.out(F("inactive("));
	MENU.out(counter_sum);
	MENU.outln(F(") "));
      }
    } else {					// *something* was awake
      inactivity_limit_time = PULSES.get_now();
      PULSES.add_time(INACTIVITY_LIMIT_TIME, &inactivity_limit_time);
    }

    last_counter_sum = counter_sum;
  } else { // flagged==0
    if(MENU.verbosity >= VERBOSITY_LOWEST)	// TODO: review
      MENU.out(F("END reached\t"));
    HARD_END_playing(false);			// END
  }
} // magical_cleanup(p)


int stop_on_LOW(void) {	// stops *only* pulses that are low
  int were_high=0;
  if(MENU.verbosity >= VERBOSITY_SOME)
    MENU.outln(F("stop_on_LOW"));

  for(int pulse=0; pulse<PL_MAX; pulse++) {
    if(PULSES.pulses[pulse].flags && PULSES.pulses[pulse].groups & g_SECONDARY) {
      if(PULSES.pulses[pulse].counter & 1)
	were_high++;
      else
	PULSES.init_pulse(pulse);
    }
  }
  return were_high;	// TODO: rethink output
}

int stop_on_LOW_H1(void) {	// TODO: DEBUG ################
  int were_high=0;

  if(MENU.verbosity >= VERBOSITY_SOME)
    MENU.out(F("stop_on_LOW_H1 "));

  if(/*were_high=*/stop_on_LOW()) {
    for(int pulse=0; pulse<PL_MAX; pulse++) {
      if(PULSES.pulses[pulse].flags  &&  PULSES.pulses[pulse].groups & g_SECONDARY  &&  PULSES.pulses[pulse].counter & 1) {
	if(! PULSES.pulses[pulse].flags & COUNTED) {
	  PULSES.pulses[pulse].flags |= COUNTED;
	  PULSES.pulses[pulse].remaining = 1;
	  were_high++;
	  if(MENU.verbosity >= VERBOSITY_SOME) {
	    MENU.out(pulse);
	    MENU.space();
	  }
	}
      }
    }
  }

  if(MENU.verbosity >= VERBOSITY_SOME)
    MENU.ln();

  return were_high;
}


void remove_all_primary_but_butlers() {
  if(MENU.verbosity >= VERBOSITY_SOME)
    MENU.out(F("remove primary"));

  for(int pulse=0; pulse<PL_MAX; pulse++) {
    if(pulse != musicBox_butler_i && pulse != cycle_monitor_i)	// do *not* kill the butlers!
      if(PULSES.pulses[pulse].flags && PULSES.pulses[pulse].groups & g_PRIMARY) {
	// DADA USE_RGB_LED_STRIP
	PULSES.init_pulse(pulse);

	if(MENU.verbosity >= VERBOSITY_SOME) {
	  MENU.space();
	  MENU.out(pulse);
	}
      }
  }

  if(MENU.verbosity >= VERBOSITY_SOME)
    MENU.ln();
}

void musicBox_butler(int pulse) {	// payload taking care of musicBox	ticking with slice_tick_period
  static uint8_t soft_end_cnt=0;
  static bool soft_cleanup_started=false;
  static int fast_cleanup_minimal_fraction_weighting=slice_weighting({1,4});
  static uint8_t stop_on_low_cnt=0;
  static short current_slice=0;
  static pulse_time_t butler_start_time;
  static pulse_time_t trigger_enable_time;

  pulse_time_t this_start_time =  PULSES.pulses[pulse].next;	// still unchanged?

  pulse_time_t this_time = PULSES.pulses[pulse].next;		// TODO: verify ################
  PULSES.sub_time(&PULSES.pulses[pulse].last, &this_time);	// so long inside this cycle

  // Harmonical::fraction_t phase = {this_time.time, PULSES.pulses[pulse].period.time};	// not used
  // float float_phase = this_time.time / PULSES.pulses[pulse].period.time;	// not used
  Harmonical::fraction_t this_division = {PULSES.pulses[pulse].counter -1 , cycle_slices};
  (*HARMONICAL).reduce_fraction(&this_division);
  int this_weighting = slice_weighting(this_division);

  if(PULSES.pulses[pulse].counter==1) {	// the butler initializes himself:
    PULSES.pulses[pulse].groups |= g_MASTER;
    current_slice=0;			// start musicBox clock
    butler_start_time = PULSES.pulses[pulse].next;	// still unchanged?
    init_primary_counters();
    soft_end_cnt=0;
    soft_cleanup_started=false;
    stop_on_low_cnt=0;

    pulse_time_t soft_end_time=musicBox_start_time;
    PULSES.add_time(&CyclesConf.used_subcycle, &soft_end_time);
    PULSES.add_time(100/*tolerance*/, &soft_end_time);	// tolerance	TODO: rethink&check
    butler_soft_end_time = soft_end_time;
    musicBox_trigger_enabled=false;			// do we need that?

  } else if(PULSES.pulses[pulse].counter==2) {	// now we might have more time for some setup
    ;
#if defined MUSICBOX_TRIGGER_BLOCK_SECONDS
    trigger_enable_time = PULSES.time_seconds(MUSICBOX_TRIGGER_BLOCK_SECONDS);
    if(MENU.verbosity >= VERBOSITY_SOME) {
      MENU.out(F("butler: prepare trigger  "));
      PULSES.display_time_human(trigger_enable_time);
      MENU.ln();
    }
    PULSES.add_time(&musicBox_start_time, &trigger_enable_time);
#endif

#if defined MUSICBOX_HARD_END_SECONDS	// savety net: limited maximal performance duration
    musicBox_hard_end_time = PULSES.time_seconds(MUSICBOX_HARD_END_SECONDS);
    if(MENU.verbosity >= VERBOSITY_SOME) {
      MENU.out(F("butler: prepare hard end "));
      PULSES.display_time_human(musicBox_hard_end_time);
      MENU.ln();
    }
    PULSES.add_time(&musicBox_start_time, &musicBox_hard_end_time);
#endif

  } else {	// all later wakeups, everything is initialised and set up

#if defined BATTERY_LEVEL_CONTROL_PIN
    if((PULSES.pulses[pulse].counter % 13) == 0) {	// keep an eye on the battery
      if(!assure_battery_level()) {
	MENU.outln(F("POWER LOW"));
	if(!assure_battery_level()) {	// double ckeck
	  HARD_END_playing(true);
	}
      }
    }
#endif

#if defined MUSICBOX_HARD_END_SECONDS		// SAVETY NET
    if(magic_autochanges) {
      if(PULSES.time_reached(musicBox_hard_end_time)) {
	MENU.out(F("butler: MUSICBOX_HARD_END_SECONDS "));
	HARD_END_playing(true);
      }
    }
#endif

#if defined LONELY_BUTLER_QUITS    // lonely butler detect SAVETY NET, TODO: will be completely *wrong* in other situations
    if((PULSES.pulses[pulse].counter % 11) == 0) {	// lonely butler detect
      bool survivor=false;
      for(int p=0; p<PL_MAX; p++) {
	if(PULSES.pulses[p].groups & g_PRIMARY) {	// any primary survivor?
	  survivor=true;
	  break;
	}
      }

      if(!survivor) {
	for(int p=0; p<PL_MAX; p++) {
	  if(PULSES.pulses[p].groups & g_SECONDARY) {	// any seconary survivor?
	    survivor=true;
	    break;
	  }
	}
      }

      if(!survivor) {	// lonely butler detect SAVETY NET, TODO: will be completely *wrong* in other situations
	MENU.out(F("butler: lonely butler quits "));
	HARD_END_playing(true);
      }
    }
#endif

#if defined MUSICBOX_TRIGGER_PIN	// trigger pin?
  #if defined MUSICBOX_TRIGGER_BLOCK_SECONDS
    if(! musicBox_trigger_enabled) {
      if(PULSES.time_reached(trigger_enable_time)) {
	if(MENU.verbosity >= VERBOSITY_LOWEST)
	  MENU.out(F("butler: "));
	activate_musicBox_trigger();
      }
    }
  #endif
#endif

    if(magic_autochanges) {	// the butler influences performance and changes phases like ending
      if(soft_end_cnt==0) {	// *before* starting soft end
	// soft end time could be reprogrammed by user interaction, always compute new:
	if(PULSES.time_reached(butler_soft_end_time)) {
	  soft_end_cnt++;
	  start_soft_ending(MagicConf.soft_end_days_to_live, MagicConf.soft_end_survive_level); // start soft end
	}

      } else {	// soft end was started already

	magical_cleanup(pulse);

	if(MusicBoxState == ENDING) {
	  if(!soft_cleanup_started) {
	    pulse_time_t start_cleanup = soft_end_start_time;
	    PULSES.add_time(soft_end_cleanup_wait, &start_cleanup);
	    if(PULSES.time_reached(start_cleanup)) {
	      soft_cleanup_started=true;
	      //fast_cleanup_minimal_fraction_weighting = slice_weighting({1,4});	// start quite high, then descend
	      fast_cleanup_minimal_fraction_weighting = 12;	// start here, then descend
	      if (MENU.verbosity){
		MENU.out(F("butler: time to stop "));
		pulse_time_t time_to_stop = PULSES.get_now();
		PULSES.sub_time(&musicBox_start_time, &time_to_stop);
		PULSES.display_time_human(time_to_stop);
		MENU.ln();
	      }
	      remove_all_primary_but_butlers();
	    }
	    else
	      if(MENU.maybe_display_more(VERBOSITY_MORE))
		MENU.out('.');	// waiting
	  } else {	// soft_cleanup_started  already

	    if(MENU.verbosity >= VERBOSITY_SOME)
	      MENU.out(F("s clean "));

	    if(true) {	// FIXME: ################
	      if(MENU.verbosity >= VERBOSITY_SOME) {
		MENU.out(fast_cleanup_minimal_fraction_weighting);
		MENU.out(F(" | "));
		MENU.outln(this_weighting);
	      }

	      if(this_weighting >= fast_cleanup_minimal_fraction_weighting) {
		if(stop_on_low_cnt++ == 0) {
		  // stop_on_LOW();				// STOP all low
		  stop_on_LOW_H1();				// STOP now
		}

		if(stop_on_low_cnt > 3) {	// TODO: test&trimm
		  // remove g_SECONDARY pulses, if gpio is high: one by one
		  for(int pulse=0; pulse<PL_MAX; pulse++) {
		    if(PULSES.pulses[pulse].flags && PULSES.pulses[pulse].groups & g_SECONDARY) {
		      if(MENU.verbosity > VERBOSITY_LOWEST) {
			MENU.out('X');
			MENU.out(pulse);
			MENU.space();
		      }
		      if(PULSES.pulses[pulse].flags & HAS_GPIO && PULSES.pulses[pulse].counter & 1) {
			if(PULSES.pulses[pulse].gpio != ILLEGAL8)
			  digitalWrite(PULSES.pulses[pulse].gpio, LOW);	// set GPIO LOW
			if(MENU.verbosity > VERBOSITY_LOWEST)
			  MENU.ln();
			break;						// if was high, then set low *one by one*...
		      }
		      PULSES.init_pulse(pulse);				// remove
		    }
		  }
		  if(MENU.verbosity > VERBOSITY_LOWEST)
		    MENU.ln();
		}
	      } else
		fast_cleanup_minimal_fraction_weighting--;	// relax stop condition
	    }
	  }
	} // ENDING
      }
    } // magic_autochanges?
  } // all later wakeups

  // (all wakeups)
  if(show_cycle_pattern)
    watch_primary_pulses();

  cycle_monitor(pulse);

  if(show_cycle_pattern || show_subcycle_position)
    MENU.ln();
}  // musicBox_butler()


void not_a_preset() {
  musicBoxConf.preset = 0;
  musicBoxConf.name = NULL;
  musicBoxConf.date = NULL;
}


void select_random_scale() {
#if defined RANDOM_ENTROPY_H
  random_entropy();	// entropy from hardware
#endif
  if(MENU.maybe_display_more(VERBOSITY_SOME))
    MENU.outln(F("random scale"));

  switch(random(39)) {		// random scale
  case 0: case 1: case 3: case 4:
    select_in(SCALES, pentatonic_minor);
    break;
  case 5: case 6: case 7: case 8:
    select_in(SCALES, europ_PENTAtonic);
    break;
  case 9: case 10: case 11:
    select_in(SCALES, minor_scale);
    break;
  case 12: case 13: case 14:
    select_in(SCALES, major_scale);
    break;
  case 15: case 16: case 17:
    select_in(SCALES, doric_scale);	// +1 for doric_scale, see below
    break;
  case 18: case 19: case 20:
    select_in(SCALES, tetraCHORD);	// +1 for tetraCHORD, see below
    break;
  case 21: case 22:
    select_in(SCALES, octave_4th_5th);
    break;
  case 23:
    select_in(SCALES, octaves_fifths);
    break;
  case 24:
    select_in(SCALES, octaves_fourths);
    break;
  case 25:
    select_in(SCALES, doric_scale);	// +1 for doric_scale
    break;
  case 26:
    select_in(SCALES, tetraCHORD);	// +1 for tetraCHORD
    break;
  case 27: case 28: case 29:
    select_in(SCALES, pentaCHORD);
    break;
  case 30: case 31: case 32:
    select_in(SCALES, pentachord);
    break;
  case 33:
    select_in(SCALES, trichord);
    break;
  case 34:
    select_in(SCALES, triad);
    break;
  case 35:	// too simple in most cases
    select_in(SCALES, TRIAD);
    break;
  case 36: case 37: case 38:
    select_in(SCALES, tetrachord);
    break;
  }

  scale_user_selected = false;
  uiConf.subcycle_user_selected = false;
  not_a_preset();
} //  select_random_scale()


void  select_random_stack_sync(void) {
  musicBoxConf.stack_sync_slices = 4 ;	// 4 slow start

  int o = random(10);
  while (o--) musicBoxConf.stack_sync_slices *= 2;
  if(random(4) == 1)		// 75% up, 25% downwards
    musicBoxConf.stack_sync_slices *= -1;	// *more up*  as up did not exist in previous sync implementation

  stack_sync_user_selected = false;
  not_a_preset();
}

void select_random_jiffle(void) {
#if defined RANDOM_ENTROPY_H
  random_entropy();	// entropy from hardware
#endif
  if(MENU.maybe_display_more(VERBOSITY_SOME))
    MENU.outln(F("random jiffle"));

  switch(random(142)) {
  case 0: case 1: case 2: case 3: case 4:
    select_in(JIFFLES, PENTAtonic_rise);
    break;
  case 5: case 6: case 7: case 8: case 9:
    select_in(JIFFLES, PENTAtonic_desc);
    break;

  case 10: case 11: case 12: case 13: case 14:
    select_in(JIFFLES, pentatonic_rise);
    break;
  case 15: case 16: case 17: case 18: case 19:
    select_in(JIFFLES, pentatonic_desc);
    break;

  case 20: case 21: case 22: case 23: case 24: case 29:	// was: too many, see below
    select_in(JIFFLES, tumtum);
    break;
  case 25: case 26: case 27:	// case 28: case 29:	// too many, see below
    select_in(JIFFLES, jiff_dec128);
    break;
  case 30: case 31: case 32: case 33: case 34:
    select_in(JIFFLES, ding_ditditdit);
    break;
  case 35: case 36: case 37: case 38: case 39:
    select_in(JIFFLES, diing_ditditdit);
    break;
  case 40: case 41: case 42: case 43: case 44:
    select_in(JIFFLES, din__dididi_dixi);
    break;
  case 45: case 46: case 47: case 48: case 49:
    select_in(JIFFLES, din_dididi);
    break;

  case 50: case 51: case 52: case 53:
    select_in(JIFFLES, PENTA_3rd_rise);
    break;
  case 54: case 55: case 56: case 57:
    select_in(JIFFLES, up_THRD);
    break;
  case 58: case 59: case 60: case 61:
    select_in(JIFFLES, up_THRD_dn);
    break;
  case 62: case 63: case 64: case 65:
    select_in(JIFFLES, dwn_THRD);
    break;
  case 66: case 67: case 68: case 69:
    select_in(JIFFLES, dwn_THRD_up);
    break;
  case 70: case 71: case 72: case 73:
    select_in(JIFFLES, PENTA_3rd_down_5);
    break;
  case 74: case 75: case 76: case 77:
    select_in(JIFFLES, penta_3rd_down_5);
    break;
  case 78: case 79: case 80: case 81:
    select_in(JIFFLES, rising_pent_them);
    break;
  case 82: case 83: case 84: case 85:
    select_in(JIFFLES, penta_3rd_rise);
    break;
  case 86: case 87: case 88: case 89:
    select_in(JIFFLES, simple_theme);
    break;

  case 90:
    select_in(JIFFLES, jiff_dec_pizzica);
    break;
  case 91: case 92:
    select_in(JIFFLES, pent_patternA);
    break;
  case 93: case 94:
    select_in(JIFFLES, pent_patternB);
    break;
  case 95: case 96:
    select_in(JIFFLES, pent_top_wave);
    break;
  case 97:
    select_in(JIFFLES, pent_top_wave_0);
    break;
  case 98:	// add some very simple jiffles:
    select_in(JIFFLES, d4096_3072);
    break;
  case 99:
    select_in(JIFFLES, d4096_2048);
    break;
  case 100: case 28:	// was: too many, see above
    select_in(JIFFLES, d4096_1024);
    break;
  case 101:
    select_in(JIFFLES, d4096_512);
    break;
  case 102:
    select_in(JIFFLES, d4096_256);
    break;
  case 103:
    select_in(JIFFLES, d4096_128);
    break;
  case 104:
    select_in(JIFFLES, d4096_64);
    break;
  case 105:
    select_in(JIFFLES, d4096_32);
    break;
  case 106:
    select_in(JIFFLES, d4096_16);
    break;
  case 107:
    select_in(JIFFLES, d4096_12);
    break;
  case 108:
    select_in(JIFFLES, tanboura);
    break;
  case 109: case 110: case 111:
    select_in(JIFFLES, doric_rise);
    break;
  case 112: case 113:
    select_in(JIFFLES, minor_rise);
    break;
  case 114:
    select_in(JIFFLES, doric_descend);
    break;
  case 115:
    select_in(JIFFLES, minor_descend);
    break;
  case 116:
    select_in(JIFFLES, major_descend);
    break;
  case 117: case 118:
    select_in(JIFFLES, major_rise);
    break;
  case 119: case 120:
    select_in(JIFFLES, pentaCHORD_rise);
    break;
  case 121: case 122: case 123: case 124: case 125:
    select_in(JIFFLES, tumtumtum);
    break;
  case 126: case 127: case 128: case 129: case 130:
    select_in(JIFFLES, tumtumtumtum);
    break;
  case 131: case 132:
    select_in(JIFFLES, pentachord_rise);
    break;
  case 133:
    select_in(JIFFLES, pentaCHORD_desc);	// TODO: test
    break;
  case 134:
    select_in(JIFFLES, pentachord_descend);	// TODO: test
    break;
  case 135: case 136:
    select_in(JIFFLES, tetraCHORD_rise);
    break;
  case 137:
    select_in(JIFFLES, tetraCHORD_desc);
    break;
  case 138: case 139: case 140: case 141:
    select_in(JIFFLES, mechanical);
    break;
  }

  jiffle_user_selected = false;
  not_a_preset();
} // select_random_jiffle()


void random_metric_pitches() {
  if(MENU.maybe_display_more(VERBOSITY_LOWEST))
    MENU.out(F("random *metric* tuning "));

#if defined RANDOM_ENTROPY_H
  random_entropy();	// entropy from hardware
#endif

  switch (random(17)) {
  case 0:
  case 1:
  case 3:
    musicBoxConf.chromatic_pitch = 1; // a
    musicBoxConf.pitch.multiplier = 1;
    musicBoxConf.pitch.divisor = 220; // 220	// A 220  ***not*** harmonical	// TODO: define role of multiplier, pitch.divisor
    break;
  case 4:
  case 5:
  case 6:
    musicBoxConf.chromatic_pitch = 8; // e
    musicBoxConf.pitch.multiplier = 1;
    musicBoxConf.pitch.divisor=330; // 329.36	// E4  ***not*** harmonical
    break;
  case 7:
  case 8:
    musicBoxConf.chromatic_pitch = 6; // d
    musicBoxConf.pitch.multiplier = 1;
    musicBoxConf.pitch.divisor = 294;		// 293.66 = D4
    break;
  case 9:
  case 10:
  case 11:
    musicBoxConf.chromatic_pitch = 11; // g
    musicBoxConf.pitch.multiplier = 1;
    musicBoxConf.pitch.divisor=196; // 196.00	// G3  ***not*** harmonical
    break;
  case 12:
  case 13:
    musicBoxConf.chromatic_pitch = 4; // c
    musicBoxConf.pitch.multiplier = 1;
    musicBoxConf.pitch.divisor=262; // 261.63	// C4  ***not*** harmonical
    break;
  case 14:
  case 15:
    musicBoxConf.chromatic_pitch = 9; // f
    musicBoxConf.pitch.multiplier = 1;
    musicBoxConf.pitch.divisor=174; // 174.16	// F3  ***not*** harmonical
    break;
  case 16:
    musicBoxConf.chromatic_pitch = 2; // Bb
    musicBoxConf.pitch.multiplier = 1;
    musicBoxConf.pitch.divisor=233; // 233.08	// Bb3 ***not*** harmonical
    break;
    //    musicBoxConf.pitch.divisor=247; // 246.94	// B3  ***not*** harmonical  }
  }

  if(musicBoxConf.chromatic_pitch) {
    MENU.space(2);
    MENU.out(metric_mnemonic);
  }

  pitch_user_selected = false;
  uiConf.subcycle_user_selected = false;
  not_a_preset();
} // random_metric_pitches()

void random_octave_shift(void) {
  if(MENU.maybe_display_more(VERBOSITY_SOME))
    MENU.out(F("random octave shift: "));

  switch(random(7)) {
  case 0: case 1:			// leave as is
    if(MENU.maybe_display_more(VERBOSITY_SOME))
      MENU.ln();
    return;	// (do not change octave_user_selected)
    break;
  case 2: case 3: case 4:  case 5:	// down
    MENU.play_KB_macro(F("*2"));
    break;
  case 6:
    MENU.play_KB_macro(F("/2"));	// up
    break;
  }
  octave_user_selected = false;
  not_a_preset();
}


/*
void furzificate() {	// switch to a quiet, farting patterns, u.a.
  very simple one shot implementation
  does not work as is with musicBox any more
  adapt or remove?
*/
void furzificate() {	// switch to a quiet, farting patterns, u.a.
  MENU.outln(F("furzificate()"));
  set_MusicBoxState(SNORING);

#if defined RANDOM_ENTROPY_H
  random_entropy();	// entropy from hardware
#endif
  switch (random(10)) {
  case 0:	// kalimbaxyl
    select_in(JIFFLES, kalimbaxyl);
    MENU.play_KB_macro("j");
    break;
  case 1:	// back_to_ground
    select_in(JIFFLES, back_to_ground);
    MENU.play_KB_macro("j");
    break;
  case 2:	// *3 jiffletab0	"Frosch"
    select_in(JIFFLES, jiffletab0);
    MENU.play_KB_macro(F("*3 j"));
    break;
  case 3:	// J3j jiff_december
    select_in(JIFFLES, jiff_december);
    MENU.play_KB_macro(F("*4/3 j"));
    break;
  case 4:	// jiff_dec128  the drummer in the cathedral
    select_in(JIFFLES, jiff_dec128);
    MENU.play_KB_macro(F("*3/2 j S0n"));
    break;
  case 5:	// jiffletab, silent rhythmes
    select_in(JIFFLES, jiffletab);
    MENU.play_KB_macro("j");
    break;
  case 6:	// jiff_dec_pizzica	dirty!
    select_in(JIFFLES, jiff_dec_pizzica);
    MENU.play_KB_macro(F("*3 j"));
    break;
  case 7:	// jiffletab0	fröhliches Knatterfurzkonzert explodiert
    select_in(JIFFLES,jiffletab0 );
    MENU.play_KB_macro(F("*6 j S0n"));
    break;
  case 8:	// aktivitätswellen  jiffletab01
    select_in(JIFFLES, jiffletab01);
    MENU.play_KB_macro(F("*3 j"));
    break;
  case 9:	// d4096_6 churzi plipps
    select_in(JIFFLES, d4096_6);
    MENU.play_KB_macro("j");
    break;
  }
  MENU.out(F("jiffle: "));
  MENU.outln(selected_name(JIFFLES));
}


// save and restore _user_selected configuration over deep sleep (*only*):
/*
  see: https://lastminuteengineers.com/esp32-sleep-modes-power-consumption/
  store fixed configuration options in RTC memory during deep_sleep (*only*)
*/
RTC_DATA_ATTR unsigned int * scale_stored_RTC=NULL;
RTC_DATA_ATTR unsigned int * jiffle_stored_RTC=NULL;
RTC_DATA_ATTR unsigned long multiplier_stored_RTC=0;
RTC_DATA_ATTR unsigned long divisor_stored_RTC=0;
RTC_DATA_ATTR int sync_stored_RTC=ILLEGAL32;
RTC_DATA_ATTR short stack_sync_slices_stored_RTC=0;
RTC_DATA_ATTR uint8_t metric_tunings_stored_RTC=2;	// 2 means off (&1)
RTC_DATA_ATTR bool magic_autochanges_OFF_stored_RTC=false;


void rtc_save_configuration() {
  MENU.out(F("save to RTC memory\t"));

  scale_stored_RTC	=NULL;
  jiffle_stored_RTC	=NULL;
  sync_stored_RTC	=ILLEGAL32;	// hmmm, not bullet proof	TODO: sync_stored_RTC
  stack_sync_slices_stored_RTC = 0;
  divisor_stored_RTC	=ILLEGAL32;	// !=0 after wake up flags deep sleep wakeup
  multiplier_stored_RTC	=ILLEGAL32;

  metric_tunings_stored_RTC = 2;	// 2 means off (&1)
  bool some_metric_tunings_only = MagicConf.some_metric_tunings_only;
  if(some_metric_tunings_only)
    metric_tunings_stored_RTC |= 1;	// 2 means off (&1)

  magic_autochanges_OFF_stored_RTC = ! magic_autochanges;

  if(scale_user_selected)
    scale_stored_RTC = selected_in(SCALES);

  if(sync_user_selected)
    sync_stored_RTC = musicBoxConf.sync;

  if(stack_sync_user_selected)
    stack_sync_slices_stored_RTC = musicBoxConf.stack_sync_slices;

  if(jiffle_user_selected)
    jiffle_stored_RTC = selected_in(JIFFLES);

  if(pitch_user_selected) {	// TODO: ################################################################
    multiplier_stored_RTC = musicBoxConf.pitch.multiplier;
    divisor_stored_RTC = musicBoxConf.pitch.divisor;
  }

//if(uiConf.subcycle_user_selected) ;
//if(octave_user_selected) ;
}


bool no_restore_from_RTCmem=false;	// one shot, gets reset
void maybe_restore_from_RTCmem() {	// RTC data get's always cleared unless waking up from *deep sleep*
  MENU.out(F("restore from RTCmem "));
  if(no_restore_from_RTCmem) {
    MENU.outln(F("no"));
    no_restore_from_RTCmem = false;	// one shot
    return;
  }

  if(divisor_stored_RTC) {	// divisor == 0 when *not* waking up from deep sleep, ignore
    MENU.out(F("data ok "));

    if(metric_tunings_stored_RTC & 1)	// 2 means off, &1
      MagicConf.some_metric_tunings_only=true;
    else
      MagicConf.some_metric_tunings_only=false;	// 2 means off

    if(magic_autochanges_OFF_stored_RTC)
      magic_autochanges = false;

    if(scale_stored_RTC != NULL) {
      MENU.out(F("SCALE "));
      select_in(SCALES, scale_stored_RTC);
      scale_user_selected = true;
    }

    if(sync_stored_RTC != ILLEGAL32) {
      MENU.out(F("SYNC "));
      musicBoxConf.sync = sync_stored_RTC;
      sync_user_selected = true;
    }

    if(stack_sync_slices_stored_RTC) {
      MENU.out(F("stack | "));
      musicBoxConf.stack_sync_slices = stack_sync_slices_stored_RTC;
      stack_sync_user_selected = true;
    }

    if(jiffle_stored_RTC != NULL) {
      MENU.out(F("JIFFLE "));
      select_in(JIFFLES, jiffle_stored_RTC);
      jiffle_user_selected = true;
    }

    if((multiplier_stored_RTC != ILLEGAL32) && divisor_stored_RTC != ILLEGAL32) {
      MENU.out(F("PITCH "));
      musicBoxConf.pitch.multiplier = multiplier_stored_RTC;
      musicBoxConf.pitch.divisor = divisor_stored_RTC;
      pitch_user_selected = true;
    }

    MENU.ln();
  }
  else	// *not* wake up from deep sleep
    MENU.outln_invalid();
} // maybe_restore_from_RTCmem()


pulse_time_t get_pause_time(bool display=true) {
  PULSES.fix_global_next();
  pulse_time_t pause =  PULSES.global_next;
  PULSES.sub_time(&musicBox_start_time, &pause);
  if(display) {
    MENU.out(F("pause\t"));	// TODO: get_pause_time() should not display pause==0
    PULSES.display_time_human(pause);
    MENU.ln();
  }
  return pause;
} // get_pause_time()


void start_musicBox() {
#if defined BLUETOOTH_ENABLE_PIN
  set_bluetooth_according_switch();
#endif
  MENU.out(F("\nstart_musicBox()\t"));

#if defined USE_MONOCHROME_DISPLAY && defined MUSICBOX_SHOW_PROGRAM_VERSION	// default *off*
  MC_show_program_version();
#endif

#if defined USE_RGB_LED_STRIP
  rgb_led_reset_to_default();	// reset rgb led strip management to default conditions
#endif

  set_MusicBoxState(AWAKE);

//#if defined USE_MONOCHROME_DISPLAY
//  extern void MC_show_musicBox_parameters();
//  MC_show_musicBox_parameters();
//#endif

#if defined  USE_RTC_MODULE
  show_DS1307_time_stamp();
  MENU.ln();
#endif

#if defined MUSICBOX_TRIGGER_PIN
  pinMode(MUSICBOX_TRIGGER_PIN, INPUT);	// looks like we need that ???
  MENU.out(F("trigger pin: "));
  MENU.outln(MUSICBOX_TRIGGER_PIN);
#endif
  musicBox_trigger_enabled=false;
  blocked_trigger_shown = false;	// show only once a run
  musicBox_butler_i=ILLEGAL32;

#if defined BATTERY_LEVEL_CONTROL_PIN
  show_battery_level();
  if(assure_battery_level())
    MENU.outln(F("power accepted"));
  else {
    MENU.outln(F(">>> NO POWER <<<"));
    if(!assure_battery_level())	// double ckeck
      HARD_END_playing(false);
  }
#endif

#if defined PERIPHERAL_POWER_SWITCH_PIN
  peripheral_power_switch_ON();

  MENU.out(F("peripheral POWER ON "));
  MENU.outln(PERIPHERAL_POWER_SWITCH_PIN);

  delay(250);	// give peripheral supply voltage time to stabilise
#endif

  /*
    maybe_restore_from_RTCmem();
    *either* from
    pulses.ino setup()	would be better, but does not work?
    *or* from
    start_musicBox();	must be blocked if appropriate
  */
  maybe_restore_from_RTCmem();		// only after deep sleep, else noop

  MENU.ln();

  MENU.men_selected = musicBox_page;
  MENU.verbosity = VERBOSITY_LOWEST;

  // TODO: REWORK:  setup_bass_middle_high()  used in musicBox, but not really compatible
  MENU.ln();	// start setup sequence output "block"
  setup_bass_middle_high(musicBoxConf.bass_pulses, musicBoxConf.middle_pulses, musicBoxConf.high_pulses);

  if(!scale_user_selected)	// if *not* set by user interaction	// TODO: factor out randomisation
    select_random_scale();	//   random scale

  if(!jiffle_user_selected)	// if *not* set by user interaction	// TODO: factor out randomisation
    select_random_jiffle();	//   random jiffle

#if defined USE_MPU6050
  if(selected_in(JIFFLES) != jiffle_RAM) {	// maybe populate jiffle ram for motion UI?
    if(jiffle_RAM[0] == '\0')
      load2_jiffle_RAM(selected_in(JIFFLES));	// anything seems better than nothing ;)
  }
#endif

  next_gpio(0);			// FIXME: TODO: HACK  would destroy an already running configuration....
  setup_jiffle_thrower_selected(selected_actions);	// FIXME: why does this give 'no free GPIO' ???

  PULSES.add_selected_to_group(g_PRIMARY);
  // set_primary_block_bounds();		// delayed, see below

  if(!sync_user_selected) {	// if *not* set by user interaction	// TODO: factor out randomisation
    musicBoxConf.sync = random(6);		// random sync	// MAYBE: define  select_random_sync()  ???
    not_a_preset();
  }

  // stack_sync
  if(!stack_sync_user_selected) {	// if *not* set by user interaction	// TODO: factor out randomisation
    if (random(2) == 1)		// stack_sync in 50%
      select_random_stack_sync();
    else
      musicBoxConf.stack_sync_slices = 0;	// 50% normal sync only, still automagic, but sync stacking is off

    not_a_preset();	// well, most of the time
  }

  // time_unit
  PULSES.time_unit=1000000;	// default metric

  // random pitch	// TODO: factor out randomisation
  if(!pitch_user_selected) {	// if *not* set by user interaction
    if(!MagicConf.some_metric_tunings_only) {	// RANDOM tuning?
      musicBoxConf.chromatic_pitch = 0;

#if defined RANDOM_ENTROPY_H
      random_entropy();	// entropy from hardware
#endif

      musicBoxConf.pitch.divisor = random(160, 450);	// *not* tuned for other instruments	// TODO: factor out randomisation
      if(MENU.maybe_display_more(VERBOSITY_SOME))
	MENU.out(F("random pitch\t"));
    } else {			// *some RANDOMLY selected METRIC A=440 tunings*
      random_metric_pitches();	// random *metric* pitches	// TODO: factor out randomisation
      MENU.tab();
    }
    not_a_preset();
  }

// #define OLDSTYLE_TUNE_AND_LIMIT	// use (buggy) old style tuning and lowering mechanism, for recording old setups
#if defined OLDSTYLE_TUNE_AND_LIMIT	// use (buggy) old style tuning and lowering mechanism?	// TODO: OBSOLETE?
  // HACK: backwards compatibility for multiplier/divisor	################
  tune_2_scale(voices, multiplier*musicBoxConf.pitch.multiplier, divisor*musicBoxConf.pitch.divisor, selected_in(SCALES)); // TODO: OBSOLETE? TODO: define role of multiplier, divisor
#else	// working on new style tune_selected_2_scale_limited(musicBoxConf.pitch, selected_in(SCALES), limit);
  tune_selected_2_scale_limited(musicBoxConf.pitch, selected_in(SCALES), 409600*2L);	// 2 bass octaves // TODO: adjust limit appropriate...
#endif

  extern short steps_in_octave;
  MENU.out(F("notes/octave "));
  MENU.outln(steps_in_octave);


#if defined USE_RGB_LED_STRIP	// set a hue_slice_cnt that fits the tuning
  if(RGBstringConf.set_automagic_hue_slices) {
    switch(steps_in_octave) {
    case 5: // pentatonic scales
      RGBstringConf.hue_slice_cnt = 15;	// try also 30
      break;
    case 7: // heptatonic aka "diatonic" scales
      RGBstringConf.hue_slice_cnt = 21;
      break;
    case 4: // i.e. tetrachord
      RGBstringConf.hue_slice_cnt = 12;	// try 24
      break;
    case 3: // octave-fourth-fifth,   TRIADs,	TRICHORDs	...
      RGBstringConf.hue_slice_cnt = 18;	// try 9
      break;
    case 2: // octave-fourth,	octave-fifth,	borduns
      RGBstringConf.hue_slice_cnt = 12;	// try 6, 24
      break;
    case 1: // just octaves
      RGBstringConf.hue_slice_cnt = 24;	// try 12, 6, 3
      break;

    default:
      RGBstringConf.hue_slice_cnt = steps_in_octave * 3;
      if(RGBstringConf.hue_slice_cnt < 6)
	RGBstringConf.hue_slice_cnt;
      break;
    } // switch(steps_in_octave)

    MENU.out(F("set "));
  } else	// if(set_automagic_hue_slices)
    MENU.out(F("fixed "));

  MENU.out(F("hue slices "));
  MENU.outln(RGBstringConf.hue_slice_cnt);
#endif

  if(!pitch_user_selected)		// if *not* set by user interaction	// TODO: factor out randomisation
    random_octave_shift();		// random octave shift

#if defined OLDSTYLE_TUNE_AND_LIMIT
  // *not* regarding pitch_user_selected as selected frequencies might be too high, check anyway...
  lower_audio_if_too_high(409600*2);	// 2 bass octaves	// TODO: adjust appropriate...
#endif

  set_primary_block_bounds();			// remember where the primary block starts and stops
  highest_primary_at_start = musicBoxConf.highest_primary;	// needed for primary pattern display in watch_primary_pulses()

  get_normalised_pitch();
  MENU.out(F("normalised_pitch "));
  MENU.out(pitch_normalised, 6);
  MENU.ln();

#if defined PERIPHERAL_POWER_SWITCH_PIN
  peripheral_power_switch_ON();
#endif

  pulse_time_t period_lowest = PULSES.pulses[musicBoxConf.lowest_primary].period;
  CyclesConf.harmonical_CYCLE = scale2harmonical_cycle(selected_in(SCALES), &period_lowest);

  if(! uiConf.subcycle_user_selected) {
    CyclesConf.used_subcycle = CyclesConf.harmonical_CYCLE;
    CyclesConf.subcycle_octave = 0;

    if(max_subcycle_seconds) {

#if defined PULSES_USE_DOUBLE_TIMES
      while (CyclesConf.used_subcycle > PULSES.time_seconds(max_subcycle_seconds))
	{
	  PULSES.div_time(&CyclesConf.used_subcycle, 2);
	  CyclesConf.subcycle_octave++;
	} // was too long?

#else // old int overflow style
      CyclesConf.used_subcycle = PULSES.simple_time(max_subcycle_seconds*1000000L);
      pulse_time_t this_subcycle=CyclesConf.harmonical_CYCLE;
      while(true) {
	if(this_subcycle.time <= CyclesConf.used_subcycle.time && this_subcycle.overflow==CyclesConf.used_subcycle.overflow) {
	  CyclesConf.used_subcycle = this_subcycle;
	  break;
	}
	PULSES.div_time(&this_subcycle, 2);
	CyclesConf.subcycle_octave++;
      }
#endif

    } // if(max_subcycle_seconds)
  }

  MENU.ln();
  musicBox_short_info();
#if defined USE_MONOCHROME_DISPLAY
  MC_show_musicBox_parameters();
#endif

  //  MENU.outln(F("\n >>> * <<<"));	// start output block with configurations

#if defined  USE_RTC_MODULE		// repeat that in here, keeping first one for power failing cases
  MENU.out(F("date="));
  show_DS1307_time_stamp();
  MENU.ln();
#endif

  if(MENU.verbosity >= VERBOSITY_MORE) {
    show_cycle(CyclesConf.harmonical_CYCLE);	// shows multiple cycle octaves
    MENU.ln();
  }

  set_cycle_slice_number(cycle_slices);


/*  I was testing syncing primaries and butler in *one* activate_selected_synced_now() call
  //    but decided to stay with the old implementation...

  // setup butler:
  // remember pulse index of the butler, so we can call him, if we need him ;)
  musicBox_butler_i =							\
    PULSES.setup_pulse(&musicBox_butler, ACTIVE, PULSES.get_now(), slice_tick_period);
  PULSES.pulses[musicBox_butler_i].groups |= g_MASTER;	// savety net, until butler has initialised itself
  PULSES.select_pulse(musicBox_butler_i);	// select butler for syncing
*/
  musicBox_start_time = PULSES.get_now();	// keep musicBox_start_time

  if(musicBoxConf.stack_sync_slices) {
    pulse_time_t slice;
    slice = PULSES.pulses[musicBoxConf.base_pulse].period;
    PULSES.div_time(&slice, abs(musicBoxConf.stack_sync_slices));
    PULSES.activate_selected_stack_sync_now(slice, musicBoxConf.sync, (musicBoxConf.stack_sync_slices>0 /*aka 'ascending'*/));
  } else {
    PULSES.activate_selected_synced_now(musicBoxConf.sync);	// 'n' 'N' sync and activate
  }

  PULSES.fix_global_next();			// cannot understand why i need that here...
  pulse_time_t pause = get_pause_time(true/*do display*/);

  // TODO: TEST: start pause detection and skipping
  if(musicBoxConf.sync || musicBoxConf.stack_sync_slices) {	// start pause possible?
    if(musicBoxConf.stack_sync_slices) { // stack_sync sliced?
      if(MagicConf.autoskip_pause) {	 //   autoskip?
	PULSES.time_skip_selected(pause);
	MENU.out(F("skipped\t"));
	PULSES.display_time_human(pause);
	MENU.ln();

      } else	// pause is *not* autoskipped
	MENU.outln(F("no pause autoskip"));
//#if defined USE_MONOCHROME_DISPLAY
//	char s[] = {'p', 0};		// TODO: how to clear that?
//	MC_print2x2(0, 0, s);	// TODO: how to clear that?
//#endif

    } else {	// stack_sync_slices==0
      if(MENU.verbosity >= VERBOSITY_LOWEST) {
	MENU.ln();
	musicBox_short_info();
	MENU.ln(2);
      }
    } // stack_sync_slices?
  } // possible?

  // setup butler:	works better if activated a tad later than the primary pulses...
  // remember pulse index of the butler, so we can call him, if we need him ;)
  musicBox_butler_i =	\
    PULSES.setup_pulse(&musicBox_butler, ACTIVE, PULSES.get_now(), slice_tick_period);
  PULSES.pulses[musicBox_butler_i].groups |= g_MASTER;	// savety net, until butler has initialised itself

  stress_event_cnt = -4;	// some stress events will often happen after starting the musicBox
  stress_count = 0;
} // start_musicBox()


void relax() {		// kill highest secondary pulse
  for(int pulse=PL_MAX ; pulse >= 0; pulse--) {
    if(PULSES.pulses[pulse].groups & g_SECONDARY) {
      if(PULSES.pulses[pulse].flags & ACTIVE) {			// highest active secondary pulse
	if(PULSES.pulses[pulse].flags & HAS_GPIO)		// set GPIO low?  maybe MAKES NOISE?
	  if(PULSES.pulses[pulse].gpio != ILLEGAL8)
	    digitalWrite(PULSES.pulses[pulse].gpio, LOW);

	PULSES.init_pulse(pulse);				// remove highest secondary pulses

#if defined STRESS_MONITOR_LEVEL		// *any* stress monitoring level triggers notice
	MENU.out(F("relax p["));
	MENU.out(pulse);
	MENU.outln(']');
#endif

	return;		// removed just the highest one
      }
    } // secondary
  } // pulse loop
} // relax()


void magical_stress_release() {		// special stress release for magical music box
  if (voices) {	// normal case, I have never seen exceptions
    PULSES.init_pulse(musicBoxConf.highest_primary);		// *remove* topmost voice
#if defined USE_RGB_LED_STRIP
    clear_RGB_string_pixel(musicBoxConf.highest_primary);
#endif
    PULSES.select_n(--voices);

    MENU.out(F("magical_stress_release() V"));
    MENU.outln(voices);
    stress_count = 0;		// configure pulses stress managment
    stress_event_cnt = -3;	// some *heavy* stress event expected after magical_stress_release()...
  } else {
    tabula_rasa();	// *could* happen some time, maybe *SAVETY NET*
    stress_count = 0;		// configure pulses stress managment
    stress_event_cnt = 0;
  }
  set_primary_block_bounds();
  // instant_stress_release();	// DEACTIVATED
}


// magical fart on reading a floating GPIO pin ;)
portMUX_TYPE magical_fart_MUX = portMUX_INITIALIZER_UNLOCKED;
unsigned int magical_fart_cnt=0;
gpio_pin_t magical_fart_output_pin=ILLEGAL8;

void magical_fart_ISR() {
  portENTER_CRITICAL_ISR(&magical_fart_MUX);
  magical_fart_cnt++;

  if(magical_fart_output_pin != ILLEGAL8)
    digitalWrite(magical_fart_output_pin, ! digitalRead(magical_fart_output_pin));

  //MENU.out(F("magical_fart_ISR()\t"));
  //MENU.outln(magical_fart_cnt);
  portEXIT_CRITICAL_ISR(&magical_fart_MUX);
}

void magical_fart_setup(gpio_pin_t sense_pin, gpio_pin_t output_pin) {
  magical_fart_output_pin = output_pin;
  MENU.out(F("magical_FART\t"));
  MENU.out(sense_pin);
  MENU.out('>');
  MENU.outln(output_pin);
  pinMode(sense_pin, INPUT);
  attachInterrupt(digitalPinToInterrupt(sense_pin), magical_fart_ISR, CHANGE);
}


// light_sleep();  works fine as default for triggered musicBox	  but bluetooth does *not* wake up
void light_sleep() {	// see: bool do_pause_musicBox	flag to go sleeping from main loop
  MENU.out(F("light_sleep()\t"));

#if defined USE_BLUETOOTH_SERIAL_MENU
  bt_status_before_sleeping = esp_bt_controller_get_status();

  /*
  BLUEtoothSerial.end();	// reboots instead of sleeping
  yield();
  */

  // without esp_bluedroid_disable() crashes instead of sleeping, or crashes soon after waking up
  MENU.ok_or_error_ln(F("esp_bluedroid_disable()"), esp_bluedroid_disable());
  yield();

  /*
  // esp_bt_controller_disable() does no good nor harm
  MENU.ok_or_error_ln(F("esp_bt_controller_disable()"), esp_bt_controller_disable());
  */

  // BLUEtoothSerial.end();	// accepted here, but still no BT after wakeup

  if(MENU.verbosity >= VERBOSITY_SOME)
    show_bt_controller_status();
#endif

#if defined USE_WIFI_telnet_menu
  esp_wifi_stop();
#endif

  /*
    there was an ugly noise on DACs during sleep
    among a lot of other things i tried all the following without in vain:

    rtc_gpio_isolate((gpio_num_t) 25);	// no influence on noise :(
    rtc_gpio_isolate((gpio_num_t) 26);	// no influence on noise :(
    pinMode(25, INPUT);	// no influence on noise :(
    pinMode(26, INPUT);	// no influence on noise :(

    dacWrite(25,0);	// no influence on noise :(
    dacWrite(26,0);	// no influence on noise :(

    if(esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF))
      MENU.error_ln(F("esp_sleep_pd_config()"));

    if(esp_sleep_pd_config(ESP_PD_DOMAIN_XTAL, ESP_PD_OPTION_OFF))
      MENU.error_ln(F("esp_sleep_pd_config()"));

    dac_i2s_disable();

    nothing helped against the noise
  */

  // kill the ugly noise on DACs during sleep modes:
  pinMode(25, OUTPUT);		// avoid ugly noise on DAC during sleep
  digitalWrite(25, LOW);

  pinMode(26, OUTPUT);		// avoid ugly noise on DAC during sleep
  digitalWrite(26, LOW);

#if defined MUSICBOX_TRIGGER_PIN	// trigger pin?
  if (esp_sleep_enable_ext0_wakeup((gpio_num_t) MUSICBOX_TRIGGER_PIN, 1))
    MENU.error_ln(F("esp_sleep_enable_ext0_wakeup()"));
/*
  if(gpio_wakeup_enable((gpio_num_t) MUSICBOX_TRIGGER_PIN, GPIO_INTR_LOW_LEVEL))
    MENU.error_ln(F("gpio_wakeup_enable()"));

  if (esp_sleep_enable_gpio_wakeup())
    MENU.error_ln(F("esp_sleep_enable_gpio_wakeup"));
*/
#endif

//  if(esp_sleep_enable_uart_wakeup(0))		// TODO: ################
//    MENU.error_ln(F("esp_sleep_enable_uart_wakeup(0)"));
//
//  if(esp_sleep_enable_uart_wakeup(1))
//    MENU.error_ln(F("esp_sleep_enable_uart_wakeup(1)"));

  MENU.outln(F("sleep well..."));
  delay(1500);

  MENU.ok_or_error_ln(F("esp_light_sleep_start()"), esp_light_sleep_start());

  MENU.out(F("\nAWOKE\t"));
  yield();

#if defined USE_BLUETOOTH_SERIAL_MENU
  /*
  if(esp_bluedroid_enable())	// did not wake up with this one here, now does, or not ;)
    MENU.error_ln(F("esp_bluedroid_enable()"));
  */

  /* first boot: ERROR	TODO: ################ comment?
  MENU.ok_or_error_ln(F("esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT)"), \
		      esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT));
  */

  /*	// is in bluetooth_setup();
  MENU.ok_or_error_ln(F("esp_bt_controller_enable((esp_bt_mode_t) 0x02)"), \
		      esp_bt_controller_enable((esp_bt_mode_t) 0x02));
  */

#if defined BLUETOOTH_ENABLE_PIN
  if(bluetooth_switch_info())
#endif
    bluetooth_setup();		// does no good, does no harm

  /*
  if(esp_bluedroid_enable())	// wakes up *if* after bluetooth_setup(), but still no BT
    MENU.error_ln(F("esp_bluedroid_enable()"));
  */
#endif

  int cause = esp_sleep_get_wakeup_cause();
  // see  https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/system/sleep_modes.html#_CPPv218esp_sleep_source_t
  switch (cause = esp_sleep_get_wakeup_cause()) {
  case 0:	// ESP_SLEEP_WAKEUP_UNDEFINED	0
    MENU.outln(F("wakeup undefined"));
#if defined AUTOSTART
    MENU.out(F("HACK: restart anyway "));
    AUTOSTART
#endif
    break;
  case 2:	// ESP_SLEEP_WAKEUP_EXT0	2
    MENU.outln(F("wakeup EXT0\t"));
    // TODO: gpio?
    break;
  case 7:	// ESP_SLEEP_WAKEUP_GPIO	7
    /*	// TODO: FIXME: GPIO?
    MENU.outln(F("wakeup gpio\t"));
    unsigned int bits;
    bits = esp_sleep_get_ext1_wakeup_status();
    MENU.outBIN(bits, 40);
    MENU.ln();
    MENU.outln(bits);	// ################
    */
    break;
  default:
    MENU.outln(cause);
  }
}


void deep_sleep() {
  MENU.out(F("deep_sleep()\t"));

  rtc_save_configuration();

#if defined USE_BLUETOOTH_SERIAL_MENU	// do we use bluetooth?
  esp_bluedroid_disable();
  esp_bt_controller_disable();
#endif

#if defined USE_WIFI_telnet_menu	// do we use WIFI?
  esp_wifi_stop();
#endif

#if defined MUSICBOX_TRIGGER_PIN	// trigger pin?
  if (esp_sleep_enable_ext0_wakeup((gpio_num_t) MUSICBOX_TRIGGER_PIN, 1))
    MENU.error_ln(F("esp_sleep_enable_ext0_wakeup()"));
#endif

  /* ONLY HELPS in light_sleep()
  // kill the ugly noise on DACs during sleep modes:
  pinMode(25, OUTPUT);		// avoid ugly noise on DAC during sleep
  digitalWrite(25, LOW);

  pinMode(26, OUTPUT);		// avoid ugly noise on DAC during sleep
  digitalWrite(26, LOW);
  */				// DOES NOT HELP HERE :(

  /* testing rtc_gpio_isolate(),	trying different sequences...
  if(MENU.verbosity >= VERBOSITY_LOWEST)
    MENU.outln(F("rtc_gpio_isolate DAC"));

  // does *not* work
  rtc_gpio_isolate((gpio_num_t) 25);   // DOES NOT HELP  TODO: rtc_gpio_hold_dis
  rtc_gpio_isolate((gpio_num_t) 26);   // DOES NOT HELP  TODO: rtc_gpio_hold_dis

  // does *not* work either
  pinMode(25, OUTPUT);		// avoid ugly noise on DAC during sleep?
  digitalWrite(25, LOW);
  pinMode(26, OUTPUT);		// avoid ugly noise on DAC during sleep?
  digitalWrite(26, LOW);
  rtc_gpio_isolate((gpio_num_t) 25);   // DOES NOT HELP  TODO: rtc_gpio_hold_dis
  rtc_gpio_isolate((gpio_num_t) 26);   // DOES NOT HELP  TODO: rtc_gpio_hold_dis
  */

#if defined BATTERY_LEVEL_CONTROL_PIN
  MENU.out(F("pausing...\t"));
  delay(1600);	// beware of DAC snoring! let power go down enough
#endif
  MENU.outln(F("esp_deep_sleep_start()"));

  esp_deep_sleep_start();	// sleep well ... ... ...
  // it will never get here
}


/* **************************************************************** */
void sync_shifting(Harmonical::fraction_t shift) {
  pulse_time_t this_shift;
  if(shift.multiplier) { // no zero shift?
    for (int pulse=musicBoxConf.lowest_primary; pulse <= musicBoxConf.highest_primary; pulse++) {
      if(PULSES.pulses[pulse].flags & ACTIVE) {
	this_shift = PULSES.pulses[pulse].period;
	PULSES.mul_time(&this_shift, shift.multiplier);
	PULSES.div_time(&this_shift, shift.divisor);
	PULSES.add_time(&this_shift, &PULSES.pulses[pulse].next);
      }
    }
    PULSES.fix_global_next();
    /*
    // TODO:; DADA	REMOVE:
    MENU.out(F("sync_shifting("));
    display_fraction_int(shift);
    MENU.outln(')');
    // TODO:; DADA	REMOVE:
    */
  } // no shift
}

/* **************************************************************** */
void musicBox_setup() {	// TODO:update
  MENU.ln();

#if defined MUSICBOX_TRIGGER_PIN		// TODO: use HARDWARE musicbox_trigger_pin != ILLEGAL8; ?
  MENU.out(F("musicBox trigger pin:\t"));
  MENU.outln(MUSICBOX_TRIGGER_PIN);

#if defined MUSICBOX_TRIGGER_BLOCK_SECONDS	// TODO: should go to musicBoxConf or HARDWARE
  MENU.out(F("disable retriggering:\t"));
  MENU.outln(MUSICBOX_TRIGGER_BLOCK_SECONDS);
  MENU.ln();
#endif
#endif //  MUSICBOX_TRIGGER_PIN


#if defined MAX_SUBCYCLE_SECONDS
  MENU.out(F("max subcycle seconds:\t"));
  MENU.outln(max_subcycle_seconds);
#endif

#if defined MUSICBOX_HARD_END_SECONDS
  MENU.out(F("hard stop:\t\t"));
  MENU.outln(MUSICBOX_HARD_END_SECONDS);
#endif

#if defined MUSICBOX_TRIGGER_PIN	// trigger pin?
  if (esp_sleep_enable_ext0_wakeup((gpio_num_t) MUSICBOX_TRIGGER_PIN, 1))
    MENU.error_ln(F("esp_sleep_enable_ext0_wakeup()"));
#endif

#if defined ESP32_USB_DAC_ONLY || defined ESP32_USB_DAC_ONLY_OLED	// *minimal* usb powered *DAC only* setups
  musicBoxConf.bass_pulses=14;	// see  setup_bass_middle_high()
  musicBoxConf.middle_pulses=0;	// see  setup_bass_middle_high()
  musicBoxConf.high_pulses=22;	// see  setup_bass_middle_high()
#else	// most setups:
  musicBoxConf.bass_pulses=14;	// see  setup_bass_middle_high()
  musicBoxConf.middle_pulses=15;	// see  setup_bass_middle_high()
  musicBoxConf.high_pulses=7;	// see  setup_bass_middle_high()
#endif
}


#include "presets.h" // TODO: #define HAS_PRESETS	################

/* **************************************************************** */
// musicBox menu

void musicBox_display() {
#if defined BLUETOOTH_ENABLE_PIN
  set_bluetooth_according_switch();	// side effect
#endif

  MENU.outln(F("The Harmonical Music Box  http://github.com/reppr/pulses/\n"));

  MENU.out(F("MusicBoxState "));
  MENU.out(MusicBoxState_name);
  MENU.tab();
  MENU.out(F("butler "));
  if(musicBox_butler_i == ILLEGAL32)
    MENU.out('-');
  else {
    MENU.out(musicBox_butler_i);
    MENU.tab();

    Harmonical::fraction_t current_phase = {PULSES.pulses[musicBox_butler_i].counter, cycle_slices};
    MENU.out('[');
    display_fraction_int(current_phase);

    (*HARMONICAL).reduce_fraction(&current_phase);
    if(current_phase.divisor != cycle_slices) {	// fraction has been reduced
      MENU.out(F(" = "));
      display_fraction_int(current_phase);
    }

    MENU.out(F(" = "));
    MENU.out((float) current_phase.multiplier / current_phase.divisor, 6);
    MENU.out(']');
  }
  MENU.ln();

  MENU.out(F("harmonical cycle 'c'\t"));
  PULSES.display_time_human_format(CyclesConf.harmonical_CYCLE);
  MENU.tab();

  MENU.out(F("2^"));
  MENU.out(CyclesConf.subcycle_octave);
  MENU.out(F(" subcycle  | "));
  PULSES.display_time_human(CyclesConf.used_subcycle);
  MENU.out(F("| \tslices '&' "));
  MENU.out(cycle_slices);
  MENU.tab();

  set_cycle_slice_number(cycle_slices);	// make sure slice_tick_period is ok
  PULSES.display_time_human(slice_tick_period);
  MENU.ln();

  MENU.out(F("subcycle octave 'O+' 'O-'\tresync/restart now 'N'\t't' metric tuning"));
  MENU.out_ON_off(MagicConf.some_metric_tunings_only);
  MENU.out(F("  'F' "));
  if(scale_user_selected && sync_user_selected && jiffle_user_selected  && pitch_user_selected && stack_sync_user_selected /* && uiConf.subcycle_user_selected*/)
    MENU.out(F("un"));
  MENU.outln(F("freeze parameters"));

  MENU.ln();

  MENU.out(F("'o' show position ticker"));
  MENU.out_ON_off(show_subcycle_position);

  MENU.out(F("\t'p' show cycle pattern 'p*' toggle"));
  MENU.out_ON_off(show_cycle_pattern);
  MENU.ln();

  MENU.out(F("'a' autochanges"));
  MENU.out_ON_off(magic_autochanges);

  MENU.tab();
  MENU.out(F("'c' cycle "));

  pulse_time_t period_lowest = PULSES.pulses[musicBoxConf.lowest_primary].period;
  CyclesConf.harmonical_CYCLE = scale2harmonical_cycle(selected_in(SCALES), &period_lowest); // TODO: rethink ################
  PULSES.display_time_human(CyclesConf.harmonical_CYCLE);
  MENU.ln();

  MENU.space(4);
  MENU.out(F("soft_end("));
  MENU.out(MagicConf.soft_end_days_to_live);	// remaining days of life after soft end
  MENU.out_comma_();
  MENU.out(MagicConf.soft_end_survive_level);	// the level a pulse must have reached to survive soft en
  MENU.out(F(")\t'd'=days to survive  'l'=level minimal age 'E'= start soft end now  'w' minimal weight "));
  MENU.outln(MagicConf.soft_cleanup_minimal_fraction_weighting);

  MENU.space(4);
  MENU.outln(F("hard end='H'"));	// old meaning deactivated
  MENU.ln();

  // MENU.outln(F("'L'=stop when low\t'LL'=stop only low\thard end='H'"));	// old meaning deactivated
  MENU.out(F("'EF[DLHRUP]'  deep_sleep, light_sleep, hibernate, restart, user, presets\t"));
  show_when_done_function();
  MENU.ln();

#if defined USE_RGB_LED_STRIP
  rgb_led_string_UI_display();
  MENU.ln();
#endif

#if defined USE_ESP_NOW
  MENU.outln(F("'C<macro>'=send macro 'CC<num>'=recipient 'C?'=peers 'C'='CC'=build net 'CS'=automagic sync landscape"));
  MENU.ln();
#endif

  tuning_pitch_and_scale_UI_display();

#if defined PERIPHERAL_POWER_SWITCH_PIN
  MENU.out(F("'v' peripheral power"));
  MENU.out_ON_off(peripheral_power_on);
#endif
  MENU.out(F("   '|' sync slices ="));
  MENU.out(musicBoxConf.stack_sync_slices);
  MENU.out(F(" '|b' base  ="));
  MENU.out(musicBoxConf.base_pulse);
  MENU.ln(2);

  MENU.out(F("'P'="));
  if(MusicBoxState == OFF)
    MENU.out(F("START"));
  else
    MENU.out(F("STOP"));
  MENU.out(F("\t'P1'= stop primary\t'P2'= stop secondary"));
  if(MusicBoxState == OFF)
    MENU.outln(F("\t'W<seconds>' = wait&start"));

  MENU.outln(F("'m'= set mode\t'mm' 'mM'= manual\t'ma' 'mA'= automagic"));
#if defined USE_MPU6050		// MPU-6050 6d accelero/gyro
  MENU.out(F("'Y'= ~AccelGyro "));	// TODO: UPDATE:
  MENU.out_ON_off(accGyro_is_active);
  MENU.ln();
#endif

  MENU.out(F("'I'= info"));

#if defined USE_MONOCHROME_DISPLAY
  MENU.tab();
  oled_ui_display();
#endif

  muting_actions_UI_line();

  MENU.ln(2);
  musicBox_short_info();

/*	*deactivated*
  MENU.outln(F("fart='f'"));
*/
  MENU.ln();
  stress_event_cnt = -3;	// heavy stress expected after musicBox_display
} // musicBox_display()

bool load_preset_and_start(short preset_new, bool start=true) {	// returns error
  short preset_was = musicBoxConf.preset;
  musicBoxConf.preset = preset_new;
  if(load_preset(musicBoxConf.preset)) {	// error?
    MENU.outln_invalid();
    musicBoxConf.preset = preset_was;
    return true;		// ERROR
  } // else

  if(start) {
    if(MusicBoxState != OFF)	// end a running session?
      tabula_rasa();
    start_musicBox();		// play new preset
  } else
    musicBox_short_info();
}

void input_preset_and_start() {	// factored out UI component	// TODO: sets preset, how to unset?	################
  int input_value;
  bool load=false;

  MENU.ln();
  MENU.out(F("PRESET "));
  switch(MENU.peek()) {
  case '+':
    MENU.drop_input_token();
    if(musicBoxConf.preset<MUSICBOX_PRESETs) {
      musicBoxConf.preset++;
      load = true;
    }
    break;
  case '-':
    MENU.drop_input_token();
    if(musicBoxConf.preset>0)
      musicBoxConf.preset--;
      load = true;
    break;

  default:	// numeric input (expected)
    if(MENU.is_numeric()) {
      if(input_value = MENU.numeric_input(0)) {
	musicBoxConf.preset = input_value;
	load = true;
      } else
	play_random_preset();	// selecting zero plays a *random* preset
    } else
      MENU.outln_invalid();
  }

  bool start = ! MENU.check_next('T');	// '<number>T' loads preset, but does not start

  if(load)
    load_preset_and_start(musicBoxConf.preset, start);
} // input_preset_and_start()


void sync_landscape_time_sliced() {	// set this instruments time slice
  if(my_IDENTITY.esp_now_time_slice != ILLEGAL8) {
    musicBoxConf.sync = my_IDENTITY.esp_now_time_slice;	// receiver sets sync to its individual time slice
    MENU.out(F("sync_landscape_time_sliced()  my sync "));
    MENU.outln(musicBoxConf.sync);
  }
}


bool Y_UI() {	// "eXtended motion UI" planed eXtensions: other input sources: ADC, 'analog'Touch, distance sensors, etc
#if defined USE_MPU6050		// MPU-6050 6d accelero/gyro
  // TODO: REPLACE BY NEW ENGINE ################################################################
  // TODO: monochrome feedback on motion UI switching
  bool switch_activity=false;
  bool do_next_letter=true;
  bool recognised = false;
  if(MENU.peek() == EOF8) {	// bare 'U'	switch_activity
    switch_activity = true;
    recognised = true;
  } else {
    while(do_next_letter) {	// a group of symbols as optional sequence toggling active axis
      switch(MENU.peek()) {	// second letter
      case '0':			// Y0 =	restart at zero		-.--  -----
      case '=':
	MENU.drop_input_token();
	accGyro_mode = 0;
	accGyro_is_active = false;
	recognised = true;
	break;
      case 'X':			// YX acc	toggle AG_mode_Ax	-.--  -..-
	MENU.drop_input_token();
	accGyro_mode ^= AG_mode_Ax;
	accGyro_is_active = accGyro_mode;
	if(!mpu6050_available)
	  accGyro_is_active = false;

	recognised = true;
	break;
      case 'Y':			// YY acc	toggle AG_mode_Ay	-.--  -.--
	MENU.drop_input_token();
	accGyro_mode ^= AG_mode_Ay;
	accGyro_is_active = accGyro_mode;
	if(!mpu6050_available)
	  accGyro_is_active = false;

	recognised = true;
	break;
      case 'Z':			// YZ GYRO	toggle AG_mode_Gz	-.--  --..
	MENU.drop_input_token();
	accGyro_mode ^= AG_mode_Gz;
	accGyro_is_active = accGyro_mode;
	if(!mpu6050_available)
	  accGyro_is_active = false;

	recognised = true;
	break;


      case 'P':	// 'UP' UI presets
	MENU.drop_input_token();
	if(accGyro_preset == 1) {
	  accGyro_preset = 2;
	  MENU.outln(F("TUNING"));
	} else {
	  accGyro_preset = 1;
	  MENU.outln(F("accGyro_preset 1"));
	}

	MENU.out(F("accGyro_preset "));
	MENU.outln(accGyro_preset);
	recognised = true;
	break;

      case 'T':	// 'UT' MPU6050 tuning	// 'UT' toggle gZ TUNING mode
	/*
	  TODO: planed:	 'UT' MPU6050 tuning mode   'UTZ' Zgyro || 'UTX' Xacc || 'UTY'Yacc
	*/
	MENU.drop_input_token();
	extern void accGyro_toggle_TUNING_mode();
	accGyro_toggle_TUNING_mode();
	break;

      default:
	do_next_letter = false;
      } // known second letters
    } // do_next_letter
  }

  if(switch_activity)
    accGyro_is_active ^= 1;
  if(!mpu6050_available)
    accGyro_is_active = false;


  if(accGyro_mode==0)		// deconfigured, so deactivate
    accGyro_is_active = false;

  display_accGyro_mode();
  MENU.tab();
  MENU.out_ON_off(accGyro_is_active);
  if(!mpu6050_available)
    MENU.out(F("\tMPU6050 not available"));
  MENU.ln();

  return recognised;
  // TODO: REPLACE BY NEW ENGINE until here

  //	// TEMPLATE:
/*
  'U'	master switch on/off motionUI	// (sampling might continue)
  '[U]X'	toggle X-axis configured actions, both ACCELERO & GYRO
  '[U]Y'	toggle Y-axis configured actions, both ACCELERO & GYRO
  '[U]Z'	toggle Z-axis configured actions, both ACCELERO & GYRO
  '[U]A'	toggle ACCELERO configured actions, all axes
  '[U]G'	toggle GYRO configured actions, all axes
*/
//	  case 'U': // 'U' UI configuration	motion UI
//	    {
//	      bool check_next=true;
//	      while(check_next) {
//		switch(MENU.peek()) {
//		case 'X': // 'UX'
//		  MENU.drop_input_token();
//		  MENU.error_ln(F("TODO: implement"));
//		  break;
//
//		case 'Y': // 'UY'
//		  MENU.drop_input_token();
//		  MENU.error_ln(F("TODO: implement"));
//		  break;
//
//		case 'Z': // 'UZ'
//		  MENU.drop_input_token();
//		  MENU.error_ln(F("TODO: implement"));
//		  break;
//
//		case 'A': // 'UA'
//		  MENU.drop_input_token();
//		  MENU.error_ln(F("TODO: implement"));
//		  break;
//
//		case 'G': // 'UG'
//		  MENU.drop_input_token();
//		  MENU.error_ln(F("TODO: implement"));
//		  break;
//
//		default: // EOF8 or unknown
//		  check_next = false;
//		} // peek second letter
//	      }
//	    }
//	//    {	// 'UP' (provisoric)
//	//    if(accGyro_preset == 1)
//	//      accGyro_preset = 2;
//	//    else
//	//      accGyro_preset = 1;
//	//
//	//    MENU.out(F("accGyro_preset "));
//	//    MENU.outln(accGyro_preset);
//	//    }
#else	// no MPU6050
  return false;
#endif
} // Y_UI()


bool musicBox_reaction(char token) {
  int input_value, cnt;
  char next_token;
  pulse_time_t T_scratch;

//  // check for numeric preset input first	(OVERRIDES pulse selection in pulses)
//  MENU.restore_input_token();
//  if(MENU.is_numeric()) {
//    input_preset_and_start();	// read preset number
//    return true;		// early exit
//  } else
//    MENU.drop_input_token();	// restore state as usual, token is read

  switch(token) {
  case '?': // musicBox_display();
    do_on_other_core(&musicBox_display);	// musicBox_display() on other core
    break;
  case ',': // show parameters		',,' shows parameters and parameters in source code format
    if (MENU.menu_mode)	{ // *exclude* special menu modes
      MENU.restore_input_token();
      return false;	// for other menu modes let pulses menu do the work ;)	// TODO: TEST:
    } else {
      musicBox_short_info();
      if(MENU.check_next(',')) {     // ',,' shows parameters and parameters in source code format
	MENU.ln();
	show_configuration_code();
      }
    }
    break;
  case 'X':
  case 'Y':
  case 'Z':
  case 'A':
  case 'G':
    if(Y_UI())
      return true;
    break;
  case 'a': // magic_autochanges    // 'a'
    MENU.out(F("autochanges"));
    toggle_magic_autochanges();
    MENU.out_ON_off(magic_autochanges);
    MENU.ln();
    break;
  case 'c': // show cycle
    show_cycle(CyclesConf.harmonical_CYCLE);
    break;
  case 'E': // 'E' (bare):  start_soft_ending(soft_end_days_to_live, soft_end_survive_level);
    if(MENU.check_next('F')) {			// case "EFx" configure function musicBox_when_done();
      if(MENU.peek() != EOF8) {
	switch(MENU.peek()) {
	case 'ED':		// TODO: huch? ################################################################
	  musicBox_when_done=&deep_sleep;		// "EFD" deep_sleep()
	  break;
	case 'EL':
	  musicBox_when_done=&light_sleep;	// "EFL" light_sleep()
	  break;
	case 'EH':
	  MENU.out(F("hibernate() *DEACTIVATED* "));		// "EFH" hibernate()	TODO: CRASH: DEBUG: ################
	  //musicBox_when_done=&hibernate;	// "EFH" hibernate()	TODO: CRASH: DEBUG: ################
	  // TODO: interface to int musicBox_pause_seconds, see: 'r'
	  break;
	case 'ER':
	  musicBox_when_done=&restart;		// "EFR" restart()

	  MENU.drop_input_token();		// "EFr<nn>" set musicBox_pause_seconds
	  input_value = MENU.numeric_input(musicBox_pause_seconds);	// drop 'r' first
	  if(input_value >= 0)
	    musicBox_pause_seconds = input_value;
	  MENU.restore_input_token();		// dummy, see below
	  break;
	case 'EU':
	  musicBox_when_done=&user;		// "EFU" user()
	  break;
	case 'EP':
	  musicBox_when_done=&random_preset;	// "EFP" random_preset()
	  break;
	default:
	  MENU.outln_invalid();
	}
	MENU.drop_input_token();
      } // else	// bare "EF"  just show configured function
      show_when_done_function();
      MENU.ln();
    } else	// plain 'E' start_soft_ending(...)
      start_soft_ending(MagicConf.soft_end_days_to_live, MagicConf.soft_end_survive_level);
    break;
  case 'd': // soft_end_days_to_live
    input_value = MENU.numeric_input(MagicConf.soft_end_days_to_live);
    if(input_value >= 0)
      MagicConf.soft_end_days_to_live = input_value;
    break;
  case 'l': // soft_end_survive_level
    input_value = MENU.numeric_input(MagicConf.soft_end_survive_level);
    if(input_value >= 0)
      MagicConf.soft_end_survive_level = input_value;
    break;

  case 'm': // mode
    switch(MENU.peek()) {	// second letter
    case 'M':	// 'mM' fully manual, like manual, but even more so ;)
      MENU.out(F("fully "));
      magic_autochanges = false;
    case 'm':	// 'mm' manual mode
      MENU.drop_input_token();
      parameters_by_user();
      musicBox_when_done = &user;
      musicBox_short_info();
      break;
    case 'A':	// 'mA'
    case 'a':	// 'ma'
      MENU.drop_input_token();
      musicBox_when_done = MUSICBOX_WHEN_DONE_FUNCTION_DEFAULT;
      if(!magic_autochanges)
	toggle_magic_autochanges();
      parameters_get_randomised();
      break;
    }	// known second letters
    break;

  case 'w': // soft_cleanup_minimal_fraction_weighting
    MagicConf.soft_cleanup_minimal_fraction_weighting = MENU.numeric_input(MagicConf.soft_cleanup_minimal_fraction_weighting);
    break;

  case 'H': // HARD_END_playing(true);
    HARD_END_playing(true);
    break;

#if defined USE_MPU6050		// MPU-6050 6d accelero/gyro
  case 'U': // Xmotion UI
    if(Y_UI())
      return true;		// note: EARLY RETURN	// TODO: hmmm?

    // TODO: VERBOSITY etc	// move this to Y_UI()
    display_accGyro_mode();
    MENU.tab();
    MENU.out_ON_off(accGyro_is_active);
    MENU.ln();
    break;
#endif

  case 'C': // 'C' hierarchy: esp now send or configure
#if defined USE_ESP_NOW
    // normal case is (second letter != 'C') (exclude 'CC...' and such:)
    //	esp_now send rest of the line,	see below

    // exclude special first 'CC...' 'EOF' '?' and such:
    switch(MENU.peek()) {	// second letter after 'CC...' configure esp_now sending
    case EOF8:  // bare 'C'	// *broadcast* to spread peer detection
      MENU.outln(F("esp_now_call_participants()"));
      esp_now_call_participants();
      break;

    case '?':	// 'CC?' == 'C?'
      MENU.drop_input_token();
      display_peer_ID_list();
      break;

    // 'CC' hierarchy
    case 'C':	// 'CC' second letter: 'CC...' configure esp_now sending
      MENU.drop_input_token();

      if(MENU.peek() == EOF8) {	// bare 'CC'
	MENU.outln(F("esp_now_call_participants()"));
	esp_now_call_participants();

      } else if(MENU.check_next('?')) {		// 'CC?' == 'C?'
	display_peer_ID_list();

	} else if(MENU.check_next('S')) {	// 'CCS' set sync of *other* instruments to time slice,	see: 'CCSS'
	MENU.outln(F("set sync of instruments to time slice "));
	esp_now_send_bare(broadcast_mac, N_ST);

	if(MENU.check_next('S')) {		// 'CCSS' set *all* instruments (including sender) to time slice sync
	  MENU.out(F("sync =\t"));
	  if(my_IDENTITY.esp_now_time_slice != ILLEGAL8)
	    musicBoxConf.sync=my_IDENTITY.esp_now_time_slice;	// include *sender* in time sliced landscape
	  else
	    MENU.out(F("*no* sender time slice\t"));

	  MENU.outln(musicBoxConf.sync);
	}

      } else {	// more input after 'CC'
	if(MENU.is_numeric()) {	// 'CC<numeric>'
	  /*
	   *ATTENTION* UI numbering is one more as the field index:
	   UI 0 means NULL pointer		   alias 'known_peers_mac_p'
	   UI 1 means esp_now_pulses_known_peers[0]  == broadcast
	   UI 2 means first other			   individual "2"
	  */
	  input_value = MENU.numeric_input(0);		// default to broadcast
	  if(input_value == 0) {
	    esp_now_send2_mac_p = known_peers_mac_p;	// NULL == *all known* peers
	  } else if((input_value <= ESP_NOW_MAX_TOTAL_PEER_NUM) && (input_value > 0)) {
	    esp_now_send2_mac_p = esp_now_pulses_known_peers[input_value -1 ].mac_addr;
	  } else {
	    MENU.outln_invalid();
	    display_peer_ID_list();
	  }

	} else {	// other *NON NUMERIC* input after 'CC<xxx>'	TODO: search preName
	  MENU.out("DADA implement peer name search?\t");
/*
	  char c = MENU.peek();
	  if(c != EOF8) {
	    MENU.out(F("skipping "));
	    while (c != EOF8)
	      MENU.out(c = MENU.drop_input_token());
	    MENU.ln();
	  }
*/
	  MENU.ln();
	} // numeric/non numeric after 'CC'
      } // more input after 'CC'
      break;

    default:	// normal case (second letter != 'C' (from 'CC')
      {
	if(MENU.maybe_display_more(VERBOSITY_LOWEST) || DEBUG_ESP_NOW_b)
	  MENU.out(F("send NOW\t"));

	int len=0;
	char c;
	for(;len < 64; len++) {
	  if(MENU.peek(len) == EOF8)
	    break;
	}
	char* macro = (char*) malloc(len + 1);

	int i;
	for (i=0; i<len; i++)
	  *(macro + i) = (char) MENU.drop_input_token();
	*(macro + i) = '\0';

	esp_now_send_maybe_do_macro(esp_now_send2_mac_p, macro);
	free(macro);
      }
    } // switch(MENU.peek())	second letter after 'C'

#else	// ! defined USE_ESP_NOW
    {
      MENU.out(F("ESP_NOW not available"));
      char c = MENU.peek();
      if(c != EOF8) {
	MENU.out(F("skipped: "));
	while(c != EOF8) {
	  MENU.out(c);
	  c = MENU.drop_input_token();
	}
      }
      MENU.ln();
    }
#endif	// defined USE_ESP_NOW
    break;

  case 'M':	// 'M<x>' mute actions
    muting_actions_UI();
    break; // 'M<x>'

  case 'N':	// 'N' 'n' restart now	(like menu pulses 'n')
  case 'n':	// 'N' 'n' restart now	(like menu pulses 'n')
    if(musicBox_butler_i != ILLEGAL32)	// remove butler
      PULSES.init_pulse(musicBox_butler_i);

    if(PULSES.how_many_selected() == 0)	//	looks dead
      start_musicBox();			//	TODO: just start?  or better select n & butler, then continue?

    else {	// something *was* selected before, activate and sync it
      for(int pulse=0; pulse<PL_MAX; pulse++)
	if (PULSES.pulse_is_selected(pulse))
	  PULSES.pulses[pulse].counter = 0;	// reset counters of selected pulses

      set_MusicBoxState(AWAKE);
      musicBox_start_time = PULSES.get_now();

      PULSES.activate_selected_synced_now(musicBoxConf.sync);	// sync and activate

      // the butler starts just a pad *after* musicBox_start_time
      musicBox_butler_i =						\
	PULSES.setup_pulse(&musicBox_butler, ACTIVE, PULSES.get_now(), slice_tick_period);
      PULSES.pulses[musicBox_butler_i].groups |= g_MASTER;	// savety net, until butler has initialised itself
    }

    stress_event_cnt = -3;	// some stress events will often happen after starting the musicBox
    break;

  case '&': // TODO: review, fix cycle_slices
    if((input_value = MENU.numeric_input(cycle_slices) > 0)) {
      set_cycle_slice_number(input_value);
      PULSES.pulses[musicBox_butler_i].period = slice_tick_period;	// a bit adventurous ;)
      // TODO: feedback
    }
    break;

  case 'o': // show_subcycle_position
    show_subcycle_position ^= 1 ;
    if(MENU.verbosity > VERBOSITY_LOWEST) {
      if(show_subcycle_position)
	MENU.out(F("SHOW"));
      else
	MENU.out(F("do *not show*"));
      MENU.outln(F(" position in circle"));
    }
    break;

//  case 'L': // 'L' == stop_on_LOW_H1()  ||  "LL" == stop_on_LOW()/*only*/)	// TODO: REMOVE?
//    if(MENU.peek() != 'L') {	// 'L' case	stop_on_LOW_H1()
//      MENU.out(stop_on_LOW_H1());
//      MENU.outln(F(" were high "));
//    } else {			// "LL" case	stop_on_LOW()/*only*/
//      MENU.drop_input_token();
//      MENU.out(stop_on_LOW()/*only*/);
//      MENU.outln(F(" were high "));
//    }
//    break;

  case 'L':	 // 'L' RGB LED STRING
#if defined USE_RGB_LED_STRIP
    rgb_led_string_UI();
#else
    MENU.outln(F("*NO* rgb led string code"));
#endif
    break;

  case 'P': // 'P' Play start/stop
    switch(MENU.peek()) {
    case '1':	// 'P1' kill primary pulses
      MENU.drop_input_token();
      cnt = kill_primary();
      if(MENU.maybe_display_more(VERBOSITY_LOWEST)) {
	MENU.out(F("kill primary "));
	MENU.outln(cnt);
      }
      break;
    case '2':	// 'P2' kill secondary pulses
      MENU.drop_input_token();
      cnt = kill_secondary();
      if(MENU.maybe_display_more(VERBOSITY_LOWEST)) {
	MENU.out(F("kill secondary "));
	MENU.outln(cnt);
      }
      break;
    default:	// 'P' start/stop musicBox
      {
	bool start=false;
	if(MusicBoxState != OFF)
	  tabula_rasa();	// MusicBoxState is OFF now
	else
	  start=true;

	if(MENU.maybe_display_more(VERBOSITY_SOME))
	  musicBox_display();

#if defined USE_MONOCHROME_DISPLAY
	MC_show_musicBox_parameters();
#endif
	if(start)
	  start_musicBox();
      }
    } // switch(MENU.peek())
    break;

  case 'I':	// info		// force OLED resdisplay or morse	// TODO: display help
    switch(MENU.peek()) {
    case '?':	// 'I?'
      MENU.drop_input_token();
    case EOF8:	// bare 'I'
      musicBox_short_info();
#if defined USE_MONOCHROME_DISPLAY
      MC_show_musicBox_parameters();
#endif
      break;

    case 'N':	// 'IN'	prename, name, preset#
      MENU.drop_input_token();
      MC_show_names();
      break;

#if defined USE_ESP_NOW
    case 'C':	// 'IC'	peer list
      MENU.drop_input_token();
      for(int row=0, i=0; i<ESP_NOW_MAX_TOTAL_PEER_NUM; i++) {
	if(mac_is_non_zero(esp_now_pulses_known_peers[i].mac_addr)) {
	  MENU.out(i + 1);	// start counting with 1
	  if(i<9)
	    MENU.space();
	  MENU.space(2);
	  extended_output(esp_now_pulses_known_peers[i].preName, 0, 2*row, true);
	  row++;
	  MENU.ln();
	}
      }
      break;
#endif

#if defined USE_MONOCHROME_DISPLAY
    case 'O': // 'IO' clear OLED	// TODO: OBSOLETE?
      MENU.drop_input_token();
      MC_clear_display();
      break;

    case 'R':	// 'IR' == 'IT' == 'IJ'
    case 'T':	// 'IR' == 'IT' == 'IJ'
    case 'J':	// 'IR' == 'IT' == 'IJ'
      MENU.drop_input_token();
      {
	char* str;
	str = array2name(SCALES, selected_in(SCALES));
	monochrome_big_or_multiline(0, str);
	MENU.outln(str);

	str = array2name(JIFFLES, selected_in(JIFFLES));
	monochrome_big_or_multiline(3, str);
	MENU.outln(str);
	// TODO: monochrome metric_mnemonic 'IR...'
      }
      break;

  #if defined COMPILE_MORSE_CHEAT_SHEETS
    case 'M':	// 'IM'
      MENU.drop_input_token();
      morse_cheat_sheets_UI();
      break;
  #endif // COMPILE_MORSE_CHEAT_SHEETS

#endif // USE_MONOCHROME_DISPLAY
    } // switch token after 'I'
    break;

  case 'v':
    if(peripheral_power_on)
      peripheral_power_switch_OFF();
    else
      peripheral_power_switch_ON();
    break;

  case 'W':
    if(MENU.is_numeric()) {	// "P<number>" wait <number> seconds before starting musicBox, *see below*
      musicBox_short_info();
      input_value = MENU.numeric_input(-1);	// *do* read number anyway
      if(MusicBoxState == OFF) {	// act *only* if musicBox is *not* running
	if(input_value > 0)			// delay(sic!) *only* if positive
	  delay(input_value * 1000);		// FIXME:  quickHACK: using delay as nothing is running, *probably* ;)
	start_musicBox();
      } // ignore if musicBox is running, hmm (there *could* be a use case while it is running...)
    } // not numeric, ignore
    break;

  case 'p': // 'p' switch cycle pattern display
    if(MENU.check_next('*')) {	// 'p*' toggle interval symbols in cycle pattern
      show_cycle_pattern_intervals ^= 1;
      show_cycle_pattern = true;
    } else
      show_cycle_pattern ^= 1;

    if(MENU.verbosity > VERBOSITY_LOWEST) {
      if(show_cycle_pattern)
	MENU.out(F("SHOW"));
      else
	MENU.out(F("do *not show*"));
      MENU.outln(F(" cycle pattern"));
    }
    break;

  case 'R':	// OBSOLETE?: ################
    select_scale__UI();		// TODO: see pulses.ino 'R'	make them identical? ################	// OBSOLETE?:
    MENU.out(F("SCALE: "));
    MENU.outln(selected_name(SCALES));
    break;

  case 'T':	// Tuning pitch and scale
    tuning_pitch_and_scale_UI();
    break;

  // TODO: REPLACE: this is just a *proof of concept* test, works, but is not usable ################
  case 's':	// in musicBox toggle voice groups 'sb' 'sm' 'st'	// TODO: OBSOLETE: rework or remove
    {
      int start=0;
      int end =0;
      switch(MENU.peek()) {
      case 'b':	// 'sb' toggle bottom voices
	start = musicBoxConf.lowest_primary;
	end = start + 7;	// TODO: FIXME: 7 just a test
	break;
      case 'm':	// 'sm' toggle middle voices
	start = musicBoxConf.lowest_primary + 7;
	end = musicBoxConf.highest_primary - 7;	// TODO: FIXME: 7 just a test
	break;
      case 't':	// 'st' 'sh' toggle top=high voices
      case 'h':	// 'st' 'sh' toggle top=high voices
	start = musicBoxConf.highest_primary -7 ;	// TODO: FIXME: 7 just a test
	end = musicBoxConf.highest_primary;
	break;
      case '~':	// 's~' toggle all voices
	start = musicBoxConf.lowest_primary;
	end = musicBoxConf.highest_primary;
	break;

      default:	// EOF8 or invalid
	MENU.restore_input_token();
	return false;	// bare 's' see: pulses menu	*RETURN*
	break;
      }
      // invalid input and EOF do *not* get here
      MENU.drop_input_token();	// second letter from above
      for(int pulse = start; pulse <= end; pulse++)
	PULSES.pulses[pulse].action_flags ^= noACTION;		// toggle action_flags

      if(MENU.verbosity >= VERBOSITY_LOWEST) {
	MENU.out(F("toggled pulses "));
	MENU.out(start);
	MENU.out(F(" to "));
	MENU.outln(end);
      }
    }
    break;

  case 't':	// tuning: toggle some_metric_tunings_only
    MENU.out(F("fixed metric tunings"));
    MENU.out_ON_off(MagicConf.some_metric_tunings_only = !MagicConf.some_metric_tunings_only);
    MENU.ln();
    break;

  case 'F':	// freeze-unfreeze parameters
    if(scale_user_selected && sync_user_selected && jiffle_user_selected && pitch_user_selected && stack_sync_user_selected /* && uiConf.subcycle_user_selected*/) {
      parameters_get_randomised();
    } else {
      parameters_by_user();
      musicBox_short_info();
    }
    break;

  case 'O':	// 'O<x>' OLED_UI()	or:  'O' subcycle_octave TODO: what to do while playing???
#if defined USE_MONOCHROME_DISPLAY
    extern bool OLED_UI();
    if(OLED_UI())
      return true;
#endif

    if(MENU.check_next('+')) {		// higher octave is shorter
      PULSES.div_time(&CyclesConf.used_subcycle,2);
      CyclesConf.subcycle_octave++;
      //uiConf.subcycle_user_selected=true;
    } else {				// default and '-' is longer
      PULSES.mul_time(&CyclesConf.used_subcycle,2);
      CyclesConf.subcycle_octave--;
      MENU.check_next('-');		// '-' is default, drop it
      //uiConf.subcycle_user_selected=true;
    }
    show_cycles_1line();

    set_cycle_slice_number(cycle_slices);	// make sure slice_tick_period is ok
    break;

/*
  void furzificate() {	// switch to a quiet, farting patterns, u.a.
  very simple one shot implementation
  does not work as is with musicBox any more
  adapt or remove?

  case'f':	// furzificate
    furzificate();
    break;
*/

  case '|':	// '|' stack_sync_slices	(and '|b' base_pulse)
    if(MENU.check_next('b')) {		// '|b' base_pulse
      input_value = MENU.numeric_input(musicBoxConf.base_pulse);
      if(input_value >= 0 && input_value < PL_MAX) {
	musicBoxConf.base_pulse = input_value;
	stack_sync_user_selected=true;	// RETHINK: maybe, maybe not?
	not_a_preset();
      }
    } else {	// bare '|'
      short input_value = MENU.numeric_input(musicBoxConf.stack_sync_slices);
      musicBoxConf.stack_sync_slices = input_value;
      stack_sync_user_selected=true;
      not_a_preset();
    }

    if(MENU.verbosity >= VERBOSITY_LOWEST) {
      MENU.out(F("stacked_sync | "));
      MENU.out(musicBoxConf.stack_sync_slices);
      MENU.out(F("  base_pulse "));
      MENU.outln(musicBoxConf.base_pulse);
    }
    break;

  // special case number only: play preset	(OVERRIDES pulse selection in pulses)
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
    MENU.restore_input_token();	// restore first chiffre
  case 'y':
    input_preset_and_start();	// reads preset number
    break;

  case 'D':		// musicBox 'D'
    {
      #include "men_MuD.h"
    }
    break;

  default:
    return false;
  } // switch(token)

  // note: Y_UI() could have already returned
  return true;
} // musicBox_reaction()
