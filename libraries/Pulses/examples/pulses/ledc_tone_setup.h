/*
  ledc_tone_setup.h
*/

#ifndef LEDC_PIN
  #if defined MORSE_OUTPUT_PIN
    #define	LEDC_PIN	MORSE_OUTPUT_PIN
  #else
    #define	LEDC_PIN	0
  #endif
#endif


#ifdef LEDC_PIN	// use LEDc
  MENU.out(F("LEDC pin "));
  MENU.outln(LEDC_PIN);
#endif

ledcSetup(ledc_channel, ledc_frequency, ledc_resolution);
ledcWriteTone(ledc_channel, ledc_frequency);
//ledcAttachPin(LEDC_PIN, ledc_channel);

/*
MENU.outln('A');
for(int i=0; i < 8 ; i++) {
  ledcAttachPin(LEDC_PIN, ledc_channel);
  delay(500);
  ledcDetachPin(LEDC_PIN);
  delay(500);
}
*/
