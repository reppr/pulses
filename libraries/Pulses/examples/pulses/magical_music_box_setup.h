/*
  magical_music_box_setup.h
*/

// setup MAGICAL_TRIGGER_PIN ISR
#if defined MAGICAL_TRIGGER_PIN
MENU.out(F("magical trigger: "));
MENU.outln(MAGICAL_TRIGGER_PIN);

pinMode(MAGICAL_TRIGGER_PIN, INPUT);
attachInterrupt(digitalPinToInterrupt(MAGICAL_TRIGGER_PIN), magical_trigger_ISR, RISING);
#endif	// MAGICAL_TRIGGER_PIN
