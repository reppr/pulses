/*
  men_MuD.h
  temporary test menu UI during development
*/

#if defined USE_ADS1115_AT_ADDR
//ads1115_first_test_A0();
//ads1115_first_test_diff_A2_A3();
#endif

{
  MENU.out(metric_mnemonic_str(musicBoxConf.chromatic_pitch));
  char s[] = {0, 0};
  s[0] = scale_symbol_char();
  MENU.outln(s);
}


MENU.out(F("normalised_pitch "));
MENU.out(get_normalised_pitch(), 6);
MENU.ln();


// determine_imu_zero_offsets(2000);

//  show_morse_event_buffer();

    /*
    debug_accgyro_sample = !debug_accgyro_sample;
    MENU.out(F("debug_accgyro_sample"));
    MENU.out_ON_off(debug_accgyro_sample);
    MENU.ln();
    */

    //MENU.outln(F("HARDWARE configuration"));
    //show_hardware_conf(&HARDWARE);

    //display_peer_ID_list();
    //random_RGB_string(MENU.numeric_input(4));

    /*
    extern short steps_in_octave;
    MENU.out(F("steps_in_octave "));
    MENU.outln(steps_in_octave);
    */

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
