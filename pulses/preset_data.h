/*
  preset_data.h

  also see: preset-hashtags.txt
*/

#define MUSICBOX_PRESETs	170	// default: all presets, can get replaced by SHORT_PRESET_COLLECTION
#define SHORT_PRESET_COLLECTION	104	// only the first <nn> presets are at choice, redefines MUSICBOX_PRESETs

bool /*error*/ load_preset(int new_preset) {	// TODO: sets preset, how to unset? ################
  if(new_preset < 1 || new_preset >= MUSICBOX_PRESETs)
    return true;	// *ERROR*

  preset = new_preset;
  MENU.out(F("load preset "));
  MENU.out(preset);

  name=NULL;		// initialisation and defaults:
  date=NULL;
  octave_shift = 0;
  pitch = {1, 1};	// TEST: ok to set as default?

  PULSES.time_unit = 1000000;	// defaults:
  stack_sync_slices = 0;
  chromatic_pitch = 0;

  switch(preset) {
  case 1:
    name = F("wunderschön melodisch :) :) :)");
    //comment: mit schönem, aberendlosem schluss ;)
    //# FAVORITE
    //# melodies
    //# harmonie
    //# shortIsOk
    //# dropouts
    //# aesthetic	beware of dropouts
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
    name = F("LIFT OFF");
    //comment: :) :) :) :) :)
    //# FAVORITE
    //# shortIsOk
    //# favorite
    //# mystic
    //# SoundSpaceShip
    //# cosmic
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
    name = F("Blue Night Cadence");
    //# FAVORITE
    //# harmonie	a bit like some jazz chord progressions
    //# shortIsOk
    //# systemStress seems cured :)
    //# aesthetic
    //# birds
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
    name = F("harpsichord");
    //# FAVORITE
    //# shortIsOk
    //# melodies
    //# harmonies
    //# classicHarmonical
    //# nice
    //# cosmic
    //# aesthetic
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

  case 5:
    name = F("dragons dancing in the sky");
    //comment: :) :) :)
    //# FAVORITE
    //# melodies
    //# shortIsOk
    //# mystic
    //# aurora
    //# lesley	a bit
    //# aesthetic
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
    name = F("aurora elfetanz");
    //# FAVORITE
    //# melodies
    //# shortIsOk
    //# dense	a bit
    //# aurora
    //# lesley
    //# mystic
    //# fairies
    //# aesthetic
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
    name = F("klassisch, wunderschön, vo A bis Z :)");
    //comment: und übrigens:	harmonical cycle    12' 19"
    //# FAVORITE
    //# harmonies
    //# shortIsOk
    //# classicHarmonical
    //# cosmic
    //# nice
    //# simple
    //# aesthetic
    date = F("2018-11-18_20h09m42s	SUN");
    select_array_in(SCALES, major_scale);
    select_array_in(JIFFLES, din_dididi);
    sync = 2;
    pitch = {32768, 351};
    // CYC: 2^0	   12' 19"
    subcycle_octave = -2;	// 3'  5"	| subcycle |
    break;

  case 8:
    name = F("wunderschöns obenabe vogelgetzwitscher bistüüfinbass :)");
    //# FAVORITE
    //# melodies
    //# shortIsOk
    //# aurora	some
    //# lesley	a bit
    //# birds
    //# mystic
    //# nice
    //# simple
    //# happy
    //# aesthetic
    date = F("2018-11-06_15h52m17s	TUE");
    select_array_in(SCALES, octaves_fifths);
    select_array_in(JIFFLES, dwn_THRD_up);
    sync = 5;
    pitch = {32768, 322};
    break;

  case 9:
    name = F("e rechts füürwerch a jubelnde wirbel oderso :)");
    //# FAVORITE
    //# melodies
    //# shortIsOk
    //# dense
    //# aurora
    //# lesley
    //# mystic
    //# cosmic
    //# aesthetic
    date = F("2018-11-07_17h36m34s	WED");
    select_array_in(SCALES, major_scale);
    select_array_in(JIFFLES, penta_3rd_down_5);
    sync = 1;
    pitch = {32768, 229};
    // CYC: 2^0	   18' 53"
    break;

  case 10:
    name = F("2019-02-01_aRainOfJoy");
    //# FAVORITE
    //# shortIsOk
    //# melodies
    //# harmonies
    //# lesley
    //# happy
    //# mystic
    //# soundscape
    //# birds
    //# fairies
    //# aurora
    //# aesthetic
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

  case 11:
    name = F("genial jazzpiano version");
    // big bang
    //# FAVORITE
    //# shortIsOk
    //# melodies
    //# jazz
    //# dramatic
    //# crazy
    //# dense
    //# chaotic
    //# aesthetic	chaotic
    select_array_in(SCALES, pentachord);
    sync = 0;
    select_array_in(JIFFLES, tumtumtum);
    pitch = {1, 12};	// 8	*metric* a
    // harmonical CYCLE: 6h 33' 12"	SUBCYCLE: | 6' 9" |
    break;

  case 12:
    name = F("a love like the morning birds");
    //# FAVORITE
    //# melodies
    //# harmonies
    //# shortIsOk
    //# birds
    //# happy
    //# simple
    //# aesthetic
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
    name = F("sehr ruhigs festlichs stimmigsbild");
    //# FAVORITE
    //# shortIsOk
    //# melodies
    //# harmonies
    //# classicHarmonical
    //# nice
    //# simple
    //# cosmic
    //# aesthetic
    //comment: bordunartig, stehend
    date = F("2018-11-06_20h02m08s	TUE");
    select_array_in(SCALES, major_scale);
    select_array_in(JIFFLES, diing_ditditdit);
    sync = 5;
    pitch = {32768, 417};
    // fix:	play_KB_macro  *2
    break;

  case 14:
    name = F("ewig tickends uhr gloggespiel");
    //# FAVORITE ???
    //# shortIsOk
    //# simple
    //# simlicistic
    //# clock
    //# elementary
    date = F("2018-11-07_17h49m47s	WED");
    select_array_in(SCALES, octave_4th_5th);
    select_array_in(JIFFLES, diing_ditditdit);
    sync = 1;
    pitch = {32768, 410};
    // CYC: 2^0	5h 37' 35"
    break;

  case 15:
    name = F("tickende uhr");
    //# clock
    //# favorite+
    //# shortIsOk
    //# classicHarmonical
    //# simlicistic
    //# elementary
    date = F("2018-11-06_20h48m16s	TUE");
    select_array_in(SCALES, octaves_fifths);
    select_array_in(JIFFLES, ding_ditditdit);
    sync = 1;
    pitch = {32768, 214};
    // fix:	play_KB_macro  *2
    break;

  case 16:
    name = F("langsams iiordne in es ruhigs piipskonzert");
    //# favorite+
    //# shortIsOk
    //# harmonies
    //# classicHarmonical
    //# nice
    //# simple
    //# simlicistic
    //# elementary
    //# peep
    date = F("2018-11-08_18h05m59s	THU");
    select_array_in(SCALES, octaves_fifths);
    select_array_in(JIFFLES, din__dididi_dixi);
    sync = 3;
    // fixed tuning e	330
    pitch = {32768, 330};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	12d 10h 14' 58"
    break;

  case 17:
    name = F("Ouverture");
    //comment: :) :) :) !!!
    //# favorite+
    //# dropouts
    //# heavyStart
    //# rhytm&pulse
    date = F("2018-12-09_15h24m13s	SUN");
    select_array_in(SCALES, octave_4th_5th);
    select_array_in(JIFFLES, PENTAtonic_rise);
    sync = 0;
    pitch = {32768, 375};
    // CYC: 2^0	6h  9'  5"
    subcycle_octave = -5;	// 11' 32"	| subcycle |
    break;

  case 18:
    name = F("klassisch TIMEMACHINE style");
    // :)
    //# FAVORITE ???
    //# harmonies
    //# shortIsOk
    //# systemStress	just occasionally
    //# classicHarmonical
    //# nice
    //# simple
    //# aesthetic
    date = F("2018-11-07_18h20m53s	WED");
    select_array_in(SCALES, minor_scale);
    select_array_in(JIFFLES, ding_ditditdit);
    sync = 2;
    pitch = {32768, 359};
    // no_octave_shift = true;
    // fix:	play_KB_macro  *2
    // CYC: 2^0	2h  0' 29"
    break;

  case 19:
    name = F("simple, hübsche aafang");
    //# bigBang	is ok
    //# FAVORITE ???
    //# shortIsOk
    //# bigBang
    //# melodies
    //# harmonies
    //# classicHarmonical
    //# nice
    //# simple
    //# aesthetic
    date = F("2018-11-08_14h38m32s	THU");
    select_array_in(SCALES, pentatonic_minor);
    select_array_in(JIFFLES, ding_ditditdit);
    sync = 0;
    pitch = {32768, 210};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	3h 25' 58"
    break;

  case 20:
    name = F("nice, simple slow harmonics");
    //# FAVORITE ???
    //# shortIsOk
    //# melodies
    //# harmonies
    //# aesthetic
    //# nice
    //# simple
    //# birds
    //# hasSlowPulse
    //# lyric
    date = F("2018-11-13_15h04m11s	TUE");
    select_array_in(SCALES, octaves_fifths);
    select_array_in(JIFFLES, penta_3rd_rise);
    sync = 2;
    // fixed tuning f	175
    pitch = {32768, 175};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	11d 17h 12' 24"
    break;

  case 21:
    name = F("klassischi brachemagie");
    //# FAVORITE ???
    //# shortIsOk
    //# melodies
    //# harmonies
    //# aesthetic
    //# nice
    //# simple
    //# birds
    date = F("2018-11-13_12h23m54s	TUE");
    select_array_in(SCALES, europ_PENTAtonic);
    select_array_in(JIFFLES, pentatonic_rise);
    sync = 5;
    // fixed tuning b	233
    pitch = {32768, 233};
    // CYC: 2^0	1h 14' 15"
    break;

  case 22:
    name = F("hübsche start mit geknatter");
    //# FAVORITE ???-	has dropouts
    //# bigBang	is ok
    //# shortIsOk
    //# melodies
    //# harmonies
    //# dropouts
    //# systemStress
    //# nice
    //# simple
    //# rumble
    //# aesthetic
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
    name = F("fröhlichs, witzigs Tänzli :)");
    //# favorite
    //# shortIsOk
    //# crazy
    //# funny
    //# chaotic	a bit
    date = F("2018-11-08_14h27m25s	THU");
    select_array_in(SCALES, major_scale);
    select_array_in(JIFFLES, simple_theme);
    sync = 5;
    pitch = {32768, 380};
    // fix:	play_KB_macro  *2
    break;

  case 24:
    name = F("wienen rägeboge vo striicher wo abeschwebed uf t erde bis tüüf in bass und meh und meh i en zyclische reige iistimmed");
    //# FAVORITE ???
    //# shortIsOk
    //# fullCycleIsOk		maybe
    //# motives
    //# melodies
    //# harmonies
    //# aesthetic
    //# nice
    //# happy
    //# strange	very
    //# dense		but coordinated
    //# chaotic	not really
    //# mystic
    //# SoundSpaceShip
    //# fractal
    //# soundscape
    //# fairies
    //# aurora
    //# lesley
    //# hasSlowPulse
    //# lyric
    date = F("2018-11-21_13h23m41s	WED");
    select_array_in(SCALES, doric_scale);
    select_array_in(JIFFLES, rising_pent_them);
    sync = 1;
    pitch = {32768, 295};
    // CYC: 2^0	1h 13' 19"
    subcycle_octave = -4;	// 4' 35"	| subcycle |
    break;

  case 25:
    name = F("e psinnlichi ziit");	// some morningbirds too
    //# FAVORITE ???
    //# shortIsOk
    //# fullCycleIsOk		maybe
    //# motives
    //# melodies
    //# harmonies
    //# aesthetic
    //# nice
    //# happy
    //# strange	quite
    //# systemStress
    //# dense		but coordinated
    //# chaotic	not really
    //# mystic
    //# SoundSpaceShip
    //# fractal
    //# soundscape
    //# fairies
    //# aurora
    //# lesley
    //# hasSlowPulse
    //# lyric
    date = F("2018-11-21_14h33m31s	WED");
    select_array_in(SCALES, octave_4th_5th);
    select_array_in(JIFFLES, pentatonic_desc);
    sync = 2;
    pitch = {32768, 354};
    // fix:	play_KB_macro  *4
    // CYC: 2^0	1d 2h  3' 55"
    subcycle_octave = -9;	// 3'  3"	| subcycle |
    break;

  case 26:
    name = F("en andere brachklassiker");
    //# FAVORITE ???
    //# shortIsOk
    //# harmonies
    //# nice
    //# lesley
    //# SoundSpaceShip
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
    name = F("nomal en timemachine klassiker");
    //# FAVORITE ???
    //# shortIsOk
    //# melodies
    //# harmonies
    //# classicHarmonical
    //# aesthetic
    //# nice
    //# simple
    //# fractal
    //# humming
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
    name = F("PENT eifache reige");
    //# FAVORITE ???
    //# shortIsOk
    //# melodies
    //# harmonies
    //# classicHarmonical
    //# aesthetic
    //# nice
    //# simple
    //# cosmic
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
    name = F("triggered");
    //# FAVORITE ???
    //# shortIsOk
    //# melodies
    //# harmonies
    //# aesthetic	complex
    //# nice		;)
    //# simple
    //# SoundSpaceShip
    //# hasSlowPulse
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
    name = F("short cycled !");	// TODO: check start
    //# FAVORITE ???
    //# bigBang
    //# heavyStart	bur passes :)
    //# fullCycleIsOk	// TODO: test
    //# shortIsOk
    //# strange
    //# harmonies
    //# mystic
    //# aesthetic	wide
    date = F("2018-11-14_19h47m20s	WED");
    select_array_in(SCALES, major_scale);
    select_array_in(JIFFLES, pentatonic_rise);
    sync = 0;
    // fixed tuning e	330
    pitch = {32768, 330};
    // CYC: 2^0	   13'  6"
    break;

  case 31:
    name = F("sanfti landig");
    //# FAVORITE ???
    //# shortIsOk
    //# fullCycleIsOk
    //# melodies	whirles
    //# harmonies
    //# structure
    //# aesthetic
    //# nice
    //# strange
    //# mystic
    //# cosmic
    //# SoundSpaceShip
    //# birds
    //# fairies
    //# aurora
    //# lesley
    //# lyric
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
    name = F("schön, ruhig, eifach");
    //# FAVORITE ???
    //# shortIsOk
    //# melodies
    //# harmonies
    //# structure
    //# aesthetic
    //# nice
    //# strange
    //# simple
    //# mystic
    //# cosmic
    //# SoundSpaceShip
    //# fractal
    //# birds
    //# fairies
    //# aurora
    //# lesley
    //# hasSlowPulse
    //# lyric
    //comment: *lange* harmonical cycle 11h 46' 9"	???
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
    name = F("magical toilet gejodel");
    //# FAVORITE ???	has some light stress and maybe dropout issues, but sounds crazy ok
    //# shortIsOk
    //# melodies
    //# harmonies
    //# structure
    //# dropouts
    //# systemStress
    //# aesthetic
    //# crazy
    //# strange
    //# funny
    //# dense	but good coordinated
    //# mystic
    //# cosmic
    //# SoundSpaceShip
    //# fractal
    //# birds
    //# fairies
    //# aurora
    //# lesley
    //# hasSlowPulse
    //# lyric
    date = F("2018-11-15_15h38m13s	THU");
    select_array_in(SCALES, doric_scale);
    select_array_in(JIFFLES, penta_3rd_down_5);
    sync = 5;
    // fixed tuning c	262
    pitch = {32768, 262};
    // CYC: 2^0	1h 22' 33"
    break;

  case 34:
    name = F("hübsch, langipausenamaafang");
    //# FAVORITE ???
    //# shortIsOk
    //# melodies
    //# harmonies
    //# structure
    //# aesthetic
    //# nice
    //# strange	not too much
    //# simple	moderate
    //# mystic
    //# cosmic
    //# SoundSpaceShip
    //# fractal
    //# birds
    //# fairies
    //# aurora
    //# lesley
    //# hasSlowPulse
    //# lyric
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
    name = F("uusprägts melodiemotiv");
    //# FAVORITE ???
    //# shortIsOk
    //# motives
    //# melodies
    //# harmonies
    //# structure
    //# aesthetic
    //# nice
    //# simple	moderate
    //# mystic
    //# fractal
    //# birds
    //# fairies
    //# aurora
    //# lesley
    //# hasSlowPulse
    //# lyric
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
    name = F("maschinells ticke, klassisch");
    //# FAVORITE ???
    //# shortIsOk
    //# fullCycleIsOk		possibly
    //# motives
    //# melodies
    //# harmonies
    //# structure
    //# classicHarmonical
    //# aesthetic
    //# nice
    //# melancholic
    //# simple
    //# cosmic
    //# bigBang
    //# heavyStart	should go ok
    //# clock
    //# hasSlowPulse
    //# lyric
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
    name = F("sehr sehr spezielle sync 0 aafang, wunderschön");
    //# FAVORITE ???
    //# heavy start	on some instruments...
    //# shortIsOk
    //# fullCycleIsOk		possibly
    //# motives
    //# melodies
    //# harmonies
    //# structure	!!! very nice big bang
    //# systemStress		some
    //# aesthetic
    //# nice
    //# strange
    //# simple	*sounds* simple somehow, but is not
    //# mystic
    //# cosmic
    //# fractal
    //# bigBang
    //# aurora
    //# lesley
    //# hasSlowPulse
    //# lyric
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
    name = F("allereifachsts melodischs iiticke");
    //# FAVORITE ???
    //# shortIsOk
    //# rhytm&pulse
    //# structure
    //# nice
    //# simple
    //# simlicistic
    //# fractal
    //# elementary
    //# clock
    //# hasSlowPulse
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
    name = F("näbel");	// DADA: FIXME: did sound different...	################
    //# systemStress
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
    name = F("jazzig");
    //# FAVORITE ???
    //# shortIsOk
    //# melodies	runs
    //# harmonies	a bit crazy
    //# rhytm&pulse
    //# structure
    //# aesthetic	jazzy
    //# nervous
    //# crazy
    //# funny
    //# chaotic
    //# jazz
    //# lyric
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
    name = F("chileglogge?");
    //# FAVORITE ???
    //# shortIsOk
    //# fullCycleIsOk		maybe?
    //# motives
    //# melodies
    //# harmonies
    //# structure
    //# aesthetic
    //# nice
    //# happy
    //# strange
    //# mystic
    //# cosmic
    //# fractal
    //# birds
    //# fairies
    //# aurora
    //# lesley
    //# hasSlowPulse
    //# lyric
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
    name = F("lesley");
    //# FAVORITE ???
    //# shortIsOk
    //# motives
    //# melodies
    //# harmonies
    //# dropouts	maybe
    //# systemStress
    //# aesthetic
    //# strange
    //# crazy
    //# dense
    //# chaotic	sometimes a bit
    //# mystic
    //# cosmic
    //# SoundSpaceShip
    //# fractal
    //# birds
    //# fairies
    //# aurora
    //# lesley
    //# hasSlowPulse
    //# lyric
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
    name = F("vorbereitige zum abflug?");
    //# FAVORITE ???
    //# shortIsOk
    //# motives
    //# melodies
    //# harmonies
    //# aesthetic
    //# strange
    //# mystic
    //# cosmic
    //# SoundSpaceShip
    //# fractal
    //# fairies
    //# aurora
    //# lesley
    //# hasSlowPulse
    //# lyric
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
    name = F("melodischs tänzle");
    //# FAVORITE ???
    //# shortIsOk
    //# fullCycleIsOk		probably yes
    //# motives
    //# melodies
    //# harmonies
    //# aesthetic
    //# nice
    //# strange
    //# mystic
    //# cosmic
    //# SoundSpaceShip
    //# fractal
    //# fairies
    //# aurora
    //# lesley
    //# hasSlowPulse
    //# lyric
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
    name = F("motivtanz");
    //# favorite+
    //# motives
    //# melodies
    //# harmonies
    //# structure
    //# aesthetic
    //# nice
    //# strange
    //# cosmic
    //# SoundSpaceShip
    //# fractal
    //# birds
    //# lesley
    //# hasSlowPulse
    //# lyric
    date = F("2019-04-11_18h21m28s	THU");
    select_array_in(SCALES, octave_4th_5th);
    select_array_in(JIFFLES, up_THRD);
    sync = 4;
    stack_sync_slices = 0;
    // base_pulse = 15;	// TODO: rethink that
    pitch = {1, 175};
    chromatic_pitch = 9;	// f
    // subcycle_octave = 6;	// TODO: rethink that
    // cycle 9h 59' 10"  subcycle | 9' 22" |
    break;

  case 46:
    name = F("madrigal machine");
    //# favorite+
    //# shortIsOk
    //# fullCycleIsOk		maybe
    //# motives
    //# melodies
    //# harmonies
    //# classicHarmonical
    //# aesthetic
    //# nice
    //# simple
    //# fractal
    //# noise
    //# hasSlowPulse
    //# lyric
    date = F("2019-04-11_14h");
    select_array_in(SCALES, tetraCHORD);
    select_array_in(JIFFLES, mechanical);
    sync = 2;
    stack_sync_slices = 256;
    // base_pulse = 15;	// TODO: rethink that
    pitch = {1, 233};
    chromatic_pitch = 2;	// a#
    // subcycle_octave = 4;	// TODO: rethink that
    break;

  case 47:
    name = F("es sphärischs netz vo eigenartige melodie wiegt über öis");
    //# favorite
    //# shortIsOk
    //# fullCycleIsOk		possible
    //# motives
    //# melodies
    //# harmonies
    //# structure	nice start sequence
    //# aesthetic
    //# strange
    //# dense		quite, but structured
    //# mystic
    //# cosmic
    //# SoundSpaceShip
    //# soundscape
    //# aurora
    //# lesley
    //# lyric
    date = F("2018-11-21_15h49m05s	WED");
    select_array_in(SCALES, minor_scale);
    select_array_in(JIFFLES, pent_patternA);
    sync = 3;
    pitch = {32768, 161};
    // used SUBCYCLE:    4' 12"
    break;

  case 48:
    name = F("eifach und schön");	// harmonical cycle    55' 27"
    //# FAVORITE
    //# shortIsOk
    //# fullCycleIsOk		probably very nice!
    //# motives
    //# melodies
    //# harmonies
    //# structure
    //# aesthetic
    //# nice
    //# simple		but not too simple
    //# cosmic
    //# SoundSpaceShip
    //# fractal
    //# birds
    //# aurora
    //# lesley	a bit
    //# hasSlowPulse
    //# lyric
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
    name = F("herzig ;)");
    //# favorite+
    //# shortIsOk
    //# fullCycleIsOk		42' 6"
    //# motives
    //# melodies
    //# structure
    //# systemStress	not sure
    //# aesthetic
    //# nice
    //# happy
    //# melancholic
    //# nervous	fast delay effekt, but structure remains quiet
    //# birds
    //# aurora
    //# lesley
    //# delay
    //# hasSlowPulse
    //# lyric
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
    name = F("slow simple melodic ostinato dance");	// with fast ornaments: piepiep: Intensivstation
    //# favorite+
    //# shortIsOk
    //# fullCycleIsOk		52' 37"
    //# motives
    //# melodies
    //# structure
    //# aesthetic
    //# nice		but also strange
    //# strange
    //# funny
    //# simple	not really
    //# soundscape
    //# birds
    //# aurora
    //# lesley
    //# hasSlowPulse
    //# lyric
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
    name = F("langsam sprudelnd");
    //# favorite+
    //# shortIsOk
    //# fullCycleIsOk
    //# motives
    //# melodies
    //# harmonies
    //# structure
    //# aesthetic
    //# nice
    //# mystic
    //# fractal
    //# soundscapey
    //# lyric
    //# water
    date = F("2019-04-11_10h");
    select_array_in(SCALES, tetraCHORD);
    select_array_in(JIFFLES, up_THRD_dn);
    sync = 2;
    stack_sync_slices = -16;
    // base_pulse = 15;	// TODO: rethink that
    pitch = {1, 196};
    chromatic_pitch = 11;	// g
    // subcycle_octave = 5;	// TODO: rethink that
    break;

  case 52:
    name = F("fractal time web");
    //# favorite
    //# shortIsOk
    //# fullCycleIsOk	harmonical CYCLE: 28' 21"
    //# motives
    //# harmonies
    //# structure
    //# classicHarmonical
    //# aesthetic
    //# nice
    //# fractal
    //# birds
    //# lesley
    //# hasSlowPulse
    //# lyric
    date = F("2018-12-05 15h40m");
    sync = 1;
    select_array_in(SCALES, doric_scale);
    select_array_in(JIFFLES, up_THRD_dn);
    pitch = {1, 32};
    break;

  case 53:
    name = F("church bells are still ringing");
    //# favorite+
    //# shortIsOk
    //# motives
    //# harmonies
    //# dense
    //# chaotic
    //# soundscape
    //# churchbells
    //# birds
    //# delay
    //# hasSlowPulse
    date = F("2018-11-21_21h12m38s	WED");
    select_array_in(SCALES, pentatonic_minor);
    select_array_in(JIFFLES, pent_patternA);
    sync = 2;
    pitch = {32768, 221};
    // CYC: 2^0	1h 37' 51"
    subcycle_octave = -5;	// 3'  3"	| subcycle |
    break;

  case 54:
    name = F("kid's room");
    //# favorite
    //# shortIsOk
    //# fullCycleIsOk	harmonical CYCLE: 28' 21"
    //# motives
    //# melodies
    //# harmonies
    //# aesthetic
    //# nice
    //# simple
    //# fractal
    //# hasSlowPulse
    date = F("2018-11-22_08h48m23s	THU");
    select_array_in(SCALES, octaves_fifths);
    select_array_in(JIFFLES, pent_top_wave_0);
    sync = 5;
    pitch = {32768, 415};
    // fix:	play_KB_macro  *2
    // used SUBCYCLE:    3' 28"
    break;

  case 55:
    name = F("uufregig im hüehnerstall weg de chileglogge");
    //# favorite
    //# shortIsOk
    //# motives
    //# melodies
    //# harmonies
    //# structure
    //# dropouts	?
    //# systemStress	?
    //# crazy
    //# funny
    //# dense
    //# chaotic
    //# soundscape
    //# birds
    //# churchbells
    //# alarm
    //# hasSlowPulse
    //# lyric
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
    name = F("klassischi harmonikali aesthetik");
    //# favorite
    //# shortIsOk
    //# fullCycleIsOk	harmonical CYCLE: 13' 9"
    //# motives
    //# melodies
    //# harmonies
    //# systemStress	some
    //# classicHarmonical
    //# aesthetic
    //# nice
    //# simple
    //# fractal
    //# hasSlowPulse
    //# lyric
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
    name = F("klassisch, schön, als grundlag zu eme 22' 1\" stuck");
    // FIXME: is double speed on chamber orchestra
    //# favorite+
    //# shortIsOk
    //# fullCycleIsOk	harmonical CYCLE: 11' 0" or 22' 01"
    //# melodies
    //# harmonies
    //# classicHarmonical
    //# aesthetic
    //# nice
    //# simple
    //# fractal
    //# hasSlowPulse
    //# lyric
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
    name = F("eifach und eigenartig schön");
    //# favorite
    //# shortIsOk
    //# motives
    //# melodies
    //# classicHarmonical
    //# aesthetic
    //# nice
    //# simple
    //# fractal
    //# hasSlowPulse
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
    name = F("gradnomal wunderschön :)");
    //# favorite+
    //# shortIsOk
    //# motives
    //# melodies
    //# harmonies
    //# classicHarmonical
    //# aesthetic
    //# nice
    //# simple	quite
    //# bigBang
    //# birds
    //# churchbells
    //# hasSlowPulse
    //# lyric
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
    name = F("landing, seed, growth, ....");
    //comment: strong beginning ;)
    //# favorite+
    //# shortIsOk
    //# motives
    //# melodies
    //# harmonies
    //# aesthetic
    //# nice
    //# simple	not really, but easy to understand
    //# mystic
    //# cosmic
    //# SoundSpaceShip
    //# fractal
    //# fairies
    //# aurora
    //# lesley
    //# hasSlowPulse
    //# lyric
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
    name = F("Baldrian");
    //comment: entspannig pur
    //comment: try longer cycles ;)
    //# favorite
    //# shortIsOk
    //# motives
    //# melodies
    //# harmonies
    //# mystic	a bit
    //# fractal
    //# soundscape
    //# churchbells
    //# aurora
    //# hasSlowPulse
    //# lyric
    date = F("2018-11-25_19h14m10s	SUN");
    select_array_in(SCALES, octave_4th_5th);
    select_array_in(JIFFLES, PENTA_3rd_down_5);
    sync = 2;
    pitch = {32768, 296};
    // random octave shift: // fix:	play_KB_macro  *2
    // CYC: 2^0	15h 35' 11"
    subcycle_octave = -8;	// 3' 39"	| subcycle |
    break;

  case 62:
    name = F("dorisch");
    //# favorite
    //# shortIsOk
    //# harmonies
    //# classicHarmonical
    //# aesthetic
    //# simple
    //# cosmic
    //# fractal
    //# hasSlowPulse
    //# lyric
    date = F("2018-11-25_19h24m24s	SUN");
    select_array_in(SCALES, doric_scale);
    select_array_in(JIFFLES, din_dididi);
    sync = 0;
    pitch = {32768, 219};
    // random octave shift: used SUBCYCLE:	3' 5"y61
    // CYC: 2^0	1h 38' 45"
    subcycle_octave = -5;	// 3'  5"	| subcycle |
    break;

  case 63:
    name = F("no meh melodie :)");
    //# favorite+
    //# shortIsOk
    //# motives
    //# melodies
    //# aesthetic
    //# nice
    //# simple
    //# fractal
    //# soundscape
    //# bigBang
    //# churchbells
    //# fairies
    //# hasSlowPulse
    //# lyric
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
    name = F("lost name");
    //# favorite+
    //# shortIsOk
    //# motives
    //# melodies
    //# harmonies
    //# classicHarmonical
    //# aesthetic
    //# nice
    //# strange
    //# simple	quite
    //# mystic
    //# fractal
    //# soundscape
    //# birds	some
    //# hasSlowPulse
    //# lyric
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
    name = F("eigenartigi harmonischi melodiegebilde");
    //# favorite+
    //# shortIsOk
    //# fullCycleIsOk	harmonical CYCLE: 1h 4' 0"
    //# motives
    //# melodies
    //# harmonies
    //# classicHarmonical
    //# aesthetic
    //# nice
    //# strange
    //# mystic
    //# cosmic
    //# fractal
    //# soundscape
    //# birds
    //# fairies
    //# aurora
    //# hasSlowPulse
    //# lyric
    select_array_in(SCALES, pentatonic_minor);
    select_array_in(JIFFLES, pentatonic_rise);
    pitch = {1, 128};
    sync = 4;
    break;

  case 66:
    name = F("no ohni name");
    //# favorite+
    //# shortIsOk
    //# motives		!
    //# melodies
    //# harmonies
    //# aesthetic
    //# nice
    //# mystic
    //# fractal
    //# soundscape
    //# birds
    //# hasSlowPulse
    //# lyric
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
    name = F("sehr eifach & schön");		// ähnlich love like a morning bird
    //# favorite+
    //# shortIsOk
    //# melodies
    //# harmonies
    //# classicHarmonical
    //# aesthetic
    //# nice
    //# happy
    //# simple
    //# mystic
    //# cosmic
    //# fractal
    //# birds
    //# fairies
    //# hasSlowPulse
    //# lyric
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
    name = F("wow");
    //# FAVORITE
    //# shortIsOk
    //# melodies
    //# harmonies
    //# structure
    //# classicHarmonical
    //# aesthetic
    //# nice
    //# simple
    //# cosmic
    //# fractal
    //# hasSlowPulse
    //# lyric
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
    name = F("schön melodisch, mit recht guetem schluss");
    //comment: de schluss chönnt no chli schneller fertig sii
    //# favorite+
    //# shortIsOk
    //# fullCycleIsOk	harmonical CYCLE: 11' 25"
    //# systemStress
    //# motives
    //# melodies
    //# harmonies
    //# structure
    //# aesthetic
    //# nice
    //# strange
    //# mystic
    //# cosmic
    //# fractal
    //# birds
    //# churchbells
    //# fairies
    //# aurora
    //# lesley
    //# hasSlowPulse
    //# lyric
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
    name = F("PENTAtheme");
    //# favorite
    //# shortIsOk
    //# motives
    //# melodies
    //# harmonies
    //# heavyStart
    //# aesthetic
    //# strange
    //# mystic
    //# cosmic
    //# fractal
    //# soundscape
    //# churchbells
    //# fairies
    //# hasSlowPulse
    //# lyric
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
    name = F("elfechinde");
    //# favorite
    //# shortIsOk
    //# motives
    //# melodies
    //# harmonies
    //# dense	not too much
    //# chaotic	some
    //# mystic
    //# fractal
    //# soundscape
    //# churchbells
    //# fairies
    //# aurora
    //# lesley
    //# hasSlowPulse
    //# lyric
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
    name = F("elfechinde 2");
    //# favorite
    //# shortIsOk
    //# motives
    //# melodies
    //# harmonies
    //# aesthetic
    //# nice
    //# strange
    //# mystic
    //# birds
    //# churchbells
    //# fairies
    //# hasSlowPulse
    //# lyric
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
    name = F("simplistic");
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
    name = F("rhythms");
    //# favorite+
    //# shortIsOk
    //# motives
    //# harmonies
    //# rhytm&pulse
    //# structure
    //# heavyStart
    //# simple
    //# simlicistic
    //# soundscape
    //# elementary
    //# bigBang
    //# clock
    //# peep
    //# robot
    //# rumble
    //# hasSlowPulse
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
    name = F("ruhig schwebend");
    //# favorite+
    //# shortIsOk
    //# fullCycleIsOk	harmonical CYCLE: 1h 33' 5"
    //# motives
    //# harmonies
    //# aesthetic
    //# nice
    //# quiet
    //# strange
    //# dense	but harmonic
    //# mystic
    //# cosmic
    //# fractal
    //# soundscape
    //# churchbells
    //# fairies
    //# aurora
    //# lesley
    //# hasSlowPulse
    //# lyric
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
    name = F("magical tree");
    //comment: maybe as music for the tree woman from Theaterspektakel, Zürich 2018
    //# favorite
    //# shortIsOk
    //# fullCycleIsOk	harmonical CYCLE: 48' 28"
    //# motives
    //# melodies
    //# harmonies
    //# aesthetic
    //# quiet
    //# strange	little
    //# mystic
    //# fractal
    //# soundscape
    //# birds
    //# churchbells
    //# fairies
    //# aurora
    //# lesley
    //# hasSlowPulse
    //# lyric
    date = F("2018-12-07_13h03m35s	FRI");
    select_array_in(SCALES, europ_PENTAtonic);
    select_array_in(JIFFLES, dwn_THRD);
    sync = 1;
    pitch = {32768, 357};
    // CYC: 2^0	   48' 28"
    subcycle_octave = -3;	// 6'  3"	| subcycle |
    break;

  case 77:
    name = F("tanboura");
    //# favorite
    //# shortIsOk	even very short ;)
    //# melodies
    //# harmonies	dense
    //# bourdon
    //# classicHarmonical
    //# aesthetic
    //# nice		can get boring
    //# quiet
    //# simple
    //# simlicistic
    //# dense		harmonies
    //# elementary
    //# robot
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
    name = F("melodischs Netz uf I IV V");
    //# favorite+
    //# shortIsOk
    //# motives
    //# melodies	very nice
    //# harmonies
    //# bourdon		some included
    //# structure
    //# aesthetic
    //# nice
    //# happy
    //# melancholic
    //# quiet
    //# simple
    //# fractal
    //# birds
    //# fairies
    //# lyric
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
    name = F("Aurora");
    //comment: very dense pulsating web
    //comment: good ending ;)
    //# FAVORITE
    //# favorite+
    //# shortIsOk
    //# motives
    //# melodies	dense net of melodies
    //# harmonies
    //# bourdon
    //# structure	dense net of melodies
    //# aesthetic
    //# nice
    //# quiet
    //# strange
    //# simple		somehow yes ;)
    //# dense
    //# mystic
    //# cosmic
    //# SoundSpaceShip
    //# fractal
    //# soundscape
    //# fairies
    //# aurora		AURORA
    //# lesley
    //# hasSlowPulse
    //# lyric
    date = F("2018-12-07_15h57m55s	FRI");
    select_array_in(SCALES, minor_scale);
    select_array_in(JIFFLES, PENTA_3rd_down_5);
    sync = 3;
    pitch = {32768, 300};
    // CYC: 2^0	1h 12'  5"
    subcycle_octave = -3;	// 9'  1"	| subcycle |
    break;

  case 80:
    name = F("Aurora mit Tänzelde Liechter");
    //# FAVORITE
    //# fullCycleIsOk	harmonical CYCLE: 55' 27"
    //# motives
    //# melodies
    //# harmonies
    //# bourdon
    //# structure
    //# aesthetic
    //# happy
    //# melancholic
    //# strange
    //# dense
    //# mystic
    //# cosmic
    //# fractal
    //# soundscape
    //# birds
    //# churchbells
    //# fairies
    //# aurora
    //# lesley
    //# hasSlowPulse	soft and not always
    //# lyric
    //# water		why not?
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
    name = F("Melodie Kabinet");
    //comment: wunderschön tänzelnds Melodienetz
    //comment: wo sich langsam au i di undere Stimme uusdehnt
    //# favorite
    //# shortIsOk
    //# fullCycleIsOk	harmonical CYCLE: 48' 44"
    //# motives
    //# melodies
    //# harmonies
    //# bourdon
    //# structure	dancing web of melodies
    //# aesthetic
    //# nice		but complicated
    //# quiet		but busy
    //# strange
    //# dense
    //# chaotic		sometimes
    //# mystic
    //# cosmic
    //# fractal
    //# soundscape
    //# hasSlowPulse	subtle
    //# lyric
    //# water		possible, bubbles?
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
    name = F("Akkordeon");
    //# favorite
    //# shortIsOk
    //# motives
    //# melodies
    //# harmonies
    //# happy
    //# melancholic
    //# quiet		but busy spinning around
    //# strange
    //# nervous
    //# dense
    //# chaotic
    //# mystic
    //# fractal
    //# soundscape
    //# hasSlowPulse	subtle
    //# lyric
    //# water
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
    name = F("MusikdoseTraum");
    //comment: für langi, vertröimti szene
    //# favorite+
    //# shortIsOk
    //# melodies	simplicistic
    //# harmonies
    //# bourdon		a bit
    //# structure	simplicistic
    //# aesthetic	but simplicistic
    //# nice		maybe too simple
    //# simple
    //# simlicistic
    //# clock
    //# delay
    //# hasSlowPulse
    //# lyric		maybe too simple
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
    name = F("Klangschwade mit Puls");
    // heavy big bang
    //# favorite-
    //# shortIsOk
    //# motives
    //# harmonies	static, more like bourdon
    //# bourdon		permanent
    //# heavyStart
    //# quiet
    //# static
    //# dramatic
    //# nervous
    //# simple
    //# simlicistic
    //# dense		harmonies more like bourdon
    //# elementary
    //# bigBang
    //# clock
    //# robot
    //# humming
    //# hasSlowPulse	soft and not steady
    //# lyric		but very simple, static
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
    name = F("Pentaweb");
    //comment: stimmigsvoll
    //comment: mit sehnsucht ufs ziel zue
    //# favorite
    //# shortIsOk
    //# fullCycleIsOk	harmonical CYCLE: 1h 4' 0"
    //# motives
    //# melodies
    //# harmonies
    //# bourdon		some
    //# structure	pentaweb
    //# aesthetic
    //# nice
    //# happy
    //# melancholic
    //# quiet
    //# strange		yes
    //# dense		melody patterns, but still nice
    //# mystic
    //# cosmic
    //# SoundSpaceShip
    //# fractal
    //# birds
    //# churchbells
    //# fairies
    //# aurora
    //# lesley
    //# humming		bourdon
    //# hasSlowPulse	slow and subtle
    //# lyric		poetic
    sync = 2;
    select_array_in(SCALES, pentatonic_minor);
    select_array_in(JIFFLES, PENTAtonic_desc);
    pitch = {1, 128};
    // harmonical cycle 1h  9' 59"	subcycle    8' 45"
    break;

  case 86:
    name = F("Schwester Pentaweb");
    //comment: e ZwillingsPentawebSchwöster :)
    //# favorite+
    //# shortIsOk
    //# fullCycleIsOk	harmonical CYCLE: 1h 21' 48"
    //# motives
    //# melodies
    //# harmonies
    //# structure	PENTAweb sister
    //# aesthetic
    //# nice
    //# happy
    //# strange		only little
    //# fractal
    //# soundscape
    //# birds
    //# churchbells
    //# fairies
    //# aurora
    //# lesley		a bit
    //# hasSlowPulse	some
    //# lyric		maybe?
    //# water		why not?
    date = F("2018-12-07_19h04m44s	FRI");
    select_array_in(SCALES, europ_PENTAtonic);
    select_array_in(JIFFLES, penta_3rd_rise);
    sync = 1;
    pitch = {32768, 423};
    // CYC: 2^0	1h 21' 48"
    subcycle_octave = -3;	// 10' 13"	| subcycle |
    break;

  case 87:
    name = F("PentaMusicBox");
    //# favorite
    //# shortIsOk
    //# fullCycleIsOk	harmonical CYCLE: 57' 49"
    //# motives
    //# harmonies
    //# bourdon
    //# classicHarmonical
    //# aesthetic
    //# nice
    //# quiet
    //# static
    //# simple
    //# simlicistic
    //# clock
    //# robot
    //# humming
    //# hasSlowPulse	by a melody motive
    //# lyric
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
    name = F("I_IV_V mit Knatterrhytheme");
    //# favorite
    //# shortIsOk	rhythm will probably not start yet
    //# motives		rhythmical melodies
    //# melodies	simplicistic
    //# rhytm&pulse	takes it's time
    //# bourdon
    //# structure	simple, static
    //# quiet
    //# static
    //# simple
    //# simlicistic
    //# elementary
    //# robot
    //# hasSlowPulse	by motives
    date = F("2018-12-07_19h48m57s	FRI");
    select_array_in(SCALES, octave_4th_5th);
    select_array_in(JIFFLES, d4096_512);
    sync = 1;
    pitch = {32768, 187};
    // CYC: 2^0	6h 10'  4"
    //	subcycle_octave = -5;	// 11' 34"	| subcycle |
    break;

  case 89:
    name = F("The Landing Of The Sound Space Ship");
    //# favorite+
    //# shortIsOk
    //# fullCycleIsOk	harmonical CYCLE: 57' 13"
    //# motives
    //# melodies
    //# harmonies	nice and strangeP
    //# aesthetic
    //# nice		yet complex
    //# strange
    //# dense		but still nice
    //# mystic
    //# cosmic
    //# SoundSpaceShip
    //# fractal
    //# fairies
    //# aurora
    //# lesley
    //# hasSlowPulse	has slow structures
    //# lyric
    //# water		everything is possible
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
    name = F("Striicherbordun");
    //comment: dichts melodienetz mit tüüfem stehendem bass, wi tüüfi striicher
    //# favorite
    //# shortIsOk
    //# fullCycleIsOk	harmonical CYCLE: 16' 28" !!!
    //# motives
    //# melodies
    //# bourdon		deep string like bass
    //# structure	dense melody net
    //# aesthetic	advanced
    //# happy
    //# melancholic
    //# quiet		but dense
    //# static		little
    //# dense
    //# mystic
    //# fractal
    //# soundscape
    //# birds
    //# churchbells
    //# fairies
    //# aurora
    //# humming		deep string like bass
    //# hasSlowPulse	simple
    //# lyric
    //# water		slow waves
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
    name = F("simple and beautiful");
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
    name = F("de Reige vo de Dezämberfee");
    //# favorite+
    //# shortIsOk
    //# fullCycleIsOk	harmonical CYCLE: 1h 3' 53"
    //# motives
    //# melodies
    //# bourdon		often
    //# structure	light net of dancing december fairies dancing in a circle
    //# aesthetic
    //# nice		ritch
    //# quiet
    //# static		somehow, dancing in a circle
    //# strange		mystic
    //# dense		not too much
    //# mystic
    //# cosmic
    //# SoundSpaceShip
    //# fractal
    //# birds
    //# churchbells
    //# fairies
    //# aurora
    //# lesley
    //# hasSlowPulse	you can always hear something, subtle
    //# lyric
    //# water
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
    name = F("Saiteresonanze");
    //comment: git grad zuefellig wonderschöni Saiteresonanze uf de Gitarre &co
    //# favorite+
    //# shortIsOk
    //# fullCycleIsOk	harmonical CYCLE: 1h 19' 44"
    //# motives
    //# melodies	busy rising melodies like air bubbles in water
    //# harmonies
    //# aesthetic
    //# nice
    //# happy		busy
    //# quiet		but busy
    //# fractal
    //# birds
    //# churchbells
    //# fairies
    //# aurora
    //# lesley
    //# hasSlowPulse	not really, but quite
    //# lyric
    //# water	busy rising melodies like air bubbles in water
    date = F("2018-12-11_13h29m08s	TUE");
    select_array_in(SCALES, europ_PENTAtonic);
    select_array_in(JIFFLES, pentatonic_rise);
    sync = 5;
    pitch = {32768, 217};
    // CYC: 2^0	1h 19' 44"
    subcycle_octave = -4;	// 4' 59"	| subcycle |
    break;

  case 94:
    name = F("Chindertanz -- Children's Dance");
    //comment: fröhlich gspunnene chindertanz
    //# favorite+
    //# shortIsOk
    //# motives
    //# melodies
    //# harmonies	a bit crazy
    //# rhytm&pulse	simple
    //# bourdon		yes
    //# structure	simplicistic
    //# happy
    //# static		turning in cyrcles
    //# strange
    //# funny
    //# mystic
    //# clock		not so much
    //# peep
    //# robot
    //# lesley
    //# delay
    //# rumble
    //# hasSlowPulse	simle motiv ostinatos
    //# lyric
    //# children
    //# dance
    date = F("2018-12-13_10h24m06s	THU");
    select_array_in(SCALES, tetraCHORD);
    select_array_in(JIFFLES, rising_pent_them);
    sync = 4;
    pitch = {32768, 302};
    // CYC: 2^0	57' 17"
    subcycle_octave = -4;	// 3' 35"	| subcycle |
    break;

  case 95:
    name = F("Good Old Major Sync3");
    //comment: use whole cycle :)
    //# FAVORITE
    //# shortIsOk	maybe use whole cycle instead?
    //# fullCycleIsOk	harmonical CYCLE: 12' 48"
    //# melodies
    //# harmonies
    //# structure	classical harmonical cycle sync=3
    //# classicHarmonical
    //# aesthetic
    //# nice	and profound
    //# happy	and profound
    //# quiet
    //# cosmic
    //# clock	if you want you can hear it like this...
    //# lyric
    //# dance
    //# festive
    date = F("2018-12-13_10h47m46s	THU");
    select_array_in(SCALES, major_scale);
    select_array_in(JIFFLES, d4096_1024);
    sync = 3;
    // harmonical CYCLE: 13' 11"	SUBCYCLE: | 6' 36" |
    subcycle_octave = -1;
    break;

  case 96:
    name = F("Cosmic SoundWave Visiting Earth");
    //# favorite+
    //# shortIsOk
    //# fullCycleIsOk	harmonical CYCLE: 1h 3' 1"
    //# harmonies	slow and dense
    //# bourdon		quite steady
    //# structure	slow waves and whirls
    //# aesthetic
    //# melancholic	maybe a bit
    //# quiet
    //# static
    //# dense
    //# mystic
    //# cosmic
    //# SoundSpaceShip
    //# fractal
    //# soundscape
    //# fairies
    //# aurora
    //# lesley
    date = F("2018-12-13_16h59m40s	THU");
    select_array_in(SCALES, major_scale);
    select_array_in(JIFFLES, PENTA_3rd_down_5);
    sync = 1;
    pitch = {32768, 208};
    // harmonical CYCLE: 20' 48"	SUBCYCLE: | 5' 12" |
    // SUBCYCLE: (2^-2) 5' 12"
    break;

  case 97:
    name = F("Jazzigi Rhythme mit schnarrend schnarchende KnatterKläng");	// really???
    // meh wi e musikdose	steady endless looping
    //# favorite
    //# shortIsOk
    //# motives		simplicistic
    //# melodies	simplicistic
    //# harmonies	simplicistic
    //# rhytm&pulse	simplicistic
    //# bourdon		yes
    //# structure	simplicistic
    //# aesthetic
    //# nice
    //# quiet		steady endless looping
    //# static		steady endless looping
    //# simple
    //# simlicistic
    //# fractal
    //# clock
    //# hasSlowPulse	simplicistic
    //# lyric
    //# children
    date = F("2018-12-13_19h42m01s	THU");
    select_array_in(SCALES, octaves_fifths);
    select_array_in(JIFFLES, diing_ditditdit);
    sync = 2;
    pitch = {32768, 449};
    // harmonical CYCLE: 4d 13h 36' 6"		SUBCYCLE: | 6' 25" |
    subcycle_octave = -10;
    break;

  case 98:
    name = F("Harmonikale Schrittanz");
    //# FAVORITE
    //# shortIsOk
    //# fullCycleIsOk	armonical CYCLE: 59' 49"
    //# melodies
    //# harmonies	strange and blue ;)
    //# structure	blue chord progression
    //# heavyStart
    //# aesthetic	blue, strange and rich
    //# nice
    //# strange
    //# dense		a bit
    //# mystic
    //# fractal
    //# soundscape
    //# bigBang
    //# fairies
    //# aurora
    //# jazz
    //# lesley
    //# hasSlowPulse	chord progression
    //# lyric
    //# water		possible
    //# dance		slowly progressing
    //# festive		blue night festival
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
    name = F("Church Bells, blue jay way");
    //comment: es paar Alphörner het s glaub au no ;)
    //# favorite
    //# shortIsOk
    //# motives
    //# melodies
    //# harmonies	floating
    //# rhytm&pulse	pulsating, vibrating
    //# bourdon
    //# structure	church bells
    //# aesthetic	advanced
    //# static		static waving
    //# dense		quite
    //# mystic
    //# cosmic		cosmic waiting
    //# fractal
    //# soundscape
    //# churchbells
    //# fairies
    //# aurora
    //# delay
    //# lyric
    //# water
    //# floating
    date = F("2018-12-14_18h24m10s	FRI");
    select_array_in(SCALES, pentatonic_minor);
    select_array_in(JIFFLES, pent_patternA);
    sync = 5;
    pitch = {32768, 431};
    // harmonical CYCLE: 1h 40' 21"	SUBCYCLE: | 6' 16" |
    subcycle_octave = -4;
    break;

  case 100:
    name = F("PENTAtonic Clock Organ");
    //# favorite
    //# shortIsOk
    //# motives
    //# melodies
    //# harmonies
    //# rhytm&pulse
    //# bourdon
    //# aesthetic
    //# structure
    //# quiet
    //# static
    //# nervous		can be perceived as
    //# simple
    //# simlicistic
    //# fractal
    //# elementary
    //# fairies
    //# aurora
    //# clock
    //# peep
    //# robot
    //# hasSlowPulse	archaic
    //# children
    //# dance
    date = F("2018-12-14_18h51m01s	FRI");
    select_array_in(SCALES, europ_PENTAtonic);
    select_array_in(JIFFLES, diing_ditditdit);
    sync = 3;
    pitch = {32768, 429};
    // fix:	play_KB_macro  *2
    // harmonical CYCLE: 2h 41' 19"	SUBCYCLE: | 5' 2" |
    break;

  case 101:
    name = F("Good Night Choir");
    //# favorite+
    //# shortIsOk
    //# motives
    //# melodies
    //# harmonies
    //# rhytm&pulse
    //# bourdon
    //# structure	chord progression
    //# aesthetic
    //# nice	and a bit blue
    //# quiet	spare and slow
    //# mystic
    //# cosmic
    //# fractal
    //# bigBang
    //# fairies
    //# aurora
    //# lesley
    //# rumble
    //# hasSlowPulse	chord progression and others
    //# lyric
    //# dance		very slow, quiet but strong dance
    //# festive		somehow yes, but quietly so
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
    name = F("Classical Beauty");
    //# favorite
    //# shortIsOk
    //# melodies
    //# harmonies
    //# structure	harmonical progression
    //# classicHarmonical
    //# aesthetic
    //# nice
    //# quiet
    //# simple
    //# hasSlowPulse	somehow
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
    name = F("PENTA Classic");
    //# favorite+
    //# shortIsOk
    //# fullCycleIsOk	harmonical CYCLE: 51' 57"
    //# motives
    //# melodies
    //# harmonies
    //# structure	chord progression
    //# classicHarmonical
    //# aesthetic
    //# nice
    //# strange		some strange blueness
    //# mystic
    //# fairies
    //# aurora
    //# lesley
    //# hasSlowPulse	chord progression
    //# lyric
    //# water		possible
    //# children	maybe also
    //# dance		quiet, slow and strong
    //# festive		somehow
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
    name = F("pentatonic reaching down to rhythm");
    //# favorite+
    //# shortIsOk
    //# fullCycleIsOk	harmonical CYCLE: 1h 3' 3"
    //# melodies	jazz like
    //# harmonies
    //# rhytm&pulse
    //# aesthetic	jazz aesthetics
    //# dramatic
    //# nervous
    //# crazy
    //# funny
    //# dense		not too much
    //# chaotic		there's order in the chaos ;)
    //# fractal
    //# jazz
    //# rumble
    //# hasSlowPulse	jazzy, and not steady
    //# dance		crazy dance
    date = F("2018-12-15_12h58m14s	SAT");
    select_array_in(SCALES, pentatonic_minor);
    select_array_in(JIFFLES, tumtum);
    sync = 2;
    pitch = {32768, 222};
    // harmonical CYCLE: 1h 37' 25"	SUBCYCLE: | 6' 5" |
    subcycle_octave = -4;
    break;

    // DADA2+	todo: CHECK FROM HERE <<<====	====>>>   #define SHORT_PRESET_COLLECTION	104

  case 105:
    name = F("Nameless Beauty");
    date = F("2018-12-15_15h06m14s	SAT");
    select_array_in(SCALES, minor_scale);
    select_array_in(JIFFLES, pentatonic_rise);
    sync = 0;
    pitch = {32768, 223};
    // harmonical CYCLE: 1h 36' 59"	SUBCYCLE: | 6' 4" |
    subcycle_octave = -4;
    break;

  case 106:
    name = F("melodischs gfüeg");
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
    name = F("RhythmicalTroubadourBordun");
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
    name = F("another melody net");
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
    name = F("eifache schöne reige");
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
    name = F("another nice simple one");	// hübsch
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
    name = F("descending ascending whirles");
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
    name = F("im Zaubergarte");
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
    name = F("Chläderrose");
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
    name = F("dream angel came from heaven to earth");
    date = F("2018-12-19_22h08m50s	WED");
    select_array_in(SCALES, pentatonic_minor);
    select_array_in(JIFFLES, penta_3rd_down_5);
    sync = 1;
    pitch = {32768, 226};
    // harmonical CYCLE: 1h 35' 41"	SUBCYCLE: | 5' 59" |
    subcycle_octave = -4;
    break;

  case 115:
    name = F("im Zaubergarte");
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
    name = F("relax and get ready");
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
    name = F("birds and other musicians calling");
    date = F("2018-12-20_16h54m10s	THU");
    select_array_in(SCALES, octaves_fifths);
    select_array_in(JIFFLES, pentatonic_desc);
    sync = 4;
    pitch = {32768, 271};
    // harmonical CYCLE: 3d 18h 47' 44"	SUBCYCLE: | 5' 19" |
    subcycle_octave = -10;
    break;

  case 118:
    name = F("triad tanboura");
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
    name = F("slow dreams");
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
    name = F("schlichte Tanz");
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
    name = F("schlicht ruhends muster, friedlich, erwartigsvoll, lebensfroh");
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
    name = F("und es ratteret und brummet tickend de motor");	// sehr starche puls!
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
    name = F("pentatonische wasserfall");
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
    name = F("brummbärs ratterkiste");
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
    name = F("a firework of sounds");
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
    name = F("firework #2");
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
    name = F("acoustic telegraph lines");
    date = F("2018-12-22_20h17m39s	SAT");
    select_array_in(SCALES, octaves_fourths);
    select_array_in(JIFFLES, rising_pent_them);
    sync = 5;
    pitch = {32768, 322};
    // harmonical CYCLE: 9d 13h 14' 40"	SUBCYCLE: | 6' 43" |
    subcycle_octave = -11;
    break;

  case 128:
    name = F("clear winter sky");
    date = F("2018-12-23_18h45m37s	SUN");
    select_array_in(SCALES, pentatonic_minor);
    select_array_in(JIFFLES, PENTAtonic_desc);
    sync = 1;
    pitch = {32768, 444};
    // harmonical CYCLE: 1h 37' 25"	SUBCYCLE: | 6' 5" |
    subcycle_octave = -4;
    break;

  case 129:
    name = F("the mad old jazz piano man");
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
    name = F("nameless beauty");
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
    name = F("penta trichord");
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
    name = F("Santa Claus");
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
    name = F("2018-12-25_20h28m58s (recorded)");
    date = F("2018-12-25_20h28m58s	TUE");
    select_array_in(SCALES, doric_scale);
    select_array_in(JIFFLES, din__dididi_dix);
    sync = 2;
    pitch = {32768, 32};
    // harmonical CYCLE: 2h 48' 57"	SUBCYCLE: | 5' 17" |
    subcycle_octave = -5;
    name = F("sound waves like ever descending rain");
    date = F("2019-01-04_18h44m09s	FRI");
    select_array_in(SCALES, tetraCHORD);
    select_array_in(JIFFLES, PENTAtonic_desc);
    sync = 2;
    pitch = {32768, 234};
    // harmonical CYCLE: 2h 27' 52"	SUBCYCLE: | 4' 37" |
    subcycle_octave = -5;
    break;

  case 134:
    name = F("Childrens Christmas Night");
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
    name = F("daydreams");
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
    name = F("Harmonical Musicbox alpha 0.010");
    sync = 5;
    select_array_in(SCALES, pentaCHORD);
    select_array_in(JIFFLES, tumtum);
    pitch = {1, 128};
    // harmonical CYCLE: 51' 48"	SUBCYCLE: | 6' 28" |
    break;

  case 137:
    name = F("TRIAD");
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
    name = F("rhythm tuned in");
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
    name = F("2019-02-01 ardour5 noname");
    sync = 1;
    select_array_in(JIFFLES, PENTA_3rd_rise);
    pitch = {1, 32};	//	*metric* ?
    // harmonical CYCLE: 1h 24' 29"	SUBCYCLE: | 5' 17" |
    break;

  case 140:	// TODO: broken?
    name = F("2019-02-02 ardour5 noname");	// TODO: broken?
    date = F("2019-02-02_17h49m06s	SAT");	// TODO: broken?
    select_array_in(SCALES, doric_scale);
    select_array_in(JIFFLES, pentatonic_rise);
    sync = 0;
    pitch = {32768, 32};
    // harmonical CYCLE: 2h 48' 57"	SUBCYCLE: | 3' 41" |
    // | 2' 38" |
    break;

  case 141:
    name = F("2019-02-04_doric_cycle_S1");
    //comment: ardour 5
    sync = 1;
    select_array_in(JIFFLES, d4096_1024);
    pitch = {1, 32};	//		*metric* a
    // harmonical CYCLE: 2h 48' 57"	SUBCYCLE: | 2' 38" |
    break;

  case 142:
    name = F("2019-02-04_bordun");	// with rhythm
    //comment: ardour5
    select_array_in(SCALES, octave_4th_5th);
    sync = 0;
    select_array_in(JIFFLES, tumtumtum);
    pitch = {2, 61};
    // harmonical CYCLE: 6h 0' 26"	SUBCYCLE: | 2' 38" |
    break;

  case 143:
    name = F("2019-02-06_major_doric_rise");
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
    name = F("2019-02-6_tumtum_pentatonic_minor_S2");
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
    name = F("zagg  2019-02-06_tetraCHORD_pentachord_rise_S1");
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
    name = F("2019-02-06_RG3_S0_d64_melody-rhythm");
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
    name = F("noname_a");
    select_array_in(SCALES, octave_4th_5th);
    sync = 0;
    select_array_in(JIFFLES, d4096_64);
    pitch = {2, 61};	// 44	*metric* d
    // harmonical CYCLE: 0"	SUBCYCLE: | 3' 41" |
    break;

  case 148:
    name = F("noname_b");
    select_array_in(SCALES, tetraCHORD);
    sync = 1;
    select_array_in(JIFFLES, pentachord_rise);
    pitch = {3, 10};	// 24	*metric* a
    // harmonical CYCLE: 2h 37' 17"	SUBCYCLE: | 4' 55" |
    break;

  case 149:
    name = F("noname_c");
    select_array_in(SCALES, europ_PENTAtonic);
    sync = 4;
    select_array_in(JIFFLES, tumtumtumtum);
    pitch = {1, 12};	// 8	*metric* f
    // harmonical CYCLE: 49' 26"	SUBCYCLE: | 6' 11" |
    break;

  case 150:
    // catched ERROR: no period	via lower_audio_if_too_high(limit)	// TODO: REMOVE: this line
    name = F("simplicistic_d0");
    select_array_in(SCALES, octaves_fourths);
    sync = 2;
    select_array_in(JIFFLES, din_dididi);
    pitch = {3, 52};	// 4288		*metric* g
    // harmonical CYCLE: 7d 20h 18' 29"	SUBCYCLE: | 5' 31" |
    break;

  case 151:
    name = F("2019-02-07_classical_time_machine_S4");
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
    name = F("noname_");
    //comment: try variations in SYNC :)
    select_array_in(SCALES, europ_PENTAtonic);
    sync = 5;
    select_array_in(JIFFLES, pentaCHORD_rise);
    pitch = {1, 12};	// 8  1/175	*metric* f
    // harmonical CYCLE: 49' 26"	SUBCYCLE: | 6' 11" |
    break;

  case 153:
    name = F("eifach wunderschön");
    //comment: sehr ruhig
    date = F("2019-02-26_12h46m19s	TUE");
    select_array_in(SCALES, doric_scale);
    sync = 1;
    select_array_in(JIFFLES, diing_ditditdit);
    pitch = {32768, 12};	// 8 1/330 metric e
    // harmonical CYCLE: 3h 18' 35"	SUBCYCLE: | 6' 12" |
    break;

  case 154:
    name = F("2019-04-01 doric stack");      // TODO: starts too slow
    select_array_in(SCALES, doric_scale);
    sync = 1;
    stack_sync_slices = 8;
    base_pulse = 15;
    select_array_in(JIFFLES, din__dididi_dixi);
    pitch = {32768, 12};	// 8 1/175 metric f
    // harmonical CYCLE: 3h 7' 14"	2^4 SUBCYCLE: | 11' 42" |
    break;

  case 155:
    name = F("Marianne 1	eingesperrt<->Freiheit	sync? 4|1024");
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
    name = F("Marianne 2	warten");
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
    name = F("Marianne 3	s erst Mal underwägs");
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
    name = F("Marianne 4	Chatzemusik!");
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
    name = F("Marianne 5	Gefängnisgeburt");
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
    // start may be problematic
    name = F("rising ostinatos over rhythmic bordun");  // nice and harmonic, humming, good ending
    //comment: very nice ending :)
    sync = 0;
    select_array_in(SCALES, octave_4th_5th);
    select_array_in(JIFFLES, pentatonic_rise);
    pitch = {2, 6144};
    // harmonical cycle 'c'	12h 24'  7"	subcycle    5' 49"
    break;

  case 161:
    name = F("au sehr schön:");
    sync = 2;
    select_array_in(SCALES, tetraCHORD);
    select_array_in(JIFFLES, PENTA_3rd_rise);
    pitch = {3, 1024};
    break;

  case 162:
    name = F("klassisch und simpel schön");
    sync = 0;
    select_array_in(SCALES, tetraCHORD);
    select_array_in(JIFFLES, pentatonic_desc);
    pitch = {3, 1024};
    break;

  case 163:
    name = F("harmonikali jazzsession");
    // laaaaangi pause!		sehr laaaangsam TODO: FIXME: ################
    //comment: harmonical cycle    54'  4"
    date = F("2018-11-21_20h21m11s	WED");
    select_array_in(SCALES, minor_scale);
    select_array_in(JIFFLES, tumtum);
    sync = 5;
    pitch = {32768, 200};
    // CYC: 2^0	   54'  4"
    subcycle_octave = -4;	// 3' 23"	| subcycle |
    break;

  case 164:
    name = F("busy soundscape");      // zarts soundgflecht
    date = F("");
    sync = 5;
    stack_sync_slices = 16;
    select_array_in(SCALES,europ_PENTAtonic );
    select_array_in(JIFFLES, pent_patternA);
    pitch = {1, 262};	// metric c
    chromatic_pitch = 4;
    break;

  case 165:
    name = F("stacked clicks");	// stacked start :)
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

  case 166:
    name = F("nice little dance");
    date = F("2019-04-11_10h");
    select_array_in(SCALES, triad);
    select_array_in(JIFFLES, pent_top_wave_0);
    sync = 1;
    stack_sync_slices = 1024;
    // base_pulse = 15;	// TODO: rethink that
    pitch = {1, 196};
    chromatic_pitch = 11;	// g
    break;

  case 167:
    // check start
    name = F("mad accordeon players whichdance with old indio's flute babbling");
    date = F("2018-11-21_17h35m59s	WED");
    select_array_in(SCALES, pentatonic_minor);
    select_array_in(JIFFLES, dwn_THRD_up);
    sync = 0;
    pitch = {32768, 384};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	1h 52' 38"
    subcycle_octave = -5;	// 3' 31"	| subcycle |
    break;

  case 168:
    name = F("noname starting 2019-04-11_12h");
    date = F("2019-04-11_12h");
    select_array_in(SCALES, pentatonic_minor);
    select_array_in(JIFFLES, pentatonic_desc);
    sync = 0;
    stack_sync_slices = 32;
    // base_pulse = 15;	// TODO: rethink that
    pitch = {1, 220};
    chromatic_pitch = 1;	// a
    // subcycle_octave = 3;	// TODO: rethink that
    break;

  case 169:
    name = F("crazy, roboterhafts piepse");
    //# bigBang
    //# favorite
    //# shortIsOk
    //# melodies
    //# harmonies
    //# crazy
    //# classicHarmonical
    //# nervous
    //# peep
    //# alarm
    //# robot
    //# noise
    //# dropouts
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

  case 170:
    name = F("veryverybad");
    //# shortIsOk
    //# dense
    //# melancholic
    //# dramatic
    //# crazy
    //# chaotic
    select_array_in(SCALES, europ_PENTAtonic);
    sync = 3;
    select_array_in(JIFFLES, tetraCHORD_desc);
    pitch = {1, 12};	// 8	*metric* c
    // harmonical CYCLE: 1h 6' 2"	SUBCYCLE: | 4' 8" |
    break;

    // >>>> *DO NOT FORGET* to update MUSICBOX_PRESETs <<<<

  default:
    MENU.error_ln(F("invalid preset"));		// ERROR, should not happen
    return true;
  }

  MENU.tab();
  parameters_by_user();
  HARMONICAL.reduce_fraction(&pitch);		// ;)

  if(name) {
    MENU.tab();
    MENU.out(name);
  }
  if(date) {
    MENU.tab();
    MENU.out(date);
  }
  MENU.ln();
  musicBox_short_info();

  return false;		// ok, no error
}


/*
  some favorites, ideas

  name = F("a love like the morning birds");
  name = F("wunderschöns obenabe vogelgetzwitscher bistüüfinbass :)");
  name = F("motivtanz");
  name = F("langsam sprudelnd");
  name = F("Good Night Choir");

  name = F("Marianne 1	eingesperrt<->Freiheit	sync? 4|1024");
	what am i doing here?
	i can here the birds outside
	why am I in here?
	freedom is sleeping

  name = F("es sphärischs netz vo eigenartige melodie wiegt über öis");
  name = F("busy soundscape");      // zarts soundgflecht
  name = F("another nice simple one");	// hübsch
  name = F("sehr eifach & schön");		// ähnlich love like a morning bird
  name = F("rising ostinatos over rhythmic bordun");  // nice and harmonic, humming, good ending
  name = F("2019-02-07_classical_time_machine_S4");
  name = F("e psinnlichi ziit");	// some morningbirds too
  name = F("klassisch TIMEMACHINE style");
  name = F("eifach und schön");	// harmonical cycle    55' 27"

  name = F("stacked clicks");	// stacked start :)
	make a very short piece?

  name = F("rhythm tuned in");
  name = F("e rechts füürwerch a jubelnde wirbel oderso :)");
  name = F("The Landing Of The Sound Space Ship");
 31	name = F("sanfti landig")
*/

/*
 *short* samples list for BRACHE 2019-04
 8	name = F("wunderschöns obenabe vogelgetzwitscher bistüüfinbass :)");
 9	name = F("e rechts füürwerch a jubelnde wirbel oderso :)");
 10	name = F("fröhlichs, witzigs Tänzli :)");
 11	name = F("klassisch TIMEMACHINE style");
 12	name = F("a love like the morning birds");
 14	name = F("2019-02-01_aRainOfJoy");
 18	name = F("ewig tickends uhr gloggespiel");
 31	name = F("sanfti landig")
 34	name = F("hübsch, langipausenamaafang");
 38	name = F("allereifachsts melodischs iiticke");
 45	name = F("wienen rägeboge vo striicher wo abeschwebed uf t erde bis tüüf in bass und meh und meh i en zyclische reige iistimmed");
 46	name = F("e psinnlichi ziit");	// some morningbirds too
 47	name = F("es sphärischs netz vo eigenartige melodie wiegt über öis");
 48	name = F("eifach und schön");	// harmonical cycle    55' 27"
 49	name = F("herzig ;)");
 55	name = F("uufregig im hüehnerstall weg de chileglogge");
 57	name = F("klassisch, schön, als grundlag zu eme 22' 1\" stuck");
 64	name = F("lost name");
 67	name = F("sehr eifach & schön");		// ähnlich love like a morning bird
 74	name = F("rhythms");
 75	name = F("ruhig schwebend");
 77	name = F("tanboura");
 78	name = F("melodischs Netz uf I IV V");
 83	name = F("MusikdoseTraum");
 89	name = F("The Landing Of The Sound Space Ship");
 93	name = F("Saiteresonanze");
 98	name = F("Harmonikale Schrittanz");
 101	name = F("Good Night Choir");
 104	name = F("pentatonic reaching down to rhythm");
 110	name = F("another nice simple one");	// hübsch
 135	name = F("daydreams");
 137	name = F("TRIAD");
 138	name = F("rhythm tuned in");
 141	name = F("2019-02-04_doric_cycle_S1");
 142	name = F("2019-02-04_bordun");	// with rhythm
 148	name = F("und es ratteret und brummet tickend de motor");	// sehr starche puls!	TODO: test!
 150	name = F("simplicistic_d0");
 151	name = F("2019-02-07_classical_time_machine_S4");
 152	name = F("noname_");
 155	name = F("Marianne 1	eingesperrt<->Freiheit	sync? 4|1024");
 156	name = F("Marianne 2	warten");
 161	name = F("au sehr schön:");
 163	name = F("fractal time web");
 164	name = F("busy soundscape");      // zarts soundgflecht
 165	name = F("stacked clicks");	// stacked start :)
 167	name = F("langsam sprudelnd");
 170	name = F("motivtanz");
*/
