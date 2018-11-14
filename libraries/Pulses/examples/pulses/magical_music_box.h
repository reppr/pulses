/*
  magical_musicbox.h
*/

#define AUTOMAGIC_CYCLE_TIMING_MINUTES	65	// *max minutes*, sets performance timing based on cycle
// #define SOME_FIXED_TUNINGS_ONLY		// fixed pitchs only like E A D G C F B  see: HACK_11_11_11_11

//#define DEBUGGING_MAGICAL_MUSICBOX
#if defined DEBUGGING_MAGICAL_MUSICBOX
 #define MAGICAL_PERFORMACE_SECONDS	30
 #define MAGICAL_TRIGGER_BLOCK_SECONDS	6
 #define MUSICBOX_HARD_END_SECONDS	2*60
#endif

#if defined BRACHE_NOV_2018_SETTINGS
  #define MAGICAL_PERFORMACE_SECONDS	6*60	// BRACHE
  #define MAGICAL_TRIGGER_BLOCK_SECONDS	20	// BRACHE
  #define MUSICBOX_HARD_END_SECONDS	8*60	// BRACHE
#endif

#include <esp_sleep.h>
// #include "rom/gpio.h"
// #include "driver/gpio.h"

#include "random_entropy.h"

#if defined PERIPHERAL_POWER_SWITCH_PIN
  #include "peripheral_power_switch.h"
#endif

#ifndef MAGICAL_TRIGGER_PIN
  #define MAGICAL_TRIGGER_PIN	34
#endif

#ifndef MAGICAL_PERFORMACE_SECONDS
  #define MAGICAL_PERFORMACE_SECONDS	12*60
#endif

#if ! defined MAGICAL_TRIGGER_BLOCK_SECONDS
  #define MAGICAL_TRIGGER_BLOCK_SECONDS	3
#endif

bool blocked_trigger_shown=false;	// show only once a run

#if ! defined MUSICBOX_HARD_END_SECONDS
  #define  MUSICBOX_HARD_END_SECONDS	15*60
#endif

#if ! defined MAGICAL_MUSICBOX_ENDING	// *one* of the following:
//  #define  MAGICAL_MUSICBOX_ENDING	light_sleep();	// works fine

#if defined HACK_11_11_11_11		// never ending jam session
  #define  MAGICAL_MUSICBOX_ENDING	;	// just deactivated ;) 11.11.
#else
  #define  MAGICAL_MUSICBOX_ENDING	light_sleep();	// works fine
#endif

//#define  MAGICAL_MUSICBOX_ENDING	deep_sleep();	// still DAC noise!!!
  //#define  MAGICAL_MUSICBOX_ENDING	ESP.restart();	// works fine
#endif

// #define PERIPHERAL_POWER_SWITCH_PIN		// TODO: file?


// MagicalMusicState
enum magicalmusicbox_state_t {OFF=0, ENDING, SLEEPING, SNORING, AWAKE, FADE};
magicalmusicbox_state_t MagicalMusicState=OFF;
void set_MagicalMusicState(magicalmusicbox_state_t state) {	// sets the state unconditionally
  switch (state) {			// initializes state if necessary
  case OFF:
    break;
  case ENDING:
    break;
  case SLEEPING:
    break;
  case SNORING:
    break;
  case AWAKE:
    break;
  case FADE:
    break;
  default:
    MENU.outln(F("unknown MagicalMusicState"));	// should not happen
    return;					// error, return
  }

  MagicalMusicState = state;		// OK
}

// TODO: check&fix
#if defined HACK_11_11_11_11	// magic_autochanges=false;
  bool magic_autochanges=false;	// never ending jam sessions...
#else
bool magic_autochanges=true;	// switch if to end normal playing after MAGICAL_PERFORMACE_SECONDS
#endif

portMUX_TYPE magical_MUX = portMUX_INITIALIZER_UNLOCKED;

// very simple one shot implementation:
//void furzificate(int dummy=0) {	// switch to a quiet, farting patterns, u.a.
//  /* with the dummy int parameter it passes as payload for a pulse */
void furzificate() {	// switch to a quiet, farting patterns, u.a.
  MENU.outln(F("furzificate()"));
  set_MagicalMusicState(SNORING);

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

void magic_trigger_ON();	// forward declaration
bool magical_trigger_enabled=false;

struct time musicbox_start_time;

struct time cycle;	// TODO: move to Harmonical?

#if defined AUTOMAGIC_CYCLE_TIMING_MINUTES
struct time used_subcycle;
#endif

void show_cycle(struct time cycle) {
  MENU.out(F("\nharmonical cycle "));
  if(selected_in(SCALES)==NULL) {
    MENU.outln(F("no scale"));
    return;
  }

  PULSES.display_time_human(cycle);
  MENU.ln();

  // TODO: do *not* expect it on pulse pulses[voices-1]
  struct time period = PULSES.pulses[voices-1].period;
  struct time shortest = scale2harmonical_cycle(selected_in(SCALES), &period);

  //  PULSES.display_time_human(PULSES.pulses[PULSES.fastest_from_selected()].period);
  MENU.out(F("fastest * pulse"));
  //  PULSES.display_realtime_sec(PULSES.pulses[PULSES.fastest_from_selected()].period);
  PULSES.display_time_human(shortest);
  MENU.ln();

  int i=0;
  //                                   !!!  a tolerance of 128 seemed *not* to be enough
  while(cycle.time >= (shortest.time - 256/*tolerance*/) || cycle.overflow) {	// display cycle and relevant octaves
    MENU.out(F("2^"));
    MENU.out(i--);
    MENU.tab();
    PULSES.display_time_human(cycle);
    if(cycle.time == used_subcycle.time)
      MENU.out(F("\t| subcycle |"));
    MENU.ln();

    PULSES.div_time(&cycle, 2);
  }
  MENU.ln();
}

// cycle_monitor(p)  payload to give infos where in the cycle we are
unsigned short cycle_show_divisions = 180;	// test&adapt	sometimes less is more
/*
   unsigned short cycle_show_divisions = 72;	// test&adapt   classical aesthetics?
   unsigned short cycle_show_divisions = 120;	// test&adapt   classical aesthetics?
   unsigned short cycle_show_divisions = 90*3;	// test&adapt   simplified keeping important details?
   unsigned short cycle_show_divisions = 180*3;	// test&adapt	interesting lot of detail
   unsigned short cycle_show_divisions = 180;	// test&adapt	sometimes less is more
   unsigned short cycle_show_divisions = 360;	// test&adapt   classical aesthetics?
*/
uint8_t cycle_monitor_last_seen_division=0;	// reset that on a start
void cycle_monitor(int pulse) {	// show markers at important cycle divisions
  MENU.out(F("* "));

  struct time this_time = PULSES.get_now();
  PULSES.sub_time(&PULSES.pulses[pulse].last, &this_time);	// so long inside this cycle
  /* TESTED: works fine disregarding overflow :)
  if(this_time.overflow != PULSES.pulses[pulse].last.overflow)
    MENU.outln(F("over"));
  */
  fraction phase = {this_time.time, PULSES.pulses[pulse].period.time};
  // float float_phase = this_time.time / PULSES.pulses[pulse].period.time;	// not used
  fraction this_division = {cycle_monitor_last_seen_division, cycle_show_divisions};
  HARMONICAL.reduce_fraction(&this_division);
  MENU.out(this_division.multiplier);
  MENU.out('/');
  MENU.outln(this_division.divisor);

  cycle_monitor_last_seen_division++;
  cycle_monitor_last_seen_division %= cycle_show_divisions;
}

void magical_butler(int p);		// pre declare payload
void start_musicbox() {
  MENU.outln(F("start_musicbox()"));
  set_MagicalMusicState(AWAKE);
  magical_trigger_enabled=false;
  blocked_trigger_shown = false;	// show only once a run

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

  MENU.outln(F("\n >>> * <<<"));
  MENU.men_selected = 0;
  MENU.play_KB_macro(F("-E40 "), false); // initialize, the space avoids output from E40, no newline

  switch(random(23)) {		// random scale
  case 0: case 1: case 3: case 4:
    select_array_in(SCALES, pentatonic_minor);
    break;
  case 5: case 6: case 7: case 8:
    select_array_in(SCALES, european_pentatonic);
    break;
  case 9: case 10: case 11:
    select_array_in(SCALES, minor_scale);
    break;
  case 12: case 13: case 14:
    select_array_in(SCALES, major_scale);
    break;
  case 15: case 16: case 17:
    select_array_in(SCALES, doric_scale);
    break;
  case 18: case 19:
    select_array_in(SCALES, octaves_fourths_fifths);
    break;
  case 20: case 21:
    select_array_in(SCALES, tetrachord);
    break;
  case 22:
    select_array_in(SCALES, octaves_fifths);
    break;
  case 23:
    select_array_in(SCALES, octaves_fourths);
    break;
  }
  MENU.out(F("SCALE: "));
  MENU.outln(selected_name(SCALES));

  // random jiffle
  switch(random(99)) {
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

  case 20: case 21: case 22: case 23: case 24:
    select_array_in(JIFFLES, tumtum);
    break;
  case 25: case 26: case 27: case 28: case 29:
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

  case 90: case 91:
    select_array_in(JIFFLES, jiff_dec_pizzicato);
    break;
  case 92: case 93:
    select_array_in(JIFFLES, pent_patternA);
    break;
  case 94: case 95:
    select_array_in(JIFFLES, pent_patternB);
    break;
  case 96: case 97:
    select_array_in(JIFFLES, pent_top_wave);
    break;
  case 98:
    select_array_in(JIFFLES, pent_top_wave_0);
    break;
  }
  MENU.out(F("JIFFLE: "));
  MENU.outln(selected_name(JIFFLES));
  setup_jiffle_thrower_selected(selected_actions);

  sync = random(6);		// random sync
  MENU.out(F("sync "));
  MENU.outln(sync);

  // pitch
  PULSES.time_unit=1000000;	// default metric
  multiplier=4096;		// uses 1/4096 jiffles
  multiplier *= 8;	// TODO: adjust appropriate...

#if ! defined SOME_FIXED_TUNINGS_ONLY	// random tuning
  divisor = random(160, 450);	// *not* tuned for other instruments
#else				// some randomly selected metric A=440 tunings for jam sessions like in HACK_11_11_11_11

  MENU.out(F("fixed tuning "));
  switch (random(20)) {
  case 0:
  case 1:
  case 3:
    MENU.out('a');
    divisor = 220; // 220	// A 220  ***not*** harmonical
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
  MENU.tab();
  MENU.outln(divisor);
#endif

  MENU.out("time_unit: ");
  MENU.out(PULSES.time_unit);
  MENU.tab();
  MENU.out('*');
  MENU.out(multiplier);
  MENU.slash();
  MENU.outln(divisor);

  tune_2_scale(voices, multiplier, divisor, sync, selected_in(SCALES));
  lower_audio_if_too_high(409600*2);	// 2 bass octaves  // TODO: adjust appropriate...

  // random pitch shift down
  switch(random(9)) {
  case 0: case 1:
    break;
  case 2: case 3: case 4:  case 5:
    MENU.play_KB_macro(F("*2"));
    break;
  case 6:
    MENU.play_KB_macro(F("*4"));
    break;
  case 8:
    MENU.play_KB_macro(F("/2"));
    break;
  }

  MENU.outln(F(" <<< * >>>"));
#if defined PERIPHERAL_POWER_SWITCH_PIN		// FIXME: try again... ################################################################
  peripheral_power_switch_ON();
#endif
  // TODO: do *not* expect it on pulse pulses[0]
  struct time period = PULSES.pulses[0].period;
  cycle = scale2harmonical_cycle(selected_in(SCALES), &period);
  used_subcycle = cycle;
  //  #define AUTOMAGIC_CYCLE_TIMING_MINUTES	7	// *max minutes*, sets performance timing based on cycle
#if defined AUTOMAGIC_CYCLE_TIMING_MINUTES
  used_subcycle={AUTOMAGIC_CYCLE_TIMING_MINUTES*60*1000000L,0};
  {
    struct time scratch=cycle;
    while(true) {
      if(scratch.time <= used_subcycle.time && scratch.overflow==used_subcycle.overflow) {
	used_subcycle = scratch;
	break;
      }
      PULSES.div_time(&scratch, 2);
    }
  }
#endif
  MENU.out(F("used subcycle:"));
  PULSES.display_time_human(used_subcycle);
  MENU.ln();
  show_cycle(cycle);

  musicbox_start_time = PULSES.get_now();	// keep musicbox_start_time
  PULSES.activate_selected_synced_now(sync);	// 'n' sync and activate

  struct time duration;	// dummy, *the butler knows* when to do what...
  duration.overflow=0;
  duration.time=0;

  PULSES.setup_pulse(&magical_butler, ACTIVE, PULSES.get_now(), duration);

  duration = used_subcycle;
  PULSES.div_time(&duration, cycle_show_divisions);

  cycle_monitor_last_seen_division=0;
  PULSES.setup_pulse(&cycle_monitor, ACTIVE, PULSES.get_now(), duration);
  stress_event_cnt = -3;	// some stress events will often happen after starting the musicbox
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


unsigned int magical_trigger_cnt=0;
void magic_trigger_OFF();

void magical_trigger_ISR() {	// can also be used on the non interrupt version :)
  portENTER_CRITICAL_ISR(&magical_MUX);
  static struct time triggered_at=PULSES.get_now();	// preserves last seen fresh trigger time

  struct time new_trigger = PULSES.get_now();		// new trigger time
  struct time duration = new_trigger;			// remember

  bool triggered=false;
  switch (MagicalMusicState) {
  case OFF:
  case ENDING:
  case SNORING:
    triggered = true;			// always accept trigger in these states
    break;
  case AWAKE:
    if(magical_trigger_enabled)		// enabled?  was it awake long enough for triggering again?
      triggered=true;			//   accept
    else if(!blocked_trigger_shown) {
      MENU.outln(F("trigger blocked"));	// FIXME: ################
      blocked_trigger_shown = true;
    }
    break;
  default:
    MENU.outln(F("magical_trigger_ISR unknown state"));	// should not happen
    triggered = true;	// not save... but
  }

  if(triggered) {
    magical_trigger_enabled = false;
    blocked_trigger_shown = false;
    triggered_at = new_trigger;
    magical_trigger_cnt++;
  }

  portEXIT_CRITICAL_ISR(&magical_MUX);
}

void magic_trigger_ON() {
#if ! defined MAGICAL_TOILET_HACKS	// some quick dirty hacks *NOT* using the interrupt
  MENU.out(F("MAGICAL_TRIGGER ON\t"));
  MENU.out(MAGICAL_TRIGGER_PIN);
  MENU.tab();
  MENU.outln(magical_trigger_cnt);
  pinMode(MAGICAL_TRIGGER_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(MAGICAL_TRIGGER_PIN), magical_trigger_ISR, RISING);
#else
  ;
#endif
}

// TODO: ################  DOES NOT WORK: isr is *not* called any more BUT THE SYSTEM BECOMES STRESSED!
void magic_trigger_OFF() {
#if ! defined MAGICAL_TOILET_HACKS	// some quick dirty hacks *NOT* using the interrupt
  MENU.outln(F("magic_trigger_OFF\t"));
  detachInterrupt(digitalPinToInterrupt(MAGICAL_TRIGGER_PIN));
  //  esp_intr_free(digitalPinToInterrupt(MAGICAL_TRIGGER_PIN));
#endif
  magical_trigger_enabled=false;
}


#if defined MAGICAL_TOILET_HACKS	// some quick dirty hacks
//digitalRead(MAGICAL_TRIGGER_PIN)
void magical_trigger_got_hot() {	// must be called when magical trigger was detected high
  if(magical_trigger_enabled) {
    magical_trigger_ISR();	// *not* as ISR
    MENU.outln(F("\nTRIGGERED!"));
    start_musicbox();
#if defined PERIPHERAL_POWER_SWITCH_PIN		// FIXME: try again... ################################################################
    peripheral_power_switch_ON();
#endif
  }
  magical_trigger_enabled=false;
}
#endif


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

void light_sleep() {
  MENU.out(F("light_sleep()\t"));

#if defined USE_BLUETOOTH
  esp_bluedroid_disable();
  esp_bt_controller_disable();
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

  if (esp_sleep_enable_ext0_wakeup((gpio_num_t) MAGICAL_TRIGGER_PIN, 1))
    MENU.error_ln(F("esp_sleep_enable_ext0_wakeup()"));
/*
  if(gpio_wakeup_enable((gpio_num_t) MAGICAL_TRIGGER_PIN, GPIO_INTR_LOW_LEVEL))
    MENU.error_ln(F("gpio_wakeup_enable()"));

  if (esp_sleep_enable_gpio_wakeup())
    MENU.error_ln(F("esp_sleep_enable_gpio_wakeup"));
*/

  MENU.outln(F("sleep well..."));
  delay(1500);

  if (esp_light_sleep_start())
    MENU.error_ln(F("esp_light_sleep_start()"));

  MENU.out(F("\nAWOKE\t"));
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
    MENU.out(F("wakeup EXT0\t"));
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

  if (esp_sleep_enable_ext0_wakeup((gpio_num_t) MAGICAL_TRIGGER_PIN, 1))
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

  rtc_gpio_deinit((gpio_num_t) MAGICAL_TRIGGER_PIN);  // TODO: it will never get here, FIXME: ################
}


unsigned short soft_end_days_to_live = 1;	// remaining days of life after soft end
unsigned short soft_end_survive_level = 4;	// the level a pulse must have reached to survive soft end

void soft_end_playing(int days_to_live, int survive_level) {	// soft ending of magical musicbox
/*
  void soft_end_playing(int days_to_live, int survive_level);

  (primary pulses are selected)
  if(MagicalMusicState)
    if(MagicalMusicState > ENDING) {		// in a play mode: initiate end
      kill all selected pulses that have not been used yet
      also kill all selected p that have not been alive yet for least survive_level times

      set survivers to be COUNTED pulses with 'days_to_live' repeats
      set_MagicalMusicState(ENDING);
    } else
    if(MagicalMusicState == ENDING) {		// in a play mode: initiate end
      check for survivors
      when no one is left, then *SWITCH OFF* musicbox
*/
  if(MagicalMusicState == OFF)
    return;

  if(MagicalMusicState > ENDING) {		// initiate end
    set_MagicalMusicState(ENDING);
    MENU.out(F("soft_end_playing("));		// info
    MENU.out(soft_end_days_to_live);
    MENU.out(F(", "));
    MENU.out(soft_end_survive_level);
    MENU.outln(')');

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
    if(MagicalMusicState == ENDING) {		// ENDING
      for (int pulse=0; pulse<PL_MAX; pulse++) {	// check for any active pulses
	if (PULSES.pulses[pulse].flags & ACTIVE)	//   still something active?
	  return;
      }
      // no activity remaining

      MagicalMusicState = OFF;
      MENU.outln(F("playing ended"));

#if defined PERIPHERAL_POWER_SWITCH_PIN
      MENU.out(F("peripheral POWER OFF "));
      MENU.outln(PERIPHERAL_POWER_SWITCH_PIN);
      peripheral_power_switch_OFF();
      delay(600);	// let power go down softly
#endif

      reset_all_flagged_pulses_GPIO_OFF();
      delay(600);	// send remaining output

      MAGICAL_MUSICBOX_ENDING;	// sleep, restart or somesuch	// *ENDED*
    }
  }
}

void HARD_END_playing(bool with_title) {	// switch off peripheral power and hard end playing
  if(MagicalMusicState == OFF)
    return;

  if(with_title)
    MENU.outln(F("HARD_END_playing()"));

#if defined PERIPHERAL_POWER_SWITCH_PIN
  MENU.out(F("peripheral POWER OFF "));
  MENU.outln(PERIPHERAL_POWER_SWITCH_PIN);
  peripheral_power_switch_OFF();
  delay(600);	// let power go down softly
#endif

  reset_all_flagged_pulses_GPIO_OFF();
  MagicalMusicState = OFF;
  delay(600);	// send remaining output

  MAGICAL_MUSICBOX_ENDING;	// sleep, restart or somesuch	// *ENDED*
}

//#define DEBUG_CLEANUP  TODO: remove debug code
void magical_cleanup(int p) {	// deselect unused primary pulses, check if playing has ended
  if(!magic_autochanges)	// completely switched off by magic_autochanges==false
    return;			// noop

#if defined DEBUG_CLEANUP
  MENU.out(F("CLEANUP "));
#endif
  PULSES.deselect_unused_pulses();	// deselect unused (primary) pulses

  int cnt=0;
  for(int pulse=0; pulse<PL_MAX; pulse++) {
    if(PULSES.pulses[pulse].flags) {	// check if playing has ended  activity?
#if defined DEBUG_CLEANUP
      MENU.out('p');
      MENU.out(pulse);
#endif
      if(PULSES.pulses[pulse].periodic_do == &magical_butler) {
#if defined DEBUG_CLEANUP
	MENU.out(" butler");
#endif
	;
      } else if(PULSES.pulses[pulse].periodic_do == &magical_cleanup) {
#if defined DEBUG_CLEANUP
	MENU.out(" cleanup");
#endif
	;
      } else if(PULSES.pulses[pulse].periodic_do == &cycle_monitor) {
#if defined DEBUG_CLEANUP
	MENU.out(" cycle_monitor");
#endif
	;
      } else
	cnt++;
#if defined DEBUG_CLEANUP
      MENU.tab();
#endif
    }
  }
  PULSES.deselect_unused_pulses();	// dopplet gnäht... nütztabernüt
#if defined DEBUG_CLEANUP
  MENU.ln();
#endif

  if(cnt==0) {
    MENU.outln(F("END reached"));
    HARD_END_playing(false);
  }
}


/*
  void magical_butler(int p)

  enables trigger when due
  then if(magic_autochanges) {
    start soft_end_playing() and start magical_cleanup()
    start HARD_END_playing() if cleanup did not detect END already
*/
//#define DEBUG_BUTLER	TODO: remove debug code
void magical_butler(int p) {
#if defined DEBUG_BUTLER
  MENU.out(F("BUTLER: "));
#endif
  switch(PULSES.pulses[p].counter) {
  case 1:	// prepare enable trigger
    PULSES.pulses[p].period.time = MAGICAL_TRIGGER_BLOCK_SECONDS*1000000L;
    break;
  case 2:	// enable trigger and prepare soft end
    magical_trigger_enabled = true;
    MENU.outln(F("trigger enabled"));
#if defined AUTOMAGIC_CYCLE_TIMING_MINUTES	// MAX minutes
    {
      struct time til_soft_end_time=used_subcycle;
      PULSES.sub_time(MAGICAL_TRIGGER_BLOCK_SECONDS*1000000L, &til_soft_end_time);
      PULSES.add_time(100, &til_soft_end_time);	// tolerance
      PULSES.pulses[p].period = til_soft_end_time;
    }
#else
    PULSES.pulses[p].period.time = (MAGICAL_PERFORMACE_SECONDS - MAGICAL_TRIGGER_BLOCK_SECONDS)*1000000L;
#endif
    break;
  case 3:	// start soft ending and cleanup pulse, prepare for butler hard end
    if(magic_autochanges)
      soft_end_playing(soft_end_days_to_live, soft_end_survive_level);
    // prepare hard end
#if defined AUTOMAGIC_CYCLE_TIMING_MINUTES
    {
      struct time til_hard_end_time = used_subcycle;
      PULSES.div_time(&til_hard_end_time, 2);	// cycle/2 for soft end, then HARD end	// TODO: test&adjust
      PULSES.pulses[p].period = til_hard_end_time;
    }
#else
    PULSES.pulses[p].period.time =
      (MUSICBOX_HARD_END_SECONDS - MAGICAL_PERFORMACE_SECONDS - MAGICAL_TRIGGER_BLOCK_SECONDS)*1000000L;
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
}


void magical_music_box_setup() {
  MENU.ln();

#if defined MAGICAL_TRIGGER_PIN
  MENU.out(F("MAGICAL TRIGGER PIN:\t"));
  MENU.outln(MAGICAL_TRIGGER_PIN);
#endif	// MAGICAL_TRIGGER_PIN

#if defined MAGICAL_PERFORMACE_SECONDS
  MENU.out(F("performance seconds:\t"));
  MENU.outln(MAGICAL_PERFORMACE_SECONDS);
#endif

#if defined MAGICAL_TRIGGER_BLOCK_SECONDS
  MENU.out(F("disable retriggering:\t"));
  MENU.outln(MAGICAL_TRIGGER_BLOCK_SECONDS);
#endif

#if defined MUSICBOX_HARD_END_SECONDS
  MENU.out(F("hard stop:\t\t"));
  MENU.outln(MUSICBOX_HARD_END_SECONDS);
#endif

  if (esp_sleep_enable_ext0_wakeup((gpio_num_t) MAGICAL_TRIGGER_PIN, 1))
    MENU.error_ln(F("esp_sleep_enable_ext0_wakeup()"));
}

/* **************************************************************** */
// musicBox menu

void musicBox_display() {
  MENU.outln(F("The Harmonical Music Box  http://github.com/reppr/pulses/\n"));

  MENU.out(F("autochanges "));
  if(magic_autochanges)
    MENU.out(F("ON"));
  else
    MENU.out(F("off"));
  MENU.out(F(" 'a' to toggle"));

  MENU.tab();
  MENU.out(F("'c' cycle "));
  // TODO: do *not* expect it on pulse pulses[0]
  struct time period = PULSES.pulses[0].period;
  PULSES.display_time_human(scale2harmonical_cycle(selected_in(SCALES), &period));
  MENU.ln();

  MENU.out(F("soft_end("));
  MENU.out(soft_end_days_to_live);	// remaining days of life after soft end
  MENU.out(F(", "));
  MENU.out(soft_end_survive_level);	// the level a pulse must have reached to survive soft en
  MENU.outln(F(")\t'd'=days to survive  'l'=level minimal age 'E'= start soft end now"));
  MENU.outln(F("hard end='H'"));
}


bool musicBox_reaction(char token) {
  int input_value;

  switch(token) {
  case 'a':
    magic_autochanges = !magic_autochanges;
    break;
  case 'c':
    show_cycle(cycle);
    break;
  case 'E':
    soft_end_playing(soft_end_days_to_live, soft_end_survive_level);
    break;
  case 'd':
    input_value = MENU.numeric_input(soft_end_days_to_live);
    if(input_value >= 0)
      soft_end_days_to_live = input_value;
    break;
  case 'l':
    input_value = MENU.numeric_input(soft_end_survive_level);
    if(input_value >= 0)
      soft_end_survive_level = input_value;
    break;
  case 'H':
    HARD_END_playing(true);
    break;
  default:
    return false;
  }

  return true;
}
