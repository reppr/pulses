// array_descriptor_setup.h

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

select_array_in(SCALES, pentatonic_minor);  	// default scale


#include "jiffles.h"
init_arr_DB(JIFFLES, sizeof(JIFFLES), "JIFFLES");
REGISTER_JIFFLE(jiffle_RAM);		// 0
REGISTER_JIFFLE(gling128);		// 1	FIXME: replace ################
REGISTER_JIFFLE(jiffletab);		// 2
REGISTER_JIFFLE(jiff_december);		// 3
REGISTER_JIFFLE(jiff_dec128);		// 4
REGISTER_JIFFLE(jiff_dec_pizzicato);	// 5
REGISTER_JIFFLE(jiffletab01);		// 6
REGISTER_JIFFLE(jiffletab01);		// 7 ==6 ??? FIXME: replace
REGISTER_JIFFLE(jiffletab02);		// 8
REGISTER_JIFFLE(jiffletab03);		// 9
REGISTER_JIFFLE(jiffletab04);		// 10
REGISTER_JIFFLE(jiffletab05);		// 11
REGISTER_JIFFLE(jiffletab06);		// 12
REGISTER_JIFFLE(jiffletab06);		// 13 ==12 ??? FIXME: replace
REGISTER_JIFFLE(gling128_0);		// 14
REGISTER_JIFFLE(gling128_1);		// 15
REGISTER_JIFFLE(gling128_2);		// 16
REGISTER_JIFFLE(harmonics4);		// 17
REGISTER_JIFFLE(ting1024);		// 18
REGISTER_JIFFLE(ting4096);		// 19
REGISTER_JIFFLE(arpeggio4096);		// 20
REGISTER_JIFFLE(arpeggio4096down);	// 21
REGISTER_JIFFLE(arpeggio_cont);		// 22
REGISTER_JIFFLE(arpeggio_and_down);	// 23
REGISTER_JIFFLE(stepping_down);		// 24
REGISTER_JIFFLE(back_to_ground);	// 25
REGISTER_JIFFLE(arpeggio_and_sayling);	// 26
REGISTER_JIFFLE(simple_theme);		// 27
REGISTER_JIFFLE(pentatonic_rising);	// 28
REGISTER_JIFFLE(tingeling4096);		// 29
REGISTER_JIFFLE(ding1024);		// 30
REGISTER_JIFFLE(kalimbaxyl);		// 31
REGISTER_JIFFLE(ting_tick4096);		// 32
REGISTER_JIFFLE(tigg_ding4096);		// 33
REGISTER_JIFFLE(tumtum);		// 34
REGISTER_JIFFLE(piip2048);		// 35
REGISTER_JIFFLE(tanboura);		// 36
REGISTER_JIFFLE(d2_4096);		// 37
REGISTER_JIFFLE(d4_4096);		// 38
REGISTER_JIFFLE(d6_4096);		// 39
REGISTER_JIFFLE(d8_4096);		// 39
REGISTER_JIFFLE(d16_4096);		// 40
REGISTER_JIFFLE(d32_4096);		// 41
REGISTER_JIFFLE(d64_4096);		// 42
REGISTER_JIFFLE(d128_4096);		// 43
REGISTER_JIFFLE(d256_4096);		// 44
REGISTER_JIFFLE(d512_4096);		// 45
REGISTER_JIFFLE(d1024_4096);		// 46
REGISTER_JIFFLE(loong);			// 47
REGISTER_JIFFLE(looong);		// 48
REGISTER_JIFFLE(loooong);		// 49

select_array_in(JIFFLES, d1024_4096);  	// default jiffle


#include "iCODE.h"
init_arr_DB(iCODEs, sizeof(iCODEs), "iCODEs");
REGISTER_iCODE(d1024_4096_LED);		// 0

select_array_in(iCODEs, (unsigned int *) d1024_4096_LED);  	// default iCODE
