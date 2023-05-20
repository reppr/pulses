/*
  menu_D_tmp.h

  temporary test code for pulses menu 'D'
*/

/*
(*u8x8_p).setPowerSave(0);
delay(1000);
(*u8x8_p).setPowerSave(1);
delay(1000);
*/

#if defined COMPILE_ACCEL_GYRO_SPEED_TEST
accGyro_speed_test();
#endif

/*
sample_accelero_giro_6d();

MENU.out(accelero_gyro_current.ax);
MENU.tab();
MENU.out(accelero_gyro_current.ay);
MENU.tab();
MENU.out(accelero_gyro_current.az);
MENU.tab();
MENU.out(accelero_gyro_current.gx);
MENU.tab();
MENU.out(accelero_gyro_current.gy);
MENU.tab();
MENU.out(accelero_gyro_current.gz);
MENU.ln();
*/

#undef NOT_DEFINED	// DEACTIVATE...
#if defined USE_MORSE && defined NOT_DEFINED	// && defined MORSE_COMPILE_HELPERS

  // morse_show_tokens(true);
  // MENU.ln();
  // morse_show_tokens();

  // morse_decode();

  //    MENU.ln();

  //  #if defined MORSE_TOKEN_DEBUG		// a *lot* of debug info...
  //    morse_debug_token_info();
  //    morse_show_tokens();
  //    MENU.ln();
  //  #endif
  //
  //  #if defined MORSE_OUTPUT_PIN
  //    morse_play_out_tokens();	// play saved tokens in current morse speed ;)
  //  #endif

    /*
    #if defined MORSE_TOKEN_DEBUG
      morse_show_tokens();
      MENU.ln();

      morse_debug_token_info();

      morse_show_tokens();
      MENU.ln();

      morse_token_cnt=0;
    #else
      morse_show_tokens();
      MENU.ln();

      morse_token_cnt=0;
    #endif
    */

    /*
      for(int i = 0; i<MORSE_DEFINITIONS; i++) {
	string definition = morse_definitions_tab[i];
	morse_reset_definition(definition);
	morse_read_definition(i);
	morse_show_definition();
      }
    */

  /*
    MENU.outln("DADA");
    morse_find_definition("-.-");
    morse_find_definition("-.---");
    morse_find_definition(".-.");
    MENU.outln("DADA");
  */

  /*
    for(int i = 0; i<MORSE_DEFINITIONS; i++) {
      morse_read_definition(i);
      morse_definition_set_show(morse_uppercase);	// probably only used for DEBUGGING
    }
  */

  //    morse_2ACTION();

  /*
    #if defined MORSE_OUTPUT_PIN
      morse_play_out_tokens();	// play and show saved tokens in current morse speed
    #endif
  */
#endif // USE_MORSE

    /*
    #if defined USE_MCP23017
      MCP23017.digitalWrite(0, ++testmcp & 1);
    #endif
    */

    // lower_audio_if_too_high(409600);

    /*
    MENU.outln(next_gpio());
    MENU.outln(next_gpio());
    MENU.outln(next_gpio());
    MENU.outln(next_gpio());

    MENU.outln(next_gpio(2));
    MENU.outln(next_gpio());

    MENU.outln(next_gpio(-123));
    MENU.outln(next_gpio(999));
    */

    // MENU.outln(sizeof(gpio_pins) / sizeof(gpio_pin_t));


/*
    input_value=MENU.numeric_input(-1);
    if (input_value > -1)
      PULSES.select_pulse(input_value);
*/

/*
    // ESP32 DAC test
    MENU.out(F("DAC test "));
    dacWrite(BOARD_DAC1, input_value=MENU.numeric_input(-1));
    dacWrite(BOARD_DAC2, input_value);
    MENU.outln(input_value);
*/
