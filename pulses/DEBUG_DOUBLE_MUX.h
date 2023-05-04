/*
  DEBUG_DOUBLE_MUX.h
*/

void DEBUG_DOUBLE_MUX_one_shot_tests() {	// aborts after that
  set_used_font(medium_font_p);

  monochrome_clear();		// or: MC_clear_display()?
  MENU.outln("\nePaper_print_at()");
  extern void ePaper_print_at(uint16_t col, uint16_t row, const char* text, int16_t offset_y=0);
  ePaper_print_at(0,0, "ePaper_print_at");
  ePaper.display(true);
  delay(6000);

  monochrome_clear();		// or: MC_clear_display()?
  MENU.outln("\nmulticore_ePaper_print_at()");
  // extern void multicore_ePaper_print_at(uint16_t col, uint16_t row, const char* text, int16_t offset_y=0);
  extern void multicore_ePaper_print_at(int16_t col, int16_t row, const char* text, int16_t offset_y=0);
  multicore_ePaper_print_at(0,0, "multicore_ePaper@");
  //  ePaper.display(true);
  delay(6000);

//  monochrome_clear();		// or: MC_clear_display()?
//  MENU.outln("\nMC_ePaper_print_at()");
//  extern void MC_ePaper_print_at(uint16_t col, uint16_t row, const char* text, int16_t offset_y=0);
//  MC_ePaper_print_at(0,0, "MC_ePaper_print_at");

  monochrome_clear();		// or: MC_clear_display()?
  MENU.outln("\nMC_printBIG_at()");
  extern void MC_printBIG_at(int16_t col, int16_t row, const char* text, int16_t offset_y=0);
  MC_printBIG_at(0, 0, "MC_printBIG_at");
  //  ePaper.display(true);
  delay(6000);

  monochrome_clear();		// or: MC_clear_display()?
  MENU.outln(F("\n MuBo parameters"));
  monochrome_clear();		// or: MC_clear_display()?
  musicBox_short_info();
  MC_show_musicBox_parameters();

  MENU.outln(F("\nDEBUG_DOUBLE_MUX_one_shot_tests()  done"));
  while(true)
    {
    }
} // DEBUG_DOUBLE_MUX_one_shot_tests()
