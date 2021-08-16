/*
  ePaper_GxEPD2.h
  see: GxEPD2/examples/GxEPD2_GFX_Example
*/
#if ! defined GXEPD2_EPAPER_H


//#define DEBUG_ePAPER
#define MC_DELAY_MS	10	// delay MC_mux lock release	// TODO: test&trimm	maybe obsolete?
#define USE_MANY_FONTS		// uses some more program storage space


#include <GxEPD2_BW.h>		// tested versions 1.2.13   1.3.4
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


#if defined HAS_ePaper290_on_PICO_KIT
/*
  ESP32-PICO-KIT_V4.1 connections:

  BUSY	4
  RST	9
  DC	10
  CS	5
  CLK	18
  DIN	23
  GND	GND
  VCC	3.3V
*/
  GxEPD2_BW<GxEPD2_290, GxEPD2_290::HEIGHT> ePaper(GxEPD2_290(/*CS*/ 5, /*DC*/ 10, /*RST*/ 9, /*BUSY*/ 4));

#elif defined HAS_ePaper290_on_DEV_KIT
/*
  ESP32 dev kit connections:

  BUSY	4
  RST	16 (UART2 RCV)
  DC	17 (UART2 TX)
  CS	5
  CLK	18
  DIN	23
  GND	GND
  VCC	3.3V
*/
 GxEPD2_BW<GxEPD2_290, GxEPD2_290::HEIGHT> ePaper(GxEPD2_290(/*CS*/ 5, /*DC*/ 17, /*RST*/ 16, /*BUSY*/ 4));

#elif defined BOARD_LILYGO_T5
 // GDEH0213B73
 GxEPD2_BW<GxEPD2_213_B73, GxEPD2_213_B73::HEIGHT> ePaper(GxEPD2_213_B73(/*CS=5*/ SS, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4));

#else
  #error no ePaper display defined for GxEPD2
#endif


#if defined MULTICORE_DISPLAY
  #include "multicore_display_common.h"
#endif // MULTICORE_DISPLAY


void ePaper_infos() {
  MENU.outln(F("ePaper_infos()"));

  MENU.out(F("height\t"));
  MENU.out(ePaper.height());
  MENU.out(F("\twidth\t"));
  MENU.out(ePaper.width());
  MENU.out(F("\trotation\t"));
  MENU.out(ePaper.getRotation());
  MENU.out(F("\tFastPartialUpdate "));
  if(ePaper.epd2.hasFastPartialUpdate)
    MENU.outln(F("yes"));
  else
    MENU.outln(F("no"));

  int rotation_was=ePaper.getRotation();
  for(int rotation=0; rotation<4; rotation++) {
    ePaper.setRotation(rotation);
    MENU.out(F("rotation("));
    MENU.out(rotation);			//	0	1	2	3
    MENU.out(F(")\tpage hight "));
    MENU.out(ePaper.pageHeight());	//	250	250	250	250
    MENU.out(F("\twidth "));
    MENU.outln(ePaper.width());		//	128	250	128	250
  }
  ePaper.setRotation(rotation_was);

  MENU.ln();
} // ePaper_infos()


// fonts:
//GFXfont* tiny_font_p=	(GFXfont*) &Picopixel;
GFXfont* small_font_p=	(GFXfont*) NULL;
GFXfont* medium_font_p=	(GFXfont*) &FreeSansBold9pt7b;		// /* mono */ &FreeMonoBold9pt7b
GFXfont* big_font_p=	(GFXfont*) &FreeSansBold12pt7b;		// /* mono */ &FreeMonoBold12pt7b

GFXfont* used_font_p = (GFXfont*) &FreeMonoBold9pt7b;		// /* mono */ &FreeMonoBold9pt7b
uint8_t used_font_x=11;	// TODO: FIXME: ################
uint8_t used_font_yAdvance;
uint8_t font_linlen=22;	// TODO: FIXME: ################

//bool used_font_is_monospace=false;	// TODO: use?
//bool used_font_is_bold=false;		// TODO: use?
//bool used_font_is_light=false;	// TODO: use?


void set_used_font(const GFXfont* font_p) {
#if defined DEBUG_ePAPER
  MENU.out(F("DEBUG_ePAPER\tset_used_font()"));
#endif
  if(font_p == &FreeMonoBold9pt7b) {
#if defined DEBUG_ePAPER
    MENU.out(F("\tFreeMonoBold9pt7b"));
#endif
    used_font_x = 250/22;	// 11
    font_linlen=22;	// TODO: FIXME: ################
    //used_font_is_monospace=true;
    //used_font_is_bold=true;
    //used_font_is_light=false;

  } else if(font_p == &FreeSansBold9pt7b) {
#if defined DEBUG_ePAPER
    MENU.out(F("\tFreeSansBold9pt7b"));
#endif
    //used_font_x = 250/22;	// 11	*NOT* a fixed width font!
    font_linlen=33;	// TODO: FIXME: ################
    //used_font_is_monospace=false;
    //used_font_is_bold=true;
    //used_font_is_light=false;

  } else if(font_p == &FreeMonoBold12pt7b) {
#if defined DEBUG_ePAPER
    MENU.out(F("\tFreeMonoBold12pt7b"));
#endif
    used_font_x = 14;		// ~250/18;
    font_linlen=18;	// TODO: FIXME: ################
    //used_font_is_monospace=true;
    //used_font_is_bold=true;
    //used_font_is_light=false;

  } else if(font_p == &FreeSansBold12pt7b) {
#if defined DEBUG_ePAPER
    MENU.out(F("\tFreeSansBold12pt7b"));
#endif
    //used_font_x = 14;		// ~250/18;	*NOT* a fixed width font!
    font_linlen=33;		//		*NOT* a fixed width font!
    //used_font_is_monospace=false;
    //used_font_is_bold=true;
    //used_font_is_light=false;

  } else {
    MENU.error_ln(F("unknown font size"));
  }

#if defined DEBUG_ePAPER
  MENU.out(F("\tyAdvance "));
  MENU.out(font_p->yAdvance);
  MENU.out(F("\t rows "));
  MENU.outln((float) ePaper.height() / (float) font_p->yAdvance);
#endif

  ePaper.setFont(font_p);
  used_font_p = (GFXfont*) font_p;
  used_font_yAdvance = used_font_p->yAdvance;
} // set_used_font()
#undef DEBUG_ePAPER


int16_t col2x(int16_t col) {	// *do* call set_used_font() before using that
  return col*used_font_x;
}

int16_t row2y(int16_t row) {	// *do* call set_used_font() before using that
  return (row+1)*used_font_yAdvance;
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

  ePaper.fillRect(x, y - used_font_yAdvance + 4, strlen(text)*used_font_x, used_font_yAdvance, GxEPD_WHITE);
  ePaper.setCursor(x, y);
  ePaper.print(text);
  ePaper.display(true);
} // ePaper_print_at()

#if defined MULTICORE_DISPLAY
TaskHandle_t ePaper_print_at_handle;

void ePaper_print_at_task(void* data_) {
  print_descrpt_t* data = (print_descrpt_t*) data_;

  xSemaphoreTake(MC_mux, portMAX_DELAY);
  ePaper_print_at(data->col, data->row, data->text, data->offset_y);
  free_text_buffer(data);

  vTaskDelay(MC_DELAY_MS / portTICK_PERIOD_MS);
  xSemaphoreGive(MC_mux);
  vTaskDelete(NULL);
}

void multicore_ePaper_print_at(int16_t col, int16_t row, char* text, int16_t offset_y=0) {	// create and start a one shot task
  print_descrpt_t* txt_descript_p = (print_descrpt_t*) malloc(sizeof(print_descrpt_t));
  if(txt_descript_p == NULL) {
    MENU.error_ln(F("txt_descript malloc()"));
    return;	// ERROR
  }
  copy_text_to_text_buffer(text, txt_descript_p);

  txt_descript_p->col = col;
  txt_descript_p->row = row;
  txt_descript_p->offset_y = offset_y;

  BaseType_t err = xTaskCreatePinnedToCore(ePaper_print_at_task,		// function
					   "ePaper_print_at",			// name
					   2000,				// stack size
					   txt_descript_p,			// task input parameter
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
#if defined DEBUG_ePAPER
  MENU.outln(F("DEBUG_ePAPER\tMC_print_at()"));
#endif
  multicore_ePaper_print_at(col, row, text, offset_y);
}

#else
void MC_print_at(int16_t col, int16_t row, char* text, int16_t offset_y=0) {
  ePaper_print_at(col, row, text, offset_y);
}
#endif


void MC_printBIG_at(int16_t col, int16_t row, char* text, int16_t offset_y=0) {
#if defined DEBUG_ePAPER
  MENU.outln(F("DEBUG_ePAPER\tMC_printBIG_at()"));
#endif

  xSemaphoreTake(MC_mux, portMAX_DELAY);
  set_used_font(big_font_p);
  // vTaskDelay(MC_DELAY_MS / portTICK_PERIOD_MS);	// hope we don*t need that...
  xSemaphoreGive(MC_mux);

  MC_print_at(col, row, text, offset_y);
} // MC_printBIG_at()


void monochrome_clear() {
#if defined  DEBUG_ePAPER
  MENU.outln(F("DEBUG_ePAPER\tmonochrome_clear()"));
#endif

  xSemaphoreTake(MC_mux, portMAX_DELAY);
  ePaper.setFullWindow();
  ePaper.fillScreen(GxEPD_WHITE);
  ePaper.display(true);

  vTaskDelay(MC_DELAY_MS / portTICK_PERIOD_MS);
  xSemaphoreGive(MC_mux);
} // monochrome_clear()


void setup_ePaper_GxEPD2() {
  MENU.outln(F("setup_ePaper_GxEPD2()"));

  if(MC_mux == NULL)
    MC_mux = xSemaphoreCreateMutex();

  //ePaper.init(500000);	// debug baudrate
  ePaper.init(0);		// no debugging

  int rotation=1;		// TODO: rotation should go into HARDWARE
  ePaper.setRotation(rotation);	// TODO: rotation should go into HARDWARE

  set_used_font(used_font_p);

  delay(1000);			// TODO: test, maybe REMOVE?:
} // setup_ePaper_GxEPD2()


void inline hw_display_setup() {
  setup_ePaper_GxEPD2();

#if defined DEBUG_ePAPER
//   // little helpers while implementing, TODO: REMOVE?:
  ePaper_infos();
#endif

  delay(1500);
  /*	// TODO: TESTING ################	deactivated for a test
  extern void ePaper_show_program_version();
  MC_do_on_other_core(&ePaper_show_program_version);
  delay(1000);			// TODO: test, maybe REMOVE?:
  */
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

  ePaper.fillRect(x, y - used_font_yAdvance + 4, ePaper.width(), used_font_yAdvance, GxEPD_WHITE);	// TODO test&trimm: -4
  ePaper.setCursor(x, y);
  ePaper.print(text);
  ePaper.display(true);
} // ePaper_print_1line_at()

#if defined MULTICORE_DISPLAY
TaskHandle_t ePaper_1line_at_handle;

void ePaper_1line_at_task(void* data_) {
  print_descrpt_t* data = (print_descrpt_t*) data_;

  xSemaphoreTake(MC_mux, portMAX_DELAY);
  // set_used_font(used_font_p);
  ePaper_print_1line_at(data->row, data->text, data->offset_y);
  free_text_buffer(data);

  vTaskDelay(MC_DELAY_MS / portTICK_PERIOD_MS);
  xSemaphoreGive(MC_mux);
  vTaskDelete(NULL);
}

void multicore_ePaper_1line_at(int16_t row, char* text, int16_t offset_y) {	// create and start a one shot task
  print_descrpt_t* txt_descrpt_p = (print_descrpt_t*) malloc(sizeof(print_descrpt_t));
  if(txt_descrpt_p == NULL) {
    MENU.error_ln(F("txt_descript malloc()"));
    return;	// ERROR
  }
  copy_text_to_text_buffer(text, txt_descrpt_p);
  txt_descrpt_p->row = row;
  txt_descrpt_p->offset_y = offset_y;

  BaseType_t err = xTaskCreatePinnedToCore(ePaper_1line_at_task,		// function
					   "ePaper_1line_at",			// name
					   2000,				// stack size
					   txt_descrpt_p,			// task input parameter
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
#if defined DEBUG_ePAPER
  MENU.out(F("DEBUG_ePAPER\tMC_print_1line_at()\t"));
  MENU.out(row);
  MENU.tab();
  MENU.outln(text);
#endif
  multicore_ePaper_1line_at(row, text, offset_y);
} // MC_print_1line_at()

#else
void MC_print_1line_at(int16_t row, char* text, int16_t offset_y=0) {
  ePaper_print_1line_at(row, text, offset_y);
}
#endif


void ePaper_print_str(char* text) {	// unused?
#if defined DEBUG_ePAPER
  MENU.out(F("DEBUG_ePAPER\tePaper_print_str()\t"));
  MENU.outln(text);
#endif

  ePaper.setRotation(1);
  ePaper.setTextColor(GxEPD_BLACK);

  ePaper.firstPage();
  do
  {
    ePaper.print(text);
  }
  while (ePaper.nextPage());
} // ePaper_print_str()


void ePaper_BIG_or_multiline(int16_t row, char* text) {	// unused?
#if defined DEBUG_ePAPER
  MENU.out(F("DEBUG_ePAPER\tePaper_print_str()\t"));
  MENU.out(row);
  MENU.tab();
  MENU.outln(text);
#endif

  xSemaphoreTake(MC_mux, portMAX_DELAY);	// TODO: could delay application core

  int16_t col=0;

  if(strlen(text) <= 18)
    set_used_font(big_font_p);		// BIG font
  else
    set_used_font(medium_font_p);	// normal size font
  ePaper.setTextColor(GxEPD_BLACK);
  ePaper_print_at(col/*0*/, row, text);
  ePaper.display(true);

  xSemaphoreGive(MC_mux);
} // ePaper_BIG_or_multiline()

#if defined DEBUG_ePAPER || true
  #include "ePaper_debugging.h"
#endif

#define GXEPD2_EPAPER_H
#endif
