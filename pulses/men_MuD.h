/*
  men_MuD.h
  temporary test menu UI during development
*/

    debug_accgyro_sample = !debug_accgyro_sample;
    MENU.out(F("debug_accgyro_sample"));
    MENU.out_ON_off(debug_accgyro_sample);
    MENU.ln();

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