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

  int verbosity_was=MENU.verbosity;
  MENU.verbosity = 0;		// set verbosity==0 to avoid output from set_metric_pitch()
  load_preset(random(MUSICBOX_PRESETs) + 1);
  MENU.verbosity = verbosity_was;
  no_restore_from_RTCmem = true;		// one shot, gets reset
  start_musicBox();
#if defined HAS_OLED
  monochrome_print(' ');
#endif
} // play_random_preset()


#if defined HAS_OLED
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
    monochrome_print(musicBoxConf.preset);
    MENU.out(musicBoxConf.preset);
    monochrome_print(' ');
    MENU.space();
    col=2;
    if(musicBoxConf.preset>9)
      col++;
    if(musicBoxConf.preset>99)
      col++;
    strncpy(buffer, musicBoxConf.name, sizeof(buffer) - col -1);
    monochrome_print(buffer);
    MENU.outln(musicBoxConf.name);
  }

  last_shown_preset=musicBoxConf.preset;
  musicBoxConf.preset = preset_was;
  load_preset((int) preset_was, false);		// restore preset
  MENU.verbosity = verbosity_was;
} // monochrome_preset_names()

#elif defined HAS_ePaper

short preset_list_start_at=0;	// next start

void ePaper_preset_names() {
  int preset_was = musicBoxConf.preset;
  int verbosity_was=MENU.verbosity;
  MENU.verbosity = 0;	// *no* output from set_metric_pitch
  short rows = 5;
  uint8_t col;
  char buffer[28] = {0};	// TODO: 28 was trial&error

  ePaper.setFullWindow();
  ePaper.fillScreen(GxEPD_WHITE);
  ePaper.setTextWrap(false);	// trimm names to one line on ePaper
  set_used_font(medium_font_p);
  extern void ePaper_setCursor_0_0();
  ePaper_setCursor_0_0();

  musicBoxConf.preset = preset_list_start_at;
  for(int row=0; row<rows; row++) {
    if(musicBoxConf.preset++ >= MUSICBOX_PRESETs) {
      musicBoxConf.preset=0;
      break;
    } // else
    load_preset((int) musicBoxConf.preset, false);
    ePaper.print(musicBoxConf.preset);
    MENU.out(musicBoxConf.preset);
    ePaper.print(' ');
    MENU.space();
    col=2;
    if(musicBoxConf.preset>9)
      col++;
    if(musicBoxConf.preset>99)
      col++;
    uint8_t maxlen = 25;	// TODO: adapt if needed
    maxlen -= col;
    strncpy(buffer, musicBoxConf.name, maxlen);
    ePaper.println(buffer);
    MENU.outln(musicBoxConf.name);
  }
  ePaper.display(true);
  ePaper.setTextWrap(true);	// restore default

  preset_list_start_at=musicBoxConf.preset;
  musicBoxConf.preset = preset_was;
  load_preset((int) preset_was, false);		// restore preset
  MENU.verbosity = verbosity_was;
} // ePaper_preset_names(void)

void show_ePaper_preset_names(short start_at_preset) {
  if(start_at_preset > 0)
    preset_list_start_at = start_at_preset - 1;

#if defined MULTICORE_DISPLAY
  MC_do_on_other_core(&ePaper_preset_names);
#else
  ePaper_preset_names();
#endif
}
#endif

int search_preset_list(char* s) {
  if(s==NULL || strlen(s)==0)
    return -1;	// error

  int preset_was = musicBoxConf.preset;
  int verbosity_was=MENU.verbosity;
  MENU.verbosity = 0;	// *no* output from set_metric_pitch

  int len = strlen(s);
  char* lower_search_str = (char*) malloc(len + 1);
  if(lower_search_str==NULL) {
    MENU.malloc_error();
    return -1;	// error
  }
  extern void copy_string_to_lower(char* source, char* destination, size_t max);
  copy_string_to_lower(s, lower_search_str, len);


  int max= 100;
  char* name_buffer = (char*) malloc(max + 1);
  if(name_buffer==NULL) {
    MENU.malloc_error();
    free(lower_search_str);
    return -1;	// error
  }

  int found=0;
  for (musicBoxConf.preset=1; musicBoxConf.preset <= MUSICBOX_PRESETs; musicBoxConf.preset++) {
    load_preset(musicBoxConf.preset, false);
    copy_string_to_lower(musicBoxConf.name, name_buffer, max);
    if(strstr(name_buffer, lower_search_str)) {
      MENU.out(musicBoxConf.preset);
      MENU.tab();
      MENU.outln(musicBoxConf.name);
      snprintf(name_buffer, max, "%i %s", musicBoxConf.preset, musicBoxConf.name);
#if defined HAS_OLED
      monochrome_print_1line(found, name_buffer);
#elif defined HAS_DISPLAY
      MC_print_1line_at(found, name_buffer);
#endif
      found++;
    }
  }

#if defined HAS_OLED	// DADA TODO: HAS_ePaper
  // if(monochrome_can_be_used()) {
  if(found < (*u8x8_p).getRows())	// avoid scrolling when screen is full
    (*u8x8_p).clearLine(found);		// else empty line
  // }
#endif

  MENU.out(F(">>  "));
  MENU.out(found);
  MENU.out("\t\"");
  MENU.out(s);
  MENU.outln(F("\" found\n"));

  free(lower_search_str);
  free(name_buffer);

  load_preset(musicBoxConf.preset = preset_was, false);	// restore preset
  MENU.verbosity = verbosity_was;
  return found;
} // search_preset_list()
