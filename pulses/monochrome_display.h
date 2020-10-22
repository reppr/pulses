/*
  monochrome_display.h

  see: https://github.com/olikraus/u8g2/wiki/u8x8reference
*/
#ifndef MONOCHROME_DISPLAY_H


//#define MC_DELAY_MS	10	// delay MC_mux lock release		// TODO: test&trimm	maybe obsolete?
#define MC_DELAY_MS	0	// *NO* delay MC_mux lock release?	// TODO: maybe OBSOLETE:?

#if defined MULTICORE_DISPLAY
  #include "multicore_display_common.h"
#endif

#include <U8x8lib.h>

uint8_t monochrome_power_save=0;

// avoid sound glitches when using OLED:
bool oled_feedback_while_playing = true;	// do *not* give morse feedback while playing

bool monochrome_can_be_used() {	// monochrome output is appropriate?
  // TODO: has_display_hardware	fix&use monochrome_display detection

  if(monochrome_power_save)
    return false;			// power save

  if(! musicbox_is_idle()) {		// musicBox is running
    if(! oled_feedback_while_playing)
      return false;			// && no feedback allowed, niet!
  }

  return true;
} // monochrome_can_be_used()


void monochrome_show_program_version() {	// monochrome oled display
  if(monochrome_can_be_used()) {
    (*u8x8_p).clear();
    (*u8x8_p).setCursor(0,1);
    (*u8x8_p).print(F(STRINGIFY(PROGRAM_VERSION)));

    (*u8x8_p).setCursor(0,3);
    if(my_IDENTITY.preName != "")
      (*u8x8_p).print(my_IDENTITY.preName);
#if defined PROGRAM_SUB_VERSION
    else
      (*u8x8_p).print(F(STRINGIFY(PROGRAM_SUB_VERSION)));
#endif

    (*u8x8_p).setCursor(0,5);	// option mnemonics

#if defined USE_BLUETOOTH_SERIAL_MENU
  #if defined BLUETOOTH_ENABLE_PIN
    pinMode(HARDWARE.bluetooth_enable_pin, INPUT);
    digitalRead(HARDWARE.bluetooth_enable_pin);		// i do not trust the very first reading...
    if((HARDWARE.bluetooth_enable_pin==ILLEGAL8) || digitalRead(HARDWARE.bluetooth_enable_pin))
      (*u8x8_p).print('B');	// HARDWARE.bluetooth_enable_pin enabled?
    else
      (*u8x8_p).print('b');
  #else			// *no* BLUETOOTH_ENABLE_PIN
    (*u8x8_p).print('B');
  #endif

    (*u8x8_p).print(' ');
#else	// *no* USE_BLUETOOTH_SERIAL_MENU
    (*u8x8_p).print(' ');
    (*u8x8_p).print(' ');
#endif	// USE_BLUETOOTH_SERIAL_MENU

#if defined MUSICBOX_TRIGGER_PIN
    if(musicBox_trigger_enabled)
      (*u8x8_p).print('I');
    else
      (*u8x8_p).print('i');
#else
    (*u8x8_p).print(' ');
#endif
    (*u8x8_p).print(' ');

#if defined USE_RTC_MODULE
    if(rtc_module_is_usable)
      (*u8x8_p).print('R');
    else
      (*u8x8_p).print('r');
#else
    (*u8x8_p).print(' ');
#endif
    (*u8x8_p).print(' ');

#if defined BATTERY_LEVEL_CONTROL_PIN
    (*u8x8_p).print(F("V "));
#endif

#if ! defined NO_GPIO_PINS
#if GPIO_PINS > 0
    (*u8x8_p).print('G');
    (*u8x8_p).print((int) GPIO_PINS);
#endif
#endif
  } // if(monochrome_can_be_used())
} // monochrome_show_program_version()


#if defined MULTICORE_DISPLAY
void inline MC_show_program_version() {
  MC_do_on_other_core(&monochrome_show_program_version);
}
#else
void inline MC_show_program_version() {
  monochrome_show_program_version();
}
#endif // MULTICORE_DISPLAY


void monochrome_show_subcycle_octave() {
  if(! monochrome_power_save) {
    if(selected_in(SCALES) != NULL) {
      (*u8x8_p).print(F("2^"));
      (*u8x8_p).print(CyclesConf.subcycle_octave);
    }
  }
}


// no MC_xxx version
uint8_t /*next_row*/ monochrome_multiline_string(uint8_t row, char* s) { // multiline string from row to bottom (max)
  if(s && *s /*no empty string*/) {
    uint8_t cols = (*u8x8_p).getCols();
    uint8_t rows = (*u8x8_p).getRows();

    char c ;
    int col;
    while(*s && row<rows) {
      col=0;
      (*u8x8_p).clearLine(row);
      (*u8x8_p).setCursor(0,row++);		// row control

      while(c = *s++) {
	(*u8x8_p).print(c);
	if ((++col % cols) == 0)
	  break;
      }
      col %= cols;

      if(c==0) {
	while(col++ < cols)  (*u8x8_p).print(' ');	// fill line witch spaces
	if(row < rows)
	  (*u8x8_p).clearLine(row++);			// clear one more line
	break;
      }
    } // all chars
  } // there *is* string content
  return row;
} // monochrome multiline string()


void monochrome_show_musicBox_parameters() {
  extern char run_state_symbol();

  if(monochrome_can_be_used()) {
    uint8_t cols = (*u8x8_p).getCols();
    uint8_t rows = (*u8x8_p).getRows();
    int row=0;	// first 2 lines empty for 2x2 output (i.e. MORSE)
    (*u8x8_p).clearLine(row++);	// clear empty line
    (*u8x8_p).clearLine(row++);	// clear empty line

    (*u8x8_p).clearLine(row);
    if(musicBoxConf.preset) {
      (*u8x8_p).setCursor(0,row);
      (*u8x8_p).print(run_state_symbol());
      (*u8x8_p).print(' ');

      (*u8x8_p).setInverseFont(1);
      (*u8x8_p).print(F("P "));
      (*u8x8_p).print(musicBoxConf.preset);
      (*u8x8_p).print(' ');
      (*u8x8_p).setInverseFont(0);
      (*u8x8_p).print(' ');
      monochrome_show_subcycle_octave();

      char run_state[2];
      run_state[0] = run_state_symbol();
      run_state[1] = 0;
      (*u8x8_p).draw2x2String(0, 0, run_state);      // *BIG* run state

      // *BIG* PRESET number on top		// poor old eyes little helper ;)
      char preset[10] = {0};
      itoa(musicBoxConf.preset, preset, 10);
      (*u8x8_p).draw2x2String(2, 0, preset);
    }
    row++;

    (*u8x8_p).clearLine(row);
    (*u8x8_p).drawString(0, row++, selected_name(SCALES));

    (*u8x8_p).clearLine(row);
#if defined ICODE_INSTEAD_OF_JIFFLES
    (*u8x8_p).drawString(0, row++, selected_name(iCODEs));
#else
    (*u8x8_p).drawString(0, row++, selected_name(JIFFLES));
#endif
    (*u8x8_p).clearLine(row);
    (*u8x8_p).setCursor(0,row++);
    (*u8x8_p).print(F("S="));	// sync
    (*u8x8_p).print(musicBoxConf.sync);

    if(musicBoxConf.stack_sync_slices) {	// stack_sync_slices?
      (*u8x8_p).print(F(" |"));
      (*u8x8_p).print(musicBoxConf.stack_sync_slices);
      if(musicBoxConf.base_pulse != ILLEGAL16) {
	(*u8x8_p).print(F(" p["));
	(*u8x8_p).print(musicBoxConf.base_pulse);
	(*u8x8_p).print(']');
      }

    } else {	// no stack_sync_slices, so there is space for other info:	// (can be removed)
      (*u8x8_p).print(F("  "));
      (*u8x8_p).print(musicBoxConf.pitch.multiplier);	// pitch
      (*u8x8_p).print('/');
      (*u8x8_p).print(musicBoxConf.pitch.divisor);
    }

    if(musicBoxConf.name && *musicBoxConf.name /*no empty string*/) {	// one line from name
      (*u8x8_p).setInverseFont(1);
      monochrome_multiline_string(row, musicBoxConf.name);
      (*u8x8_p).setInverseFont(0);

    } else { // if there is no name
      (*u8x8_p).clearLine(row);
      (*u8x8_p).setCursor(0,row);

      if(selected_in(SCALES) != NULL) {
	monochrome_show_subcycle_octave();
	(*u8x8_p).print(' ');
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
	  (*u8x8_p).print(days);
	  (*u8x8_p).print(F("d "));
	}
	if(hours || days) {
	  (*u8x8_p).print(hours);
	  (*u8x8_p).print(F("h "));
	}
	if(minutes || hours || days) {
	  (*u8x8_p).print(minutes);
	  (*u8x8_p).print(F("\' "));
	}
	(*u8x8_p).print(seconds);
	(*u8x8_p).print(F("\" "));
      } else									// TODO: factor out
	(*u8x8_p).print('-');
    } // name or no name

    if(musicBoxConf.chromatic_pitch) {
      extern char* metric_mnemonic;
      (*u8x8_p).draw2x2String(10, 0, metric_mnemonic);
      char s[] = {0, 0};
      extern char scale_symbol_char();
      s[0] = scale_symbol_char();
      (*u8x8_p).draw2x2String(14, 0, s);
    }
  } //if(monochrome_can_be_used())
} // monochrome_show_musicBox_parameters()

#if defined MULTICORE_DISPLAY
void inline MC_show_musicBox_parameters() {
  MC_do_on_other_core(&monochrome_show_musicBox_parameters);
}
#else
void inline MC_show_musicBox_parameters() {
  monochrome_show_musicBox_parameters();
}
#endif // MULTICORE_DISPLAY


void monochrome_show_line(uint8_t row, char * s) {	// TODO: redundant? see: monochrome_print_1line()
  if(monochrome_can_be_used()) {
    uint8_t cols = (*u8x8_p).getCols();
    char full_line[cols+1] = {0};
    char c;

    int i;
    for (i=0; i<cols; i++) {
      if(c = s[i])
	full_line[i] = c;
      else	// end of string reached
	break;
    }
    while(i< cols)	// fill line with spaces (avoid (*u8x8_p).clearLine() as it takes too long)
      full_line[i++] = ' ';
    full_line[cols] = '\0';

    (*u8x8_p).setCursor(0,row);
    (*u8x8_p).print(full_line);
  }
} // monochrome_show_line()

void monochrome_display_message(char* message) {
  if(monochrome_can_be_used()) {
    (*u8x8_p).setCursor((*u8x8_p).getCols() - strlen(message),  (*u8x8_p).getRows() -1);	// last line > right is message spot
    (*u8x8_p).print(message);
  }
}

#if defined MULTICORE_DISPLAY
TaskHandle_t multicore_display_message_handle;

void multicore_display_message_task(void* data_) {
  print_descrpt_t* data = (print_descrpt_t*) data_;

  xSemaphoreTake(MC_mux, portMAX_DELAY);

  monochrome_display_message(data->text);

  free_text_buffer(data);
  vTaskDelay(MC_DELAY_MS / portTICK_PERIOD_MS);
  xSemaphoreGive(MC_mux);
  vTaskDelete(NULL);
}

void multicore_display_message(char* text) {	// create and do one shot task
  print_descrpt_t* txt_descript_p = (print_descrpt_t*) malloc(sizeof(print_descrpt_t));
  if(txt_descript_p == NULL) {
    MENU.error_ln(F("txt_descript malloc()"));
    return;	// ERROR
  }
  copy_text_to_text_buffer(text, txt_descript_p);

  BaseType_t err = xTaskCreatePinnedToCore(multicore_display_message_task,		// function
					   "display_message",			// name
					   2000,				// stack size
					   txt_descript_p,				// task input parameter
					   MONOCHROME_PRIORITY,			// task priority
					   &multicore_display_message_handle,		// task handle
					   0);					// core 0
  if(err != pdPASS) {
    MENU.out(err);
    MENU.space();
    MENU.error_ln(F("display_message"));
  }
}

void MC_display_message(char* str) {	// extern declaration does not work with inline version
  multicore_display_message(str);
}

#else
 void MC_display_message(char* str) {	// extern declaration does not work with inline version
  monochrome_display_message(str);
 }
#endif // MULTICORE_DISPLAY


void oled_ui_display() {
  MENU.outln(F("'O<x>' OLED\t'OA'='OE'=on  'OT'=off  'OP'=whilePlaying"));
}

bool OLED_UI() {	// follows 'O'		'OE'	'OT'	'OP'
  switch (MENU.peek()) {
  case 'E':		// 'OA' == 'OE'	all on   (morse friendly)
  case 'A':		// 'OA' == 'OE'	all on   analogue morse COMMAND 'OA'
    MENU.drop_input_token();
    monochrome_power_save = 0;
    (*u8x8_p).setPowerSave(monochrome_power_save);
    oled_feedback_while_playing = true;
    MENU.outln(F("OLED on"));
    break;

  case 'T':		// 'OT'	OLED off (morse friendly)
    MENU.drop_input_token();
    monochrome_power_save = 1;
    (*u8x8_p).setPowerSave(monochrome_power_save);
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
    int max=((*u8x8_p).getCols()/2);	// limit length
    char truncated[max+1]={0};

    char c;
    for(int i=0; i<max; i++) {
      truncated[i] = str[i];
      if(truncated[i] == 0)
	break;
    }
    (*u8x8_p).draw2x2String(col, row, truncated);
  } // if(monochrome_can_be_used())
} // monochrome_print2x2()

#if defined MULTICORE_DISPLAY
TaskHandle_t multicore_print2x2_handle;

void multicore_print2x2_task(void* data_) {
  print_descrpt_t* data = (print_descrpt_t*) data_;

  xSemaphoreTake(MC_mux, portMAX_DELAY);

  monochrome_print2x2(data->col, data->row, data->text);

  free_text_buffer(data);
  vTaskDelay(MC_DELAY_MS / portTICK_PERIOD_MS);
  xSemaphoreGive(MC_mux);
  vTaskDelete(NULL);
}

void multicore_print2x2(uint8_t col, uint8_t row, char* text) {	// create and do one shot task
  print_descrpt_t* txt_descript_p = (print_descrpt_t*) malloc(sizeof(print_descrpt_t));
  if(txt_descript_p == NULL) {
    MENU.error_ln(F("txt_descript malloc()"));
    return;	// ERROR
  }
  copy_text_to_text_buffer(text, txt_descript_p);
  txt_descript_p->col = col;
  txt_descript_p->row = row;

  BaseType_t err = xTaskCreatePinnedToCore(multicore_print2x2_task,		// function
					   "print2x2",				// name
					   2000,				// stack size
					   txt_descript_p,			// task input parameter
					   MONOCHROME_PRIORITY,			// task priority
					   &multicore_print2x2_handle,		// task handle
					   0);					// core 0
  if(err != pdPASS) {
    MENU.out(err);
    MENU.space();
    MENU.error_ln(F("2x2display task"));
  }
}

void inline MC_printBIG_at(uint8_t col, uint8_t row, char* str) {
  multicore_print2x2(col, row, str);
}

#else
void inline MC_printBIG_at(uint8_t col, uint8_t row, char* str) {
  monochrome_print2x2(col, row, str);
}
#endif // MULTICORE_DISPLAY


void monochrome_println2x2(uint8_t row, char* str) {	// 2x2 lines
  if(monochrome_can_be_used()) {
    int max=((*u8x8_p).getCols()/2);	// limit length
    char truncated[max+1];
    for(int i=0; i<max; i++)
      truncated[i] = ' ';	// space filled to clear line
    truncated[max] = 0;		// /0 terminated

    char c;
    for(int i=0; i<=max; i++) {
      if(c = str[i])
	truncated[i] = c;
      else
	break;
    }

    (*u8x8_p).draw2x2String(0, row, truncated);
  } // if(monochrome_can_be_used())
} // monochrome_println2x2()

#if defined MULTICORE_DISPLAY
TaskHandle_t multicore_println2x2_handle;

void multicore_println2x2_task(void* data_) {
  print_descrpt_t* data = (print_descrpt_t*) data_;
  xSemaphoreTake(MC_mux, portMAX_DELAY);

  monochrome_println2x2(data->row, data->text);

  free_text_buffer(data);
  vTaskDelay(MC_DELAY_MS / portTICK_PERIOD_MS);
  xSemaphoreGive(MC_mux);
  vTaskDelete(NULL);
}

void multicore_println2x2(uint8_t row, char* text) {	// create and do one shot task
  print_descrpt_t* txt_descript_p = (print_descrpt_t*) malloc(sizeof(print_descrpt_t));
  if(txt_descript_p == NULL) {
    MENU.error_ln(F("txt_descript malloc()"));
    return;	// ERROR
  }
  copy_text_to_text_buffer(text, txt_descript_p);
  txt_descript_p->row = row;

  BaseType_t err = xTaskCreatePinnedToCore(multicore_println2x2_task,		// function
					   "println2x2",			// name
					   2000,				// stack size
					   txt_descript_p,			// task input parameter
					   MONOCHROME_PRIORITY,			// task priority
					   &multicore_println2x2_handle,	// task handle
					   0);					// core 0
  if(err != pdPASS) {
    MENU.out(err);
    MENU.space();
    MENU.error_ln(F("println2x2"));
  }
}

void inline MC_printlnBIG(uint8_t row, char* str) {
  multicore_println2x2(row, str);
}

#else
void inline MC_printlnBIG(uint8_t row, char* str) {
  monochrome_println2x2(row, str);
}
#endif // MULTICORE_DISPLAY


void monochrome_print_1line(uint8_t row, char* str) { // truncating, padding	// TODO: redundant? see: monochrome_show_line()
  if(monochrome_can_be_used()) {
    if(str) {
      //      (*u8x8_p).clearLine(row);
      uint8_t cols = (*u8x8_p).getCols();
      int chars;
      char buffer[cols + 1];

      char c=1;	// dummy
      for(chars=0; chars<cols; chars++)
	if(c = str[chars])
	  buffer[chars] = c;
	else
	  break;

      for(; chars<cols; chars++)	// string was short?
	buffer[chars] = ' ';		//   pad with spaces
      buffer[cols]='\0';

      (*u8x8_p).setCursor(0,row);
      (*u8x8_p).print(buffer);
    }
  }
} // monochrome_print_1line()

uint8_t /*next_row*/ monochrome_big_or_multiline(int row, char* str) {
  /*
    print one line on monochrome
    use 2x2 size if it fits
    else use basic size
    even multinline, if needed

    return next_row
  */

  if(monochrome_can_be_used()) {
    int len = strlen(str);
    if(len <= ((*u8x8_p).getCols() / 2)) {	// fits in one 2x2 line?
      monochrome_println2x2(row, str);
      row += 2;
    } else				// too long for 2x2
      row = monochrome_multiline_string(row, str);
  } // if(monochrome_can_be_used())

  return row;
} // monochrome_big_or_multiline()


#if defined MULTICORE_DISPLAY
TaskHandle_t multicore_big_or_multiline_handle;

void multicore_big_or_multiline_task(void* data_) {
  print_descrpt_t* data = (print_descrpt_t*) data_;
  xSemaphoreTake(MC_mux, portMAX_DELAY);

  monochrome_big_or_multiline(data->row, data->text);

  free_text_buffer(data);
  vTaskDelay(MC_DELAY_MS / portTICK_PERIOD_MS);
  xSemaphoreGive(MC_mux);
  vTaskDelete(NULL);
}

void multicore_big_or_multiline(uint8_t row, char* text) {	// create and do one shot task
  print_descrpt_t* txt_descript_p = (print_descrpt_t*) malloc(sizeof(print_descrpt_t));
  if(txt_descript_p == NULL) {
    MENU.error_ln(F("txt_descript malloc()"));
    return;	// ERROR
  }
  copy_text_to_text_buffer(text, txt_descript_p);
  txt_descript_p->row = row;

  BaseType_t err = xTaskCreatePinnedToCore(multicore_big_or_multiline_task,	// function
					   "big_or_multilin",			// name
					   2000,				// stack size
					   txt_descript_p,			// task input parameter
					   MONOCHROME_PRIORITY,			// task priority
					   &multicore_big_or_multiline_handle,	// task handle
					   0);					// core 0
  if(err != pdPASS) {
    MENU.out(err);
    MENU.space();
    MENU.error_ln(F("big_or_multiline"));
  }
}

void MC_big_or_multiline(uint8_t row, char* str) {	// does not work as inline
  multicore_big_or_multiline(row, str);
}

#else
void inline MC_big_or_multiline(uint8_t row, char* str) {
  monochrome_big_or_multiline(row, str);
}
#endif // MULTICORE_DISPLAY


inline void monochrome_setInverseFont() {
  if(monochrome_can_be_used())
    (*u8x8_p).setInverseFont(1);
}

#if defined MULTICORE_DISPLAY
void inline MC_setInverseFont() {
  do_on_other_core(monochrome_setInverseFont);
}
#else
void inline MC_setInverseFont() {
  monochrome_setInverseFont();
}
#endif // MULTICORE_DISPLAY


inline void monochrome_clearInverseFont() {
  if(monochrome_can_be_used())
    (*u8x8_p).setInverseFont(0);
}

#if defined MULTICORE_DISPLAY
void inline MC_clearInverseFont() {
  do_on_other_core(&monochrome_clearInverseFont);
}
#else
void inline MC_clearInverseFont() {
  monochrome_clearInverseFont();
}
#endif // MULTICORE_DISPLAY


inline void monochrome_setPowerSave(uint8_t value) {
  (*u8x8_p).setPowerSave(value);	// try to set it anyway
}


inline void monochrome_setCursor(uint8_t col, uint8_t row) {
  if(monochrome_can_be_used())
    (*u8x8_p).setCursor(col, row);
}

#if defined MULTICORE_DISPLAY
TaskHandle_t multicore_setCursor_handle;

void multicore_setCursor_task(void* data_) {
  print_descrpt_t* data = (print_descrpt_t*) data_;
  xSemaphoreTake(MC_mux, portMAX_DELAY);

  monochrome_setCursor(data->col, data->row);

  free_text_buffer(data);
  vTaskDelay(MC_DELAY_MS / portTICK_PERIOD_MS);
  xSemaphoreGive(MC_mux);
  vTaskDelete(NULL);
}

void multicore_setCursor(uint8_t col, uint8_t row) {	// create and do one shot task
  print_descrpt_t* txt_descript_p = (print_descrpt_t*) malloc(sizeof(print_descrpt_t));
  if(txt_descript_p == NULL) {
    MENU.error_ln(F("txt_descript malloc()"));
    return;	// ERROR
  }
  txt_descript_p->col = col;
  txt_descript_p->row = row;

  BaseType_t err = xTaskCreatePinnedToCore(multicore_setCursor_task,		// function
					   "setCursor",			// name
					   2000,				// stack size
					   txt_descript_p,				// task input parameter
					   MONOCHROME_PRIORITY,			// task priority
					   &multicore_setCursor_handle,		// task handle
					   0);					// core 0
  if(err != pdPASS) {
    MENU.out(err);
    MENU.space();
    MENU.error_ln(F("setCursor"));
  }
}

void inline MC_setCursor(uint8_t col, uint8_t row) {
  multicore_setCursor(col, row);
}

#else
void inline MC_setCursor(uint8_t col, uint8_t row) {
  monochrome_setCursor(col, row);
}
#endif // MULTICORE_DISPLAY


inline void monochrome_print(char* str) {
  if(monochrome_can_be_used())
    (*u8x8_p).print(str);
}

#if defined MULTICORE_DISPLAY
TaskHandle_t multicore_print_handle;

void multicore_print_task(void* data_) {
  print_descrpt_t* data = (print_descrpt_t*) data_;
  xSemaphoreTake(MC_mux, portMAX_DELAY);

  monochrome_print(data->text);
  
  free_text_buffer(data);
  vTaskDelay(MC_DELAY_MS / portTICK_PERIOD_MS);
  xSemaphoreGive(MC_mux);
  vTaskDelete(NULL);
}

void multicore_print(char* text) {	// create and do one shot task
  print_descrpt_t* txt_descript_p = (print_descrpt_t*) malloc(sizeof(print_descrpt_t));
  if(txt_descript_p == NULL) {
    MENU.error_ln(F("txt_descript malloc()"));
    return;	// ERROR
  }
  copy_text_to_text_buffer(text, txt_descript_p);

  BaseType_t err = xTaskCreatePinnedToCore(multicore_print_task,		// function
					   "print",			// name
					   2000,				// stack size
					   txt_descript_p,			// task input parameter
					   MONOCHROME_PRIORITY,			// task priority
					   &multicore_print_handle,		// task handle
					   0);					// core 0
  if(err != pdPASS) {
    MENU.out(err);
    MENU.space();
    MENU.error_ln(F("MC_print"));
  }
}

void inline MC_print(char* str) {
  multicore_print(str);
}

#else
 void inline MC_print(char* str) {
   monochrome_print(str);
 }
#endif // MULTICORE_DISPLAY


inline void monochrome_print_f(float f) {
  if(monochrome_can_be_used())
    (*u8x8_p).print(f);
}

inline void monochrome_print_f(float f, int chiffres) {
  if(monochrome_can_be_used())
    (*u8x8_p).print(f, chiffres);
}

inline uint8_t monochrome_getCols() {
  return (*u8x8_p).getCols();
}

inline uint8_t monochrome_getRows() {
  return (*u8x8_p).getRows();
}

inline void monochrome_print(char c) {
  if(monochrome_can_be_used())
    (*u8x8_p).print(c);
}

inline void monochrome_print(int i) {
  if(monochrome_can_be_used())
    (*u8x8_p).print(i);
}


inline void monochrome_clear() {	// slow, but does it *now*
  if(monochrome_can_be_used())
    (*u8x8_p).clear();
}

#if defined MULTICORE_DISPLAY
void inline MC_clear_display() {	// does it later, might be too late...	(use: monochrome_clear())
  do_on_other_core(&monochrome_clear);
}
#else
void inline MC_clear_display() {
  monochrome_clear();
}
#endif // MULTICORE_DISPLAY


inline void monochrome_clearLine(uint8_t row) {	// slow
  (*u8x8_p).clearLine(row);
}

#if defined MULTICORE_DISPLAY
TaskHandle_t multicore_clearLine_handle;

void multicore_clearLine_task(void* data_) {
  print_descrpt_t* data = (print_descrpt_t*) data_;
  xSemaphoreTake(MC_mux, portMAX_DELAY);

  monochrome_clearLine(data->row);

  free_text_buffer(data);
  vTaskDelay(MC_DELAY_MS / portTICK_PERIOD_MS);
  xSemaphoreGive(MC_mux);
  vTaskDelete(NULL);
}

void multicore_clearLine(uint8_t row) {	// create and do one shot task
  print_descrpt_t* txt_descript_p = (print_descrpt_t*) malloc(sizeof(print_descrpt_t));
  if(txt_descript_p == NULL) {
    MENU.error_ln(F("txt_descript malloc()"));
    return;	// ERROR
  }
  txt_descript_p->text = NULL;
  txt_descript_p->row = row;

  BaseType_t err = xTaskCreatePinnedToCore(multicore_clearLine_task,		// function
					   "clearLine",			// name
					   2000,				// stack size
					   txt_descript_p,			// task input parameter
					   MONOCHROME_PRIORITY,			// task priority
					   &multicore_clearLine_handle,	// task handle
					   0);					// core 0
  if(err != pdPASS) {
    MENU.out(err);
    MENU.space();
    MENU.error_ln(F("clearLine"));
  }
}

void inline MC_clearLine(uint8_t row) {
  multicore_clearLine(row);
}

#else
 void inline MC_clearLine(uint8_t row) {
  monochrome_clearLine(row);
 }
#endif // MULTICORE_DISPLAY


inline void monochrome_setFont(const uint8_t *font_8x8) {
  (*u8x8_p).setFont(font_8x8);
}


void monochrome_show_names() {
  uint8_t rows = monochrome_getRows();
  uint8_t next_row = 0;

  next_row = extended_output(my_IDENTITY.preName, 0, next_row, true);
  MENU.ln();
  monochrome_clearLine(next_row++);	// clear one more line
  if((musicBoxConf.name != NULL) && (*musicBoxConf.name)) {
    MENU.outln(musicBoxConf.name);
    next_row = monochrome_big_or_multiline(next_row, musicBoxConf.name);
  } // name

  while (next_row < rows - 2)
    monochrome_clearLine(next_row++);		// clear in between lines

  if(next_row <= (rows - 2)) {			// 2 more free rows?
    if(musicBoxConf.preset) {
      monochrome_clearLine(next_row);		// clear 2 lines
      monochrome_clearLine(next_row +1);
      char txt[9];
      char* format = F("%i#  %s");
      extern char* metric_mnemonic;
      snprintf(txt, 8, format, musicBoxConf.preset, metric_mnemonic);
      monochrome_print2x2(0, next_row, txt);	//   yes, show preset number# and metric_mnemonic
    } // preset# and metric_mnemonic
  } // free 2 bottom lines?
} // monochrome_show_names()

#if defined MULTICORE_DISPLAY
void inline MC_show_names() {
  do_on_other_core(&monochrome_show_names);
}
#else
 void inline MC_show_names() {
   monochrome_show_names();
 }
#endif // MULTICORE_DISPLAY


void monochrome_show_tuning() {
  if(monochrome_can_be_used()) {
    (*u8x8_p).clear();
    monochrome_big_or_multiline(0, selected_name(SCALES));

    char txt[9];
    char* format = F("%i / %i  %s");
    extern char* metric_mnemonic;
    snprintf(txt, 9, format, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, metric_mnemonic);
    monochrome_print2x2(0, 3, txt);
    MENU.outln(txt);
  }
}

#if defined MULTICORE_DISPLAY
void inline MC_show_tuning() {
  MC_do_on_other_core(&monochrome_show_tuning);
}
#else
 void inline MC_show_tuning() {
   monochrome_show_tuning();
 }
#endif // MULTICORE_DISPLAY


#if defined USE_ESP_NOW
void monochrome_show_peer_list() {
  int peer_cnt=0;
  for(int i=0; i<ESP_NOW_MAX_TOTAL_PEER_NUM; i++) {
    if(mac_is_non_zero(esp_now_pulses_known_peers[i].mac_addr))
      peer_cnt++;
  }
  (*u8x8_p).clear();

  uint8_t row=0;
  bool is_send_to_peer;
  bool small = peer_cnt > (monochrome_getRows() / 2);
  for(int i=0; i<ESP_NOW_MAX_TOTAL_PEER_NUM; i++) {
    if(mac_is_non_zero(esp_now_pulses_known_peers[i].mac_addr)) {
      if(is_send_to_peer = (! mac_cmp(esp_now_send2_mac_p, esp_now_pulses_known_peers[i].mac_addr)))	// send to peer?
	(*u8x8_p).setInverseFont(1);

      if(small) {
	(*u8x8_p).setCursor(0,row++);
	(*u8x8_p).print(esp_now_pulses_known_peers[i].preName);
      } else {
	//	(*u8x8_p).draw2x2String(0, 2*row++, esp_now_pulses_known_peers[i].preName);
	monochrome_big_or_multiline(2*row++, esp_now_pulses_known_peers[i].preName);
      }
      if(is_send_to_peer)
	(*u8x8_p).setInverseFont(0);
    }
  }
} // monochrome_show_peer_list()


void MC_esp_now_peer_list() {
#if defined MULTICORE_DISPLAY
  MC_do_on_other_core(&monochrome_show_peer_list);
#else
  monochrome_show_peer_list();
#endif
}
#endif // USE_ESP_NOW


void inline monochrome_set_default_font() {
  //  monochrome_setFont(u8x8_font_chroma48medium8_r);	// *Umlaute missing*
  monochrome_setFont(u8x8_font_amstrad_cpc_extended_f);	// *Umlaute here, but strange*
}


void try_monochrome_fix() {	// DADA:  ################	not working appropriate
  MENU.outln(F("TODO: try_monochrome_fix()"));

  // MENU.outln(F("(*u8x8_p).initDisplay();"));
  // (*u8x8_p).initDisplay();

  /*
    MENU.outln(F("monochrome_begin()"));
    monochrome_begin();	// is in hw_display_setup() now
  */

  /*
    MENU.outln(F("monochrome <ESC>c"));
    monochrome_print((char) 27);
    monochrome_print('c');

    MENU.outln(F("monochrome <ESC>C"));
    monochrome_print((char) 27);
    monochrome_print('C');
  */

  MENU.outln(F("(*u8x8_p).setFlipMode(0)"));
  (*u8x8_p).setFlipMode(0);

  /*
    MENU.outln(F("(*u8x8_p).refreshDisplay()"));	// see: Only required for SSD1606
    (*u8x8_p).refreshDisplay();
  */

  MENU.outln(F("(*u8x8_p).clear()"));
  (*u8x8_p).clear();
} // try_monochrome_fix()

void hw_display_setup() {
  MENU.out(F("hw_display_setup() "));
  switch(HARDWARE.monochrome_type) {
  case monochrome_type_heltec:
    u8x8_p = new U8X8_SSD1306_128X64_NONAME_SW_I2C(/* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16); // BOARD_HELTEC_OLED
    MENU.out(F("heltec c15 d4 r16"));
    (*u8x8_p).begin();
    monochrome_set_default_font();
    break;
  case monochrome_type_LiPO:
    u8x8_p = new U8X8_SSD1306_128X64_NONAME_SW_I2C(/* clock=*/ 4, /* data=*/ 5, /* reset=*/ 16);  // BOARD_OLED_LIPO
    MENU.out(F("Lipo c4 d5 r16"));
    (*u8x8_p).begin();
    monochrome_set_default_font();
    break;

  case monochrome_type_off:
    MENU.error_ln(F("monochrome is off in nvs?"));
#if defined BOARD_OLED_LIPO		// try to fix from pp macros
    HARDWARE.monochrome_type = monochrome_type_LiPO;
    hw_display_setup();		// fixed(?), recurse
#elif defined BOARD_HELTEC_OLED		// heltec
    HARDWARE.monochrome_type = monochrome_type_heltec;
    hw_display_setup();		// fixed(?), recurse
#endif
    break;
  default:
    MENU.error_ln(F("unknown monochrome_type"));
  }
  MENU.ln();

  if(MC_mux == NULL)
    MC_mux = xSemaphoreCreateMutex();

} // hw_display_setup()

#define MONOCHROME_DISPLAY_H
#endif
