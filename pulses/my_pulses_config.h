/*
  my_pulses_config.h
*/


#define BATTERY_OLED_BOARD	// LiPo battery OLED BOARD	// TODO: move to a configuration file
//#define HELTEC_OLED_BOARD	// Heltec OLED BOARD		// FIXME: display_basic_musicBox_parameters() *DOES NOT WORK*

#if defined ESP32
 // DEFINE how many DACs you want to use
 #define USE_DACs		BOARD_has_DACs	// TODO: FIXME: configuration sequence
#endif


// magic musicBox can produce tunings compatible to a=440 tunings only:
#define SOME_METRIC_TUNINGS_ONLY_DEFAULT	true	// fixed pitchs only like E A D G C F B  was: SOME_FIXED_TUNINGS_ONLY
//#define SOME_METRIC_TUNINGS_ONLY_DEFAULT	false;	// free pitch tuning
