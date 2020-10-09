/*
  LILYGO_ePaper_test.h
  see: GxEPD2/examples/GxEPD2_GFX_Example
*/

#define COMPILE_FONTTEST
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

  LILYGO_ePaper_infos();

  int rotation=1;		// TODO: rotation should go into HARDWARE
  ePaper.setRotation(rotation);	// TODO: rotation should go into HARDWARE

  extern void ePaper_bounds();
  ePaper_bounds();

  extern void ePaper_show_program_version();
  do_on_other_core(&ePaper_show_program_version);
  delay(1000);
//  ePaper.setFont(&FreeMonoBold9pt7b);
//  ePaper.setTextColor(GxEPD_BLACK);
//
//  int16_t tbx, tby; uint16_t tbw, tbh;
//  ePaper.getTextBounds("123456789ABCDEFGHIKLMN", 0, 0, &tbx, &tby, &tbw, &tbh);	// 22 chars
//
//  // center bounding box by transposition of origin:
//  uint16_t x = ((ePaper.width() - tbw) / 2) - tbx;
//  uint16_t y = ((ePaper.height() - tbh) / 2) - tby;
//  ePaper.setFullWindow();
//
//  ePaper.firstPage();
//  do
//  {
//    ePaper.fillScreen(GxEPD_WHITE);
//    ePaper.setCursor(x, y);
//    ePaper.print("RugselBuxelButzlMutzel");	// 22 chars
//  }
//  while (ePaper.nextPage());
} // setup_LILYGO_ePaper() very FIRST TESTs

void ePaper_basic_parameters() {
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
  ePaper.setFullWindow();
  ePaper.fillScreen(GxEPD_WHITE);
  ePaper.setTextColor(GxEPD_BLACK);

  char txt[23];

//  char* format_IstrI = F("|%s|");
//  char* format2s = F("%s  %s");
  char* format_s = F("%s");
//  char* format_is = F("%i %s");
//  char* format_sync = F("S=%i");

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
  ePaper.setFullWindow();
  ePaper.fillScreen(GxEPD_WHITE);
  ePaper.setTextColor(GxEPD_BLACK);

  char txt[23];

//  char* format_IstrI = F("|%s|");
//  char* format2s = F("%s  %s");
  char* format_s = F("%s");
//  char* format_is = F("%i %s");
//  char* format_sync = F("S=%i");

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


#if defined COMPILE_FONTTEST
void ePaper_fonttest() {
  ePaper.setFullWindow();
  ePaper.fillScreen(GxEPD_WHITE);
  ePaper.setTextColor(GxEPD_BLACK);
  //ePaper.setFont(&FreeMonoBold9pt7b);	// see below
  char txt[23];
  char* format_s = F("%s");
  int wait=10000;

  ePaper.firstPage();
  do
  {
    ePaper.fillScreen(GxEPD_WHITE);
    ePaper.setCursor(0, 0);

    ePaper.setFont(&FreeMonoBold9pt7b);
    ePaper.println();

    snprintf(txt, 23, format_s, "FreeMonoBold9pt7b");
    ePaper.println(txt);

    ePaper.setFont(&FreeMono9pt7b);
    snprintf(txt, 23, format_s, "FreeMono9pt7b");
    ePaper.println(txt);

    ePaper.setFont(&FreeMonoBoldOblique9pt7b);
    snprintf(txt, 23, format_s, "FreeMonoBoldOblique9pt7b");
    ePaper.println(txt);

    ePaper.setFont(&FreeSans9pt7b);
    snprintf(txt, 23, format_s, "FreeSans9pt7b");
    ePaper.println(txt);

    ePaper.setFont(&FreeSansBold9pt7b);
    snprintf(txt, 23, format_s, "FreeSansBold9pt7b");
    ePaper.println(txt);

    ePaper.setFont(&FreeSansOblique9pt7b);
    snprintf(txt, 23, format_s, "FreeSansOblique9pt7b");
    ePaper.println(txt);
  }
  while (ePaper.nextPage());
  delay(wait);

  ePaper.setFullWindow();
  ePaper.fillScreen(GxEPD_WHITE);
  ePaper.setTextColor(GxEPD_BLACK);

  ePaper.firstPage();
  do
  {
    ePaper.fillScreen(GxEPD_WHITE);
    ePaper.setCursor(0, 0);

    ePaper.setFont(&FreeSerif9pt7b);
    ePaper.println();
    snprintf(txt, 23, format_s, "FreeSerif9pt7b");
    ePaper.println(txt);

    ePaper.setFont(&FreeSerifBold9pt7b);
    snprintf(txt, 23, format_s, "FreeSerifBold9pt7b");
    ePaper.println(txt);

    ePaper.setFont(&FreeSerifBoldItalic9pt7b);
    snprintf(txt, 23, format_s, "FreeSerifBoldItalic9pt7b");
    ePaper.println(txt);

    ePaper.setFont(&FreeSerifItalic9pt7b);
    snprintf(txt, 23, format_s, "FreeSerifItalic9pt7b");
    ePaper.println(txt);

    ePaper.setFont(&Org_01);
    snprintf(txt, 23, format_s, "Org_01");
    ePaper.println(txt);

    ePaper.setFont(&Picopixel);
    snprintf(txt, 23, format_s, "Picopixel");
    ePaper.println(txt);
  }
  while (ePaper.nextPage());
  delay(wait);

  ePaper.setFullWindow();
  ePaper.fillScreen(GxEPD_WHITE);
  ePaper.setTextColor(GxEPD_BLACK);

  ePaper.firstPage();
  do
  {
    ePaper.fillScreen(GxEPD_WHITE);
    ePaper.setCursor(0, 0);

    ePaper.setFont(&Org_01);
    ePaper.println();
    snprintf(txt, 23, format_s, "Org_01");
    ePaper.println(txt);
    ePaper.println();
    ePaper.println();

    ePaper.setFont(&Picopixel);
    snprintf(txt, 23, format_s, "Picopixel");
    ePaper.println(txt);
    ePaper.println();
    ePaper.println();

    ePaper.setFont(&Tiny3x3a2pt7b);
    snprintf(txt, 23, format_s, "Tiny3x3a2pt7b");
    ePaper.println(txt);
    ePaper.println();
    ePaper.println();

    ePaper.setFont(&TomThumb);
    snprintf(txt, 23, format_s, "TomThumb");
    ePaper.println(txt);
    ePaper.println();
    ePaper.println();

    ePaper.setFont(&FreeSans9pt7b);
    ePaper.print("erstes ");
    ePaper.print(99);
    ePaper.println(" zweites MonB");

    ePaper.print("erstes ");
    ePaper.print(99);
    ePaper.println(" zweites Sans");

    ePaper.print("erstes ");
    ePaper.print(99);
    ePaper.println(" zweites SansBold");


  }
  while (ePaper.nextPage());
} // ePaper_fonttest()
#endif // COMPILE_FONTTEST


void inline monochrome_setup() {
  setup_LILYGO_ePaper();
}

// void inline MC_setInverseFont() {
//   //ePaper.fillScreen(GxEPD_BLACK);	// does this make sense?
//   ePaper.setTextColor(GxEPD_WITE);
// }
//
// void inline MC_clearInverseFont() {
//   //ePaper.fillScreen(GxEPD_WHITE);	// does this make sense?
//   ePaper.setTextColor(GxEPD_BLACK);
// }

//void ePaper_tests() {
//  ePaper.setFullWindow();
//  //ePaper.fillScreen(GxEPD_WHITE);
//  ePaper.setTextColor(GxEPD_BLACK);
//  ePaper.setFont(&FreeMonoBold12pt7b);
//
//  ePaper.firstPage();
//  do
//  {
//    ePaper.fillScreen(GxEPD_WHITE);
//    ePaper.setCursor(0, 0);
//    ePaper.println();
//  }
//  while (ePaper.nextPage());
//}

void ePaper_bounds() {
  MENU.outln(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");

  int16_t tbx, tby; uint16_t tbw, tbh;
  //ePaper.getTextBounds("123456789ABCDEFGHIKLMN", 0, 0, &tbx, &tby, &tbw, &tbh);	// 22 chars
  char* str;

  ePaper.setFont(&FreeMonoBold9pt7b);
  MENU.outln("FreeMonoBold9pt7b");

  for(int i=0; i<2; i++) {
    str = "M";
    ePaper.getTextBounds(str, 50, 50, &tbx, &tby, &tbw, &tbh);
    MENU.out(str); MENU.tab();
    MENU.out("x, y, w, h\t");
    MENU.out(tbx); MENU.tab();
    MENU.out(tby); MENU.tab();
    MENU.out(tbw); MENU.tab();
    MENU.outln(tbh);

    str = "X";
    ePaper.getTextBounds(str, 50, 50, &tbx, &tby, &tbw, &tbh);
    MENU.out(str); MENU.tab();
    MENU.out("x, y, w, h\t");
    MENU.out(tbx); MENU.tab();
    MENU.out(tby); MENU.tab();
    MENU.out(tbw); MENU.tab();
    MENU.outln(tbh);

    str="1234567890";
    ePaper.getTextBounds(str, 50, 50, &tbx, &tby, &tbw, &tbh);	// 10 chars
    MENU.out(str); MENU.tab();
    MENU.out("x, y, w, h\t");
    MENU.out(tbx); MENU.tab();
    MENU.out(tby); MENU.tab();
    MENU.out(tbw); MENU.tab();
    MENU.outln(tbh);

    ePaper.setFont(&FreeMonoBold12pt7b);
    MENU.outln("FreeMonoBold12pt7b");
  }
  MENU.outln(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
}

void ePaper_line_matrix() {
  MENU.outln(F("ePaper_line_matrix()"));

  ePaper.setFullWindow();
  ePaper.setTextColor(GxEPD_BLACK);
  ePaper.setFont(&FreeMonoBold9pt7b);
  MENU.out(F("FreeMonoBold9pt7b.yAdvance "));
  MENU.outln(FreeMonoBold9pt7b.yAdvance);

  char txt[24];
  char* format_is = F("%i.%s");

  ePaper.firstPage();
  do
  {
    ePaper.fillScreen(GxEPD_WHITE);
    ePaper.setCursor(0, 0);
    ePaper.println();

    for(int i=0; i<7; i++) {
      snprintf(txt, 23, format_is, i, "3456789012345678901234567890");
      ePaper.println(txt);
    }
  }
  while (ePaper.nextPage());
} // void ePaper_line_matrix()

void ePaper_print_1line(uint16_t y, char* text) {
  MENU.out(F("ePaper_print_1line() "));
  MENU.outln(y);

  // uint16_t font_yAdvance = FreeMonoBold9pt7b.yAdvance;
  ePaper.setRotation(1);
  ePaper.setFont(&FreeMonoBold9pt7b);
  ePaper.setTextColor(GxEPD_BLACK);
  ePaper.setPartialWindow(0, y-11, ePaper.width(), 12);

  ePaper.firstPage();
  do
  {
    ePaper.fillScreen(GxEPD_WHITE);  // clear line
    ePaper.setCursor(0, y);
    ePaper.print(text);
  }
  while (ePaper.nextPage());
} // ePaper_print_1line
