/*
  ledc_audio.h

  see: https://github.com/espressif/arduino-esp32/blob/a4305284d085caeddd1190d141710fb6f1c6cbe1/cores/esp32/esp32-hal-ledc.h
*/

#ifndef LEDC_AUDIO_H
/*
  LEDC: maximal frequency is 80000000 / 2^resolution
  ...
  9  bit kHz 156.25
  10 bit kHz  78.125
  11 bit kHz  39.0625	<- ***
  12 bit kHz  19.53125	<- *****
  13 bit kHz   9.765625
  ...
*/

#include "driver/ledc.h"

uint8_t	ledc_audio_channel=0;
uint8_t	ledc_audio_resolution=12;
double	ledc_audio_frequency=19.53125;

unsigned int ledc_audio_max;	// keeps maximal pwm value possible (at this resolution)

unsigned int ledc_audio_set_max() {
  ledc_audio_max = 1 << ledc_audio_resolution;
  return --ledc_audio_max;
}

//	unsigned_int ledc_audio_set_pwm_frequency(float frequency /* kHz */) {
//	}


/*
  see: https://github.com/espressif/esp-idf/blob/master/examples/peripherals/ledc/main/ledc_example_main.c
*/
void ledc_audio_setup() {
  MENU.outln(F("ledc_audio_setup()"));
// compiler does not accept that :(
//  ledc_timer_config_t ledc_audio_timer_cf = {
//    .duty_resolution = LEDC_TIMER_12_BIT,	// resolution of PWM duty
//    .freq_hz = 19531,				// frequency of PWM signal
//    .speed_mode = LEDC_HIGH_SPEED_MODE,		// timer mode
//    .timer_num = LEDC_TIMER_0			// timer index
//  };
//
//  // Set configuration of timer0 for high speed
//  if(ledc_timer_config(&ledc_audio_timer_cf);)
//    ERROR_ln("ledc_timer_config()");
//  else {
//    ;
//  }
  ledcSetup(ledc_audio_channel, ledc_audio_frequency, ledc_audio_resolution);

  ledc_audio_set_max();		// get maximal possible pwm value
}


#define LEDC_AUDIO_H
#endif
