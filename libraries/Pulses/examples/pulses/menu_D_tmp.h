/*
  menu_D_tmp.h

  temporary test code for pulses menu 'D'
*/

#ifdef USE_MORSE
    morse_show_tokens();
    MENU.ln();
  #if defined MORSE_TOKEN_DEBUG		// a *lot* of debug info...
    morse_debug_token_info();
    morse_show_tokens();
    MENU.ln();
  #endif

  /*
  #if defined MORSE_OUTPUT_PIN
    morse_play_out_tokens();	// play saved tokens in current morse speed ;)
  #endif
  */

  MENU.outln("DADA morse77");
  show_morse_output_buffer();
  morse_2ACTION();
  //    morse_tokens2meaning_state();	// obsolete?

  morse_OUTPUT_cnt=0;

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

#endif // USE_MORSE

    /*
    #if defined USE_MCP23017
      MCP23017.digitalWrite(0, ++testmcp & 1);
    #endif
    */

    // lower_audio_if_too_high(409600);

    /*
    MENU.play_KB_macro("X 0");
    PULSES.setup_pulse_synced(NULL, ACTIVE, PULSES.get_now(), 4000000, 1, 1, sync);
    PULSES.pulses[0].base_time = PULSES.pulses[0].period.time;

    PULSES.set_icode_p(0, TEST_ICODE, true);	// set (and activate) icode
    //    PULSES.set_icode_p(0, (icode_t*) d4096_1024, true);
    MENU.play_KB_macro("n.");	// TODO: why do we need that?
    */

    // PULSES.set_payload(2, &pulse_info_1line); // test: set and activate payload

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