/*
  musicBox.h
*/

#define MUSICBOX_VERSION	alpha 0.007++

// PRESETS: uncomment *one* (or zero) of the following setups:
#define SETUP_BRACHE				BRACHE_2018-12
//#define SETUP_BAHNPARKPLATZ	BahnParkPlatz 18
//#define SETUP_CHAMBER_ORCHESTRA	The Harmonical Chamber Orchestra 2018-12


// pre defined SETUPS:
#if defined SETUP_BRACHE
  #define MUSICBOX_SUB_VERSION			SETUP_BRACHE
  #define AUTOMAGIC_CYCLE_TIMING_SECONDS	7*60	// *max seconds*, produce short sample pieces	BRACHE Dez 2018
  #define MUSICBOX_HARD_END_SECONDS		15*60	// SAVETY NET, we're low on energy...
  #define MUSICBOX_TRIGGER_BLOCK_SECONDS	30	// BRACHE

#elif defined SETUP_BAHNPARKPLATZ
  #define MUSICBOX_SUB_VERSION			SETUP_BAHNPARKPLATZ
  #define AUTOMAGIC_CYCLE_TIMING_SECONDS	12*60	// *max seconds*, produce sample pieces		BahnParkPlatz 18
  #define MUSICBOX_TRIGGER_BLOCK_SECONDS	13	// BahnParkPlatz

#elif defined SETUP_CHAMBER_ORCHESTRA
  #define MUSICBOX_SUB_VERSION			SETUP_CHAMBER_ORCHESTRA

  //#define AUTOMAGIC_CYCLE_TIMING_SECONDS	21*60	// *max seconds*, produce sample pieces   The Harmonical Chambre Orchestra
  #define AUTOMAGIC_CYCLE_TIMING_SECONDS	12*60	// *max seconds*, produce sample pieces   The Harmonical Chambre Orchestra
  //#define AUTOMAGIC_CYCLE_TIMING_SECONDS	7*60	// DEBUG *max seconds*, produce sample pieces   The Harmonical Chambre Orchestra

  #define MUSICBOX_TRIGGER_BLOCK_SECONDS	1	// debounce only

#endif	// (pre defined setups)

#if ! defined AUTOMAGIC_CYCLE_TIMING_SECONDS
  //#define AUTOMAGIC_CYCLE_TIMING_SECONDS	7*60	// *max seconds*, produce short sample pieces	BRACHE Dez 2018
  //#define AUTOMAGIC_CYCLE_TIMING_SECONDS	12*60	// *max seconds*, produce sample pieces		BahnParkPlatz 18
  #define AUTOMAGIC_CYCLE_TIMING_SECONDS	18*60	// *max seconds*, produce moderate length sample pieces  DEFAULT
  //#define AUTOMAGIC_CYCLE_TIMING_SECONDS	65*60	// *max seconds*, sets performance timing based on cycle
#endif

// #define SOME_FIXED_TUNINGS_ONLY		// fixed pitchs only like E A D G C F B  see: HACK_11_11_11_11


#include <esp_sleep.h>
// #include "rom/gpio.h"
// #include "driver/gpio.h"

#include "random_entropy.h"

#if defined PERIPHERAL_POWER_SWITCH_PIN
  #include "peripheral_power_switch.h"
#endif

#ifndef MUSICBOX_TRIGGER_PIN
  #define MUSICBOX_TRIGGER_PIN	34
#endif

#ifndef MUSICBOX_PERFORMACE_SECONDS
  #define MUSICBOX_PERFORMACE_SECONDS	12*60
#endif

#if ! defined MUSICBOX_TRIGGER_BLOCK_SECONDS
  #define MUSICBOX_TRIGGER_BLOCK_SECONDS	3	// just playing, DEBUGGING
#endif

#if ! defined MUSICBOX_HARD_END_SECONDS		// savety net
  #if defined AUTOMAGIC_CYCLE_TIMING_SECONDS
    #define  MUSICBOX_HARD_END_SECONDS	(AUTOMAGIC_CYCLE_TIMING_SECONDS*2)	// TODO: first try, FIXME: determine at run time
  #else
    #define  MUSICBOX_HARD_END_SECONDS	90*60	// TODO: review that
  #endif
#endif

#if ! defined MUSICBOX_ENDING_FUNCTION	// *one* of the following:
//  #define  MUSICBOX_ENDING_FUNCTION	light_sleep();	// works fine

#if defined HACK_11_11_11_11		// never ending jam session
  #define  MUSICBOX_ENDING_FUNCTION	;	// just deactivated ;) 11.11.
#else
  void light_sleep();	// pre declaration
  #define  MUSICBOX_ENDING_FUNCTION	light_sleep();	// works fine as default for triggered musicBox

  void start_musicBox();	// pre declaration
  //#define  MUSICBOX_ENDING_FUNCTION	delay(6*1000); start_musicBox();	// sound recording loop?
#endif

  //#define  MUSICBOX_ENDING_FUNCTION	deep_sleep();	// still DAC noise!!!
  //#define  MUSICBOX_ENDING_FUNCTION	ESP.restart();	// works fine
#endif


struct time musicBox_start_time;
struct time musicBox_hard_end_time;

// struct time harmonical_CYCLE;	// is in pulses.ino now TODO: move to Harmonical?
struct time used_subcycle;	// TODO: move to Harmonical? ? ?

/*
  select something like
   unsigned short cycle_slices = 72;	// test&adapt   classical aesthetics?
   unsigned short cycle_slices = 120;	// test&adapt   classical aesthetics?
   unsigned short cycle_slices = 90*3;	// test&adapt   simplified keeping important details?
   unsigned short cycle_slices = 180*3;	// test&adapt	interesting lot of detail
   unsigned short cycle_slices = 180;	// test&adapt	sometimes less is more
   unsigned short cycle_slices = 360;	// test&adapt   classical aesthetics?
*/
unsigned short cycle_slices = 540;	// *DO NOT SET DIRECTLY* use set_cycle_slice_number(n);
//unsigned short cycle_slices = 540*3;	// *DO NOT SET DIRECTLY* use set_cycle_slice_number(n);

struct time slice_tick_period;	// *DO NOT SET DIRECTLY* use set_cycle_slice_number(n);

void set_cycle_slice_number(short ticks_a_cycle) {
  cycle_slices = ticks_a_cycle;
  slice_tick_period = used_subcycle;
  PULSES.div_time(&slice_tick_period, cycle_slices);
}

// remember pulse index of the butler, so we can call him, if we need him ;)
int musicBox_butler_i=ILLEGAL;	// pulse index of musicBox_butler(p)

// some pre declarations:
void musicBox_butler(int);
void magical_butler(int);	// old version, obsolete?


// MusicBoxState
enum musicbox_state_t {OFF=0, ENDING, SLEEPING, SNORING, AWAKE, FADE};
musicbox_state_t MusicBoxState=OFF;
char * MusicBoxState_name;
void set_MusicBoxState(musicbox_state_t state) {	// sets the state unconditionally
  int butler_survivors=0;	// checks for stray butler pulses when state switched to OFF

  switch (state) {			// initializes state if necessary
  case OFF:
    MusicBoxState_name = F("OFF");

    // control if the butler is still running || musicBox_butler_i != ILLEGAL
    if(musicBox_butler_i != ILLEGAL) {	// musicBox_butler(p) seems running?
      if(PULSES.pulses[musicBox_butler_i].periodic_do == &musicBox_butler) {
	PULSES.init_pulse(musicBox_butler_i);
	if(MENU.maybe_display_more(VERBOSITY_MORE))
	  MENU.outln(F("set_MusicBoxState: butler removed"));	// butler still running on musicBox_butler_i
      } else {
	if(MENU.maybe_display_more(VERBOSITY_SOME)) {
	  MENU.out(F("butler not at id "));			// musicBox_butler_i was set, but no butler there
	  MENU.outln(musicBox_butler_i);
	}
      }
      musicBox_butler_i = ILLEGAL;				// invalidate pulse index of musicBox_butler(p)
    }

    // test if there is really no butler left:
    for(int p=0; p<PL_MAX; p++) {
      if(PULSES.pulses[musicBox_butler_i].periodic_do == &musicBox_butler)
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

// TODO: check&fix
#if defined HACK_11_11_11_11	// magic_autochanges=false;
  bool magic_autochanges=false;	// never ending jam sessions...
#else
  bool magic_autochanges=true;	// switch if to end normal playing after MUSICBOX_PERFORMACE_SECONDS
#endif


unsigned long primary_counters[PL_MAX] = { 0 };	// preserve last seen counters

void init_primary_counters() {
  memset(&primary_counters, 0, sizeof(primary_counters));
}

bool show_cycle_pattern=true;

int lowest_primary=ILLEGAL, highest_primary=ILLEGAL;	// remember start configuration

void set_primary_block_bounds() {	// remember where the primary block starts and stops
  lowest_primary=ILLEGAL;
  for(int pulse=0; pulse<PL_MAX; pulse++) {
    if(PULSES.pulses[pulse].groups & g_PRIMARY) {
      if(lowest_primary == ILLEGAL)
	lowest_primary = pulse;
      highest_primary = pulse;
    }
  }
}

void watch_primary_pulses() {
  long diff;
  int primary_cnt=0;
  int secondary_cnt=ILLEGAL;

  for(int pulse=highest_primary; pulse>=lowest_primary; pulse--) {
    if(PULSES.pulses[pulse].groups & g_PRIMARY) {
      primary_cnt++;
      diff = PULSES.pulses[pulse].counter - primary_counters[pulse];	// has the counter changed?
      primary_counters[pulse]=PULSES.pulses[pulse].counter;		// update to new counter
      if(show_cycle_pattern) {
	switch(diff) {
	case 0:		// no change
	  if(PULSES.pulses[pulse].flags & COUNTED)
	    MENU.out(PULSES.pulses[pulse].remaining);	// counted pulse waiting for it's turn
	  else
	    MENU.space();
	  break;
	case 1:		// primary pulse was called *once*
	  MENU.out('*');
	  break;
	default:
	  if(diff > 1)	// slice too long to see all the individual wakeups
	    MENU.out(diff);
	  else	// could possibly happen after some reset???
	    MENU.out(diff);	// DEBUGGING: avoid that to happen
	}
      } // show pattern
    } else	// is *not* g_PRIMARY
      MENU.out('-');	// was removed, replaced or something
  }

  if(primary_cnt == 0 || MENU.verbosity >= VERBOSITY_SOME) {
    secondary_cnt=0;

    for(int pulse=0; pulse < PL_MAX; pulse++) {
      if(PULSES.pulses[pulse].groups & g_SECONDARY)
	secondary_cnt++;
    }

    MENU.out(F(" S("));
    MENU.out(secondary_cnt);
    MENU.out(')');
  }
}

void show_cycles_1line() {	// no scale, no cycle
  if(selected_in(SCALES)==NULL) {
    MENU.outln(F("no scale, no cycle"));
    return;
  } // else

  // this version assumes harmonical_CYCLE and used_subcycle *are* set
  MENU.out(F("harmonical CYCLE: "));
  PULSES.display_time_human(harmonical_CYCLE);
  MENU.out(F("\tSUBCYCLE: | "));
  PULSES.display_time_human(used_subcycle);
  MENU.outln('|');

  // TODO: show current position (float)
  return;
}

void show_cycle(struct time cycle) {
  MENU.out(F("\nharmonical cycle:  "));
  if(selected_in(SCALES)==NULL) {
    MENU.outln(F("no scale, no cycle"));
    return;
  }

  int lowest_primary=ILLEGAL, highest_primary=ILLEGAL;
  for(int pulse=0; pulse<PL_MAX; pulse++) {
    if(PULSES.pulses[pulse].groups & g_PRIMARY) {
      if(lowest_primary == ILLEGAL)
	lowest_primary = pulse;
      highest_primary = pulse;
    }
  }

  PULSES.display_time_human(cycle);
  MENU.ln();

  struct time period_highest = PULSES.pulses[highest_primary].period;
  struct time shortest = scale2harmonical_cycle(selected_in(SCALES), &period_highest);

  if(MENU.verbosity >= VERBOSITY_MORE) {
    MENU.out(F("shortest pulse's harmonical cycle: "));
    PULSES.display_time_human(shortest);
  }
  MENU.ln();

  int i=0;
  //                                   !!!  a tolerance of 128 seemed *not* to be enough
  while(cycle.time >= (used_subcycle.time - 256/*tolerance*/) || \
	cycle.time >= (shortest.time - 256/*tolerance*/) || \
	cycle.overflow)		// display cycle and relevant octaves
    {
      if(cycle.time == used_subcycle.time)
	MENU.out('|');

      MENU.out(F("(2^"));
      MENU.out(i--);
      MENU.out(F(") "));
      PULSES.display_time_human(cycle);

      if(cycle.time == used_subcycle.time)
	MENU.out('|');

      if(i % 4)	// 4 items a line
	MENU.tab();
      else
	MENU.ln();

      PULSES.div_time(&cycle, 2);
    }

  if(i % 4)	// 4 items a line
    MENU.ln();
}

int slice_weighting(fraction F) {
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

  if(HARMONICAL.is_small_prime(F.multiplier))
    weighting++;

  return weighting;
}


// cycle_monitor(p)  payload to give infos where in the cycle we are
bool show_subcycle_position=true;
int cycle_monitor_i=ILLEGAL;	// pulse index of cycle_monitor(p)

void cycle_monitor(int pulse) {	// show markers at important cycle divisions
  static unsigned short cycle_monitor_last_seen_division=0;
  if(PULSES.pulses[pulse].counter == 1)
    cycle_monitor_last_seen_division =  0;

  struct time this_time = PULSES.get_now();
  PULSES.sub_time(&PULSES.pulses[pulse].last, &this_time);	// so long inside this cycle
  /* TESTED: works fine disregarding overflow :)
  if(this_time.overflow != PULSES.pulses[pulse].last.overflow)
    MENU.outln(F("over"));
  */
  fraction phase = {this_time.time, PULSES.pulses[pulse].period.time};
  // float float_phase = this_time.time / PULSES.pulses[pulse].period.time;	// not used
  fraction this_division = {cycle_monitor_last_seen_division, cycle_slices};
  HARMONICAL.reduce_fraction(&this_division);

  if(show_subcycle_position /*&& slice_weighting(this_division) > 0*/) {	// weighting influence switched off
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

    MENU.out((float) this_division.multiplier/this_division.divisor, 6);
    MENU.space(2);

    if(MENU.verbosity >= VERBOSITY_SOME)
      MENU.out(slice_weighting(this_division));
  }

  cycle_monitor_last_seen_division++;
  cycle_monitor_last_seen_division %= cycle_slices;
}


bool go_light_sleep=false;	// triggers MUSICBOX_ENDING_FUNCTION;	// sleep, restart or somesuch

int soft_cleanup_minimal_fraction_weighting=1;	// TODO: adjust default
unsigned short soft_end_days_to_live = 1;	// remaining days of life after soft end
unsigned short soft_end_survive_level = 4;	// the level a pulse must have reached to survive soft end
struct time soft_end_start_time;
unsigned long soft_end_cleanup_wait=60*1000000L;	// default 60"
void soft_end_playing(int days_to_live, int survive_level) {	// initiate soft ending of musicBox
/*
  void soft_end_playing(int days_to_live, int survive_level);

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
  /*	*no* let it always *try* to work...
  if(MusicBoxState == OFF)
    return;
  */

  if(MusicBoxState > ENDING) {		// initiate end
    soft_end_start_time = PULSES.get_now();
    set_MusicBoxState(ENDING);

    if(MENU.verbosity >= VERBOSITY_LOWEST) {
      struct time main_part_duration = soft_end_start_time;
      PULSES.sub_time(&musicBox_start_time, &main_part_duration);
      MENU.out(F("main part "));
      PULSES.display_time_human(main_part_duration);
      MENU.ln();

      MENU.out(F("soft_end_playing("));		// info
      MENU.out(soft_end_days_to_live);
      MENU.out(F(", "));
      MENU.out(soft_end_survive_level);
      MENU.outln(')');
    }

    for (int pulse=0; pulse<PL_MAX; pulse++) {	// make days_to_live COUNTED generating pulses
      if (PULSES.pulse_is_selected(pulse)) {
	if(PULSES.pulses[pulse].counter > survive_level) {	// pulse was already awake (long enough)?
	  PULSES.pulses[pulse].remaining = days_to_live;	//   still repeat, then vanish
	  PULSES.pulses[pulse].flags |= COUNTED;
	} else {
	  PULSES.init_pulse(pulse);			// unborn pulse or too young, just remove
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
	struct time play_time = PULSES.get_now();
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

      reset_all_flagged_pulses_GPIO_OFF();

      /* ???
      if(MENU.verbosity)
	MENU.ln();
      */
      go_light_sleep = true;	// triggers MUSICBOX_ENDING_FUNCTION;	// sleep, restart or somesuch	*ENDED*
    }
  }
}

void HARD_END_playing(bool with_title) {	// switch off peripheral power and hard end playing
  if(with_title)	// TODO: maybe use MENU.verbosity, but see also 'o'
    MENU.outln(F("HARD_END_playing()"));

  if(MusicBoxState != OFF)
    set_MusicBoxState(OFF);

  if (MENU.verbosity){
    struct time play_time = PULSES.get_now();
    PULSES.sub_time(&musicBox_start_time, &play_time);
    MENU.out(F("played "));
    PULSES.display_time_human(play_time);
    MENU.ln();

    show_UI_basic_setup();
    show_cycles_1line();
    MENU.ln();
  }

  delay(777);

#if defined PERIPHERAL_POWER_SWITCH_PIN
  MENU.out(F("peripheral POWER OFF "));
  MENU.out(PERIPHERAL_POWER_SWITCH_PIN);
  MENU.tab();
  MENU.outln(read_battery_level());
  peripheral_power_switch_OFF();
  delay(800);	// let power go down softly
#endif

  reset_all_flagged_pulses_GPIO_OFF();
  musicBox_butler_i = ILLEGAL;
  set_MusicBoxState(OFF);
  MENU.ln();

  go_light_sleep = true;	//  triggers MUSICBOX_ENDING_FUNCTION;	sleep, restart or somesuch	// *ENDED*
}

portMUX_TYPE musicBox_trigger_MUX = portMUX_INITIALIZER_UNLOCKED;

void musicBox_trigger_ON();	// forward declaration
bool musicBox_trigger_enabled=false;
bool blocked_trigger_shown=false;	// show only once a run

void activate_musicBox_trigger(int dummy_p) {
  musicBox_trigger_enabled = true;
  if(MENU.verbosity >= VERBOSITY_LOWEST)
    MENU.outln(F("trigger enabled"));
}


unsigned int musicbox_trigger_cnt=0;
void musicBox_trigger_OFF();

void musicBox_trigger_ISR() {	// can also be used on the non interrupt version :)
  portENTER_CRITICAL_ISR(&musicBox_trigger_MUX);
  static struct time triggered_at=PULSES.get_now();	// preserves last seen fresh trigger time

  struct time new_trigger = PULSES.get_now();		// new trigger time
  struct time duration = new_trigger;			// remember

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
  MENU.out(MUSICBOX_TRIGGER_PIN);
  MENU.tab();
  MENU.outln(musicbox_trigger_cnt);
  pinMode(MUSICBOX_TRIGGER_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(MUSICBOX_TRIGGER_PIN), musicBox_trigger_ISR, RISING);
#else
  ;
#endif
}

// TODO: ################  DOES NOT WORK: isr is *not* called any more BUT THE SYSTEM BECOMES STRESSED!
void musicBox_trigger_OFF() {
#if ! defined MAGICAL_TOILET_HACKS	// some quick dirty hacks *NOT* using the interrupt
  MENU.outln(F("musicBox_trigger_OFF\t"));
  detachInterrupt(digitalPinToInterrupt(MUSICBOX_TRIGGER_PIN));
  //  esp_intr_free(digitalPinToInterrupt(MUSICBOX_TRIGGER_PIN));
#endif
  musicBox_trigger_enabled=false;
}


#if defined MAGICAL_TOILET_HACKS	// some quick dirty hacks
void start_musicBox();			// forward declaration
//
void musicBox_trigger_got_hot() {	// must be called when magical trigger was detected high
  if(musicBox_trigger_enabled) {
    musicBox_trigger_ISR();	// *not* as ISR
    MENU.outln(F("\nTRIGGERED!"));
    start_musicBox();
#if defined PERIPHERAL_POWER_SWITCH_PIN
    peripheral_power_switch_ON();
#endif
  }
  musicBox_trigger_enabled=false;
}
#endif


//#define DEBUG_CLEANUP  TODO: maybe remove debug code, but can give interesting insights...
// TODO: rethink
void magical_cleanup(int p) {	// deselect unused primary pulses, check if playing has ended
  // PULSES.pulses[p].groups |= g_PRIMARY;	// TODO: rethink: maybe, maybe not

  if(!magic_autochanges)	// completely switched off by magic_autochanges==false
    return;			// noop

#if defined DEBUG_CLEANUP
  MENU.out(F("CLEANUP "));
#endif

  unsigned int deselected = PULSES.deselect_zombie_primaries();	// deselect deleted primary pulses
  bool do_display = MENU.maybe_display_more(VERBOSITY_SOME);
  /*
  if(do_display) {
    if(deselected) {
      MENU.out(F("removed unused "));
      MENU.outln(deselected);
    }
  }
  */
  int skipped=0;
  int flagged=0;
  for(int pulse=0; pulse<PL_MAX; pulse++) {
    if(PULSES.pulses[pulse].flags) {	// check if playing has ended  activity?
      if(PULSES.pulses[pulse].periodic_do == &musicBox_butler) {
	if(do_display) {
	  MENU.out('B');
	}
	skipped++;
      } else if(PULSES.pulses[pulse].periodic_do == &magical_butler) {
	if(do_display) {
	  MENU.out('u');
	}
	skipped++;
      } else if(PULSES.pulses[pulse].periodic_do == &magical_cleanup) {
	if(do_display) {
	  MENU.out('C');
	}
	skipped++;
      } else if(PULSES.pulses[pulse].periodic_do == &cycle_monitor) {
	if(do_display) {
	  MENU.out('M');
	}
	skipped++;
      } else {
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

  if(flagged==0) {
    if(MENU.verbosity)	// TODO: review
      MENU.outln(F("END reached"));
    delay(3200); // aesthetics
    HARD_END_playing(false);
  }
}

int stop_on_LOW(void) {	// stops *only* pulses that are low
  int were_high=0;
  if(MENU.verbosity >= VERBOSITY_LOWEST)	// TODO: rethink output
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

  if(MENU.verbosity >= VERBOSITY_SOME)	// TODO: rethink output
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

// TODO: musicBox_butler(p) LONELY BUTLER CHECK	################
void musicBox_butler(int pulse) {	// payload taking care of musicBox	ticking with slice_tick_period
  static uint8_t soft_end_cnt=0;
  static bool soft_cleanup_started=false;
  static int fast_cleanup_minimal_fraction_weighting=slice_weighting({1,4});
  static uint8_t stop_on_low_cnt=0;
  static short current_slice=0;
  static struct time butler_start_time;
  struct time this_start_time =  PULSES.pulses[pulse].next;	// still unchanged?

  struct time this_time = PULSES.pulses[pulse].next;		// TODO: verify ################
  PULSES.sub_time(&PULSES.pulses[pulse].last, &this_time);	// so long inside this cycle

  // fraction phase = {this_time.time, PULSES.pulses[pulse].period.time};	// not used
  // float float_phase = this_time.time / PULSES.pulses[pulse].period.time;	// not used
  fraction this_division = {PULSES.pulses[pulse].counter -1 , cycle_slices};
  HARMONICAL.reduce_fraction(&this_division);
  int this_weighting = slice_weighting(this_division);

  if(PULSES.pulses[pulse].counter==1) {	// the butler initializes himself:
    PULSES.pulses[pulse].groups |= g_MASTER;
    current_slice=0;			// start musicBox clock
    butler_start_time = PULSES.pulses[pulse].next;	// still unchanged?
    init_primary_counters();
    soft_end_cnt=0;
    soft_cleanup_started=false;
    stop_on_low_cnt=0;

  } else if(PULSES.pulses[pulse].counter==2) {	// now we might have more time for some initialization

    struct time trigger_enable_time = {MUSICBOX_TRIGGER_BLOCK_SECONDS*1000000, 0};
    if(MENU.verbosity >= VERBOSITY_SOME) {
      MENU.out(F("butler: prepare trigger  "));
      PULSES.display_time_human(trigger_enable_time);
      MENU.ln();
    }
    PULSES.add_time(&musicBox_start_time, &trigger_enable_time);

    musicBox_hard_end_time = {MUSICBOX_HARD_END_SECONDS*1000000, 0};
    if(MENU.verbosity >= VERBOSITY_SOME) {
      MENU.out(F("butler: prepare hard end "));
      PULSES.display_time_human(musicBox_hard_end_time);
      MENU.ln();
    }
    PULSES.add_time(&musicBox_start_time, &musicBox_hard_end_time);

  } else {	// all later wakeups

#if defined USE_BATTERY_CONTROL
    if((PULSES.pulses[pulse].counter % 5) == 0) {	// keep an eye on the battery
      if(!assure_battery_level()) {
	MENU.outln(F("POWER LOW"));
	HARD_END_playing(true);
      }
    }
#endif

#if defined MUSICBOX_HARD_END_SECONDS		// SAVETY NET
    {
      struct time scratch = musicBox_hard_end_time;
      PULSES.sub_time(&this_start_time, &scratch);	// is it time?
      if(scratch.overflow) {			//   negative, so it *is*
	MENU.out(F("butler: "));
	HARD_END_playing(true);
      }
    }
#endif

    if(magic_autochanges) {
      if(soft_end_cnt==0) {	// first time
	// soft end time could be reprogrammed by user interaction, always compute new:
	struct time soft_end_time=musicBox_start_time;
	PULSES.add_time(&used_subcycle, &soft_end_time);
	PULSES.add_time(100, &soft_end_time);		// tolerance
	struct time thisNow = this_start_time;
	PULSES.sub_time(&thisNow, &soft_end_time);	// is it time?
	if(soft_end_time.overflow) {			//   negative, so it *is*
	  if(soft_end_cnt++ == 0)
	    soft_end_playing(soft_end_days_to_live, soft_end_survive_level); // start soft end
	}
      } else {	// soft end was called already

	magical_cleanup(pulse/*dummy*/);

	if(MusicBoxState == ENDING) {
	  if(!soft_cleanup_started) {
	    struct time scratch = PULSES.get_now();
	    PULSES.sub_time(&soft_end_start_time, &scratch);
	    PULSES.sub_time(soft_end_cleanup_wait, &scratch);
	    if(!scratch.overflow) {
	      soft_cleanup_started=true;
	      //fast_cleanup_minimal_fraction_weighting = slice_weighting({1,4});	// start quite high, then descend
	      fast_cleanup_minimal_fraction_weighting = 12;	// start here, then descend
	      if (MENU.verbosity){
		MENU.out(F("butler: time to stop "));
		struct time time_to_stop = PULSES.get_now();
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
	    MENU.out(F("s clean "));
	    if(true) {	// FIXME: ################
	      // TODO: VERBOSITY
	      MENU.out(fast_cleanup_minimal_fraction_weighting);
	      MENU.out(F(" | "));
	      MENU.outln(this_weighting);
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
  if(show_cycle_pattern) {
    watch_primary_pulses();
    MENU.tab();
  }

  cycle_monitor(pulse);

  if(show_cycle_pattern || show_subcycle_position)
    MENU.ln();
}  // musicBox_butler()


void select_random_scale() {
#if defined RANDOM_ENTROPY_H
  random_entropy();	// entropy from hardware
#endif
  if(MENU.maybe_display_more(VERBOSITY_SOME))
    MENU.outln(F("random scale"));

  switch(random(27)) {		// random scale
  case 0: case 1: case 3: case 4:
    select_array_in(SCALES, pentatonic_minor);
    break;
  case 5: case 6: case 7: case 8:
    select_array_in(SCALES, european_PENTAtonic);
    break;
  case 9: case 10: case 11:
    select_array_in(SCALES, minor_scale);
    break;
  case 12: case 13: case 14:
    select_array_in(SCALES, major_scale);
    break;
  case 15: case 16: case 17:
    select_array_in(SCALES, doric_scale);	// +1 for doric_scale, see below
    break;
  case 18: case 19: case 20:
    select_array_in(SCALES, tetrachord);	// +1 for tetrachord, see below
    break;
  case 21: case 22:
    select_array_in(SCALES, octaves_fourths_fifths);
    break;
  case 23:
    select_array_in(SCALES, octaves_fifths);
    break;
  case 24:
    select_array_in(SCALES, octaves_fourths);
    break;
  case 25:
    select_array_in(SCALES, doric_scale);	// +1 for doric_scale
    break;
  case 26:
    select_array_in(SCALES, tetrachord);	// +1 for tetrachord
    break;
  }
  scale_was_set_by_menu = false;
}


void select_random_jiffle(void) {
#if defined RANDOM_ENTROPY_H
  random_entropy();	// entropy from hardware
#endif
  if(MENU.maybe_display_more(VERBOSITY_SOME))
    MENU.outln(F("random jiffle"));

  switch(random(109)) {
  case 0: case 1: case 2: case 3: case 4:
    select_array_in(JIFFLES, PENTAtonic_rise);
    break;
  case 5: case 6: case 7: case 8: case 9:
    select_array_in(JIFFLES, PENTAtonic_descend);
    break;

  case 10: case 11: case 12: case 13: case 14:
    select_array_in(JIFFLES, pentatonic_rise);
    break;
  case 15: case 16: case 17: case 18: case 19:
    select_array_in(JIFFLES, pentatonic_descend);
    break;

  case 20: case 21: case 22: case 23: case 24: case 29:	// was: too many, see below
    select_array_in(JIFFLES, tumtum);
    break;
  case 25: case 26: case 27:	// case 28: case 29:	// too many, see below
    select_array_in(JIFFLES, jiff_dec128);
    break;
  case 30: case 31: case 32: case 33: case 34:
    select_array_in(JIFFLES, ding_ditditdit);
    break;
  case 35: case 36: case 37: case 38: case 39:
    select_array_in(JIFFLES, diing_ditditdit);
    break;
  case 40: case 41: case 42: case 43: case 44:
    select_array_in(JIFFLES, din__dididi_dixi);
    break;
  case 45: case 46: case 47: case 48: case 49:
    select_array_in(JIFFLES, din_dididi);
    break;

  case 50: case 51: case 52: case 53:
    select_array_in(JIFFLES, PENTA_3rd_rise);
    break;
  case 54: case 55: case 56: case 57:
    select_array_in(JIFFLES, up_THRD);
    break;
  case 58: case 59: case 60: case 61:
    select_array_in(JIFFLES, up_THRD_dn);
    break;
  case 62: case 63: case 64: case 65:
    select_array_in(JIFFLES, dwn_THRD);
    break;
  case 66: case 67: case 68: case 69:
    select_array_in(JIFFLES, dwn_THRD_up);
    break;
  case 70: case 71: case 72: case 73:
    select_array_in(JIFFLES, PENTA_3rd_down_5);
    break;
  case 74: case 75: case 76: case 77:
    select_array_in(JIFFLES, penta_3rd_down_5);
    break;
  case 78: case 79: case 80: case 81:
    select_array_in(JIFFLES, rising_pent_theme);
    break;
  case 82: case 83: case 84: case 85:
    select_array_in(JIFFLES, penta_3rd_rise);
    break;
  case 86: case 87: case 88: case 89:
    select_array_in(JIFFLES, simple_theme);
    break;

  case 90:
    select_array_in(JIFFLES, jiff_dec_pizzicato);
    break;
  case 91: case 92:
    select_array_in(JIFFLES, pent_patternA);
    break;
  case 93: case 94:
    select_array_in(JIFFLES, pent_patternB);
    break;
  case 95: case 96:
    select_array_in(JIFFLES, pent_top_wave);
    break;
  case 97:
    select_array_in(JIFFLES, pent_top_wave_0);
    break;
  case 98:	// add some very simple jiffles:
    select_array_in(JIFFLES, d4096_3072);
    break;
  case 99:
    select_array_in(JIFFLES, d4096_2048);
    break;
  case 100: case 28:	// was: too many, see above
    select_array_in(JIFFLES, d4096_1024);
    break;
  case 101:
    select_array_in(JIFFLES, d4096_512);
    break;
  case 102:
    select_array_in(JIFFLES, d4096_256);
    break;
  case 103:
    select_array_in(JIFFLES, d4096_128);
    break;
  case 104:
    select_array_in(JIFFLES, d4096_64);
    break;
  case 105:
    select_array_in(JIFFLES, d4096_32);
    break;
  case 106:
    select_array_in(JIFFLES, d4096_16);
    break;
  case 107:
    select_array_in(JIFFLES, d4096_12);
    break;
  case 108:
    select_array_in(JIFFLES, tanboura);
    break;
  }

  jiffle_was_set_by_menu = false;
}

void random_fixed_pitches(void) {
  if(MENU.maybe_display_more(VERBOSITY_LOWEST))
    MENU.out(F("fixed random tuning "));

#if defined RANDOM_ENTROPY_H
  random_entropy();	// entropy from hardware
#endif

  switch (random(20)) {
  case 0:
  case 1:
  case 3:
    MENU.out('a');
    divisor = 220; // 220	// A 220  ***not*** harmonical	// TODO: define role of multiplier, divisor
    break;
  case 4:
  case 5:
  case 6:
    MENU.out('e');
    divisor=330; // 329.36	// E4  ***not*** harmonical
    break;
  case 7:
  case 8:
    MENU.out('d');
    divisor = 294;		// 293.66 = D4
    break;
  case 9:
  case 10:
  case 11:
    MENU.out('g');
    divisor=196; // 196.00	// G3  ***not*** harmonical
    break;
  case 12:
  case 13:
    MENU.out('c');
    divisor=262; // 261.63	// C4  ***not*** harmonical
    break;
  case 14:
  case 15:
    MENU.out('f');
    divisor=175; // 174.16	// F3  ***not*** harmonical
    break;
  case 16:
  case 17:	// 11.11. ;)
  case 18:	// 11.11. ;)
  case 19:	// 11.11. ;)
    MENU.out('b');
    divisor=233; // 233.08	// Bb3 ***not*** harmonical
    break;
    //    divisor=247; // 246.94	// B3  ***not*** harmonical  }
  }
  pitch_was_set_by_menu = false;
}

void random_octave_shift(void) {
  if(MENU.maybe_display_more(VERBOSITY_SOME))
    MENU.out(F("random octave shift: "));

  switch(random(7)) {
  case 0: case 1:			// leave as is
    if(MENU.maybe_display_more(VERBOSITY_SOME))
      MENU.ln();
    return;	// (do not change octave_was_set_by_menu)
    break;
  case 2: case 3: case 4:  case 5:	// down
    MENU.play_KB_macro(F("*2"));
    break;
  case 6:
    MENU.play_KB_macro(F("/2"));	// up
    break;
  }
  octave_was_set_by_menu = false;
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
    select_array_in(JIFFLES, kalimbaxyl);
    MENU.play_KB_macro("j");
    break;
  case 1:	// back_to_ground
    select_array_in(JIFFLES, back_to_ground);
    MENU.play_KB_macro("j");
    break;
  case 2:	// *3 jiffletab0	"Frosch"
    select_array_in(JIFFLES, jiffletab0);
    MENU.play_KB_macro(F("*3 j"));
    break;
  case 3:	// J3j jiff_december
    select_array_in(JIFFLES, jiff_december);
    MENU.play_KB_macro(F("*4/3 j"));
    break;
  case 4:	// jiff_dec128  the drummer in the cathedral
    select_array_in(JIFFLES, jiff_dec128);
    MENU.play_KB_macro(F("*3/2 j S0n"));
    break;
  case 5:	// jiffletab, silent rhythmes
    select_array_in(JIFFLES, jiffletab);
    MENU.play_KB_macro("j");
    break;
  case 6:	// jiff_dec_pizzicato	dirty!
    select_array_in(JIFFLES, jiff_dec_pizzicato);
    MENU.play_KB_macro(F("*3 j"));
    break;
  case 7:	// jiffletab0	fröhliches Knatterfurzkonzert explodiert
    select_array_in(JIFFLES,jiffletab0 );
    MENU.play_KB_macro(F("*6 j S0n"));
    break;
  case 8:	// aktivitätswellen  jiffletab01
    select_array_in(JIFFLES, jiffletab01);
    MENU.play_KB_macro(F("*3 j"));
    break;
  case 9:	// d4096_6 churzi plipps
    select_array_in(JIFFLES, d4096_6);
    MENU.play_KB_macro("j");
    break;
  }
  MENU.out(F("jiffle: "));
  MENU.outln(selected_name(JIFFLES));
}


short bass_pulses;	// see  setup_bass_middle_high()
short middle_pulses;	// see  setup_bass_middle_high()
short high_pulses;	// see  setup_bass_middle_high()


void start_musicBox() {
  MENU.out(F("\nstart_musicBox()\t"));
  MENU.out(F(STRINGIFY(MUSICBOX_VERSION)));

#if defined MUSICBOX_SUB_VERSION
  MENU.tab();
  MENU.outln(F(STRINGIFY(MUSICBOX_SUB_VERSION)));
#else
  MENU.ln();
#endif

  set_MusicBoxState(AWAKE);
  musicBox_trigger_enabled=false;
  blocked_trigger_shown = false;	// show only once a run
  musicBox_butler_i=ILLEGAL;

#if defined  USE_RTC_MODULE
  show_DS1307_time_stamp();
  MENU.ln();
#endif

#if defined USE_BATTERY_CONTROL
  show_battery_level();
  void HARD_END_playing(bool);	// defined below
  if(assure_battery_level())
    MENU.outln(F("power accepted"));
  else {
    MENU.outln(F(">>> NO POWER <<<"));
    HARD_END_playing(false);
  }
#endif

#if defined PERIPHERAL_POWER_SWITCH_PIN
  peripheral_power_switch_ON();

  MENU.out(F("peripheral POWER ON "));
  MENU.outln(PERIPHERAL_POWER_SWITCH_PIN);

  delay(250);	// give peripheral supply voltage time to stabilise
#endif

  MENU.outln(F("\n >>> * <<<"));	// start output block with configurations

#if defined  USE_RTC_MODULE		// repeat that in here, keeping first one for power failing cases
  show_DS1307_time_stamp();
  MENU.ln();
#endif

  MENU.men_selected = 0;	// starting point (might be changed by kb macro)
  MENU.play_KB_macro(F("-:M "), false); // initialize, the space avoids output from :M , no newline
  MENU.ln();

  // TODO: REWORK:  setup_bass_middle_high()  used in musicBox, but not really compatible
  setup_bass_middle_high(bass_pulses, middle_pulses, high_pulses);

  if(!scale_was_set_by_menu)	// if *not* set by user interaction
    select_random_scale();	//   random scale
  MENU.out(F("SCALE:\t"));
  MENU.outln(selected_name(SCALES));

  if(!jiffle_was_set_by_menu)	// if *not* set by user interaction
    select_random_jiffle();	//   random jiffle
  MENU.out(F("JIFFLE:\t"));
  MENU.outln(selected_name(JIFFLES));
  setup_jiffle_thrower_selected(selected_actions);

  PULSES.add_selected_to_group(g_PRIMARY);
  set_primary_block_bounds();	// remember where the primary block starts and stops

  if(!sync_was_set_by_menu) {	// if *not* set by user interaction
    sync = random(6);		// random sync	// MAYBE: define  select_random_sync()  ???
    if(MENU.maybe_display_more(VERBOSITY_SOME))
      MENU.outln(F("random sync"));
  }

  MENU.out(F("SYNC:\t"));
  MENU.outln(sync);

  // time_unit
  PULSES.time_unit=1000000;	// default metric
  MENU.out("TIME_U:\t");
  MENU.out(PULSES.time_unit);
  MENU.tab();

  // pitch
  multiplier=4096;	// uses 1/4096 jiffles		// TODO: define role of multiplier, divisor
  multiplier *= 8;	// TODO: adjust appropriate...

  // random pitch
#if ! defined SOME_FIXED_TUNINGS_ONLY	// random tuning // TODO: make that a run time option
  #if defined RANDOM_ENTROPY_H
    random_entropy();	// entropy from hardware
  #endif
    if(!pitch_was_set_by_menu) {	// if *not* set by user interaction
      divisor = random(160, 450);	// *not* tuned for other instruments
      if(MENU.maybe_display_more(VERBOSITY_SOME))
	MENU.outln(F("random pitch"));
    }
#else				// some randomly selected metric A=440 tunings for jam sessions like in HACK_11_11_11_11
  if(!pitch_was_set_by_menu)	// if *not* set by user interaction
    random_fixed_pitches(void);	// random fixed pitches
  MENU.tab();
  MENU.outln(divisor);	// TODO: define role of multiplier, divisor
#endif

  MENU.out('*');
  MENU.out(multiplier);	// TODO: define role of multiplier, divisor
  MENU.slash();
  MENU.outln(divisor);	// TODO: define role of multiplier, divisor

  tune_2_scale(voices, multiplier, divisor, sync, selected_in(SCALES));	// TODO: define role of multiplier, divisor
  lower_audio_if_too_high(409600*2);	// 2 bass octaves  // TODO: adjust appropriate...

  random_octave_shift();  // random octave shift

#if defined PERIPHERAL_POWER_SWITCH_PIN
    peripheral_power_switch_ON();
#endif

  struct time period_lowest = PULSES.pulses[lowest_primary].period;
  harmonical_CYCLE = scale2harmonical_cycle(selected_in(SCALES), &period_lowest);
  used_subcycle = harmonical_CYCLE;

#if defined AUTOMAGIC_CYCLE_TIMING_SECONDS
  used_subcycle={AUTOMAGIC_CYCLE_TIMING_SECONDS*1000000L,0};
  {
    struct time this_subcycle=harmonical_CYCLE;
    while(true) {
      if(this_subcycle.time <= used_subcycle.time && this_subcycle.overflow==used_subcycle.overflow) {
	used_subcycle = this_subcycle;
	break;
      }
      PULSES.div_time(&this_subcycle, 2);
    }
  }
#endif

  show_cycles_1line();
  MENU.outln(F(" <<< * >>>"));	// end output block

  if(MENU.verbosity >= VERBOSITY_SOME || true) {
    show_cycle(harmonical_CYCLE);	// shows multiple cycle octaves
    MENU.ln();
  }

  set_cycle_slice_number(cycle_slices);

  musicBox_start_time = PULSES.get_now();	// keep musicBox_start_time

  PULSES.activate_selected_synced_now(sync);	// 'n' sync and activate

  if(sync) {
    if(MENU.verbosity >= VERBOSITY_LOWEST) {
      PULSES.fix_global_next();			// cannot understand why i need that here...
      struct time pause = PULSES.global_next;
      PULSES.sub_time(&musicBox_start_time, &pause);
      MENU.out(F("sync pause "));
      PULSES.display_time_human(pause);
      MENU.ln(); MENU.ln();
    }
  }

  // the butler starts just a pad *after* musicBox_start_time
  // remember pulse index of the butler, so we can call him, if we need him ;)
  musicBox_butler_i = \
    PULSES.setup_pulse(&musicBox_butler, ACTIVE, PULSES.get_now(), slice_tick_period);

  // TODO: RETHINK: group of butler, it is closer to a master then a primary...
  PULSES.pulses[musicBox_butler_i].groups |= g_MASTER;	// savety net, until butler has initialised itself

  stress_event_cnt = -3;	// some stress events will often happen after starting the musicBox
}


void magical_stress_release() {		// special stress release for magical music box
  if (voices) {	// normal case, I have never seen exceptions
    PULSES.init_pulse(--voices);		// *remove* topmost voice
    PULSES.select_n(voices);
    MENU.out(F("magical_stress_release() V"));
    MENU.outln(voices);

    stress_count = 0;		// configure pulses stress managment
    stress_event_cnt = -3;	// some *heavy* stress event expected after magical_stress_release()...
  } else {
    MENU.play_KB_macro("X");	// *could* happen some time, maybe *SAVETY NET*
    stress_count = 0;		// configure pulses stress managment
    stress_event_cnt = 0;
  }
}


// magical fart on reading a floating GPIO pin ;)
portMUX_TYPE magical_fart_MUX = portMUX_INITIALIZER_UNLOCKED;
unsigned int magical_fart_cnt=0;
gpio_pin_t magical_fart_output_pin=ILLEGAL;

void magical_fart_ISR() {
  portENTER_CRITICAL_ISR(&magical_fart_MUX);
  magical_fart_cnt++;

  if(magical_fart_output_pin != ILLEGAL)
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


// #include <bt/bluedroid/api/include/api/esp_bt_main.h>
#include <esp_bt_main.h>
#include <esp_bt.h>
#include "esp_wifi.h"
#include "WiFi.h"
#include "driver/rtc_io.h"
#include "driver/dac.h"

void light_sleep() {	// see: bool go_light_sleep	flag to go sleeping from main loop
  MENU.out(F("light_sleep()\t"));

#if defined USE_BLUETOOTH_SERIAL_MENU
  bt_status_before_sleeping = show_bt_status();

  // BLUEtoothSerial.end();	// reboots instead of sleeping

  // without esp_bluedroid_disable() crashes instead of sleeping, or crashes soon after waking up
  MENU.ok_or_error_ln(F("esp_bluedroid_disable()"), esp_bluedroid_disable());

  /*
  // esp_bt_controller_disable() does no good nor harm
  MENU.ok_or_error_ln(F("esp_bt_controller_disable()"), esp_bt_controller_disable());
  */

  // BLUEtoothSerial.end();	// accepted here, but still no BT after wakeup

  show_bt_status();
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

  if (esp_sleep_enable_ext0_wakeup((gpio_num_t) MUSICBOX_TRIGGER_PIN, 1))
    MENU.error_ln(F("esp_sleep_enable_ext0_wakeup()"));
/*
  if(gpio_wakeup_enable((gpio_num_t) MUSICBOX_TRIGGER_PIN, GPIO_INTR_LOW_LEVEL))
    MENU.error_ln(F("gpio_wakeup_enable()"));

  if (esp_sleep_enable_gpio_wakeup())
    MENU.error_ln(F("esp_sleep_enable_gpio_wakeup"));
*/

//  if(esp_sleep_enable_uart_wakeup(0))
//    MENU.error_ln(F("esp_sleep_enable_uart_wakeup(0)"));
//
//  if(esp_sleep_enable_uart_wakeup(1))
//    MENU.error_ln(F("esp_sleep_enable_uart_wakeup(1)"));

  MENU.outln(F("sleep well..."));
  delay(1500);

  MENU.ok_or_error_ln(F("esp_light_sleep_start()"), esp_light_sleep_start());

  MENU.out(F("\nAWOKE\t"));

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

  bluetooth_setup();		// does no good, does no harm

  /*
  if(esp_bluedroid_enable())	// wakes up *if* after bluetooth_setup(), but still no BT
    MENU.error_ln(F("esp_bluedroid_enable()"));
  */
#endif

  /*
    in some rare cases the trigger woke up the system, but playing was not started
    so i try calling  musicBox_trigger_got_hot()  from here
    just pretending trigger was high ;)
    TODO: test...

    //  musicBox_trigger_got_hot();	// must be called when musicBox trigger was detected high  TODO: TEST: ################
    deactivated, was probably unrelated
    TODO: test, remove
  */

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

  // TODO: conditionally...
  esp_bluedroid_disable();
  esp_bt_controller_disable();
  esp_wifi_stop();

  if (esp_sleep_enable_ext0_wakeup((gpio_num_t) MUSICBOX_TRIGGER_PIN, 1))
    MENU.error_ln(F("esp_sleep_enable_ext0_wakeup()"));

  /* ONLY HELPS in light_sleep()
  // kill the ugly noise on DACs during sleep modes:
  pinMode(25, OUTPUT);		// avoid ugly noise on DAC during sleep
  digitalWrite(25, LOW);

  pinMode(26, OUTPUT);		// avoid ugly noise on DAC during sleep
  digitalWrite(26, LOW);
  */				// DOES NOT HELP HERE :(

  MENU.outln(F("esp_deep_sleep_start()"));
  delay(1500);

  esp_deep_sleep_start();	// sleep well ... ... ...

  // TODO: it will never get here, FIXME: ################
  MENU.out(F("AWOKE\t"));
  // ESP_SLEEP_WAKEUP_GPIO	7
  MENU.outln(esp_sleep_get_wakeup_cause());

  rtc_gpio_deinit((gpio_num_t) MUSICBOX_TRIGGER_PIN);  // TODO: it will never get here, FIXME: ################
}


/*
  void magical_butler(int p)	 OBSOLETE

  enables trigger when due
  then if(magic_autochanges) {
    start soft_end_playing() and start magical_cleanup()
    start HARD_END_playing() if cleanup did not detect END already
*/
//#define DEBUG_BUTLER	TODO: remove debug code
void magical_butler(int p) {	// TODO: OBSOLETE?
#if defined DEBUG_BUTLER
  MENU.out(F("BUTLER: "));
#endif
  switch(PULSES.pulses[p].counter) {
  case 1:	// prepare enable trigger
    PULSES.pulses[p].period.time = MUSICBOX_TRIGGER_BLOCK_SECONDS*1000000L;
    break;
  case 2:	// enable trigger and prepare soft end
    musicBox_trigger_enabled = true;
    MENU.outln(F("trigger enabled"));
#if defined AUTOMAGIC_CYCLE_TIMING_SECONDS	// MAX seconds
    {
      struct time til_soft_end_time=used_subcycle;
      PULSES.sub_time(MUSICBOX_TRIGGER_BLOCK_SECONDS*1000000L, &til_soft_end_time);
      PULSES.add_time(100, &til_soft_end_time);	// tolerance
      PULSES.pulses[p].period = til_soft_end_time;
    }
#else
    PULSES.pulses[p].period.time = (MUSICBOX_PERFORMACE_SECONDS - MUSICBOX_TRIGGER_BLOCK_SECONDS)*1000000L;
#endif
    break;
  case 3:	// start soft ending and cleanup pulse, prepare for butler hard end
    if(magic_autochanges)
      soft_end_playing(soft_end_days_to_live, soft_end_survive_level);
    // prepare hard end
#if defined AUTOMAGIC_CYCLE_TIMING_SECONDS
    {
      struct time til_hard_end_time = used_subcycle;
      PULSES.div_time(&til_hard_end_time, 2);	// cycle/2 for soft end, then HARD end	// TODO: test&adjust
      PULSES.pulses[p].period = til_hard_end_time;
    }
#else
    PULSES.pulses[p].period.time =
      (MUSICBOX_HARD_END_SECONDS - MUSICBOX_PERFORMACE_SECONDS - MUSICBOX_TRIGGER_BLOCK_SECONDS)*1000000L;
#endif
    // start magical_cleanup() pulse taking care of selections, check for end, stop *if* end reached
    struct time duration;
    duration.overflow=0;
    duration.time=2*1000000;	// magical_cleanup all 2 seconds
    PULSES.setup_pulse(&magical_cleanup, ACTIVE, PULSES.get_now(), duration);	// start magical_cleanup() pulse
    break;
  case 4:	// hard end playing
    if(magic_autochanges) {
      // the end was not reached by magical_cleanup(), so we abort playing now
      MENU.out(F("butler: "));
      HARD_END_playing(true);
    }
    break;
  default:	// we should never get here...	savety net
    PULSES.init_pulse(p);	// with  magic_autochanges==false it's normal
  }
} // magical_butler()	OBSOLETE


/* **************************************************************** */
void musicBox_setup() {	// TODO:update
  MENU.ln();

#if defined MUSICBOX_TRIGGER_PIN
  MENU.out(F("musicBox trigger pin:\t"));
  MENU.outln(MUSICBOX_TRIGGER_PIN);
#endif

#if defined MUSICBOX_TRIGGER_BLOCK_SECONDS
  MENU.out(F("disable retriggering:\t"));
  MENU.outln(MUSICBOX_TRIGGER_BLOCK_SECONDS);
  MENU.ln();
#endif

#if defined AUTOMAGIC_CYCLE_TIMING_SECONDS
  MENU.out(F("cycle used max seconds:\t"));
    MENU.outln(AUTOMAGIC_CYCLE_TIMING_SECONDS);
#endif

#if defined MUSICBOX_PERFORMACE_SECONDS
  MENU.out(F("performance seconds:\t"));
  MENU.outln(MUSICBOX_PERFORMACE_SECONDS);
#endif

#if defined MUSICBOX_HARD_END_SECONDS
  MENU.out(F("hard stop:\t\t"));
  MENU.outln(MUSICBOX_HARD_END_SECONDS);
#endif

  if (esp_sleep_enable_ext0_wakeup((gpio_num_t) MUSICBOX_TRIGGER_PIN, 1))
    MENU.error_ln(F("esp_sleep_enable_ext0_wakeup()"));

  bass_pulses=14;	// see  setup_bass_middle_high()
  middle_pulses=15;	// see  setup_bass_middle_high()
  high_pulses=7;	// see  setup_bass_middle_high()
}


/* **************************************************************** */
// musicBox menu

void musicBox_display() {
  MENU.outln(F("The Harmonical Music Box  http://github.com/reppr/pulses/\n"));

  MENU.out(F("MusicBoxState "));
  MENU.out(MusicBoxState_name);
  MENU.tab();
  MENU.out(F("butler "));
  if(musicBox_butler_i == ILLEGAL)
    MENU.out('-');
  else {
    MENU.out(musicBox_butler_i);
    MENU.tab();

    struct fraction current_phase = {PULSES.pulses[musicBox_butler_i].counter, cycle_slices};
    MENU.out('[');
    display_fraction_int(current_phase);

    HARMONICAL.reduce_fraction(&current_phase);
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
  PULSES.display_time_human_format(harmonical_CYCLE);
  MENU.ln();

  MENU.out(F("subcycle\t\t"));
  PULSES.display_time_human_format(used_subcycle);
  MENU.ln();

  MENU.out(cycle_slices);
  MENU.out(F(" slices='n'  "));
  set_cycle_slice_number(cycle_slices);	// make sure slice_tick_period is ok
  PULSES.display_time_human(slice_tick_period);
  MENU.ln();

  MENU.ln();

  MENU.out(F("'o' show position ticker "));
  MENU.out_ON_off(show_subcycle_position);

  MENU.out(F("\t'p' show cycle pattern "));
  MENU.out_ON_off(show_cycle_pattern);

  MENU.out(F("\t'a' autochanges "));
  MENU.out_ON_off(magic_autochanges);

  MENU.tab();
  MENU.out(F("'c' cycle "));

  struct time period_lowest = PULSES.pulses[lowest_primary].period;
  harmonical_CYCLE = scale2harmonical_cycle(selected_in(SCALES), &period_lowest); // TODO: rethink ################
  PULSES.display_time_human(harmonical_CYCLE);
  MENU.ln();

  MENU.out(F("soft_end("));
  MENU.out(soft_end_days_to_live);	// remaining days of life after soft end
  MENU.out(F(", "));
  MENU.out(soft_end_survive_level);	// the level a pulse must have reached to survive soft en
  MENU.outln(F(")\t'd'=days to survive  'l'=level minimal age 'E'= start soft end now"));
  MENU.outln(F("'w' minimal weigh "));
  MENU.out(soft_cleanup_minimal_fraction_weighting);
  MENU.outln(F("\thard end='H'"));

  MENU.out(F("\"L\"=stop when low\t\"LL\"=stop only low\t'P'="));

  if(MusicBoxState == OFF)
    MENU.outln(F("START"));
  else {
    MENU.outln(F("STOP"));
  }

  show_UI_basic_setup();

/*	*deactivated*
  MENU.outln(F("fart='f'"));
*/
  MENU.ln();
}


bool musicBox_reaction(char token) {
  int input_value;

  switch(token) {
  case '?': // musicBox_display();
    musicBox_display();
    break;
  case ',':
    if (MENU.menu_mode == 0) {	// exclude special cases
      show_UI_basic_setup();
      show_cycles_1line();
    } else
      return false;	// for other menu modes let pulses menu do the work ;)	// TODO: TEST:
    break;
  case 'a': // magic_autochanges
    magic_autochanges = !magic_autochanges;
    break;
  case 'c': // show cycle
    show_cycle(harmonical_CYCLE);
    break;
  case 'E': // soft_end_playing(soft_end_days_to_live, soft_end_survive_level);
    soft_end_playing(soft_end_days_to_live, soft_end_survive_level);
    break;
  case 'd': // soft_end_days_to_live
    input_value = MENU.numeric_input(soft_end_days_to_live);
    if(input_value >= 0)
      soft_end_days_to_live = input_value;
    break;
  case 'l': // soft_end_survive_level
    input_value = MENU.numeric_input(soft_end_survive_level);
    if(input_value >= 0)
      soft_end_survive_level = input_value;
    break;
  case 'w': // soft_cleanup_minimal_fraction_weighting
    soft_cleanup_minimal_fraction_weighting = MENU.numeric_input(soft_cleanup_minimal_fraction_weighting);
    break;
  case 'H': // HARD_END_playing(true);
    HARD_END_playing(true);
    break;
  case 'n': // TODO: review, fix cycle_slices
    if((input_value = MENU.numeric_input(cycle_slices) > 0)) {
      set_cycle_slice_number(input_value);
      PULSES.pulses[musicBox_butler_i].period = slice_tick_period;	// a bit adventurous ;)
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

  case 'L': // 'L' == stop_on_LOW_H1()  ||  "LL" == stop_on_LOW()/*only*/)
    if(MENU.cb_peek() != 'L') {	// 'L' case	stop_on_LOW_H1()
      MENU.out(stop_on_LOW_H1());
      MENU.outln(F(" were high "));
    } else {			// "LL" case	stop_on_LOW()/*only*/
      MENU.drop_input_token();
      MENU.out(stop_on_LOW()/*only*/);
      MENU.outln(F(" were high "));
    }
    break;

  case 'P': // 'P' Play start/stop
    if(MusicBoxState != OFF) {
      reset_all_flagged_pulses_GPIO_OFF();
      set_MusicBoxState(OFF);
      if(MENU.maybe_display_more(VERBOSITY_LOWEST))
	musicBox_display();
    } else
      start_musicBox();
    break;

  case 'p': // 'p' switch cycle pattern display
    show_cycle_pattern ^= 1;

    if(MENU.verbosity > VERBOSITY_LOWEST) {
      if(show_cycle_pattern)
	MENU.out(F("SHOW"));
      else
	MENU.out(F("do *not show*"));
      MENU.outln(F(" cycle pattern"));
    }
    break;

/*
  void furzificate() {	// switch to a quiet, farting patterns, u.a.
  very simple one shot implementation
  does not work as is with musicBox any more
  adapt or remove?

  case'f':
    furzificate();
    break;
*/

  default:
    return false;
  }

  return true;
}
