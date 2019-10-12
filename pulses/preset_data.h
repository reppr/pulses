/*
  preset_data.h

  this is a *temporary* file version to test sample configurations
	collected over developping time

  also see: preset-hashtags.txt
*/

#define MUSICBOX_PRESETs	176	// default: all presets, can get replaced by SHORT_PRESET_COLLECTION

// *all* existing presets should be playable now :)
#define SHORT_PRESET_COLLECTION	176	// only the first <nn> presets are at choice, redefines MUSICBOX_PRESETs

bool /*error*/ load_preset(int new_preset) {	// TODO: sets preset, how to unset? ################
  if(new_preset < 1 || new_preset > MUSICBOX_PRESETs)
    return true;	// *ERROR*

  musicBoxConf.preset = new_preset;
  MENU.out(F("load preset "));
  MENU.out(musicBoxConf.preset);

  // initialisation and defaults:
  musicBoxConf.name=NULL;
  musicBoxConf.date=NULL;
  musicBoxConf.pitch={1,1};
  musicBoxConf.stack_sync_slices = 0;
  PULSES.time_unit = 1000000;	// DEFAULT	a bit rude

  switch(musicBoxConf.preset) {
  case 1:
    musicBoxConf.name = F("wunderschöns obenabe vogelgetzwitscher bistüüfinbass :)");
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
    musicBoxConf.date = F("2018-11-06_15h52m17s	TUE");
    select_in(SCALES, octaves_fifths);
    select_in(JIFFLES, dwn_THRD_up);
    musicBoxConf.sync = 5;
    musicBoxConf.pitch = {32768, 322};
    break;

  case 2:
    musicBoxConf.name = F("LIFT OFF");
    //comment: :) :) :) :) :)
    //# FAVORITE
    //# shortIsOk
    //# mystic
    //# SoundSpaceShip
    //# cosmic
    musicBoxConf.date = F("2018-12-11_20h48m37s	TUE");
    select_in(SCALES, doric_scale);
    select_in(JIFFLES, PENTAtonic_rise);
    musicBoxConf.sync = 5;
    musicBoxConf.pitch = {32768, 217};
    // fix:	play_KB_macro  /2
    // CYC: 2^0	   49' 50"
    musicBoxConf.subcycle_octave = -3;	// 6' 14"	| subcycle |
    break;

  case 3:
    musicBoxConf.name = F("Blue Night Cadence");
    //# FAVORITE
    //# harmonie	a bit like some jazz chord progressions
    //# shortIsOk
    //# systemStress seems cured :)
    //# aesthetic
    //# birds
    //comment: very rich chord progression :) :) :)
    musicBoxConf.date = F("2018-12-19_20h24m56s	WED");
    select_in(SCALES, pentatonic_minor);
    select_in(JIFFLES, pentatonic_rise);
    musicBoxConf.sync = 4;
    musicBoxConf.pitch = {32768, 292};
    // harmonical CYCLE: 1h 14' 4"	SUBCYCLE: | 4' 38" |
    musicBoxConf.subcycle_octave = -4;
    break;

  case 4:
    musicBoxConf.name = F("harpsichord");
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
    musicBoxConf.date = F("2018-11-13_12h11m04s	TUE");
    select_in(SCALES, doric_scale);
    select_in(JIFFLES, din__dididi_dixi);
    musicBoxConf.sync = 2;
    // fixed tuning d	294
    musicBoxConf.pitch = {32768, 294};
    // CYC: 2^0	1h 13' 33"
    break;

  case 5:
    musicBoxConf.name = F("dragons dancing in the sky");
    //comment: :) :) :)
    //# FAVORITE
    //# melodies
    //# shortIsOk
    //# mystic
    //# aurora
    //# lesley	a bit
    //# aesthetic
    musicBoxConf.date = F("2018-12-22_19h57m19s	SAT");
    select_in(SCALES, tetraCHORD);
    select_in(JIFFLES, PENTAtonic_desc);
    musicBoxConf.sync = 2;
    musicBoxConf.pitch = {32768, 263};
    // fix:	play_KB_macro  *2
    // harmonical CYCLE: 4h 23' 8"	SUBCYCLE: | 4' 7" |
    musicBoxConf.subcycle_octave = -6;
    break;

  case 6:
    musicBoxConf.name = F("aurora elfetanz");
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
    musicBoxConf.date = F("2018-11-13_11h57m23s	TUE");
    select_in(SCALES, tetraCHORD);
    select_in(JIFFLES, dwn_THRD_up);
    musicBoxConf.sync = 1;
    // fixed tuning g	196
    musicBoxConf.pitch = {32768, 196};
    // CYC: 2^0	2h 56' 32"
    break;

  case 7:
    musicBoxConf.name = F("klassisch, wunderschön, vo A bis Z :)");
    //comment: und übrigens:	harmonical cycle    12' 19"
    //# FAVORITE
    //# harmonies
    //# shortIsOk
    //# classicHarmonical
    //# cosmic
    //# nice
    //# simple
    //# aesthetic
    musicBoxConf.date = F("2018-11-18_20h09m42s	SUN");
    select_in(SCALES, major_scale);
    select_in(JIFFLES, din_dididi);
    musicBoxConf.sync = 2;
    musicBoxConf.pitch = {32768, 351};
    // CYC: 2^0	   12' 19"
    musicBoxConf.subcycle_octave = -2;	// 3'  5"	| subcycle |
    break;

  case 8:
    musicBoxConf.name = F("wunderschön melodisch :) :) :)");
    //comment: mit schönem, aberendlosem schluss ;)
    //# FAVORITE
    //# bigBang		// sometimes problematic	use autostack_S0
    //# melodies
    //# harmonie
    //# shortIsOk
    //# dropouts
    //# aesthetic	beware of dropouts
    musicBoxConf.date = F("2018-11-19_17h42m49s	MON");
    select_in(SCALES, octaves_fifths);
    select_in(JIFFLES, dwn_THRD);
    musicBoxConf.sync = 0;
    musicBoxConf.pitch = {32768*2, 281};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	14d 14h 15' 26"
    musicBoxConf.subcycle_octave = -1;	// 2	    5'  8"	| subcycle |
    break;

  case 9:
    musicBoxConf.name = F("e rechts füürwerch a jubelnde wirbel oderso :)");
    //# FAVORITE
    //# melodies
    //# shortIsOk
    //# dense
    //# aurora
    //# lesley
    //# mystic
    //# cosmic
    //# aesthetic
    musicBoxConf.date = F("2018-11-07_17h36m34s	WED");
    select_in(SCALES, major_scale);
    select_in(JIFFLES, penta_3rd_down_5);
    musicBoxConf.sync = 1;
    musicBoxConf.pitch = {32768, 229};
    // CYC: 2^0	   18' 53"
    break;

  case 10:
    musicBoxConf.name = F("2019-02-01_aRainOfJoy");
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
    musicBoxConf.date = F("2019-02-02_12h12m11s	SAT");
    select_in(SCALES, europ_PENTAtonic);
    select_in(JIFFLES, penta_3rd_down_5);
    musicBoxConf.sync = 1;
    // random *metric* tuning d
    musicBoxConf.pitch = {32768, 294};
    // fix:	play_KB_macro  *2
    // harmonical CYCLE: 58' 51"	SUBCYCLE: | 3' 41" |
    musicBoxConf.subcycle_octave = -4;
    break;

  case 11:
    musicBoxConf.name = F("genial jazzpiano version");
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
    select_in(SCALES, pentachord);
    musicBoxConf.sync = 0;
    select_in(JIFFLES, tumtumtum);
    musicBoxConf.pitch = {1, 12};	// 8	*metric* a
    // harmonical CYCLE: 6h 33' 12"	SUBCYCLE: | 6' 9" |
    break;

  case 12:
    musicBoxConf.name = F("a love like the morning birds");
    //# FAVORITE
    //# melodies
    //# harmonies
    //# shortIsOk
    //# birds
    //# happy
    //# simple
    //# aesthetic
    musicBoxConf.date = F("2019-01-06_21h29m36s	SUN");
    select_in(SCALES, octaves_fourths);
    select_in(JIFFLES, PENTAtonic_desc);
    musicBoxConf.sync = 4;
    musicBoxConf.pitch = {32768, 371};
    // fix:	play_KB_macro  *2
    // harmonical CYCLE: 8d 6h 58' 1"	SUBCYCLE: | 5' 50" |
    musicBoxConf.subcycle_octave = -11;
    break;

  case 13:
    musicBoxConf.name = F("sehr ruhigs festlichs stimmigsbild");
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
    musicBoxConf.date = F("2018-11-06_20h02m08s	TUE");
    select_in(SCALES, major_scale);
    select_in(JIFFLES, diing_ditditdit);
    musicBoxConf.sync = 5;
    musicBoxConf.pitch = {32768, 417};
    // fix:	play_KB_macro  *2
    break;

  case 14:
    musicBoxConf.name = F("ewig tickends uhr gloggespiel");
    //# FAVORITE
    //# shortIsOk
    //# simple
    //# simlicistic
    //# clock
    //# elementary
    musicBoxConf.date = F("2018-11-07_17h49m47s	WED");
    select_in(SCALES, octave_4th_5th);
    select_in(JIFFLES, diing_ditditdit);
    musicBoxConf.sync = 1;
    musicBoxConf.pitch = {32768, 410};
    // CYC: 2^0	5h 37' 35"
    break;

  case 15:
    musicBoxConf.name = F("tickende uhr");
    //# clock
    //# favorite+
    //# shortIsOk
    //# classicHarmonical
    //# simlicistic
    //# elementary
    musicBoxConf.date = F("2018-11-06_20h48m16s	TUE");
    select_in(SCALES, octaves_fifths);
    select_in(JIFFLES, ding_ditditdit);
    musicBoxConf.sync = 1;
    musicBoxConf.pitch = {32768, 214};
    // fix:	play_KB_macro  *2
    break;

  case 16:
    musicBoxConf.name = F("langsams iiordne in es ruhigs piipskonzert");
    //# FAVORITE
    //# shortIsOk
    //# harmonies
    //# classicHarmonical
    //# nice
    //# simple
    //# simlicistic
    //# elementary
    //# beep
    musicBoxConf.date = F("2018-11-08_18h05m59s	THU");
    select_in(SCALES, octaves_fifths);
    select_in(JIFFLES, din__dididi_dixi);
    musicBoxConf.sync = 3;
    // fixed tuning e	330
    musicBoxConf.pitch = {32768, 330};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	12d 10h 14' 58"
    break;

  case 17:
    musicBoxConf.name = F("Ouverture");
    //comment: :) :) :) !!!
    //# FAVORITE
    //# dropouts
    //# heavyStart		// sometimes problematic	use autostack_S0
    //# rhytm&pulse
    musicBoxConf.date = F("2018-12-09_15h24m13s	SUN");
    select_in(SCALES, octave_4th_5th);
    select_in(JIFFLES, PENTAtonic_rise);
    musicBoxConf.sync = 0;
    musicBoxConf.pitch = {32768, 375};
    // CYC: 2^0	6h  9'  5"
    musicBoxConf.subcycle_octave = -5;	// 11' 32"	| subcycle |
    break;

  case 18:
    musicBoxConf.name = F("klassisch TIMEMACHINE style");
    // :)
    //# FAVORITE
    //# harmonies
    //# shortIsOk
    //# systemStress	just occasionally
    //# classicHarmonical
    //# nice
    //# simple
    //# aesthetic
    musicBoxConf.date = F("2018-11-07_18h20m53s	WED");
    select_in(SCALES, minor_scale);
    select_in(JIFFLES, ding_ditditdit);
    musicBoxConf.sync = 2;
    musicBoxConf.pitch = {32768, 359};
    // no_octave_shift = true;
    // fix:	play_KB_macro  *2
    // CYC: 2^0	2h  0' 29"
    break;

  case 19:
    musicBoxConf.name = F("simple, hübsche bigBang");
    //# bigBang	is ok	sometimes problematic	use autostack_S0
    //# FAVORITE
    //# shortIsOk
    //# bigBang
    //# melodies
    //# harmonies
    //# classicHarmonical
    //# nice
    //# simple
    //# aesthetic
    musicBoxConf.date = F("2018-11-08_14h38m32s	THU");
    select_in(SCALES, pentatonic_minor);
    select_in(JIFFLES, ding_ditditdit);
    musicBoxConf.sync = 0;
    musicBoxConf.pitch = {32768, 210};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	3h 25' 58"
    break;

  case 20:
    musicBoxConf.name = F("nice, simple slow harmonics");
    //# FAVORITE
    //# shortIsOk
    //# melodies
    //# harmonies
    //# aesthetic
    //# nice
    //# simple
    //# birds
    //# hasSlowPulse
    //# lyric
    musicBoxConf.date = F("2018-11-13_15h04m11s	TUE");
    select_in(SCALES, octaves_fifths);
    select_in(JIFFLES, penta_3rd_rise);
    musicBoxConf.sync = 2;
    // fixed tuning f	175
    musicBoxConf.pitch = {32768, 175};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	11d 17h 12' 24"
    break;

  case 21:
    musicBoxConf.name = F("klassischi pent brachemagie");
    //# FAVORITE
    //# shortIsOk
    //# melodies
    //# harmonies
    //# aesthetic
    //# nice
    //# simple
    //# birds
    musicBoxConf.date = F("2018-11-13_12h23m54s	TUE");
    select_in(SCALES, europ_PENTAtonic);
    select_in(JIFFLES, pentatonic_rise);
    musicBoxConf.sync = 5;
    // fixed tuning b	233
    musicBoxConf.pitch = {32768, 233};
    // CYC: 2^0	1h 14' 15"
    break;

  case 22:
    musicBoxConf.name = F("hübsche start mit geknatter");
    //# FAVORITE(-)	has dropouts, (nice) system stress
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
    musicBoxConf.date = F("2018-11-13_11h40m30s	TUE");
    select_in(SCALES, octave_4th_5th);
    select_in(JIFFLES, pentatonic_desc);
    musicBoxConf.sync = 0;
    // fixed tuning g	196
    musicBoxConf.pitch = {32768, 196};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	11h 46'  9"
    break;

  case 23:
    musicBoxConf.name = F("fröhlichs, crazy witzigs Tänzli :)");
    //# favorite	DADA?
    //# shortIsOk
    //# crazy
    //# funny
    //# chaotic	a bit
    musicBoxConf.date = F("2018-11-08_14h27m25s	THU");
    select_in(SCALES, major_scale);
    select_in(JIFFLES, simple_theme);
    musicBoxConf.sync = 5;
    musicBoxConf.pitch = {32768, 380};
    // fix:	play_KB_macro  *2
    break;

  case 24:
    musicBoxConf.name = F("wienen rägeboge vo striicher wo abeschwebed uf t erde bis tüüf in bass und meh und meh i en zyclische reige iistimmed");
    //# FAVORITE
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
    musicBoxConf.date = F("2018-11-21_13h23m41s	WED");
    select_in(SCALES, doric_scale);
    select_in(JIFFLES, rising_pent_them);
    musicBoxConf.sync = 1;
    musicBoxConf.pitch = {32768, 295};
    // CYC: 2^0	1h 13' 19"
    musicBoxConf.subcycle_octave = -4;	// 4' 35"	| subcycle |
    break;

  case 25:
    musicBoxConf.name = F("e psinnlichi ziit");	// some morningbirds too
    //# FAVORITE
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
    musicBoxConf.date = F("2018-11-21_14h33m31s	WED");
    select_in(SCALES, octave_4th_5th);
    select_in(JIFFLES, pentatonic_desc);
    musicBoxConf.sync = 2;
    musicBoxConf.pitch = {32768, 354};
    // fix:	play_KB_macro  *4
    // CYC: 2^0	1d 2h  3' 55"
    musicBoxConf.subcycle_octave = -9;	// 3'  3"	| subcycle |
    break;

  case 26:
    musicBoxConf.name = F("en andere brachklassiker");
    //# FAVORITE+	wunderschön :)
    //# shortIsOk
    //# harmonies
    //# nice
    //# lesley
    //# SoundSpaceShip
    //comment: müestemalnolengerlose...
    //comment: schöni uufbou
    //comment: vilichtmengischauchlilangwiiligi?
    musicBoxConf.date = F("2018-11-13_15h12m50s	TUE");
    select_in(SCALES, major_scale);
    select_in(JIFFLES, pentatonic_rise);
    musicBoxConf.sync = 5;
    // fixed tuning a	220
    musicBoxConf.pitch = {32768, 220};
    // CYC: 2^0	   19' 40"
    break;

  case 27:
    musicBoxConf.name = F("nomal en timemachine klassiker");
    //# FAVORITE
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
    musicBoxConf.date = F("2018-11-13_17h33m15s	TUE");
    select_in(SCALES, europ_PENTAtonic);
    select_in(JIFFLES, ding_ditditdit);
    musicBoxConf.sync = 1;
    // fixed tuning b	233
    musicBoxConf.pitch = {32768, 233};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	2h 28' 30"
    break;

  case 28:
    musicBoxConf.name = F("PENT eifache reige");
    //# FAVORITE
    //# shortIsOk
    //# melodies
    //# harmonies
    //# classicHarmonical
    //# aesthetic
    //# nice
    //# simple
    //# cosmic
    musicBoxConf.date = F("2018-11-14_13h27m35s	WED");
    select_in(SCALES, europ_PENTAtonic);
    select_in(JIFFLES, ding_ditditdit);
    musicBoxConf.sync = 5;
    // fixed tuning a	220
    musicBoxConf.pitch = {32768, 220};
    // fix:	play_KB_macro  /2
    // CYC: 2^0	   39' 19"
    break;

  case 29:
    musicBoxConf.name = F("triggered");
    //# FAVORITE(?)
    //# shortIsOk
    //# melodies
    //# harmonies
    //# aesthetic	complex
    //# nice		;)
    //# simple
    //# SoundSpaceShip
    //# hasSlowPulse
    //comment: da lauft öppis, so simpel das es bliibt	:)
    musicBoxConf.date = F("2018-11-14_14h58m29s	WED");
    select_in(SCALES, minor_scale);
    select_in(JIFFLES, PENTAtonic_rise);
    musicBoxConf.sync = 3;
    // fixed tuning f	175
    musicBoxConf.pitch = {32768, 175};
    // CYC: 2^0	1h  1' 47"
    break;

  case 30:
    musicBoxConf.name = F("short cycled !");
    //# FAVORITE
    //# bigBang		// sometimes problematic	use autostack_S0
    //# heavyStart	// problematic, but passes, sometimes ;)	use autostack_S0
    //# fullCycleIsOk	// TODO: test
    //# shortIsOk
    //# strange
    //# harmonies
    //# mystic
    //# aesthetic	wide
    musicBoxConf.date = F("2018-11-14_19h47m20s	WED");
    select_in(SCALES, major_scale);
    select_in(JIFFLES, pentatonic_rise);
    musicBoxConf.sync = 0;
    // fixed tuning e	330
    musicBoxConf.pitch = {32768, 330};
    // CYC: 2^0	   13'  6"
    break;

  case 31:
    musicBoxConf.name = F("sanfti landig");
    //# FAVORITE
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
    musicBoxConf.date = F("2018-11-15_09h10m37s	THU");
    select_in(SCALES, europ_PENTAtonic);
    select_in(JIFFLES, dwn_THRD_up);
    musicBoxConf.sync = 1;
    // fixed tuning a	220
    musicBoxConf.pitch = {32768, 220};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	2h 37' 17"
    break;

  case 32:
    musicBoxConf.name = F("schön, ruhig, eifach");
    //# FAVORITE
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
    musicBoxConf.date = F("2018-11-15_15h15m34s	THU");
    select_in(SCALES, tetraCHORD);
    select_in(JIFFLES, pentatonic_rise);
    musicBoxConf.sync = 3;
    // fixed tuning g	196
    musicBoxConf.pitch = {32768, 196};
    // fix:	play_KB_macro  *4
    // CYC: 2^0	11h 46'  9"
    break;

  case 33:
    musicBoxConf.name = F("magical toilet gejodel");
    //# FAVORITE(crazy)		has some light stress and maybe dropout issues, but sounds crazy ok
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
    musicBoxConf.date = F("2018-11-15_15h38m13s	THU");
    select_in(SCALES, doric_scale);
    select_in(JIFFLES, penta_3rd_down_5);
    musicBoxConf.sync = 5;
    // fixed tuning c	262
    musicBoxConf.pitch = {32768, 262};
    // CYC: 2^0	1h 22' 33"
    break;

  case 34:
    musicBoxConf.name = F("hübsch, langipausenamaafang");
    //# FAVORITE
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
    musicBoxConf.date = F("2018-11-15_15h53m44s	THU");
    select_in(SCALES, europ_PENTAtonic);
    select_in(JIFFLES, dwn_THRD_up);
    musicBoxConf.sync = 5;
    // fixed tuning b	233
    musicBoxConf.pitch = {32768, 233};
    // fix:	play_KB_macro  *4
    // CYC: 2^0	4h 57'  1"
    musicBoxConf.subcycle_octave = -3;	// 37'  8"	| subcycle |
    break;

  case 35:
    musicBoxConf.name = F("uusprägts melodiemotiv, fughetta");
    //# FAVORITE
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
    musicBoxConf.date = F("2018-11-16_12h29m17s	FRI");
    select_in(SCALES, octaves_fifths);
    select_in(JIFFLES, PENTA_3rd_rise);
    musicBoxConf.sync = 5;
    musicBoxConf.pitch = {32768, 383};
    // fix:	play_KB_macro  /2
    // used subcycle:    3' 46"
    // harmonical cycle 2d 16h 14' 40"
    break;

  case 36:
    musicBoxConf.name = F("maschinells ticke, klassisch");
    //# FAVORITE
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
    //# bigBang		// sometimes problematic	use autostack_S0
    //# heavyStart	should go ok,  but sometimes problematic	use autostack_S0
    //# clock
    //# hasSlowPulse
    //# lyric
    musicBoxConf.date = F("2018-11-17_11h59m15s	SAT");
    select_in(SCALES, europ_PENTAtonic);
    select_in(JIFFLES, ding_ditditdit);
    musicBoxConf.sync = 0;
    musicBoxConf.pitch = {32768, 207};
    // fix:	play_KB_macro  *4
    // CYC: 2^0	5h 34' 19"
    musicBoxConf.subcycle_octave = -8;	// 1' 18"	| subcycle |
    break;

  case 37:
    musicBoxConf.name = F("sehr sehr spezielle S0 aafang, wunderschön");
    //# FAVORITE
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
    musicBoxConf.date = F("2018-11-17_20h41m20s	SAT");
    select_in(SCALES, europ_PENTAtonic);
    select_in(JIFFLES, PENTAtonic_desc);
    musicBoxConf.sync = 0;
    musicBoxConf.pitch = {32768, 379};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	1h 31' 18"
    musicBoxConf.subcycle_octave = -4;	// 5' 42"	| subcycle |
    break;

  case 38:
    musicBoxConf.name = F("allereifachsts melodischs iiticke");
    //# FAVORITE
    //# shortIsOk
    //# rhytm&pulse
    //# structure
    //# nice
    //# simple
    //# simlicistic
    //# fractal
    //# elementary
    //# clock
    //# beep
    //# hasSlowPulse
    //comment: schluss müest me no händisch mache...
    musicBoxConf.date = F("2018-11-18_20h26m18s	SUN");
    select_in(SCALES, octaves_fifths);
    select_in(JIFFLES, ding_ditditdit);
    musicBoxConf.sync = 1;
    musicBoxConf.pitch = {32768, 420};
    // CYC: 2^0	4d 21h 10' 10"
    musicBoxConf.subcycle_octave = -1;	// 1	    3' 26"	| subcycle |
    break;

  case 39:
    musicBoxConf.name = F("näbel");
    //# FAVORITE
    //# systemStress	really???
    musicBoxConf.date = F("2018-11-19_10h30m25s	MON");
    select_in(SCALES, minor_scale);
    select_in(JIFFLES, dwn_THRD_up);
    musicBoxConf.sync = 2;
    musicBoxConf.pitch = {32768, 200};	// quite random to tune it lower
    // was: musicBoxConf.pitch = {32768, 399};
    // fix:	play_KB_macro  *4
    // CYC: 2^0	3h 36' 48"
    musicBoxConf.subcycle_octave = -6;	// 3' 23"	| subcycle |
    break;

  case 40:
    musicBoxConf.name = F("jazzig");
    //# FAVORITE
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
    musicBoxConf.date = F("2018-11-19_20h29m59s	MON");
    select_in(SCALES, doric_scale);
    select_in(JIFFLES, tumtum);
    musicBoxConf.sync = 4;
    musicBoxConf.pitch = {32768, 217};
    // fix:	play_KB_macro  /2
    // CYC: 2^0	   49' 50"
    musicBoxConf.subcycle_octave = -4;	// 3'  7"	| subcycle |
    break;

  case 41:
    musicBoxConf.name = F("chileglogge?");
    //# FAVORITE
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
    musicBoxConf.date = F("2018-11-20_11h40m15s	TUE");
    select_in(SCALES, pentatonic_minor);
    select_in(JIFFLES, up_THRD);
    musicBoxConf.sync = 1;
    musicBoxConf.pitch = {32768, 172};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	2h  5' 44"
    musicBoxConf.subcycle_octave = -5;	// 3' 56"	| subcycle |
    break;

  case 42:
    musicBoxConf.name = F("lesley");
    //# FAVORITE
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
    musicBoxConf.date = F("2018-11-20_14h34m14s	TUE");
    select_in(SCALES, minor_scale);
    select_in(JIFFLES, rising_pent_them);
    musicBoxConf.sync = 4;
    musicBoxConf.pitch = {32768, 195};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	1h 50' 54"
    musicBoxConf.subcycle_octave = -5;	// 3' 28"	| subcycle |
    break;

  case 43:
    musicBoxConf.name = F("vorbereitige zum abflug?");
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
    musicBoxConf.date = F("2018-11-20_15h15m48s	TUE");
    select_in(SCALES, europ_PENTAtonic);
    select_in(JIFFLES, up_THRD_dn);
    musicBoxConf.sync = 1;
    musicBoxConf.pitch = {32768, 225};
    // CYC: 2^0	1h 16' 54"
    musicBoxConf.subcycle_octave = -4;	// 4' 48"	| subcycle |
    break;

  case 44:
    musicBoxConf.name = F("melodischs tänzle");
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
    musicBoxConf.date = F("2018-11-20_20h31m27s	TUE");
    select_in(SCALES, europ_PENTAtonic);
    select_in(JIFFLES, pent_top_wave);
    musicBoxConf.sync = 5;
    musicBoxConf.pitch = {32768, 323};
    // CYC: 2^0	   53' 34"
    musicBoxConf.subcycle_octave = -4;	// 3' 21"	| subcycle |
    break;

  case 45:
    musicBoxConf.name = F("motivtanz");
    //# FAVORITE
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
    musicBoxConf.date = F("2019-04-11_18h21m28s	THU");
    select_in(SCALES, octave_4th_5th);
    select_in(JIFFLES, up_THRD);
    musicBoxConf.sync = 4;
    musicBoxConf.stack_sync_slices = 0;
    // musicBoxConf.base_pulse = 15;	// TODO: rethink that
    musicBoxConf.pitch = {1, 175};
    musicBoxConf.chromatic_pitch = 9;	// f
    // musicBoxConf.subcycle_octave = 6;	// TODO: rethink that
    // cycle 9h 59' 10"  subcycle | 9' 22" |
    break;

  case 46:
    musicBoxConf.name = F("madrigal machine");
    //# FAVORITE
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
    musicBoxConf.date = F("2019-04-11_14h");
    select_in(SCALES, tetraCHORD);
    select_in(JIFFLES, mechanical);
    musicBoxConf.sync = 2;
    musicBoxConf.stack_sync_slices = 256;
    // musicBoxConf.base_pulse = 15;	// TODO: rethink that
    musicBoxConf.pitch = {1, 233};
    musicBoxConf.chromatic_pitch = 2;	// a#
    // musicBoxConf.subcycle_octave = 4;	// TODO: rethink that
    break;

  case 47:
    musicBoxConf.name = F("es sphärischs netz vo eigenartige melodie wiegt über öis");
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
    musicBoxConf.date = F("2018-11-21_15h49m05s	WED");
    select_in(SCALES, minor_scale);
    select_in(JIFFLES, pent_patternA);
    musicBoxConf.sync = 3;
    musicBoxConf.pitch = {32768, 161};
    // used SUBCYCLE:    4' 12"
    break;

  case 48:
    musicBoxConf.name = F("eifach und schön, blue");	// harmonical cycle    55' 27"
    // simile 'Blue Night Cadence'
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
    musicBoxConf.date = F("2018-11-21_16h08m40s	WED");
    select_in(SCALES, pentatonic_minor);
    select_in(JIFFLES, pentatonic_rise);
    musicBoxConf.sync = 5;
    musicBoxConf.pitch = {32768, 195};
    // usedSUBCYCLE:    3' 28"
    // CYC: 2^0	   55' 27"
    musicBoxConf.subcycle_octave = -4;	// 3' 28"	| subcycle |
    break;

  case 49:
    musicBoxConf.name = F("herzig ;)");
    //# FAVORITE
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
    musicBoxConf.date = F("2018-11-21_16h54m05s	WED");
    select_in(SCALES, europ_PENTAtonic);
    select_in(JIFFLES, pent_patternA);
    musicBoxConf.sync = 5;
    musicBoxConf.pitch = {32768, 266};
    // fix:	play_KB_macro  *4
    // CYC: 2^0	4h 20' 10"
    musicBoxConf.subcycle_octave = -6;	// 4'  4"	| subcycle |
    break;

  case 50:
    musicBoxConf.name = F("slow simple melodic ostinato dance");	// with fast ormusicBoxConf.naments: piepiep: Intensivstation
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
    select_in(SCALES, doric_scale);
    select_in(JIFFLES, rising_pent_them);
    musicBoxConf.sync = 5;
    musicBoxConf.pitch = {32768, 266};
    // fix:	play_KB_macro  /2
    // CYC: 2^0	   40' 39"
    musicBoxConf.subcycle_octave = -3;	// 5'  5"	| subcycle |
    break;

  case 51:
    musicBoxConf.name = F("langsam sprudelnd");
    //# FAVORITE
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
    musicBoxConf.date = F("2019-04-11_10h");
    select_in(SCALES, tetraCHORD);
    select_in(JIFFLES, up_THRD_dn);
    musicBoxConf.sync = 2;
    musicBoxConf.stack_sync_slices = -16;
    // musicBoxConf.base_pulse = 15;	// TODO: rethink that
    musicBoxConf.pitch = {1, 196};
    musicBoxConf.chromatic_pitch = 11;	// g
    // musicBoxConf.subcycle_octave = 5;	// TODO: rethink that
    break;

  case 52:
    musicBoxConf.name = F("The Landing Of The Fairies");
    //# FAVORITE
    //# shortIsOk
    //# fullCycleIsOk
    //# ritch
    //# fractal
    //# soundscape
    //# slow	nice slow bass
    //# fast	web
    //# floating
    //# motives
    //# melodies
    //# harmonies
    //# aesthetic
    //# nice
    //# mystic
    //# cosmic
    //# SoundSpaceShip
    //# fairies
    //# aurora
    //# sound
    //# lesley
    //# delay
    //# hasSlowPulse	barely audible
    //# lyric
    //# dream
    //# angels
    //# heaven
    //# air
    //# children
    //# dance
    //# festive
    //# solemn
    //# ritual
    //# happy
    select_in(SCALES, pentatonic_minor);
    musicBoxConf.sync = 1;
    select_in(JIFFLES, dwn_THRD);
    musicBoxConf.pitch = {1, 357};	// 8	*metric* c
    // harmonical CYCLE: 1h 6' 2"	SUBCYCLE: 2^3 | 11' 28" |	???
    break;

  case 53:
    musicBoxConf.name = F("church bells are still ringing");
    //# FAVORITE
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
    musicBoxConf.date = F("2018-11-21_21h12m38s	WED");
    select_in(SCALES, pentatonic_minor);
    select_in(JIFFLES, pent_patternA);
    musicBoxConf.sync = 2;
    musicBoxConf.pitch = {32768, 221};
    // CYC: 2^0	1h 37' 51"
    musicBoxConf.subcycle_octave = -5;	// 3'  3"	| subcycle |
    break;

  case 54:
    musicBoxConf.name = F("kid's room");
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
    musicBoxConf.date = F("2018-11-22_08h48m23s	THU");
    select_in(SCALES, octaves_fifths);
    select_in(JIFFLES, pent_top_wave_0);
    musicBoxConf.sync = 5;
    musicBoxConf.pitch = {32768, 415};
    // fix:	play_KB_macro  *2
    // used SUBCYCLE:    3' 28"
    break;

  case 55:
    musicBoxConf.name = F("uufregig im hüehnerstall weg de chileglogge");
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
    musicBoxConf.date = F("2018-11-22_09h26m06s	THU");
    select_in(SCALES, major_scale);
    select_in(JIFFLES, dwn_THRD_up);
    musicBoxConf.sync = 4;
    musicBoxConf.pitch = {32768/4, 308};
    // fix:	play_KB_macro  /2
    // CYC: 2^0	    7'  1"
    musicBoxConf.subcycle_octave = -1;	// 3' 31"	| subcycle |
    break;

  case 56:
    musicBoxConf.name = F("klassischi harmonikali aesthetik");
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
    musicBoxConf.date = F("2018-11-22_10h44m17s	THU");
    select_in(SCALES, major_scale);
    select_in(JIFFLES, pentatonic_desc);
    musicBoxConf.sync = 2;
    musicBoxConf.pitch = {32768, 329};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	   26' 18"
    musicBoxConf.subcycle_octave = -3;	// 3' 17"	| subcycle |
    break;

  case 57:
    musicBoxConf.name = F("klassisch, schön, als grundlag zu eme 22' 1\" stuck");
    // FIXME: is double speed on chamber orchestra
    //# FAVORITE+
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
    musicBoxConf.date = F("2018-11-22_11h45m56s	THU");
    select_in(SCALES, major_scale);
    select_in(JIFFLES, din_dididi);
    musicBoxConf.sync = 2;
    musicBoxConf.pitch = {32768, 393};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	   22'  1"
    musicBoxConf.subcycle_octave = -2;	// 5' 30"	| subcycle |
    break;

  case 58:
    musicBoxConf.name = F("eifach und eigenartig schön");
    //# FAVORITE
    //# shortIsOk
    //# motives
    //# melodies
    //# classicHarmonical
    //# aesthetic
    //# nice
    //# simple
    //# fractal
    //# hasSlowPulse
    //# lyric
    musicBoxConf.date = F("2018-11-25_16h18m50s	SUN");
    select_in(SCALES, pentatonic_minor);
    select_in(JIFFLES, pentatonic_rise);
    musicBoxConf.sync = 5;
    musicBoxConf.pitch = {32768, 167};
    // random octave shift: // fix:	play_KB_macro  *2
    // CYC: 2^0	2h  9' 30"
    musicBoxConf.subcycle_octave = -5;	// 4'  3"	| subcycle |
    break;

  case 59:
    musicBoxConf.name = F("gradnomal wunderschön :)");
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
    musicBoxConf.date = F("2018-11-25_16h31m56s	SUN");
    select_in(SCALES, europ_PENTAtonic);
    select_in(JIFFLES, PENTAtonic_rise);
    musicBoxConf.sync = 0;
    musicBoxConf.pitch = {32768, 261};
    // random octave shift: // fix:	play_KB_macro  *4
    // CYC: 2^0	4h 25'  9"
    musicBoxConf.subcycle_octave = -6;	// 4'  9"	| subcycle |
    break;

  case 60:
    musicBoxConf.name = F("landing, seed, growth, ....");
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
    musicBoxConf.date = F("2018-11-25_18h49m01s	SUN");
    select_in(SCALES, tetraCHORD);
    select_in(JIFFLES, dwn_THRD_up);
    musicBoxConf.sync = 1;
    musicBoxConf.pitch = {32768, 247};
    // random octave shift: // fix:	play_KB_macro  /2
    // CYC: 2^0	1h 10'  3"
    musicBoxConf.subcycle_octave = -4;	// 4' 23"	| subcycle |
    break;

  case 61:
    musicBoxConf.name = F("Baldrian");
    //comment: entspannig pur
    //comment: try longer cycles ;)
    //# FAVORITE
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
    musicBoxConf.date = F("2018-11-25_19h14m10s	SUN");
    select_in(SCALES, octave_4th_5th);
    select_in(JIFFLES, PENTA_3rd_down_5);
    musicBoxConf.sync = 2;
    musicBoxConf.pitch = {32768, 296};
    // random octave shift: // fix:	play_KB_macro  *2
    // CYC: 2^0	15h 35' 11"
    musicBoxConf.subcycle_octave = -8;	// 3' 39"	| subcycle |
    break;

  case 62:
    musicBoxConf.name = F("dorisch");
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
    musicBoxConf.date = F("2018-11-25_19h24m24s	SUN");
    select_in(SCALES, doric_scale);
    select_in(JIFFLES, din_dididi);
    musicBoxConf.sync = 0;
    musicBoxConf.pitch = {32768, 219};
    // random octave shift: used SUBCYCLE:	3' 5"y61
    // CYC: 2^0	1h 38' 45"
    musicBoxConf.subcycle_octave = -5;	// 3'  5"	| subcycle |
    break;

  case 63:
    musicBoxConf.name = F("rising ostinatos over rhythmic bordun");  // nice and harmonic, humming, good ending
    //comment: very nice ending :)
    //# FAVORITE
    //# SPECIALITY	from rhythm, beat to sound effects, everything's there

    //# heavyStart	but ok
    //# shortIsOk
    //# bigBang
    //# fractal		from rhythm, beat to sound effects, everything's there
    //# soundscape
    //# slow
    //# fast
    //# ticking		strongly audible very steady, driving pulse
    //# steady
    //# busy
    //# motives
    //# melodies
    //# harmonies	often strange and staccato
    //# rhytm&pulse	strongly audible very steady, driving pulse
    //# beat
    //# whirles
    //# waves
    //# structure
    //# aesthetic
    //# nice
    //# dramatic	no, but very steady, driving pulse
    //# sound
    //# noise
    //# humming
    //# rumble
    //# hasSlowPulse
    //# lyric
    //# children
    //# dance
    //# ritual
    musicBoxConf.sync = 0;
    select_in(SCALES, octave_4th_5th);
    select_in(JIFFLES, pentatonic_rise);
    musicBoxConf.pitch = {2, 6144};
    // harmonical cycle 'c'	12h 24'  7"	subcycle    5' 49"
    break;

  case 64:
    musicBoxConf.name = F("lost name");
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
    select_in(SCALES, pentatonic_minor);
    select_in(JIFFLES, PENTAtonic_rise);
    musicBoxConf.sync = 4;
    musicBoxConf.pitch = {32768, 211};
    // random octave shift: // fix:	play_KB_macro  *2
    // used SUBCYCLE:	3' 12"
    // CYC: 2^0	3h 24' 59"
    musicBoxConf.subcycle_octave = -6;	// 3' 12"	| subcycle |
    break;

  case 65:
    musicBoxConf.name = F("eigenartigi harmonischi melodiegebilde");
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
    select_in(SCALES, pentatonic_minor);
    select_in(JIFFLES, pentatonic_rise);
    musicBoxConf.pitch = {1, 128};
    musicBoxConf.sync = 4;
    break;

  case 66:
    musicBoxConf.name = F("no ohni name");
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
    //# lyric	er sitzt nur da und lost uf t vögel wo zwitschered im gebüsch
    //# lyric	(mer findet en en) gspässige Vogel
    musicBoxConf.date = F("2018-11-28_21h02m41s	WED");
    select_in(SCALES, octaves_fifths);
    select_in(JIFFLES, PENTA_3rd_rise);
    musicBoxConf.sync = 3;
    musicBoxConf.pitch = {32768, 375};
    // random octave shift:
    // CYC: 2^0	5d 11h 13' 47"
    musicBoxConf.subcycle_octave = -1;	// 0	    7' 41"	| subcycle |
    break;

  case 67:
    musicBoxConf.name = F("sehr eifach & schön");		// ähnlich love like a morning bird
    //# FAVORITE	// morning birds sibling :)
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
    musicBoxConf.date = F("2018-11-29_14h14m39s	THU");
    select_in(SCALES, octaves_fifths);
    select_in(JIFFLES, PENTAtonic_desc);
    musicBoxConf.sync = 2;
    musicBoxConf.pitch = {32768, 172};
    // random octave shift:
    // CYC: 2^0	5d 23h  3' 20"
    musicBoxConf.subcycle_octave = -1;	// 1	    4' 11"	| subcycle |
    break;

  case 68:
    musicBoxConf.name = F("wow");
    //# FAVORITE	+++ :)
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
    musicBoxConf.date = F("2018-12-01_12h11m13s	SAT");
    select_in(SCALES, doric_scale);
    select_in(JIFFLES, ding_ditditdit);
    musicBoxConf.sync = 4;
    musicBoxConf.pitch = {32768, 288};
    // random octave shift: // fix:	play_KB_macro  *2
    // CYC: 2^0	2h 30' 11"
    musicBoxConf.subcycle_octave = -5;	// 4' 42"	| subcycle |
    break;

  case 69:
    musicBoxConf.name = F("schön melodisch, mit recht guetem schluss");
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
    musicBoxConf.date = F("2018-12-02_20h43m32s	SUN");
    select_in(SCALES, major_scale);
    select_in(JIFFLES, pentatonic_desc);
    musicBoxConf.sync = 2;
    musicBoxConf.pitch = {32768, 379};
    // random octave shift: // fix:	play_KB_macro  *4
    // CYC: 2^0	   45' 39"
    musicBoxConf.subcycle_octave = -3;	// 5' 42"	| subcycle |
    break;

  case 70:
    musicBoxConf.name = F("PENTAtheme");
    //# favorite
    //# shortIsOk
    //# motives
    //# melodies
    //# harmonies
    //# heavyStart	// Störfall?
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
    musicBoxConf.date = F("2018-12-03_08h30m25s	MON");
    select_in(SCALES, europ_PENTAtonic);
    select_in(JIFFLES, up_THRD);
    musicBoxConf.sync = 0;
    musicBoxConf.pitch = {32768, 230};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	2h 30' 26"
    musicBoxConf.subcycle_octave = -5;	// 4' 42"	| subcycle |
    break;

  case 71:
    musicBoxConf.name = F("elfechinde");
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
    musicBoxConf.date = F("2018-12-04_10h16m55s	TUE");
    select_in(SCALES, tetraCHORD);
    select_in(JIFFLES, dwn_THRD);
    musicBoxConf.sync = 3;
    musicBoxConf.pitch = {32768, 410};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	5h 37' 35"
    musicBoxConf.subcycle_octave = -6;	// 5' 16"	| subcycle |
    break;

  case 72:
    musicBoxConf.name = F("elfechinde 2");
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
    musicBoxConf.date = F("2018-12-04_10h24m24s	TUE");
    select_in(SCALES, tetraCHORD);
    select_in(JIFFLES, pent_top_wave);
    musicBoxConf.sync = 3;
    musicBoxConf.pitch = {32768, 375};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	6h  9'  5"
    musicBoxConf.subcycle_octave = -6;	// 5' 46"	| subcycle |
    break;

  case 73:
    musicBoxConf.name = F("simplistic");
    musicBoxConf.date = F("2018-12-05_19h18m09s	WED");
    select_in(SCALES, octaves_fifths);
    select_in(JIFFLES, din_dididi);
    musicBoxConf.sync = 5;
    musicBoxConf.pitch = {32768, 256};
    // fix:	play_KB_macro  /2
    // CYC: 2^0	2d 0h  3' 28"
    musicBoxConf.subcycle_octave = -9;	// 5' 38"	| subcycle |
    break;

  case 74:
    musicBoxConf.name = F("rhythms");
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
    //# beep
    //# robot
    //# rumble
    //# hasSlowPulse
    //comment: very good example for note-metrum interval analogy :)
    //comment: will be a good test case for a better end algorithm ;)
    musicBoxConf.date = F("2018-12-06_08h14m01s	THU");
    select_in(SCALES, octaves_fourths);
    select_in(JIFFLES, tumtum);
    musicBoxConf.sync = 0;
    musicBoxConf.pitch = {32768, 413};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	14d 21h 27' 58"
    musicBoxConf.subcycle_octave = -1;	// 2	    5' 14"	| subcycle |
    break;

  case 75:
    musicBoxConf.name = F("ruhig schwebend");
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
    musicBoxConf.date = F("2018-12-07_09h44m49s	FRI");
    select_in(SCALES, pentatonic_minor);
    select_in(JIFFLES, PENTA_3rd_down_5);
    musicBoxConf.sync = 3;
    musicBoxConf.pitch = {32768, 176};
    // fix:	play_KB_macro  *4
    // CYC: 2^0	4h  5' 45"
    musicBoxConf.subcycle_octave = -5;	// 7' 41"	| subcycle |
    break;

  case 76:
    musicBoxConf.name = F("magical tree");
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
    musicBoxConf.date = F("2018-12-07_13h03m35s	FRI");
    select_in(SCALES, europ_PENTAtonic);
    select_in(JIFFLES, dwn_THRD);
    musicBoxConf.sync = 1;
    musicBoxConf.pitch = {32768, 357};
    // CYC: 2^0	   48' 28"
    musicBoxConf.subcycle_octave = -3;	// 6'  3"	| subcycle |
    break;

  case 77:
    musicBoxConf.name = F("tanboura");
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
    musicBoxConf.date = F("2018-12-07_13h53m29s	FRI");
    select_in(SCALES, doric_scale);
    select_in(JIFFLES, tanboura);
    musicBoxConf.sync = 5;
    musicBoxConf.pitch = {32768, 299};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	2h 24' 39"
    musicBoxConf.subcycle_octave = -4;	// 9'  2"	| subcycle |
    break;

  case 78:
    musicBoxConf.name = F("melodischs Netz uf I IV V");
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
    musicBoxConf.date = F("2018-12-07_16h22m20s	FRI");
    select_in(SCALES, octave_4th_5th);
    select_in(JIFFLES, pent_top_wave);
    musicBoxConf.sync = 3;
    musicBoxConf.pitch = {32768, 257};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	8h 58' 33"
    musicBoxConf.subcycle_octave = -6;	// 8' 25"	| subcycle |
    break;

  case 79:
    musicBoxConf.name = F("Aurora");
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
    musicBoxConf.date = F("2018-12-07_15h57m55s	FRI");
    select_in(SCALES, minor_scale);
    select_in(JIFFLES, PENTA_3rd_down_5);
    musicBoxConf.sync = 3;
    musicBoxConf.pitch = {32768, 300};
    // CYC: 2^0	1h 12'  5"
    musicBoxConf.subcycle_octave = -3;	// 9'  1"	| subcycle |
    break;

  case 80:
    musicBoxConf.name = F("Aurora mit tänzelnde Liechter");
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
    musicBoxConf.date = F("2018-12-07_16h32m24s	FRI");
    select_in(SCALES, pentatonic_minor);
    select_in(JIFFLES, PENTA_3rd_down_5);
    musicBoxConf.sync = 1;
    musicBoxConf.pitch = {32768, 195};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	1h 50' 54"
    musicBoxConf.subcycle_octave = -4;	// 6' 56"	| subcycle |
    break;

  case 81:
    musicBoxConf.name = F("Melodie Kabinet");
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
    musicBoxConf.date = F("2018-12-07_17h23m46s	FRI");
    select_in(SCALES, europ_PENTAtonic);
    select_in(JIFFLES, PENTA_3rd_rise);
    musicBoxConf.sync = 3;
    musicBoxConf.pitch = {32768, 355};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	1h 37' 28"
    musicBoxConf.subcycle_octave = -4;	// 6'  6"	| subcycle |
    break;

  case 82:
    musicBoxConf.name = F("Akkordeon");
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
    musicBoxConf.date = F("2018-12-07_17h34m18s	FRI");
    select_in(SCALES, tetraCHORD);
    select_in(JIFFLES, PENTAtonic_desc);
    musicBoxConf.sync = 5;
    musicBoxConf.pitch = {32768, 260};
    // fix:	play_KB_macro  /2
    // CYC: 2^0	1h  6' 32"
    musicBoxConf.subcycle_octave = -3;	// 8' 19"	| subcycle |
    break;

  case 83:
    musicBoxConf.name = F("MusikdoseTraum");
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
    musicBoxConf.date = F("2018-12-07_17h45m57s	FRI");
    select_in(SCALES, tetraCHORD);
    select_in(JIFFLES, din__dididi_dixi);
    musicBoxConf.sync = 3;
    musicBoxConf.pitch = {32768, 195};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	5h 54' 53"
    musicBoxConf.subcycle_octave = -5;	// 11'  5"	| subcycle |
    break;

  case 84:
    musicBoxConf.name = F("The Enchanted Forest Temple");
    //# FAVORITE
    //# shortIsOk
    //# ritch
    //# fractal
    //# soundscape
    //# slow	relatively low tuned melody net
    //# floating
    //# motives
    //# melodies
    //# harmonies	nice long bass chords
    //# bourdon		often
    //# sound
    //# aesthetic
    //# nice		a bit advanced
    //# static		a bit
    //# mystic
    //# cosmic
    //# fairies
    //# aurora
    //# lyric
    //# dream
    //# dance		very quietly moving with fast detail patterns
    musicBoxConf.date = F("2019-04-04");
    select_in(SCALES, octave_4th_5th);
    select_in(JIFFLES, penta_3rd_down_5);
    musicBoxConf.sync = 1;
    musicBoxConf.pitch = {1, 330};
    break;

  case 85:
    musicBoxConf.name = F("Pentaweb");
    //comment: stimmigsvoll
    //comment: mit sehnsucht und chraft ufs ziel zue
    //# FAVORITE
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
    musicBoxConf.sync = 2;
    select_in(SCALES, pentatonic_minor);
    select_in(JIFFLES, PENTAtonic_desc);
    musicBoxConf.pitch = {1, 128};
    // harmonical cycle 1h  9' 59"	subcycle    8' 45"
    break;

  case 86:
    musicBoxConf.name = F("Schwester Pentaweb");
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
    musicBoxConf.date = F("2018-12-07_19h04m44s	FRI");
    select_in(SCALES, europ_PENTAtonic);
    select_in(JIFFLES, penta_3rd_rise);
    musicBoxConf.sync = 1;
    musicBoxConf.pitch = {32768, 423};
    // CYC: 2^0	1h 21' 48"
    musicBoxConf.subcycle_octave = -3;	// 10' 13"	| subcycle |
    break;

  case 87:
    musicBoxConf.name = F("PentaMusicBox");
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
    musicBoxConf.date = F("2018-12-07_19h16m36s	FRI");
    select_in(SCALES, pentatonic_minor);
    select_in(JIFFLES, din__dididi_dixi);
    musicBoxConf.sync = 1;
    musicBoxConf.pitch = {32768, 187};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	1h 55' 39"
    musicBoxConf.subcycle_octave = -4;	// 7' 14"	| subcycle |
    break;

  case 88:
    musicBoxConf.name = F("I_IV_V mit Knatterrhytheme");
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
    musicBoxConf.date = F("2018-12-07_19h48m57s	FRI");
    select_in(SCALES, octave_4th_5th);
    select_in(JIFFLES, d4096_512);
    musicBoxConf.sync = 1;
    musicBoxConf.pitch = {32768, 187};
    // CYC: 2^0	6h 10'  4"
    //	musicBoxConf.subcycle_octave = -5;	// 11' 34"	| subcycle |
    break;

  case 89:
    musicBoxConf.name = F("The Landing Of The Sound Space Ship");
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
    musicBoxConf.date = F("2018-12-07_20h03m19s	FRI");
    select_in(SCALES, doric_scale);
    select_in(JIFFLES, dwn_THRD_up);
    musicBoxConf.sync = 1;
    musicBoxConf.pitch = {32768, 189};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	1h 54' 25"
    musicBoxConf.subcycle_octave = -4;	// 7'  9"	| subcycle |
    break;

  case 90:
    musicBoxConf.name = F("Striicherbordun");
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
    musicBoxConf.date = F("2018-12-09_20h14m56s	SUN");
    select_in(SCALES, major_scale);
    select_in(JIFFLES, penta_3rd_rise);
    musicBoxConf.sync = 3;
    musicBoxConf.pitch = {32768, 170};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	   25' 27"
    musicBoxConf.subcycle_octave = -2;	// 6' 22"	| subcycle |
    break;

  case 91:
    //# FAVORITE
    // TODO: add tags
    musicBoxConf.name = F("simple and beautiful");
    musicBoxConf.date = F("2018-12-10_17h02m05s	MON");
    select_in(SCALES, doric_scale);
    select_in(JIFFLES, d4096_1024);
    musicBoxConf.sync = 2;
    musicBoxConf.pitch = {32768, 297};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	2h 25' 38"
    musicBoxConf.subcycle_octave = -5;	// 4' 33"	| subcycle |
    break;

  case 92:
    musicBoxConf.name = F("de Reige vo de Dezämberfee");
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
    musicBoxConf.date = F("2018-12-10_17h27m01s	MON");
    select_in(SCALES, pentatonic_minor);
    select_in(JIFFLES, dwn_THRD_up);
    musicBoxConf.sync = 3;
    musicBoxConf.pitch = {32768, 244};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	2h 57' 16"
    musicBoxConf.subcycle_octave = -5;	// 5' 32"	| subcycle |
    break;

  case 93:
    musicBoxConf.name = F("Saiteresonanze");
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
    musicBoxConf.date = F("2018-12-11_13h29m08s	TUE");
    select_in(SCALES, europ_PENTAtonic);
    select_in(JIFFLES, pentatonic_rise);
    musicBoxConf.sync = 5;
    musicBoxConf.pitch = {32768, 217};
    // CYC: 2^0	1h 19' 44"
    musicBoxConf.subcycle_octave = -4;	// 4' 59"	| subcycle |
    break;

  case 94:
    musicBoxConf.name = F("Chindertanz -- Children's Dance");
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
    //# beep
    //# robot
    //# lesley
    //# delay
    //# rumble
    //# hasSlowPulse	simle motiv ostinatos
    //# lyric
    //# children
    //# dance
    musicBoxConf.date = F("2018-12-13_10h24m06s	THU");
    select_in(SCALES, tetraCHORD);
    select_in(JIFFLES, rising_pent_them);
    musicBoxConf.sync = 4;
    musicBoxConf.pitch = {32768, 302};
    // CYC: 2^0	57' 17"
    musicBoxConf.subcycle_octave = -4;	// 3' 35"	| subcycle |
    break;

  case 95:
    musicBoxConf.name = F("Good Old Major Sync3");
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
    musicBoxConf.date = F("2018-12-13_10h47m46s	THU");
    select_in(SCALES, major_scale);
    select_in(JIFFLES, d4096_1024);
    musicBoxConf.sync = 3;
    // harmonical CYCLE: 13' 11"	SUBCYCLE: | 6' 36" |
    musicBoxConf.subcycle_octave = -1;
    break;

  case 96:
    musicBoxConf.name = F("Cosmic SoundWave Visiting Earth");
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
    musicBoxConf.date = F("2018-12-13_16h59m40s	THU");
    select_in(SCALES, major_scale);
    select_in(JIFFLES, PENTA_3rd_down_5);
    musicBoxConf.sync = 1;
    musicBoxConf.pitch = {32768, 208};
    // harmonical CYCLE: 20' 48"	SUBCYCLE: | 5' 12" |
    // SUBCYCLE: (2^-2) 5' 12"
    break;

  case 97:
    musicBoxConf.name = F("Jazzigi Rhythme mit schnarrend schnarchende KnatterKläng");	// really???
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
    musicBoxConf.date = F("2018-12-13_19h42m01s	THU");
    select_in(SCALES, octaves_fifths);
    select_in(JIFFLES, diing_ditditdit);
    musicBoxConf.sync = 2;
    musicBoxConf.pitch = {32768, 449};
    // harmonical CYCLE: 4d 13h 36' 6"		SUBCYCLE: | 6' 25" |
    musicBoxConf.subcycle_octave = -10;
    break;

  case 98:
    musicBoxConf.name = F("Harmonikale Schrittanz");
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
    musicBoxConf.date = F("2018-12-13_21h35m38s	THU");
    select_in(SCALES, minor_scale);
    select_in(JIFFLES, pentatonic_rise);
    musicBoxConf.sync = 0;
    musicBoxConf.pitch = {32768, 234};
    // fix:	play_KB_macro  *2
    // harmonical CYCLE: 3h 4' 50"	SUBCYCLE: | 5' 47" |
    musicBoxConf.subcycle_octave = -5;
    break;

  case 99:
    musicBoxConf.name = F("Church Bells, blue jay way");
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
    musicBoxConf.date = F("2018-12-14_18h24m10s	FRI");
    select_in(SCALES, pentatonic_minor);
    select_in(JIFFLES, pent_patternA);
    musicBoxConf.sync = 5;
    musicBoxConf.pitch = {32768, 431};
    // harmonical CYCLE: 1h 40' 21"	SUBCYCLE: | 6' 16" |
    musicBoxConf.subcycle_octave = -4;
    break;

  case 100:
    musicBoxConf.name = F("PENTAtonic Clock Organ");
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
    //# beep
    //# robot
    //# hasSlowPulse	archaic
    //# children
    //# dance
    musicBoxConf.date = F("2018-12-14_18h51m01s	FRI");
    select_in(SCALES, europ_PENTAtonic);
    select_in(JIFFLES, diing_ditditdit);
    musicBoxConf.sync = 3;
    musicBoxConf.pitch = {32768, 429};
    // fix:	play_KB_macro  *2
    // harmonical CYCLE: 2h 41' 19"	SUBCYCLE: | 5' 2" |
    break;

  case 101:
    musicBoxConf.name = F("Good Night Choir (mit Knatterrhytme)");
    //# favorite+
    //# shortIsOk
    //# motives
    //# melodies
    //# harmonies
    //# rhytm&pulse    // mit KNATTERRHYTHME
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
    musicBoxConf.date = F("2018-12-14_19h03m53s	FRI");
    select_in(SCALES, octaves_fourths);
    select_in(JIFFLES, pentatonic_desc);
    musicBoxConf.sync = 0;
    musicBoxConf.pitch = {32768, 352};
    // fix:	play_KB_macro  *2
    // harmonical CYCLE: 17d 11h 24' 48"	SUBCYCLE: | 6' 9" |
    musicBoxConf.subcycle_octave = -12;
    break;

  case 102:
    musicBoxConf.name = F("Classical Beauty");
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
    musicBoxConf.date = F("2018-12-14_19h37m48s	FRI");
    select_in(SCALES, doric_scale);
    select_in(JIFFLES, d4096_1024);
    musicBoxConf.sync = 1;
    musicBoxConf.pitch = {32768, 215};
    // fix:	play_KB_macro  *2
    // harmonical CYCLE: 3h 21' 10"	SUBCYCLE: | 6' 17" |
    musicBoxConf.subcycle_octave = -5;
    break;

  case 103:
    musicBoxConf.name = F("PENTA Classic");
    //# FAVORITE
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
    musicBoxConf.date = F("2018-12-14_19h48m35s	FRI");
    select_in(SCALES, europ_PENTAtonic);
    select_in(JIFFLES, up_THRD);
    musicBoxConf.sync = 2;
    musicBoxConf.pitch = {32768, 333};
    // fix:	play_KB_macro  *2
    // harmonical CYCLE: 1h 43' 55"	SUBCYCLE: | 6' 30" |
    musicBoxConf.subcycle_octave = -4;
    break;

  case 104:
    musicBoxConf.name = F("pentatonic reaching down to rhythm");
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
    musicBoxConf.date = F("2018-12-15_12h58m14s	SAT");
    select_in(SCALES, pentatonic_minor);
    select_in(JIFFLES, tumtum);
    musicBoxConf.sync = 2;
    musicBoxConf.pitch = {32768, 222};
    // harmonical CYCLE: 1h 37' 25"	SUBCYCLE: | 6' 5" |
    musicBoxConf.subcycle_octave = -4;
    break;

  case 105:
    musicBoxConf.name = F("Nameless Beauty");
    //# favorite+
    //# shortIsOk
    //# fullCycleIsOk	harmonical CYCLE: 55' 27"
    //# melodies
    //# harmonies
    //# structure	melody whirls
    //# classicHarmonical
    //# aesthetic
    //# nice
    //# simple		somehow
    //# cosmic
    //# SoundSpaceShip
    //# fractal
    //# soundscape	melody whirls scape
    //# hasSlowPulse	whirl structure
    //# lyric
    //# water		positive
    //# children	possible
    //# dance		whirdance?
    //# floating	in a whirlpool?
    musicBoxConf.date = F("2018-12-15_15h06m14s	SAT");
    select_in(SCALES, minor_scale);
    select_in(JIFFLES, pentatonic_rise);
    musicBoxConf.sync = 0;
    musicBoxConf.pitch = {32768, 223};
    // harmonical CYCLE: 1h 36' 59"	SUBCYCLE: | 6' 4" |
    musicBoxConf.subcycle_octave = -4;
    break;

  case 106:
    musicBoxConf.name = F("melodischs gfüeg");
    //# favorite+
    //# shortIsOk
    //# fullCycleIsOk	harmonical CYCLE: 55' 27"
    //# melodies	melody whirls
    //# structure	whirls
    //# classicHarmonical	whirly stile
    //# aesthetic	advanced
    //# nice		advanced
    //# happy
    //# fractal
    //# lesley
    //# hasSlowPulse	whirl structure
    //# lyric
    //# water
    //# children
    //# dance
    //# festive
    //# floating	in the dance
    musicBoxConf.date = F("2018-12-18_11h56m09s	TUE");
    select_in(SCALES, tetraCHORD);
    select_in(JIFFLES, rising_pent_them);
    musicBoxConf.sync = 1;
    musicBoxConf.pitch = {32768, 420};
    // fix:	play_KB_macro  *2
    // harmonical CYCLE: 5h 29' 32"	SUBCYCLE: | 5' 9" |
    musicBoxConf.subcycle_octave = -6;
    break;

  case 107:
    musicBoxConf.name = F("RhythmicalTroubadourBordun");
    // could imagine it in a song :)
    //# FAVORITE
    //# shortIsOk
    //# fullCycleIsOk	harmonical CYCLE: 55' 27"
    //# motives
    //# melodies	some very beautiful
    //# harmonies
    //# bourdon		the melody whirls are like a very slow bourdon, somehow
    //# structure	melody pattern
    //# classicHarmonical advanced
    //# aesthetic	advanced
    //# nice		advanced
    //# simple		evolving structure
    //# cosmic
    //# birds
    //# fairies
    //# lesley
    //# hasSlowPulse	follows evolving structure
    //# lyric
    //# dance		steady advancing
    //# festive		advanced
    musicBoxConf.date = F("2018-12-19_14h13m01s	WED");
    select_in(SCALES, octave_4th_5th);
    select_in(JIFFLES, tumtum);
    musicBoxConf.sync = 0;
    musicBoxConf.pitch = {32768, 413};
    // fix:	play_KB_macro  *2
    // harmonical CYCLE: 11h 10' 15"	SUBCYCLE: | 5' 14" |
    musicBoxConf.subcycle_octave = -7;
    break;

  case 108:
    musicBoxConf.name = F("another melody net");
    //# favorite
    //# shortIsOk
    //# fullCycleIsOk	harmonical CYCLE: 55' 27"
    //# motives
    //# melodies	melody whirls
    //# harmonies
    //# structure	melody whirls with harmonies
    //# classicHarmonical	intermediate
    //# aesthetic	intermediate
    //# happy
    //# fractal
    //# fairies
    //# aurora
    //# hasSlowPulse	whirl starting bass notes
    //# lyric
    //# water		sure, what not?
    //# children
    //# dance
    musicBoxConf.date = F("2018-12-19_15h29m35s	WED");
    select_in(SCALES, tetraCHORD);
    select_in(JIFFLES, pentatonic_rise);
    musicBoxConf.sync = 5;
    musicBoxConf.pitch = {32768, 323};
    // fix:	play_KB_macro  /2
    // harmonical CYCLE: 1h 47' 8"	SUBCYCLE: | 6' 42" |
    musicBoxConf.subcycle_octave = -4;
    break;

  case 109:
    musicBoxConf.name = F("eifache schöne reige");
    //# favorite+
    //# shortIsOk
    //# fullCycleIsOk	harmonical CYCLE: 55' 27"
    //# melodies
    //# harmonies
    //# rhytm&pulse	whirl starts
    //# structure	whirls
    //# classicHarmonical	advanced
    //# aesthetic	intermediate
    //# nice	intermediate
    //# happy	beautiful
    //# quiet
    //# simple
    //# hasSlowPulse	whirl structure
    //# lyric
    //# dance
    //# festive
    musicBoxConf.date = F("2018-12-19_15h49m12s	WED");
    select_in(SCALES, octave_4th_5th);
    select_in(JIFFLES, dwn_THRD_up);
    musicBoxConf.sync = 4;
    musicBoxConf.pitch = {32768, 354};
    // fix:	play_KB_macro  *2
    // harmonical CYCLE: 13h 1' 57"	SUBCYCLE: | 6' 7" |
    musicBoxConf.subcycle_octave = -7;
    break;

  case 110:
    musicBoxConf.name = F("another nice simple one");	// hübsch
    // e bizz wie n es cembalo :)
    //# favorite
    //# shortIsOk
    //# fullCycleIsOk	harmonical CYCLE: 1h 0' 14"
    //# melodies	harmonic progression
    //# harmonies
    //# structure	harmonic progression
    //# classicHarmonical
    //# aesthetic
    //# nice
    //# cosmic
    //# hasSlowPulse	harmonic progression
    //# lyric
    //# festive
    musicBoxConf.date = F("2018-12-19_15h42m05s	WED");
    select_in(SCALES, minor_scale);
    select_in(JIFFLES, d4096_512);
    musicBoxConf.sync = 5;
    musicBoxConf.pitch = {32768, 228};
    // fix:	play_KB_macro  /2
    // harmonical CYCLE: 47' 26"	SUBCYCLE: | 5' 56" |
    musicBoxConf.subcycle_octave = -3;
    break;

  case 111:
    musicBoxConf.name = F("descending ascending whirles");
    //# favorite+
    //# shortIsOk
    //# fullCycleIsOk	harmonical CYCLE: 57' 59"
    //# motives
    //# melodies	in whirls
    //# harmonies	whirls
    //# whirles		everywhere
    //# structure	follows whirls
    //# classicHarmonical	with whirls
    //# aesthetic	whirls
    //# nice
    //# happy
    //# dramatic	at least steady advancing, decided
    //# fractal
    //# hasSlowPulse	whirl structure
    //# lyric
    //# dance
    //# festive
    musicBoxConf.date = F("2018-12-19_15h57m43s	WED");
    select_in(SCALES, doric_scale);
    select_in(JIFFLES, pentatonic_rise);
    musicBoxConf.sync = 5;
    musicBoxConf.pitch = {32768, 373};
    // fix:	play_KB_macro  *2
    // harmonical CYCLE: 1h 55' 57"	SUBCYCLE: | 3' 37" |
    musicBoxConf.subcycle_octave = -5;
    break;

  case 112:
    musicBoxConf.name = F("im Zaubergarte");
    //# favorite
    //# shortIsOk	short cycled anyway ;)
    //# fullCycleIsOk	harmonical CYCLE: 10' 41"
    //# motives
    //# melodies
    //# harmonies
    //# bourdon		deep harmonies
    //# whirles
    //# structure	whirlnet
    //# systemStress	some
    //# heavyStart	???
    //# aesthetic	enchanted
    //# nice
    //# dense		it *is*, but still nice
    //# mystic
    //# cosmic
    //# SoundSpaceShip
    //# fractal
    //# soundscape
    //# fairies
    //# aurora
    //# lesley
    //# humming		deep slow harmonies
    //# hasSlowPulse	advanced progressing
    //# dance		very slow witch dance
    //# festive
    //# floating
    musicBoxConf.date = F("2018-12-20_13h11m49s	THU");
    select_in(SCALES, major_scale);
    select_in(JIFFLES, penta_3rd_down_5);
    musicBoxConf.sync = 1;
    musicBoxConf.pitch = {32768, 262};
    // fix:	play_KB_macro  /2
    // harmonical CYCLE: 8' 15"	SUBCYCLE: | 4' 8" |
    musicBoxConf.subcycle_octave = -1;
    break;

  case 113:
    musicBoxConf.name = F("Chläderrose");
    //# FAVORITE
    //# shortIsOk
    //# motives
    //# melodies
    //# harmonies
    //# bourdon
    //# whirles
    //# classicHarmonical	but very strange
    //# aesthetic		expert
    //# strange			it *is* strange
    //# funny		even funny, comic
    //# dense
    //# mystic
    //# fractal
    //# fairies
    //# lyric
    //# dance
    //# floating
    musicBoxConf.date = F("2018-12-19_20h43m46s	WED");
    select_in(SCALES, minor_scale);
    select_in(JIFFLES, up_THRD);
    musicBoxConf.sync = 3;
    musicBoxConf.pitch = {32768, 400};
    // fix:	play_KB_macro  *2
    // harmonical CYCLE: 1h 48' 8"	SUBCYCLE: | 6' 45" |
    musicBoxConf.subcycle_octave = -4;
    break;

  case 114:
    musicBoxConf.name = F("dream angel came from heaven to earth");
    //# FAVORITE
    //# shortIsOk
    //# fullCycleIsOk	armonical CYCLE: 1h 1' 56"
    //# motives
    //# melodies
    //# harmonies
    //# whirles
    //# structure	whirls of light
    //# systemStress	maybe a little bit
    //# classicHarmonical	intermediate
    //# aesthetic	advanced
    //# nice	advanced
    //# happy
    //# melancholic
    //# quiet
    //# strange		but very natural, like instruments
    //# simple
    //# mystic
    //# cosmic
    //# fractal
    //# fairies
    //# aurora
    //# lesley
    //# hasSlowPulse	slowly pulsating whirl structure
    //# lyric
    //# dream
    //# angels
    //# heaven
    //# earth
    //# air
    //# dance		very mystical
    //# festive
    musicBoxConf.date = F("2018-12-19_22h08m50s	WED");
    select_in(SCALES, pentatonic_minor);
    select_in(JIFFLES, penta_3rd_down_5);
    musicBoxConf.sync = 1;
    musicBoxConf.pitch = {32768, 226};
    // harmonical CYCLE: 1h 35' 41"	SUBCYCLE: | 5' 59" |
    musicBoxConf.subcycle_octave = -4;
    break;

  case 115:
    musicBoxConf.name = F("crazy, roboterhafts piepse");
    //# bigBang
    //# favorite-
    //# shortIsOk
    //# melodies
    //# harmonies
    //# crazy
    //# classicHarmonical
    //# nervous
    //# beep
    //# alarm
    //# robot
    //# noise
    //# dropouts
    //comment: alarmgepiepse
    //comment: langsams, maschinells schriite
    //comment: ständig umpiepst vo de nervende alärm ;)
    musicBoxConf.date = F("2018-11-08_15h07m35s	THU");
    select_in(SCALES, pentatonic_minor);
    select_in(JIFFLES, din__dididi_dixi);
    musicBoxConf.sync = 0;
    musicBoxConf.pitch = {32768, 386};
    // fix:	play_KB_macro  /2
    // CYC: 2^0	   28'  1"
    break;

  case 116:
    musicBoxConf.name = F("relax and get ready");
    //# favorite
    //# shortIsOk
    //# motives
    //# melodies
    //# harmonies
    //# whirles
    //# heavyStart	*churze* Störfall
    //# classicHarmonical	with whirls
    //# aesthetic
    //# quiet
    //# strange
    //# mystic
    //# fractal
    //# bigBang
    //# birds
    //# churchbells
    //# fairies
    //# aurora
    //# lesley
    //# hasSlowPulse	whirles
    //# earth
    //# children
    //# lyric
    musicBoxConf.date = F("2018-12-20_16h05m35s	THU");
    select_in(SCALES, tetraCHORD);
    select_in(JIFFLES, pentatonic_desc);
    musicBoxConf.sync = 0;
    musicBoxConf.pitch = {32768, 411};
    // fix:	play_KB_macro  *2
    // harmonical CYCLE: 5h 36' 45"	SUBCYCLE: | 5' 16" |
    musicBoxConf.subcycle_octave = -6;
    break;

  case 117:
    musicBoxConf.name = F("birds and other musicians calling");
    //# FAVORITE
    //# shortIsOk
    //# motives
    //# melodies
    //# harmonies
    //# rhytm&pulse
    //# whirles
    //# structure	net with space in between
    //# classicHarmonical	advanced
    //# aesthetic
    //# nice	intermediate
    //# happy
    //# melancholic
    //# quiet
    //# simple
    //# mystic
    //# cosmic
    //# fractal
    //# birds
    //# churchbells
    //# fairies
    //# aurora
    //# lesley
    //# hasSlowPulse	air in the net
    //# lyric
    //# dream
    //# angels
    //# heaven
    //# air
    //# children
    //# dance	slow movements
    //# festive and quiet, very concentrated
    musicBoxConf.date = F("2018-12-20_16h54m10s	THU");
    select_in(SCALES, octaves_fifths);
    select_in(JIFFLES, pentatonic_desc);
    musicBoxConf.sync = 4;
    musicBoxConf.pitch = {32768, 271};
    // harmonical CYCLE: 3d 18h 47' 44"	SUBCYCLE: | 5' 19" |
    musicBoxConf.subcycle_octave = -10;
    break;

  case 118:
    musicBoxConf.name = F("triad tanboura");
    //comment: nice for a while
    //comment: can get boring ;)
    //# favorite-
    //# shortIsOk
    //# motives
    //# melodies	simplicistic
    //# harmonies	bourdon
    //# rhytm&pulse	simplicistic
    //# bourdon
    //# structure	simplicistic
    //# classicHarmonical	simplicistic
    //# aesthetic	simplicistic
    //# static
    //# simple
    //# simlicistic
    //# elementary
    //# clock
    //# humming
    //# rumble
    //# hasSlowPulse	simplicistic
    //# boring
    musicBoxConf.date = F("2018-12-21_11h54m41s	FRI");
    select_in(SCALES, triad);
    select_in(JIFFLES, tanboura);
    musicBoxConf.sync = 3;
    musicBoxConf.pitch = {32768, 248};
    // fix:	play_KB_macro  /2
    // harmonical CYCLE: 3h 52' 32"	SUBCYCLE: | 3' 38" |
    musicBoxConf.subcycle_octave = -6;
    break;

  case 119:
    musicBoxConf.name = F("slow dreams");
    //# FAVORITE
    //# shortIsOk
    //# motives
    //# melodies
    //# harmonies
    //# structure	slowly progressing
    //# aesthetic
    //# nice		but strange, blue
    //# quiet
    //# strange
    //# mystic
    //# cosmic
    //# SoundSpaceShip
    //# fractal
    //# soundscape
    //# churchbells
    //# fairies
    //# aurora
    //# jazz
    //# lesley
    //# humming
    //# hasSlowPulse	slowly oscillating
    //# lyric
    //# dream
    //# angels
    //# heaven
    //# air
    //# children
    //# dance
    //# festive
    //# floating
    //# slow
    //# steady
    musicBoxConf.date = F("2018-12-21_18h55m55s	FRI");
    select_in(SCALES, europ_PENTAtonic);
    select_in(JIFFLES, dwn_THRD);
    musicBoxConf.sync = 2;
    musicBoxConf.pitch = {32768, 300};
    // fix:	play_KB_macro  *2
    // harmonical CYCLE: 1h 55' 20"	SUBCYCLE: | 3' 36" |
    musicBoxConf.subcycle_octave = -5;
    break;

  case 120:
    musicBoxConf.name = F("schlichte Tanz");
    //# favorite
    //# shortIsOk
    //# fullCycleIsOk	harmonical CYCLE: 1h 3' 1"
    //# melodies	harmonic progression
    //# harmonies
    //# structure	steady harmonic progression
    //# classicHarmonical
    //# aesthetic
    //# nice
    //# quiet
    //# simple
    //# mystic
    //# cosmic
    //# fractal
    //# fairies
    //# hasSlowPulse	steady harmonic progression
    //# lyric
    //# dream
    //# angels
    //# heaven
    //# earth
    //# festive
    //# slow
    //# steady
    //# happy
    //# tired
    musicBoxConf.date = F("2018-12-21_19h57m09s	FRI");
    select_in(SCALES, pentaCHORD);
    select_in(JIFFLES, din_dididi);
    musicBoxConf.sync = 2;
    musicBoxConf.pitch = {32768, 416};
    // fix:	play_KB_macro  *2
    // harmonical CYCLE: 2h 46' 21"	SUBCYCLE: | 5' 12" |
    musicBoxConf.subcycle_octave = -5;
    break;

  case 121:
    musicBoxConf.name = F("schlicht ruhends muster, friedlich, erwartigsvoll, lebensfroh");
    //# favorite+
    //# shortIsOk
    //# motives
    //# melodies
    //# harmonies
    //# structure	simple progression, with air
    //# classicHarmonical
    //# aesthetic
    //# nice
    //# quiet
    //# simple
    //# mystic
    //# cosmic
    //# fractal
    //# hasSlowPulse	very quietly progressing
    //# lyric
    //# dream
    //# air
    //# festive
    //# slow
    //# steady
    musicBoxConf.date = F("2018-12-22_09h52m20s	SAT");
    select_in(SCALES, octaves_fourths);
    select_in(JIFFLES, PENTAtonic_desc);
    musicBoxConf.sync = 2;
    musicBoxConf.pitch = {32768, 350};
    // fix:	play_KB_macro  *2
    // harmonical CYCLE: 17d 13h 48' 36"	SUBCYCLE: | 6' 11" |
    musicBoxConf.subcycle_octave = -12;
    break;

  case 122:
    musicBoxConf.name = F("und es ratteret und brummet tickend de motor");	// sehr starche puls!
    //# favorite+
    //# favorite
    //# shortIsOk
    //# harmonies	simlicistic
    //# rhytm&pulse
    //# beat
    //# bourdon
    //# structure	simlicistic
    //# static
    //# dramatic
    //# simple
    //# simlicistic
    //# elementary
    //# bigBang
    //# clock
    //# humming
    //# rumble
    //# hasSlowPulse	beat
    //# dance	hypnotic
    //# boring
    //# steady
    musicBoxConf.date = F("2018-12-22_10h34m45s	SAT");
    select_in(SCALES, octaves_fourths);
    select_in(JIFFLES, d4096_3072);
    musicBoxConf.sync = 0;
    musicBoxConf.pitch = {32768, 188};
    // fix:	play_KB_macro  *2
    // harmonical CYCLE: 16d 8h 38' 32"	SUBCYCLE: | 5' 45" |
    musicBoxConf.subcycle_octave = -12;
    break;

  case 123:
    musicBoxConf.name = F("pentatonische wasserfall");
    //# favorite+
    //# shortIsOk
    //# fullCycleIsOk	harmonical CYCLE: 47' 48"
    //# motives
    //# melodies
    //# harmonies
    //# structure	pentatonic waterfall
    //# classicHarmonical	advanced
    //# aesthetic	beautiful, but advanced
    //# nice		advanced
    //# quiet
    //# cosmic
    //# fractal
    //# birds
    //# fairies
    //# aurora
    //# lesley
    //# lyric
    //# dream
    //# water
    //# dance	always down
    //# floating
    //# slow
    //# steady	in waves
    musicBoxConf.date = F("2018-12-22_15h51m27s	SAT");
    select_in(SCALES, pentaCHORD);
    select_in(JIFFLES, pentatonic_desc);
    musicBoxConf.sync = 5;
    musicBoxConf.pitch = {32768, 181};
    // fix:	play_KB_macro  *2
    // harmonical CYCLE: 1h 35' 35"	SUBCYCLE: | 5' 58" |
    musicBoxConf.subcycle_octave = -4;
    break;

  case 124:
    musicBoxConf.name = F("brummbärs ratterkiste");
    //# favorite+
    //# shortIsOk
    //# melodies	jazzy harmonic progression
    //# harmonies
    //# rhytm&pulse	jazzy
    //# beat	split
    //# structure	mad jazz piano player
    //# classicHarmonical	jazz version
    //# aesthetic
    //# happy
    //# dramatic
    //# nervous
    //# crazy
    //# funny
    //# clock
    //# jazz
    //# rumble
    //# hasSlowPulse	split
    //# dance	crazy
    //# fast
    //# steady
    //# tired	maybe tired or drunk, who knows?
    musicBoxConf.date = F("2018-12-22_15h58m50s	SAT");
    select_in(SCALES, pentatonic_minor);
    select_in(JIFFLES, tumtum);
    musicBoxConf.sync = 2;
    musicBoxConf.pitch = {32768, 257};
    // fix:	play_KB_macro  *2
    // harmonical CYCLE: 2h 48' 18"	SUBCYCLE: | 5' 16" |
    musicBoxConf.subcycle_octave = -5;
    break;

  case 125:
    musicBoxConf.name = F("a firework of sounds");
    musicBoxConf.date = F("2018-12-22_18h52m33s	SAT");
    select_in(SCALES, major_scale);
    select_in(JIFFLES, PENTAtonic_rise);
    musicBoxConf.sync = 1;
    musicBoxConf.pitch = {32768, 428};
    // fix:	play_KB_macro  /2
    // harmonical CYCLE: 10' 6"	SUBCYCLE: | 5' 3" |
    // | (2^-1) 5' 3" |
    break;

  case 126:
    musicBoxConf.name = F("firework #2");
    musicBoxConf.date = F("2018-12-22_19h03m01s	SAT");
    select_in(SCALES, europ_PENTAtonic);
    select_in(JIFFLES, din_dididi);
    musicBoxConf.sync = 5;
    musicBoxConf.pitch = {32768, 354};
    // fix:	play_KB_macro  /2
    // harmonical CYCLE: 24' 26"	SUBCYCLE: | 6' 7" |
    musicBoxConf.subcycle_octave = -2;
    break;

  case 127:
    musicBoxConf.name = F("acoustic telegraph lines");
    //# FAVORITE
    //# shortIsOk
    //# melodies
    //# harmonies
    //# rhytm&pulse
    //# beat
    //# bourdon
    //# structure	simple
    //# aesthetic	tecnical
    //# static
    //# nervous		no, not really
    //# crazy
    //# funny
    //# simple
    //# simlicistic
    //# fractal
    //# clock
    //# beep
    //# robot
    //# lesley
    //# delay
    //# humming
    //# rumble
    //# hasSlowPulse	steady
    //# lyric
    //# dream
    //# earth
    //# boring	but interesting effects
    //# slow
    //# steady
    //# tired	maybe just a bit
    musicBoxConf.date = F("2018-12-22_20h17m39s	SAT");
    select_in(SCALES, octaves_fourths);
    select_in(JIFFLES, rising_pent_them);
    musicBoxConf.sync = 5;
    musicBoxConf.pitch = {32768, 322};
    // harmonical CYCLE: 9d 13h 14' 40"	SUBCYCLE: | 6' 43" |
    musicBoxConf.subcycle_octave = -11;
    break;

  case 128:
    musicBoxConf.name = F("clear winter sky");
    //# favorite+
    //# shortIsOk
    //# fullCycleIsOk	harmonical CYCLE: 1h 10' 13"
    //# motives
    //# melodies
    //# harmonies
    //# whirles
    //# structure	whirls of melodies witth slow harmonies
    //# classicHarmonical	extended
    //# aesthetic	extended
    //# nice		extended
    //# quiet
    //# static
    //# simple
    //# soundscape
    //# fairies
    //# aurora
    //# lesley
    //# humming
    //# lyric
    //# dream
    //# angels
    //# heaven
    //# air
    //# dance
    //# festive
    //# floating
    //# slow
    musicBoxConf.date = F("2018-12-23_18h45m37s	SUN");
    select_in(SCALES, pentatonic_minor);
    select_in(JIFFLES, PENTAtonic_desc);
    musicBoxConf.sync = 1;
    musicBoxConf.pitch = {32768, 444};
    // harmonical CYCLE: 1h 37' 25"	SUBCYCLE: | 6' 5" |
    musicBoxConf.subcycle_octave = -4;
    break;

  case 129:
    musicBoxConf.name = F("the mad old jazz piano man");
    //# favorite
    //# shortIsOk
    //# motives
    //# harmonies
    //# rhytm&pulse	jazzy, split
    //# aesthetic	jazzy, a bit crazy
    //# dramatic
    //# nervous
    //# crazy
    //# funny
    //# chaotic		might sound like
    //# jazz
    //# robot
    //# delay
    //# rumble
    //# hasSlowPulse	crazy
    //# dream
    //# earth
    //# dance		crazy
    //# fast
    //# steady		stubborn
    musicBoxConf.date = F("2018-12-23_18h53m20s	SUN");
    select_in(SCALES, pentatonic_minor);
    select_in(JIFFLES, tumtumtum);
    musicBoxConf.sync = 4;
    musicBoxConf.pitch = {32768, 398};
    // fix:	play_KB_macro  *2
    // harmonical CYCLE: 1h 48' 40"	SUBCYCLE: | 6' 48" |
    musicBoxConf.subcycle_octave = -4;
    break;

  case 130:
    musicBoxConf.name = F("nameless beauty");
    //# favorite+
    //# shortIsOk
    //# motives
    //# melodies
    //# harmonies
    //# whirles
    //# classicHarmonical	advanced
    //# aesthetic	blue
    //# strange
    //# mystic
    //# cosmic
    //# fractal
    //# churchbells
    //# fairies
    //# aurora
    //# jazz
    //# lesley	deep interferences, nice!
    //# noise	deep interferences, nice!
    //# rumble
    //# hasSlowPulse	patterns
    //# lyric
    //# dream
    //# festive
    //# slow
    musicBoxConf.date = F("2018-12-24_20h35m07s	MON");
    select_in(SCALES, trichord);
    select_in(JIFFLES, pentatonic_rise);
    musicBoxConf.sync = 2;
    musicBoxConf.pitch = {32768, 286};
    // fix:	play_KB_macro  *2
    // harmonical CYCLE: 10h 45' 15"	SUBCYCLE: | 5' 2" |
    musicBoxConf.subcycle_octave = -7;
    break;

  case 131:
    musicBoxConf.name = F("penta trichord");
    //# favorite+
    //# shortIsOk
    //# motives
    //# melodies
    //# harmonies
    //# whirles
    //# aesthetic
    //# nice
    //# happy
    //# melancholic
    //# quiet
    //# strange
    //# simple
    //# mystic
    //# cosmic
    //# fractal
    //# churchbells
    //# fairies
    //# aurora
    //# jazz
    //# lesley
    //# lyric
    //# dream
    //# angels
    //# heaven
    //# air
    //# floating
    //# slow
    //# tired	maybe a bit?
    musicBoxConf.date = F("2018-12-24_20h49m58s	MON");
    select_in(SCALES, trichord);
    select_in(JIFFLES, PENTAtonic_desc);
    musicBoxConf.sync = 2;
    musicBoxConf.pitch = {32768, 288};
    // fix:	play_KB_macro  *2
    // harmonical CYCLE: 10h 40' 46"	SUBCYCLE: | 5' 0" |
    musicBoxConf.subcycle_octave = -7;
    break;

  case 132:
    musicBoxConf.name = F("Santa Claus");
    //# favorite+
    //# shortIsOk
    //# fullCycleIsOk	harmonical CYCLE: 57' 59"
    //# motives
    //# melodies
    //# harmonies
    //# whirles
    //# aesthetic
    //# nice	intermediate
    //# quiet
    //# dense	a bit	loose a bit
    //# mystic
    //# fractal
    //# soundscape
    //# fairies
    //# aurora
    //# hasSlowPulse	whirl structure
    //# lyric
    //# children
    //# festive
    //# slow
    //# steady
    //# happy
    musicBoxConf.date = F("2018-12-25_19h16m25s	TUE");
    select_in(SCALES, octave_4th_5th);
    select_in(JIFFLES, penta_3rd_rise);
    musicBoxConf.sync = 1;
    // fixed random tuning b	233
    musicBoxConf.pitch = {32768, 233};
    // fix:	play_KB_macro  *2
    // harmonical CYCLE: 9h 54' 1"	SUBCYCLE: | 4' 38" |
    musicBoxConf.subcycle_octave = -7;
    break;

  case 133:
    musicBoxConf.name = F("sound waves like ever descending rain");
    //# RECORDED (similar)	name = F("2018-12-25_20h28m58s (recorded)");
    //# FAVORITE
    //# shortIsOk
    //# loose	quite
    //# ritch
    //# fractal
    //# slow
    //# motives
    //# melodies
    //# harmonies
    //# whirles
    //# waves
    //# aesthetic
    //# nice
    //# happy
    //# quiet
    //# mystic
    //# fairies
    //# aurora
    //# lesley
    //# hasSlowPulse	waves
    //# lyric
    //# dream
    //# angels
    //# heaven
    //# water
    //# air
    //# children
    //# dance
    //# festive
    //# happy
    musicBoxConf.date = F("2018-12-25_20h28m58s	TUE");
    select_in(SCALES, doric_scale);
    select_in(JIFFLES, din__dididi_dix);
    musicBoxConf.sync = 2;
    musicBoxConf.pitch = {32768, 32};
    // harmonical CYCLE: 2h 48' 57"	SUBCYCLE: | 5' 17" |
    musicBoxConf.subcycle_octave = -5;
    musicBoxConf.date = F("2019-01-04_18h44m09s	FRI");
    select_in(SCALES, tetraCHORD);
    select_in(JIFFLES, PENTAtonic_desc);
    musicBoxConf.sync = 2;
    musicBoxConf.pitch = {32768, 234};
    // harmonical CYCLE: 2h 27' 52"	SUBCYCLE: | 4' 37" |
    musicBoxConf.subcycle_octave = -5;
    break;

  case 134:
    musicBoxConf.name = F("Childrens Christmas Night");
    //# FAVORITE+
    //# shortIsOk
    //# fullCycleIsOk
    //# ritch
    //# slow
    //# busy
    //# floating
    //# melodies
    //# harmonies
    //# aesthetic
    //# nice
    //# happy
    //# melancholic
    //# quiet
    //# cosmic
    //# hasSlowPulse	simplicistic motives
    //# lyric
    //# dream
    //# angels
    //# heaven
    //# children
    //# festive
    //# solemn
    //# ritual
    //# happy
    //# sad
    //# tired	a bit, maybe
    musicBoxConf.date = F("2018-12-25_19h42m27s	TUE");
    select_in(SCALES, pentaCHORD);
    select_in(JIFFLES, din__dididi_dixi);
    musicBoxConf.sync = 4;
    musicBoxConf.pitch = {32768, 128};
    // fix:	play_KB_macro  *2
    // harmonical CYCLE: 2h 15' 10"	SUBCYCLE: | 4' 13" |
    musicBoxConf.subcycle_octave = -5;
    break;

  case 135:
    musicBoxConf.name = F("daydreams");
    //# favorite
    //# shortIsOk
    //# fullCycleIsOk	armonical CYCLE: 1h 11' 51"
    //# ritch
    //# fractal
    //# soundscape
    //# busy
    //# floating
    //# motives
    //# melodies
    //# harmonies
    //# bourdon		slow bass chords
    //# waves
    //# melancholic	maybe
    //# quiet
    //# static
    //# mystic
    //# SoundSpaceShip
    //# churchbells
    //# fairies
    //# aurora
    //# sound
    //# lesley
    //# delay
    //# lyric
    //# dream
    //# angels
    //# heaven
    //# water
    //# air
    //# children
    //# ritual
    musicBoxConf.date = F("2019-01-06_16h16m57s	SUN");
    select_in(SCALES, pentatonic_minor);
    select_in(JIFFLES, penta_3rd_down_5);
    musicBoxConf.sync = 3;
    musicBoxConf.pitch = {32768, 301};
    // fix:	play_KB_macro  *2
    // harmonical CYCLE: 1h 11' 51"	SUBCYCLE: | 4' 29" |
    musicBoxConf.subcycle_octave = -4;
    break;

  case 136:
    musicBoxConf.name = F("Harmonical Musicbox alpha 0.010");
    //# favorite
    //# shortIsOk
    //# fullCycleIsOk	harmonical CYCLE: 51' 12"
    //# steady
    //# motives
    //# melodies
    //# harmonies
    //# rhytm&pulse	jazzy split
    //# beat		split
    //# aesthetic	jazz
    //# nervous		not too much
    //# crazy		a littl
    //# jazz
    //# hasSlowPulse	jazzy
    //# children
    //# dance
    musicBoxConf.sync = 5;
    select_in(SCALES, pentaCHORD);
    select_in(JIFFLES, tumtum);
    musicBoxConf.pitch = {1, 128};
    // harmonical CYCLE: 51' 48"	SUBCYCLE: | 6' 28" |
    break;

  case 137:
    musicBoxConf.name = F("TRIAD");
    //# FAVORITE
    //# SPECIALITY	strong sound effects
    //# shortIsOk
    //# ritch
    //# fractal
    //# slow
    //# motives
    //# melodies
    //# harmonies
    //# rhytm&pulse	subtle
    //# waves
    //# aesthetic
    //# nice
    //# quiet
    //# static
    //# robot
    //# sound		special sound effect combination :)
    //# lesley
    //# delay
    //# rumble
    //# hasSlowPulse	slow and subtle motiv ostinato
    //# lyric
    //# dream
    //# angels
    //# heaven
    //# children
    //# festive
    //# ritual
    musicBoxConf.date = F("2019-01-06_22h12m45s	SUN");
    select_in(SCALES, TRIAD);
    select_in(JIFFLES, rising_pent_them);
    musicBoxConf.sync = 1;
    musicBoxConf.pitch = {32768, 428};
    // fix:	play_KB_macro  /2
    // harmonical CYCLE: 3h 35' 35"	SUBCYCLE: | 6' 44" |
    musicBoxConf.subcycle_octave = -5;
    break;

  case 138:
    musicBoxConf.name = F("rhythm tuned in");
    //comment: very nice, simple octaves_fourths pattern, deep down to rhythmical roots
    //# favorite+
    //# SPECIALITY	rhythm and pulse
    //# shortIsOk
    //# bigBang
    //# loose
    //# elementary
    //# fractal
    //# simple
    //# simlicistic
    //# slow
    //# steady
    //# busy
    //# rhytm&pulse	*strong* slow pulse clicks & rhythms
    //# beat		slow clicks
    //# structure	simplicistic
    //# aesthetic
    //# nice
    //# quiet
    //# static
    //# clock
    //# rumble
    //# hasSlowPulse	*strong* slow pulse clicks & rhythms
    musicBoxConf.date = F("2019-01-08_08h46m00s	TUE");
    select_in(SCALES, octaves_fourths);
    select_in(JIFFLES, din_dididi);
    musicBoxConf.sync = 0;
    // fixed random tuning a
    musicBoxConf.pitch = {32768, 220};
    // harmonical CYCLE: 6d 23h 45' 55"	SUBCYCLE: | 4' 55" |
    musicBoxConf.subcycle_octave = -11;
    break;

  case 139:
    musicBoxConf.name = F("2019-02-01 ardour5 noname");
    //# RECORDED	name = F("2019-02-01 ardour5 nomusicBoxConf.name");
    //# favorite+
    //# shortIsOk
    //# ritch
    //# slow
    //# motives
    //# melodies
    //# waves
    //# aesthetic
    //# nice
    //# quiet
    //# static
    //# churchbells
    //# sound
    //# dream
    //# ritual	maybe
    musicBoxConf.sync = 1;
    select_in(JIFFLES, PENTA_3rd_rise);
    musicBoxConf.pitch = {1, 32};	//	*metric* ?
    // harmonical CYCLE: 1h 24' 29"	SUBCYCLE: | 5' 17" |
    break;

  case 140:
    musicBoxConf.name = F("noname starting 2019-04-11_12h");
    //# favorite
    //# shortIsOk
    //# fullCycleIsOk	harmonical CYCLE: 1h 14' 28"
    //# stackedStart
    //# dense
    //# ritch
    //# fractal
    //# chaotic
    //# soundscape
    //# busy
    //# floating
    //# motives
    //# melodies
    //# harmonies
    //# whirles
    //# waves
    //# structure	stacked sync waves
    //# aesthetic
    //# quiet
    //# mystic
    //# cosmic
    //# SoundSpaceShip
    //# fairies
    //# aurora
    //# sound
    //# lesley
    //# delay
    //# lyric
    //# dream
    //# children
    //# happy
    musicBoxConf.date = F("2019-04-11_12h");
    select_in(SCALES, pentatonic_minor);
    select_in(JIFFLES, pentatonic_desc);
    musicBoxConf.sync = 0;
    musicBoxConf.stack_sync_slices = 32;
    // musicBoxConf.base_pulse = 15;	// TODO: rethink that
    musicBoxConf.pitch = {1, 220};
    musicBoxConf.chromatic_pitch = 1;	// a
    // musicBoxConf.subcycle_octave = 3;	// TODO: rethink that
    break;

  case 141:
    musicBoxConf.name = F("2019-02-04_doric_cycle_S1");
    //comment: ardour 5
    //# RECORDED	    //comment: ardour 5
    //# favorite+
    //# shortIsOk
    //# fractal
    //# simple
    //# slow
    //# motives
    //# melodies
    //# harmonies
    //# waves
    //# aesthetic
    //# quiet
    //# static
    //# beep
    //# robot
    //# boring
    musicBoxConf.sync = 1;
    select_in(JIFFLES, d4096_1024);
    musicBoxConf.pitch = {1, 32};	//		*metric* a
    // harmonical CYCLE: 2h 48' 57"	SUBCYCLE: | 2' 38" |
    break;

  case 142:
    musicBoxConf.name = F("2019-02-04_bordun, rhythm");	// with rhythm
    //comment: ardour5
    //# RECORDED    //comment: ardour5
    //# FAVORITE
    //# SPECIALITY
    //# shortIsOk
    //# bigBang
    //# elementary
    //# simple
    //# simlicistic
    //# slow
    //# fast
    //# steady
    //# busy
    //# rhytm&pulse	simplicistic
    //# beat
    //# bourdon
    //# static
    //# clock
    //# jazz
    //# rumble
    //# hasSlowPulse
    //# boring
    select_in(SCALES, octave_4th_5th);
    musicBoxConf.sync = 0;
    select_in(JIFFLES, tumtumtum);
    musicBoxConf.pitch = {2, 61};
    // harmonical CYCLE: 6h 0' 26"	SUBCYCLE: | 2' 38" |
    break;

  case 143:
    musicBoxConf.name = F("nice little dance");
    //# FAVORITE
    //# shortIsOk
    //# ritch
    //# fractal
    //# busy
    //# motives
    //# melodies
    //# harmonies
    //# aesthetic	very nice sound effects
    //# nice
    //# happy		happy and satisfied
    //# quiet
    //# strange
    //# funny
    //# birds		no, but other creatures ;)
    //# sound		very nice sound effects
    //# lesley
    //# delay
    //# noise
    //# humming
    //# lyric
    //# children
    //# dance
    musicBoxConf.date = F("2019-04-11_10h");
    select_in(SCALES, triad);
    select_in(JIFFLES, pent_top_wave_0);
    musicBoxConf.sync = 1;
    musicBoxConf.stack_sync_slices = 1024;
    // musicBoxConf.base_pulse = 15;	// TODO: rethink that
    musicBoxConf.pitch = {1, 196};
    musicBoxConf.chromatic_pitch = 11;	// g
    break;

  case 144:
    musicBoxConf.name = F("2019-02-6_tumtum_pentatonic_minor_S2");
    //# favorite
    //# shortIsOk
    //# steady
    //# busy
    //# melodies
    //# harmonies
    //# rhytm&pulse	jazzy
    //# beat
    //# aesthetic	jazzy
    //# nervous		a bit
    //# crazy		just a bit
    //# funny
    //# jazz
    //# rumble
    //# children
    //# dance
    musicBoxConf.date = F("2019-02-06_15h44m39s	WED");
    select_in(SCALES, pentatonic_minor);
    select_in(JIFFLES, tumtum);
    musicBoxConf.sync = 2;
    // random *metric* tuning a
    musicBoxConf.pitch = {32768, 220};
    // fix:	play_KB_macro  *2
    // harmonical CYCLE: 1h 38' 18"	SUBCYCLE: | 6' 9" |
    musicBoxConf.subcycle_octave = -4;
    break;

  case 145:
    musicBoxConf.name = F("zagg  2019-02-06_tetraCHORD_pentachord_rise_S1");
    //# favorite
    //# shortIsOk
    //# soundscape
    //# slow
    //# floating
    //# motives
    //# melodies
    //# whirles
    //# waves
    //# aesthetic
    //# quiet
    select_in(JIFFLES, pentaCHORD_rise);
    musicBoxConf.pitch = {3, 10};	// 24	*metric* A
    // harmonical CYCLE: 0"	SUBCYCLE: | 4' 13" |
    musicBoxConf.date = F("2019-02-06_20h31m52s	WED");
    select_in(SCALES, tetraCHORD);
    select_in(JIFFLES, pentaCHORD_rise);
    musicBoxConf.sync = 1;
    musicBoxConf.pitch = {32768, 1024};
    // harmonical CYCLE: 2h 15' 10"	SUBCYCLE: | 4' 13" |
    musicBoxConf.subcycle_octave = -5;
    break;

  case 146:
    musicBoxConf.name = F("2019-02-06_RG3_S0_d64_melody-rhythm");    // pitch variation of    "noname_a"
    //# FAVORITE
    //# SPECIALITY	rhythm and short jiffs	see: "nomusicBoxConf.name_a"
    //# rhytm&pulse
    //# shortIsOk
    //# bigBang
    //# loose
    //# elementary
    //# simple
    //# simlicistic
    //# motives
    //# harmonies
    //# structure	loose
    //# classicHarmonical
    //# aesthetic
    //# nice
    //# quiet
    //# rumble
    musicBoxConf.pitch = {2, 61};	// 44	*metric* G
    // harmonical CYCLE: 0"	SUBCYCLE: | 5' 38" |
    musicBoxConf.date = F("2019-02-06_21h18m28s	WED");
    select_in(SCALES, octave_4th_5th);
    select_in(JIFFLES, d4096_64);
    musicBoxConf.sync = 0;
    musicBoxConf.pitch = {32768, 6144};
    // harmonical CYCLE: 6h 0' 26"	SUBCYCLE: | 5' 38" |
    musicBoxConf.subcycle_octave = -6;
    break;

  case 147:
    musicBoxConf.name = F("noname_a");	// pitch variation of "2019-02-06_RG3_S0_d64_melody-rhythm"
    //# FAVORITE
    //# SPECIALITY	rhythm and short jiffs	see: "2019-02-06_RG3_S0_d64_melody-rhythm"
    //# shortIsOk
    //# bigBang
    //# loose
    //# elementary
    //# simple
    //# simlicistic
    //# motives
    //# harmonies
    //# structure	loose
    //# classicHarmonical
    //# aesthetic
    //# nice
    //# quiet
    //# rumble
    select_in(SCALES, octave_4th_5th);
    musicBoxConf.sync = 0;
    select_in(JIFFLES, d4096_64);
    musicBoxConf.pitch = {2, 61};	// 44	*metric* d
    // harmonical CYCLE: 0"	SUBCYCLE: | 3' 41" |
    break;

  case 148:
    musicBoxConf.name = F("noname_b");
    //# favorite
    //# shortIsOk
    //# motives
    //# melodies
    //# harmonies
    //# whirles
    //# waves
    //# quiet
    //# static	a bit
    //# hasSlowPulse
    //# lyric
    //# sound
    //# dream
    //# children
    select_in(SCALES, tetraCHORD);
    musicBoxConf.sync = 1;
    select_in(JIFFLES, pentachord_rise);
    musicBoxConf.pitch = {3, 10};	// 24	*metric* a
    // harmonical CYCLE: 2h 37' 17"	SUBCYCLE: | 4' 55" |
    break;

  case 149:
    musicBoxConf.name = F("noname_c");
    //# favorite
    //# shortIsOk
    //# motives
    //# aesthetic
    //# nice
    //# quiet
    //# static
    //# sound
    //# lesley
    //# delay
    //# boring
    select_in(SCALES, europ_PENTAtonic);
    musicBoxConf.sync = 4;
    select_in(JIFFLES, tumtumtumtum);
    musicBoxConf.pitch = {1, 12};	// 8	*metric* f
    // harmonical CYCLE: 49' 26"	SUBCYCLE: | 6' 11" |
    break;

  case 150:
    // catched ERROR: no period	via lower_audio_if_too_high(limit)	// TODO: REMOVE: this line
    musicBoxConf.name = F("simplicistic_d0");
    //# favorite
    //# shortIsOk
    //# loose
    //# elementary
    //# simple
    //# simlicistic
    //# slow
    //# steady
    //# busy
    //# rhytm&pulse	fundamental
    //# quiet
    //# static
    //# clock
    //# boring
    select_in(SCALES, octaves_fourths);
    musicBoxConf.sync = 2;
    select_in(JIFFLES, din_dididi);
    musicBoxConf.pitch = {3, 52};	// 4288		*metric* g
    // harmonical CYCLE: 7d 20h 18' 29"	SUBCYCLE: | 5' 31" |
    break;

  case 151:
    musicBoxConf.name = F("2019-02-07_classical_time_machine_S4");
    //comment: resonating strings not really tuned...
    //comment: retuned version with problems in ending...
    //# RECORDED	probably yes, ???
    //# FAVORITE-	beware of stress!
    //# systemStress	some
    //# shortIsOk
    //# fullCycleIsOk	harmonical CYCLE: 53' 25"
    //# fractal
    //# simple
    //# slow
    //# melodies	harmonical progression
    //# harmonies
    //# classicHarmonical
    //# aesthetic
    //# nice
    //# cosmic
    //# hasSlowPulse	harmonical progression
    //# lyric
    //# festive
    //# solemn		beware of stress!
    musicBoxConf.date = F("2019-02-07_09h46m29s	THU");
    select_in(SCALES, minor_scale);
    select_in(JIFFLES, d4096_1024);
    musicBoxConf.sync = 4;
    // random *metric* tuning c
    musicBoxConf.pitch = {32768, 262};
    // harmonical CYCLE: 1h 22' 33"	SUBCYCLE: | 5' 10" |
    musicBoxConf.subcycle_octave = -4;
    break;

  case 152:
    musicBoxConf.name = F("noname_");
    //# favorite
    //# shortIsOk	but easily *too short*
    //# fullCycleIsOk	harmonical CYCLE: 1h 8' 16"
    //# loose
    //# ritch	very interesting melody sequences :) :) :)
    //# fractal
    //# slow	but busy
    //# busy
    //# motives
    //# melodies	very interesting melody sequences :) :) :)
    //# harmonies
    //# whirles
    //# waves
    //# structure	very interesting melody sequences :) :) :)
    //# aesthetic
    //# nice
    //# quiet
    //# mystic
    //# lyric
    //# dream
    //# children
    //# ritual	secret simple knowledge
    //comment: try variations in SYNC :)
    select_in(SCALES, europ_PENTAtonic);
    musicBoxConf.sync = 5;
    select_in(JIFFLES, pentaCHORD_rise);
    musicBoxConf.pitch = {1, 12};	// 8  1/175	*metric* f
    // harmonical CYCLE: 49' 26"	SUBCYCLE: | 6' 11" |
    break;

  case 153:
    musicBoxConf.name = F("eifach wunderschön");
    //# FAVORITE
    //comment: sehr ruhig
    musicBoxConf.date = F("2019-02-26_12h46m19s	TUE");
    select_in(SCALES, doric_scale);
    musicBoxConf.sync = 1;
    select_in(JIFFLES, diing_ditditdit);
    musicBoxConf.pitch = {1, 330};	// 8 1/330 metric e
    // was:  musicBoxConf.pitch = {32768, 12};	// 8 1/330 metric e
    // harmonical CYCLE: 3h 18' 35"	SUBCYCLE: | 6' 12" |
    break;

  case 154:
    musicBoxConf.name = F("2019-04-01 doric stack");
    select_in(SCALES, doric_scale);
    musicBoxConf.sync = 1;
    musicBoxConf.stack_sync_slices = 8;
    musicBoxConf.base_pulse = 15;
    select_in(JIFFLES, din__dididi_dixi);
    musicBoxConf.pitch = {1, 175};	// 8 1/175 metric f
    // was:  musicBoxConf.pitch = {32768, 12};	// 8 1/175 metric f
    // harmonical CYCLE: 3h 7' 14"	2^4 SUBCYCLE: | 11' 42" |
    break;

  case 155:
    musicBoxConf.name = F("Marianne 1	eingesperrt<->Freiheit	sync? 4|1024");
    /*
      in einer Gefängniszelle
      mit Vögel draussen
      Eingesperrt sein | Freiheit
    */
    //# FAVORITE
    //# shortIsOk
    //# fractal
    //# soundscape
    //# slow		but steady busy
    //# steady
    //# busy
    //# melodies	very nice
    //# structure	imprisoned freedom
    //# aesthetic
    //# nice
    //# quiet
    //# mystic
    //# birds
    //# churchbells
    //# sound
    //# hasSlowPulse	melody net structure
    //# lyric		"it is like it is"
    //# dream
    //# children
    //# dance		"it is like it is"
    //# festive
    //# tired		possible
    select_in(SCALES, tetraCHORD);
    select_in(JIFFLES,tetraCHORD_rise);
    musicBoxConf.sync = 4;		// S unleserlich, 4?
    musicBoxConf.stack_sync_slices=1024;
    // p[15]|1024	// unleserlich, 1024?
    musicBoxConf.chromatic_pitch = 0;
    musicBoxConf.pitch={1, 294};		// d just a random default
    break;

  case 156:
    musicBoxConf.name = F("Marianne 2	warten");
    /*
      warten
      Langeweile
    */
    //# favorite
    //# shortIsOk
    //# stackedStart
    //# dense	at stacked start only
    //# loose
    //# fractal
    //# simple
    //# slow
    //# ticking
    //# busy	quite slow
    //# structure	very simple
    //# quiet
    //# static		seems to be, but is not
    //# clock
    //# beep
    //# robot
    //# humming
    //# rumble		probably a bit
    //# hasSlowPulse	beepbeep
    //# children
    //# festive		possibly
    //# boring		possible
    select_in(SCALES, pentatonic_minor);
    select_in(JIFFLES, ding_ditditdit);
    musicBoxConf.sync = 0;
    musicBoxConf.stack_sync_slices=64;
    musicBoxConf.pitch={1, 175};		// f
    musicBoxConf.chromatic_pitch = 9;	// f
    // 11'42"
    break;

  case 157:
    musicBoxConf.name = F("Marianne 3	s erst Mal underwägs");
    /*
      Bild: Spital in Shri Lanka
      ein Mann ist das allererste Mal mit seinen Prothesen unterwegs
    */
    //# FAVORITE
    //# shortIsOk
    //# fast
    //# busy
    //# melodies	harmonic progression
    //# harmonies
    //# beat		not steady, but strongly pulsating decelerando
    //# structure	resulting pattern is very nice structured
    //# aesthetic
    //# nice
    //# happy
    //# hasSlowPulse	no, but clear structure to follow
    //# lyric
    //# children
    //# festive		can be
    select_in(SCALES, doric_scale);
    select_in(JIFFLES, d4096_128);
    musicBoxConf.sync = 1;
    musicBoxConf.stack_sync_slices=128;
    musicBoxConf.pitch={1, 330};
    musicBoxConf.chromatic_pitch = 4;		// e
    // 6'12"
    break;

  case 158:
    musicBoxConf.name = F("Marianne 4	Chatzemusik!");
    /*
      Chatzemusik!
      Mariannes Katze kommt, als das Stück läuft...
    */
    //# favorite
    //# shortIsOk
    //# fractal
    //# chaotic
    //# soundscape
    //# slow
    //# fast
    //# melodies	harmonic progression
    //# harmonies
    //# aesthetic	can be ;)
    //# funny
    //# robot
    //# children
    //# boring
    select_in(SCALES, pentaCHORD);	// unleserlich,,,
    select_in(JIFFLES, pent_top_wave_0);
    musicBoxConf.sync = 2;
    musicBoxConf.stack_sync_slices=0;
    musicBoxConf.pitch={1, 175};		// f
    musicBoxConf.chromatic_pitch = 9;		// f
    // 9'22"
    break;

  case 159:
    musicBoxConf.name = F("Marianne 5	Gefängnisgeburt");
    /*
      eine Freundin von Marianne bekommt im Gefängnis ihr Baby
    */
    //# favorite
    //# shortIsOk
    //# fractal
    //# chaotic		some will perceive it is
    //# soundscape	strange motives
    //# steady
    //# busy
    //# motives		strange, a bit like biologic, animals
    //# melodies
    //# harmonies
    //# crazy
    //# funny
    //# mystic
    //# jazz
    //# dream
    //# hell
    //# earth
    //# water
    //# air
    //# children
    //# ritual
    //# boring
    //# tired
    select_in(SCALES, octave_4th_5th);
    select_in(JIFFLES, simple_theme);
    musicBoxConf.sync = 3;
    musicBoxConf.stack_sync_slices=0;
    musicBoxConf.pitch={1, 294};		// d
    musicBoxConf.chromatic_pitch = 6;		// d
    // zeit unleserlich
    break;

  case 160:
    musicBoxConf.name = F("rising ostinatos over rhythmic bordun");  // nice and harmonic, humming, good ending
    //comment: very nice ending :)
    //# FAVORITE
    //# SPECIALITY	from rhythm, beat to sound effects, everything's there

    //# heavyStart	but ok
    //# shortIsOk
    //# bigBang
    //# fractal		from rhythm, beat to sound effects, everything's there
    //# soundscape
    //# slow
    //# fast
    //# ticking		strongly audible very steady, driving pulse
    //# steady
    //# busy
    //# motives
    //# melodies
    //# harmonies	often strange and staccato
    //# rhytm&pulse	strongly audible very steady, driving pulse
    //# beat
    //# whirles
    //# waves
    //# structure
    //# aesthetic
    //# nice
    //# dramatic	no, but very steady, driving pulse
    //# sound
    //# noise
    //# humming
    //# rumble
    //# hasSlowPulse
    //# lyric
    //# children
    //# dance
    //# ritual
    musicBoxConf.sync = 0;
    select_in(SCALES, octave_4th_5th);
    select_in(JIFFLES, pentatonic_rise);
    musicBoxConf.pitch = {2, 6144};
    // harmonical cycle 'c'	12h 24'  7"	subcycle    5' 49"
    break;

  case 161:
    musicBoxConf.name = F("au sehr schön:");
    //# favorite+
    //# shortIsOk
    //# ritch
    //# fractal
    //# slow
    //# busy
    //# floating
    //# motives
    //# melodies
    //# harmonies	strange and blue, sometimes
    //# aesthetic
    //# quiet
    //# strange
    //# crazy
    //# mystic
    //# sound
    //# dream
    //# children
    //# dance
    //# festive
    //# ritual
    //# happy
    musicBoxConf.sync = 2;
    select_in(SCALES, tetraCHORD);
    select_in(JIFFLES, PENTA_3rd_rise);
    musicBoxConf.pitch = {3, 1024};
    break;

  case 162:
    musicBoxConf.name = F("klassisch und simpel schön");
    //# favorite
    //# heavyStart	problematcic	use autostack_S0
    //# shortIsOk
    //# bigBang
    //# ritch
    //# fractal
    //# simple
    //# soundscape
    //# slow
    //# steady
    //# busy
    //# motives
    //# melodies
    //# harmonies
    //# bourdon
    //# whirles
    //# waves
    //# aesthetic
    //# nice
    //# quiet
    //# sound
    //# humming
    //# rumble
    musicBoxConf.sync = 0;
    select_in(SCALES, tetraCHORD);
    select_in(JIFFLES, pentatonic_desc);
    musicBoxConf.pitch = {3, 1024};
    break;

  case 163:
    musicBoxConf.name = F("harmonikali jazzsession");
    //comment: harmonical cycle    54'  4"
    musicBoxConf.date = F("2018-11-21_20h21m11s	WED");
    select_in(SCALES, minor_scale);
    select_in(JIFFLES, tumtum);
    musicBoxConf.sync = 5;
    musicBoxConf.pitch = {1, 128};
    // was:  musicBoxConf.pitch = {32768, 200};
    // CYC: 2^0	   54'  4"
    musicBoxConf.subcycle_octave = -4;	// 3' 23"	| subcycle |
    break;

  case 164:
    musicBoxConf.name = F("busy soundscape");      // zarts soundgflecht
    musicBoxConf.date = F("");
    //# favorite+
    //# shortIsOk
    //# dense
    //# ritch
    //# fractal
    //# soundscape
    //# busy
    //# floating
    //# motives
    //# melodies
    //# harmonies
    //# aesthetic
    //# nice
    //# quiet
    //# static
    //# mystic
    //# cosmic
    //# churchbells
    //# fairies
    //# humming
    //# dream
    //# children
    //# festive		possibly
    //# ritual		who knows?
    //# boring		not for me
    musicBoxConf.sync = 5;
    musicBoxConf.stack_sync_slices = 16;
    select_in(SCALES,europ_PENTAtonic );
    select_in(JIFFLES, pent_patternA);
    musicBoxConf.pitch = {1, 262};	// metric c
    musicBoxConf.chromatic_pitch = 4;
    break;

  case 165:
    musicBoxConf.name = F("stacked clicks");	// stacked start :)
    //comment: interesting start ;)
    //# favorite+
    //# shortIsOk
    //# fullCycleIsOk	harmonical CYCLE: 1h 23' 35"
    //# melodies	harmonic progression
    //# harmonies
    //# rhytm&pulse	strong at stat, stacking
    //# bourdon
    //# structure	stacked start, then bourdon and harmonical progression
    //# classicHarmonical	variation
    //# aesthetic	a bit technical
    //# nice		a bit technical
    //# static
    //# simple
    //# simlicistic
    //# fractal
    //# clock
    //# beep
    //# robot
    //# noise
    //# humming
    //# rumble
    //# hasSlowPulse	stacked sync rumbling
    //# lyric
    //# dream
    //# boring
    //# slow
    //# steady	insisting
    select_in(SCALES,europ_PENTAtonic);
    select_in(JIFFLES,d4096_32);
    musicBoxConf.sync = 0;
    musicBoxConf.stack_sync_slices = 64;
    musicBoxConf.base_pulse = 15;
    musicBoxConf.pitch={1, 262};
    musicBoxConf.chromatic_pitch=4;	// c
    // musicBoxConf.subcycle_octave = -1;	// TODO: select a short piece ################
    break;

  case 166:
    musicBoxConf.name = F("mad accordeon players whichdance with old indio's flute babbling");
    //# favorite
    //# systemStress
    // Störfall? mit musicBoxConf.stack_sync_slices=0
    //# heavyStart	// added musicBoxConf.stack_sync_slices = -64; to try a fix	// TODO: test&trim
    //# shortIsOk
    //# bigBang
    //# dense
    //# ritch
    //# fractal
    //# chaotic
    //# soundscape
    //# slow
    //# floating
    //# motives
    //# melodies
    //# harmonies
    //# crazy
    //# funny
    //# mystic
    //# cosmic
    //# churchbells
    //# fairies
    //# sound
    //# lesley
    //# delay
    //# humming
    //# lyric
    //# dream
    //# boring
    musicBoxConf.date = F("2018-11-21_17h35m59s	WED");
    select_in(SCALES, pentatonic_minor);
    select_in(JIFFLES, dwn_THRD_up);
    musicBoxConf.sync = 0;
    musicBoxConf.stack_sync_slices = -64;	// TODO: test&trim
    musicBoxConf.pitch = {32768, 384};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	1h 52' 38"
    musicBoxConf.subcycle_octave = -5;	// 3' 31"	| subcycle |
    break;

  case 167:
    musicBoxConf.name = F("fractal time web");
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
    musicBoxConf.date = F("2018-12-05 15h40m");
    musicBoxConf.sync = 1;
    select_in(SCALES, doric_scale);
    select_in(JIFFLES, up_THRD_dn);
    musicBoxConf.pitch = {1, 32};
    break;

  case 168:
    musicBoxConf.name = F("Perlechetti");
    //# favorite+
    //# shortIsOk
    //# fullCycleIsOk	harmonical CYCLE: 1h 13' 48"
    //# ritch
    //# fractal
    //# soundscape
    //# floating
    //# motives
    //# aesthetic
    //# nice
    //# children
    //# dance
    musicBoxConf.date = F("2019-04-15");
    select_in(SCALES, pentatonic_minor);
    select_in(JIFFLES, pentatonic_desc);
    musicBoxConf.sync = 1;
    musicBoxConf.pitch = {1, 111};
    break;

  case 169:
    musicBoxConf.name = F("2019-02-06_major_doric_rise");
    //comment: ardour 5
    //# RECORDED    //comment: ardour 5
    //# ISSUE	TODO: does not start, *VERY* long pause
    //	random pitch as workaround
    musicBoxConf.date = F("2019-02-06_13h32m52s	WED");
    select_in(SCALES, major_scale);
    select_in(JIFFLES, doric_rise);
    musicBoxConf.sync = 4;
    musicBoxConf.pitch = {1, 128};	// just a random pitch...
    // was: musicBoxConf.pitch = {32768, 32};
    // harmonical CYCLE: 33' 47"	SUBCYCLE: | 5' 31" |
    // | 5' 31" |
    break;

  case 170:
    musicBoxConf.name = F("doric musicbox S2");
    //# favorite+
    //# shortIsOk
    //# fractal
    //# simple
    //# slow
    //# harmonies
    //# classicHarmonical
    //# aesthetic
    //# nice
    //# quiet
    //# cosmic
    //# hasSlowPulse	harmonical progression
    //# dream
    //# ritual
    musicBoxConf.date = F("2019-04-02");
    select_in(SCALES, doric_scale);
    select_in(JIFFLES, diing_ditditdit);
    musicBoxConf.sync = 2;
    musicBoxConf.pitch = {1, 196};	// metric g
    break;

  case 171:
    musicBoxConf.name = F("George's Dream");
    //# favorite
    //# shortIsOk
    //# fullCycleIsOk
    //# fractal
    //# simple		quite, somehow
    //# slow
    //# motives
    //# melodies
    //# aesthetic	modern
    //# nice		modern
    //# quiet
    //# static
    //# funny
    //# jazz
    //# sound
    //# lesley
    //# hasSlowPulse	repeating motivpatterns
    //# dream
    //# air
    //# children
    //# boring
    //# tired		maybe?
    musicBoxConf.date = F("2019-03-29");
    select_in(SCALES, TRIAD);
    select_in(JIFFLES, up_THRD_dn);
    musicBoxConf.sync = 1;
    musicBoxConf.pitch = {1, 428};
    break;

  case 172:
    musicBoxConf.name = F("no meh melodie :)");
    // SCHWÄRE STÖRFALL !
    //# favorite--	only with autostack_S0
    //#heavyStart	PROBLEMATIC, throughout...	use autostack_S0 !
    //# dropouts	problematic
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
    musicBoxConf.date = F("2018-11-27_17h22m21s	TUE");
    select_in(SCALES, octave_4th_5th);
    select_in(JIFFLES, dwn_THRD_up);
    musicBoxConf.sync = 0;
    musicBoxConf.pitch = {32768, 357};
    // random octave shift: used SUBCYCLE:	3' 2"
    // CYC: 2^0	6h 27' 42"
    musicBoxConf.subcycle_octave = -7;	// 3'  2"	| subcycle |
    break;

  case 173:
    musicBoxConf.name = F("Klangschwade mit Puls");
    // heavy big bang	Störfall ???  kän grund zur Beunruhigung...
    //# favorite  with autostack_S0
    //# shortIsOk
    //# motives
    //# harmonies	static, more like bourdon
    //# bourdon		permanent
    //# heavyStart	use autostack_S0 !
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
    musicBoxConf.date = F("2018-12-07_18h34m01s	FRI");
    select_in(SCALES, europ_PENTAtonic);
    select_in(JIFFLES, tanboura);
    musicBoxConf.sync = 0;
    musicBoxConf.pitch = {32768, 275};
    // fix:	play_KB_macro  *2
    // CYC: 2^0	2h  5' 49"
    musicBoxConf.subcycle_octave = -4;	// 7' 52"	| subcycle |
    break;

  case 174:
    musicBoxConf.name = F("2019-02-02 ardour5 noname");	// TODO: broken?
    //# RECORDED	name = F("2019-02-02 ardour5 noname");	// TODO: broken?
    //# favorite  with autostack_S0
    //# ISSUE	was broken, set *random* pitch {1,128} as workaround
    //# ISSUE heavyStart is problematic   use autostack_S0 !
    //# heavyStart
    musicBoxConf.date = F("2019-02-02_17h49m06s	SAT");
    select_in(SCALES, doric_scale);
    select_in(JIFFLES, pentatonic_rise);
    musicBoxConf.sync = 0;
    // was: musicBoxConf.pitch = {32768, 32};
    musicBoxConf.pitch = {1, 128};
    // harmonical CYCLE: 2h 48' 57"	SUBCYCLE: | 3' 41" |
    // | 2' 38" |
    break;

  case 175:
    musicBoxConf.name = F("Kaleidoskop 1500");
    musicBoxConf.date = F("2019_10_12_10h SAT");
    select_in(SCALES, octaves_fifths);
    select_in(JIFFLES, jiff_1500_2);
    musicBoxConf.sync = 3;
    musicBoxConf.pitch = {1, 110};
    musicBoxConf.chromatic_pitch = 0;	// a
    // harmonical CYCLE: 7d 1h 27' 4" 	2^10 SUBCYCLE: | 9' 56" |
    break;

  case 176:
    musicBoxConf.name = F("veryverybad");
    //# shortIsOk
    //# dense
    //# melancholic
    //# dramatic
    //# crazy
    //# chaotic
    select_in(SCALES, europ_PENTAtonic);
    musicBoxConf.sync = 3;
    select_in(JIFFLES, tetraCHORD_desc);
    musicBoxConf.pitch = {1, 12};	// 8	*metric* c
    // harmonical CYCLE: 1h 6' 2"	SUBCYCLE: | 4' 8" |
    break;

    // DADA:	todo: CHECK FROM HERE <<<====	====>>>   #define SHORT_PRESET_COLLECTION	175
    // DADA: >>>> *DO NOT FORGET* to upmusicBoxConf.date MUSICBOX_PRESETs <<<<	175


  default:
    MENU.error_ln(F("invalid preset"));		// ERROR, should not happen
    return true;
  }

  if(musicBoxConf.sync==0 && musicBoxConf.stack_sync_slices==0) {		// *AUTOMAGIC WORKAROUND* for problematic S0 setups:
    musicBoxConf.stack_sync_slices=MagicConf.autostack_S0;	// using autostack_S0
    if(MENU.verbosity >= VERBOSITY_LOWEST) {
      MENU.out(F("\tAUTOSTACK: "));
      MENU.outln(musicBoxConf.stack_sync_slices);
    }
  }

  MENU.tab();
  parameters_by_user(true);	// true: block output
  HARMONICAL.reduce_fraction(&musicBoxConf.pitch);	// ;)

  if(musicBoxConf.name) {
    MENU.tab();
    MENU.out(musicBoxConf.name);
  }
  if(musicBoxConf.date) {
    MENU.tab();
    MENU.out(musicBoxConf.date);
  }
  MENU.ln();
  musicBox_short_info();

  return false;		// ok, no error
} // load_preset()


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
