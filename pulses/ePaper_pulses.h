/*
  ePaper_pulses.h
*/


#define LIN_BUF_MAX	42	// temporal local line buffers only


void MC_display_message(const char* text) {	// inline does not work
#if defined DEBUG_ePAPER
  MENU.out(F("DEBUG_ePAPER\tMC_display_message()\t"));
#endif
  MENU.outln(text);
  set_used_font(medium_font_p);
  MC_print_1line_at(3, text, /*offset_y*/ 27);	// bottom line
} // MC_display_message()


#define ePAPER_at_0_0_POSITIONING_WORKAROUND	// the old code did not work right any more: position was too low!
void ePaper_setCursor_0_0() {
#if defined ePAPER_at_0_0_POSITIONING_WORKAROUND // WORKAROUND to fix Cursor(0,0) y-positioning
  #warning uses ePAPER_at_0_0_POSITIONING_WORKAROUND
  ePaper.setCursor(0, used_font_yAdvance - 6);	// 6 by TRIAL&ERROR	was: ePaper.setCursor(0,0);
#else
  ePaper.setCursor(0,0);		// old code (*was* doing the right thing)
  ePaper.println();
#endif
} // ePaper_setCursor_0_0()


#if defined USE_ESP_NOW
void ePaper_show_peer_list() {
  int peer_cnt=0;

  set_used_font(medium_font_p);
  for(int i=0; i<ESP_NOW_MAX_TOTAL_PEER_NUM; i++) {
    if(mac_is_non_zero(esp_now_pulses_known_peers[i].mac_addr))
      peer_cnt++;
  }

  ePaper.setFullWindow();
  ePaper.fillScreen(GxEPD_WHITE);
  ePaper_setCursor_0_0();

  bool is_send_to_peer;
  for(int i=0; i<ESP_NOW_MAX_TOTAL_PEER_NUM; i++) {
    if(mac_is_non_zero(esp_now_pulses_known_peers[i].mac_addr)) {
      ePaper.print(i+1);
      if((is_send_to_peer = (! mac_cmp(esp_now_send2_mac_p, esp_now_pulses_known_peers[i].mac_addr))))	// send to peer?
	ePaper.print('>');
      else
	ePaper.print(' ');
      ePaper.print(esp_now_pulses_known_peers[i].preName);
      if(is_send_to_peer)
	ePaper.print(F(" <"));
      ePaper.println();
    }
  }
  if(peer_cnt==0)
    ePaper.print("no peers");

  ePaper.display(true);
} // ePaper_show_peer_list()

void MC_esp_now_peer_list() {
#if defined MULTICORE_DISPLAY
  MC_do_on_other_core(&ePaper_show_peer_list);
#else
  ePaper_show_peer_list();
#endif
}
#endif // USE_ESP_NOW

#if defined ePAPER_SHOW_CYCLE
const short cycle_bar_width=120;	// with Pixopixel font narrow chars |:.'!
const short cycle_bar_symbol_width=2;
const char* cycle_bar_marks=
  F("!              :        .     :         '    :  .           !           .  :    '         :     .        :              !");
const short cycle_bar_offset_y=-1;	// TEST same as 0?
//const short cycle_bar_offset_y=0;	// compromise, progress bar a bit small
//const short cycle_bar_offset_y=-2;	// cuts a bit too much from top line
uint8_t cycle_bar_row=4;
volatile short last_cycle_state_seen=0;


short new_cycle_position=-999;

//#define DEBUG_CYCLE_BAR_MULTICORE
void ePaper_update_progression_multicore() {
  // else  do update:
  set_used_font(&Picopixel);

  uint16_t x0, x, y, w, h;
  x0= (last_cycle_state_seen +1) * cycle_bar_symbol_width;		// first x position for partial update
  y= ((cycle_bar_row + 1) * used_font_p->yAdvance) + cycle_bar_offset_y; // stays constant
  w=2;		// do not eat the marks
  if(new_cycle_position < 8)	// startup
    w=8;	// let a group of updates be possible at startup, no marks yet to protect there
  h=2;

#if defined DEBUG_CYCLE_BAR_MULTICORE
  MENU.out(F("\ncycle bar  last ")); MENU.out(last_cycle_state_seen);
  MENU.out(F("\tnew ")); MENU.outln(new_cycle_position);

  MENU.out(F("PartialWindow (")); MENU.out(x0); MENU.out(','); MENU.out(y-1); MENU.out(',');
  MENU.out(w); MENU.out(','); MENU.out(h); MENU.outln(')');

  MENU.out(F("first set.Cursor(")); MENU.out((last_cycle_state_seen +1) * cycle_bar_symbol_width); MENU.out(',');
  MENU.out(y); MENU.outln(')');
#endif

  ePaper.setPartialWindow(x0, y-1, w, h);	// works quite well...
  ePaper.firstPage();
  do
  {
    while(new_cycle_position > last_cycle_state_seen) {
      x= ++last_cycle_state_seen * cycle_bar_symbol_width;
      ePaper.setCursor(x, y);
      ePaper.print("|");
    }
  }
  while (ePaper.nextPage());

  ePaper.setFullWindow();	// reset
} // ePaper_update_progression_multicore()

void ePaper_update_progression(float float_position) {	// just a wrapper
  new_cycle_position = float_position * (float) cycle_bar_width;
  if(new_cycle_position <= last_cycle_state_seen)	// already ok, nothing to do
    return;

  if(new_cycle_position < 0 || new_cycle_position > (cycle_bar_width + 1 /*shown range is cycle +1*/))
    return;						// OUT OF RANGE, error ignored

  MC_do_on_other_core(&ePaper_update_progression_multicore);
} // ePaper_update_progression(float)  wrapper

#endif // ePAPER_SHOW_CYCLE


volatile bool ePaper_is_updating=false;	// wait until ePaper_musicBox_parameters() has finished

#if ! defined ePAPER_SMALL_213		// BIGGER ePaper size, i.e. 2.9" size ePaper

void ePaper_musicBox_parameters() {	// BIGGER ePaper size, i.e. 2.9" size ePaper
#if defined  DEBUG_ePAPER
  MENU.outln(F("DEBUG_ePAPER\tePaper_musicBox_parameters() bigger"));
#endif

  ePaper.setFullWindow();
  ePaper.fillScreen(GxEPD_WHITE);
  ePaper.setTextColor(GxEPD_BLACK);

  char txt[LIN_BUF_MAX];
  char* format_s = F("%s");
  char* fmt_1st_row = F("%c %i  |%s|");		// fmt_1st_row, run_state_symbol(), musicBoxConf.preset, my_IDENTITY.preName
  //char* fmt_1st_row = F("%c |%s| P%i");	// fmt_1st_row, run_state_symbol(), musicBoxConf.preset, my_IDENTITY.preName
  char* fmt_2nd_row = F("%s  %i/%i %s");	// fmt_2nd_row, selected_name(SCALES),
						//	musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, metric_mnemonic
  // char* fmt_sync = F("S=%i");		// fmt_sync, musicBoxConf.sync
  char* fmt_synstack = F(" |%i");		// fmt_synstack, musicBoxConf.stack_sync_slices
  char* fmt_basepulse = F(" p[%i]");		// fmt_basepulse, musicBoxConf.base_pulse

  ePaper.firstPage();
  do
  {
    ePaper.fillScreen(GxEPD_WHITE);
    set_used_font(big_font_p);
    ePaper.setCursor(0, used_font_yAdvance - 8);	// better compromise as ePaper_setCursor_0_0();

    extern char run_state_symbol();
    snprintf(txt, font_linlen+1, fmt_1st_row, run_state_symbol(), musicBoxConf.preset, my_IDENTITY.preName);
    ePaper.print(txt);
    // monochrome_show_subcycle_octave();

    //ePaper.println();	// before???	bigger ssstep
    set_used_font(medium_font_p);
    ePaper.println(); // or after???  smaller step

    extern char* metric_mnemonic;
    snprintf(txt, LIN_BUF_MAX, fmt_2nd_row,
	     selected_name(SCALES), musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, metric_mnemonic);
    ePaper.println(txt);

#if defined ICODE_INSTEAD_OF_JIFFLES
    snprintf(txt, LIN_BUF_MAX, format_s, selected_name(iCODEs));
#else
    snprintf(txt, LIN_BUF_MAX, format_s, selected_name(JIFFLES));
#endif
    ePaper.println(txt);

    snprintf(txt, font_linlen+1, fmt_sync, musicBoxConf.sync);
    ePaper.print(txt);
    if(musicBoxConf.stack_sync_slices) {	// stack_sync_slices?
      snprintf(txt, font_linlen+1, fmt_synstack, musicBoxConf.stack_sync_slices);
      ePaper.print(txt);
      if(musicBoxConf.base_pulse != ILLEGAL16) {
	snprintf(txt, font_linlen+1, fmt_basepulse, musicBoxConf.base_pulse);
	ePaper.print(txt);
      }
    }
    ePaper.println();

    ePaper.print(musicBoxConf.name);
  }
  while (ePaper.nextPage());

  ePaper_is_updating = false;
} // ePaper_musicBox_parameters(), bigger

#else	// ePAPER_SMALL_213	SMALL ePaper size, i.e. LilyGo 2.13" boards

void ePaper_musicBox_parameters() {	// ePAPER_SMALL_213	SMALL ePaper size, i.e. LilyGo 2.13" boards
#if defined  DEBUG_ePAPER
  MENU.outln(F("DEBUG_ePAPER\tePaper_musicBox_parameters() small"));
#endif

  ePaper.setFullWindow();
  ePaper.fillScreen(GxEPD_WHITE);
  ePaper.setTextColor(GxEPD_BLACK);

  char txt[LIN_BUF_MAX];	// TODO: malloc
  char* format_s = F("%s");
  char* fmt_1st_row = F("%c %i  |%s|");		// fmt_1st_row, run_state_symbol(), musicBoxConf.preset, my_IDENTITY.preName
  //char* fmt_1st_row = F("%c |%s| P%i");	// fmt_1st_row, run_state_symbol(), musicBoxConf.preset, my_IDENTITY.preName
  char* fmt_key_scale_sync = F("  %s  %s  S=%i");// fmt_key_scale_sync, metric_mnemonic, selected_name(SCALES), sync	// with indent

  //char* fmt_sync = F("S=%i");			// fmt_sync, musicBoxConf.sync
  char* fmt_synstack = F("  |%i");		// fmt_synstack, musicBoxConf.stack_sync_slices
  char* fmt_basepulse = F("p[%i]");		// fmt_basepulse, musicBoxConf.base_pulse

  ePaper.firstPage();
  do
  {
    ePaper.fillScreen(GxEPD_WHITE);
    set_used_font(big_font_p);
    ePaper_setCursor_0_0();

    extern char run_state_symbol();
    snprintf(txt, font_linlen+1, fmt_1st_row, run_state_symbol(), musicBoxConf.preset, my_IDENTITY.preName);
    ePaper.print(txt);
    // monochrome_show_subcycle_octave();
    ePaper.println();

#if defined ePAPER_SHOW_CYCLE
    set_used_font(&Picopixel);
    ePaper.setCursor(0, ((cycle_bar_row+1) * used_font_p->yAdvance) + cycle_bar_offset_y);
    ePaper.print(cycle_bar_marks);
#endif

    if(musicBoxConf.name) {
      int len = strlen(musicBoxConf.name);
      bool bigFont_name= len < 19;	// TODO: test&trimm
      if(bigFont_name) {
	if(len < 18) {			// try to center short preset names
	  for(int i=0; i < ((17 - len)); i++)
	    ePaper.print(' ');
	}
	set_used_font(big_font_p);	// now BIG font

      } else {				// normal size
	set_used_font(medium_font_p);
      }
      ePaper.print(musicBoxConf.name);	// preset NAME

#if defined USE_MANY_FONTS
      ePaper.setFont(&FreeSans9pt7b);
#else
      set_used_font(medium_font_p);
#endif
      ePaper.println();		// empty line
      if(bigFont_name) {  		// if name (above) was big font
	set_used_font(tiny_font_p);	// just to have a smaller lineskip
	ePaper.println();
#if defined USE_MANY_FONTS
	ePaper.setFont(&FreeSans9pt7b);	// reset
#else
	set_used_font(medium_font_p);	// reset
#endif
      } else	// *not* big font
	if(len < 30) {
	  ePaper.println();	//    empty line (after shortish medium sized line)
	}
    } else { // (musicBoxConf.name == NULL)
      ePaper.println();		//    empty line (after missing preset name)
#if defined USE_MANY_FONTS
      ePaper.setFont(&FreeSans9pt7b);
#else
      set_used_font(medium_font_p);
#endif
    } // musicBoxConf.name  exists or not?

    extern char* metric_mnemonic;
    snprintf(txt, LIN_BUF_MAX, fmt_key_scale_sync, metric_mnemonic, selected_name(SCALES), musicBoxConf.sync);
    ePaper.println(txt);

#if defined ICODE_INSTEAD_OF_JIFFLES
    snprintf(txt, LIN_BUF_MAX, format_s, selected_name(iCODEs));
#else
    snprintf(txt, LIN_BUF_MAX, format_s, selected_name(JIFFLES));
#endif
    ePaper.print("  ");	// indent
    ePaper.print(txt);

//	    snprintf(txt, font_linlen+1, fmt_sync, musicBoxConf.sync);
//	    ePaper.print(txt);
    if(musicBoxConf.stack_sync_slices) {	// stack_sync_slices?
      snprintf(txt, font_linlen+1, fmt_synstack, musicBoxConf.stack_sync_slices);
      ePaper.print(txt);
      if(musicBoxConf.base_pulse != ILLEGAL16) {
	snprintf(txt, font_linlen+1, fmt_basepulse, musicBoxConf.base_pulse);
	ePaper.print(txt);
      }
    }
  }
  while (ePaper.nextPage());

  ePaper_is_updating = false;
} // ePaper_musicBox_parameters(), SMALL

#endif	// (big | SMALL) ePaper size

uint32_t /*error=*/ MC_show_musicBox_parameters() {
#if defined DEBUG_ePAPER
  MENU.outln(F("DEBUG_ePAPER\tMC_show_musicBox_parameters()"));
#endif
#if defined MULTICORE_DISPLAY
  return MC_do_on_other_core(&ePaper_musicBox_parameters, 2250);
#else
  ePaper_musicBox_parameters();
  return 0;
#endif
}


void  ePaper_show_program_version() {
#if defined  DEBUG_ePAPER
  MENU.outln(F("DEBUG_ePAPER\tePaper_show_program_version()"));
#endif

  ePaper.setFullWindow();
  ePaper.fillScreen(GxEPD_WHITE);
  ePaper.setTextColor(GxEPD_BLACK);

  char txt[LIN_BUF_MAX];
  char* format_s = F("%s");

  ePaper.firstPage();
  do
  {
    ePaper.fillScreen(GxEPD_WHITE);
#if defined USE_MANY_FONTS
    ePaper.setFont(&FreeSans9pt7b);
#else
    ePaper.setFont(&FreeSansBold9pt7b);
#endif
    ePaper_setCursor_0_0();

    snprintf(txt, LIN_BUF_MAX, format_s, F(STRINGIFY(PROGRAM_VERSION)));
    ePaper.print(txt);

#if defined USE_MANY_FONTS
    ePaper.setFont(&FreeSans12pt7b);
#else
    ePaper.setFont(&FreeSansBold12pt7b);
#endif
    ePaper.println();

    snprintf(txt, LIN_BUF_MAX, F("|%s|  (%i)"), my_IDENTITY.preName, my_IDENTITY.esp_now_time_slice);
    ePaper.println(txt);

#if defined USE_MANY_FONTS
    ePaper.setFont(&FreeSans9pt7b);
#else
    ePaper.setFont(&FreeSansBold9pt7b);
#endif

#if defined PROGRAM_SUB_VERSION
    snprintf(txt, LIN_BUF_MAX, format_s, F(STRINGIFY(PROGRAM_SUB_VERSION)));
    ePaper.print(txt);
    ePaper.print(' ');
    ePaper.print(' ');
#endif

#if defined USE_MPU6050_at_ADDR
    extern bool mpu6050_available;
    if(mpu6050_available && HARDWARE.mpu6050_addr)
      ePaper.print("  MPU");
#endif
    ePaper.println();

    ePaper.print("DAC ");
    ePaper.print(HARDWARE.DAC1_pin);
    ePaper.print(' ');
    ePaper.print(HARDWARE.DAC2_pin);

    if(HARDWARE.gpio_pins_cnt) {
      ePaper.print("  GPIO ");
      ePaper.print(HARDWARE.gpio_pins_cnt);	// TODO: show individual pins?
    }
    ePaper.println();

#if defined  USE_RGB_LED_STRIP
    if(HARDWARE.rgb_strings) {
      ePaper.print("RGB ");
      ePaper.print(HARDWARE.rgb_pin[0]);
      ePaper.print(',');
      ePaper.print(HARDWARE.rgb_pixel_cnt[0]);
      ePaper.print(' ');
      ePaper.print(' ');
    }
#endif

    if(HARDWARE.morse_touch_input_pin != ILLEGAL8) {
      ePaper.print('M');
      ePaper.print(HARDWARE.morse_touch_input_pin);
      ePaper.print(' ');
      ePaper.print(' ');
    }

    if(HARDWARE.morse_output_pin != ILLEGAL8) {
      ePaper.print('O');
      ePaper.print(HARDWARE.morse_output_pin);
      ePaper.print(' ');
      ePaper.print(' ');
    }

    char mnemonic;
#if defined MUSICBOX_TRIGGER_PIN
    extern bool musicBox_trigger_enabled;
    if(musicBox_trigger_enabled)
      mnemonic = 'T';
    else
      mnemonic = 't';
    ePaper.print(mnemonic);
    ePaper.print(',');
    ePaper.print(HARDWARE.musicbox_trigger_pin);
    ePaper.print(' ');
    ePaper.print(' ');
#endif

#if defined USE_BATTERY_LEVEL_CONTROL
    ePaper.print("V");
    ePaper.print(HARDWARE.battery_level_control_pin);	// TODO: show level
    ePaper.print(' ');
    ePaper.print(' ');
#endif

#if defined USE_MIDI
    ePaper.print("MIDI ");
    ePaper.print(HARDWARE.MIDI_in_pin);
    ePaper.print(' ');
    ePaper.print(HARDWARE.MIDI_out_pin);
    ePaper.print(' ');
#endif

    ePaper.println();
  }
  while (ePaper.nextPage());

  set_used_font(used_font_p);	// restore last used font
} // ePaper_show_program_version()

void inline MC_show_program_version() {
#if defined DEBUG_ePAPER
  MENU.outln(F("DEBUG_ePAPER\tMC_show_program_version()"));
#endif
#if defined MULTICORE_DISPLAY
  MC_do_on_other_core(&ePaper_show_program_version, 2048);
#else
  ePaper_show_program_version();
#endif
} // MC_show_program_version()


void  ePaper_show_tuning() {
#if defined  DEBUG_ePAPER
  MENU.outln(F("DEBUG_ePAPER\tePaper_show_tuning()"));	// DADA remove debugging code ################
#endif

  ePaper.setFullWindow();
  ePaper.fillScreen(GxEPD_WHITE);
  ePaper.setTextColor(GxEPD_BLACK);

  char txt[LIN_BUF_MAX];
  char* format_s = F("%s");

  ePaper.firstPage();
  do
  {
    ePaper.setFont(big_font_p);
    ePaper.fillScreen(GxEPD_WHITE);
    ePaper_setCursor_0_0();

    snprintf(txt, LIN_BUF_MAX, format_s, selected_name(SCALES));
    ePaper.println(txt);
    ePaper.println();

    extern char* metric_mnemonic;
    snprintf(txt, 12, F("%i/%i %s"), musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, metric_mnemonic);
    ePaper.println(txt);
  }
  while (ePaper.nextPage());

  set_used_font(used_font_p);	// restore last used font
} // ePaper_show_tuning()

void inline MC_show_tuning() {
#if defined DEBUG_ePAPER
  MENU.outln(F("DEBUG_ePAPER\tMC_show_tuning()"));
#endif
#if defined MULTICORE_DISPLAY
  MC_do_on_other_core(&ePaper_show_tuning);
#else
  ePaper_show_tuning();
#endif
}

extern char run_state_symbol();
void ePaper_put_run_state_symbol(char symbol=run_state_symbol()) {
  char text[] = {symbol, 0};
  MC_printBIG_at(0,0, text, -6);	// offset_y = -6
}
