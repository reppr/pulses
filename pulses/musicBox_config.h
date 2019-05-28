/*
  musicBox_config.h
*/

#if ! defined MUSICBOX_CONFIG_H

#define RANDOM_PRESET_LOOP

#define LONELY_BUTLER_QUITS	// lonely butler detect SAVETY NET, TODO: will be completely *wrong* in other situations

// PRE DEFINED SETUPS: uncomment *one* (or zero) of the following setups:
#define SETUP_PORTABLE_DAC_ONLY		2019-04
//#define SETUP_BRACHE_TRIGGERED_PRESETs		BRACHE_2019-04
//#define SETUP_BRACHE		BRACHE_2019-03
//#define SETUP_BAHNPARKPLATZ	BahnParkPlatz 2018/2019
//#define SETUP_CHAMBER_ORCHESTRA	The Harmonical Chamber Orchestra 2018-12

// TODO: REMOVE OLDSTYLE_TUNE_AND_LIMIT
// compatibility: use (buggy) old style tuning and lowering mechanism for backwards compatibility reproducing older setups?
//#define OLDSTYLE_TUNE_AND_LIMIT	// use (buggy) old style tuning and lowering mechanism for backwards compatibility

// #define AUTOSTACK_S0_DEFAULT	-4096*32	// a possible default, seems to work ok on (small) speakers, not on vibration speakers
#define AUTOSTACK_S0_DEFAULT	-4096		// TEST: on vibration speakers	TODO: test&trim		TODO: UI?



// SANITY CHECKS:	===>  *DO NOT CHANGE* THE LINES BELOW  <===
#if ! defined AUTOSTACK_S0_DEFAULT
  #define AUTOSTACK_S0_DEFAULT	0
#endif

#define MUSICBOX_CONFIG_H
#endif
