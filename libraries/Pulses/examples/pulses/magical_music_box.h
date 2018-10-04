/*
  magical_musicbox.h
*/

#include "random_entropy.h"

portMUX_TYPE magical_MUX = portMUX_INITIALIZER_UNLOCKED;

short musicbox_incarnation=0;	// debugging only
enum music_box_state {OFF=0, SLEEPING, SNORING, AWAKE, FADE};

#ifndef MAGICAL_TRIGGER_PIN
//#define MAGICAL_TRIGGER_PIN	35	// == PLL
//#define MAGICAL_TRIGGER_PIN	12	// just a test...
  #define MAGICAL_TRIGGER_PIN	34	// == InMorse
#endif

void magic_trigger_ON();	// forward declaration

void start_musicbox() {
  // magic_trigger_OFF();
  musicbox_incarnation++;	// debugging only
  MENU.outln(F("start_musicbox()"));

  MENU.play_KB_macro(F("-E40,")); // initialize, the comma avoids output from E40
  sync = random(6);		// random sync
  MENU.out(F("sync "));
  MENU.outln(sync);

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
  MENU.out(F("scale: "));
  MENU.outln(selected_name(SCALES));

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
  switch(random(7)) {
  case 0: case 1:
    break;
  case 2: case 3:
    MENU.play_KB_macro(F("*2"));
    break;
   case 4:
    MENU.play_KB_macro(F("*4"));
    break;
   case 5:
    MENU.play_KB_macro(F("*6"));
    break;
  case 6:
    MENU.play_KB_macro(F("/2"));
    break;
  }

  // random jiffle
  switch(random(99)) {
  case 0: case 1: case 2: case 3: case 4:
    select_array_in(JIFFLES, PENTAtonic_rising);
    break;
  case 5: case 6: case 7: case 8: case 9:
    select_array_in(JIFFLES, PENTAtonic_descending);
    break;

  case 10: case 11: case 12: case 13: case 14:
    select_array_in(JIFFLES, pentatonic_rising);
    break;
  case 15: case 16: case 17: case 18: case 19:
    select_array_in(JIFFLES, pentatonic_descending);
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
    select_array_in(JIFFLES, PENTAtonic_thirds_rising);
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
    select_array_in(JIFFLES, PENTAtonic_thirds_falling_5);
    break;
  case 74: case 75: case 76: case 77:
    select_array_in(JIFFLES, pentatonic_thirds_falling_5);
    break;
  case 78: case 79: case 80: case 81:
    select_array_in(JIFFLES, rising_pent_theme);
    break;
  case 82: case 83: case 84: case 85:
    select_array_in(JIFFLES, pentatonic_thirds_rising);
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
  MENU.out(F("jiffle: "));
  MENU.outln(selected_name(JIFFLES));

  setup_jiffle_thrower_selected(selected_actions);
  PULSES.activate_selected_synced_now(sync);	// 'n' sync and activate
}

void magical_stress_release() {		// special stress release for magical music box
  PULSES.deactivate_pulse(voices);	// *remove* topmost voice
  PULSES.select_n(--voices);
  MENU.out(F("magical_stress_release() "));
  MENU.outln(voices);
  // magic_trigger_ON();	// TODO: test if we might need this...
}

// very simple one shot implementation:
void furzificate() {	// switch to a quiet, farting patterns, u.a.
  MENU.outln(F("furzificate()"));
  musicbox_incarnation++;	// debugging only

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

  // magic_trigger_ON();
}


// enum music_box_state {OFF=0, SLEEPING, SNORING, AWAKE, FADE};

unsigned int magical_trigger_cnt=0;
void magic_trigger_OFF();
bool switch_magical_trigger_off=false;

void magical_trigger_ISR() {	// can also be used on the non interrupt version :)
  portENTER_CRITICAL_ISR(&magical_MUX);
  // magic_trigger_OFF();
  static struct time triggered_at=PULSES.get_now();	// preserves last seen fresh trigger time

  struct time new_trigger = PULSES.get_now();		// new trigger time
  struct time duration = new_trigger;			// remember

  bool triggered=false;
  //MENU.outln('t');	// TODO: remove
  //MENU.out(F("magical_trigger_ISR()\t"));	// TODO: remove
  //MENU.outln(magical_trigger_cnt);		// TODO: remove

  PULSES.sub_time(&triggered_at, &duration);
  if(duration.overflow)
    triggered=true;
  else if (duration.time > 30*1000000)	// block trigger for 30 seconds
      triggered=true;

  if(triggered) {
    //magic_trigger_OFF();		// dopplet gnäht
    switch_magical_trigger_off = true;	// dopplet gnäht
    magical_trigger_cnt++;
    triggered_at = new_trigger;
  }

//  switch (music_box_state) {
//  case SNORING:
//    MENU.outln(F("SNORING"));	// TODO: remove
//    break;
//  case AWAKE:
//    MENU.outln(F("AWAKE"));	// TODO: remove
//    break;
//  default:
//    MENU.outln(F("???"));	// TODO: remove
//  }
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
    MENU.outln(F("TRIGGERED!"));
    start_musicbox();
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
  MENU.out("magical_FART\t");
  MENU.out(sense_pin);
  MENU.out('>');
  MENU.outln(output_pin);
  pinMode(sense_pin, INPUT);
  attachInterrupt(digitalPinToInterrupt(sense_pin), magical_fart_ISR, CHANGE);
}
