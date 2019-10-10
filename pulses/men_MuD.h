/*
  men_MuD.h
  temporary test menu UI during development
*/

#if defined USE_ADS1115_AT_ADDR
{
  static int16_t just_read = ADS1115.readADC_SingleEnded(0);	// initialise to get sum right
  int16_t read_before=just_read;
  just_read = ADS1115.readADC_SingleEnded(0);

  static float volts= (just_read * 0.1875)/1000;	// initialise to get sum right
  float volts_before=volts;
  volts = (just_read * 0.1875)/1000;

  MENU.out(F("read "));
  MENU.out(just_read);
  MENU.tab();
  MENU.out(volts, 6);
  MENU.tab();

  float diff = volts - volts_before;
  if(diff == 0)
    MENU.out('=');
  else if(diff > 0)
    MENU.out('+');
  MENU.out(diff, 6);
  MENU.tab();

  static float sum=0.0;
  sum += diff;
  MENU.out(sum, 6);
  MENU.ln();
}

#else
MENU.out(F("normalised_pitch "));
MENU.out(get_normalised_pitch(), 6);
MENU.ln();

#endif

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
