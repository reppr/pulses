/*
  array_descriptors_setup.h
*/

#define EDIT_SCALE_RAM_SIZE	31+1	// FIXME: maybe ;)
unsigned int edit_scale_RAM[EDIT_SCALE_RAM_SIZE] = {0};


init_DB(SCALES, sizeof(SCALES), "SCALES");
REGISTER_SCALE(edit_scale_RAM);			// 0
REGISTER_SCALE(octaves);			// 1
REGISTER_SCALE(octaves_fifths);			// 2
REGISTER_SCALE(octave_4th_5th);			// 3
REGISTER_SCALE(tetraCHORD);			// 4
REGISTER_SCALE(pentatonic_minor);		// 5
REGISTER_SCALE(doric_scale);			// 6
REGISTER_SCALE(minor_scale);			// 7
REGISTER_SCALE(major_scale);			// 8
REGISTER_SCALE(triad);				// 9
REGISTER_SCALE(TRIAD);				// 10
REGISTER_SCALE(octaves_fourths);		// 11
REGISTER_SCALE(pentachord);			// 12
REGISTER_SCALE(pentaCHORD);			// 13
REGISTER_SCALE(tetrachord);			// 14
REGISTER_SCALE(europ_PENTAtonic);		// 15
REGISTER_SCALE(trichord);			// 16
REGISTER_SCALE(mimic_japan_pentatonic);		// 17
REGISTER_SCALE(scale_int);			// 18
REGISTER_SCALE(overnotes);			// 19
REGISTER_SCALE(scale_rationals);		// 20

select_in(SCALES, pentatonic_minor);	// default scale
DB_info(SCALES);


#include "jiffles.h"
init_DB(JIFFLES, sizeof(JIFFLES), "JIFFLES");

// RAM
REGISTER_JIFFLE(jiffle_RAM);
// REGISTER_JIFFLE(jiffle_RAM1);
// REGISTER_JIFFLE(jiffle_RAM2);
// REGISTER_JIFFLE(jiffle_RAM3);

// ???

// padding ;)
REGISTER_JIFFLE(gling128);

// simple, short -> long (*even* counts only)
REGISTER_JIFFLE(d4096_2);
REGISTER_JIFFLE(d4096_4);
REGISTER_JIFFLE(d4096_6);
REGISTER_JIFFLE(d4096_8);
REGISTER_JIFFLE(d4096_12);
REGISTER_JIFFLE(d4096_16);
REGISTER_JIFFLE(d4096_24);
REGISTER_JIFFLE(d4096_32);
REGISTER_JIFFLE(d4096_48);
REGISTER_JIFFLE(d4096_64);
REGISTER_JIFFLE(d4096_96);
REGISTER_JIFFLE(d4096_128);
REGISTER_JIFFLE(d4096_192);
REGISTER_JIFFLE(d4096_256);
REGISTER_JIFFLE(d4096_384);
REGISTER_JIFFLE(d4096_512);
REGISTER_JIFFLE(d4096_768);
REGISTER_JIFFLE(d4096_1024);	// == 1/4
REGISTER_JIFFLE(d4096_1536);
REGISTER_JIFFLE(d4096_2048);	// == 1/2
REGISTER_JIFFLE(d4096_3072);	// == 3/4

// sparkles
REGISTER_JIFFLE(din_dididi);
REGISTER_JIFFLE(din__dididi);
REGISTER_JIFFLE(din__dididi_dix);
REGISTER_JIFFLE(din__dididi_dixi);
REGISTER_JIFFLE(ding_ditditdit);
REGISTER_JIFFLE(diing_ditditdit);

REGISTER_JIFFLE(jiff_1500);	// new for commit 1500
REGISTER_JIFFLE(jiff_1500_2);	// new for commit 1500
REGISTER_JIFFLE(jiff_1500_3);	// new for commit 1500

// effects
REGISTER_JIFFLE(mech1023);
REGISTER_JIFFLE(mech511);
REGISTER_JIFFLE(mech255);
REGISTER_JIFFLE(mech2);
REGISTER_JIFFLE(mechanical);
REGISTER_JIFFLE(with_pauses);

REGISTER_JIFFLE(d2048_div6);

REGISTER_JIFFLE(tumtum);
REGISTER_JIFFLE(tumtumtum);
REGISTER_JIFFLE(tumtumtumtum);

// padding ;)
REGISTER_JIFFLE(tum8);
REGISTER_JIFFLE(tum8up);
//REGISTER_JIFFLE(tum16);
//REGISTER_JIFFLE(tum15u);

// scale patterns
REGISTER_JIFFLE(tetraCHORD_rise);
REGISTER_JIFFLE(tetraCHORD_desc);

REGISTER_JIFFLE(pentatonic_rise);
REGISTER_JIFFLE(pentatonic_desc);
REGISTER_JIFFLE(PENTAtonic_rise);
REGISTER_JIFFLE(PENTAtonic_desc);

REGISTER_JIFFLE(doric_rise);
REGISTER_JIFFLE(doric_descend);
REGISTER_JIFFLE(minor_rise);
REGISTER_JIFFLE(minor_descend);
REGISTER_JIFFLE(major_rise);
REGISTER_JIFFLE(major_descend);

REGISTER_JIFFLE(pentaCHORD_rise);
REGISTER_JIFFLE(pentaCHORD_desc);
REGISTER_JIFFLE(pentachord_rise);
REGISTER_JIFFLE(pentachord_descend);

// melodies
REGISTER_JIFFLE(simple_theme);
REGISTER_JIFFLE(rising_pent_them);
REGISTER_JIFFLE(pent_top_wave_0);
REGISTER_JIFFLE(pent_top_wave);
REGISTER_JIFFLE(pent_patternA);
REGISTER_JIFFLE(pent_patternB);

REGISTER_JIFFLE(PENTA_3rd_down_5);
REGISTER_JIFFLE(PENTA_3rd_rise);
REGISTER_JIFFLE(penta_3rd_down_5);
REGISTER_JIFFLE(penta_3rd_rise);

REGISTER_JIFFLE(up_THRD);
REGISTER_JIFFLE(up_THRD_dn);
REGISTER_JIFFLE(top_RETurn);
REGISTER_JIFFLE(dwn_THRD);
REGISTER_JIFFLE(dwn_THRD_up);

// arpeggios
REGISTER_JIFFLE(arpeggio4096);
REGISTER_JIFFLE(arpeggio4096down);
REGISTER_JIFFLE(arpeggio_cont);
REGISTER_JIFFLE(arpeggio_and_down);
REGISTER_JIFFLE(arpeggio_and_sayling);

// FIXME: TODO: out of order, leftovers...
REGISTER_JIFFLE(back_to_ground);
REGISTER_JIFFLE(stepping_down);
REGISTER_JIFFLE(loong);		// length 255/256
REGISTER_JIFFLE(looong);	// length 511/512
REGISTER_JIFFLE(loooong);	// length 1023/1024
REGISTER_JIFFLE(gling128);	// FIXME: replace ################
REGISTER_JIFFLE(gling128_2);
REGISTER_JIFFLE(jiffletab);
REGISTER_JIFFLE(jiff_december);
REGISTER_JIFFLE(jiff_dec128);	// jiff_dec128 with sync=0 to hear the big drum in the cathedral ;)
REGISTER_JIFFLE(jiff_dec_pizzica);
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
REGISTER_JIFFLE(harmonics4);
REGISTER_JIFFLE(ting1024);
REGISTER_JIFFLE(ting4096);	// RENAME:

REGISTER_JIFFLE(tingeling4096);
REGISTER_JIFFLE(ding1024);
REGISTER_JIFFLE(kalimbaxyl);
REGISTER_JIFFLE(ting_tick4096);
REGISTER_JIFFLE(tigg_ding4096);
REGISTER_JIFFLE(piip2048);
REGISTER_JIFFLE(tanboura);

REGISTER_JIFFLE(jiff_TEST);
REGISTER_JIFFLE(jiff_TEST2);
REGISTER_JIFFLE(jiff_TEST3);

REGISTER_JIFFLE(doDe_i);	// test only, remove?
REGISTER_JIFFLE(asdfg);		// test only, remove?

select_in(JIFFLES, din_dididi);	// default jiffle
DB_info(JIFFLES);


#include "iCODE.h"
init_DB(iCODEs, sizeof(iCODEs), "iCODEs");
REGISTER_iCODE(d4096_1024_i2cLED);		// 0
REGISTER_iCODE(d4096_1024_icode_jiff);

#if defined USE_i2c
  select_in(iCODEs, (unsigned int *) d4096_1024_i2cLED);		// default iCODE LED spots on i2c
#else
  select_in(iCODEs, (unsigned int *) d4096_1024_icode_jiff);	// default iCODE, jiffling, *no* i2c
#endif

include_DB_in_DB(iCODEs, JIFFLES, 1);

DB_info(iCODEs);
MENU.ln();
