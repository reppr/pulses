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
    (*u8x8_p).print(' ');

}

#if defined USE_MONOCHROME_DISPLAY
void monochrome_preset_names(short start_at_preset=0) {
  static short last_shown_preset=0;

  int preset_was = musicBoxConf.preset;
  int verbosity_was=MENU.verbosity;
  MENU.verbosity = 0;	// *no* output from set_metric_pitch
  short rows = (*u8x8_p).getRows();
  uint8_t col;
  char buffer[(*u8x8_p).getCols() +1] = {0};
  if(start_at_preset > 0)
    last_shown_preset = start_at_preset - 1;

  (*u8x8_p).clear();
  musicBoxConf.preset = last_shown_preset;
  for(int row=0; row<rows; row++) {
    if(musicBoxConf.preset++ >= MUSICBOX_PRESETs) {
      musicBoxConf.preset=0;
      break;
    } // else
    load_preset((int) musicBoxConf.preset, false);
    (*u8x8_p).setCursor(0,row);
    (*u8x8_p).print(musicBoxConf.preset);
    MENU.out(musicBoxConf.preset);
    (*u8x8_p).print(' ');
    MENU.space();
    col=2;
    if(musicBoxConf.preset>9)
      col++;
    if(musicBoxConf.preset>99)
      col++;
    strncpy(buffer, musicBoxConf.name, sizeof(buffer) - col -1);
    (*u8x8_p).print(buffer);
    MENU.outln(musicBoxConf.name);
  }

  last_shown_preset=musicBoxConf.preset;
  musicBoxConf.preset = preset_was;
  load_preset((int) preset_was, false);		// restore preset
  MENU.verbosity = verbosity_was;
} // monochrome_preset_names()
#endif // USE_MONOCHROME_DISPLAY
