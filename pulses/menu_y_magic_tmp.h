/*
  menu_y_magic_tmp.h
  temporary development file
*/

    switch(MENU.numeric_input(ILLEGAL)) {
    case ILLEGAL:
      MENU.outln(F("sizeof() config musicBox, magical, UI:" ));
      MENU.outln(sizeof(musicBoxConf));
      MENU.outln(sizeof(magicalConf));
      MENU.outln(sizeof(uiConf));

    case 1:
      MENU.outln(F("Marianne Pletscher 1 sync unleserlich"));
      /*
	tetraCHORD
	tetraCHORD_rise
	S unleserlich, 4?
	p[15]|1024
      */
      chromatic_pitch = 0;
      parameters_by_user();
      break;

    case 2:
      MENU.outln(F("Marianne Pletscher 2"));
      select_array_in(SCALES, pentatonic_minor);
      select_array_in(JIFFLES, ding_ditditdit);
      sync=0;
      stack_sync_slices=64;
      pitch={1, 175};		// f
      chromatic_pitch = 9;		// f
      // 11'42"
      parameters_by_user();
      break;

    case 3:
      MENU.outln(F("Marianne Pletscher 3"));
      select_array_in(SCALES, doric_scale);
      select_array_in(JIFFLES, d4096_128);
      sync=1;
      stack_sync_slices=128;
      pitch={1, 330};
      chromatic_pitch = 4;		// e
      // 6'12"
      parameters_by_user();
      break;

    case 4:
      MENU.outln(F("Marianne Pletscher 4"));
      select_array_in(SCALES, pentaCHORD);	// unleserlich,,,
      select_array_in(JIFFLES, pent_top_wave_0);
      sync=2;
      stack_sync_slices=0;
      pitch={1, 175};		// f
      chromatic_pitch = 9;		// f
      // 9'22"
      parameters_by_user();
      break;

    case 5:
      MENU.outln(F("Marianne Pletscher 5"));
      select_array_in(SCALES, octave_4th_5th);
      select_array_in(JIFFLES, simple_theme);
      sync=3;
      stack_sync_slices=0;
      pitch={1, 294};		// d
      chromatic_pitch = 6;		// d
      // zeit unleserlich
      parameters_by_user();
      break;

    case 6:
      MENU.outln(F("eifach wunderschön"));
      // 2019-02-26_12h46m19s
      select_array_in(SCALES, doric_scale);
      select_array_in(JIFFLES, diing_ditditdit);
      sync=1;
      stack_sync_slices=0;
      pitch={1, 330};		// e
      chromatic_pitch = 8;	// e
      // SUBCYCLE: | 6' 12" |
      parameters_by_user();
      break;

    case 7:
      MENU.outln(F("young dragons dancing in the sky"));
      // 2018-12-22_19h57m19s
      select_array_in(SCALES, tetraCHORD);
      select_array_in(JIFFLES, PENTAtonic_desc);
      sync=2;
      stack_sync_slices=0;
      pitch={32768*2, 263};
      chromatic_pitch = 0;		// u
      // 4' 7"
      parameters_by_user();
      break;

    case 8:
      MENU.outln(F("2019-04-01 doric stack"));
      select_array_in(SCALES, doric_scale);
      select_array_in(JIFFLES, din__dididi_dixi);
      sync = 1;
      stack_sync_slices = 8;
      pitch = {1, 175};		// f
      chromatic_pitch = 8;	// f
      // 2^4 SUBCYCLE: | 11' 42" |
      parameters_by_user();
      break;

    case 9:
      MENU.outln(F("noname_ "));
      select_array_in(SCALES, europ_PENTAtonic);
      select_array_in(JIFFLES, pentaCHORD_rise);
      sync = 5;
      stack_sync_slices = 0;
      pitch = {1, 175};		// f
      chromatic_pitch = 8;	// f
      // SUBCYCLE: | 6' 11" |
      parameters_by_user();
      break;

    case 10:
      MENU.outln(F("genial jazzpiano version"));
      select_array_in(SCALES, pentachord);
      select_array_in(JIFFLES, tumtumtum);
      sync = 0;
      stack_sync_slices = 0;
      pitch = {1, 128};		// u
      chromatic_pitch = 0;	// u
      // 
      parameters_by_user();
      break;

    case 11:
      MENU.outln(F("2019-02-07_classical_time_machine_S4"));
      select_array_in(SCALES, minor_scale);
      select_array_in(JIFFLES, d4096_1024);
      sync = 4;
      stack_sync_slices = 0;
      pitch = {32768, 262};		// 
      chromatic_pitch = 0;		// u
      // 2^-4 SUBCYCLE: | 5' 10" |
      parameters_by_user();
      break;

    case 12:
      MENU.outln(F("simplicistic_d0"));
      select_array_in(SCALES, octaves_fourths);
      select_array_in(JIFFLES, din_dididi);
      sync = 2;
      stack_sync_slices = 0;
      pitch = {3, 524288};		// g?
      chromatic_pitch = 0;		// u?
      // SUBCYCLE: | 5' 31" |
      parameters_by_user();
      break;

/*
    case :
      MENU.outln(F(""));
      select_array_in(SCALES, );
      select_array_in(JIFFLES, );
      sync = ;
      stack_sync_slices = ;
      pitch = {, };		// 
      chromatic_pitch = ;		// u
      // 2^ SUBCYCLE: | ' " |
      parameters_by_user();
      break;
*/
    }
