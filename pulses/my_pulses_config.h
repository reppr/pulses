/*
  my_pulses_config.h

  this file is for configuration options likely to change often
  put your own configuration here

*/

//#define PRENAME	SoundShip1		// individual name	// better use nvs_PRENAME
/*			0123456789abcdef	// 16 bytes	*/

#define FAMILY_NAME	SoundShip1		// configuration family	// see: pulses_engine_config.h
// /*			0123456789abcdef	// 16 bytes	*/

#define USE_MORSE	// touch morse code is in use :)

#define USE_MPU6050	// MPU-6050 6d accelero/gyro

#define PERIPHERAL_POWER_SWITCH_PIN	12	// == MORSE_OUTPUT_PIN	green LED in many musicBoxes
// TODO: change PERIPHERAL_POWER_SWITCH_PIN as GPIO12 is active during boot process...

#define ESP32_USB_DAC_ONLY

//#define BATTERY_OLED_BOARD	// LiPo battery OLED BOARD
#define HELTEC_OLED_BOARD	// Heltec OLED BOARD


#if defined ESP32
 // DEFINE how many DACs you want to use
 #define USE_DACs		BOARD_has_DACs	// TODO: FIXME: configuration sequence
#endif


// magic musicBox can produce tunings compatible to a=440 tunings only:
#define SOME_METRIC_TUNINGS_ONLY_DEFAULT	true	// fixed pitchs only like E A D G C F B  was: SOME_FIXED_TUNINGS_ONLY
//#define SOME_METRIC_TUNINGS_ONLY_DEFAULT	false;	// free pitch tuning

//#define NVS_PUT_PRENAME_HACK	LillyBoy	// 1 shot HACK to put a PRENAME		TODO: UI
//#define NVS_PUT_PRENAME_HACK	CHATEAU		// 1 shot HACK to put a PRENAME		TODO: UI
//#define NVS_PUT_PRENAME_HACK	FitNess		// 1 shot HACK to put a PRENAME		TODO: UI
//#define NVS_PUT_PRENAME_HACK	LillyBoy_	// 1 shot HACK to put a PRENAME		TODO: UI