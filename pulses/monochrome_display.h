/*
  monochrome_display.h

  see: https://github.com/olikraus/u8g2/wiki/u8x8reference
*/

// DADA	TODO: monochrome_conf_t

// TODO: void u8x8_SetPowerSave(u8x8_t *u8x8, uint8_t is_enable);

#ifndef MONOCHROME_DISPLAY_H

#include <U8x8lib.h>

uint8_t monochrome_power_save=0;

// avoid sound glitches when using OLED:
bool oled_feedback_while_playing = true;	// do *not* give morse feedback while playing

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
  if(monochrome_can_be_used()) {
    u8x8.clear();
    u8x8.setCursor(0,1);
    u8x8.print(F(STRINGIFY(PROGRAM_VERSION)));

    u8x8.setCursor(0,3);
    if(my_IDENTITY.preName != "")
      u8x8.print(my_IDENTITY.preName);
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

char scale_symbol_char() {	// 1 char SCALE index as hex and beyond for monochrome display, morse out(?)
  char s = (char) pointer2index(SCALES, selected_in(SCALES));		// start with index in SCALES
  s--;	// scale 0 is in SCALES[1]
  if(s < 10)
    s += '0';	// chiffres 0...9
  else {
    s -= 10;
    s += 'A';	// hex and beyond ;)
  }
  return s;
} // scale_symbol_char()


extern bool musicbox_is_awake();
extern bool musicbox_is_idle();
extern bool musicbox_is_ending();

char run_state_symbol() {
  if(musicbox_is_awake())
    return '!';
  if(musicbox_is_idle())
    return '.';
  if(musicbox_is_ending())
    return 'e';
  // 'p' is used for pause

  return '?';
}

void monochrome_multiline_string(uint8_t row, char* s) {	// multiline string from row to bottom (max)
  if(musicBoxConf.name && *musicBoxConf.name /*no empty string*/) {	// one line from name
    uint8_t cols = u8x8.getCols();
    uint8_t rows = u8x8.getRows();

    char c ;
    int col;
    while(*s && row<rows) {
      col=0;
      u8x8.clearLine(row);
      u8x8.setCursor(0,row++);
      while(c = *s++) {
	u8x8.print(c);
	if ((++col % cols) == 0)
	  break;
      }
      col %= cols;

      if(c==0) {
	while(col++ < cols)  u8x8.print(' ');	// fill line witch spaces
	if(row < rows)
	  u8x8.clearLine(row);			// clear one more line
	break;
      }
    } // all chars
  } // there *is* string content
} // monochrome multiline string()

void monochrome_show_musicBox_parameters() {	// ATTENTION: takes too long to be used while playing
  if(monochrome_can_be_used()) {
    uint8_t cols = u8x8.getCols();
    uint8_t rows = u8x8.getRows();
    int row=0;	// first 2 lines empty for 2x2 output (i.e. MORSE)
    u8x8.clearLine(row++);	// clear empty line
    u8x8.clearLine(row++);	// clear empty line

    u8x8.clearLine(row);
    if(musicBoxConf.preset) {
      u8x8.setCursor(0,row);
      u8x8.print(run_state_symbol());
      u8x8.print(' ');

      u8x8.setInverseFont(1);
      u8x8.print(F("P "));
      u8x8.print(musicBoxConf.preset);
      u8x8.print(' ');
      u8x8.setInverseFont(0);
      u8x8.print(' ');
      monochrome_show_subcycle_octave();

      char run_state[2];
      run_state[0] = run_state_symbol();
      run_state[1] = 0;
      u8x8.draw2x2String(0, 0, run_state);      // *BIG* run state

      // *BIG* PRESET number on top		// poor old eyes little helper ;)
      char preset[10] = {0};
      itoa(musicBoxConf.preset, preset, 10);
      u8x8.draw2x2String(2, 0, preset);
    }
    row++;

    u8x8.clearLine(row);
    u8x8.drawString(0, row++, array2name(SCALES, selected_in(SCALES)));

    u8x8.clearLine(row);
    u8x8.drawString(0, row++, array2name(JIFFLES, selected_in(JIFFLES)));

    u8x8.clearLine(row);
    u8x8.setCursor(0,row++);
    u8x8.print(F("S="));	// sync
    u8x8.print(musicBoxConf.sync);

    if(musicBoxConf.stack_sync_slices) {	// stack_sync_slices?
      u8x8.print(F(" |"));
      u8x8.print(musicBoxConf.stack_sync_slices);
      if(musicBoxConf.base_pulse != ILLEGAL16) {
	u8x8.print(F(" p["));
	u8x8.print(musicBoxConf.base_pulse);
	u8x8.print(']');
      }

    } else {	// no stack_sync_slices, so there is space for other info:	// (can be removed)
      u8x8.print(F("  "));
      u8x8.print(musicBoxConf.pitch.multiplier);	// pitch
      u8x8.print('/');
      u8x8.print(musicBoxConf.pitch.divisor);
    }

    if(musicBoxConf.name && *musicBoxConf.name /*no empty string*/) {	// one line from name
      u8x8.setInverseFont(1);
      monochrome_multiline_string(row, musicBoxConf.name);
      u8x8.setInverseFont(0);

    } else { // if there is no name
      u8x8.clearLine(row);
      u8x8.setCursor(0,row);

      if(selected_in(SCALES) != NULL) {
	monochrome_show_subcycle_octave();
	u8x8.print(' ');
#if defined PULSES_USE_DOUBLE_TIMES
	unsigned long seconds = ((CyclesConf.used_subcycle / 1000000.0) + 0.5);	// TODO: factor out, build a string
#else // old int overflow style
	unsigned long seconds = (((float) CyclesConf.used_subcycle.time / 1000000.0) + 0.5);	// TODO: factor out, build a string
	if (CyclesConf.used_subcycle.overflow)
	  seconds += CyclesConf.used_subcycle.overflow * 4295;	// FIXME: (float) and round *after* overflow correction
#endif

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
    } // name or no name

    if(musicBoxConf.chromatic_pitch) {
      extern char* metric_mnemonic;
      u8x8.draw2x2String(10, 0, metric_mnemonic);
      char s[] = {0, 0};
      extern char scale_symbol_char();
      s[0] = scale_symbol_char();
      u8x8.draw2x2String(14, 0, s);
    }
  } //if(monochrome_can_be_used())
} // monochrome_show_musicBox_parameters()


void monochrome_show_line(uint8_t row, char * s) {
  if(monochrome_can_be_used()) {
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


void oled_ui_display() {
  MENU.outln(F("'O<x>' OLED\t'OA'='OE'=on  'OT'=off  'OP'=whilePlaying"));
}

bool OLED_UI() {	// follows 'O'		'OE'	'OT'	'OP'
  switch (MENU.peek()) {
  case 'E':		// 'OA' == 'OE'	all on   (morse friendly)
  case 'A':		// 'OA' == 'OE'	all on   analogue morse COMMAND 'OA'
    MENU.drop_input_token();
    monochrome_power_save = 0;
    u8x8.setPowerSave(monochrome_power_save);
    oled_feedback_while_playing = true;
    MENU.outln(F("OLED on"));
    break;

  case 'T':		// 'OT'	OLED off (morse friendly)
    MENU.drop_input_token();
    monochrome_power_save = 1;
    u8x8.setPowerSave(monochrome_power_save);
    MENU.outln(F("OLED off"));
    break;

  case 'P':		// 'OP' toggle oled_feedback_while_playing
    MENU.drop_input_token();
    oled_feedback_while_playing = ! oled_feedback_while_playing;
    MENU.out(F("OLED while playing "));
    MENU.out_ON_off(oled_feedback_while_playing);
    MENU.ln();
    break;

  default:
    return false;
  }

  return true;
} // OLED_UI()


void monochrome_print2x2(uint8_t col, uint8_t row, char* str) {	// for short 2x2 strings
  if(monochrome_can_be_used()) {
    int max=(u8x8.getCols()/2);	// limit length
    char truncated[max+1]={0};

    char c;
    for(int i=0; i<max; i++) {
      truncated[i] = str[i];
      if(truncated[i] == 0)
	break;
    }
    u8x8.draw2x2String(col, row, truncated);
  }
} // monochrome_print2x2()

void monochrome_println2x2(uint8_t row, char* str) {	// 2x2 lines
  if(monochrome_can_be_used()) {
    int max=(u8x8.getCols()/2);	// limit length
    char truncated[max+1];
    for(int i=0; i<max; i++)
      truncated[i] = ' ';		// space filled to clear line
    truncated[max] = 0;		// /0 terminated

    char c;
    for(int i=0; i<=max; i++) {
      if(c = str[i])
	truncated[i] = c;
      else
	break;
    }

    u8x8.draw2x2String(0, row, truncated);
  }
} // monochrome_println2x2()

void monochrome_println_big_or_multiline(int row, char* str) {
  /*
    print one line on monochrome
    use 2x2 size if it fits
    else use basic size
    even multinline, if needed
  */
  int len = strlen(str);
  if(len <= (u8x8.getCols() / 2))	// fits in one 2x2 line?
    monochrome_println2x2(row, str);
  else				// too long for 2x2
    monochrome_multiline_string(row, str);
} // monochrome_println_big_or_multiline()


void monochrome_setInverseFont(uint8_t inverse) {
  if(monochrome_can_be_used())
    u8x8.setInverseFont(inverse);
}

void monochrome_setPowerSave(uint8_t value) {
  u8x8.setPowerSave(value);	// try to set it anyway
}

void monochrome_setCursor(uint8_t col, uint8_t row) {
  if(monochrome_can_be_used())
    u8x8.setCursor(col, row);
}

void monochrome_print(char* str) {
  if(monochrome_can_be_used())
    u8x8.print(str);
}

void monochrome_print_f(float f) {
  if(monochrome_can_be_used())
    u8x8.print(f);
}

void monochrome_print_f(float f, int chiffres) {
  if(monochrome_can_be_used())
    u8x8.print(f, chiffres);
}

uint8_t monochrome_getCols() {
  return u8x8.getCols();
}

//	void monochrome_setup() {
//	  MENU.out(F("monochrome_setup() "));
//	  switch(HARDWARE.monochrome_type) {
//	  case monochrome_type_heltec:
//	    U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16); // BOARD_HELTEC_OLED
//	    MENU.out(F("heltec"));
//	    break;
//	  case monochrome_type_LiPO:
//	    U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 4, /* data=*/ 5, /* reset=*/ 16);  // BOARD_OLED_LIPO	TODO: move to setup()
//	    MENU.out(F("Lipo"));
//	    break;
//	  case monochrome_type_off:
//	    MENU.error_ln(F("monochrome is off in nvs?"));
//	#if defined BOARD_OLED_LIPO		// try to take from pp macros
//	    U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 4, /* data=*/ 5, /* reset=*/ 16);  // BOARD_OLED_LIPO	TODO: move to setup()
//	#elif defined BOARD_HELTEC_OLED		// heltec
//	    U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16); // BOARD_HELTEC_OLED
//	#endif
//	    break;
//	  default:
//	    MENU.error(F("unknown monochrome_type"));
//	  }
//	  MENU.ln();
//	} // monochrome_setup()

#define MONOCHROME_DISPLAY_H
#endif
