/*
  ePaper_pulses.h
*/


#define LIN_BUF_MAX	42	// temporal local line buffers only


void inline MC_display_message(char* text) {
#if defined DEBUG_ePAPER
  MENU.out(F("DEBUG_ePAPER\tMC_display_message()\t"));
#endif
  MENU.outln(text);
  set_used_font(medium_font_p);
  MC_print_1line_at(3, text, /*offset*/ 27);	// bottom line
} // MC_display_message()



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
  ePaper.setCursor(0,0);
  ePaper.println();

  uint8_t row=0;
  bool is_send_to_peer;
  for(int i=0; i<ESP_NOW_MAX_TOTAL_PEER_NUM; i++) {
    if(mac_is_non_zero(esp_now_pulses_known_peers[i].mac_addr)) {
      ePaper.print(i+1);
      if(is_send_to_peer = (! mac_cmp(esp_now_send2_mac_p, esp_now_pulses_known_peers[i].mac_addr)))	// send to peer?
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


void ePaper_musicBox_parameters() {
#if defined  DEBUG_ePAPER
  MENU.outln(F("DEBUG_ePAPER\tePaper_musicBox_parameters()"));
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
  char* fmt_sync = F("S=%i");			// fmt_sync, musicBoxConf.sync
  char* fmt_synstack = F(" |%i");		// fmt_synstack, musicBoxConf.stack_sync_slices
  char* fmt_basepulse = F(" p[%i]");		// fmt_basepulse, musicBoxConf.base_pulse

  ePaper.firstPage();
  do
  {
    ePaper.fillScreen(GxEPD_WHITE);
    set_used_font(big_font_p);
    ePaper.setCursor(0, 0);
    ePaper.println();

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
} // ePaper_musicBox_parameters()


void inline MC_show_musicBox_parameters() {
#if defined DEBUG_ePAPER
  MENU.outln(F("DEBUG_ePAPER\tMC_show_musicBox_parameters()"));
#endif
  MC_do_on_other_core(&ePaper_musicBox_parameters);
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
    ePaper.setCursor(0, 0);
    ePaper.println();

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

#if defined USE_MPU6050
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
  MC_do_on_other_core(&ePaper_show_program_version);
}


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
    ePaper.setCursor(0, 0);
    ePaper.println();

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
  MC_do_on_other_core(&ePaper_show_tuning);
}


void try_ePaper_fix() {	// maybe OBSOLETE?	let's hope ;)
#if defined DEBUG_ePAPER
  MENU.outln(F("DEBUG_ePAPER\ttry_ePaper_fix()"));
#endif

  MENU.ln();
  MENU.error_ln(F("\tTODO: IMPLEMENT\ttry_ePaper_fix()\tOBOLETE?"));
  //~GxEPD2_BW<GxEPD2_213_B73, GxEPD2_213_B73::HEIGHT>();
  //~GxEPD2_BW();	// destructor ?

  /*
  MENU.out(F("before:\t"));
  MENU.print_free_RAM();
  MENU.ln();
  */
  setup_ePaper_GxEPD2();
  /*
  MENU.out(F("after:\t"));
  MENU.print_free_RAM();
  MENU.ln();
  */

  delay(1000);	// TODO: ?

  //  ePaper_show_program_version();	// just as a test
  extern void MC_show_musicBox_parameters();
  delay(2000); MC_show_musicBox_parameters();	// just as a test
  delay(1500);	// TODO: ?
} // try_ePaper_fix()

void inline try_monochrome_fix() {
  try_ePaper_fix();
}
