/*
  menu_y_tmp.h

  temporary test code for pulses menu 'y'
*/


#if defined MAGICAL_MUSIC_BOX
    //    magical_fart_setup(12, 15);		// ;)
#endif


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
      time_t length;
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
