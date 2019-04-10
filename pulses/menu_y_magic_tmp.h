/*
  menu_y_magic_tmp.h
  temporary development file
*/

{
  title=NULL;
  date=NULL;
  // pitch = {1, 1};	// TODO: set as default

  PULSES.time_unit = 1000000;	// sorry, i just reset that
  stack_sync_slices = 0;	// sorry, i just reset that
  chromatic_pitch = 0;		// sorry, i just reset that

  MENU.ln(2);
  MENU.out(F("PRESET "));

  input_value = MENU.numeric_input(1);
  if(input_value == 0)		// selecting zero gives random preset
    input_value = random(165) + 1;
  bool valid_selection = true;
  switch(input_value) {
  case 1:
    title = F("wunderschön melodisch :) :) :)");
    //comment: mit schönem, aberendlosem schluss ;)
    date = F("2018-11-19_17h42m49s	MON");
    select_array_in(SCALES, octaves_fifths);
    select_array_in(JIFFLES, dwn_THRD);
    sync = 0;
    pitch = {32768*2, 281};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	14d 14h 15' 26"
    subcycle_octave = -1;	// 2	    5'  8"	| subcycle |
    break;

  case 2:
    title = F("LIFT OFF");
    //comment: :) :) :) :) :)
    date = F("2018-12-11_20h48m37s	TUE");
    select_array_in(SCALES, doric_scale);
    select_array_in(JIFFLES, PENTAtonic_rise);
    sync = 5;
    pitch = {32768, 217};
    // fix:	play_KB_macro  /2
    // CYC: 2^0	   49' 50"
    subcycle_octave = -3;	// 6' 14"	| subcycle |
    break;

  case 3:
    title = F("Blue Night Cadence");
    //comment: very rich chord progression :) :) :)
    date = F("2018-12-19_20h24m56s	WED");
    select_array_in(SCALES, pentatonic_minor);
    select_array_in(JIFFLES, pentatonic_rise);
    sync = 4;
    pitch = {32768, 292};
    // harmonical CYCLE: 1h 14' 4"	SUBCYCLE: | 4' 38" |
    subcycle_octave = -4;
    break;

  case 4:
    title = F("Ouverture");
    //comment: :) :) :) !!!
    date = F("2018-12-09_15h24m13s	SUN");
    select_array_in(SCALES, octave_4th_5th);
    select_array_in(JIFFLES, PENTAtonic_rise);
    sync = 0;
    pitch = {32768, 375};
    // CYC: 2^0	6h  9'  5"
    subcycle_octave = -5;	// 11' 32"	| subcycle |
    break;

  case 5:
    title = F("dragons dancing in the sky");
    //comment: :) :) :)
    date = F("2018-12-22_19h57m19s	SAT");
    select_array_in(SCALES, tetraCHORD);
    select_array_in(JIFFLES, PENTAtonic_desc);
    sync = 2;
    pitch = {32768, 263};
    // fix:	play_KB_macro  *2
    // harmonical CYCLE: 4h 23' 8"	SUBCYCLE: | 4' 7" |
    subcycle_octave = -6;
    break;

  case 6:
    title = F("aurora elfetanz");
    //comment: sphärischi liechtwälle vom Himmel uf t erde	:) :) :) <======
    //comment: elfetanz
    //comment: grillezirpe
    //comment: langsami reige under de girlande vo de himmelswirbel
    //comment: keimendi engelssaat
    //comment: s wachst langsam im garte
    //comment: 29/30 letschti psinnig, denn zur rueh choo
    //comment: 59/60 ready?
    //comment: 1/1   silently restart :)
    date = F("2018-11-13_11h57m23s	TUE");
    select_array_in(SCALES, tetraCHORD);
    select_array_in(JIFFLES, dwn_THRD_up);
    sync = 1;
    // fixed tuning g	196
    pitch = {32768, 196};
    // CYC: 2^0	2h 56' 32"
    break;

  case 7:
    title = F("klassisch, wunderschön, vo A bis Z :)");
    //comment: und übrigens:	harmonical cycle    12' 19"
    date = F("2018-11-18_20h09m42s	SUN");
    select_array_in(SCALES, major_scale);
    select_array_in(JIFFLES, din_dididi);
    sync = 2;
    pitch = {32768, 351};
    // CYC: 2^0	   12' 19"
    subcycle_octave = -2;	// 3'  5"	| subcycle |
    break;

  case 8:
    title = F("wunderschöns obenabe vogelgetzwitscher bistüüfinbass :)");
    date = F("2018-11-06_15h52m17s	TUE");
    select_array_in(SCALES, octaves_fifths);
    select_array_in(JIFFLES, dwn_THRD_up);
    sync = 5;
    pitch = {32768, 322};
    break;

  case 9:
    title = F("e rechts füürwerch a jubelnde wirbel oderso :)");
    date = F("2018-11-07_17h36m34s	WED");
    select_array_in(SCALES, major_scale);
    select_array_in(JIFFLES, penta_3rd_down_5);
    sync = 1;
    pitch = {32768, 229};
    // CYC: 2^0	   18' 53"
    break;

  case 10:
    title = F("fröhlichs, witzigs Tänzli :)");
    date = F("2018-11-08_14h27m25s	THU");
    select_array_in(SCALES, major_scale);
    select_array_in(JIFFLES, simple_theme);
    sync = 5;
    pitch = {32768, 380};
    // fix:	play_KB_macro  *2
    break;

  case 11:
    title = F("klassisch TIMEMACHINE style");
    date = F("2018-11-07_18h20m53s	WED");
    select_array_in(SCALES, minor_scale);
    select_array_in(JIFFLES, ding_ditditdit);
    sync = 2;
    pitch = {32768, 359};
    // no_octave_shift = true;
    // fix:	play_KB_macro  *2
    // CYC: 2^0	2h  0' 29"
    break;

  case 12:
    title = F("a love like the morning birds");
    date = F("2019-01-06_21h29m36s	SUN");
    select_array_in(SCALES, octaves_fourths);
    select_array_in(JIFFLES, PENTAtonic_desc);
    sync = 4;
    pitch = {32768, 371};
    // fix:	play_KB_macro  *2
    // harmonical CYCLE: 8d 6h 58' 1"	SUBCYCLE: | 5' 50" |
    subcycle_octave = -11;
    break;

  case 13:
    title = F("veryverybad");
    select_array_in(SCALES, europ_PENTAtonic);
    sync = 3;
    select_array_in(JIFFLES, tetraCHORD_desc);
    pitch = {1, 12};	// 8	*metric* c
    // harmonical CYCLE: 1h 6' 2"	SUBCYCLE: | 4' 8" |
    break;

  case 14:
    title = F("2019-02-01_aRainOfJoy");
    date = F("2019-02-02_12h12m11s	SAT");
    select_array_in(SCALES, europ_PENTAtonic);
    select_array_in(JIFFLES, penta_3rd_down_5);
    sync = 1;
    // random *metric* tuning d
    pitch = {32768, 294};
    // fix:	play_KB_macro  *2
    // harmonical CYCLE: 58' 51"	SUBCYCLE: | 3' 41" |
    subcycle_octave = -4;
    break;
 
  case 15:
    title = F("genial jazzpiano version");
    select_array_in(SCALES, pentachord);
    sync = 0;
    select_array_in(JIFFLES, tumtumtum);
    pitch = {1, 12};	// 8	*metric* a
    // harmonical CYCLE: 6h 33' 12"	SUBCYCLE: | 6' 9" |
    break;

  case 16:
    title = F("sehr ruhigs festlichs stimmigsbild");
    //comment: bordunartig, stehend
    date = F("2018-11-06_20h02m08s	TUE");
    select_array_in(SCALES, major_scale);
    select_array_in(JIFFLES, diing_ditditdit);
    sync = 5;
    pitch = {32768, 417};
    // fix:	play_KB_macro  *2
    break;

  case 17:
    title = F("tickende uhr");
    date = F("2018-11-06_20h48m16s	TUE");
    select_array_in(SCALES, octaves_fifths);
    select_array_in(JIFFLES, ding_ditditdit);
    sync = 1;
    pitch = {32768, 214};
    // fix:	play_KB_macro  *2
    break;

  case 18:
    title = F("ewig tickends uhr gloggespiel");
    date = F("2018-11-07_17h49m47s	WED");
    select_array_in(SCALES, octave_4th_5th);
    select_array_in(JIFFLES, diing_ditditdit);
    sync = 1;
    pitch = {32768, 410};
    // CYC: 2^0	5h 37' 35"
    break;

  case 19:
    title = F("simple, hübsche aafang");
    date = F("2018-11-08_14h38m32s	THU");
    select_array_in(SCALES, pentatonic_minor);
    select_array_in(JIFFLES, ding_ditditdit);
    sync = 0;
    pitch = {32768, 210};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	3h 25' 58"
    break;

  case 20:
    title = F("crazy, roboterhafts piepse");
    //comment: alarmgepiepse
    //comment: langsams, maschinells schriite
    //comment: ständig umpiepst vo de nervende alärm ;)
    date = F("2018-11-08_15h07m35s	THU");
    select_array_in(SCALES, pentatonic_minor);
    select_array_in(JIFFLES, din__dididi_dixi);
    sync = 0;
    pitch = {32768, 386};
    // fix:	play_KB_macro  /2
    // CYC: 2^0	   28'  1"
    break;

  case 21:
    title = F("langsams iiordne in es ruhigs piipskonzert");
    date = F("2018-11-08_18h05m59s	THU");
    select_array_in(SCALES, octaves_fifths);
    select_array_in(JIFFLES, din__dididi_dixi);
    sync = 3;
    // fixed tuning e	330
    pitch = {32768, 330};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	12d 10h 14' 58"
    break;

  case 22:
    title = F("hübsche start mit geknatter");
    date = F("2018-11-13_11h40m30s	TUE");
    select_array_in(SCALES, octave_4th_5th);
    select_array_in(JIFFLES, pentatonic_desc);
    sync = 0;
    // fixed tuning g	196
    pitch = {32768, 196};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	11h 46'  9"
    break;

  case 23:
    title = F("harpsichord");
    //comment: wie n alte musiker wo e langsam schreitendi,
    //comment: langi dorischi kadenz uf sim cembalo spillt
    //comment: eifach und ruhig schreitend mit glitzerfunkewirbel drüberobe
    date = F("2018-11-13_12h11m04s	TUE");
    select_array_in(SCALES, doric_scale);
    select_array_in(JIFFLES, din__dididi_dixi);
    sync = 2;
    // fixed tuning d	294
    pitch = {32768, 294};
    // CYC: 2^0	1h 13' 33"
    break;
    
  case 24:
    title = F("klassischi brachemagie");
    date = F("2018-11-13_12h23m54s	TUE");
    select_array_in(SCALES, europ_PENTAtonic);
    select_array_in(JIFFLES, pentatonic_rise);
    sync = 5;
    // fixed tuning b	233
    pitch = {32768, 233};
    // CYC: 2^0	1h 14' 15"
    break;

  case 25:
    title = F("nice, simple slow harmonics");
    date = F("2018-11-13_15h04m11s	TUE");
    select_array_in(SCALES, octaves_fifths);
    select_array_in(JIFFLES, penta_3rd_rise);
    sync = 2;
    // fixed tuning f	175
    pitch = {32768, 175};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	11d 17h 12' 24"
    break;

  case 26:
    title = F("en andere brachklassiker");
    //comment: müestemalnolengerlose...
    //comment: schöni uufbou
    //comment: vilichtmengischauchlilangwiiligi?
    date = F("2018-11-13_15h12m50s	TUE");
    select_array_in(SCALES, major_scale);
    select_array_in(JIFFLES, pentatonic_rise);
    sync = 5;
    // fixed tuning a	220
    pitch = {32768, 220};
    // CYC: 2^0	   19' 40"
    break;

  case 27:
    title = F("nomal en timemachine klassiker");
    //comment: zum uussueche...
    //comment: bald mit tüüfem gebrumm
    date = F("2018-11-13_17h33m15s	TUE");
    select_array_in(SCALES, europ_PENTAtonic);
    select_array_in(JIFFLES, ding_ditditdit);
    sync = 1;
    // fixed tuning b	233
    pitch = {32768, 233};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	2h 28' 30"
    break;

  case 28:
    title = F("PENT eifache reige");
    date = F("2018-11-14_13h27m35s	WED");
    select_array_in(SCALES, europ_PENTAtonic);
    select_array_in(JIFFLES, ding_ditditdit);
    sync = 5;
    // fixed tuning a	220
    pitch = {32768, 220};
    // fix:	play_KB_macro  /2
    // CYC: 2^0	   39' 19"
    break;

  case 29:
    title = F("triggered");
    //comment: da lauft öppis, so simpel das es bliibt	:)
    date = F("2018-11-14_14h58m29s	WED");
    select_array_in(SCALES, minor_scale);
    select_array_in(JIFFLES, PENTAtonic_rise);
    sync = 3;
    // fixed tuning f	175
    pitch = {32768, 175};
    // CYC: 2^0	1h  1' 47"
    break;

  case 30:
    title = F("short cycled !");
    date = F("2018-11-14_19h47m20s	WED");
    select_array_in(SCALES, major_scale);
    select_array_in(JIFFLES, pentatonic_rise);
    sync = 0;
    // fixed tuning e	330
    pitch = {32768, 330};
    // CYC: 2^0	   13'  6"
    break;

  case 31:
    title = F("sanfti landig");
    date = F("2018-11-15_09h10m37s	THU");
    select_array_in(SCALES, europ_PENTAtonic);
    select_array_in(JIFFLES, dwn_THRD_up);
    sync = 1;
    // fixed tuning a	220
    pitch = {32768, 220};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	2h 37' 17"
    break;

  case 32:
    title = F("schön, ruhig, eifach");
    //comment: *lange* harmonical cycle 11h 46' 9"
    date = F("2018-11-15_15h15m34s	THU");
    select_array_in(SCALES, tetraCHORD);
    select_array_in(JIFFLES, pentatonic_rise);
    sync = 3;
    // fixed tuning g	196
    pitch = {32768, 196};
    // fix:	play_KB_macro  *4
    // CYC: 2^0	11h 46'  9"
    break;

  case 33:
    title = F("magical toilet gejodel");
    date = F("2018-11-15_15h38m13s	THU");
    select_array_in(SCALES, doric_scale);
    select_array_in(JIFFLES, penta_3rd_down_5);
    sync = 5;
    // fixed tuning c	262
    pitch = {32768, 262};
    // CYC: 2^0	1h 22' 33"
    break;

  case 34:
    title = F("hübsch, langipausenamaafang");
    date = F("2018-11-15_15h53m44s	THU");
    select_array_in(SCALES, europ_PENTAtonic);
    select_array_in(JIFFLES, dwn_THRD_up);
    sync = 5;
    // fixed tuning b	233
    pitch = {32768, 233};
    // fix:	play_KB_macro  *4
    // CYC: 2^0	4h 57'  1"
    subcycle_octave = -3;	// 37'  8"	| subcycle |
    break;


  case 35:
    title = F("uusprägts melodiemotiv");
    date = F("2018-11-16_12h29m17s	FRI");
    select_array_in(SCALES, octaves_fifths);
    select_array_in(JIFFLES, PENTA_3rd_rise);
    sync = 5;
    pitch = {32768, 383};
    // fix:	play_KB_macro  /2
    // used subcycle:    3' 46"
    // harmonical cycle 2d 16h 14' 40"
    break;

  case 36:
    title = F("maschinells ticke");
    date = F("2018-11-17_11h59m15s	SAT");
    select_array_in(SCALES, europ_PENTAtonic);
    select_array_in(JIFFLES, ding_ditditdit);
    sync = 0;
    pitch = {32768, 207};
    // fix:	play_KB_macro  *4
    // CYC: 2^0	5h 34' 19"
    subcycle_octave = -8;	// 1' 18"	| subcycle |
    break;

  case 37:
    title = F("sehr sehr spezielle sync 0 aafang, wunderschön");
    //comment: chönnt mer guet au 1/2 cycle näh
    //comment: u.a.
    date = F("2018-11-17_20h41m20s	SAT");
    select_array_in(SCALES, europ_PENTAtonic);
    select_array_in(JIFFLES, PENTAtonic_desc);
    sync = 0;
    pitch = {32768, 379};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	1h 31' 18"
    subcycle_octave = -4;	// 5' 42"	| subcycle |
    break;

  case 38:
    title = F("allereifachsts melodischs iiticke");
    //comment: schluss müest me no händisch mache...
    date = F("2018-11-18_20h26m18s	SUN");
    select_array_in(SCALES, octaves_fifths);
    select_array_in(JIFFLES, ding_ditditdit);
    sync = 1;
    pitch = {32768, 420};
    // CYC: 2^0	4d 21h 10' 10"
    subcycle_octave = -1;	// 1	    3' 26"	| subcycle |
    break;

  case 39:
    title = F("näbel");
    date = F("2018-11-19_10h30m25s	MON");
    select_array_in(SCALES, minor_scale);
    select_array_in(JIFFLES, dwn_THRD_up);
    sync = 2;
    pitch = {32768, 399};
    // fix:	play_KB_macro  *4
    // CYC: 2^0	3h 36' 48"
    subcycle_octave = -6;	// 3' 23"	| subcycle |
    break;

  case 40:
    title = F("jazzig");
    //comment: und übrigens:	harmonical cycle    49' 50"
    date = F("2018-11-19_20h29m59s	MON");
    select_array_in(SCALES, doric_scale);
    select_array_in(JIFFLES, tumtum);
    sync = 4;
    pitch = {32768, 217};
    // fix:	play_KB_macro  /2
    // CYC: 2^0	   49' 50"
    subcycle_octave = -4;	// 3'  7"	| subcycle |
    break;

  case 41:
    title = F("chileglogge?");
    date = F("2018-11-20_11h40m15s	TUE");
    select_array_in(SCALES, pentatonic_minor);
    select_array_in(JIFFLES, up_THRD);
    sync = 1;
    pitch = {32768, 172};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	2h  5' 44"
    subcycle_octave = -5;	// 3' 56"	| subcycle |
    break;

  case 42:
    title = F("lesley");
    date = F("2018-11-20_14h34m14s	TUE");
    select_array_in(SCALES, minor_scale);
    select_array_in(JIFFLES, rising_pent_them);
    sync = 4;
    pitch = {32768, 195};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	1h 50' 54"
    subcycle_octave = -5;	// 3' 28"	| subcycle |
    break;

  case 43:
    title = F("vorbereitige zum abflug?");
    //comment: oderso,
    //comment:   muesdeaafangnomallose ;)
    date = F("2018-11-20_15h15m48s	TUE");
    select_array_in(SCALES, europ_PENTAtonic);
    select_array_in(JIFFLES, up_THRD_dn);
    sync = 1;
    pitch = {32768, 225};
    // CYC: 2^0	1h 16' 54"
    subcycle_octave = -4;	// 4' 48"	| subcycle |
    break;

  case 44:
    title = F("melodischs tänzle");
    //comment: und übrigens:	harmonical cycle    53' 34"
    date = F("2018-11-20_20h31m27s	TUE");
    select_array_in(SCALES, europ_PENTAtonic);
    select_array_in(JIFFLES, pent_top_wave);
    sync = 5;
    pitch = {32768, 323};
    // CYC: 2^0	   53' 34"
    subcycle_octave = -4;	// 3' 21"	| subcycle |
    break;

  case 45:
    title = F("wienen rägeboge vo striicher wo abeschwebed uf t erde bis tüüf in bass und meh und meh i en zyclische reige iistimmed");
    date = F("2018-11-21_13h23m41s	WED");
    select_array_in(SCALES, doric_scale);
    select_array_in(JIFFLES, rising_pent_them);
    sync = 1;
    pitch = {32768, 295};
    // CYC: 2^0	1h 13' 19"
    subcycle_octave = -4;	// 4' 35"	| subcycle |
    break;

  case 46:
    title = F("e psinnlichi ziit");
    date = F("2018-11-21_14h33m31s	WED");
    select_array_in(SCALES, octave_4th_5th);
    select_array_in(JIFFLES, pentatonic_desc);
    sync = 2;
    pitch = {32768, 354};
    // fix:	play_KB_macro  *4
    // CYC: 2^0	1d 2h  3' 55"
    subcycle_octave = -9;	// 3'  3"	| subcycle |
    break;

  case 47:
    title = F("es netz vo eigenartige melodie wiegt über öis");
    date = F("2018-11-21_15h49m05s	WED");
    select_array_in(SCALES, minor_scale);
    select_array_in(JIFFLES, pent_patternA);
    sync = 3;
    pitch = {32768, 161};
    // used SUBCYCLE:    4' 12"
    break;

  case 48:
    title = F("eifach und schön");
    //comment: und übrigens:	harmonical cycle    55' 27"
    date = F("2018-11-21_16h08m40s	WED");
    select_array_in(SCALES, pentatonic_minor);
    select_array_in(JIFFLES, pentatonic_rise);
    sync = 5;
    pitch = {32768, 195};
    // usedSUBCYCLE:    3' 28"
    // CYC: 2^0	   55' 27"
    subcycle_octave = -4;	// 3' 28"	| subcycle |
    break;

  case 49:
    title = F("herzig ;)");
    date = F("2018-11-21_16h54m05s	WED");
    select_array_in(SCALES, europ_PENTAtonic);
    select_array_in(JIFFLES, pent_patternA);
    sync = 5;
    pitch = {32768, 266};
    // fix:	play_KB_macro  *4
    // CYC: 2^0	4h 20' 10"
    subcycle_octave = -6;	// 4'  4"	| subcycle |
    break;

  case 50:
    title = F("slow simple melodic ostinato dance");
    //comment: harmonical cycle    40' 39"	...
    select_array_in(SCALES, doric_scale);
    select_array_in(JIFFLES, rising_pent_them);
    sync = 5;
    pitch = {32768, 266};
    // fix:	play_KB_macro  /2
    // CYC: 2^0	   40' 39"
    subcycle_octave = -3;	// 5'  5"	| subcycle |
    break;

  case 51:
    title = F("mad accordeon players whichdance with old indio's flute babbling");
    date = F("2018-11-21_17h35m59s	WED");
    select_array_in(SCALES, pentatonic_minor);
    select_array_in(JIFFLES, dwn_THRD_up);
    sync = 0;
    pitch = {32768, 384};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	1h 52' 38"
    subcycle_octave = -5;	// 3' 31"	| subcycle |
    break;

  case 52:
    title = F("harmonikali jazzsession");
    //comment: harmonical cycle    54'  4"
    date = F("2018-11-21_20h21m11s	WED");
    select_array_in(SCALES, minor_scale);
    select_array_in(JIFFLES, tumtum);
    sync = 5;
    pitch = {32768, 200};
    // CYC: 2^0	   54'  4"
    subcycle_octave = -4;	// 3' 23"	| subcycle |
    break;

  case 53:
    title = F("church bells are still ringing");
    date = F("2018-11-21_21h12m38s	WED");
    select_array_in(SCALES, pentatonic_minor);
    select_array_in(JIFFLES, pent_patternA);
    sync = 2;
    pitch = {32768, 221};
    // CYC: 2^0	1h 37' 51"
    subcycle_octave = -5;	// 3'  3"	| subcycle |
    break;

  case 54:
    title = F("kid's room");
    date = F("2018-11-22_08h48m23s	THU");
    select_array_in(SCALES, octaves_fifths);
    select_array_in(JIFFLES, pent_top_wave_0);
    sync = 5;
    pitch = {32768, 415};
    // fix:	play_KB_macro  *2
    // used SUBCYCLE:    3' 28"
    break;

  case 55:
    title = F("uufregig im hüehnerstall weg de chileglogge");
    //comment: //comment: ;) ;)
    //comment: harmonical cycle     7'  1"
    date = F("2018-11-22_09h26m06s	THU");
    select_array_in(SCALES, major_scale);
    select_array_in(JIFFLES, dwn_THRD_up);
    sync = 4;
    pitch = {32768, 308};
    // fix:	play_KB_macro  /2
    // CYC: 2^0	    7'  1"
    subcycle_octave = -1;	// 3' 31"	| subcycle |
    break;

  case 56:
    title = F("klassischi harmonikali aesthetik");
    //comment: harmonical cycle    26' 18"	...
    date = F("2018-11-22_10h44m17s	THU");
    select_array_in(SCALES, major_scale);
    select_array_in(JIFFLES, pentatonic_desc);
    sync = 2;
    pitch = {32768, 329};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	   26' 18"
    subcycle_octave = -3;	// 3' 17"	| subcycle |
    break;

  case 57:
    title = F("klassisch, schön, als grundlag zu eme 22' 1\" stuck");
    //comment: harmonical cycle    22'  1"
    //comment: jiffle no uusprobiere ;)
    date = F("2018-11-22_11h45m56s	THU");
    select_array_in(SCALES, major_scale);
    select_array_in(JIFFLES, din_dididi);
    sync = 2;
    pitch = {32768, 393};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	   22'  1"
    subcycle_octave = -2;	// 5' 30"	| subcycle |
    break;

  case 58:
    title = F("eifach und eigenartig schön");
    date = F("2018-11-25_16h18m50s	SUN");
    select_array_in(SCALES, pentatonic_minor);
    select_array_in(JIFFLES, pentatonic_rise);
    sync = 5;
    pitch = {32768, 167};
    // random octave shift: // fix:	play_KB_macro  *2
    // CYC: 2^0	2h  9' 30"
    subcycle_octave = -5;	// 4'  3"	| subcycle |
    break;

  case 59:
    title = F("gradnomal wunderschön :)");
    date = F("2018-11-25_16h31m56s	SUN");
    select_array_in(SCALES, europ_PENTAtonic);
    select_array_in(JIFFLES, PENTAtonic_rise);
    sync = 0;
    pitch = {32768, 261};
    // random octave shift: // fix:	play_KB_macro  *4
    // CYC: 2^0	4h 25'  9"
    subcycle_octave = -6;	// 4'  9"	| subcycle |
    break;

  case 60:
    title = F("landing, seed, growth, ....");
    //comment: strong beginning ;)
    date = F("2018-11-25_18h49m01s	SUN");
    select_array_in(SCALES, tetraCHORD);
    select_array_in(JIFFLES, dwn_THRD_up);
    sync = 1;
    pitch = {32768, 247};
    // random octave shift: // fix:	play_KB_macro  /2
    // CYC: 2^0	1h 10'  3"
    subcycle_octave = -4;	// 4' 23"	| subcycle |
    break;

  case 61:
    title = F("Baldrian");
    //comment: entspannig pur
    //comment: try longer cycles ;)
    date = F("2018-11-25_19h14m10s	SUN");
    select_array_in(SCALES, octave_4th_5th);
    select_array_in(JIFFLES, PENTA_3rd_down_5);
    sync = 2;
    pitch = {32768, 296};
    // random octave shift: // fix:	play_KB_macro  *2
    // CYC: 2^0	15h 35' 11"
    subcycle_octave = -8;	// 3' 39"	| subcycle |

  case 62:
    title = F("dorisch");
    date = F("2018-11-25_19h24m24s	SUN");
    select_array_in(SCALES, doric_scale);
    select_array_in(JIFFLES, din_dididi);
    sync = 0;
    pitch = {32768, 219};
    // random octave shift: used SUBCYCLE:	3' 5"
    // CYC: 2^0	1h 38' 45"
    subcycle_octave = -5;	// 3'  5"	| subcycle |
    break;

  case 63:
    title = F("no meh melodie :)");
    date = F("2018-11-27_17h22m21s	TUE");
    select_array_in(SCALES, octave_4th_5th);
    select_array_in(JIFFLES, dwn_THRD_up);
    sync = 0;
    pitch = {32768, 357};
    // random octave shift: used SUBCYCLE:	3' 2"
    // CYC: 2^0	6h 27' 42"
    subcycle_octave = -7;	// 3'  2"	| subcycle |
    break;

  case 64:
    title = F("lost title");
    select_array_in(SCALES, pentatonic_minor);
    select_array_in(JIFFLES, PENTAtonic_rise);
    sync = 4;
    pitch = {32768, 211};
    // random octave shift: // fix:	play_KB_macro  *2
    // used SUBCYCLE:	3' 12"
    // CYC: 2^0	3h 24' 59"
    subcycle_octave = -6;	// 3' 12"	| subcycle |
    break;

  case 65:
    title = F("eigenartigi harmonischi melodiegebilde");
    select_array_in(SCALES, pentatonic_minor);
    select_array_in(JIFFLES, pentatonic_rise);
    pitch = {1, 128};
    sync = 4;
    break;

  case 66:
    title = F("no ohni name");
    date = F("2018-11-28_21h02m41s	WED");
    select_array_in(SCALES, octaves_fifths);
    select_array_in(JIFFLES, PENTA_3rd_rise);
    sync = 3;
    pitch = {32768, 375};
    // random octave shift:
    // CYC: 2^0	5d 11h 13' 47"
    subcycle_octave = -1;	// 0	    7' 41"	| subcycle |
    break;

  case 67:
    title = F("sehr eifach & schön");
    date = F("2018-11-29_14h14m39s	THU");
    select_array_in(SCALES, octaves_fifths);
    select_array_in(JIFFLES, PENTAtonic_desc);
    sync = 2;
    pitch = {32768, 172};
    // random octave shift:
    // CYC: 2^0	5d 23h  3' 20"
    subcycle_octave = -1;	// 1	    4' 11"	| subcycle |
    break;

  case 68:
    title = F("wow");
    date = F("2018-12-01_12h11m13s	SAT");
    select_array_in(SCALES, doric_scale);
    select_array_in(JIFFLES, ding_ditditdit);
    sync = 4;
    pitch = {32768, 288};
    // random octave shift: // fix:	play_KB_macro  *2
    // CYC: 2^0	2h 30' 11"
    subcycle_octave = -5;	// 4' 42"	| subcycle |
    break;

  case 69:
    title = F("schön melodisch, mit recht guetem schluss");
    //comment: de schluss chönnt no chli schneller fertig sii
    date = F("2018-12-02_20h43m32s	SUN");
    select_array_in(SCALES, major_scale);
    select_array_in(JIFFLES, pentatonic_desc);
    sync = 2;
    pitch = {32768, 379};
    // random octave shift: // fix:	play_KB_macro  *4
    // CYC: 2^0	   45' 39"
    subcycle_octave = -3;	// 5' 42"	| subcycle |
    break;

  case 70:
    title = F("PENTAtheme");
    date = F("2018-12-03_08h30m25s	MON");
    select_array_in(SCALES, europ_PENTAtonic);
    select_array_in(JIFFLES, up_THRD);
    sync = 0;
    pitch = {32768, 230};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	2h 30' 26"
    subcycle_octave = -5;	// 4' 42"	| subcycle |
    break;

  case 71:
    title = F("elfechinde");
    date = F("2018-12-04_10h16m55s	TUE");
    select_array_in(SCALES, tetraCHORD);
    select_array_in(JIFFLES, dwn_THRD);
    sync = 3;
    pitch = {32768, 410};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	5h 37' 35"
    subcycle_octave = -6;	// 5' 16"	| subcycle |
    break;

  case 72:
    title = F("elfechinde 2");
    date = F("2018-12-04_10h24m24s	TUE");
    select_array_in(SCALES, tetraCHORD);
    select_array_in(JIFFLES, pent_top_wave);
    sync = 3;
    pitch = {32768, 375};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	6h  9'  5"
    subcycle_octave = -6;	// 5' 46"	| subcycle |
    break;

  case 73:
    title = F("simplistic");
    date = F("2018-12-05_19h18m09s	WED");
    select_array_in(SCALES, octaves_fifths);
    select_array_in(JIFFLES, din_dididi);
    sync = 5;
    pitch = {32768, 256};
    // fix:	play_KB_macro  /2
    // CYC: 2^0	2d 0h  3' 28"
    subcycle_octave = -9;	// 5' 38"	| subcycle |
    break;

  case 74:
    title = F("rhythms");
    //comment: very good example for note-metrum interval analogy :)
    //comment: will be a good test case for a better end algorithm ;)
    date = F("2018-12-06_08h14m01s	THU");
    select_array_in(SCALES, octaves_fourths);
    select_array_in(JIFFLES, tumtum);
    sync = 0;
    pitch = {32768, 413};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	14d 21h 27' 58"
    subcycle_octave = -1;	// 2	    5' 14"	| subcycle |
    break;

  case 75:
    title = F("ruhig schwebend");
    date = F("2018-12-07_09h44m49s	FRI");
    select_array_in(SCALES, pentatonic_minor);
    select_array_in(JIFFLES, PENTA_3rd_down_5);
    sync = 3;
    pitch = {32768, 176};
    // fix:	play_KB_macro  *4
    // CYC: 2^0	4h  5' 45"
    subcycle_octave = -5;	// 7' 41"	| subcycle |
    break;

  case 76:
    title = F("magical tree");
    //comment: maybe as music for the tree woman from Theaterspektakel, Zürich 2018
    date = F("2018-12-07_13h03m35s	FRI");
    select_array_in(SCALES, europ_PENTAtonic);
    select_array_in(JIFFLES, dwn_THRD);
    sync = 1;
    pitch = {32768, 357};
    // CYC: 2^0	   48' 28"
    subcycle_octave = -3;	// 6'  3"	| subcycle |
    break;

  case 77:
    title = F("tanboura");
    date = F("2018-12-07_13h53m29s	FRI");
    select_array_in(SCALES, doric_scale);
    select_array_in(JIFFLES, tanboura);
    sync = 5;
    pitch = {32768, 299};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	2h 24' 39"
    subcycle_octave = -4;	// 9'  2"	| subcycle |
    break;

  case 78:
    title = F("melodischs Netz uf I IV V");
    date = F("2018-12-07_16h22m20s	FRI");
    select_array_in(SCALES, octave_4th_5th);
    select_array_in(JIFFLES, pent_top_wave);
    sync = 3;
    pitch = {32768, 257};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	8h 58' 33"
    subcycle_octave = -6;	// 8' 25"	| subcycle |
    break;

  case 79:
    title = F("Aurora");
    //comment: very dense pulsating web
    //comment: good ending ;)
    date = F("2018-12-07_15h57m55s	FRI");
    select_array_in(SCALES, minor_scale);
    select_array_in(JIFFLES, PENTA_3rd_down_5);
    sync = 3;
    pitch = {32768, 300};
    // CYC: 2^0	1h 12'  5"
    subcycle_octave = -3;	// 9'  1"	| subcycle |
    break;

  case 80:
    title = F("Aurora mit Tänzelde Liechter");
    date = F("2018-12-07_16h32m24s	FRI");
    select_array_in(SCALES, pentatonic_minor);
    select_array_in(JIFFLES, PENTA_3rd_down_5);
    sync = 1;
    pitch = {32768, 195};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	1h 50' 54"
    subcycle_octave = -4;	// 6' 56"	| subcycle |
    break;

  case 81:
    title = F("Melodie Kabinet");
    //comment: wunderschön tänzelnds Melodienetz
    //comment: wo sich langsam au i di undere Stimme uusdehnt
    date = F("2018-12-07_17h23m46s	FRI");
    select_array_in(SCALES, europ_PENTAtonic);
    select_array_in(JIFFLES, PENTA_3rd_rise);
    sync = 3;
    pitch = {32768, 355};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	1h 37' 28"
    subcycle_octave = -4;	// 6'  6"	| subcycle |
    break;

  case 82:
    title = F("Akkordeon");
    date = F("2018-12-07_17h34m18s	FRI");
    select_array_in(SCALES, tetraCHORD);
    select_array_in(JIFFLES, PENTAtonic_desc);
    sync = 5;
    pitch = {32768, 260};
    // fix:	play_KB_macro  /2
    // CYC: 2^0	1h  6' 32"
    subcycle_octave = -3;	// 8' 19"	| subcycle |
    break;

  case 83:
    title = F("MusikdoseTraum");
    //comment: für langi, vertröimti szene
    date = F("2018-12-07_17h45m57s	FRI");
    select_array_in(SCALES, tetraCHORD);
    select_array_in(JIFFLES, din__dididi_dixi);
    sync = 3;
    pitch = {32768, 195};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	5h 54' 53"
    subcycle_octave = -5;	// 11'  5"	| subcycle |
    break;

  case 84:
    title = F("Klangschwade mit Puls");
    date = F("2018-12-07_18h34m01s	FRI");
    select_array_in(SCALES, europ_PENTAtonic);
    select_array_in(JIFFLES, tanboura);
    sync = 0;
    pitch = {32768, 275};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	2h  5' 49"
    subcycle_octave = -4;	// 7' 52"	| subcycle |
    break;

  case 85:
    title = F("Pentaweb");
    //comment: stimmigsvoll
    //comment: mit sehnsucht ufs ziel zue
    sync = 2;
    select_array_in(SCALES, pentatonic_minor);
    select_array_in(JIFFLES,	PENTAtonic_desc);
    pitch = {1, 128};
    // harmonical cycle 1h  9' 59"	subcycle    8' 45"
    break;

  case 86:
    title = F("Schwester Pentaweb");
    //comment: e ZwillingsPentawebSchwöster :)
    date = F("2018-12-07_19h04m44s	FRI");
    select_array_in(SCALES, europ_PENTAtonic);
    select_array_in(JIFFLES, penta_3rd_rise);
    sync = 1;
    pitch = {32768, 423};
    // CYC: 2^0	1h 21' 48"
    subcycle_octave = -3;	// 10' 13"	| subcycle |
    break;

  case 87:


    title = F("PentaMusicBox");
    date = F("2018-12-07_19h16m36s	FRI");
    select_array_in(SCALES, pentatonic_minor);
    select_array_in(JIFFLES, din__dididi_dixi);
    sync = 1;
    pitch = {32768, 187};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	1h 55' 39"
    subcycle_octave = -4;	// 7' 14"	| subcycle |
    break;

  case 88:
    title = F("I_IV_V mit Knatterrhytheme");
    date = F("2018-12-07_19h48m57s	FRI");
    select_array_in(SCALES, octave_4th_5th);
    select_array_in(JIFFLES, d4096_512);
    sync = 1;
    pitch = {32768, 187};
    // CYC: 2^0	6h 10'  4"
    //	subcycle_octave = -5;	// 11' 34"	| subcycle |
    break;

  case 89:
    title = F("The Landing Of The Sound Space Ship");
    date = F("2018-12-07_20h03m19s	FRI");
    select_array_in(SCALES, doric_scale);
    select_array_in(JIFFLES, dwn_THRD_up);
    sync = 1;
    pitch = {32768, 189};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	1h 54' 25"
    subcycle_octave = -4;	// 7'  9"	| subcycle |
    break;

  case 90:
    title = F("Striicherbordun");
    //comment: dichts melodienetz mit tüüfem stehendem bass, wi tüüfi striicher
    date = F("2018-12-09_20h14m56s	SUN");
    select_array_in(SCALES, major_scale);
    select_array_in(JIFFLES, penta_3rd_rise);
    sync = 3;
    pitch = {32768, 170};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	   25' 27"
    subcycle_octave = -2;	// 6' 22"	| subcycle |
    break;

  case 91:
    title = F("simple and beautiful");
    date = F("2018-12-10_17h02m05s	MON");
    select_array_in(SCALES, doric_scale);
    select_array_in(JIFFLES, d4096_1024);
    sync = 2;
    pitch = {32768, 297};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	2h 25' 38"
    subcycle_octave = -5;	// 4' 33"	| subcycle |
    break;

  case 92:
    title = F("de Reige vo de Dezämberfee");
    date = F("2018-12-10_17h27m01s	MON");
    select_array_in(SCALES, pentatonic_minor);
    select_array_in(JIFFLES, dwn_THRD_up);
    sync = 3;
    pitch = {32768, 244};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	2h 57' 16"
    subcycle_octave = -5;	// 5' 32"	| subcycle |
    break;

  case 93:
    title = F("Saiteresonanze");
    //comment: git grad zuefellig wonderschöni Saiteresonanze uf de Gitarre &co
    date = F("2018-12-11_13h29m08s	TUE");
    select_array_in(SCALES, europ_PENTAtonic);
    select_array_in(JIFFLES, pentatonic_rise);
    sync = 5;
    pitch = {32768, 217};
    // CYC: 2^0	1h 19' 44"
    subcycle_octave = -4;	// 4' 59"	| subcycle |
    break;

  case 94:
    title = F("Chindertanz -- Children's Dance");
    //comment: fröhlich gspunnene chindertanz
    date = F("2018-12-13_10h24m06s	THU");
    select_array_in(SCALES, tetraCHORD);
    select_array_in(JIFFLES, rising_pent_them);
    sync = 4;
    pitch = {32768, 302};
    // CYC: 2^0	57' 17"
    subcycle_octave = -4;	// 3' 35"	| subcycle |
  
  case 95:
    title = F("Good Old Major Sync3");
    //comment: use whole cycle :)
    date = F("2018-12-13_10h47m46s	THU");
    select_array_in(SCALES, major_scale);
    select_array_in(JIFFLES, d4096_1024);
    sync = 3;
    // harmonical CYCLE: 13' 11"	SUBCYCLE: | 6' 36" |
    subcycle_octave = -1;
    break;

  case 96:
    title = F("Cosmic SoundWave Visiting Earth");
    date = F("2018-12-13_16h59m40s	THU");
    select_array_in(SCALES, major_scale);
    select_array_in(JIFFLES, PENTA_3rd_down_5);
    sync = 1;
    pitch = {32768, 208};
    // harmonical CYCLE: 20' 48"	SUBCYCLE: | 5' 12" |
    // SUBCYCLE: (2^-2) 5' 12"
    break;

  case 97:
    title = F("Jazzigi Rhythme mit schnarrend schnarchende KnatterKläng");
    date = F("2018-12-13_19h42m01s	THU");
    select_array_in(SCALES, octaves_fifths);
    select_array_in(JIFFLES, diing_ditditdit);
    sync = 2;
    pitch = {32768, 449};
    // harmonical CYCLE: 4d 13h 36' 6"		SUBCYCLE: | 6' 25" |
    subcycle_octave = -10;
    break;

  case 98:
    title = F("Harmonikale Schrittanz");
    date = F("2018-12-13_21h35m38s	THU");
    select_array_in(SCALES, minor_scale);
    select_array_in(JIFFLES, pentatonic_rise);
    sync = 0;
    pitch = {32768, 234};
    // fix:	play_KB_macro  *2
    // harmonical CYCLE: 3h 4' 50"	SUBCYCLE: | 5' 47" |
    subcycle_octave = -5;
    break;

  case 99:
    title = F("Church Bells");
    //comment: es paar Alphörner het s glaub au no ;)
    date = F("2018-12-14_18h24m10s	FRI");
    select_array_in(SCALES, pentatonic_minor);
    select_array_in(JIFFLES, pent_patternA);
    sync = 5;
    pitch = {32768, 431};
    // harmonical CYCLE: 1h 40' 21"	SUBCYCLE: | 6' 16" |
    subcycle_octave = -4;
    break;

  case 100:
    title = F("PENTAtonic Clock Organ");
    date = F("2018-12-14_18h51m01s	FRI");
    select_array_in(SCALES, europ_PENTAtonic);
    select_array_in(JIFFLES, diing_ditditdit);
    sync = 3;
    pitch = {32768, 429};
    // fix:	play_KB_macro  *2
    // harmonical CYCLE: 2h 41' 19"	SUBCYCLE: | 5' 2" |
    break;

  case 101:
    title = F("Good Night Choir");
    date = F("2018-12-14_19h03m53s	FRI");
    select_array_in(SCALES, octaves_fourths);
    select_array_in(JIFFLES, pentatonic_desc);
    sync = 0;
    pitch = {32768, 352};
    // fix:	play_KB_macro  *2
    // harmonical CYCLE: 17d 11h 24' 48"	SUBCYCLE: | 6' 9" |
    subcycle_octave = -12;
    break;

  case 102:
    title = F("Classical Beauty");
    date = F("2018-12-14_19h37m48s	FRI");
    select_array_in(SCALES, doric_scale);
    select_array_in(JIFFLES, d4096_1024);
    sync = 1;
    pitch = {32768, 215};
    // fix:	play_KB_macro  *2
    // harmonical CYCLE: 3h 21' 10"	SUBCYCLE: | 6' 17" |
    subcycle_octave = -5;
    break;

  case 103:
    title = F("PENTA Classic");
    date = F("2018-12-14_19h48m35s	FRI");
    select_array_in(SCALES, europ_PENTAtonic);
    select_array_in(JIFFLES, up_THRD);
    sync = 2;
    pitch = {32768, 333};
    // fix:	play_KB_macro  *2
    // harmonical CYCLE: 1h 43' 55"	SUBCYCLE: | 6' 30" |
    subcycle_octave = -4;
    break;

  case 104:
    title = F("pentatonic reaching down to rhythm");
    date = F("2018-12-15_12h58m14s	SAT");
    select_array_in(SCALES, pentatonic_minor);
    select_array_in(JIFFLES, tumtum);
    sync = 2;
    pitch = {32768, 222};
    // harmonical CYCLE: 1h 37' 25"	SUBCYCLE: | 6' 5" |
    subcycle_octave = -4;
    break;

  case 105:
    title = F("Nameless Beauty");
    date = F("2018-12-15_15h06m14s	SAT");
    select_array_in(SCALES, minor_scale);
    select_array_in(JIFFLES, pentatonic_rise);
    sync = 0;
    pitch = {32768, 223};
    // harmonical CYCLE: 1h 36' 59"	SUBCYCLE: | 6' 4" |
    subcycle_octave = -4;
    break;

  case 106:
    title = F("melodischs gfüeg");
    date = F("2018-12-18_11h56m09s	TUE");
    select_array_in(SCALES, tetraCHORD);
    select_array_in(JIFFLES, rising_pent_them);
    sync = 1;
    pitch = {32768, 420};
    // fix:	play_KB_macro  *2
    // harmonical CYCLE: 5h 29' 32"	SUBCYCLE: | 5' 9" |
    subcycle_octave = -6;
    break;

  case 107:
    title = F("RhythmicalTroubadourBordun");
    date = F("2018-12-19_14h13m01s	WED");
    select_array_in(SCALES, octave_4th_5th);
    select_array_in(JIFFLES, tumtum);
    sync = 0;
    pitch = {32768, 413};
    // fix:	play_KB_macro  *2
    // harmonical CYCLE: 11h 10' 15"	SUBCYCLE: | 5' 14" |
    subcycle_octave = -7;
    break;

  case 108:
    title = F("another melody net");
    date = F("2018-12-19_15h29m35s	WED");
    select_array_in(SCALES, tetraCHORD);
    select_array_in(JIFFLES, pentatonic_rise);
    sync = 5;
    pitch = {32768, 323};
    // fix:	play_KB_macro  /2
    // harmonical CYCLE: 1h 47' 8"	SUBCYCLE: | 6' 42" |
    subcycle_octave = -4;
    break;

  case 109:
    title = F("eifache schöne reige");
    date = F("2018-12-19_15h49m12s	WED");
    select_array_in(SCALES, octave_4th_5th);
    select_array_in(JIFFLES, dwn_THRD_up);
    sync = 4;
    pitch = {32768, 354};
    // fix:	play_KB_macro  *2
    // harmonical CYCLE: 13h 1' 57"	SUBCYCLE: | 6' 7" |
    subcycle_octave = -7;
    break;

  case 110:
    title = F("another nice simple one");
    date = F("2018-12-19_15h42m05s	WED");
    select_array_in(SCALES, minor_scale);
    select_array_in(JIFFLES, d4096_512);
    sync = 5;
    pitch = {32768, 228};
    // fix:	play_KB_macro  /2
    // harmonical CYCLE: 47' 26"	SUBCYCLE: | 5' 56" |
    subcycle_octave = -3;
    break;

  case 111:
    title = F("descending ascending whirles");
    date = F("2018-12-19_15h57m43s	WED");
    select_array_in(SCALES, doric_scale);
    select_array_in(JIFFLES, pentatonic_rise);
    sync = 5;
    pitch = {32768, 373};
    // fix:	play_KB_macro  *2
    // harmonical CYCLE: 1h 55' 57"	SUBCYCLE: | 3' 37" |
    subcycle_octave = -5;
    break;

  case 112:
    title = F("im Zaubergarte");
    date = F("2018-12-20_13h11m49s	THU");
    select_array_in(SCALES, major_scale);
    select_array_in(JIFFLES, penta_3rd_down_5);
    sync = 1;
    pitch = {32768, 262};
    // fix:	play_KB_macro  /2
    // harmonical CYCLE: 8' 15"	SUBCYCLE: | 4' 8" |
    subcycle_octave = -1;
    break;

  case 113:
    title = F("Chläderrose");
    date = F("2018-12-19_20h43m46s	WED");
    select_array_in(SCALES, minor_scale);
    select_array_in(JIFFLES, up_THRD);
    sync = 3;
    pitch = {32768, 400};
    // fix:	play_KB_macro  *2
    // harmonical CYCLE: 1h 48' 8"	SUBCYCLE: | 6' 45" |
    subcycle_octave = -4;
    break;

  case 114:
    title = F("dream angel came from heaven to earth");
    date = F("2018-12-19_22h08m50s	WED");
    select_array_in(SCALES, pentatonic_minor);
    select_array_in(JIFFLES, penta_3rd_down_5);
    sync = 1;
    pitch = {32768, 226};
    // harmonical CYCLE: 1h 35' 41"	SUBCYCLE: | 5' 59" |
    subcycle_octave = -4;
    break;

  case 115:
    title = F("im Zaubergarte");
    date = F("2018-12-20_13h11m49s	THU");
    select_array_in(SCALES, major_scale);
    select_array_in(JIFFLES, penta_3rd_down_5);
    sync = 1;
    pitch = {32768, 262};
    // fix:	play_KB_macro  /2
    // harmonical CYCLE: 8' 15"	SUBCYCLE: | 4' 8" |
    // | (2^-1) 4' 8" |
    break;

  case 116:
    title = F("relax and get ready");
    date = F("2018-12-20_16h05m35s	THU");
    select_array_in(SCALES, tetraCHORD);
    select_array_in(JIFFLES, pentatonic_desc);
    sync = 0;
    pitch = {32768, 411};
    // fix:	play_KB_macro  *2
    // harmonical CYCLE: 5h 36' 45"	SUBCYCLE: | 5' 16" |
    subcycle_octave = -6;
    break;

  case 117:
    title = F("birds and other musicians calling");
    date = F("2018-12-20_16h54m10s	THU");
    select_array_in(SCALES, octaves_fifths);
    select_array_in(JIFFLES, pentatonic_desc);
    sync = 4;
    pitch = {32768, 271};
    // harmonical CYCLE: 3d 18h 47' 44"	SUBCYCLE: | 5' 19" |
    subcycle_octave = -10;
    break;

  case 118:
    title = F("triad tanboura");
    //comment: nice for a while
    //comment: can get boring ;)
    date = F("2018-12-21_11h54m41s	FRI");
    select_array_in(SCALES, triad);
    select_array_in(JIFFLES, tanboura);
    sync = 3;
    pitch = {32768, 248};
    // fix:	play_KB_macro  /2
    // harmonical CYCLE: 3h 52' 32"	SUBCYCLE: | 3' 38" |
    subcycle_octave = -6;
    break;

  case 119:
    title = F("slow dreams");
    date = F("2018-12-21_18h55m55s	FRI");
    select_array_in(SCALES, europ_PENTAtonic);
    select_array_in(JIFFLES, dwn_THRD);
    sync = 2;
    pitch = {32768, 300};
    // fix:	play_KB_macro  *2
    // harmonical CYCLE: 1h 55' 20"	SUBCYCLE: | 3' 36" |
    subcycle_octave = -5;
    break;

  case 120:
    title = F("schlichte Tanz");
    date = F("2018-12-21_19h57m09s	FRI");
    select_array_in(SCALES, pentaCHORD);
    select_array_in(JIFFLES, din_dididi);
    sync = 2;
    pitch = {32768, 416};
    // fix:	play_KB_macro  *2
    // harmonical CYCLE: 2h 46' 21"	SUBCYCLE: | 5' 12" |
    subcycle_octave = -5;
    break;

  case 121:
    title = F("schlicht ruhends muster, friedlich, erwartigsvoll, lebensfroh");
    date = F("2018-12-22_09h52m20s	SAT");
    select_array_in(SCALES, octaves_fourths);
    select_array_in(JIFFLES, PENTAtonic_desc);
    sync = 2;
    pitch = {32768, 350};
    // fix:	play_KB_macro  *2
    // harmonical CYCLE: 17d 13h 48' 36"	SUBCYCLE: | 6' 11" |
    subcycle_octave = -12;
    break;

  case 122:
    title = F("und es ratteret und brummet tickend de motor");
    date = F("2018-12-22_10h34m45s	SAT");
    select_array_in(SCALES, octaves_fourths);
    select_array_in(JIFFLES, d4096_3072);
    sync = 0;
    pitch = {32768, 188};
    // fix:	play_KB_macro  *2
    // harmonical CYCLE: 16d 8h 38' 32"	SUBCYCLE: | 5' 45" |
    subcycle_octave = -12;
    break;

  case 123:
    title = F("pentatonische wasserfall");
    date = F("2018-12-22_15h51m27s	SAT");
    select_array_in(SCALES, pentaCHORD);
    select_array_in(JIFFLES, pentatonic_desc);
    sync = 5;
    pitch = {32768, 181};
    // fix:	play_KB_macro  *2
    // harmonical CYCLE: 1h 35' 35"	SUBCYCLE: | 5' 58" |
    subcycle_octave = -4;
    break;

  case 124:
    title = F("brummbärs ratterkiste");
    date = F("2018-12-22_15h58m50s	SAT");
    select_array_in(SCALES, pentatonic_minor);
    select_array_in(JIFFLES, tumtum);
    sync = 2;
    pitch = {32768, 257};
    // fix:	play_KB_macro  *2
    // harmonical CYCLE: 2h 48' 18"	SUBCYCLE: | 5' 16" |
    subcycle_octave = -5;
    break;

  case 125:
    title = F("a firework of sounds");
    date = F("2018-12-22_18h52m33s	SAT");
    select_array_in(SCALES, major_scale);
    select_array_in(JIFFLES, PENTAtonic_rise);
    sync = 1;
    pitch = {32768, 428};
    // fix:	play_KB_macro  /2
    // harmonical CYCLE: 10' 6"	SUBCYCLE: | 5' 3" |
    // | (2^-1) 5' 3" |
    break;

  case 126:
    title = F("firework #2");
    date = F("2018-12-22_19h03m01s	SAT");
    select_array_in(SCALES, europ_PENTAtonic);
    select_array_in(JIFFLES, din_dididi);
    sync = 5;
    pitch = {32768, 354};
    // fix:	play_KB_macro  /2
    // harmonical CYCLE: 24' 26"	SUBCYCLE: | 6' 7" |
    subcycle_octave = -2;
    break;

  case 127:
    title = F("acoustic telegraph lines");
    date = F("2018-12-22_20h17m39s	SAT");
    select_array_in(SCALES, octaves_fourths);
    select_array_in(JIFFLES, rising_pent_them);
    sync = 5;
    pitch = {32768, 322};
    // harmonical CYCLE: 9d 13h 14' 40"	SUBCYCLE: | 6' 43" |
    subcycle_octave = -11;
    break;

  case 128:
    title = F("clear winter sky");
    date = F("2018-12-23_18h45m37s	SUN");
    select_array_in(SCALES, pentatonic_minor);
    select_array_in(JIFFLES, PENTAtonic_desc);
    sync = 1;
    pitch = {32768, 444};
    // harmonical CYCLE: 1h 37' 25"	SUBCYCLE: | 6' 5" |
    subcycle_octave = -4;
    break;

  case 129:
    title = F("the mad old jazz piano man");
    date = F("2018-12-23_18h53m20s	SUN");
    select_array_in(SCALES, pentatonic_minor);
    select_array_in(JIFFLES, tumtumtum);
    sync = 4;
    pitch = {32768, 398};
    // fix:	play_KB_macro  *2
    // harmonical CYCLE: 1h 48' 40"	SUBCYCLE: | 6' 48" |
    subcycle_octave = -4;
    break;

  case 130:
    title = F("nameless beauty");
    date = F("2018-12-24_20h35m07s	MON");
    select_array_in(SCALES, trichord);
    select_array_in(JIFFLES, pentatonic_rise);
    sync = 2;
    pitch = {32768, 286};
    // fix:	play_KB_macro  *2
    // harmonical CYCLE: 10h 45' 15"	SUBCYCLE: | 5' 2" |
    subcycle_octave = -7;
    break;

  case 131:
    title = F("penta trichord");
    date = F("2018-12-24_20h49m58s	MON");
    select_array_in(SCALES, trichord);
    select_array_in(JIFFLES, PENTAtonic_desc);
    sync = 2;
    pitch = {32768, 288};
    // fix:	play_KB_macro  *2
    // harmonical CYCLE: 10h 40' 46"	SUBCYCLE: | 5' 0" |
    subcycle_octave = -7;
    break;

  case 132:
    title = F("Santa Claus");
    date = F("2018-12-25_19h16m25s	TUE");
    select_array_in(SCALES, octave_4th_5th);
    select_array_in(JIFFLES, penta_3rd_rise);
    sync = 1;
    // fixed random tuning b	233
    pitch = {32768, 233};
    // fix:	play_KB_macro  *2
    // harmonical CYCLE: 9h 54' 1"	SUBCYCLE: | 4' 38" |
    subcycle_octave = -7;
    break;

  case 133:
    title = F("2018-12-25_20h28m58s (recorded)");
    date = F("2018-12-25_20h28m58s	TUE");
    select_array_in(SCALES, doric_scale);
    select_array_in(JIFFLES, din__dididi_dix);
    sync = 2;
    pitch = {32768, 32};
    // harmonical CYCLE: 2h 48' 57"	SUBCYCLE: | 5' 17" |
    subcycle_octave = -5;
    title = F("sound waves like ever descending rain");
    date = F("2019-01-04_18h44m09s	FRI");
    select_array_in(SCALES, tetraCHORD);
    select_array_in(JIFFLES, PENTAtonic_desc);
    sync = 2;
    pitch = {32768, 234};
    // harmonical CYCLE: 2h 27' 52"	SUBCYCLE: | 4' 37" |
    subcycle_octave = -5;
    break;

  case 134:
    title = F("Childrens Christmas Night");
    date = F("2018-12-25_19h42m27s	TUE");
    select_array_in(SCALES, pentaCHORD);
    select_array_in(JIFFLES, din__dididi_dixi);
    sync = 4;
    pitch = {32768, 128};
    // fix:	play_KB_macro  *2
    // harmonical CYCLE: 2h 15' 10"	SUBCYCLE: | 4' 13" |
    subcycle_octave = -5;
    break;

  case 135:
    title = F("daydreams");
    date = F("2019-01-06_16h16m57s	SUN");
    select_array_in(SCALES, pentatonic_minor);
    select_array_in(JIFFLES, penta_3rd_down_5);
    sync = 3;
    pitch = {32768, 301};
    // fix:	play_KB_macro  *2
    // harmonical CYCLE: 1h 11' 51"	SUBCYCLE: | 4' 29" |
    subcycle_octave = -4;
    break;

  case 136:
    title = F("Harmonical Musicbox alpha 0.010");
    sync = 5;
    select_array_in(SCALES, pentaCHORD);
    select_array_in(JIFFLES, tumtum);
    pitch = {1, 128};
    // harmonical CYCLE: 51' 48"	SUBCYCLE: | 6' 28" |
    break;

  case 137:
    title = F("TRIAD");
    date = F("2019-01-06_22h12m45s	SUN");
    select_array_in(SCALES, TRIAD);
    select_array_in(JIFFLES, rising_pent_them);
    sync = 1;
    pitch = {32768, 428};
    // fix:	play_KB_macro  /2
    // harmonical CYCLE: 3h 35' 35"	SUBCYCLE: | 6' 44" |
    subcycle_octave = -5;
    break;

  case 138:
    title = F("rhythm tuned in");
    //comment: very nice, simple octaves_fourths pattern, deep down to rhythmical roots
    date = F("2019-01-08_08h46m00s	TUE");
    select_array_in(SCALES, octaves_fourths);
    select_array_in(JIFFLES, din_dididi);
    sync = 0;
    // fixed random tuning a
    pitch = {32768, 220};
    // harmonical CYCLE: 6d 23h 45' 55"	SUBCYCLE: | 4' 55" |
    subcycle_octave = -11;
    break;

  case 139:
    title = F("2019-02-01 ardour5 noname");
    sync = 1;
    select_array_in(JIFFLES, PENTA_3rd_rise);
    pitch = {1, 32};	//	*metric* ?
    // harmonical CYCLE: 1h 24' 29"	SUBCYCLE: | 5' 17" |
    break;

  case 140:
    title = F("2019-02-02 ardour5 noname");
    date = F("2019-02-02_17h49m06s	SAT");
    select_array_in(SCALES, doric_scale);
    select_array_in(JIFFLES, pentatonic_rise);
    sync = 0;
    pitch = {32768, 32};
    // harmonical CYCLE: 2h 48' 57"	SUBCYCLE: | 3' 41" |
    // | 2' 38" |
    break;

  case 141:
    title = F("2019-02-04_doric_cycle_S1");
    //comment: ardour 5
    sync = 1;
    select_array_in(JIFFLES, d4096_1024);
    pitch = {1, 32};	//		*metric* a
    // harmonical CYCLE: 2h 48' 57"	SUBCYCLE: | 2' 38" |
    break;

  case 142:
    title = F("2019-02-04_bordun");
    //comment: ardour5
    select_array_in(SCALES, octave_4th_5th);
    sync = 0;
    select_array_in(JIFFLES, tumtumtum);
    pitch = {2, 61};
    // harmonical CYCLE: 6h 0' 26"	SUBCYCLE: | 2' 38" |
    break;

  case 143:
    title = F("2019-02-06_major_doric_rise");
    //comment: ardour 5
    date = F("2019-02-06_13h32m52s	WED");
    select_array_in(SCALES, major_scale);
    select_array_in(JIFFLES, doric_rise);
    sync = 4;
    pitch = {32768, 32};
    // harmonical CYCLE: 33' 47"	SUBCYCLE: | 5' 31" |
    // | 5' 31" |
    break;

  case 144:
    title = F("2019-02-6_tumtum_pentatonic_minor_S2");
    date = F("2019-02-06_15h44m39s	WED");
    select_array_in(SCALES, pentatonic_minor);
    select_array_in(JIFFLES, tumtum);
    sync = 2;
    // random *metric* tuning a
    pitch = {32768, 220};
    // fix:	play_KB_macro  *2
    // harmonical CYCLE: 1h 38' 18"	SUBCYCLE: | 6' 9" |
    subcycle_octave = -4;
    break;

  case 145:
    title = F("zagg  2019-02-06_tetraCHORD_pentachord_rise_S1");
    select_array_in(JIFFLES, pentaCHORD_rise);
    pitch = {3, 10};	// 24	*metric* A
    // harmonical CYCLE: 0"	SUBCYCLE: | 4' 13" |
    date = F("2019-02-06_20h31m52s	WED");
    select_array_in(SCALES, tetraCHORD);
    select_array_in(JIFFLES, pentaCHORD_rise);
    sync = 1;
    pitch = {32768, 1024};
    // harmonical CYCLE: 2h 15' 10"	SUBCYCLE: | 4' 13" |
    subcycle_octave = -5;
    break;

  case 146:
    title = F("2019-02-06_RG3_S0_d64_melody-rhythm");
    pitch = {2, 61};	// 44	*metric* G
    // harmonical CYCLE: 0"	SUBCYCLE: | 5' 38" |
    date = F("2019-02-06_21h18m28s	WED");
    select_array_in(SCALES, octave_4th_5th);
    select_array_in(JIFFLES, d4096_64);
    sync = 0;
    pitch = {32768, 6144};
    // harmonical CYCLE: 6h 0' 26"	SUBCYCLE: | 5' 38" |
    subcycle_octave = -6;
    break;

  case 147:
    title = F("noname_a");
    select_array_in(SCALES, octave_4th_5th);
    sync = 0;
    select_array_in(JIFFLES, d4096_64);
    pitch = {2, 61};	// 44	*metric* d
    // harmonical CYCLE: 0"	SUBCYCLE: | 3' 41" |
    break;

  case 148:
    title = F("noname_b");
    select_array_in(SCALES, tetraCHORD);
    sync = 1;
    select_array_in(JIFFLES, pentachord_rise);
    pitch = {3, 10};	// 24	*metric* a
    // harmonical CYCLE: 2h 37' 17"	SUBCYCLE: | 4' 55" |
    break;

  case 149:
    title = F("noname_c");
    select_array_in(SCALES, europ_PENTAtonic);
    sync = 4;
    select_array_in(JIFFLES, tumtumtumtum);
    pitch = {1, 12};	// 8	*metric* f
    // harmonical CYCLE: 49' 26"	SUBCYCLE: | 6' 11" |
    break;

  case 150:
    title = F("simplicistic_d0");
    select_array_in(SCALES, octaves_fourths);
    sync = 2;
    select_array_in(JIFFLES, din_dididi);
    pitch = {3, 52};	// 4288		*metric* g
    // harmonical CYCLE: 7d 20h 18' 29"	SUBCYCLE: | 5' 31" |
    break;

  case 151:
    title = F("2019-02-07_classical_time_machine_S4");
    //comment: resonating strings not really tuned...
    //comment: retuned version with problems in ending...
    date = F("2019-02-07_09h46m29s	THU");
    select_array_in(SCALES, minor_scale);
    select_array_in(JIFFLES, d4096_1024);
    sync = 4;
    // random *metric* tuning c
    pitch = {32768, 262};
    // harmonical CYCLE: 1h 22' 33"	SUBCYCLE: | 5' 10" |
    subcycle_octave = -4;
    break;

  case 152:
    title = F("noname_");
    //comment: try variations in SYNC :)
    select_array_in(SCALES, europ_PENTAtonic);
    sync = 5;
    select_array_in(JIFFLES, pentaCHORD_rise);
    pitch = {1, 12};	// 8  1/175	*metric* f
    // harmonical CYCLE: 49' 26"	SUBCYCLE: | 6' 11" |
    break;

  case 153:
    title = F("eifach wunderschön");
    //comment: sehr ruhig
    date = F("2019-02-26_12h46m19s	TUE");
    select_array_in(SCALES, doric_scale);
    sync = 1;
    select_array_in(JIFFLES, diing_ditditdit);
    pitch = {32768, 12};	// 8 1/330 metric e
    // harmonical CYCLE: 3h 18' 35"	SUBCYCLE: | 6' 12" |
    break;

  case 154:
    title = F("2019-04-01 doric stack");
    select_array_in(SCALES, doric_scale);
    sync = 1;
    stack_sync_slices = 8;
    base_pulse = 15;
    select_array_in(JIFFLES, din__dididi_dixi);
    pitch = {32768, 12};	// 8 1/175 metric f
    // harmonical CYCLE: 3h 7' 14"	2^4 SUBCYCLE: | 11' 42" |
    break;

  case 155:
    title = F("Marianne 1	eingesperrt<->Freiheit	sync? 4|1024");
    /*
      in einer Gefängniszelle
      mit Vögel draussen
      Eingesperrt sein | Freiheit
    */
    select_array_in(SCALES, tetraCHORD);
    select_array_in(JIFFLES,tetraCHORD_rise);
    sync = 4;		// S unleserlich, 4?
    stack_sync_slices=1024;
    // p[15]|1024	// unleserlich, 1024?
    chromatic_pitch = 0;
    pitch={1, 294};		// d just a random default
    break;

  case 156:
    title = F("Marianne 2	warten");
    /*
      warten
      Langeweile
    */
    select_array_in(SCALES, pentatonic_minor);
    select_array_in(JIFFLES, ding_ditditdit);
    sync=0;
    stack_sync_slices=64;
    pitch={1, 175};		// f
    chromatic_pitch = 9;	// f
    // 11'42"
    break;

  case 157:
    title = F("Marianne 3	s erst Mal underwägs");
    /*
      Bild: Spital in Shri Lanka
      ein Mann ist das allererste Mal mit seinen Prothesen unterwegs
    */
    select_array_in(SCALES, doric_scale);
    select_array_in(JIFFLES, d4096_128);
    sync=1;
    stack_sync_slices=128;
    pitch={1, 330};
    chromatic_pitch = 4;		// e
    // 6'12"
    break;

  case 158:
    title = F("Marianne 4	Chatzemusik!");
    /*
      Chatzemusik!
      Mariannes Katze kommt, als das Stück läuft...
    */
    select_array_in(SCALES, pentaCHORD);	// unleserlich,,,
    select_array_in(JIFFLES, pent_top_wave_0);
    sync=2;
    stack_sync_slices=0;
    pitch={1, 175};		// f
    chromatic_pitch = 9;		// f
    // 9'22"
    break;

  case 159:
    title = F("Marianne 5	Gefängnisgeburt");
    /*
      eine Freundin von Marianne bekommt im Gefängnis ihr Baby
    */
    select_array_in(SCALES, octave_4th_5th);
    select_array_in(JIFFLES, simple_theme);
    sync=3;
    stack_sync_slices=0;
    pitch={1, 294};		// d
    chromatic_pitch = 6;		// d
    // zeit unleserlich
    break;

  case 160:
    title = F("rising ostinatos over rhythmic bordun");
    //comment: very nice ending :)
    sync = 0;
    select_array_in(SCALES, octave_4th_5th);
    select_array_in(JIFFLES, pentatonic_rise);
    pitch = {2, 6144};
    // harmonical cycle 'c'	12h 24'  7"	subcycle    5' 49"
    break;

  case 161:
    title = F("au sehr schön:");
    sync = 2;
    select_array_in(SCALES, tetraCHORD);
    select_array_in(JIFFLES, PENTA_3rd_rise);
    pitch = {3, 1024};
    break;

  case 162:
    title = F("klassisch und simpel schön");
    sync = 0;
    select_array_in(SCALES, tetraCHORD);
    select_array_in(JIFFLES, pentatonic_desc);
    pitch = {3, 1024};
    break;

  case 163:
    title = F("fractal time web");
    date = F("2018-12-05 15h40m");
    sync = 1;
    select_array_in(SCALES, doric_scale);
    select_array_in(JIFFLES, up_THRD_dn);
    pitch = {1, 32};
  break;

  case 164:
    title = F("busy soundscape");
    date = F("");
    sync = 5;
    stack_sync_slices = 16;
    select_array_in(SCALES,europ_PENTAtonic );
    select_array_in(JIFFLES, pent_patternA);
    pitch = {1, 262};	// metric c
    chromatic_pitch = 4;
    break;

  case 165:
    title = F("stacked clicks");
    //comment: interesting start ;)
    select_array_in(SCALES,europ_PENTAtonic);
    select_array_in(JIFFLES,d4096_32);
    sync=0;
    stack_sync_slices=64;
    base_pulse=15;
    pitch={1, 262};
    chromatic_pitch=4;	// c
    // subcycle_octave = -1;	// TODO: select a short piece ################
    break;

  default:
    valid_selection = false;
  }

  if(valid_selection) {
    HARMONICAL.reduce_fraction(&pitch);		// ;)

    MENU.out(input_value);
    if(title) {
      MENU.tab();
      MENU.out(title);
    }
    if(date) {
      MENU.tab();
      MENU.out(date);
    }
    MENU.ln();

    parameters_by_user();
    start_musicBox();
  } else
    MENU.outln_invalid();
}
