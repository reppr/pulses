/*
  ePaper_debugging.h	DEBUGGING only
*/

#define COMPILE_FONT_TEST

void ePaper_line_matrix() {
  MENU.outln(F("\nDEBUG_ePAPER\tePaper_line_matrix()"));

  ePaper.setFullWindow();
  ePaper.setTextColor(GxEPD_BLACK);
  ePaper.setFont(used_font_p);

  char txt[24];
  char* format_is = F("%i.%s");
  int rows = ePaper.height() / used_font_p->yAdvance;

  ePaper.firstPage();
  do
  {
    ePaper.fillScreen(GxEPD_WHITE);
    ePaper.setCursor(0, 0);
    ePaper.println();

    for(int i=0; i<rows; i++) {
      //      snprintf(txt, font_linlen+1, format_is, i, "23456789012345678901234567890");
      //      snprintf(txt, font_linlen+1, format_is, i, "234567890123456789012");
      snprintf(txt, font_linlen+1, format_is, i, "2345678901234");
      ePaper.println(txt);
    }
  }
  while (ePaper.nextPage());
  delay(750);
} // void ePaper_line_matrix()


void ePaper_bounds() {
  MENU.outln(F("\nDEBUG_ePAPER\tePaper_bounds"));

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
  MENU.outln(F("\nDEBUG_ePAPER\tePaper_bounds()"));
} // ePaper_bounds()


#if defined COMPILE_FONT_TEST
void ePaper_font_test() {
  MENU.outln(F("ePaper_font_test()"));

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
} // ePaper_font_test()
#endif // COMPILE_FONT_TEST
