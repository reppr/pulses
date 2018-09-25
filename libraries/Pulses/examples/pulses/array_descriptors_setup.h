/*
  array_descriptors_setup.h
*/

#define EDIT_SCALE_RAM_SIZE	31+1	// FIXME: maybe ;)
unsigned int edit_scale_RAM[EDIT_SCALE_RAM_SIZE] = {0};


init_arr_DB(SCALES, sizeof(SCALES), "SCALES");
REGISTER_SCALE(edit_scale_RAM);			// 0
REGISTER_SCALE(octaves);			// 1
REGISTER_SCALE(octaves_fifths);			// 2
REGISTER_SCALE(octaves_fourths_fifths);		// 3
REGISTER_SCALE(tetrachord);			// 4
REGISTER_SCALE(pentatonic_minor);		// 5
REGISTER_SCALE(european_pentatonic);		// 6
REGISTER_SCALE(mimic_japan_pentatonic);		// 7
REGISTER_SCALE(minor_scale);			// 8
REGISTER_SCALE(major_scale);			// 9
REGISTER_SCALE(scale_int);			// 10
REGISTER_SCALE(overnotes);			// 11
REGISTER_SCALE(scale_rationals);		// 12
REGISTER_SCALE(octaves_fourths);		// 13
REGISTER_SCALE(doric_scale);			// 14

select_array_in(SCALES, pentatonic_minor);	// default scale


#include "jiffles.h"
init_arr_DB(JIFFLES, sizeof(JIFFLES), "JIFFLES");
REGISTER_JIFFLE(jiffle_RAM);
REGISTER_JIFFLE(gling128);		// FIXME: replace ################
REGISTER_JIFFLE(jiffletab);
REGISTER_JIFFLE(jiff_december);
REGISTER_JIFFLE(jiff_dec128);	// jiff_dec128 with sync=0 to hear the big drum in the cathedral ;)
REGISTER_JIFFLE(jiff_dec_pizzicato);
REGISTER_JIFFLE(jiffletab0);
REGISTER_JIFFLE(jiffletab01);
REGISTER_JIFFLE(jiffletab02);
REGISTER_JIFFLE(jiffletab03);
REGISTER_JIFFLE(jiffletab04);
REGISTER_JIFFLE(jiffletab05);
REGISTER_JIFFLE(jiffletab06);
REGISTER_JIFFLE(jiffletab06);		 // ==12 ??? FIXME: replace
REGISTER_JIFFLE(gling128_0);
REGISTER_JIFFLE(gling128_1);
REGISTER_JIFFLE(gling128_2);
REGISTER_JIFFLE(harmonics4);
REGISTER_JIFFLE(ting1024);
REGISTER_JIFFLE(ting4096);
REGISTER_JIFFLE(arpeggio4096);
REGISTER_JIFFLE(arpeggio4096down);
REGISTER_JIFFLE(arpeggio_cont);
REGISTER_JIFFLE(arpeggio_and_down);
REGISTER_JIFFLE(stepping_down);
REGISTER_JIFFLE(back_to_ground);
REGISTER_JIFFLE(arpeggio_and_sayling);
REGISTER_JIFFLE(simple_theme);
REGISTER_JIFFLE(pentatonic_rising);
REGISTER_JIFFLE(pentatonic_descending);
REGISTER_JIFFLE(rising_pent_theme);
REGISTER_JIFFLE(pent_top_wave_0);
REGISTER_JIFFLE(pent_top_wave);
REGISTER_JIFFLE(pent_patternA);
REGISTER_JIFFLE(pent_patternB);
REGISTER_JIFFLE(PENTAtonic_rising);
REGISTER_JIFFLE(PENTAtonic_descending);
REGISTER_JIFFLE(PENTAtonic_thirds_falling_5);
REGISTER_JIFFLE(PENTAtonic_thirds_rising);
REGISTER_JIFFLE(tingeling4096);
REGISTER_JIFFLE(ding1024);
REGISTER_JIFFLE(kalimbaxyl);
REGISTER_JIFFLE(ting_tick4096);
REGISTER_JIFFLE(tigg_ding4096);
REGISTER_JIFFLE(tumtum);
REGISTER_JIFFLE(piip2048);
REGISTER_JIFFLE(tanboura);
REGISTER_JIFFLE(d4096_2);
REGISTER_JIFFLE(d4096_4);
REGISTER_JIFFLE(d4096_6);
REGISTER_JIFFLE(d4096_8);
REGISTER_JIFFLE(d4096_16);
REGISTER_JIFFLE(d4096_12);
REGISTER_JIFFLE(d4096_32);
REGISTER_JIFFLE(d4096_64);
REGISTER_JIFFLE(d4096_128);
REGISTER_JIFFLE(d4096_256);
REGISTER_JIFFLE(d4096_512);
REGISTER_JIFFLE(d4096_1024);
REGISTER_JIFFLE(d4096_2048);
REGISTER_JIFFLE(d4096_3072);
REGISTER_JIFFLE(loong);
REGISTER_JIFFLE(looong);
REGISTER_JIFFLE(loooong);
REGISTER_JIFFLE(din__dididi_dix);
REGISTER_JIFFLE(din__dididi_dixi);
REGISTER_JIFFLE(din_dididi);
REGISTER_JIFFLE(ding_ditditdit);
REGISTER_JIFFLE(diing_ditditdit);

REGISTER_JIFFLE(pentatonic_thirds_falling_5);	// TODO: re order
REGISTER_JIFFLE(pentatonic_thirds_rising);

select_array_in(JIFFLES, d4096_1024);	// default jiffle


#include "iCODE.h"
init_arr_DB(iCODEs, sizeof(iCODEs), "iCODEs");
REGISTER_iCODE(d4096_1024_i2cLED);		// 0
REGISTER_iCODE(d4096_1024_icode_jiff);

#if defined USE_i2c
  select_array_in(iCODEs, (unsigned int *) d4096_1024_i2cLED);		// default iCODE LED spots on i2c
#else
  select_array_in(iCODEs, (unsigned int *) d4096_1024_icode_jiff);	// default iCODE, jiffling, *no* i2c
#endif

include_DB_in_DB(iCODEs, JIFFLES, 1);
// display_arr_names(iCODEs);
