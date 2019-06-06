/*
  monochrome_display.h
*/

void monochrome_show_program_version() {	// monochrome oled display
    u8x8.clear();
    u8x8.setCursor(0,1);
    u8x8.print(F(STRINGIFY(PROGRAM_VERSION)));

    u8x8.setCursor(0,3);
    if(preName)
      u8x8.print(preName);
    else
#if defined PROGRAM_SUB_VERSION
      u8x8.print(F(STRINGIFY(PROGRAM_SUB_VERSION)));
#else
      ;
#endif

    u8x8.setCursor(0,5);	// option mnemonics

    #if defined USE_BLUETOOTH_SERIAL_MENU
      #if defined BLUETOOTH_ENABLE_PIN
	if(bluetooth_switch_info())
	  u8x8.print('B');
	else
	  u8x8.print('b');
      #else
	  u8x8.print('B');
      #endif
      u8x8.print(' ');
    #else
      u8x8.print(' ');
      u8x8.print(' ');
    #endif	// USE_BLUETOOTH_SERIAL_MENU

    #if defined MUSICBOX_TRIGGER_PIN
      if(musicBox_trigger_enabled)
	u8x8.print('I');
      else
	u8x8.print('i');
    #else
      u8x8.print(' ');
    #endif
    u8x8.print(' ');

    #if defined USE_RTC_MODULE
      if(rtc_module_is_usable)
	u8x8.print('R');
      else
	u8x8.print('r');
     #else
      u8x8.print(' ');
    #endif
    u8x8.print(' ');

    #if defined USE_BATTERY_CONTROL
      u8x8.print(F("V "));
    #endif

    #if ! defined NO_GPIO_PINS
      #if GPIO_PINS > 0
	u8x8.print('G');
	u8x8.print((int) GPIO_PINS);
      #endif
    #endif

#if defined OLED_HALT_PIN0
    // do *not* change pin mode (0) here...
    while(digitalRead(0) == LOW) { delay(1000); MENU.out('°'); }  // ATTENTION: dangerous *not* tested with GPIO00 as click or such...
#endif
}

void monochrome_show_subcycle_octave() {
  if(selected_in(SCALES) != NULL) {
    u8x8.print(F("2^"));
    u8x8.print(subcycle_octave);
  }
}

void monochrome_show_musicBox_parameters() {	// ATTENTION: takes too long to be used while playing
  uint8_t cols = u8x8.getCols();
  uint8_t rows = u8x8.getRows();
  int row=0;

  if(preset) {
    u8x8.clearLine(row);
    u8x8.setCursor(0,row++);
    u8x8.setInverseFont(1);
    u8x8.print(F("PRESET "));
    u8x8.print(preset);
    u8x8.print(' ');
    u8x8.setInverseFont(0);
    u8x8.print(' ');
    monochrome_show_subcycle_octave();
  } else
    u8x8.clearLine(row++);

  u8x8.clearLine(row);
  u8x8.drawString(0, row++, array2name(SCALES, selected_in(SCALES)));

  u8x8.clearLine(row++);	// empty line

  u8x8.clearLine(row);
  u8x8.drawString(0, row++, array2name(JIFFLES, selected_in(JIFFLES)));

  u8x8.clearLine(row++);	// empty line

  u8x8.clearLine(row);
  u8x8.setCursor(0,row++);
  u8x8.print('S');		// sync
  u8x8.print(sync);
  if(stack_sync_slices) {	// stack_sync_slices
    u8x8.print('|');
    u8x8.print(stack_sync_slices);
  }
  u8x8.print(F("  "));
  u8x8.print(pitch.multiplier);	// pitch
  u8x8.print('/');
  u8x8.print(pitch.divisor);

  if(stack_sync_slices && base_pulse != ILLEGAL) {
    u8x8.clearLine(row);	// stack_sync_slices or empty line
    u8x8.setCursor(0,row++);
    u8x8.print(F("p["));
    u8x8.print(base_pulse);
    u8x8.print(F("]|"));
    u8x8.print(stack_sync_slices);
  }

  u8x8.clearLine(row);	// name or subcycle
  u8x8.setCursor(0,row++);
  if(name && *name /*no empty string*/) {	// one line from name
    char * s = name;
    char c ;
    int col=0;
    u8x8.setInverseFont(1);
    while(c = *s++) {
      u8x8.print(c);
      if ((++col % cols) == 0)
	break;
    }
    col %= cols;
    if(c==0)
      while(col++ < cols)  u8x8.print(' ');	// fill line witch spaces

    while ( c && row < rows) {	// if name is longer
      u8x8.clearLine(row);	// name or subcycle
      u8x8.setCursor(0,row++);
      while(c = *s++) {
	u8x8.print(c);
	if ((++col % cols) == 0)
	  break;
      }
      col %= cols;
      if(c==0)
	while(col++ < cols)  u8x8.print(' ');	// fill line witch spaces
    }
    u8x8.setInverseFont(0);
  } else {
    if(selected_in(SCALES) != NULL) {
      monochrome_show_subcycle_octave();
      u8x8.print(' ');
      unsigned long seconds = (((float) used_subcycle.time / 1000000.0) + 0.5);	// TODO: factor out, build a string
      if (used_subcycle.overflow)
	seconds += used_subcycle.overflow * 4295;	// FIXME: (float) and round *after* overflow correction

      unsigned int days = seconds / 86400;
      seconds %= 86400;
      unsigned int hours = seconds / 3600;
      seconds %= 3600;
      unsigned int minutes = seconds / 60;
      seconds %= 60;
      if(days) {
	u8x8.print(days);
	u8x8.print(F("d "));
      }
      if(hours || days) {
	u8x8.print(hours);
	u8x8.print(F("h "));
      }
      if(minutes || hours || days) {
	u8x8.print(minutes);
	u8x8.print(F("\' "));
      }
      u8x8.print(seconds);
      u8x8.print(F("\" "));
    } else									// TODO: factor out
      u8x8.print('-');
  }
}

/*
void display_string(char * s) {	// ATTENTION: takes too long to be used while playing
  uint8_t cols = u8x8.getCols();
  uint8_t rows = u8x8.getRows();
  char c;

  //u8x8.clear();
  for(uint8_t row=0; row<rows; row++) {
    u8x8.setCursor(0,row);
    u8x8.clearLine(row);
    for(int col=0; col<cols; col++) {
      if(c = *s++)
	u8x8.print(c);
      else		// string end reached
	return;		// rest of line is clear
    }
  }
}
*/
