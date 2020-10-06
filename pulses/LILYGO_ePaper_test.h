/*
  LILYGO_ePaper_test.h
  see: GxEPD2/examples/GxEPD2_GFX_Example
*/

#include <BitmapDisplay.h>

GxEPD2_BW<GxEPD2_213_B73, GxEPD2_213_B73::HEIGHT> display(GxEPD2_213_B73(/*CS=5*/ SS, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4)); // GDEH0213B73

void setup_LILYGO_ePaper() {
  MENU.outln(F("setup_LILYGO_ePaper()"));

  //display.init(500000);	// debug baudrate
  display.init(0);		// no debugging

  display.setRotation(1);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);

  int16_t tbx, tby; uint16_t tbw, tbh;
  display.getTextBounds("123456789ABCDEFGHIKLMN", 0, 0, &tbx, &tby, &tbw, &tbh);	// 22 chars

  // center bounding box by transposition of origin:
  uint16_t x = ((display.width() - tbw) / 2) - tbx;
  uint16_t y = ((display.height() - tbh) / 2) - tby;
  display.setFullWindow();

  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(x, y);
    display.print("RugselBuxelButzlMutzel");	// 22 chars
  }
  while (display.nextPage());
} // setup_LILYGO_ePaper() very FIRST TESTs  

void ePaper_basic_parameters() {
  display.setFullWindow();
  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  //display.setFont(&FreeMonoBold9pt7b);
  char txt[23];

  char* format_IstrI = F("|%s|");
  char* format2s = F("%s  %s");
  char* format_s = F("%s");
  char* format_is = F("%i %s");
  char* format_sync = F("S=%i");

  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(0, 0);
    display.setFont(&FreeMonoBold12pt7b);
    display.println();

    snprintf(txt, 23, format_IstrI, my_IDENTITY.preName);
    display.println(txt);

    display.setFont(&FreeMonoBold9pt7b);
    snprintf(txt, 23, format_is, musicBoxConf.preset, musicBoxConf.name);
    display.println(txt);

    extern char* metric_mnemonic;
    snprintf(txt, 23, format2s, selected_name(SCALES), metric_mnemonic);
    display.println(txt);

#if defined ICODE_INSTEAD_OF_JIFFLES
    snprintf(txt, 23, format_s, selected_name(iCODEs));
#else
    snprintf(txt, 23, format_s, selected_name(JIFFLES));
#endif
    display.println(txt);

    snprintf(txt, 23, format_sync, musicBoxConf.sync);
    display.println(txt);
  }
  while (display.nextPage());
} // ePaper_basic_parameters()
