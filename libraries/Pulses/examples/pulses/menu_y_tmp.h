/*
  menu_y_tmp.h

  temporary test code for pulses menu 'y'
*/


//   TODO: did not touch the multiplier/divisor mess	################
tune_selected_2_scale_limited(pitch, selected_in(SCALES), 409600*2L);	// 2 bass octaves // TODO: adjust appropriate...
//tune_selected_2_scale_limited({multiplier*2048, divisor}, selected_in(SCALES), 409600*2L);	// 2 bass octaves // TODO: adjust appropriate...


#if defined USE_BATTERY_CONTROL
    show_battery_level();
#endif

#if defined MAGICAL_MUSIC_BOX
    //    magical_fart_setup(12, 15);		// ;)
#endif

#if defined USE_MORSE
    /*
    for(int i = 0; i<MORSE_DEFINITIONS; i++) {
      string definition = morse_definitions_tab[i];
      morse_reset_definition(definition);
      morse_read_definition(i);
      morse_show_definition();
    }
    */
    morse_find_definition("-.-");
    morse_find_definition("-.---");
    morse_find_definition(".-.");

    /*
    for(int i = 0; i<MORSE_DEFINITIONS; i++) {
      morse_read_definition(i);
      morse_definition_set_show(morse_uppercase);
    }
    */
    //
    //    morse_2ACTION();
//  morse_play_out_tokens();	// play and show saved tokens in current morse speed
#endif // USE_MORSE

    /*
    {
      // temporary least-common-multiple  test code, unfinished...	// ################ FIXME: ################
      unsigned long lcm=1L;
      for (int pulse=0; pulse<PL_MAX; pulse++) {
	if (PULSES.pulse_is_selected(pulse)) {
	  MENU.out(pulse); MENU.tab(); PULSES.pulses[pulse].period.time; MENU.tab();
	  lcm = HARMONICAL.LCM(lcm, PULSES.pulses[pulse].period.time);
	  MENU.outln(lcm);
	}
      }

      MENU.out(F("==> lcm ")); MENU.out(lcm);
      struct time length;
      length.time = lcm;
      length.overflow = 0;
      PULSES.display_realtime_sec(length);
      MENU.ln();

      for (int pulse=0; pulse<PL_MAX; pulse++)
	if ((PULSES.pulse_is_selected(pulse)) && PULSES.pulses[pulse].period.time) {
	  MENU.out(pulse);
	  MENU.tab();
	  MENU.outln(lcm/PULSES.pulses[pulse].period.time);
	}
    }
    */
  break;
