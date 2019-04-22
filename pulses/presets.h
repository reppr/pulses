/*
  presets.h
  temporary development file
*/

#include "preset_data.h"

#if defined SHORT_PRESET_COLLECTION
  #undef MUSICBOX_PRESETs
  #define MUSICBOX_PRESETs	SHORT_PRESET_COLLECTION		// *from now on* use only first <n> presets
#endif

void play_random_preset() {
  tabula_rasa();
  MENU.ln();

  MENU.out(F("random("));
  MENU.out(MUSICBOX_PRESETs);
  MENU.out(F(") "));
  load_preset(random(MUSICBOX_PRESETs) + 1);	// TODO: sets preset, how to unset? ################
  no_restore_from_RTCmem = true;	// one shot, gets reset
  start_musicBox();
}
