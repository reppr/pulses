/*
  men_MuD.h
  temporary test menu UI during development
*/

display_icode(melody0, sizeof(melody0));

//#include "SPEED-TESTS.h"

#if defined USE_MONOCHROME_DISPLAY
MENU.outln(F("TODO: MONOCHROME FIX?"));
// MENU.outln(F("(*u8x8_p).initDisplay();"));
// (*u8x8_p).initDisplay();

/*
MENU.outln(F("monochrome_begin()"));
monochrome_begin();
*/

/*
MENU.outln(F("monochrome <ESC>c"));
(*u8x8_p).print((char) 27);
(*u8x8_p).print('c');

MENU.outln(F("monochrome <ESC>C"));
(*u8x8_p).print((char) 27);
(*u8x8_p).print('C');
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
