/*
  magical_music_box_setup.h
*/

MENU.ln();

#if defined MAGICAL_TRIGGER_PIN
  MENU.out(F("MAGICAL TRIGGER PIN: "));
  MENU.outln(MAGICAL_TRIGGER_PIN);
#endif	// MAGICAL_TRIGGER_PIN

#if defined MAGICAL_PERFORMACE_SECONDS
  MENU.out(F("performance seconds: "));
  MENU.outln(MAGICAL_PERFORMACE_SECONDS);
#endif

#if defined MAGICAL_MUSICBOX_TRIGGER_BLOCK_SECONDS
  MENU.out(F("disable retriggering: "));
  MENU.outln(MAGICAL_MUSICBOX_TRIGGER_BLOCK_SECONDS);
#endif
