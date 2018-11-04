/*
  magical_musicbox.h
*/

//#define DEBUGGING_MAGICAL_MUSICBOX
#if defined DEBUGGING_MAGICAL_MUSICBOX
 #define MAGICAL_PERFORMACE_SECONDS	20
 #define MAGICAL_TRIGGER_BLOCK_SECONDS	2
 #define MUSICBOX_HARD_END_SECONDS	30
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
  #if defined BRACHE_NOV_2018_SETTINGS
    #define MAGICAL_PERFORMACE_SECONDS	6*60	// BRACHE
  #else
    #define MAGICAL_PERFORMACE_SECONDS	12*60
  #endif
#endif

#if ! defined MAGICAL_TRIGGER_BLOCK_SECONDS
  #if defined BRACHE_NOV_2018_SETTINGS
    #define MAGICAL_TRIGGER_BLOCK_SECONDS	30	// BRACHE
  #else
    #define MAGICAL_TRIGGER_BLOCK_SECONDS	3
  #endif
#endif

bool blocked_trigger_shown=false;	// show only once a run

#if ! defined MUSICBOX_HARD_END_SECONDS
  #if defined BRACHE_NOV_2018_SETTINGS
    #define  MUSICBOX_HARD_END_SECONDS	9*60	// BRACHE
  #else
    #define  MUSICBOX_HARD_END_SECONDS	15*60
  #endif
#endif

#if ! defined MAGICAL_MUSICBOX_ENDING	// *one* of the following:
  #define  MAGICAL_MUSICBOX_ENDING	light_sleep();	// works fine
  //#define  MAGICAL_MUSICBOX_ENDING	deep_sleep();	// still DAC noise!!!
  //#define  MAGICAL_MUSICBOX_ENDING	ESP.restart();	// works fine
#endif

// #define PERIPHERAL_POWER_SWITCH_PIN		// TODO: file?

enum music_box_state {OFF=0, ENDING, SLEEPING, SNORING, AWAKE, FADE};
music_box_state MagicalMusicState=OFF;

bool magic_autochanges=true;	// switch if to end normal playing after MAGICAL_PERFORMACE_SECONDS

portMUX_TYPE magical_MUX = portMUX_INITIALIZER_UNLOCKED;

// very simple one shot implementation:
//void furzificate(int dummy=0) {	// switch to a quiet, farting patterns, u.a.
//  /* with the dummy int parameter it passes as payload for a pulse */
void furzificate() {	// switch to a quiet, farting patterns, u.a.
  MENU.outln(F("furzificate()"));
  MagicalMusicState = SNORING;

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

struct time musicbox_start_time;
struct time musicbox_end_time;
struct time musicbox_hard_end_time;

void start_musicbox() {
  MENU.outln(F("start_musicbox()"));
  MagicalMusicState = AWAKE;

  blocked_trigger_shown = false;	// show only once a run

#if defined USE_BATTERY_CONTROL
  show_battery_level();
  void HARD_END_playing();	// defined below
  if(assure_battery_level())
    MENU.outln(F("power accepted"));
  else {
    MENU.outln(F(">>> NO POWER <<<"));
    HARD_END_playing();
  }
#endif

#if defined PERIPHERAL_POWER_SWITCH_PIN
  peripheral_power_switch_ON();

  MENU.out(F("peripheral POWER ON "));
  MENU.outln(PERIPHERAL_POWER_SWITCH_PIN);

  delay(250);	// give peripheral supply voltage time to stabilise
#endif

  MENU.outln(F(" >>> * <<<"));
  MENU.men_selected = 0;
  MENU.play_KB_macro(F("-E40,"), false); // initialize, the comma avoids output from E40, no newline

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

  // random pitch
  PULSES.time_unit=1000000;	// default metric
  multiplier=4096;		// uses 1/4096 jiffles
  multiplier *= 8;	// TODO: adjust appropriate...
  //  divisor = 294;		// 293.66 = D4	// default tuning D4

  divisor = random(160, 450);	// *not* tuned for other instruments
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

  musicbox_start_time = musicbox_end_time = PULSES.get_now();	// keep musicbox_start_time
  struct time duration;
  duration.overflow=0;
  duration.time=MAGICAL_PERFORMACE_SECONDS*1000000;	// how long to play
  PULSES.add_time(&duration, &musicbox_end_time);	// keep musicbox_end_time

  duration.overflow=0;					// compute musicbox_hard_end_time
  duration.time=MUSICBOX_HARD_END_SECONDS*1000000;	// how long to play before hard end
  musicbox_hard_end_time = PULSES.get_now();
  PULSES.add_time(&duration, &musicbox_hard_end_time);	// keep hard end time

  PULSES.activate_selected_synced_now(sync);	// 'n' sync and activate
}

void magical_stress_release() {		// special stress release for magical music box
  if (voices) {	// normal case, I have never seen exceptions
    PULSES.init_pulse(--voices);		// *remove* topmost voice
    PULSES.select_n(voices);
    MENU.out(F("magical_stress_release() "));
    MENU.outln(voices);
  } else MENU.play_KB_macro("X");		// *could* happen some time, maybe *SAVETY NET*
}


// enum music_box_state {OFF=0, SLEEPING, SNORING, AWAKE, FADE};

unsigned int magical_trigger_cnt=0;
void magic_trigger_OFF();
bool switch_magical_trigger_off=false;

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
    triggered = true;
    break;
  case AWAKE:	// was it awake long enough for triggering again?
    PULSES.sub_time(&triggered_at, &duration);
    if(duration.overflow)
      triggered=true;
    else if (duration.time > MAGICAL_TRIGGER_BLOCK_SECONDS*1000000)	// block trigger for some time
      triggered=true;
    else {					// trigger was blocked
      if(! blocked_trigger_shown)		//  show *only once* a run
	MENU.outln(F("trigger blocked"));	//  i know it is *bad*, i do it the same...
      blocked_trigger_shown = true;
    }
    break;
  default:
    triggered = true;	// not save... but
    MENU.outln(F("magical_trigger_ISR unknown state"));	// should not happen
  }

  if(triggered) {
    switch_magical_trigger_off = true;	// dopplet gnäht
    magical_trigger_cnt++;
    triggered_at = new_trigger;
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
  switch_magical_trigger_off = false;
#else
  ;
#endif
}


#if defined MAGICAL_TOILET_HACKS	// some quick dirty hacks
//digitalRead(MAGICAL_TRIGGER_PIN)
void magical_trigger_is_hot() {
  magical_trigger_ISR();	// *not* as ISR
  if (switch_magical_trigger_off) {
    MENU.outln(F("\nTRIGGERED!"));
    start_musicbox();
#if defined PERIPHERAL_POWER_SWITCH_PIN		// FIXME: try again... ################################################################
    peripheral_power_switch_ON();
#endif
  }
  switch_magical_trigger_off=false;
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

  esp_bluedroid_disable();
  esp_bt_controller_disable();
  esp_wifi_stop();

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

//  if(gpio_wakeup_enable((gpio_num_t) MAGICAL_TRIGGER_PIN, GPIO_INTR_LOW_LEVEL))
//    MENU.error_ln(F("gpio_wakeup_enable()"));
//
//  if (esp_sleep_enable_gpio_wakeup())
//    MENU.error_ln(F("esp_sleep_enable_gpio_wakeup"));

  MENU.outln(F("sleep well..."));
  delay(1500);

  if (esp_light_sleep_start())
    MENU.error_ln(F("esp_light_sleep_start()"));

  MENU.out(F("AWOKE\t"));
  int cause = esp_sleep_get_wakeup_cause();
  switch (cause = esp_sleep_get_wakeup_cause()) {
  case 0:	// ESP_SLEEP_WAKEUP_UNDEFINED	0
    MENU.outln(F("wakeup undefined"));
#if defined AUTOSTART
    MENU.out(F("HACK: restart anyway "));
    AUTOSTART
#endif
    break;
  case 7:	// ESP_SLEEP_WAKEUP_GPIO	7
    MENU.outln(F("wakeup gpio"));
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


void soft_end_playing() {	// set all selected to be counted pulses with 1 repeat
  if(MagicalMusicState == OFF)
    return;

  if(MagicalMusicState > ENDING) {		// initiate end
    MagicalMusicState = ENDING;
    MENU.outln(F("soft_end_playing()"));

    for (int pulse=0; pulse<PL_MAX; pulse++) {	// 1 shot generating pulses
      if (PULSES.pulse_is_selected(pulse)) {
	if(PULSES.pulses[pulse].counter) {	// pulse was already awake: still 1 repeat, then vanish
	  PULSES.pulses[pulse].remaining = 1;
	  PULSES.pulses[pulse].flags |= COUNTED;
	} else {
	  PULSES.init_pulse(pulse);		// pulse have not been awake yet, just remove
	}
      }
    }
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

void HARD_END_playing() {	// switch off peripheral power and hard end playing
  if(MagicalMusicState == OFF)
    return;

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


void  magical_music_box_setup() {
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
}
