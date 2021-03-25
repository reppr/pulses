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

// TODO: REMOVE: ledc_audio_pin0  and all related code
// uint8_t ledc_audio_pin0=26;	// hi jacking dac2 channel for a test
uint8_t ledc_audio_pin0=19;	// *TEMPORARY* using GPIO19 as ledc test pin
uint8_t	ledc_audio_channel_0=0;	// LEDC_CHANNEL_0

uint8_t	ledc_audio_resolution=12;
double	ledc_audio_frequency=19531.25;

unsigned int ledc_audio_max;	// keeps maximal pwm value *LIMIT* at this resolution

unsigned int ledc_audio_set_max() {
  ledc_audio_max = 1 << ledc_audio_resolution;	// limit above range
  return --ledc_audio_max;			// maximal possible value
}

// DAx_max maximal data value for digital to analogue system (either hardware DAC or LEDC)
#if ! defined LEDC_INSTEAD_OF_DACs
  int DAx_max=255;
#else
  int DAx_max= (1 << ledc_audio_resolution) -1 ;
#endif

//#define try_ARDUINO_LEDC_version	// define in my_pulses_config.h or in pulses_engine_config.h to be visible in Pulses.cpp
#if defined  try_ARDUINO_LEDC_version
	// LEDC Arduino version:
	/*
	  https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/esp32-hal-ledc.h

	double      ledcSetup(uint8_t channel, double freq, uint8_t resolution_bits);
	void        ledcWrite(uint8_t channel, uint32_t duty);
	double      ledcWriteTone(uint8_t channel, double freq);
	double      ledcWriteNote(uint8_t channel, note_t note, uint8_t octave);
	uint32_t    ledcRead(uint8_t channel);
	double      ledcReadFreq(uint8_t channel);
	void        ledcAttachPin(uint8_t pin, uint8_t channel);
	void        ledcDetachPin(uint8_t pin);
	*/

  void ledc_audio_setup() {	// ARDUINO version
	  MENU.out(F("ARDUINO ledc_audio_setup("));
	  MENU.out(ledc_audio_channel_0);
	  MENU.out(',');
	  MENU.space();
	  MENU.out(ledc_audio_frequency);
	  MENU.out(',');
	  MENU.space();
	  MENU.out(ledc_audio_resolution);
	  MENU.out(F(")\tfreq="));

	  // double ledcSetup(uint8_t channel, double freq, uint8_t resolution_bits);
	  ledcSetup(ledc_audio_channel_0, ledc_audio_frequency, ledc_audio_resolution);	// TODO: REMOVE: ledc_audio_pin0
	  MENU.out(ledcReadFreq(ledc_audio_channel_0));

	  // void ledcAttachPin(uint8_t pin, uint8_t channel);
	  ledcAttachPin(ledc_audio_pin0, ledc_audio_channel_0);	// TODO: REMOVE: ledc_audio_pin0  and all related code

	  // void ledcWrite(uint8_t channel, uint32_t duty);
	  ledcWrite(ledc_audio_channel_0, 0);		// TODO: REMOVE: ledc_audio_pin0  and all related code

	  MENU.out(F("\t0..."));
	  MENU.out(ledc_audio_set_max());
	  MENU.out(F("\tfreq read: "));
	  MENU.outln(ledcReadFreq(ledc_audio_channel_0));
	} // ARDUINO version	ledc_audio_setup()

    #define pulses_ledc_write(c, v)	ledcWrite((c), (v))

#else // ESP IDF version (not try_ARDUINO_LEDC_version)

	#include "driver/ledc.h"

	/*
	  see: https://github.com/espressif/esp-idf/blob/master/examples/peripherals/ledc/main/ledc_example_main.c
	*/
	void ledc_audio_setup() {	// ESP IDE version
	  MENU.out(F("ESP IDE  ledc_audio_setup("));
	  MENU.out(ledc_audio_channel_0);
	  MENU.out(',');
	  MENU.space();
	  MENU.out(ledc_audio_frequency);
	  MENU.out(',');
	  MENU.space();
	  MENU.out(ledc_audio_resolution);
	  MENU.out(F(")\t"));
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
	  ledcSetup(ledc_audio_channel_0, ledc_audio_frequency, ledc_audio_resolution);	// borrowed from ARDUINO version!

	  MENU.out(ledc_audio_set_max());	// get&show maximal possible pwm value

	  MENU.out(F("\tfreq read:"));
	  MENU.outln(ledcReadFreq(ledc_audio_channel_0));	// ARDUINO style does not work here
	  // uint32_t ledc_get_freq(ledc_mode_tspeed_mode, ledc_timer_t timer_num)

	  MENU.out(F("ledc_set_pin() "));
	  esp_err_t status;
#if ! defined LEDC_INSTEAD_OF_DACs
	  MENU.out(ledc_audio_pin0);	// TODO: REMOVE: ledc_audio_pin0  and all related code
	  MENU.tab();
	  status = ledc_set_pin(ledc_audio_pin0, LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);	// TODO: REMOVE: ledc_audio_pin0
	  esp_err_info(status);
#else
	  MENU.out(F("25, 26\t"));
	  ledcSetup(LEDC_CHANNEL_1, ledc_audio_frequency, ledc_audio_resolution);	// borrowed from ARDUINO version!
	  status = ledc_set_pin(25, LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1);	// channel 1 pin 25
	  esp_err_info(status);

	  ledcSetup(LEDC_CHANNEL_2, ledc_audio_frequency, ledc_audio_resolution);	// borrowed from ARDUINO version!
	  status = ledc_set_pin(26, LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2);	// channel 2 pin 26
	  esp_err_info(status);
#endif
	} // ESP IDE version  ledc_audio_setup()

	void pulses_ledc_write(ledc_channel_t channel, uint32_t value) {
	  ledc_set_duty(LEDC_HIGH_SPEED_MODE, channel, value);
	  ledc_update_duty(LEDC_HIGH_SPEED_MODE, channel);
	} // ESP IDE version  pulses_ledc_write()


	// esp_err_t ledc_set_pin(int gpio_num, ledc_mode_t speed_mode, ledc_channel_t ledc_channel)

	// esp_err_t ledc_stop(ledc_mode_tspeed_mode, ledc_channel_tchannel, uint32_t idle_level)

	// uint32_t ledc_get_freq(ledc_mode_tspeed_mode, ledc_timer_t timer_num)

	// esp_err_t ledc_set_duty(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t duty)

	// uint32_t ledc_get_duty(ledc_mode_t speed_mode, ledc_channel_t channel)

	// esp_err_t ledc_set_duty_and_update(ledc_mode_tspeed_mode, ledc_channel_tchannel, uint32_t duty, uint32_t hpoint)

	// esp_err_t ledc_update_duty(ledc_mode_t speed_mode, ledc_channel_t channel)

#endif // try_ARDUINO_LEDC_version or ESP LEDC version

#define LEDC_AUDIO_H
#endif
