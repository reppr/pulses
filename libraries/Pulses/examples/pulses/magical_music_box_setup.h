/*
  magical_music_box_setup.h
*/

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
