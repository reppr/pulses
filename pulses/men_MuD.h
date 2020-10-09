/*
  men_MuD.h
  temporary test menu UI during development
*/

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

ePaper_line_matrix();
delay(500);
ePaper_print_1line(6*128/7, "HumbuKumbu 6 ");

//for(int i=0; i<8; i++) {
for(int i=0; i<5; i++) {
    delay(2000);
    ePaper_print_1line(i*128/7, "->XY5");
    MENU.out("line ");
    MENU.outln(i);
 }

delay(1500);
ePaper_print_str("DADA here i am ");
delay(500);
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

//#include "SPEED-TESTS.h"

#if defined USE_MONOCHROME_DISPLAY
MENU.outln(F("TODO: MONOCHROME FIX?"));
// MENU.outln(F("(*u8x8_p).initDisplay();"));
// (*u8x8_p).initDisplay();

/*
MENU.outln(F("monochrome_begin()"));
monochrome_begin();	// is in monochrome_setup() now
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

MENU.outln(F("monochrome_show_musicBox_parameters();"));
monochrome_show_musicBox_parameters();
#endif

//display_icode(melody0, sizeof(melody0));

#if defined MULTICORE_DISPLAY
// MC_show_tuning();
#endif

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
