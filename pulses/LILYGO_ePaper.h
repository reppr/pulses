/*
  LILYGO_ePaper.h
  see: GxEPD2/examples/GxEPD2_GFX_Example
*/

#define DEBUG_ePAPER
#define USE_MANY_FONTS		// uses some more program storage space

#include <GxEPD2_BW.h>
#include <GxEPD2_GFX.h>

#include <Fonts/FreeMonoBold9pt7b.h>	// mono 9pt
#include <Fonts/FreeMonoBold12pt7b.h>	// mono 12pt

#include <Fonts/FreeMonoBold18pt7b.h>

#include <Fonts/FreeMono9pt7b.h>
#include <Fonts/FreeMonoBoldOblique9pt7b.h>
#include <Fonts/FreeMonoOblique9pt7b.h>
#include <Fonts/FreeSans9pt7b.h>	// <<<<
#include <Fonts/FreeSans12pt7b.h>	// <<<<
#include <Fonts/FreeSansBold9pt7b.h>	// <<<<<<<<
#include <Fonts/FreeSansBold12pt7b.h>	// <<<<<<<<
#include <Fonts/FreeSansBoldOblique9pt7b.h>
#include <Fonts/FreeSansOblique9pt7b.h>
#include <Fonts/FreeSerif9pt7b.h>
#include <Fonts/FreeSerifBold9pt7b.h>
#include <Fonts/FreeSerifBoldItalic9pt7b.h>
#include <Fonts/FreeSerifItalic9pt7b.h>
#include <Fonts/Org_01.h>
#include <Fonts/Picopixel.h>
#include <Fonts/Tiny3x3a2pt7b.h>
#include <Fonts/TomThumb.h>

GxEPD2_BW<GxEPD2_213_B73, GxEPD2_213_B73::HEIGHT> ePaper(GxEPD2_213_B73(/*CS=5*/ SS, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4)); // GDEH0213B73


#if defined MULTICORE_DISPLAY
  #include <freertos/task.h>

  // testing RTOS task priority for monochrome display routines:
  #define MONOCHROME_PRIORITY	0	// seems best
  //#define MONOCHROME_PRIORITY	1	// was 0
  //#define MONOCHROME_PRIORITY	2	// was 0

/* MUTEX for 2x2 printing (or just wait a bit... ;)
  SemaphoreHandle_t monochrome_mutex = xSemaphoreCreateMutex();
*/

  #define MONOCHROME_TEXT_BUFFER_SIZE	156	// (7*22 +2)	// TODO: more versatile implementation
  char monochrome_text_buffer[MONOCHROME_TEXT_BUFFER_SIZE] = {0};

  typedef struct display_string_t {
    int16_t col=0;
    int16_t row=0;
    int16_t offset_y=0;
    //uint8_t size=0;
    //uint8_t colour=0;
    //bool inverted=false;
    char* text = (char*) &monochrome_text_buffer;
  } display_string_t;

  display_string_t text_descriptor;	// HACK:  uses global data structure...

  void copy_text_to_text_buffer(char* text) {
    char c;
    for(int i=0; i < MONOCHROME_TEXT_BUFFER_SIZE; i++) {
      c = text_descriptor.text[i] = text[i];
      if(c==0)
	break;
    }
    text_descriptor.text[MONOCHROME_TEXT_BUFFER_SIZE - 1] = 0;
    if(strlen(text) > (MONOCHROME_TEXT_BUFFER_SIZE -1)) {
      MENU.outln(text_descriptor.text);
      MENU.error_ln(F("string too long, truncated"));
    }
  } // copy_text_to_text_buffer()
#endif


void LILYGO_ePaper_infos() {
  // give some infos:
  for(int rotation=0; rotation<4; rotation++) {
    ePaper.setRotation(rotation);
    MENU.out(F("rotation("));
    MENU.out(rotation);			//	0	1	2	3
    MENU.out(F(")\tpage hight "));
    MENU.out(ePaper.pageHeight());	//	250	250	250	250
    MENU.out(F("\twidth "));
    MENU.outln(ePaper.width());		//	128	250	128	250
  }

  MENU.out(F("hasFastPartialUpdate "));
  if(ePaper.epd2.hasFastPartialUpdate)
    MENU.outln(F("YES"));
  else
    MENU.outln(F("no"));
  MENU.ln();
} // LILYGO_ePaper_infos()


void setup_LILYGO_ePaper() {
  MENU.outln(F("setup_LILYGO_ePaper()"));

  //ePaper.init(500000);	// debug baudrate
  ePaper.init(0);		// no debugging

  int rotation=1;		// TODO: rotation should go into HARDWARE
  ePaper.setRotation(rotation);	// TODO: rotation should go into HARDWARE

  delay(1000);			// TODO: test, maybe REMOVE?:
} // setup_LILYGO_ePaper()


void inline monochrome_setup() {
  setup_LILYGO_ePaper();

  // little helpers while implementing, TODO: REMOVE?:
  LILYGO_ePaper_infos();
  LILYGO_ePaper_infos(); /*fix rotation*/ ePaper.setRotation(1);

  extern void ePaper_show_program_version();
  do_on_other_core(&ePaper_show_program_version);
  delay(1000);			// TODO: test, maybe REMOVE?:
}


// GFXfont* small_font_p = (GFXfont*) NULL;
// GFXfont* default_font_p = (GFXfont*) &FreeMonoBold9pt7b;
// GFXfont* BIG_font_p = (GFXfont*) &FreeMonoBold12pt7b;

GFXfont* used_font_p = (GFXfont*) NULL;

uint8_t used_font_x=11;
uint8_t used_font_y=128/7;	// 18
uint16_t max_line_length=22;

void set_used_font(const GFXfont* font_p) {
#if defined DEBUG_ePAPER
  MENU.out(F("DEBUG_ePAPER\tset_used_font()"));
#endif
  if(font_p == &FreeMonoBold9pt7b) {
    used_font_x = 250/22;	// 11
    used_font_y = 128/7;	// 18
    max_line_length=22;

  } else if(font_p == &FreeMonoBold12pt7b) {
    used_font_x = 14;		// ~250/18;
    used_font_y = 24;		// ~128/5 -1;
    max_line_length=18;

  } else {
    MENU.error_ln(F("unknown font size"));
    return;	// do not change pointer nor size
  }

  /*
#if defined DEBUG_ePAPER
  MENU.out(F("yAdvance "));
  MENU.outln(font_p->yAdvance);
#endif
  */

  ePaper.setFont(font_p);
  used_font_p = (GFXfont*) font_p;
} // set_used_font()


int16_t col2x(int16_t col) {	// *do* call set_used_font() before using that
  return col*used_font_x;
}

int16_t row2y(int16_t row) {	// *do* call set_used_font() before using that
  return (row+1)*used_font_y;
}

void ePaper_print_at(uint16_t col, uint16_t row, char* text, int16_t offset_y=0) {	// *do* call set_used_font() before using that
#if defined  DEBUG_ePAPER
  MENU.out(F("DEBUG_ePAPER\tePaper_print_at(...)\t"));
  MENU.outln(text);
#endif

  ePaper.setTextColor(GxEPD_BLACK);
  ePaper.setFont(used_font_p);
  int16_t x = col2x(col);
  int16_t y = row2y(row);
  y += offset_y;

  ePaper.fillRect(x, y - used_font_y + 4, strlen(text)*used_font_x, used_font_y, GxEPD_WHITE);
  ePaper.setCursor(x, y);
  ePaper.print(text);
  ePaper.display(true);
} // ePaper_print_at()

#if defined MULTICORE_DISPLAY
TaskHandle_t ePaper_print_at_handle;

void ePaper_print_at_task(void* data_) {
  display_string_t* data = (display_string_t*) data_;
  ePaper_print_at(data->col, data->row, data->text, data->offset_y);
  vTaskDelete(NULL);
}

void multicore_ePaper_print_at(int16_t col, int16_t row, char* text, int16_t offset_y) {	// create and start a one shot task
  text_descriptor.col = col;
  text_descriptor.row = row;
  text_descriptor.offset_y = offset_y;
  copy_text_to_text_buffer(text);

  BaseType_t err = xTaskCreatePinnedToCore(ePaper_print_at_task,		// function
					   "ePaper_print_at",			// name
					   2000,				// stack size
					   &text_descriptor,			// task input parameter
					   MONOCHROME_PRIORITY,			// task priority
					   &ePaper_print_at_handle,		// task handle
					   0);					// core 0
  if(err != pdPASS) {
    MENU.out(err);
    MENU.space();
    MENU.error_ln(F("ePaper_print_at_task"));
  }
} // multicore_ePaper_print_at()

void MC_print_at(int16_t col, int16_t row, char* text, int16_t offset_y=0) {
  multicore_ePaper_print_at(col, row, text, offset_y);
}

#else
void MC_print_at(int16_t col, int16_t row, char* text, int16_t offset_y=0) {
  ePaper_print_at(col, row, text, offset_y);
}
#endif

void MC_printBIG_at(int16_t col, int16_t row, char* text, int16_t offset_y=0) {
  set_used_font(&FreeMonoBold12pt7b);
  MC_print_at(col, row, text, offset_y);
}

void monochrome_clear() {
#if defined  DEBUG_ePAPER
  MENU.outln(F("DEBUG_ePAPER\tmonochrome_clear()"));
#endif
  ePaper.setFullWindow();
  ePaper.fillScreen(GxEPD_WHITE);
  ePaper.display(true);
} // monochrome_clear()


void ePaper_basic_parameters() {
#if defined  DEBUG_ePAPER
  MENU.outln(F("DEBUG_ePAPER\tePaper_basic_parameters()"));
#endif

  ePaper.setFullWindow();
  ePaper.fillScreen(GxEPD_WHITE);
  ePaper.setTextColor(GxEPD_BLACK);

  char txt[23];
  char* format_IstrI = F("|%s|");
  char* format2s = F("%s  %s");
  char* format_s = F("%s");
  char* format_is = F("%i %s");
  char* format_sync = F("S=%i");

  ePaper.firstPage();
  do
  {
    ePaper.fillScreen(GxEPD_WHITE);
    ePaper.setCursor(0, 0);

#if defined USE_MANY_FONTS
    ePaper.setFont(&FreeSansBold12pt7b);
#else
    ePaper.setFont(&FreeMonoBold12pt7b);
#endif
    ePaper.println();

    snprintf(txt, 23, format_IstrI, my_IDENTITY.preName);
    ePaper.println(txt);

#if defined USE_MANY_FONTS
    ePaper.setFont(&FreeSansBold9pt7b);
#else
    ePaper.setFont(&FreeMonoBold9pt7b);
#endif
    snprintf(txt, 23, format_is, musicBoxConf.preset, musicBoxConf.name);
    ePaper.println(txt);

    extern char* metric_mnemonic;
    snprintf(txt, 23, format2s, selected_name(SCALES), metric_mnemonic);
    ePaper.println(txt);

#if defined ICODE_INSTEAD_OF_JIFFLES
    snprintf(txt, 23, format_s, selected_name(iCODEs));
#else
    snprintf(txt, 23, format_s, selected_name(JIFFLES));
#endif
    ePaper.println(txt);

    snprintf(txt, 23, format_sync, musicBoxConf.sync);
    ePaper.println(txt);
  }
  while (ePaper.nextPage());
} // ePaper_basic_parameters()

void inline MC_show_musicBox_parameters() {
  do_on_other_core(&ePaper_basic_parameters);
}


void  ePaper_show_program_version() {
#if defined  DEBUG_ePAPER
  MENU.outln(F("DEBUG_ePAPER\tePaper_show_program_version()"));
#endif

  ePaper.setFullWindow();
  ePaper.fillScreen(GxEPD_WHITE);
  ePaper.setTextColor(GxEPD_BLACK);

  char txt[23];
  char* format_s = F("%s");

  ePaper.firstPage();
  do
  {
    ePaper.fillScreen(GxEPD_WHITE);
    ePaper.setCursor(0, 0);
#if defined USE_MANY_FONTS
    ePaper.setFont(&FreeSans12pt7b);
#else
    ePaper.setFont(&FreeSansBold12pt7b);
#endif
    ePaper.println();

    snprintf(txt, 23, format_s, F(STRINGIFY(PROGRAM_VERSION)));
    ePaper.println(txt);

#if defined USE_MANY_FONTS
    ePaper.setFont(&FreeSans9pt7b);
#else
    ePaper.setFont(&FreeSansBold9pt7b);
#endif
#if defined PROGRAM_SUB_VERSION
    snprintf(txt, 23, format_s, F(STRINGIFY(PROGRAM_SUB_VERSION)));
    ePaper.println(txt);
#endif
    ePaper.println();

#if defined USE_MANY_FONTS
    ePaper.setFont(&FreeSans12pt7b);
#else
    ePaper.setFont(&FreeSansBold12pt7b);
#endif
    snprintf(txt, 23, F("|%s|  (%i)"), my_IDENTITY.preName, my_IDENTITY.esp_now_time_slice);
    ePaper.println(txt);
    //ePaper.println();

    // TODO: option mnemonics?
  }
  while (ePaper.nextPage());
} // ePaper_show_program_version()

void inline MC_show_program_version() {
  do_on_other_core(&ePaper_show_program_version);
}


void  ePaper_show_tuning() {
#if defined  DEBUG_ePAPER
  MENU.outln(F("DEBUG_ePAPER\tePaper_show_tuning()"));	// DADA remove debugging code ################
#endif

  ePaper.setFullWindow();
  ePaper.fillScreen(GxEPD_WHITE);
  ePaper.setTextColor(GxEPD_BLACK);

  char txt[23];
  char* format_s = F("%s");

  ePaper.firstPage();
  do
  {
    //ePaper.setFont(&FreeSansBold9pt7b);
    ePaper.setFont(&FreeSansBold12pt7b);
    ePaper.fillScreen(GxEPD_WHITE);
    ePaper.setCursor(0, 0);
    ePaper.println();

    snprintf(txt, 23, format_s, selected_name(SCALES));
    ePaper.println(txt);
    ePaper.println();

    extern char* metric_mnemonic;
    snprintf(txt, 12, F("%i/%i %s"), musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, metric_mnemonic);
    //ePaper.setFont(&FreeSansBold12pt7b);
    ePaper.println(txt);
  }
  while (ePaper.nextPage());
} // ePaper_show_tuning()

void inline MC_show_tuning() {
  do_on_other_core(&ePaper_show_tuning);
}


void ePaper_print_1line_at(uint16_t row, char* text, int16_t offset_y=0) {	// *do* call set_used_font() before using that
#if defined  DEBUG_ePAPER
  MENU.out(F("DEBUG_ePAPER\tePaper_print_1line_at() "));
  MENU.out(row);
  MENU.tab();
  MENU.outln(text);
#endif

  ePaper.setRotation(1);
  ePaper.setTextColor(GxEPD_BLACK);
  ePaper.setFont(used_font_p);
  int16_t x = 0;
  int16_t y = row2y(row);
  y += offset_y;

  ePaper.fillRect(x, y - used_font_y + 4, ePaper.width(), used_font_y, GxEPD_WHITE);
  ePaper.setCursor(x, y);
  ePaper.print(text);
  ePaper.display(true);
} // ePaper_print_1line_at()

#if defined MULTICORE_DISPLAY
TaskHandle_t ePaper_1line_at_handle;

void ePaper_1line_at_task(void* data_) {
  display_string_t* data = (display_string_t*) data_;
  ePaper_print_1line_at(data->row, data->text, data->offset_y);
  vTaskDelete(NULL);
}

void multicore_ePaper_1line_at(int16_t row, char* text, int16_t offset_y) {	// create and start a one shot task
  text_descriptor.row = row;
  text_descriptor.offset_y = offset_y;
  copy_text_to_text_buffer(text);

  BaseType_t err = xTaskCreatePinnedToCore(ePaper_1line_at_task,		// function
					   "ePaper_1line_at",			// name
					   2000,				// stack size
					   &text_descriptor,			// task input parameter
					   MONOCHROME_PRIORITY,			// task priority
					   &ePaper_1line_at_handle,		// task handle
					   0);					// core 0
  if(err != pdPASS) {
    MENU.out(err);
    MENU.space();
    MENU.error_ln(F("ePaper_1line_at_task"));
  }
} // multicore_ePaper_1line_at()

void MC_print_1line_at(int16_t row, char* text, int16_t offset_y=0) {
  multicore_ePaper_1line_at(row, text, offset_y);
}

#else
void MC_print_1line_at(int16_t row, char* text, int16_t offset_y=0) {
  ePaper_print_1line_at(row, text, offset_y);
}
#endif


void ePaper_print_str(char* text) {	// unused
#if defined DEBUG_ePAPER
  MENU.out(F("DEBUG_ePAPER\tePaper_print_str()\t"));
  MENU.outln(text);
#endif

  // uint16_t font_yAdvance = FreeMonoBold9pt7b.yAdvance;
  ePaper.setRotation(1);
  ePaper.setFont(&FreeMonoBold9pt7b);
  ePaper.setTextColor(GxEPD_BLACK);

  ePaper.firstPage();
  do
  {
    ePaper.print(text);
  }
  while (ePaper.nextPage());
} // ePaper_print_str()


void try_ePaper_fix() {
  MENU.ln();
  MENU.error_ln(F("\tTODO: IMPLEMENT\ttry_ePaper_fix()"));
  //~GxEPD2_BW<GxEPD2_213_B73, GxEPD2_213_B73::HEIGHT>();
  //~GxEPD2_BW();	// destructor ?
  MENU.out(F("before:\t"));
  MENU.print_free_RAM();
  MENU.ln();

  MENU.outln(F("setup_LILYGO_ePaper()"));
  setup_LILYGO_ePaper();

  MENU.out(F("after:\t"));
  MENU.print_free_RAM();
  MENU.ln();
  delay(1000);	// TODO: ?

  ePaper_show_program_version();	// just as a test
  delay(250);	// TODO: ?
} // try_ePaper_fix()

void inline try_monochrome_fix() {
  try_ePaper_fix();
}

#if defined DEBUG_ePAPER
  #include "ePaper_debugging.h"
#endif

void ePaper_BIG_or_multiline(int16_t row, char* text) {
  int16_t col=0;
#if defined DEBUG_ePAPER
  MENU.out(F("DEBUG_ePAPER\tePaper_print_str()\t"));
  MENU.out(row);
  MENU.tab();
  MENU.outln(text);
#endif

  if(strlen(text) <= 18)
    set_used_font(&FreeMonoBold12pt7b);	// BIG
  else
    set_used_font(&FreeMonoBold9pt7b);	// normal
  ePaper.setTextColor(GxEPD_BLACK);
  ePaper_print_at(col/*0*/, row, text);
  ePaper.display(true);
} // ePaper_BIG_or_multiline()
