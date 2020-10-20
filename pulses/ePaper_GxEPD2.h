/*
  ePaper_GxEPD2.h
  see: GxEPD2/examples/GxEPD2_GFX_Example
*/

#if ! defined GXEPD2_EPAPER_H

//#define DEBUG_ePAPER
#define USE_MC_SEMAPHORE
#define MC_DELAY_MS	10	// delay MC_mux lock release	// TODO: test&trimm	maybe obsolete?    ################
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

#define LIN_BUF_MAX	42	// temporal local line buffers only

#if defined HAS_ePaper290_on_DEV_KIT
/*
  connections:

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
  #include <freertos/task.h>

  // testing RTOS task priority for monochrome display routines:
  #define MONOCHROME_PRIORITY	0	// seems best
  //#define MONOCHROME_PRIORITY	1	// was 0
  //#define MONOCHROME_PRIORITY	2	// was 0

/*
  // MUTEX for ePaper printing (or just wait a bit... ;)
  portMUX_TYPE display_MUX = portMUX_INITIALIZER_UNLOCKED;
  // SemaphoreHandle_t display_MUTEX = xSemaphoreCreateMutex();
*/

  #define MONOCHROME_TEXT_BUFFER_SIZE	156	// (7*22 +2)	// TODO: more versatile implementation
  char monochrome_text_buffer[MONOCHROME_TEXT_BUFFER_SIZE] = {0};

  typedef struct print_descrpt_t {
    int16_t col=0;
    int16_t row=0;
    int16_t offset_y=0;
    //uint8_t size=0;
    //uint8_t colour=0;
    //bool inverted=false;
    char* text = NULL;
    //char* text = (char*) &monochrome_text_buffer;
  } print_descrpt_t;

  bool /*error*/ copy_text_to_text_buffer(char* text, print_descrpt_t* txt_descr_p) {
    char* txt_p = (char*) malloc(strlen(text) + 1);
    if(txt_p == NULL) {
      goto malloc_error;
    }
    txt_descr_p->text = txt_p;

    char c;
    for(int i=0; ; i++) {
      c = txt_descr_p->text[i] = text[i];
      if(c==0)
	break;
    }
    return false;	// OK

  malloc_error:
    MENU.error_ln(F("text buffer malloc()"));
    return true;	// ERROR
  } // copy_text_to_text_buffer()

  void free_text_buffer(print_descrpt_t* txt_descr_p) {
  #if defined DEBUG_ePAPER
    MENU.out("DEBUG_ePAPER\tfree_text_buffer() ");
    MENU.ln();	//    MENU.print_free_RAM(); MENU.tab();	// deactivated	***I HAD CRASHES HERE***
  #endif
    free((*txt_descr_p).text);
    free(txt_descr_p);
  //#if defined DEBUG_ePAPER
  //  MENU.print_free_RAM(); MENU.ln();			// deactivated both
  //#endif
  }


  #if defined USE_MC_SEMAPHORE
    SemaphoreHandle_t MC_mux = NULL;
  #endif


  // MC_do_on_other_core() version with MC_mux and MC_DELAY_MS
  TaskHandle_t MC_do_on_other_core_handle;

  void MC_do_on_other_core_task(void* function_p) {
    void (*fp)() = (void (*)()) function_p;
    #if defined USE_MC_SEMAPHORE
      xSemaphoreTake(MC_mux, portMAX_DELAY);
    #endif

    (*fp)();

    vTaskDelay(MC_DELAY_MS / portTICK_PERIOD_MS);
    #if defined USE_MC_SEMAPHORE
      xSemaphoreGive(MC_mux);
    #endif
    vTaskDelete(NULL);
  }

  void MC_do_on_other_core(void (*function_p)()) {	// create and do one shot task
    BaseType_t err = xTaskCreatePinnedToCore(MC_do_on_other_core_task,	// function
					   "other_fun",			// name
					   4000,				// stack size
					   (void*) function_p,			// task input parameter
					   0,					// task priority
					   &MC_do_on_other_core_handle,		// task handle
					   0);					// core 0
    if(err != pdPASS) {
      MENU.out(err);
      MENU.space();
      MENU.error_ln(F("MC_do_on_other_core"));
    }
  }
#endif MULTICORE_DISPLAY

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


void setup_ePaper_GxEPD2() {
  MENU.outln(F("setup_ePaper_GxEPD2()"));

  #if defined USE_MC_SEMAPHORE
  if(MC_mux == NULL)
    MC_mux = xSemaphoreCreateMutex();
  #endif

  //ePaper.init(500000);	// debug baudrate
  ePaper.init(0);		// no debugging

  int rotation=1;		// TODO: rotation should go into HARDWARE
  ePaper.setRotation(rotation);	// TODO: rotation should go into HARDWARE


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


// GFXfont* small_font_p = (GFXfont*) NULL;
// GFXfont* default_font_p = (GFXfont*) &FreeMonoBold9pt7b;
// GFXfont* BIG_font_p = (GFXfont*) &FreeMonoBold12pt7b;

GFXfont* used_font_p = (GFXfont*) &FreeMonoBold9pt7b;;

//GFXfont* small_font_p = (GFXfont*) NULL;
GFXfont* middle_font_p = (GFXfont*) &FreeMonoBold9pt7b;
GFXfont* big_font_p = (GFXfont*) &FreeMonoBold12pt7b;

uint8_t used_font_x=11;
uint8_t used_font_y=128/7;	// 18
uint8_t font_linlen=22;


void set_used_font(const GFXfont* font_p) {
#if defined DEBUG_ePAPER
  MENU.outln(F("DEBUG_ePAPER\tset_used_font()"));
#endif
  if(font_p == &FreeMonoBold9pt7b) {
    used_font_x = 250/22;	// 11
    used_font_y = 128/7;	// 18
    font_linlen=22;

  } else if(font_p == &FreeSansBold9pt7b) {
    //used_font_x = 250/22;	// 11	*NOT* a fixed width font!
    used_font_y = 23;	//24	128/7;	// 18	*NOT* a fixed width font!
    font_linlen=33;

  } else if(font_p == &FreeMonoBold12pt7b) {
    used_font_x = 14;		// ~250/18;
    used_font_y = 24;		// ~128/5 -1;
    font_linlen=18;

  } else if(font_p == &FreeSansBold12pt7b) {
    //used_font_x = 14;		// ~250/18;
    used_font_y = 24;		// *NOT* a fixed width font!
    font_linlen=33;		// *NOT* a fixed width font!

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
  print_descrpt_t* data = (print_descrpt_t*) data_;
  #if defined USE_MC_SEMAPHORE
    xSemaphoreTake(MC_mux, portMAX_DELAY);
  #endif
  ePaper_print_at(data->col, data->row, data->text, data->offset_y);
  free_text_buffer(data);

  vTaskDelay(MC_DELAY_MS / portTICK_PERIOD_MS);
  #if defined USE_MC_SEMAPHORE
    xSemaphoreGive(MC_mux);
  #endif
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

  #if defined USE_MC_SEMAPHORE
    xSemaphoreTake(MC_mux, portMAX_DELAY);
  #endif
  set_used_font(&FreeMonoBold12pt7b);

  vTaskDelay(MC_DELAY_MS / portTICK_PERIOD_MS);
  #if defined USE_MC_SEMAPHORE
    xSemaphoreGive(MC_mux);
  #endif
  MC_print_at(col, row, text, offset_y);
}

void monochrome_clear() {
#if defined  DEBUG_ePAPER
  MENU.outln(F("DEBUG_ePAPER\tmonochrome_clear()"));
#endif

  #if defined USE_MC_SEMAPHORE
    xSemaphoreTake(MC_mux, portMAX_DELAY);
  #endif
  ePaper.setFullWindow();
  ePaper.fillScreen(GxEPD_WHITE);
  ePaper.display(true);

  vTaskDelay(MC_DELAY_MS / portTICK_PERIOD_MS);
  #if defined USE_MC_SEMAPHORE
    xSemaphoreGive(MC_mux);
  #endif
} // monochrome_clear()


void ePaper_basic_parameters() {
#if defined  DEBUG_ePAPER
  MENU.outln(F("DEBUG_ePAPER\tePaper_basic_parameters() new"));
#endif

  ePaper.setFullWindow();
  ePaper.fillScreen(GxEPD_WHITE);
  ePaper.setTextColor(GxEPD_BLACK);

  char txt[LIN_BUF_MAX];
  char* format_IstrI = F("|%s|");
  char* format2s = F("%s  %s");
  char* format_s = F("%s");
  char* format_is = F("%i %s");

  char* fmt_1st_row = F("%c %i  |%s|");
  //char* fmt_1st_row = F("%c |%s| P%i");
  char* fmt_sync = F("S=%i");
  char* fmt_synstack = F(" |%i");
  char* fmt_basepulse = F(" p[%i]");

  ePaper.firstPage();
  do
  {
    ePaper.fillScreen(GxEPD_WHITE);
    //set_used_font(&FreeMonoBold12pt7b);	// was: mono
    set_used_font(&FreeSansBold12pt7b);
    ePaper.setCursor(0, 0);
    ePaper.println();

    extern char run_state_symbol();
    snprintf(txt, font_linlen+1, fmt_1st_row, run_state_symbol(), musicBoxConf.preset, my_IDENTITY.preName);
    ePaper.print(txt);
    // monochrome_show_subcycle_octave();

    //ePaper.println();	// before???	bigger ssstep
    //set_used_font(&FreeMonoBold9pt7b);	// was: mono
    set_used_font(&FreeSansBold9pt7b);
    ePaper.println(); // or after???  smaller step

    extern char* metric_mnemonic;
    snprintf(txt, LIN_BUF_MAX, format2s, selected_name(SCALES), metric_mnemonic);
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
} // ePaper_basic_parameters()


void inline MC_show_musicBox_parameters() {
#if defined DEBUG_ePAPER
  MENU.outln(F("DEBUG_ePAPER\tMC_show_musicBox_parameters()"));
#endif
  MC_do_on_other_core(&ePaper_basic_parameters);
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
    ePaper.setCursor(0, 0);
#if defined USE_MANY_FONTS
    ePaper.setFont(&FreeSans12pt7b);
#else
    ePaper.setFont(&FreeSansBold12pt7b);
#endif
    ePaper.println();

    snprintf(txt, LIN_BUF_MAX, format_s, F(STRINGIFY(PROGRAM_VERSION)));
    ePaper.println(txt);

#if defined USE_MANY_FONTS
    ePaper.setFont(&FreeSans9pt7b);
#else
    ePaper.setFont(&FreeSansBold9pt7b);
#endif
#if defined PROGRAM_SUB_VERSION
    snprintf(txt, LIN_BUF_MAX, format_s, F(STRINGIFY(PROGRAM_SUB_VERSION)));
    ePaper.println(txt);
#endif
    ePaper.println();

#if defined USE_MANY_FONTS
    ePaper.setFont(&FreeSans12pt7b);
#else
    ePaper.setFont(&FreeSansBold12pt7b);
#endif
    snprintf(txt, LIN_BUF_MAX, F("|%s|  (%i)"), my_IDENTITY.preName, my_IDENTITY.esp_now_time_slice);
    ePaper.println(txt);
    //ePaper.println();

    // TODO: option mnemonics?
  }
  while (ePaper.nextPage());
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
    //ePaper.setFont(&FreeSansBold9pt7b);
    ePaper.setFont(&FreeSansBold12pt7b);
    ePaper.fillScreen(GxEPD_WHITE);
    ePaper.setCursor(0, 0);
    ePaper.println();

    snprintf(txt, LIN_BUF_MAX, format_s, selected_name(SCALES));
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
#if defined DEBUG_ePAPER
  MENU.outln(F("DEBUG_ePAPER\tMC_show_tuning()"));
#endif
  MC_do_on_other_core(&ePaper_show_tuning);
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
  print_descrpt_t* data = (print_descrpt_t*) data_;
  #if defined USE_MC_SEMAPHORE
    xSemaphoreTake(MC_mux, portMAX_DELAY);
  #endif
  set_used_font(&FreeSansBold9pt7b);
  ePaper_print_1line_at(data->row, data->text, data->offset_y);
  free_text_buffer(data);

  vTaskDelay(MC_DELAY_MS / portTICK_PERIOD_MS);
  #if defined USE_MC_SEMAPHORE
    xSemaphoreGive(MC_mux);
  #endif
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
  delay(2000); MC_show_musicBox_parameters();	// just as a test
  delay(1500);	// TODO: ?
} // try_ePaper_fix()

void inline try_monochrome_fix() {
  try_ePaper_fix();
}


void ePaper_BIG_or_multiline(int16_t row, char* text) {	// unused?
#if defined DEBUG_ePAPER
  MENU.out(F("DEBUG_ePAPER\tePaper_print_str()\t"));
  MENU.out(row);
  MENU.tab();
  MENU.outln(text);
#endif

  #if defined USE_MC_SEMAPHORE
    xSemaphoreTake(MC_mux, portMAX_DELAY);
  #endif

  int16_t col=0;

  if(strlen(text) <= 18)
    set_used_font(&FreeMonoBold12pt7b);	// BIG
  else
    set_used_font(&FreeMonoBold9pt7b);	// normal
  ePaper.setTextColor(GxEPD_BLACK);
  ePaper_print_at(col/*0*/, row, text);
  ePaper.display(true);

  #if defined USE_MC_SEMAPHORE
    xSemaphoreGive(MC_mux);
  #endif
} // ePaper_BIG_or_multiline()


void inline MC_display_message(char* text) {
#if defined DEBUG_ePAPER
  MENU.out(F("DEBUG_ePAPER\tMC_display_message()\t"));
#endif
  MENU.outln(text);
  set_used_font(&FreeSansBold9pt7b);
  MC_print_1line_at(3, text, /*offset*/ 27);	// bottom line

// TODO: REMOVE: debugging code
//  MC_print_1line_at(0, "row 0", /*offset*/ 27);	// 0	TODO: REMOVE: debugging code
//  MC_print_1line_at(1, "row 1", /*offset*/ 27);	// 1
//  MC_print_1line_at(2, "row 2", /*offset*/ 27);	// 2
} // MC_display_message()


#if defined DEBUG_ePAPER
  #include "ePaper_debugging.h"
#endif

#define GXEPD2_EPAPER_H
#endif
