/*
  men_MuD.h
  temporary test menu UI during development
*/


#if defined HAS_ePaper
MC_print_1line_at(1, "aha!");
MC_display_message("row 3    message from Alibaba");
  break;

set_used_font(&FreeMonoBold12pt7b);
ePaper_line_matrix();
delay(1000);
MENU.print_free_RAM(); MENU.ln();
MC_print_at(1,1, "MALLOC() FREE()");
MENU.print_free_RAM(); MENU.ln();
delay(1000);
MC_print_1line_at(4, "malloc() free()");
MENU.print_free_RAM(); MENU.ln();
delay(1000);
MC_print_1line_at(0, "   at the TOP");
MENU.print_free_RAM(); MENU.ln();
delay(4000);
break;

delay(1000);
set_used_font(&FreeMonoBold9pt7b);
//monochrome_clear();
ePaper_line_matrix();
delay(1000);
ePaper_BIG_or_multiline(2, "HALLO DADA");
delay(1000);
break;

delay(1000);
set_used_font(&FreeMonoBold9pt7b);
monochrome_clear();
delay(1000);
MC_print_at(0,0,"DADAsveryverysuperlongsuperduperduperlongLinewithout a break or something else continues vorever DADAsveryverysuperlongsuperduperduperlongLinewithout a break or something else continues vorever");
delay(1000);
break;

set_used_font(&FreeMonoBold9pt7b);
ePaper_line_matrix();
MC_print_at(1,2," DADA ");
delay(1000);
ePaper_print_1line_at(4, "HumbuKumbu 4 ");
delay(1000);
MC_print_1line_at(1, "LINE 1");
delay(2000);

monochrome_clear();
delay(3000);

set_used_font(&FreeMonoBold12pt7b);
ePaper_line_matrix();
ePaper_print_1line_at(4, "HumbuKumbu 4 ");
delay(4000);
break;

set_used_font(&FreeMonoBold9pt7b);
ePaper_line_matrix();
ePaper_print_at(3, 2, " abcd_");
ePaper_print_at(0, 0, "NULL");
ePaper_print_at(5, 4, "- - ");	// ePaper.getTextBounds() takes this too literally ;)
delay(4000);
break;

set_used_font(&FreeMonoBold12pt7b);
ePaper_line_matrix();
ePaper_print_at(16, 4, "X");	// why is @(17,4,"X") not possible?
delay(4000);
//break;

set_used_font(&FreeMonoBold9pt7b);
ePaper_line_matrix();
ePaper_print_at(21, 6, "X");
delay(4000);
break;
/*
ePaper.setCursor(50,100);
ePaper.print("%%%%%");
ePaper.display();
delay(6000);
*/

//ePaper_basic_parameters(); break;

//ePaper_line_testing(); break;

//ePaper_print_1line(6*128/7, " *");

//setup_LILYGO_ePaper(); break;

//set_used_font(&FreeMonoBold9pt7b);
//ePaper_line_matrix();
//delay(500);
//ePaper_print_1line(6*128/7, "HumbuKumbu 6 ");

////for(int i=0; i<8; i++) {
//for(int i=0; i<5; i++) {
//  delay(2000);
//  ePaper_print_1line(i*128/7, "->XY5");
//  MENU.out("line ");
//  MENU.outln(i);
// }

//delay(1500);
//ePaper_print_str("DADA here i am ");
//delay(500);
//ePaper.println();
//delay(100);
//ePaper_print_str("here, there&everywhere");
//delay(250);
break;
//ePaper_print_1line(7*128/7, " ."); break;
//ePaper_print_1line(7*128/7, "Y."); break;

// ePaper_show_tuning(); delay(5000);

 #if defined COMPILE_FONTTEST
  ePaper_fonttest();
 #endif

if(MENU.check_next('D')) {	// DD
  if(MENU.check_next('D'))	// DDD
    do_on_other_core(&ePaper_basic_parameters);	// DDD
  else
    ePaper_basic_parameters();	// DD
} else
  MENU.play_KB_macro(F("187 P *6 N"));	// try out MELODY mode

#else
  MENU.play_KB_macro(F("187 P *6 N"));	// try out MELODY mode
#endif // HAS_ePaper or not

//#include "SPEED-TESTS.h"

//display_icode(melody0, sizeof(melody0));

#if defined USE_ADS1115_AT_ADDR
//ads1115_first_test_A0();
//ads1115_first_test_diff_A2_A3();
#endif

//show_metric_cents_list();	// TODO: move to Tuning hierarchy

//	{
//	  MENU.out(metric_mnemonic);
//	  char s[] = {0, 0};
//	  s[0] = scale_symbol_char();
//	  MENU.outln(s);
//	}


//MENU.out(F("normalised_pitch "));	// TODO: move to Tuning hierarchy
//MENU.out(get_normalised_pitch(), 6);
//MENU.ln();


//MENU.outln((int) esp_timer_get_time());

//  show_morse_event_buffer();

    //MENU.outln(F("HARDWARE configuration"));
    //show_hardware_conf(&HARDWARE);

    //display_peer_ID_list();
    //random_RGB_string(MENU.numeric_input(4));

    // random_HSV_LED_string();

    /*
    {
      int i=0;
      strand_t * strands [STRANDCNT];
      strands[i] = &STRANDS[i];

      MENU.out("LED STRING ");
      ulong t0 = micros();
      int s = digitalLeds_drawPixels(strands, 1);
      ulong d = micros() - t0;
      MENU.out(s);
      MENU.tab();
      MENU.outln(d);

//      randomStrands(strands, i, 100, 3000);
//      scanners(strands, i, 0, 2000);
//      scanners(strands, i, 0, 2000);
//      rainbows(strands, i, 0, 4000);

    }
    */
