/*
  monochrome_display.h

  see: https://github.com/olikraus/u8g2/wiki/u8x8reference
*/

// DADA	TODO: monochrome_conf_t

// TODO: void u8x8_SetPowerSave(u8x8_t *u8x8, uint8_t is_enable);

#ifndef MONOCHROME_DISPLAY_H

uint8_t monochrome_power_save=0;

bool monochrome_can_be_used() {	// monochrome output is appropriate?
  // TODO: monochrome_display_hardware	fix&use monochrome_display detection

  if(monochrome_power_save)
    return false;			// power save

  if(! musicbox_is_idle()) {		// musicBox is running
    if(! oled_feedback_while_playing)
      return false;			// && no feedback allowed, niet!
  }

  return true;
}

void monochrome_show_program_version() {	// monochrome oled display
  if(! monochrome_power_save) {
    u8x8.clear();
    u8x8.setCursor(0,1);
    u8x8.print(F(STRINGIFY(PROGRAM_VERSION)));

    u8x8.setCursor(0,3);
    if(my_ID.preName)
      u8x8.print(my_ID.preName);
#if defined PROGRAM_SUB_VERSION
    else
      u8x8.print(F(STRINGIFY(PROGRAM_SUB_VERSION)));
#endif

    u8x8.setCursor(0,5);	// option mnemonics

#if defined USE_BLUETOOTH_SERIAL_MENU
  #if defined BLUETOOTH_ENABLE_PIN
    pinMode(BLUETOOTH_ENABLE_PIN, INPUT);
    digitalRead(BLUETOOTH_ENABLE_PIN);		// i do not trust the very first reading...
    if(digitalRead(BLUETOOTH_ENABLE_PIN))
      u8x8.print('B');	// BLUETOOTH_ENABLE_PIN enabled?
    else
      u8x8.print('b');
  #else			// *no* BLUETOOTH_ENABLE_PIN
    u8x8.print('B');
  #endif

    u8x8.print(' ');
#else	// *no* USE_BLUETOOTH_SERIAL_MENU
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

#if defined BATTERY_LEVEL_CONTROL_PIN
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
}

void monochrome_show_subcycle_octave() {
  if(! monochrome_power_save) {
    if(selected_in(SCALES) != NULL) {
      u8x8.print(F("2^"));
      u8x8.print(CyclesConf.subcycle_octave);
    }
  }
}

void monochrome_show_musicBox_parameters() {	// ATTENTION: takes too long to be used while playing
  if(! monochrome_power_save) {
    uint8_t cols = u8x8.getCols();
    uint8_t rows = u8x8.getRows();
    int row=0;

    if(musicBoxConf.preset) {
      u8x8.clearLine(row);
      u8x8.setCursor(0,row++);
      u8x8.setInverseFont(1);
      u8x8.print(F("P "));
      u8x8.print(musicBoxConf.preset);
      u8x8.print(' ');
      u8x8.setInverseFont(0);
      u8x8.print(' ');
      monochrome_show_subcycle_octave();
    } else
      u8x8.clearLine(row++);

    row++;
    u8x8.clearLine(row);
    u8x8.drawString(0, row++, array2name(SCALES, selected_in(SCALES)));

    // u8x8.clearLine(row++);	// empty line

    u8x8.clearLine(row);
    u8x8.drawString(0, row++, array2name(JIFFLES, selected_in(JIFFLES)));

    u8x8.clearLine(row++);	// empty line

    u8x8.clearLine(row);
    u8x8.setCursor(0,row++);
    u8x8.print('S');		// sync
    u8x8.print(musicBoxConf.sync);
    if(musicBoxConf.stack_sync_slices) {	// stack_sync_slices
      u8x8.print('|');
      u8x8.print(musicBoxConf.stack_sync_slices);
    }
    u8x8.print(F("  "));
    u8x8.print(musicBoxConf.pitch.multiplier);	// pitch
    u8x8.print('/');
    u8x8.print(musicBoxConf.pitch.divisor);

    if(musicBoxConf.stack_sync_slices && musicBoxConf.base_pulse != ILLEGAL) {
      u8x8.clearLine(row);	// stack_sync_slices or empty line
      u8x8.setCursor(0,row++);
      u8x8.print(F("p["));
      u8x8.print(musicBoxConf.base_pulse);
      u8x8.print(F("]|"));
      u8x8.print(musicBoxConf.stack_sync_slices);
    }

    u8x8.clearLine(row);	// name or subcycle
    u8x8.setCursor(0,row++);
    if(musicBoxConf.name && *musicBoxConf.name /*no empty string*/) {	// one line from name
      char * s = musicBoxConf.name;
      char c ;
      int col=0;
      /*
      // TESTCODE Umlaute		// TODO: fix or remove
      char line_buffer[rows];
      for(int r=0; r<rows; r++) {		// a letter for each row
      c = line_buffer[r] = s[r];
      if (c==0)				// string end
      break;
      }
      */
      u8x8.setInverseFont(1);
      /* // TESTCODE Umlaute		// TODO: fix or remove
      // U8X8::drawUTF8(uint8_t x, uint8_t y, const char *s)
      u8x8.drawUTF8(0, row, line_buffer);
      */
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
	unsigned long seconds = (((float) CyclesConf.used_subcycle.time / 1000000.0) + 0.5);	// TODO: factor out, build a string
	if (CyclesConf.used_subcycle.overflow)
	  seconds += CyclesConf.used_subcycle.overflow * 4295;	// FIXME: (float) and round *after* overflow correction

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
}

void monochrome_show_line(uint8_t row, char * s) {
  if(! monochrome_power_save) {
    uint8_t cols = u8x8.getCols();
    char full_line[cols+1] = {0};
    char c;

    int i;
    for (i=0; i<cols; i++) {
      if(c = s[i])
	full_line[i] = c;
      else	// end of string reached
	break;
    }
    while(i< cols)	// fill line with spaces (avoid u8x8.clearLine() as it takes too long)
      full_line[i++] = ' ';
    full_line[cols] = '\0';

    u8x8.setCursor(0,row);
    u8x8.print(full_line);
  }
}

void monochrome_display_message(char* message) {
  u8x8.setCursor(u8x8.getCols() - strlen(message),  u8x8.getRows() -1);	// last line > right is message spot
  u8x8.print(message);
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

#define MONOCHROME_DISPLAY_H
#endif
