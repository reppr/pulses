#define PROGRAM_VERSION	HARMONICAL v.046   // testing esp32-arduino 2.0.0 rc1
/*			0123456789abcdef   */


/* **************************************************************** */
/*
			pulses.ino

	    http://github.com/reppr/pulses/

Copyright © Robert Epprecht  www.RobertEpprecht.ch   GPLv2


		Please do read README_pulses
*/

/* **************************************************************** */


/*  GNU General Public License GPLv2

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation version 2.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/


/* **************************************************************** */
/* **************************************************************** */
// SOURCE CODE STARTS HERE:
/* **************************************************************** */
#define PULSES_SYSTEM	// softboard i.e. can check if running standalone

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <limits.h>	// just in case

using namespace std;	// ESP8266 needs that

#if defined ESP32
  #include <esp_system.h>
  #include <esp_err.h>
  extern const char* esp_err_to_name(esp_err_t);
#endif


/* **************************************************************** */
// some #define's:

#define ILLEGAL8	(uint8_t) 255
#define ILLEGAL16	(short) 0xffff
#define ILLEGAL32	(int) 0xffffffff

extern void ERROR_ln(const char* text);
/* **************************************************************** */
// configuration sequence:
#include "pulses_engine_config.h"	// pulses engine configuration file, do not change
#include "pulses_systems.h"		// different software systems
#include "pulses_boards.h"		// different boards
#include "musicBox_config.h"		// basic musicBox configuration
#include "my_pulses_config.h"		// your configuration
#include "pulses_project_conf.h"	// predefined special projects like instruments

#include "pulses_sanity_checks.h"	// check some pp macro incompatibilities

#if defined USE_BLUETOOTH_SERIAL_MENU
  #include "BluetoothSerial.h"
  BluetoothSerial BLUEtoothSerial;
#endif

#if defined USE_i2c
  #include <Wire.h>	// TODO: what about fastWire?

  #if defined USE_MCP23017
    #include "MCP23017.h"			// Adafruit, simple test version only
  #endif
#endif // USE_i2c


/* **************** Menu **************** */
class Menu;
/*
  This version definines the menu INPUT routine int men_getchar();
  and the menu OUTPUT streams
  from the *program*
  not inside the Menu class
*/
#include "menu_IO_configuration.h"
#include <Menu.h>

#if defined RAM_IS_SCARE
  #define CB_SIZE	32
#else
  #define CB_SIZE	128
#endif

#if ! defined MENU_OUTSTREAM2
// see: https://stackoverflow.com/questions/11826554/standard-no-op-output-stream
#include <iostream>
class NullBuffer :  public std::streambuf
{
public:
  int overflow(int c) { return c; }
  // streambuf::overflow is the function called when the buffer has to output data to the actual destination of the stream.
  // The NullBuffer class above does nothing when overflow is called so any stream using it will not produce any output.
};

NullBuffer null_buffer;

//#define MENU_OUTSTREAM2		std::ostream null_stream(&null_buffer)

#define MENU_OUTSTREAM2	(Stream &) null_buffer

#endif

Menu MENU(CB_SIZE, 8, &men_getchar, MENU_OUTSTREAM, MENU_OUTSTREAM2);

  #if defined USE_ADS1115_AT_ADDR
    #include "extensions/pulses_ADS1115.h"
  #endif


#if defined ESP32	// do we have a second core?
  #include "simple_do_from_other_core.h"
#endif

/* **************** Pulses **************** */

#include <Pulses.h>
#include "pulses_project_conf.h"
#include "pulses_systems.h"
#include "pulses_boards.h"

#if defined ESP32
  #include "time64bit.h"
#endif

Pulses PULSES(PL_MAX, &MENU);
// MENU and PULSES are defined now

#include <Harmonical.h>		// including early simplifies compiling
Harmonical* HARMONICAL;

/* **************************************************************** */
enum monochrome_type {
  monochrome_type_off=0,
  monochrome_type_heltec,	// chip is probably SSD1309
  monochrome_type_LiPO,		// ditto?
  monochrome_type_LILYGO_T5,	// 2.13" eInk display
  monochrome_type_unknown,
};

enum RTC_types {
  RTC_type_off=0,
  RTC_type_DS1307,
  RTC_type_DS3231,
  RTC_type_unknown,
};

#define RGB_STRINGS_MAX		1	// 8 "possible"

// some HARDWARE must be known early	*can be managed by nvs*
typedef struct pulses_hardware_conf_t {
  // MPU6050
  int16_t accGyro_offsets[6] = {0};	// aX, aY, aZ, gX, gY, gZ offsets
  uint8_t mpu6050_addr=0;		// flag and i2c addr

  // gpio
  uint8_t gpio_pins_cnt=0;		// used GPIO click pins
  //  gpio_pin_t gpio_pins[20]={ILLEGAL8};					// %4
  gpio_pin_t gpio_pins[20]={(gpio_pin_t) 255};					// %4

  // dac
  // uint8_t dac_channel_cnt=0;	// TODO:
  uint8_t DAC1_pin=ILLEGAL8;	// !flag ILLEGAL8 or 25	// ILLEGAL8==!flag or pin  // %4
  uint8_t DAC2_pin=ILLEGAL8;	// !flag ILLEGAL8 or 26	// ILLEGAL8==!flag or pin

  // trigger
  uint8_t musicbox_trigger_pin=ILLEGAL8;	// maybe include MUSICBOX_TRIGGER_BLOCK_SECONDS?

  // battery and peripheral power
  uint8_t battery_level_control_pin=ILLEGAL8;				// %4
  uint8_t periph_power_switch_pin=ILLEGAL8;

  // morse
  uint8_t morse_touch_input_pin=ILLEGAL8;	// TODO: store touch interrupt level
  uint8_t morse_gpio_input_pin=ILLEGAL8;				// %4
  uint8_t morse_output_pin=ILLEGAL8;

  // bluetooth
  uint8_t bluetooth_enable_pin=ILLEGAL8;	// 35

  // monochrome display
  uint8_t monochrome_type = monochrome_type_off;	// flag and monochrome_type
  uint8_t monochrome_reserved=0;					// %4
  /*
  uint8_t display_hw_pin0=ILLEGAL8;	// OLED		TTGO T5_V2.3_2.13 CS 5==SS	// TODO: implement
  uint8_t display_hw_pin1=ILLEGAL8;	// OLED		TTGO T5_V2.3_2.13 DC 17
  uint8_t display_hw_pin2=ILLEGAL8;	// OLED		TTGO T5_V2.3_2.13 RST 16
  uint8_t display_hw_pin3=ILLEGAL8;	// OLED		TTGO T5_V2.3_2.13 BUSY 4	// %4
  uint8_t display_hw_pin4=ILLEGAL8;	// OLED		TTGO T5_V2.3_2.13 MOSI 23
  uint8_t display_hw_pin5=ILLEGAL8;	// OLED		TTGO T5_V2.3_2.13 CLK 18
  uint8_t display_hw_colours=ILLEGAL8;	// OLED   2	TTGO T5_V2.3_2.13 2
  uint16_t display_hw_x_pix=0;								// %4
  uint16_t display_hw_y_pix=0;
  */

  // RTC
  uint8_t RTC_type = RTC_type_off;	// flag and RTC_type
  uint8_t RTC_addr=0;			// *NOT USED* see: RTC_I2C_ADDRESS 0x68    DS3231 and DS1307 use the same address
					// use RTC_I2C_ADDRESS instead of HARDWARE.RTC_addr

  // RGB LED strings
  uint8_t rgb_strings=0;		// flag and rgb led string cnt
  uint8_t rgb_pin[RGB_STRINGS_MAX]={RGB_LED_STRIP_DATA_PIN};		// %4	TODO: FIX ALIGNEMENT
  uint8_t rgb_pixel_cnt[RGB_STRINGS_MAX]={0};				// %4
  uint8_t rgb_led_voltage_type[RGB_STRINGS_MAX]={0};			// %4
  uint8_t rgb_pattern0[RGB_STRINGS_MAX]={0};				// %4

  // MIDI?
#if defined USE_MIDI && defined MIDI_IN_PIN
  uint8_t MIDI_in_pin=MIDI_IN_PIN;					// %4
#else
  uint8_t MIDI_in_pin=ILLEGAL8;						// %4
#endif

#if defined USE_MIDI && defined MIDI_OUT_PIN
  uint8_t MIDI_out_pin=MIDI_OUT_PIN;
#else
  uint8_t MIDI_out_pin=ILLEGAL8;
#endif

  // SD CARD?	// TODO: implement
  // type
  // MOSI  15
  // SCK   14
  // MISO   2
  // CS    13

  // MCP23017 et al
  // type
  // addr
  // ...

  // ADS1115 et al
  // type
  // addr
  // ...

  // CMOS PLL 4046
  // ...

  // other pins
  uint8_t magical_fart_output_pin=ILLEGAL8;	// who knows, maybe?
  uint8_t magical_sense_pin=ILLEGAL8;		// maybe?	i.e. see: magical_fart

  // 8 bytes RESERVED for future use, forward compatibility
  uint8_t esp_now_channel=ILLEGAL8;	// *not used yet*		// %4	TODO: ESP_NOW_CHANNEL?

  uint8_t midi_does_pitch_bend=false;	// *not used yet*
#if defined USE_MIDI
  uint16_t midi_baudrate_div10 = (MIDI_BAUDRATE / 10);
#else
  uint16_t midi_baudrate_div10=0;	// 0: no MIDI	3125: 31250	11520: 115200
#endif
  /*
	0	(MIDI not used)
	120	1200
	240	2400
	480	4800
	960	9600
	1920	19200
	3125	31250	<<< MIDI
	3840	38400
	5760	57600
	11520	115200	<<< UART default
	23040	230400
	25000	250000
	50000	500000
	// 111111	1000000
  */
  uint8_t reserved4=ILLEGAL8;						// %4
  uint8_t reserved5=ILLEGAL8;
  uint8_t reserved6=ILLEGAL8;
  uint8_t reserved7=ILLEGAL8;

  // other other
  uint8_t tone_pin=ILLEGAL8; // from very old code, could be recycled?	// %4

  // version
  uint8_t version = 0;		// 0 means in development

//// nvs read flags		// do we need|want them?
//  bool read_from_nvs=false;	// set if *anything* was read	do we need|want that?
//  bool gpios_from_nvs=false;	// do we need|want that?

} pulses_hardware_conf_t;

pulses_hardware_conf_t HARDWARE;	// hardware of this instrument


// peer_ID_t
typedef struct peer_ID_t {
  uint8_t mac_addr[6]={0};
  char preName[16]={0};
  uint8_t esp_now_time_slice=ILLEGAL8;	// react on broadcast or all-known-peers messages in an individual time slice
  uint8_t version=0;
} peer_ID_t;

void show_peer_id(peer_ID_t* this_peer_ID_p) {	// TODO: move?
  MENU.out(F("IDENTITY\tpreName "));
  MENU.out_IstrI(this_peer_ID_p->preName);

  extern char* MAC_str(const uint8_t* mac);
  MENU.out(F("\tMAC "));
  MENU.out(MAC_str(this_peer_ID_p->mac_addr));

  MENU.out(F("\ttime slice  "));
  MENU.out((int) this_peer_ID_p->esp_now_time_slice);

  MENU.out(F("\tversion\t"));
  MENU.outln((int) this_peer_ID_p->version);
} // show_peer_id()


// #if defined USE_ESP_NOW || defined USE_NVS	// proves easier to compile that always
  peer_ID_t my_IDENTITY;

  // MAC as string
  char* MAC_str(const uint8_t* mac) {		// TODO: TEST: mac==NULL case
    if(mac == NULL)
      return (char*) ">*< ALL KNOWN >*<";	// for esp_now
    // else

    static char MACstr[18];
    snprintf(MACstr, sizeof(MACstr), "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    return MACstr;
  } // MAC_str(const uint8_t* mac)

  uint8_t my_MAC[] = {0,0,0,0,0,0};
// #endif // defined USE_ESP_NOW || defined USE_NVS


void ERROR_ln(const char* text) {	// extended error reporting on MENU, ePaper or OLED, etc
  MENU.error_ln(text);
#if defined HAS_DISPLAY
  uint16_t len = strlen(text) + 5;	// "ERR " + text + '\0'
  char* str = (char*) malloc(len);
  if(str == NULL) {	// not even space for error message :(
    MENU.error_ln(F("malloc()"));
    return;		// we better return, heap is empty...
  } // else
  snprintf(str, len, F("ERR %s"), text);
  extern void MC_display_message(const char*);
  MC_display_message(str);
  free(str);
#endif
  // ERROR LED?		// TODO: maybe
  // morse output?	// TODO: maybe
  // save to spiffs	// TODO: maybe
} // ERROR_ln()

#if defined USE_ESP_NOW
  #include "esp_now_pulses.h"	// needs pulses_hardware_conf_t  etc
#endif


/* **************************************************************** */
typedef struct musicBox_conf_t {
  const uint8_t version = 0;	// 0 means currently under development
  const uint8_t subversion=0;
  short reserved2=0;

  char* name=NULL;		// name of a piece, a preset
  char* date=NULL;		// date  of a piece, preset or whatever

  unsigned int* scale=NULL;
  unsigned int* jiffle=NULL;
  unsigned int* iCode=NULL;

  Harmonical::fraction_t pitch = {1,1};

  uint16_t steps_in_octave=0;	// like  '5' for pentatonic  '7' for heptatonic aka diatonic scales
  short reserved34=0;

  int sync=1;			// old default, seems still ok ;)

  short preset=0;

  short lowest_primary=ILLEGAL16;
  short highest_primary=ILLEGAL16;
  short primary_count=0;	// TODO:		################ voices vs primary_count ???
  short voices=0;		// TODO: not used yet	################ voices vs primary_count ???
  short bass_pulses=0;		// see  setup_bass_middle_high()
  short middle_pulses=0;	// see  setup_bass_middle_high()
  short high_pulses=0;		// see  setup_bass_middle_high()

  short base_pulse=ILLEGAL16;

  unsigned short cycle_slices = 540;	// set slice_tick_period accordingly

  short stack_sync_slices=0;	// 0 is off	positive: upwards,  negative: downwards
  short subcycle_octave=0;

  uint8_t chromatic_pitch = 0;	// 0: pitch is not metric	set *only* by  set_metric_pitch()
  /*  chromatic_pitch:
    0  *not* metric
    1  a
    2  a#|bb
    3  b	=german "h"
    4  c
    5  c#|db
    6  d
    7  d#|eb
    8  e
    9  f
    10 f#|gb
    11 g
    12 g#|ab
    13 u		/ harmonical time unit
  */

} musicBox_conf_t;

musicBox_conf_t musicBoxConf;


/* **************************************************************** */
// misc

#include <ctype.h>
void copy_string_to_lower(char* source, char* destination, size_t max) {
  char c=1;	// dummy
  int i;
  for(i=0; c && i<max; i++)
    c = destination[i] = tolower(source[i]);

  destination[max] = 0;	// savety belt
}

/* **************************************************************** */
#include "iCODE.h"			// icode programs
#include "jiffles.h"			// old style jiffles (could also be used as icode)
#include "int_edit.h"			// displaying and editing unsigned int arrays
#include "array_descriptors.h"		// make data arrays accessible for the menu, give names to the data arrays


#if defined HAS_OLED
  #include <U8x8lib.h>
  U8X8_SSD1306_128X64_NONAME_SW_I2C* u8x8_p;
#endif

#if defined HAS_ePaper
  #include "ePaper_GxEPD2.h"
  #include "ePaper_pulses.h"
#endif

#if defined USE_MIDI
  action_flags_t selected_actions = DACsq1 | DACsq2 | sendMIDI;	// TODO: better default actions
#else
  action_flags_t selected_actions = DACsq1 | DACsq2;	// TODO: better default actions
#endif

/* **************************************************************** */
// define gpio_pin_t gpio_pins[GPIO_PINS]	// see: pulses_boards.h
#if defined NO_GPIO_PINS
  #undef GPIO_PINS
  #define GPIO_PINS	0
#endif

#if defined GPIO_PINS
  #include "pulses_CLICK_PIN_configuration.h"	// defines gpio_pins[]	// move to configuration sequence?
#endif

/* **************************************************************** */
#ifdef ESP8266	// hope it works on all ESP8266 boards, FIXME: test
  // to switch WiFi off I evaluate different methods:
  // activate *one* of these methods:
  #define WIFI_OFF_hackster
  //#define WIFI_OFF_mysensors

  #ifdef WIFI_OFF_hackster
    // see: https://www.hackster.io/rayburne/esp8266-turn-off-wifi-reduce-current-big-time-1df8ae

    #define FREQUENCY    80	// valid 80, 160
    //#define FREQUENCY    160	// valid 80, 160

    #include "ESP8266WiFi.h"  // after that min() and max() do not work any more. Say: std::min() std::max() or _min _max
    extern "C" {
      #include "user_interface.h"
    }
  #endif

  #ifdef WIFI_OFF_mysensors
    // see: https://forum.mysensors.org/topic/5120/esp8266-with-wifi-off/3
    #include <ESP8266WiFi.h>  // after that min() and max() do not work any more. Say: std::min() std::max() or _min _max
  #endif
#endif	// ESP8266


/* **************************************************************** */
bool DO_or_maybe_display(unsigned char verbosity_level) { // the flag tells *if* to display
  if (PULSES.check_maybe_do())
    return false;		// no time to display anything...

  return MENU.maybe_display_more(verbosity_level);	// flag depending verbosity and menu input
}


/* **************************************************************** */
// iCode

void display_icode(icode_t* icode_p, size_t len) {
  int item_size = sizeof(icode_p[0]);
  size_t shown=0;
  int i=0;
  MENU.out(F("{ "));
  while (shown < len)
    {
      PULSES.show_icode_mnemonic(icode_p[i++]);
      MENU.out(F(", "));
      shown += item_size;
    }
  MENU.outln('}');
}

bool setup_icode_seeder(int pulse, pulse_time_t period, icode_t* icode_p, action_flags_t dest_action_flags) {
  //  if ((pulse > ILLEGAL32) && (pulse < PL_MAX)) {
  if ((pulse > -1) && (pulse < PL_MAX) && (pulse != ILLEGAL32)) {
    PULSES.pulses[pulse].period = period;
    PULSES.set_icode_p(pulse, icode_p, false);		// icode inactive in seeder
    PULSES.set_payload(pulse, seed_icode_player);
    PULSES.pulses[pulse].dest_action_flags |= dest_action_flags;
    return true;
  }

  return false;
}

void en_icode_seeder_selected(icode_t* icode_p, action_flags_t dest_action_flags) {
  for (int pulse=0; pulse<PL_MAX; pulse++) {
    if (PULSES.pulse_is_selected(pulse)) {
      PULSES.set_icode_p(pulse, icode_p, false);		// icode inactive in seeder
      PULSES.set_payload(pulse, seed_icode_player);
      PULSES.pulses[pulse].dest_action_flags |= dest_action_flags;
    }
  }
  PULSES.fix_global_next();
}

void seed_icode_player(int seeder_pulse) {	// as payload for seeder
  int dest_pulse = PULSES.setup_pulse(NULL, ACTIVE, PULSES.pulses[seeder_pulse].next, PULSES.pulses[seeder_pulse].period);
  if (dest_pulse != ILLEGAL32) {
    /*
      pulses[pulse].icode_p	icode start pointer
      pulses[pulse].icode_index	icode index
      pulses[pulse].countdown	count down
      pulses[pulse].base_period	base period = period of starting pulse
      pulses[pulse].gpio	maybe click pin
    */
    PULSES.set_icode_p(dest_pulse, PULSES.pulses[seeder_pulse].icode_p, true);	// set (and activate) icode
    PULSES.pulses[dest_pulse].base_period = PULSES.pulses[seeder_pulse].period;
    PULSES.pulses[dest_pulse].other_pulse = seeder_pulse;
    PULSES.pulses[dest_pulse].action_flags = PULSES.pulses[seeder_pulse].dest_action_flags;
    PULSES.pulses[dest_pulse].groups |= g_SECONDARY;
    if(PULSES.pulses[seeder_pulse].flags & HAS_GPIO)		// if the seeder has gpio,
      PULSES.set_gpio(dest_pulse, PULSES.pulses[seeder_pulse].gpio);	// copy gpio
    if(PULSES.pulses[seeder_pulse].dac1_intensity)		// if the seeder has dac1_intensity
      PULSES.pulses[dest_pulse].dac1_intensity = PULSES.pulses[seeder_pulse].dac1_intensity; // set intensity
    if(PULSES.pulses[seeder_pulse].dac2_intensity)		// if the seeder has dac2_intensity
      PULSES.pulses[dest_pulse].dac2_intensity = PULSES.pulses[seeder_pulse].dac2_intensity; // set intensity

#if defined USE_i2c
  #if defined USE_MCP23017
      if(PULSES.pulses[seeder_pulse].flags & HAS_I2C_ADDR_PIN)	// if the seeder has i2c_addr and i2c_pin
	PULSES.set_i2c_addr_pin(dest_pulse, PULSES.pulses[seeder_pulse].i2c_addr, PULSES.pulses[seeder_pulse].i2c_pin);
  #endif
#endif
  }
} // seed_icode_player()

/* **************************************************************** */
// scales

#ifndef SCALES_RAM_SIZE	// scales on small harware ressources, FIXME: test	################
  #define SCALES_RAM_SIZE 9*2+2	// small buffer might fit on simple hardware
#endif

#ifdef SCALES_RAM_SIZE
  // scales:
  unsigned int scale_RAM[SCALES_RAM_SIZE] = {0};
  unsigned int scale_RAM_length = SCALES_RAM_SIZE;
  unsigned int scale_write_index=0;
//  unsigned int *scale=scale_RAM;	// OBSOLETE? see: selected_in(SCALES)
#else
  #error SCALES_RAM_SIZE is not defined
#endif // SCALES_RAM_SIZE


// scaletabs *MUST* have 2 trailing zeros

unsigned int octaves[] = {1,1, 0,0};				// zero terminated
unsigned int octaves_fifths[] = {1,1, 2,3, 0,0};			// zero terminated
unsigned int octaves_fourths[] = {1,1, 3,4, 0,0};		// zero terminated
unsigned int octave_4th_5th[] = {1,1, 3,4, 2,3, 0,0};	// zero terminated

unsigned int scale_int[]  = {1,1, 2,1, 3,1, 4,1, 5,1, 6,1, 7,1, 8,1, 0,0};  // zero terminated
unsigned int overnotes[]  = {1,1, 1,2, 1,3, 1,4, 1,5, 1,6, 1,7, 1,8, 1,9, 1,10, 1,11, 1,12, 1,13, 1,14, 1,15, 1,16, 0,0};
unsigned int scale_rationals[]  = {1,1, 1,2, 2,3, 3,4, 5,6, 6,7, 7,8, 8,9, 9,10, 0,0};  // zero terminated

unsigned int europ_PENTAtonic[] = {1,1, 8,9, 4,5, 2,3, 3,5, 0,0};	// scale each octave	zero terminated

unsigned int pentatonic_minor[] = {1,1, 5,6, 3,4, 2,3, 5*2,6*3, 0,0};	// scale each octave	zero terminated
// nice first try with "wrong" note:
//  unsigned int mimic_japan_pentatonic[] = {1,1, 8,9, 5,6, 2,3, 8*2,9*3, 1,2, 8,9*2, 5,12, 2,6, 8,9*3, 1,4, 0,0 };  // zero terminated
// second try:
unsigned int mimic_japan_pentatonic[] = {1,1, 8,9, 5,6, 2,3, 2*15,3*16, 0,0 };	// scale each octave	zero terminated


unsigned int minor_scale[] = {1,1, 8,9, 5,6, 3,4, 2,3, 5,8, 5,9, 0,0};	// scale each octave	zero terminated
// 1/1	9/8	6/5	4/3	3/2	8/5	9/5	2/1

// for doric_scale i try minor scale with major scale's sixth:
unsigned int doric_scale[] = {1,1, 8,9, 5,6, 3,4, 2,3, 3,5, 5,9, 0,0};	// scale each octave	zero terminated
// 1/1	9/8	6/5	4/3	3/2	8/5	9/5	2/1


unsigned int major_scale[] = {1,1, 8,9, 4,5, 3,4, 2,3, 3,5, 8,15, 0,0};	// scale each octave	zero terminated
// 24	27	30	32	36	40	45	48
// 1:1	9:8	5:4	4:3	3:2	5:3	15:8	2:1

unsigned int tetraCHORD[] = {1,1, 8,9, 4,5, 3,4, 0,0};		// scale major tetraCHORD
unsigned int tetrachord[] = {1,1, 8,9, 5,6, 3,4, 0,0};		// scale minor tetrachord
unsigned int pentaCHORD[] = {1,1, 8,9, 4,5, 3,4, 2,3, 0,0};	// major
unsigned int pentachord[] = {1,1, 8,9, 5,6, 3,4, 2,3, 0,0};	// minor

unsigned int trichord[] = {1,1, 8,9, 4,5, 0,0};			// major

unsigned int TRIAD[] ={1,1, 4,5, 2,3, 0,0};
unsigned int triad[] ={1,1, 5,6, 2,3, 0,0};


// cycles_conf_t
typedef struct cycles_conf_t {
  pulse_time_t harmonical_CYCLE;	// TODO: move to Harmonical?
  pulse_time_t used_subcycle;		// TODO: move to Harmonical?

  // Harmonical::fraction_t harmonical_cycle_fraction={1, 1}; // TODO: what for ?

  short subcycle_octave=0;

} cycles_conf_t;

cycles_conf_t CyclesConf;




//#define SCALE2CYCLE_INFO	// for debugging, but interesting to watch anyway ;)
pulse_time_t scale2harmonical_cycle(unsigned int* scale, pulse_time_t* duration) {		// returns harmonical cycle of a scale
  Harmonical::fraction_t f_LCM;
  f_LCM.multiplier = 1;
  f_LCM.divisor = 1;

  Harmonical::fraction_t f_F2;

  for(int i=0; scale[i]; i+=2) {
    f_F2.multiplier = selected_in(SCALES)[i];
    f_F2.divisor = selected_in(SCALES)[i+1];
#if defined SCALE2CYCLE_INFO
    display_fraction(&f_F2);
    MENU.tab();
#endif

    (*HARMONICAL).fraction_LCM(&f_F2, &f_LCM);

#if defined SCALE2CYCLE_INFO
    display_fraction(&f_LCM);
      MENU.ln();
#endif
  }

  PULSES.mul_time(duration, f_LCM.multiplier);
  PULSES.div_time(duration, f_LCM.divisor);

  return *duration;
} // scale2harmonical_cycle()


/* **************************************************************** */

// editing jiffle data
// if we have enough RAM, we work in an int array[]
// pre defined jiffletabs can be copied there before using and editing
#ifndef CODE_RAM_SIZE
  // jiffletabs *MUST* have 2 trailing zeros
  #define CODE_RAM_SIZE 9*3+2	// small buffer might fit on simple hardware
#endif
unsigned int code_RAM[CODE_RAM_SIZE] = {0};
unsigned int jiffle_write_index=0;
unsigned int jiffle_range_bottom=0;
unsigned int jiffle_range_top=0;


void fix_jiffle_range() {	// FIXME: use new implementation
  unsigned int i;

  if (jiffle_range_top >= CODE_RAM_SIZE)
    jiffle_range_top=CODE_RAM_SIZE-1;

  if (jiffle_range_bottom > jiffle_range_top) {		// swap
    i = jiffle_range_bottom;
    jiffle_range_bottom = jiffle_range_top;
    jiffle_range_top = i;
  }
};


void test_code(unsigned int* code, char* name, int count) {	// works for iCode and jiffles
  if(code==NULL || (code[0]==0 && code[1]==0))
    return;	// invalid

  int pulse;
  if((pulse = PULSES.highest_available_pulse()) == ILLEGAL32)
    return;	// no free pulse

  if(MENU.verbosity >= VERBOSITY_LOWEST)
    if(name)
      MENU.outln(name);

  pulse_time_t period = PULSES.simple_time(6000000);	// TODO: better default based on the situation ################
  setup_icode_seeder(pulse, period, (icode_t*) code, DACsq1 | DACsq2 | doesICODE);
  PULSES.pulses[pulse].flags |= COUNTED;
  PULSES.pulses[pulse].remaining = count;
  PULSES.pulses[pulse].dac1_intensity = PULSES.pulses[pulse].dac2_intensity = 20; // TODO: random test value
  PULSES.activate_pulse_synced(pulse, PULSES.get_now(), 0 /* or abs(musicBoxConf.sync) */); // test with sync=0
  PULSES.fix_global_next();
} // test_code()


/* **************************************************************** */
/*
  void display_names_OBSOLETE(char** names, int count, int selected);
  display an array of strings like names of scales, jiffles, experiments
  mark the selected one with an asterisk
*/
void display_names_OBSOLETE(const char** names, int count, int selected) {	// TODO: maybe obsolete?  (only for experiment names)
  MENU.ln();
  for (int i = 0; i < count; i++) {
    if (i==selected)
      MENU.out('*');
    else
      MENU.space();
    MENU.space();

    MENU.out(i);
    MENU.tab(); MENU.outln(names[i]);
  }
  MENU.ln();
}


#if GPIO_PINS > 0
/*	TODO: move to library Pulses
  gpio_pin_t next_gpio()	return next unused GPIO click pin (or ILLEGAL32)
  gpio_pin_t next_gpio(index)	*reset* to give gpio_pins[index] on next call
*/
gpio_pin_t next_gpio(gpio_pin_t set_i=ILLEGAL8) {
  static gpio_pin_t next_free_gpio_i=0;

  gpio_pin_t ret=ILLEGAL8;

  if (set_i == ILLEGAL8) {	// normal use: next_gpio()  *get next unused gpio pin**
    if(next_free_gpio_i < GPIO_PINS)
      ret = gpio_pins[next_free_gpio_i];	// return next free pin
    else
      if (MENU.verbosity > VERBOSITY_LOWEST)
	MENU.outln(F("no free GPIO"));
    next_free_gpio_i++;				// count pins, available or not
  } else {		// next_gpio(set_i != ILLEGAL8)   *reset* to return 'set_i' on next next_gpio() call
    if ((set_i < GPIO_PINS) && (set_i >= 0)) {
      next_free_gpio_i = set_i;
      ret = gpio_pins[next_free_gpio_i];	// rarely used, same as next call to next_gpio()
    } else {	// should not happen!
      MENU.out(F("ERROR: GPIO "));
      MENU.outln_invalid();
    }
  }
  return ret;
} // next_gpio()


gpio_pin_t this_or_next_gpio(int pulse) {
  if(pulse == ILLEGAL32) {
    ERROR_ln(F("this_or_next_gpio(ILLEGAL)"));
    return ILLEGAL8;
  }
  if (PULSES.pulses[pulse].flags & HAS_GPIO)	// if a pulse already has gpio, return it
    return PULSES.pulses[pulse].gpio;

  return next_gpio();				// else return  next_gpio()  or ILLEGAL8
}

#else	// NO_GPIO_PINS
  gpio_pin_t next_gpio(gpio_pin_t set_i=-1) {		// dummy: NO_GPIO_PINS
    return (gpio_pin_t) ILLEGAL8;
  }

  gpio_pin_t this_or_next_gpio(gpio_pin_t set_i=-1) {	// dummy: NO_GPIO_PINS
    return (gpio_pin_t) ILLEGAL8;
  }
#endif // GPIO_PINS


// reset, remove all (flagged) pulses, restart selections at none
// switch GPIO and DACs off
int reset_all_flagged_pulses_GPIO_OFF() {	  // see: tabula_rasa()
  /*
    reset pulses, switches GPIO and DACs off
    MCP23017_OUT_LOW();
  */
  int cnt=0;
  for (int pulse=0; pulse<PL_MAX; pulse++) {
    if (PULSES.pulses[pulse].flags) {
      PULSES.init_pulse(pulse);
      cnt++;
    }
  }

#if defined USE_MIDI
  extern void midi_all_notes_off();
  midi_all_notes_off();		// TODO: test and fix
#endif

#if defined USE_MORSE
  extern int morse_length_feedback_pulse_i;
  morse_length_feedback_pulse_i = ILLEGAL32;
#endif

#if GPIO_PINS > 0
  init_click_GPIOs_OutLow();		// switch them on LOW, output	current off, i.e. magnets
#endif

  PULSES.clear_selection();		// restart selections at none

#if defined LEDC_INSTEAD_OF_DACs	// use LEDC instead of DACs?	// TODO: shut off LEDC	################
//  extern void pulses_ledc_write(uint8_t, uint32_t);
//  pulses_ledc_write(1, 0);		// LEDC channel1 off
//  pulses_ledc_write(2, 0);		// LEDC channel2 off
////  ledc_set_duty(LEDC_HIGH_SPEED_MODE, 1, 0);
////  ledc_update_duty(LEDC_HIGH_SPEED_MODE, 1);
////  ledc_set_duty(LEDC_HIGH_SPEED_MODE, 2, 0);
////  ledc_update_duty(LEDC_HIGH_SPEED_MODE, 2);

#else // use esp32 genuine DACs?
  #if defined USE_DACs			// reset DACs
    dacWrite(BOARD_DAC1, 0);
    #if (USE_DACs > 1)
      dacWrite(BOARD_DAC2, 0);
    #endif
  #endif
#endif

#if defined USE_i2c
  #if defined USE_MCP23017
    MCP23017_OUT_LOW();
  #endif
#endif

  PULSES.fix_global_next();
  next_gpio(0);				// reset used gpio
  PULSES.hex_input_mask_index = 0;	// for convenience

  if (MENU.verbosity > VERBOSITY_LOWEST) {
    MENU.out(F("removed all pulses "));
    MENU.out(cnt);
    MENU.out(F("  switched pins off"));
    MENU.out(F("  freed GPIOs"));
#if defined USE_i2c
  #if defined USE_MCP23017
    MENU.out(F("  MCP23017_OUT_LOW();"));
  #endif
#endif
    MENU.ln();
  } else if(MENU.verbosity && cnt) {
    MENU.out(F("freed GPIOs "));
    MENU.outln(cnt);
  }

  return cnt;
};


// make an existing pulse an old style click pulse:
bool en_click(int pulse, gpio_pin_t pin) {
  if ((pulse > -1) && (pulse < PL_MAX) && (pulse != ILLEGAL32)) {
    PULSES.set_payload_with_pin(pulse, &click, pin);
    return true;
  }

  return false;
};


// make selected pulses click
int en_click_selected() {
  int cnt=0;

  for (int pulse=0; pulse<PL_MAX; pulse++)
    if (PULSES.pulse_is_selected(pulse))
      if (en_click(pulse, this_or_next_gpio(pulse)))
	cnt++;

  if (DO_or_maybe_display(VERBOSITY_MORE)) {
    MENU.ln();
    selected_or_flagged_pulses_info_lines();
  }

  return cnt;
};

void show_GPIOs() {
  MENU.out(GPIO_PINS);
  MENU.out(F(" GPIO pins\t{"));
#if GPIO_PINS > 0	// *does* work for GPIO_PINS==0
  for(int i=0; i < GPIO_PINS; i++) {
    MENU.out(gpio_pins[i]);
    MENU.out_comma_();
  }
#endif //  GPIO_PINS
  MENU.outln('}');
}

#if defined USE_DACs	// TODO: move to library Pulses
// set_action_flags(pulse, DACsq1 | DACsq2) activates both DACs
bool set_action_flags(int pulse, unsigned int action_flags) {
  if ((pulse > -1) && (pulse < PL_MAX) && (pulse != ILLEGAL32)) {
    PULSES.pulses[pulse].action_flags |= action_flags;
    return true;
  }

  return false;
};

// make selected pulses produce square waves with harmonical timing

int selected_set_action_flags(unsigned int action_flags) {
  int cnt=0;

  for (int pulse=0; pulse<PL_MAX; pulse++)
    if (PULSES.pulse_is_selected(pulse))
      if (set_action_flags(pulse, action_flags))
	cnt++;

  return cnt;
};

// share DAC intensity between selected pulses
int selected_share_DACsq_intensity(int intensity, int channel) {
  int cnt=0;

  for (int pulse=0; pulse<PL_MAX; pulse++)
    if (PULSES.pulse_is_selected(pulse))
      cnt++;

  if (cnt) {
    intensity /= cnt;
    for (int pulse=0; pulse<PL_MAX; pulse++) {
      if (PULSES.pulse_is_selected(pulse)) {
	switch (channel) {
	case 1:
	  PULSES.pulses[pulse].dac1_intensity=intensity;
	  break;
	case 2:
	  PULSES.pulses[pulse].dac2_intensity=intensity;
	  break;
	}
      }
    }
  } else
    intensity=0;

  return intensity;
};

// share DAC intensity of selected pulses, proportional to period
void selected_DACsq_intensity_proportional(int intensity, int channel) {
  pulse_time_t sum;
#if defined PULSES_USE_DOUBLE_TIMES	// sorry, some of the worst code ever written...
  sum=0.0;				// transition to new double time type, preseving both for some time...
#else // old int overflow style
  sum.time=0;
  sum.overflow=0;
#endif
  double factor;

  for (int pulse=0; pulse<PL_MAX; pulse++)
    if (PULSES.pulse_is_selected(pulse))
      PULSES.add_time(&PULSES.pulses[pulse].period, &sum);

#if defined PULSES_USE_DOUBLE_TIMES
  bool there_is_something = (sum != 0.0);
#else // old int overflow style
  if (sum.overflow)	// FIXME: if ever needed ;)
    ERROR_ln(F("sum.overflow"));
  bool there_is_something = sum.time;
#endif // old int overflow style

  if (there_is_something) {
    for (int pulse=0; pulse<PL_MAX; pulse++) {
      if (PULSES.pulse_is_selected(pulse)) {
#if defined PULSES_USE_DOUBLE_TIMES
	factor = PULSES.pulses[pulse].period / sum;
#else // old int overflow style
	factor = (double) PULSES.pulses[pulse].period.time / (double) sum.time;
#endif
	switch (channel) {
	case 1:
	  PULSES.pulses[pulse].dac1_intensity = factor * intensity;
	  break;
	case 2:
	  PULSES.pulses[pulse].dac2_intensity = factor * intensity;
	  break;
	}
      }
    }
  }
} // selected_DACsq_intensity_proportional()
#endif // USE_DACs	// TODO: move to library Pulses



/* **************************************************************** */
// user interface variables:

int selected_experiment=-1;
int voices=GPIO_PINS;

bool scale_user_selected=false;
bool jiffle_user_selected=false;
bool pitch_user_selected=false;	// TODO: not really implemented yet
bool octave_user_selected=false;
bool icode_user_selected=false;
bool sync_user_selected=false;
bool stack_sync_user_selected=false;

#ifdef IMPLEMENT_TUNING		// implies floating point
  #include <math.h>

// first try, see sweep_click_0()
/* tuning *= detune;
  called detune_number times
  will rise tuning by an octave	*/
  int sweep_up=0;	// sweep_up==0 no sweep, 1 up, -1 down
  double tuning=1.0;
  double detune_number=4096.0;
  double detune=1.0 / pow(2.0, 1/detune_number);

  // second try, see sweep_click()
  // unsigned long ticks_per_octave=10000000L;		// 10 seconds/octave
  // unsigned long ticks_per_octave=60000000L;		//  1 minute/octave
  // unsigned long ticks_per_octave=60000000L*10L;	// 10 minutes/octave
  unsigned long ticks_per_octave=60000000L*60UL;	//  1 hour/octave
  // unsigned long ticks_per_octave=60000000L*60L*24;	//  1 day/octave  FIXME: overflows

// re-implement, see tuned_sweep_click()
// PULSES.ticks_per_octave = ticks_per_octave;	// do that in setup
//

#endif


#ifdef USE_INPUTS
  #include <Inputs.h>
  Inputs INPUTS(4);

  bool maybe_check_inputs() {
    static unsigned long maybe_check_inputs_cnt=0;
    static unsigned long input_priority_part=3;	// check inputs every 3rd time, priority slice

    if(INPUTS.active_inputs==0)			// something active?
      return false;

    if (++maybe_check_inputs_cnt % input_priority_part == 0 ) {	// an input's turn?
      INPUTS.do_next_input();
      return true;				// at least a sample was taken, maybe more actions
    }

    return false;				// i can wait for my turn ;)
  }
#endif // USE_INPUTS

#if defined USE_MORSE
  #include "morse.h"
#endif

// #if defined USE_LEDC
//   #include "ledc_tone.h"
// #endif

#if defined USE_RTC_MODULE
  #include "RTC_module_DS1307_or_DS3231.h"
#endif

#if defined USE_MPU6050		// MPU-6050 6d accelero/gyro
  #include "mpu6050_module.h"
  #include "extensions/IMU_Zero.h"
#endif

#if defined USE_RGB_LED_STRIP
  #include "extensions/pulses_RGB_LED_string.h"
#endif

#if defined USE_MIDI
  #include "midi.h"
#endif

/* **************************************************************** */
double cent_factor = 1.0005777895;	// 1 cent
void selected_detune_cents(short cents) {
  MENU.out(F("detune_cents " ));
  MENU.outln(cents);
  if(cents == 0)
    return;

  for(int pulse=0; pulse < PL_MAX; pulse++) {
    if(PULSES.pulses[pulse].groups & g_PRIMARY) {
      PULSES.activate_tuning(pulse);
    }
  }

  if(cents>0) // tuning up
    for(short i=0; i<cents; i++)
      PULSES.tuning /= cent_factor;

  else        // tuning down
    for(short i=cents; i<0; i++)
      PULSES.tuning *= cent_factor;

  for(int pulse=0; pulse < PL_MAX; pulse++) {
    if(PULSES.pulses[pulse].groups & g_PRIMARY) {
      PULSES.stop_tuning(pulse);
    }
  }
} // selected_detune_cents()


void selected_do_detune_periods(short cents) {	// works on the period time of each pulse...
  MENU.out(F("detuning periods  cents "));	// just a quick-hack-tuning-interface by MENU input
  MENU.out(cents);				// *will produce a lot of ROUNDING ERRORS*
  if(cents==0) {				// TODO: REPLACE: by a version working on pitch and retune
    MENU.ln();
    return;
  }

  double detune = 1.0;

  if(cents > 0)		// tuning upwards
    for(short i=0; i<cents; i++)
      detune /= cent_factor;
  else			// tuning downwards
    for(short i=cents; i<0; i++)
      detune *= cent_factor;

  MENU.tab();
  MENU.out(F("\tfactor "));
  MENU.out(detune, 6);
  MENU.ln();

  if(detune != 1.0) {
    for(int pulse=0; pulse<PL_MAX; pulse++) {
      if (PULSES.pulse_is_selected(pulse)) {
#if defined PULSES_USE_DOUBLE_TIMES
	PULSES.pulses[pulse].period *= detune;

#else // old int overflow style
	double time_double;
	time_double = PULSES.pulses[pulse].period.time;
	time_double *= detune;
	PULSES.pulses[pulse].period.time = (unsigned long) (time_double + 0.5);
#endif
// WTF	if(pulse==15)
//	  MENU.out(PULSES.pulses[pulse].period.time);
// WTF	if(pulse==15) {
//	  MENU.tab();
//	  MENU.outln(PULSES.pulses[pulse].period.time);
//	}
      }
    }

    PULSES.fix_global_next();
  } // detune != 1.0
} // selected_do_detune_periods()


/* **************************************************************** */
#ifdef ARDUINO
/* Arduino setup() and loop():					*/

// needed for MENU.add_page();
// void softboard_display();
// bool softboard_reaction(char token);
int8_t softboard_page=-1;	// see: maybe_run_continuous()
int8_t musicBox_page=ILLEGAL8;	// NOTE: musicBox_page is not used	// TODO: ???



#ifndef STARTUP_DELAY
  #define STARTUP_DELAY	0	// obsolete, noop or yield()
#endif

#ifndef RAM_IS_SCARE	// enough RAM?
  #include "jiffles.h"
#endif

#include "random_entropy.h"	// TODO: only if used

#if defined PERIPHERAL_POWER_SWITCH_PIN
  #include "peripheral_power_switch.h"
#endif

#if defined USE_BATTERY_LEVEL_CONTROL
  #include "battery_control.h"
#endif

/* ESP32 NVS	*/
#if defined USE_NVS		// always used on ESP32
  #include "nvs_pulses.h"
#endif

#if defined USE_LEDC_AUDIO
  #include "ledc_audio.h"
#else
  // DAx_max maximal data value for digital to analogue system (either hardware DAC or LEDC)
  int DAx_max=255;	// used for esp DAC only (no ledc audio compiled)
#endif

#if defined USE_BLUETOOTH_SERIAL_MENU
  #include "bluetoothSerialMenu.h"
  #include "bluetooth_menu_page.h"	// hi jacking USE_BLUETOOTH_SERIAL_MENU
#endif


// declaring these, even if they are not used, makes compiling easier ;)
// TODO: stress configuration struct stress_conf_t	// DADA
unsigned int stress_emergency=4096*6;	// magical musicBox test	TODO: fine tune, maybe UI
unsigned int stress_event_level=1024;	// continue TESTING:  TODO: fine tune, maybe UI
int stress_event_cnt=0;			// counting stress_event_level events
uint8_t stress_event_cnt_MAX=1;		// if the count reaches MAX stress release needed TODO: fine tune
unsigned int stress_count=0;		// low level stress count
unsigned int relax_level=200;		// see: relax()	 (ILLEGAL32 should switch that off)
uint8_t relaxmax=4;			// up to how many relax() in one todo chain

#if defined DO_STRESS_MANAGMENT
  #include "stress_managment.h"
#endif

#if defined HAS_OLED
  #include "monochrome_display.h"
#endif

#if defined USE_LoRa		// needs: https://github.com/sandeepmistry/arduino-LoRa
  #include "LoRa_pulses.h"	//	  https://github.com/khoih-prog/ESP32TimerInterrupt
  #include "LoRa_menu_page.h"
#endif

#if defined HARMONICAL_MUSIC_BOX
  #include "musicBox.h"
#endif

#if defined USE_SYSTEM_MENU
  #include "system_menu.h"
#endif

#if defined USE_NVS_MENU
  #include "nvs_menu.h"
#endif

#if defined ESP32
  #include "esp_get_infos.h"
#endif


/* **************************************************************** */
void display_program_version() {  // program versions, mac, maybe preName.  MENU output only. see: show_program_version()
  MENU.out(F(STRINGIFY(PROGRAM_VERSION)));
  MENU.tab();
#if defined PROGRAM_SUB_VERSION
  MENU.out(F(STRINGIFY(PROGRAM_SUB_VERSION)));
#endif

#if defined FAMILY_NAME
  MENU.tab();
  MENU.out(F(STRINGIFY(FAMILY_NAME)));
#endif

  if(my_IDENTITY.preName) {
    MENU.tab();
    MENU.out_IstrI(my_IDENTITY.preName);
  }

  MENU.ln();
} // display_program_version()


void show_program_version() {	// program version on menu output *and* OLED
  display_program_version();

#if defined HAS_DISPLAY
  MC_show_program_version();
#endif
}


void setup_initial_HARDWARE_conf() {
#if defined USE_MPU6050
  if(mpu6050_available)
    HARDWARE.mpu6050_addr = 0x68;
#endif

  HARDWARE.gpio_pins_cnt = GPIO_PINS;
  if (GPIO_PINS) {
    for(int i=0; i<GPIO_PINS; i++) {
      extern gpio_pin_t gpio_pins[GPIO_PINS];
      HARDWARE.gpio_pins[i] = gpio_pins[i];
    }
  }

#if defined  USE_DACs
  HARDWARE.DAC1_pin = 25;
  #if (USE_DACs > 1)
  HARDWARE.DAC2_pin = 26;
  #endif
#endif

#if defined MUSICBOX_TRIGGER_PIN
  HARDWARE.musicbox_trigger_pin=MUSICBOX_TRIGGER_PIN;
#endif

#if defined USE_BATTERY_LEVEL_CONTROL
  HARDWARE.battery_level_control_pin=BATTERY_LEVEL_CONTROL_PIN;
#else
  HARDWARE.battery_level_control_pin=ILLEGAL8;
#endif

#if defined PERIPHERAL_POWER_SWITCH_PIN
  HARDWARE.periph_power_switch_pin = PERIPHERAL_POWER_SWITCH_PIN;
#endif

#if defined MORSE_TOUCH_INPUT_PIN
  HARDWARE.morse_touch_input_pin = MORSE_TOUCH_INPUT_PIN;
#endif

#if defined MORSE_GPIO_INPUT_PIN
  HARDWARE.morse_gpio_input_pin = MORSE_GPIO_INPUT_PIN;
#endif

#if defined MORSE_OUTPUT_PIN
  HARDWARE.morse_output_pin = MORSE_OUTPUT_PIN;
#endif

#if defined BLUETOOTH_ENABLE_PIN
  HARDWARE.bluetooth_enable_pin = BLUETOOTH_ENABLE_PIN;		// 35
#endif

#if defined HAS_DISPLAY
  #if defined BOARD_HELTEC_OLED
    HARDWARE.monochrome_type = monochrome_type_heltec;
  #elif defined BOARD_OLED_LIPO
    HARDWARE.monochrome_type = monochrome_type_LiPO;
  #elif defined ePaper213B73_BOARD_LILYGO_T5 || defined ePaper213B74_BOARD_LILYGO_T5
    HARDWARE.monochrome_type = monochrome_type_LILYGO_T5;
  #endif
#endif

#if defined USE_RGB_LED_STRIP
  HARDWARE.rgb_strings = 1;	// I use only one
  HARDWARE.rgb_pin[0] = RGB_LED_STRIP_DATA_PIN;
  HARDWARE.rgb_pixel_cnt[0] = RGB_STRING_LED_CNT;
  #if defined RGB_LED_STRING_VOLTAGE_TYPE
    HARDWARE.rgb_led_voltage_type[0] = RGB_LED_STRING_VOLTAGE_TYPE;
  #endif
#endif

#if defined USE_RTC_MODULE
  HARDWARE.RTC_type = RTC_type_DS1307;		// FIXME: RTC_type_DS3231 ???
#endif

  // other pins		// TODO: implement
  // nvs flags		// ?????	TODO: implement
  // version		// switch(version)

} // setup_initial_HARDWARE_conf()


void show_pin_or_dash(uint8_t pin) {
  if(pin==ILLEGAL8)
    MENU.out('-');
  else
    MENU.out(pin);
}

void show_monochrome_type(int type) {
  MENU.out(F("monochrome\t\t"));
  switch(type) {
  case monochrome_type_off:
    MENU.outln('-');
    break;
  case monochrome_type_heltec:
    MENU.outln(F("heltec"));
    break;
  case monochrome_type_LiPO:
    MENU.outln(F("OLED LiPO"));
    break;
  case monochrome_type_LILYGO_T5:
    MENU.outln(F("LILYGO_T5"));
    break;
  default:
    ERROR_ln(F("monochrome_type unknown"));
  }
} // show_monochrome_type(int type)


void show_hardware_conf(pulses_hardware_conf_t* hardware) {
  MENU.out(F("GPIO click pins\t\t"));
  if(hardware->gpio_pins_cnt) {
    MENU.outln(hardware->gpio_pins_cnt);
    show_GPIOs();
  } else
    MENU.outln('-');

  MENU.out(F("DAC1/DAC2 pins\t\t"));
  show_pin_or_dash(hardware->DAC1_pin);
  MENU.tab();
  show_pin_or_dash(hardware->DAC2_pin);
  MENU.ln();

  MENU.out(F("MPU6050\t\t\t"));
  if(hardware->mpu6050_addr) {
    MENU.out_hex(hardware->mpu6050_addr);
    MENU.ln();
#if defined USE_MPU6050
    extern void show_accGyro_offsets();
    show_accGyro_offsets();
#endif
  } else
    MENU.outln(F("no"));

  MENU.out(F("musicbox_trigger_pin\t"));
  show_pin_or_dash(hardware->musicbox_trigger_pin);
  MENU.ln();

  MENU.out(F("battery level pin\t"));
  show_pin_or_dash(hardware->battery_level_control_pin);
  MENU.ln();

  MENU.out(F("peripheral power switch\t"));
  show_pin_or_dash(hardware->periph_power_switch_pin);
  MENU.ln();

  MENU.out(F("morse_touch_input\t"));
  show_pin_or_dash(hardware->morse_touch_input_pin);
  MENU.ln();

  MENU.out(F("morse_gpio_input\t"));
  show_pin_or_dash(hardware->morse_gpio_input_pin);
  MENU.ln();

  MENU.out(F("morse_output_pin\t"));
  show_pin_or_dash(hardware->morse_output_pin);
  MENU.ln();

  MENU.out(F("bluetooth_enable_pin\t"));
  show_pin_or_dash(hardware->bluetooth_enable_pin);
  MENU.ln();

  show_monochrome_type(hardware->monochrome_type);

  MENU.out(F("RGB LED strings\t\t"));
  if(hardware->rgb_strings) {
    MENU.outln(hardware->rgb_strings);
    for(int i=0; i<hardware->rgb_strings; i++) {
      MENU.out(F(" #"));
      MENU.out(i);

      MENU.out(F("\tpin "));
      MENU.out(hardware->rgb_pin[i]);

      MENU.out(F("\tcnt "));
      MENU.out(hardware->rgb_pixel_cnt[i]);

      MENU.out(F("\tvoltage "));
      MENU.out(hardware->rgb_led_voltage_type[i]);

      MENU.out(F("\tstart "));
      MENU.outln(hardware->rgb_pattern0[i]);
    }

    MENU.out(F("all use voltage type\t"));
    MENU.outln(hardware->rgb_led_voltage_type[0]);
  } else
    MENU.outln('-');

  MENU.out(F("MIDI\t\t\tin "));
  show_pin_or_dash(hardware->MIDI_in_pin);
  MENU.out(F("\tout "));
  show_pin_or_dash(hardware->MIDI_out_pin);
  MENU.ln();

/*
  MENU.outln(F("RTC\t\t\tTODO:"));	// TODO: RTC
*/

  // other pins
  // switch(version)
  // nvs flags
  // LORA?
  MENU.ln();
} // show_hardware_conf()


void show_current_hardware_conf() {	// same, with title, for menu output
  MENU.outln(F("current HARDWARE configuration:"));
  show_hardware_conf(&HARDWARE);
}

bool show_pulses_pin_usage(gpio_pin_t pin) {
  bool retval=false;
  // gpio
  if(HARDWARE.gpio_pins_cnt) {
    for(int i=0; 1 < HARDWARE.gpio_pins_cnt; i++) {
      if(pin == HARDWARE.gpio_pins[i]) {
	MENU.out(F("GPIO "));
	MENU.out(pin);
	MENU.tab(2);
	retval = true;
      }
    }
  }

  // dac
  if(pin == HARDWARE.DAC1_pin) {
    MENU.out(F("DAC1 "));
    MENU.out(pin);
    MENU.tab(2);
    retval = true;
  }
  if(pin == HARDWARE.DAC2_pin) {
    MENU.out(F("DAC2 "));
    MENU.out(pin);
    MENU.tab(2);
    retval = true;
  }

  // morse
  if(pin == HARDWARE.morse_touch_input_pin) {
    MENU.out(F("morse touch "));
    MENU.out(pin);
    MENU.tab();
    retval = true;
  }
  if(pin == HARDWARE.morse_gpio_input_pin) {
    MENU.out(F("morse gpio "));
    MENU.out(pin);
    MENU.tab();
    retval = true;
  }
  if(pin == HARDWARE.morse_output_pin) {
    MENU.out(F("morse out "));
    MENU.out(pin);
    MENU.tab();
    retval = true;
  }

  // rgb led strings
  if(HARDWARE.rgb_strings) {
    for(int i=0; i < RGB_STRINGS_MAX; i++) {
      if(pin == HARDWARE.rgb_pin[i]) {
	MENU.out(F("RGB["));
	MENU.out(i);
	MENU.out(F("] data "));
	MENU.out(pin);
	MENU.tab();
	retval = true;
      }
    }
  }

  // trigger
  if(pin == HARDWARE.musicbox_trigger_pin) {
    MENU.out(F("trigger "));
    MENU.out(pin);
    MENU.tab(2);
    retval = true;
  }

  // peripheral power switch
  if(pin == HARDWARE.periph_power_switch_pin) {
    MENU.out(F("peripheral power "));
    MENU.out(pin);
    MENU.tab();
    retval = true;
  }

  // battery control
  if(pin == HARDWARE.battery_level_control_pin) {
    MENU.out(F("battery level "));
    MENU.out(pin);
    MENU.tab();
    retval = true;
  }

  // bluetooth
  if(pin == HARDWARE.bluetooth_enable_pin) {
    MENU.out(F("bluetooth enable "));
    MENU.out(pin);
    MENU.tab();
    retval = true;
  }

  // MIDI
  if(pin == HARDWARE.MIDI_in_pin) {
    MENU.out(F("MIDI IN "));
    MENU.out(pin);
    MENU.tab(2);
    retval = true;
  }
  if(pin == HARDWARE.MIDI_out_pin) {
    MENU.out(F("MIDI OUT "));
    MENU.out(pin);
    MENU.tab();
    retval = true;
  }

  // other pins
  if(pin == HARDWARE.magical_fart_output_pin) {
    MENU.out(F("magical fart "));
    MENU.out(pin);
    MENU.tab();
    retval = true;
  }

  if(pin == HARDWARE.magical_sense_pin) {
    MENU.out(F("magical sense "));
    MENU.out(pin);
    MENU.tab();
    retval = true;
  }

  if(pin == HARDWARE.tone_pin) {	// from very old code, could be recycled?
    MENU.out(F("tone "));
    MENU.out(pin);
    MENU.tab(2);
    retval = true;
  }

// HCSR04_TRIGGER_PIN
// HCSR04_ECHO_PIN

// LED_PIN
//#if defined ONBOARD_LED
//  #error IMPLEMENT ONBOARD_LED ...
//#endif

  return retval;
} // show_pulses_pin_usage()

void show_esp32_pin_capabilities(gpio_pin_t pin) {
  switch(pin) {
  case 0:
    MENU.out(F("pu, button, (touch1), adc2_1, strap"));	// PWM at boot
    break;
  case 1:					// debug output at boot
    MENU.out(F("TX"));
    break;
  case 2:
    MENU.out(F("pd, (led), touch2, adc2_2, strap"));	// sometimes onboard LED
    break;
  case 3:					// HIGH at boot
    MENU.out(F("RX"));
    break;
  case 4:
    MENU.out(F("pd, touch0, adc2_0, strap"));
    break;
  case 5:					// PWM at boot	(must be high during boot)
    MENU.out(F("pu, strap"));
    break;

  case 6:
  case 7:
  case 8:
  case 9:
  case 10:
  case 11:
    MENU.out(F("FLASH!"));
    break;

  case 12:
    MENU.out(F("touch5, adc2_5, strap"));			// boot fail if pulled high
    break;

  case 13:
    MENU.out(F("touch4, adc2_4, MISO"));			// TODO: strap or not?
    break;
  case 14:
    MENU.out(F("touch6, adc2_6, MOSI"));			// PWM at boot
    break;
  case 15:
    MENU.out(F("pu, touch3, adc2_3, strap"));			// PWM at boot	(must be high during boot)
    break;
  case 16:
    MENU.out(F("RX2, i2s1_WS"));
    break;
  case 17:
    MENU.out(F("TX2, i2s1_WS"));
    break;
  case 18:
    MENU.out(F("spi_MOSI"));
    break;
  case 19:
    MENU.out(F("spi_MISO, 12s_WS"));
    break;
  case 20:
  case 24:
  case 28:
  case 29:
  case 30:
  case 31:
    MENU.out(F("--"));
    break;
  case 21:
    MENU.out(F("i2c SDA i2s"));
    break;
  case 22:
    MENU.out(F("i2c SCL"));
    break;
  case 23:
    MENU.out(F("VSPI MOSI"));
    break;
  case 25:
    MENU.out(F("DAC1 adc2_8"));
    break;
  case 26:
    MENU.out(F("DAC2 adc2_9"));
    break;
  case 27:
    MENU.out(F("touch2_7"));
    break;
  case 32:
    MENU.out(F("touch9, adc4"));
    break;
  case 33:
    MENU.out(F("touch8, adc5"));
    break;
  case 34:
    MENU.out(F("INPUT, adc6"));
    break;
  case 35:
    MENU.out(F("INPUT, adc7"));
    break;
  case 36:
    MENU.out(F("INPUT, adc0, SVP"));
    break;
  case 37:
    MENU.out(F("INPUT, adc1"));
    break;
  case 38:
    MENU.out(F("INPUT, adc2"));
    break;
  case 39:
    MENU.out(F("INPUT, adc3, SVN"));
    break;

  default:
    ERROR_ln(F("no pin data"));
  }
} // esp32_pin_capabilities()

void pin_usage_title_lines() {
  MENU.outln(F("PULSES pin usage:"));
  MENU.out(F("pin\t\tPULSES"));
#if defined ESP32
  MENU.out(F("\t\tESP32"));
#endif
  if(MENU.verbosity <= VERBOSITY_LOWEST)
    MENU.out(F("\tmore info with '+'"));

  MENU.ln();
} // pin_usage_title_lines()

void show_pulses_all_pins_usage() {
  pin_usage_title_lines();

  for(uint8_t i=0; i < 40; i++) {
    MENU.out(F("  pin "));
    MENU.out(i);
    MENU.space();
    MENU.tab();

    show_pulses_pin_usage(i);	// +++
#if defined ESP32
    show_esp32_pin_capabilities(i);
#endif
    MENU.ln();
  }
  MENU.ln();
} // show_pulses_all_pins_usage()


void show_internal_configurations() {
  show_program_version();	// prename now known
//  #if defined HAS_DISPLAY
//    delay(1200);	// sorry for that
//  #endif

  MENU.ln();

  {
    int core;
    MENU.out(F("pulses\trunning on core "));
    MENU.out(core = xPortGetCoreID());
    if(core == 1)
      MENU.ln();
    else
      ERROR_ln(F("unexpected core"));
  }

  MENU.out(F("\tportTICK_PERIOD_MS "));
  MENU.outln(portTICK_PERIOD_MS);

#if defined MULTICORE_DISPLAY
  MENU.outln(F("\tdisplays from other core"));
#endif

#if defined MULTICORE_RGB_STRING
  MENU.outln(F("\tdrives rgb from other core"));
#endif

#if defined MULTICORE_MPU_SAMPLING  &&  defined USE_MPU6050
  MENU.outln(F("\tmpu sampling from other core"));
#endif

#if defined PULSES_USE_DOUBLE_TIMES
  MENU.outln(F("\tuses DOUBLE times"));
#else
  MENU.outln(F("\tuses int overflow times"));
#endif

#if defined ESP32
  #if defined USE_ESP_NOW
    MENU.outln(F("\tuses ESP_NOW"));
  #else
    MENU.outln(F("\tesp_now *not* used"));
  #endif

  #if defined USE_BLUETOOTH_SERIAL_MENU
    MENU.outln(F("\tuses BT Serial"));
  #endif

  MENU.ln();
  display_esp_versions();

  MENU.ln();
  MENU.out(F("MAC: "));
  MENU.outln(getMacAddress());
  MENU.ln();
#endif
} // show_internal_configurations()


int autostart_counter=0;	// can be used to change AUTOSTART i.e. for the very first one

#if defined FORCE_START_TO_USERMODE
  bool force_start_to_usermode=true;
#else
  bool force_start_to_usermode=false;
#endif

void setup() {
  setup_initial_HARDWARE_conf();

#if defined USE_RGB_LED_STRIP
  pulses_RGB_LED_string_init();	// DO THAT EARLY to switch led string off after booting
#endif

#if defined RANDOM_ENTROPY_H	// *one* call would be enough, getting crazy on it ;)
  random_entropy();	// start gathering entropy before initialisation
#endif

  HARMONICAL = new Harmonical(3628800uL);	// old style harmonical unit, obsolete?

  delay(STARTUP_DELAY);		// yield()
  Serial.begin(BAUDRATE);	// Start serial communication.

#if defined(__AVR_ATmega32U4__) || defined(ESP8266) || defined(ESP32)	// FIXME: test ESP32  ################
  /* on ATmega32U4		Leonardo, Mini, LilyPad Arduino USB
     to be able to use Serial.print() from setup()
     we *must* do that before:
  */
  while (!Serial) { yield(); }		// wait for Serial to open
#endif

  // try to get rid of menu input garbage, "dopplet gnaeht hebt vilicht besser" ;)
  //  delay(STARTUP_DELAY);
  while (Serial.available())  { Serial.read(); yield(); }
  delay(STARTUP_DELAY);
  while (MENU.peek() != EOF8) { MENU.drop_input_token(); yield(); }
  MENU.ln();	// try to get empty start lines after serial garbage...
  MENU.space(8);
  MENU.ln();

#if defined RANDOM_ENTROPY_H	// *one* call would be enough, getting crazy on it ;)
  random_entropy();	// gathering entropy from serial noise
#endif

  MENU.out(F("PULSES  http://github.com/reppr/pulses/\t\t"));
  MENU.outln(F(STRINGIFY(PROGRAM_VERSION)));
  MENU.ln();

#if defined USE_NVS
  nvs_pulses_setup();
#endif

#if defined PERIPHERAL_POWER_SWITCH_PIN	// switch peripheral power on
  // for some strange reason i had to repeat this at the end of setup(), see below
  peripheral_power_switch_ON();		// default peripheral power supply ON
  //  peripheral_power_switch_OFF();	// default peripheral power supply OFF
  delay(100);	// wait a bit longer

  #if defined USE_RGB_LED_STRIP
    pulses_RGB_LED_string_init();	// do that *AGAIN*, as the string could hang on peripheral_power...
  #endif
#endif

#if defined HAS_DISPLAY
  // SEE: https://github.com/olikraus/u8g2/wiki/u8x8reference
  hw_display_setup();	// monochrome_begin() and monochrome_set_default_font() included in hw_display_setup() now

  bool has_display_hardware=true;	// for delay only	TODO: fix&use monochrome_display detection
  MENU.ln();
#endif

  show_internal_configurations();
  #if defined HAS_DISPLAY && ! defined TRIGGERED_MUSICBOX2
    delay(3000);	// sorry for that
  #endif

  MENU.print_free_RAM();
  MENU.ln();

  /*
    maybe_restore_from_RTCmem();
    *either* from
    pulses.ino setup()	would be better, but does not work?
    *or* from
    start_musicBox();	must be blocked if appropriate
  */
  //  maybe_restore_from_RTCmem();		// only after deep sleep, else noop

  MENU.out(F("sizeof(pulse_t) "));
  MENU.out(sizeof(pulse_t));
  MENU.out(F(" * "));
  MENU.out(PL_MAX);
  MENU.out(F(" pulses = \t"));
  MENU.outln(sizeof(pulse_t)*PL_MAX);
  MENU.ln();

#if defined ESP32
  setup_timer64();
#endif

#if defined HAS_DISPLAY
  // TODO: has_display_hardware	fix&use monochrome_display detection
  if(has_display_hardware)
    delay(1111);	// give a chance to read version on oled display during setup
#endif

#if defined OLED_HALT_PIN0  // ATTENTION: dangerous *not* tested with GPIO00 as click or such...
  pinMode(0, INPUT);		// holding GPIO00 switch holds program version display on screen
  MENU.out(F("boot button0\tusermode "));

  while(! digitalRead(0))	// if button0 *is* pressed  (pin==LOW)
    {
      force_start_to_usermode = true;	// then force user mode
      delay(333);
      MENU.out('!');
    }

  MENU.out_ON_off(force_start_to_usermode);
  MENU.ln();
#endif

#if defined USE_RGB_LED_STRIP
  MENU.out(F("RGB LED string on pin "));
  MENU.outln(HARDWARE.rgb_pin[0]);	// RGB_LED_STRIP_DATA_PIN
  MENU.ln();
#endif

#if defined USE_MPU6050
  mpu6050_available = mpu6050_setup();	// this will switch MPU6050 *OFF* if not found, including sampling
#endif

#if defined USE_MIDI
  MIDI_setup(/*RX*/ HARDWARE.MIDI_in_pin, /*TX*/ HARDWARE.MIDI_out_pin);	// midi_setup(/*RX*/ 18, /*TX*/ 19);
#endif

#if defined MUSICBOX_TRIGGER_PIN
  pinMode(HARDWARE.musicbox_trigger_pin, INPUT);
#endif

#if defined PERIPHERAL_POWER_SWITCH_PIN	// output now possible, so give info now
  MENU.out(F("peripheral POWER"));
  if(HARDWARE.periph_power_switch_pin != ILLEGAL8)
    MENU.out_ON_off(digitalRead(HARDWARE.periph_power_switch_pin));
  MENU.space();
  MENU.outln(HARDWARE.periph_power_switch_pin);
  MENU.ln();
#endif

#if defined USE_BLUETOOTH_SERIAL_MENU	// setup()	double, 1/2	test brownout recognition	TODO: review
  #if defined BLUETOOTH_ENABLE_PIN
  if(bluetooth_switch_info())		// setup()	double, 1/2	test brownout recognition	TODO: review
  #endif
    {
      bluetooth_setup();
      MENU.ln();
    }
#endif

#if defined USE_LoRa
  setup_LoRa_default();
#endif

#include "array_descriptors_setup.h"

#if defined USE_NVS	// always used on ESP32
  nvs_free_entries();
  MENU.ln();
#endif

show_GPIOs();	// *does* work for GPIO_PINS==0
//MENU.ln();

// #if defined USE_LEDC
//   #include "ledc_tone_setup.h"
// #endif

#if defined USE_BATTERY_LEVEL_CONTROL
  battery_control_setup();
#endif

#ifdef USE_LEDC_AUDIO
  ledc_audio_setup();
  #if defined LEDC_INSTEAD_OF_DACs
    MENU.outln(F("uses LEDC on pin 25 & 26"));	// TODO: read dac/ledc pins from HARDWARE
  #endif
#endif

#ifdef HARMONICAL_MUSIC_BOX
  musicBox_setup();
#endif

#if GPIO_PINS > 0
  init_click_GPIOs_OutLow();		// make them GPIO, OUTPUT, LOW
#endif

#ifdef USE_WIFI_telnet_menu		// do we use WIFI?
  #ifdef AUTOSTART_WIFI		// start wifi on booting? see: WiFi_stuff.ino
    AUTOSTART_WIFI
  #endif
#else // WiFi not in use, switch it off:

#if defined(ESP8266) || defined(ESP32)	// TODO: TEST: do we need that on ESP32?

    MENU.outln("switch WiFi off");

    #ifdef WIFI_OFF_hackster
      // see: https://www.hackster.io/rayburne/esp8266-turn-off-wifi-reduce-current-big-time-1df8ae
      WiFi.forceSleepBegin();                  // turn off ESP8266 RF
      delay(1);                                // give RF section time to shutdown

      #ifdef FREQUENCY
	system_update_cpu_freq(FREQUENCY);
      #endif
    #endif

    #ifdef WIFI_OFF_mysensors
      // see:	https://forum.mysensors.org/topic/5120/esp8266-with-wifi-off/3
      WiFi.disconnect();
      WiFi.mode(WIFI_OFF);
      WiFi.forceSleepBegin();
      delay(1);
    #endif
  #endif	// ESP8266, ESP32
#endif // to WiFi or not

#if defined USE_i2c
  Wire.begin(21, 22, 400000L);
  /*
    was:
     Wire.begin();
     Wire.setClock(400000L);	// must be *after* Wire.begin()
  */

  #if defined USE_MCP23017
    MCP23017.begin();
    Wire.setClock(100000L);	// must be *after* Wire.begin()  TODO: check if 400kHz does not work
    MCP23017_OUT_LOW();
    PULSES.do_A2 = &MCP23017_write;
  #endif

  #if defined  USE_ADS1115_AT_ADDR
    pulses_ADS1115_setup();
  #endif

  #if defined  USE_RTC_MODULE
    MENU.ln();
    show_DS1307_time_stamp();	// DS3231 or DS1307
    MENU.ln();
  #endif
#endif // USE_i2c


#if defined USE_ESP_NOW
  esp_now_pulses_setup();
#else
  delay(100);	// esp_now network build up, don't change startup time if esp_now *is* used or not
#endif

#if defined RANDOM_ENTROPY_H	// *one* call would be enough, getting crazy on it ;)
  random_entropy();	// more entropy from hardware like wifi, etc
#endif


  // setting up the menu:

  // add pulses main page:
  MENU.add_page("Pulses", 'P', \
		&menu_pulses_display, &menu_pulses_reaction, 'P');

  // add softboard page:
  softboard_page = MENU.add_page("Hardware Softboard", 'H',		\
			 &softboard_display, &softboard_reaction, 'H');

  #ifdef USE_INPUTS
    // add inputs page:
    MENU.add_page("Inputs", 'I', &inputs_display, &inputs_reaction, 'I');

    int inp=0;
    INPUTS.setup_analog_read(inp, 0, 8);			// A0, oversample=0
    INPUTS.setup_raw(inp);					// no calculations
    INPUTS.selected_inputs |= ++inp;				// selected for editor

    INPUTS.setup_analog_read(inp, 0, 8);				// A0, oversample=0
    INPUTS.setup_raw(inp);					// no calculations
    INPUTS.setup_linear(inp, 0, 255, 1023, 0, PROPORTIONAL);	// 255*x/1023
    INPUTS.selected_inputs |= ++inp;				// selected for editor
  #endif


  #ifdef USE_WIFI_telnet_menu
    // add WiFi page:
    MENU.add_page("WiFi", 'W', &WiFi_menu_display, &WiFi_menu_reaction, 'W');
  #endif

  #ifdef HARMONICAL_MUSIC_BOX
    // NOTE: musicBox_page is not used
    musicBox_page = MENU.add_page("musicBox", 'M', &musicBox_display, &musicBox_reaction, 'P');
  #endif

  #if defined  USE_BLUETOOTH_SERIAL_MENU	// hi jacking USE_BLUETOOTH_SERIAL_MENU
    MENU.add_page("Bluetooth", 'B', &bluetooth_menu_display, &bluetooth_menu_reaction, 'M');
  #endif

  #if defined USE_SYSTEM_MENU
    MENU.add_page("System", 'S', &system_menu_display, &system_menu_reaction, 'P');
  #endif

  #if defined USE_NVS_MENU
    MENU.add_page("NVS", 'N', &nvs_menu_display, &nvs_menu_reaction, 'M');
  #endif

  #if defined USE_LoRa
    MENU.add_page("LoRa", 'L', &LoRa_menu_display, &LoRa_menu_reaction, 'M');
  #endif

  // display menu at startup, but not in music box
  #if ! defined HARMONICAL_MUSIC_BOX
    MENU.menu_display();
  #endif

  #ifdef IMPLEMENT_TUNING		// implies floating point
    PULSES.ticks_per_octave = ticks_per_octave;
  #endif

  // time and pulses *must* get initialized before setting up pulses:
  PULSES.init_time();			// start time
  PULSES.init_pulses();			// init pulses
  MENU.ln();

  PULSES.fix_global_next();		// we *must* call that here late in setup();

  // informations about alive pulses:
  if(MENU.verbosity >= VERBOSITY_SOME) 	// can be switched off by autostart ;)
    selected_or_flagged_pulses_info_lines();


  // try to get rid of menu input garbage, "dopplet gnaeht hebt vilicht besser" ;)
  while (Serial.available())  { Serial.read(); yield(); }
  while (MENU.peek() != EOF8) { MENU.drop_input_token(); yield(); }


#ifdef HARMONICAL_MUSIC_BOX
  #if defined MUSICBOX_TRIGGER_PIN	// trigger pin?
    #if ! defined MAGICAL_TOILET_HACKS	// some quick dirty hacks, *no* interrupt
//    magic_trigger_ON();
    #else
      if(HARDWARE.musicbox_trigger_pin != ILLEGAL8)
	pinMode(HARDWARE.musicbox_trigger_pin, INPUT);
    #endif
  #endif
#endif


#if defined PERIPHERAL_POWER_SWITCH_PIN	// no idea why this is needed again, but it is
  peripheral_power_switch_ON();		// default peripheral power supply ON
  //  peripheral_power_switch_OFF();	// default peripheral power supply OFF
#endif

#ifdef USE_MORSE
  morse_init();	// ATTENTION: *do this AFTER esp_now_pulses_setup()*
#endif

#if defined USE_MIDI
  while (midi_available()) { midi_receive(); yield(); }	// get rid of fake '255' midi input
#endif

  MENU.men_selected = musicBox_page;	// default to musicBox menu

#if defined USE_LoRa
  #if defined LoRa_SEND_PING_ON_STARTUP && defined USE_LoRa_EXPLORING
    LoRa_send_ping();		// ping activates receiving
  #else
    setup_LoRa_default();	// activates receiving
  #endif
#endif
  MENU.ln();

  if(force_start_to_usermode) {
    force_start_to_usermode=false;
    MENU.outln(F("\nforced start to user mode"));
#if defined HAS_DISPLAY
    MC_display_message("user mode active");
#endif

  } else {
#if defined USE_NVS
    if(nvs_AUTOSTART_kb_macro) {
      MENU.out(F("nvs AUTOSTART "));
      MENU.play_KB_macro(nvs_AUTOSTART_kb_macro);
    }
    else
#endif

#ifdef AUTOSTART			// see: pulses_project_conf.h
      {
	autostart_counter++;
	MENU.out(F("\nAUTOSTART "));
	MENU.out(autostart_counter);
	MENU.tab();
	MENU.outln(STRINGIFY(AUTOSTART));
	AUTOSTART;
      }
#else
    ;
#endif
  }
} // setup()


// bool low_priority_tasks();
// check lower priority tasks and do the first one that needs to be done
// return true if something was done
#if defined USE_MPU6050
  uint32_t accgyro_modulus = 21221;	// prime	// TODO: UI
#endif
//
bool low_priority_tasks() {
  static uint32_t low_priority_cnt = 0;

  low_priority_cnt++;

#if defined USE_MIDI
  #warning "MIDI_reaction() is DEACTIVATED	TODO: implement"
//  if(midi_available()) {	// TODO: implement MIDI in reaction
//    MIDI_reaction();
//    return true;
//  }
#endif

#if defined USE_ESP_NOW && defined ESP_NOW_IDLE_ID_SEND
  if(esp_now_send_idle_identity)
    if(esp_now_idle_identification())
      return true;
#endif

#if defined USE_RGB_LED_STRIP
  if(rgb_strings_active && update_RGB_LED_string && rgb_strings_available) {
  #if defined MULTICORE_RGB_STRING
    extern void multicore_rgb_string_draw();
    multicore_rgb_string_draw();
  #else
    digitalLeds_drawPixels(strands, 1);
  #endif
    update_RGB_LED_string = false;
    return true;
  }
#endif

#if defined USE_MPU6050		// MPU-6050 6d accelero/gyro
  if(accGyro_new_data) {	//   check new input data
    if(!mpu6050_available)						// catch bugs, if any ;)  TODO: REMOVE:
      ERROR_ln(F("accGyro_new_data  mpu6050_available=false"));	// catch bugs, if any ;)  TODO: REMOVE:

    //accGyro_reaction();
    accGyro_reaction_v2();
    return true;
  }

  if(accGyro_is_active) {
    if ((low_priority_cnt % accgyro_modulus) == 0) { // take a accelerGyro sample?
#if defined  MULTICORE_MPU_SAMPLING
      multicore_sample_mpu();
#else
      accGyro_sample_v2();
#endif
      return true;
    }
  }
#endif

#ifdef IMPLEMENT_TUNING		// tuning, sweeping priority below menu		*implies floating point*
  tuning = PULSES.tuning;	// FIXME: workaround for having all 3 sweep implementations in parallel
  if (maybe_stop_sweeping())		// low priority control sweep range
    return true;

  if(maybe_display_tuning_steps())	// low priority tuning steps info
    return true;
#endif

#if defined USE_MORSE	// TODO: test right priority
  if(morse_output_to_do) {
    morse_do_output();
    return true;
  }
  if(morse_output_char) {
#if defined HAS_DISPLAY
    monochrome_out_morse_char();
#else
    MENU.out(morse_output_char);
#endif
    return true;
  }
#endif

  if (maybe_run_continuous())		// even lower priority: maybe display input state changes.
    return true;

  return false;
} // low_priority_tasks()


bool lowest_priority_tasks() {

#if defined USE_MORSE
  if(MENU.verbosity >= VERBOSITY_MORE) {
    if(morse_stat_ID != morse_stat_seen_ID) {
      morse_show_saved_stats();	// sets morse_stat_seen_ID
      return true;
    }
  }

  // morse auto adapt
  if(MENU.verbosity >= VERBOSITY_SOME) {
    if(morse_stats_mean_dash_factor != 1.0) {
      morse_stats_mean_dash_factor += 1.0;
      morse_stats_mean_dash_factor /= 2.0;
      morse_TimeUnit *= morse_stats_mean_dash_factor;

      MENU.out(F("morse auto adapt "));
      MENU.out(morse_stats_mean_dash_factor);
      MENU.tab();
      MENU.outln(morse_TimeUnit);

      morse_stats_mean_dash_factor = 1.0;	// reset and switch display off
      return true;
    }
  }
#endif // defined USE_MORSE

#ifdef USE_WIFI_telnet_menu
// ################ FIXME: cleanup old WIFI code ################
  // check telnet connection
//  MENU.out("check TELNET:	");

  // look for telnet client connect trial
//	  MENU.out("telnet server ");
//	  if (telnet_server.available())
//	    MENU.out("running	");
//	  else {
//	    MENU.out("trying to start	");
//	    telnet_server.begin();
//	    telnet_server.setNoDelay(true);
//	//    delay(1000);
//	    if (telnet_server.available()) {
//	      MENU.out("OK	");
//	//      delay(8000);
//	    }
//	    else
//	      MENU.out("still dead	");
//	    }

  if (telnet_server.hasClient()) {
    MENU.out("telnet client	");

    // ################  FIXME: something sends "exit" to the menu ;)  ################
    if (!server_client || !server_client.connected()) {
      if (server_client) {
	MENU.out("server_client.stop()\t");
	server_client.stop();
      }
      server_client = telnet_server.available();
      server_client.flush();
      MENU.out("new telnet client	");
    }
// don't know about 'else', runs well without:
//	    else
//	      {
//		MENU.outln("telnet client disconnect");
//		//      server_client.flush();
//		telnet_server.available().stop();
//	      }

    MENU.ln();
  }
#endif // USE_WIFI_telnet_menu

//#define TEMPORARY_TIME64_TEST	// TODO:  REMOVE:
#if defined TEMPORARY_TIME64_TEST
  if (random(1000000)==1) {
    MENU.out(F("ovfl "));
    MENU.out(PULSES.now.overflow);
    MENU.space(2);

    test_timer64();
    return true;
  }
#endif

#if defined USE_ESP_NOW
  if(do_esp_now_send_identity) {
    send_IDENTITY_time_sliced();
    return true;
  }
#endif

#if defined USE_LoRa
  if(check_for_LoRa_jobs())
     return true;
#endif

  return false;		// ################ FIXME: return values ################
} // lowest_priority_tasks()


void loop() {	// ARDUINO
  static unsigned int loop_cnt=0;
  unsigned int this_todo_cnt=0;

  loop_cnt++;

  #ifdef ESP8266	// hope it works on all ESP8266 boards, FIXME: test
    #ifdef WIFI_OFF_hackster
      // see: https://www.hackster.io/rayburne/esp8266-turn-off-wifi-reduce-current-big-time-1df8ae
      // pat the watchdog timer
      wdt_reset();
    #endif
  #endif

  /* calling check_maybe_do() in a while loop
     increases timing priority of PULSES over the MENU.
     It has little influence on low system load levels
     but when on stress PULSES gets up to *all* available time.
     The intention is to have PULSES continue functioning and
     let the UI starve, when there is not enough time for everything.
  */

  this_todo_cnt=0;
  stress_count=0;

  uint8_t relax_cnt=0;
  while (PULSES.check_maybe_do()) {	// in stress PULSES get's *first* priority.
    ++this_todo_cnt;

#if defined STRESS_MANAGMENT_H
    ++stress_count;
    /*		// instant_stress_release DEACTIVATED
    if(stress_count > (stress_emergency/8))
      instant_stress_release();
    */

  #if defined HARMONICAL_MUSIC_BOX    // magical_stress_release();
    if (stress_count >= stress_emergency) {
      magical_stress_release();
      stress_count = 0;
      stress_event_cnt = -1;	// one heavy stress event expected after magical_stress_release()...
      relax_cnt = 0;
    }

    if((stress_event_cnt >= 0) && (stress_count >= relax_level) && (relax_cnt < relaxmax)) {
      relax();
      relax_cnt++;
      stress_count = 0;
    }

    if(do_pause_musicBox) {
      //      MENU.outln(F(STRINGIFY(MUSICBOX_ENDING_FUNCTION)));	// MUSICBOX_WHEN_DONE_FUNCTION_DEFAULT
      do_pause_musicBox = false;
      stress_count = 0;
      stress_event_cnt = 0;
      delay(600);	// send remaining output

      (*musicBox_when_done)();		//      MUSICBOX_ENDING_FUNCTION	MUSICBOX_WHEN_DONE_FUNCTION_DEFAULT

      if(musicBox_when_done != &user) {	// user() is a flag *NOT to autostart* musicBox
	if(musicBox_when_done == &light_sleep) {
	  #if defined AUTOSTART
	    AUTOSTART;	// AUTOSTART after light_sleep()
	  #else
	    ;
	  #endif
	} else
	  start_musicBox();
      }
    }
  #endif // HARMONICAL_MUSIC_BOX

/*
  // stress release code written to kill sweep pulses getting to fast
  // in time machine this makes the situation worse, so deactivated for now

    if (stress_count >= stress_emergency) {
      // EMERGENCY
      // kill fastest pulse might do it? (i.e. fast sweeping up)

      if (MENU.maybe_display_more()) {	// TODO: rethink that
	MENU.out((int) PULSES.fastest_pulse());
	MENU.out(F(" deactivated  "));
      }

      PULSES.deactivate_pulse(PULSES.fastest_pulse());

      stress_count=0;				// seems best, maybe
      // stress_count = stress_emergency / 2;	// FIXME: further tests	################
    }
*/
#endif	// DO_STRESS_MANAGMENT

#if defined USE_RGB_LED_STRIP
    if(RGBstringConf.rgb_strings_active && update_RGB_LED_string && RGBstringConf.rgb_leds_high_priority) {
      digitalLeds_drawPixels(strands, 1);
      update_RGB_LED_string = false;
    }
#endif
  }

#if defined STRESS_MONITOR_LEVEL
  if (stress_count > STRESS_MONITOR_LEVEL) {	// just a simple test tool
    if(stress_event_cnt >= 0) {			// only *unexpected* stress
      if (stress_count > stress_event_level) {
	MENU.out(F("STRESS "));
	MENU.out(stress_event_cnt);
	MENU.space();
      } else
	MENU.out(F("stress   "));

      int s=STRESS_MONITOR_LEVEL;
      while(stress_count > s) {
	MENU.out('!');
	s *= 2;
      }
      MENU.tab();
      MENU.outln(stress_count);
    }
  }
#endif

  if (stress_count > stress_event_level) {
#if defined HARMONICAL_MUSIC_BOX			// magical_stress_release();
    relax();	// kill highest secondary pulse
#endif

    if(++stress_event_cnt > stress_event_cnt_MAX) {
#if defined HARMONICAL_MUSIC_BOX			// magical_stress_release();
      magical_stress_release();
#else
      MENU.outln(F("need stress release"));	// TODO: other stress release strategies
      // instant_stress_release();	// instant_stress_release DEACTIVATED
#endif
    }
  } else if(stress_event_cnt < 0)	// stress event was expected, but did not happen
    stress_event_cnt = 0;		// reset expectations

#if defined HARMONICAL_MUSIC_BOX
  #if defined MUSICBOX_TRIGGER_PIN	// trigger pin?
    #if defined MAGICAL_TOILET_HACKS	// some quick dirty hacks
      if(HARDWARE.musicbox_trigger_pin != ILLEGAL8) {
	if(digitalRead(HARDWARE.musicbox_trigger_pin))
	  musicBox_trigger_got_hot();	// must be called when musicBox trigger was detected high
      }
    #else
//     if(switch_magical_trigger_off) {
//       magic_trigger_OFF();
//     }
    #endif
  #endif
#endif

// descend through priorities and do first thing found
#if defined USE_MORSE
 #if defined TOUCH_ISR_VERSION_3	// touch morse input V3?
  if(! check_and_treat_morse_events_v3())

#elif defined TOUCH_ISR_VERSION_2	// touch morse input V2?	// REMOVED, see: morse_unused.txt
  if(! check_and_treat_morse_input_v2())				// REMOVED, see: morse_unused.txt
 #endif
#endif


#ifdef USE_INPUTS
  if(! maybe_check_inputs())		// reading inputs can be time critical, so check early
#endif
    {
      // DEBUG: see: infos/internal/2019-08-20_backtrace.txt
      if(MENU.lurk_then_do()) {		// MENU second in priority, check if something to do,
	stress_event_cnt = -1;		//   after many menu actions there will be a stress event, ignore that
      } else {      // no, menu did not do much

	if (! low_priority_tasks())		// check low_priority_tasks()
	  lowest_priority_tasks();		// if still nothing done, check lowest_priority_tasks()
      }
    }
} // ARDUINO loop()

#else		// c++ Linux PC test version

int main() {

  printf("\nTesting pulses.ino\n");

  printf("\nNumber of pulses: %d\n", PL_MAX);

  printf("\nPULSES.init_pulses();\n");
  PULSES.init_pulses();
  printf("\n");


  printf("sizeof(pulse_t) %d * %d = \t%d\n\n",	\
	 sizeof(pulse_t), PL_MAX, sizeof(pulse_t)*PL_MAX );


  // setting up the menu:

  // add pulses main page:
  MENU.add_page("pulses", 'P', \
		&menu_pulses_display, &menu_pulses_reaction, 'P');

/*
  // softboard does not make sense on PC
  // add softboard page:
  softboard_page = MENU.add_page("Arduino Softboard", 'H',		\
		&softboard_display, &softboard_reaction, 'H');
*/

  // display menu at startup:
  MENU.menu_display();

  // time and pulses *must* get initialized before setting up pulses:
  PULSES.init_time();		// start time
  PULSES.init_pulses();		// init pulses

  PULSES.fix_global_next();	// we *must* call that here late in setup();

  // informations about alive pulses:
  MENU.ln();
  selected_or_flagged_pulses_info_lines();

  // main program loop:
  while (true) {
  /* calling check_maybe_do() in a while loop
     increases timing priority of PULSES over the MENU.
     It has little influence on low system load levels
     but when on stress PULSES gets up to *all* available time.
     The intention is to have PULSES continue functioning and
     let the UI starve, when there is not enough time for everything.
  */
  while (PULSES.check_maybe_do()) { }	// in stress PULSES get's *first* priority.

#if defined(ESP32) || defined(ESP8266)
  yield();	// maybe good to do that, who knows?
#endif

  if(! MENU.lurk_then_do())		// MENU comes second in priority.
    {					// if MENU had nothing to do, then
      if (!maybe_stop_sweeping())		// low priority control sweep range
	if(! maybe_display_tuning_steps()) {	// low priority tuning steps info
	  ;
#ifdef ARDUINO
	  maybe_run_continuous();		//    lowest priority:
#endif					//    maybe display input state changes.
      }
    }
  }

} // PC main()

#endif


/* **************************************************************** */
// piezzo clicks on arduino i/o pins to *hear* the result:
// great help when debugging and a lot of fun to play with :)

//   or connect LEDs, MOSFETs, MIDI, whatever...
//   these are just FlipFlop pins.

// Click_pulses are a sub-group of pulses that control an arduino
// digital output each.
// The pins are configured as outputs by init_click_GPIOs_OutLow()
// and get used by clicks, jiffles and the like.

// It's best to always leave click_pulses in memory.
// You can set DO_NOT_DELETE to achieve this.


void click(int pulse) {	// can be called from a pulse
  if(PULSES.pulses[pulse].gpio != ILLEGAL8)
    digitalWrite(PULSES.pulses[pulse].gpio, PULSES.pulses[pulse].counter & 1);
}


#ifdef IMPLEMENT_TUNING		// implies floating point
void sweep_click(int pulse) {	// can be called from a pulse
  double period;
#if defined PULSES_USE_DOUBLE_TIMES
  period = PULSES.pulses[pulse].period;
#else // old int overflow style
  period = PULSES.pulses[pulse].period.time;
#endif

  double detune_number = PULSES.ticks_per_octave / period;
  double detune = 1 / pow(2.0, 1/detune_number);

  switch (sweep_up) {
  case 1:
    period *= detune;
    tuning *= detune;
    break;
  case -1:
    period /= detune;
    tuning /= detune;
    break;
  }

#if defined PULSES_USE_DOUBLE_TIMES
  PULSES.pulses[pulse].period = period;
#else // old int overflow style
  PULSES.pulses[pulse].period.time = period;
  // PULSES.pulses[pulse].period.overflow = 0;
#endif
  click(pulse);
} // sweep_click()


void tuned_sweep_click(int pulse) {	// can be called from a pulse
#if defined PULSES_USE_DOUBLE_TIMES
  double detune_number = PULSES.ticks_per_octave / PULSES.pulses[pulse].period;
#else // old int overflow style
  double detune_number = PULSES.ticks_per_octave / PULSES.pulses[pulse].period.time;
#endif
  double detune = pow(2.0, 1.0/detune_number);	// fails on Arduino Mega2560

  switch (sweep_up) {
  case 1:
    PULSES.tuning *= detune;
    break;
  case -1:
    PULSES.tuning /= detune;
    break;
  }

  click(pulse);
} // tuned_sweep_click()


// first try: octave is reached by a fixed number of steps:
void sweep_click_0(int pulse) {	// can be called from a sweeping pulse	// TODO: REMOVE:
#if defined PULSES_USE_DOUBLE_TIMES
  PULSES.pulses[pulse].period = PULSES.pulses[pulse].base_period;
  PULSES.pulses[pulse].period *= tuning;

#else // old int overflow style
  PULSES.pulses[pulse].period.time = PULSES.pulses[pulse].base_period.time * tuning;
  PULSES.pulses[pulse].period.overflow = 0;
#endif
  click(pulse);

  switch (sweep_up) {
  case 1:
    tuning *= detune;
    break;
  case -1:
    tuning /= detune;
    break;
  }
}


double low_sweep_limit = 0.0;	// no limits, please ;)
				// well there's still zero and resource limitations...

double high_sweep_limit = 0.0;	// no limits, please ;)
				// well there's still zero and resource limitations...

void tuning_info() {
  MENU.out(F("tuning ")); MENU.out(PULSES.tuning, 6);
  MENU.tab();

  if (low_sweep_limit) {
    MENU.out(F("slowest "));
    MENU.out(low_sweep_limit, 6);
    MENU.tab();
  }
  if (high_sweep_limit) {
    MENU.out(F("fastest 1/"));
    MENU.out((double) 1/high_sweep_limit, 6);
    MENU.tab();
  }
}

void sweep_info() {
  MENU.out(F("sweep "));
  switch (sweep_up) {
  case 0:
    MENU.out(F("off"));
    break;
  case 1:
    MENU.out(F("up"));
    break;
  case -1:
    MENU.out(F("down"));
    break;
  }

  MENU.out(F("\ttime/octave "));
  PULSES.display_realtime_sec(PULSES.simple_time(PULSES.ticks_per_octave));

  MENU.tab();
  tuning_info();
  MENU.ln();
} // sweep_info()


int is_octave(unsigned int integer) {
  int octave=0;
  for (; octave<32; octave++) {
    if (integer == 1 << octave)
      return octave;
  }

  return -1;
}


// inform user when tuning crosses integer or simple rational value
bool maybe_display_tuning_steps() {
  static int last_tuning_step=-1;  // impossible default
  static int last_fraction=-1;	   // impossible default
  static double last_tuning=1.0;

  bool did_something = false;

  if (last_tuning != tuning) {
    last_tuning = tuning;
    pulse_time_t now = PULSES.get_now();

    int tuning_step = tuning;			// integer part
    int current_fraction = 1.0/(double) tuning;	// integer part

    if (tuning_step != last_tuning_step) {	// integer part changed
      last_tuning_step = tuning_step;
      MENU.out(F("tuning * "));
      MENU.out(tuning_step);
      MENU.space();
      if (is_octave((int) tuning_step) != -1)
	MENU.out(F("<<"));
      MENU.tab();
      PULSES.display_realtime_sec(now); MENU.tab();
      sweep_info();
      did_something = true;
    }

    if (current_fraction != last_fraction) {
      last_fraction = current_fraction;
      MENU.out(F("tuning 1/"));
      MENU.out(current_fraction);
      MENU.space();
      if (is_octave((int) current_fraction) != -1)
	MENU.out(F("<<"));
      MENU.tab();
      PULSES.display_realtime_sec(now); MENU.tab();
      sweep_info();
      did_something = true;
    }
  } // tuning has changed

  return did_something;
} // maybe_display_tuning_steps()


bool maybe_stop_sweeping() {
  if (sweep_up == 0)
    return false;

  if (low_sweep_limit != 0.0) {
    if (tuning > low_sweep_limit) {
      sweep_up=0;
      tuning=low_sweep_limit;
      MENU.out(F("sweep stopped "));
      MENU.outln(tuning);
      return true;
    }
  }

  if (high_sweep_limit != 0.0) {
    if (tuning < high_sweep_limit) {
      sweep_up=0;
      tuning=high_sweep_limit;
      MENU.out(F("sweep stopped "));
      MENU.outln(tuning);
      return true;
    }
  }

  return false;
}
#endif // ifdef IMPLEMENT_TUNING	implies floating point


/* **************************************************************** */
#ifdef ESP32
  #include "driver/gpio.h"
  //#include "driver/rtc_io.h"
  //#include "soc/rtc_io_reg.h"
  //#include "soc/rtc.h"
  //#include "soc/io_mux_reg.h"
#endif


#if GPIO_PINS > 0
// TODO: move to library Pulses
void init_click_GPIOs_OutLow() {		// make them GPIO, OUTPUT, LOW
/* gpio_pin_t gpio_pins[GPIO_PINS];
   hardware pins for click_pulses
*/
  gpio_pin_t pin;

  for (int i=0; i<GPIO_PINS; i++) {
    pin=gpio_pins[i];

#ifdef ESP8266	// pin 14 must be switched to GPIO on ESP8266
    // http://www.esp8266.com/wiki/lib/exe/detail.php?id=esp8266_gpio_pin_allocations&media=pin_functions.png
    if (pin==14)
      pinMode(pin, FUNCTION_3); // pin 14 must be switched to GPIO on ESP8266

#elif defined ESP32
    // see http://wiki.ai-thinker.com/_media/esp32/docs/esp32_chip_pin_list_en.pdf

    if (GPIO_IS_VALID_OUTPUT_GPIO(pin) && (pin < 6 || pin > 11))
      PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[pin], PIN_FUNC_GPIO);

    pinMode(pin, OUTPUT);	// on oldstyle Arduinos this is enough
    digitalWrite(pin, LOW);	// on oldstyle Arduinos this is enough
 }

  gpio_config_t gpioConfig;
  gpioConfig.pin_bit_mask = (1 << 2) | (1 << 14) | (1 << 32) | (1 << 33);
  gpioConfig.mode = GPIO_MODE_OUTPUT;
  gpioConfig.pull_up_en = GPIO_PULLUP_DISABLE;
  gpioConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
  gpioConfig.intr_type = GPIO_INTR_DISABLE;

  #if defined DEBUG_GPIO_OUTPUT_INITIALISATION
    MENU.outln(" gpio_config(&gpioConfig)");
  #endif

  gpio_config(&gpioConfig);

#elif defined ARDUINO	// *non* ESP, Arduino

  for (int i=0; i<GPIO_PINS; i++) {
    pin=gpio_pins[i];
    pinMode(pin, OUTPUT);	// on oldstyle Arduinos this is enough
    digitalWrite(pin, LOW);	// on oldstyle Arduinos this is enough
  }

#else
  #error TODO: fix init_click_GPIOs_OutLow()
#endif	// board
}
#endif	// GPIO_PINS > 0

void out_noFreePulses() {
  MENU.out(F("no free pulses"));
}


#ifdef IMPLEMENT_TUNING		// implies floating point
// make an existing pulse to a sweep click pulse:
bool en_sweep_click(int pulse) {
  if ((pulse > -1) && (pulse < PL_MAX) && (pulse != ILLEGAL32)) {	// gpio set
    if (en_click(pulse, this_or_next_gpio(pulse))) {
      PULSES.set_payload(pulse, &sweep_click);	// gpio set

      return true;
    }
  }

  return false;
};


// make an existing pulse to a sweep_click_0 pulse:
bool en_sweep_click_0(int pulse) {
  if ((pulse > -1) && (pulse < PL_MAX) && (pulse != ILLEGAL32)) {
    if (en_click(pulse, this_or_next_gpio(pulse))) {	// gpio set
      PULSES.pulses[pulse].base_period = PULSES.pulses[pulse].period;
      PULSES.set_payload(pulse, &sweep_click_0);	// gpio set
      return true;
    }
  }

  return false;
};


bool en_tuned_sweep_click(int pulse) {
  if ((pulse > -1) && (pulse < PL_MAX) && (pulse != ILLEGAL32)) {
    if (en_click(pulse, this_or_next_gpio(pulse))) {	// gpio set
      PULSES.activate_tuning(pulse);
      PULSES.set_payload(pulse, &tuned_sweep_click);	// gpio set
      return true;
    }
  }

  return false;
}
#endif	// #ifdef IMPLEMENT_TUNING	implies floating point

int setup_click_synced(pulse_time_t when, unsigned long unit, unsigned long multiplier,
		       unsigned long divisor, int sync)
{
  pulse_time_t period = PULSES.simple_time(unit);
  PULSES.mul_time(&period, multiplier);
  PULSES.div_time(&period, divisor);
  int pulse= PULSES.setup_pulse_synced(&click, ACTIVE, when, period, sync);

  if ((pulse > -1) && (pulse < PL_MAX) && (pulse != ILLEGAL32))
    PULSES.set_gpio(pulse, this_or_next_gpio(pulse));
  else // no free pulse
    out_noFreePulses();

  return pulse;
}


/* **************************************************************** */
// playing with chords:

//   init_div_123456(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync);
void init_div_123456(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync) {
//	  const unsigned long divisor=1L;
  unsigned long unit = multiplier * PULSES.time_unit;
  unit /= divisor;

  display_name5pars("init_div_123456", inverse, voices, multiplier, divisor, sync);

  pulse_time_t now;
  PULSES.get_now();
  now=PULSES.now;

  int integer;
  if (! inverse) {	// low pin number has low note
    for (integer=1; integer<=voices; integer++) {
      setup_click_synced(now, unit, 1, integer, sync);
      MENU.out(integer);
    }
  } else {	// inverse: low pin number has high frequency
    for (integer=voices; integer>0; integer--) {
      setup_click_synced(now, unit, 1, integer, sync);
      MENU.out(integer);
    }
  }
  MENU.ln();

  PULSES.fix_global_next();
}

void init_123456(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync) {
  unsigned long unit = multiplier * PULSES.time_unit;
  unit /= divisor;

  display_name5pars("init123456", inverse, voices, multiplier, divisor, sync);

  pulse_time_t now;
  PULSES.get_now();
  now=PULSES.now;

  divisor=2L*36L;
  int integer;
  if (! inverse) {	// bottom down/up click-pin mapping
    // low pin number has longest period
    for (integer=voices; integer>0; integer--) {
      setup_click_synced(now, unit, integer, divisor, sync);
      MENU.out(integer);
    }
  } else {
    // low pin number has shortest period
    for (integer=1; integer<=voices; integer++) {
      setup_click_synced(now, unit, integer, divisor, sync);
      MENU.out(integer);
    }
  }
  MENU.ln();

  PULSES.fix_global_next();
}


void init_pentatonic(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync) {
  /*
    I was looking at pentatonic scales
    one of my favorits follows the 'd f g a c' pattern

    Let's look at the  numeric relations:
    d   f   g   a   c   d

    5 : 6
    3   :   4
    2     :     3
    1         :         2
  */
  if (inverse) {
    no_g_inverse();
    return;
  }
  unsigned long unit = multiplier * PULSES.time_unit;
  unit /= divisor;

  display_name5pars("init_pentatonic", inverse, voices, multiplier, divisor, sync);

  pulse_time_t now;
  PULSES.get_now();
  now=PULSES.now;

//  setup_click_synced(now, unit, 1, 1, sync);	// 'd'
//  setup_click_synced(now, unit, 5, 6, sync);	// 'f'
//  setup_click_synced(now, unit, 3, 4, sync);	// 'g'
//  setup_click_synced(now, unit, 2, 3, sync);	// 'a'
//  setup_click_synced(now, unit/3*2, 5, 6, sync); // 'c' is fifth from 'f'
//  setup_click_synced(now, unit, 1, 2, sync);	// 'd'

  setup_jiffle_thrower_synced(now, unit, 1, 1, sync, gling128);	// 'd'
  setup_jiffle_thrower_synced(now, unit, 5, 6, sync, gling128);	// 'f'
  setup_jiffle_thrower_synced(now, unit, 3, 4, sync, gling128);	// 'g'
  setup_jiffle_thrower_synced(now, unit, 2, 3, sync, gling128);	// 'a'
  if (voices > 4) {
    setup_jiffle_thrower_synced(now, unit/3*2, 5, 6, sync, gling128); // 'c' is fifth from 'f'
    setup_jiffle_thrower_synced(now, unit, 1, 2, sync, gling128);	// 'd'
  }
  if (voices > 6) {
    setup_jiffle_thrower_synced(now, unit, 5, 2*6, sync, gling128);	// 'f'
  }
  if (voices > 7) {
    // setup_jiffle_thrower_synced(now, unit, 3, 2*4, sync, gling128);	// 'g'
    setup_jiffle_thrower_synced(now, unit, 2, 2*3, sync, gling128);	// 'a' seems better on top with 8 voices
  }

  PULSES.fix_global_next();
}




// ****************************************************************
void select_flagged() {
  PULSES.clear_selection();
  for (int pulse=0; pulse<PL_MAX; pulse++)
    if (PULSES.pulses[pulse].flags)
      PULSES.select_pulse(pulse);
}


void select_all() {
  PULSES.clear_selection();
  for (int pulse=0; pulse<PL_MAX; pulse++)
    PULSES.select_pulse(pulse);
}


void select_alive() {
  PULSES.clear_selection();
  for (int pulse=0; pulse<PL_MAX; pulse++)
    if(PULSES.pulses[pulse].flags && (PULSES.pulses[pulse].flags != SCRATCH))
      PULSES.select_pulse(pulse);
}


// ****************************************************************
/* scale[]
   a scale array has elements of multiplier/divisor pairs
   each is the integer representation of a rational number
   very useful for all kind of things like scales, chords, rhythms */

int prepare_scale(bool inverse, int voices, unsigned long multiplier, unsigned long divisor, int sync, unsigned int *scale, bool octaves=true)
{
/* prepare a couple of pulses based on a scale array.
   up to 'voices' pulses are created among the selected ones.
   return number of prepared pulses */
  if(scale[0]==0)  return 0;	// error, no data

  if (inverse) {
    no_g_inverse();
    return 0;
  }

  int prepared=0;
  unsigned long unit = multiplier * PULSES.time_unit;
  unit /= divisor;

  if (octaves)
    display_name5pars("prepare_scale fill octaves", inverse, voices, multiplier, divisor, sync);
  else
    display_name5pars("prepare_scale", inverse, voices, multiplier, divisor, sync);

  pulse_time_t now;
  PULSES.get_now();
  now=PULSES.now;

  unsigned long this_period;
  pulse_time_t new_period;
  int pulse=0;
  int octave=1;	// 1,2,4,8,...
  for (int note=0; prepared<=voices; note++) {
    multiplier = scale[note*2];
    if (multiplier==0) {
      if (octaves) {
	octave *= 2;	// one octave higher
	note = 0;		// restart at first note
	multiplier = scale[note*2];
      } else
	goto global_next;		// multiplier==0, end
    }

    divisor = scale[note*2+1];
    if (divisor==0)  goto global_next;	// divisor==0, error, end
    divisor *= octave;

    for (; pulse<PL_MAX; pulse++) {
      if (PULSES.pulse_is_selected(pulse)) {
	this_period = unit;
	this_period *= multiplier;
	this_period /= divisor;
	new_period = PULSES.simple_time(this_period);
	PULSES.setup_pulse(NULL, SCRATCH, now, new_period);
	pulse++;
	prepared++;
	break;
      }
    }
    if (pulse==PL_MAX)	// all available pulses have been tried, give up
      break;
  }

 global_next:
  PULSES.fix_global_next();
  return prepared;
} // prepare_scale()


int selected_apply_scale_on_period(int voices, unsigned int *scale, bool octaves=true) { // TODO: OBSOLETE? ################
  // FIXME: octaves are untested here ################
  if(scale[0]==0)  return 0;	// error, no data

  pulse_time_t new_period;
  int applied=0;
  unsigned long multiplier, divisor;

  int octave=1;	// 1,2,4,8,...
  for (int note=0, pulse=0; applied<voices; note++) {
    multiplier = scale[note*2];
    if (multiplier==0) {
      if (octaves) {
	octave *= 2;	// one octave higher
	note = 0;	// restart at first note
	multiplier = scale[note*2];
      } else
	goto global_next;		// multiplier==0, end
    }

    divisor=scale[note*2+1];
    if (divisor==0)
      goto global_next;		// divisor==0: error, end
    divisor *= octave;

    for (; pulse<PL_MAX; pulse++) {
      if (PULSES.pulse_is_selected(pulse)) {
	new_period = PULSES.pulses[pulse].period;
	PULSES.mul_time(&new_period, multiplier);
	PULSES.div_time(&new_period, divisor);
	PULSES.pulses[pulse].period = new_period;
	applied++;
	pulse++;
	break;
      }
    }
    if (pulse==PL_MAX)	// all available pulses have been tried, give up
      break;
  }

 global_next:
  PULSES.fix_global_next();
  return applied;
}

// see: bool no_octave_shift=false;
int tune_selected_2_scale_limited(Harmonical::fraction_t* scaling_p, unsigned int *scale, unsigned long shortest_limit) {
/*
  tune all selected pulses to the scale, start with lowest selected
  scale 'PULSES.time_unit' by 'scaling' for base_period

  check if shortest period is longer than 'shortest_limit',
  else repeat 1 octave lower setting

  shortest limit *can* be zero to switch it off

  return octave_shift
*/

  MagicConf.octave_shift = 0;

  if (MENU.verbosity >= VERBOSITY_LOWEST) {
    MENU.out(F("tune_selected_2_scale_limited("));
    display_fraction(scaling_p);
    MENU.out_comma_();
    MENU.out(selected_name(SCALES));
    MENU.out_comma_();
    MENU.out(shortest_limit);
    MENU.outln(')');
  }

  if ((scale != NULL) && scale[0] && scaling_p->divisor) {
    musicBoxConf.steps_in_octave=0;
    pulse_time_t base_period = PULSES.simple_time(PULSES.time_unit);
    PULSES.mul_time(&base_period, scaling_p->multiplier);
    PULSES.div_time(&base_period, scaling_p->divisor);

    // check if highest note is within limit
    pulse_time_t this_period = PULSES.simple_time(0);	// bluff the very first test to pass
#if defined PULSES_USE_DOUBLE_TIMES
    while (this_period <= shortest_limit) { // SHORTEST LIMIT *CAN* BE ZERO (to switch it off)
#else // old int overflow style
    while (this_period.time <= shortest_limit) { // SHORTEST LIMIT *CAN* BE ZERO (to switch it off)
#endif

      int octave=1;  // 1,2,4,8,...	the octave of this note   (*not* octave_shift)
      int note = 0;
      int multiplier=0;
      int divisor=0;

      for(int pulse=0; pulse<PL_MAX; pulse++) {
	if (PULSES.pulse_is_selected(pulse)) {
	  if ((multiplier = scale[note*2]) == 0) {	// next octave?
	    if(musicBoxConf.steps_in_octave==0)		// after the *first* octave save musicBoxConf.steps_in_octave
	      PULSES.steps_in_octave = musicBoxConf.steps_in_octave = note;	// PULSES needs to know for MELODY_MODE
	    octave *= 2;	// one octave higher
	    note = 0;	// restart at first note
	    multiplier = scale[note*2];
	  }

	  divisor=scale[note*2+1];

	  // check for some very basic pure tuning intervals
	  PULSES.pulses[pulse].groups &= ~g_OCTAVE;	// clear all tuning groups
	  PULSES.pulses[pulse].groups &= ~g_FOURTH;
	  PULSES.pulses[pulse].groups &= ~g_FIFTH;

	  if(note==0)
	    PULSES.pulses[pulse].groups |= g_OCTAVE;		// tuned to an octave
	  else {
	    switch(multiplier) {
	    case 3:
	      if (divisor==4)
		PULSES.pulses[pulse].groups |= g_FOURTH;	// tuned to a fourth
	      break;
	    case 2:
	      if (divisor==3)
		PULSES.pulses[pulse].groups |= g_FIFTH;		// tuned to a fifth
	      break;
	    }
	  }

	  divisor *= octave;

	  this_period = base_period;
	  PULSES.mul_time(&this_period, multiplier);
	  if(divisor==0)
	    ERROR_ln(F("divisor==0"));	// TODO: fix cause, if that happens
	  else
	    PULSES.div_time(&this_period, divisor);
	  PULSES.pulses[pulse].period = this_period;

	  PULSES.pulses[pulse].note_position = ++note;	// after incrementing, tonic=1
	  PULSES.pulses[pulse].note_octave = octave;	// TODO: implement and use
	} // selected
      } // pulse

      if(MagicConf.no_octave_shift) {	// no_octave_shift, so we're done
	MagicConf.no_octave_shift = false;	// hmm? TODO: check that
	return 0;
      }

#if defined PULSES_USE_DOUBLE_TIMES
      if(this_period > shortest_limit) {
#else // old int overflow style
      if(this_period.time > shortest_limit) {
#endif
	if (MagicConf.octave_shift || MENU.verbosity > VERBOSITY_SOME) {
	  MENU.out(MagicConf.octave_shift);
	  MENU.out(F(" octaves shifted\t"));
	  (*HARMONICAL).reduce_fraction(scaling_p);
	  MENU.out(F("pitch new: "));
	  display_fraction(scaling_p);
	  MENU.ln();
	}
	return MagicConf.octave_shift;	// OK, tuning fine and within limit, RETURN
      } // else repeat one octave lower...

      PULSES.mul_time(&base_period, 2);
      MagicConf.octave_shift--;
      scaling_p->multiplier *= 2;	// fixing musicBoxConf.pitch
    } // while off limit, tune octaves down...
  } else ERROR_ln(F("invalid tuning"));

  return MagicConf.octave_shift;
} // tune_selected_2_scale_limited()


bool tune_2_scale(int voices, unsigned long multiplier, unsigned long divisor, unsigned int *scale)	// TODO: OBSOLETE? #########
{
  int pulse;
  pulse_time_t base_period = PULSES.simple_time(PULSES.time_unit);
  PULSES.mul_time(&base_period, multiplier);
  PULSES.div_time(&base_period, divisor);

  if (scale != NULL) {
    if (MENU.verbosity >= VERBOSITY_LOWEST) {	// debug output
      MENU.out(F("tune_2_scale("));
      MENU.out(voices);
      MENU.out_comma_();
      MENU.out(multiplier);
      MENU.out_comma_();
      MENU.out(divisor);
      MENU.out_comma_();
      MENU.out(array2name(SCALES, scale));
      MENU.outln(')');
    }

    if (voices>0) {
      PULSES.select_n(voices);

      for (pulse=0; pulse<voices; pulse++) {
	PULSES.pulses[pulse].period = base_period;		// set all voices to base note
	//	PULSES.pulses[pulse].period.overflow = 0;
      }

      // now apply scale:
      selected_apply_scale_on_period(voices, scale, true);	// TODO: OBSOLETE?	################
      return true;

    } else
      ERROR_ln(F("no voices"));
  } else
      ERROR_ln(F("no scale"));

  return false;
} // tune_2_scale()


int lower_audio_if_too_high(unsigned long limit) {
/*
  int lower_audio_if_too_high(unsigned long limit)
  if shortest period of selected pulses is too short, shift all periods by octaves
  return octave_shift	if ok
  return ERROR INT_MIN	if first selected pulse has no period data	(not used yet)
  return ERROR INT_MAX	if shortest==0					(not used yet)
  TODO: OBSOLETE?	################
*/
  unsigned int fastest_pulse=~0;	// pulse index with shortest period.time
  int octave_shift=0;
  int pulse;
  bool shortest_is_zero=false;

#if defined PULSES_USE_DOUBLE_TIMES
  for (pulse=0; pulse<PL_MAX; pulse++) {		// check if there *is* period data at all:
    if (PULSES.pulse_is_selected(pulse)) {		//   in the first selected pulse
      if(PULSES.pulses[pulse].period == 0.0) {	//     (disregarding overflow)
	ERROR_ln(F("no period data"));
	MENU.out(F("PRESET "));
	MENU.outln(musicBoxConf.preset);
	return INT_MIN;			//   return ERROR INT_MIN  if first selected pulse has no period data (not used yet)
      }
    }
  }
  // data ok

  pulse_time_t shortest = PULSES.INVALID_time();
  for (pulse=0; pulse<PL_MAX; pulse++) {	// find fastest selected pulse
    if (PULSES.pulse_is_selected(pulse)) {
      if(shortest > PULSES.pulses[pulse].period) {
	fastest_pulse =  pulse;
	shortest =  PULSES.pulses[pulse].period;
      }
    }
  }
  shortest_is_zero = (shortest == 0.0);

#else // old int overflow style
  unsigned int shortest=~0;		// shortest period.time (no overflow implemented here)

  for (pulse=0; pulse<PL_MAX; pulse++) {		// check if there *is* period data at all:
    if (PULSES.pulse_is_selected(pulse)) {		//   in the first selected pulse
      if(PULSES.pulses[pulse].period.time == 0) {	//     (disregarding overflow)
	ERROR_ln(F("no period data"));
	MENU.out(F("PRESET "));
	MENU.outln(musicBoxConf.preset);
	return INT_MIN;			//   return ERROR INT_MIN  if first selected pulse has no period data (not used yet)
      }
    }
  }
  // data ok

  for (pulse=0; pulse<PL_MAX; pulse++) {	// find fastest selected pulse
    if (PULSES.pulse_is_selected(pulse)) {
      if(shortest > PULSES.pulses[pulse].period.time) {
	fastest_pulse =  pulse;
	shortest =  PULSES.pulses[pulse].period.time;
      }
    }
  }

  shortest_is_zero = (shortest==0);

#endif  // old int overflow style

  if(MENU.verbosity >= VERBOSITY_SOME || shortest_is_zero) {	// DEBUGGING and normal feedback
    MENU.out(F("lower_audio_if_too_high shortest "));
    MENU.out(fastest_pulse);
    MENU.tab();
    MENU.outln(shortest);
    if(shortest==0) {
      MENU.outln(F(" DEBUG: shortest==0 "));
      MENU.ln();
    }
  }

  if(shortest_is_zero) {
    MENU.outln(F("shortest period 0"));
    MENU.outln(F("PRESET "));
    MENU.outln(musicBoxConf.preset);
    return INT_MAX;	// catch the shortest == 0, ERROR	return INT_MAX, not used yet
  }

#if defined PULSES_USE_DOUBLE_TIMES
  while (PULSES.pulses[fastest_pulse].period < limit) {	// too fast?
    octave_shift--;
    musicBoxConf.pitch.multiplier *= 2;	// fixing musicBoxConf.pitch

    for (pulse=0; pulse<PL_MAX; pulse++) {
      if (PULSES.pulse_is_selected(pulse)) {
	PULSES.mul_time(&PULSES.pulses[pulse].period, 2);	// octave shift down
      }
    }
  }

#else // old int overflow style
  while (PULSES.pulses[fastest_pulse].period.time < limit) {	// too fast?
    octave_shift--;
    musicBoxConf.pitch.multiplier *= 2;	// fixing musicBoxConf.pitch

    for (pulse=0; pulse<PL_MAX; pulse++) {
      if (PULSES.pulse_is_selected(pulse)) {
	PULSES.mul_time(&PULSES.pulses[pulse].period, 2);	// octave shift down
      }
    }
  }
#endif // old int overflow style

  if (octave_shift) {
    (*HARMONICAL).reduce_fraction(&musicBoxConf.pitch);
    if (MENU.verbosity >= VERBOSITY_LOWEST) {
      MENU.out(octave_shift);
      MENU.outln(F(" octaves shifted"));
    }
  }

  return octave_shift;
} // lower_audio_if_too_high()


// ****************************************************************
void init_chord_1345689a(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync) {
  unsigned long unit = multiplier * PULSES.time_unit;
  unit /= divisor;

  display_name5pars("init_chord_1345689a", inverse, voices, multiplier, divisor, sync);

  pulse_time_t now;
  PULSES.get_now();
  now=PULSES.now;

  divisor=32L*36L;
  multiplier=1;
  setup_click_synced(now, unit, multiplier, divisor, sync);

  multiplier=3;
  setup_click_synced(now, unit, multiplier, divisor, sync);

  multiplier=4;
  setup_click_synced(now, unit, multiplier, divisor, sync);

  if(voices>3) {
    multiplier=5;
    setup_click_synced(now, unit, multiplier, divisor, sync);
  }

  PULSES.fix_global_next();
}

/* **************************************************************** */
// playing with rhythms:


// Generic setup pulse, stright or middle synced relative to 'when'.
// Pulse time and phase sync get deviated from unit, which is first
// multiplied by multiplier and divided by divisor.
// sync=0 gives stright syncing, sync=1 middle pulses synced.
// other values possible,
// negative values should not reach into the past
// (that's why the menu only allows positive. it syncs now related,
//  so a negative sync value would always reach into past.)


// some default rhythms:

// helper function to generate certain types of sequences of harmonic relations:
// for harmonics I use rational number sequences a lot.
// this is a versatile function to create them:
void init_ratio_sequence(pulse_time_t when,
			 int multiplier0, int multiplier_step,
			 int divisor0, int divisor_step, int count,
			 unsigned int scaling, int sync
			 )
// By design click pulses *HAVE* to be defined *BEFORE* any other pulses:
//
// usage:
// 1,2,3,4 pattern	init_ratio_sequence(now, 1, 1, 1, 0, 4, scaling, sync)
// 3,5,7,9 pattern	init_ratio_sequence(now, 3, 2, 1, 0, 4, scaling, sync)
// 1/2, 2/3, 3/4, 4/5	init_ratio_sequence(now, 1, 1, 2, 1, 4, scaling, sync)
{
  const unsigned long unit = scaling * PULSES.time_unit;
  unsigned long multiplier=multiplier0;
  unsigned long divisor=divisor0;

  for (; count; count--) {
    setup_click_synced(when, unit, multiplier, divisor, sync);
    multiplier += multiplier_step;
    divisor += divisor_step;
  }

  PULSES.fix_global_next();
} // init_ratio_sequence()


/* **************************************************************** */
// some pre-defined patterns:

void init_rhythm_1(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync) {
  unsigned long unit = multiplier * PULSES.time_unit;
  unit /= divisor;

  pulse_time_t now;

  display_name5pars("init_rhythm_1", inverse, voices, multiplier, divisor, sync);

  PULSES.get_now();
  now=PULSES.now;

  divisor=1;
  for (long multiplier=2L; multiplier<6L; multiplier++)	// 2, 3, 4, 5
    setup_click_synced(now, unit, multiplier, divisor, sync);

  // 2*2*3*5
  setup_click_synced(now, unit, 2L*2L*3L*5L, divisor, sync);

  PULSES.fix_global_next();
}


// frequencies ratio 1, 4, 6, 8, 10
void init_rhythm_2(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync) {
  unsigned long unit = multiplier * PULSES.time_unit;
  unit /= divisor;

  display_name5pars("init_rhythm_2", inverse, voices, multiplier, divisor, sync);

  pulse_time_t now;
  PULSES.get_now();
  now=PULSES.now;

  multiplier=1;
  for (unsigned long divisor=4; divisor<12 ; divisor += 2)
    setup_click_synced(now, unit, multiplier, divisor, sync);

  // slowest *not* synced
  setup_click_synced(now, unit, 1, 1, 0);

  PULSES.fix_global_next();
}

// nice 2 to 3 to 4 to 5 pattern with phase offsets
void init_rhythm_3(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync) {
  unsigned long unit = multiplier * PULSES.time_unit;
  unit /= divisor;

  display_name5pars("init_rhythm_3", inverse, voices, multiplier, divisor, sync);

  pulse_time_t now;
  PULSES.get_now();
  now=PULSES.now;

  divisor=36L;

  multiplier=2;
  setup_click_synced(now, unit, multiplier, divisor, sync);

  multiplier=3;
  setup_click_synced(now, unit, multiplier, divisor, sync);

  multiplier=4;
  setup_click_synced(now, unit, multiplier, divisor, sync);

  if(voices>3) {
    multiplier=5;
    setup_click_synced(now, unit, multiplier, divisor, sync);
  }

  PULSES.fix_global_next();
}


void init_rhythm_4(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync) {
  unsigned long unit = multiplier * PULSES.time_unit;
  unit /= divisor;

  pulse_time_t now;

  display_name5pars("init_rhythm_4", inverse, voices, multiplier, divisor, sync);

  PULSES.get_now();
  now=PULSES.now;

  divisor=1;
  setup_click_synced(now, unit, 1, 1, sync);     // 1
  // init_ratio_sequence(when, multiplier0, multiplier_step, divisor0, divisor_step, count, scaling, sync);
  if (voices>=5)
    init_ratio_sequence(now, 1, 1, 2, 1, 4, 1, sync);     // 1/2, 2/3, 3/4, 4/5
  else
    init_ratio_sequence(now, 1, 1, 2, 1, voices-1, 1, sync);     // 1/2, 2/3, 3/4  or  1/2, 2/3
}


// dest codes:
#define CODE_PULSES	0		// dest code pulses: apply PULSES.selected_pulses
#define CODE_TIME_UNIT	1		// dest code PULSES.time_unit
unsigned char dest = CODE_PULSES;


/* **************************************************************** */
/* Menu UI							*/

// make an existing pulse to display 1 info line:
bool en_info(int pulse) {
  if ((pulse > -1) && (pulse < PL_MAX) && (pulse != ILLEGAL32)) {
    PULSES.set_payload(pulse, &pulse_info_1line);
    return true;
  }

  return false;
}

// TODO: move to Pulses.cpp
void pulse_info_1line(int pulse) {	// one line pulse info, short version
  if (PULSES.pulse_is_selected(pulse))
    MENU.out('*');
  else
    MENU.space();

  MENU.out(F(" PULSE "));
  if (pulse<100)	// left padding 'pulse'
    MENU.space();
  if (pulse<10)
    MENU.space();
  MENU.out(pulse);
  MENU.slash();
  MENU.out((unsigned int) PULSES.pulses[pulse].counter);
  // right padding 'PULSES.pulses[pulse].counter'
  if (PULSES.pulses[pulse].counter<100000)
    MENU.space();
  if (PULSES.pulses[pulse].counter<10000)
    MENU.space();
  if (PULSES.pulses[pulse].counter<1000)
    MENU.space();
  if (PULSES.pulses[pulse].counter<100)
    MENU.space();
  if (PULSES.pulses[pulse].counter<10)
    MENU.space();

  if(PULSES.pulses[pulse].flags & COUNTED)	// if COUNTED
    MENU.out(PULSES.pulses[pulse].remaining);	//   show remaining
  else
    MENU.space();

  MENU.space();
  if(PULSES.pulses[pulse].note_position)	// show note_position, if defined	// TODO: note_octave?
    MENU.out(PULSES.pulses[pulse].note_position);
  else
    MENU.space();

  MENU.tab();
  PULSES.show_group_mnemonics(pulse);

  MENU.out(F("Pf:"));
  PULSES.show_pulse_flag_mnemonics(PULSES.pulses[pulse].flags);
  if (PULSES.pulses[pulse].action_flags) {
    MENU.out(F(" Af: "));
    PULSES.show_action_flags(PULSES.pulses[pulse].action_flags);
  } else
    MENU.tab();

  if (PULSES.pulses[pulse].flags & HAS_GPIO) {
    MENU.out(F(" p"));
    if(PULSES.pulses[pulse].gpio != ILLEGAL8) {
      MENU.out((int) PULSES.pulses[pulse].gpio);
      if (PULSES.pulses[pulse].gpio<10)	// padding?
	MENU.space();
    } else
      MENU.out(F(" _"));
  } else
    MENU.space(4);
  MENU.space();

  if (PULSES.pulses[pulse].action_flags & doesICODE) {
    MENU.out(':');
    char * name=array2name(iCODEs, (unsigned int *) PULSES.pulses[pulse].icode_p);
    if (name[0])
      MENU.out(name);
    else {	// maybe it is a jiffle, played as iCode?
      MENU.out("J:");
      MENU.out(array2name(JIFFLES, (unsigned int *) PULSES.pulses[pulse].icode_p));
    }
    MENU.space();
  }

  display_payload(pulse);

  MENU.space(); MENU.tab();
  if(PULSES.pulses[pulse].dest_action_flags || \
     PULSES.pulses[pulse].dac1_intensity || PULSES.pulses[pulse].dac2_intensity) {
    MENU.out(F("dAf: "));
    PULSES.show_action_flags(PULSES.pulses[pulse].dest_action_flags);

    MENU.out(F(" i1:"));
    MENU.pad(PULSES.pulses[pulse].dac1_intensity, 4);

    MENU.out(F("i2:"));
    MENU.pad(PULSES.pulses[pulse].dac2_intensity, 4);
  } else
    MENU.tab(2);

  if (MENU.verbosity >= VERBOSITY_SOME) {
    MENU.tab();
    PULSES.print_period_in_time_units(pulse);
  }

  if (MENU.verbosity > VERBOSITY_SOME) {
    pulse_time_t sum = PULSES.pulses[pulse].next;
    PULSES.get_now();
    pulse_time_t delta =PULSES.now;
    PULSES.sub_time(&delta, &sum);

    MENU.tab();
    MENU.out(F("expected in"));
    PULSES.display_realtime_sec(sum);
    // PULSES.display_real_ovfl_and_sec(sum);	// debugging
  }

  if (PULSES.pulse_is_selected(pulse)) {
    MENU.space();
    MENU.out('*');
  }

  MENU.ln();
} // pulse_info_1line()


// pulse_info() as paylod for pulses: print pulse info:	// ################ TODO: update ################
void pulse_info(int pulse) {
  MENU.out(F("*** PULSE info "));
  MENU.out(pulse);
  MENU.slash();
  MENU.out((unsigned int) PULSES.pulses[pulse].counter);

  MENU.tab();
  display_payload(pulse);

  MENU.out_flags_();
  MENU.outBIN(PULSES.pulses[pulse].flags, sizeof(pulse_flags_t) * 8);
  MENU.ln();

  MENU.out(F("pin ")); MENU.out((int) PULSES.pulses[pulse].gpio);
  MENU.out(F("\tindex ")); MENU.out(PULSES.pulses[pulse].index);
  MENU.out(F("\ttimes ")); MENU.out(PULSES.pulses[pulse].remaining);
  MENU.out(F("\tcntd ")); MENU.out(PULSES.pulses[pulse].countdown);
  MENU.out(F("\tdata ")); MENU.out(PULSES.pulses[pulse].data);

#if defined PULSES_USE_DOUBLE_TIMES
  MENU.out(F("\ttime ")); MENU.out(PULSES.pulses[pulse].base_period);
  MENU.ln();		// start next line
  MENU.out(PULSES.pulses[pulse].period / (double) PULSES.time_unit, 6);
  MENU.out(F(" time\t"));
  MENU.out(PULSES.pulses[pulse].period);

#else // old int overflow style
  MENU.out(F("\ttime ")); MENU.out(PULSES.pulses[pulse].base_period.time);
  MENU.ln();		// start next line
  MENU.out((float) PULSES.pulses[pulse].period.time / (float) PULSES.time_unit, 6);
  MENU.out(F(" time"));

  MENU.out(F("\tpulse/ovf "));
  MENU.out((unsigned int) PULSES.pulses[pulse].period.time);
  MENU.slash();
  MENU.out(PULSES.pulses[pulse].period.overflow);
#endif

  MENU.tab();
  PULSES.display_realtime_sec(PULSES.pulses[pulse].period);
  MENU.space();
  MENU.out(F("pulse "));

  MENU.ln();		// start next line

#if defined PULSES_USE_DOUBLE_TIMES
  MENU.out(F("last "));
  MENU.out(PULSES.pulses[pulse].last);
  MENU.out(F("   \tnext "));
  MENU.out(PULSES.pulses[pulse].next);

#else // old int overflow style
  MENU.out(F("last/ovfl "));
  MENU.out((unsigned int) PULSES.pulses[pulse].last.time);
  MENU.slash();
  MENU.out(PULSES.pulses[pulse].last.overflow);

  MENU.out(F("   \tnext/ovfl "));
  MENU.out(PULSES.pulses[pulse].next.time);
  MENU.slash();
  MENU.out(PULSES.pulses[pulse].next.overflow);
#endif // old int overflow style

  MENU.tab();
  MENU.out(F("expected "));
  PULSES.display_realtime_sec(PULSES.pulses[pulse].next);

  MENU.ln();		// start last line
  PULSES.time_info();

  MENU.ln();		// traling empty line
}


// make an existing pulse to display multiline pulse info:
bool en_INFO(int pulse) {	// FIXME: to lib Pulses
  if ((pulse > -1) && (pulse < PL_MAX) && (pulse != ILLEGAL32)) {
    PULSES.set_payload(pulse, &pulse_info);
    return true;
  }

  return false;
}

// TODO: move to Pulses.cpp ################
void display_payload(int pulse) {
  void (*scratch)(int);

  MENU.out(F(" P:"));

  scratch=&click;
  if (PULSES.pulses[pulse].payload == scratch) {
    MENU.out(F("click"));
    return;
  }

#if defined HARMONICAL_MUSIC_BOX
  scratch=&musicBox_butler;
  if (PULSES.pulses[pulse].payload == scratch) {
    MENU.out(F("musicBox_butler"));
    return;
  }

  scratch=&magical_cleanup;
  if (PULSES.pulses[pulse].payload == scratch) {
    MENU.out(F("magical_cleanup"));
    return;
  }

  scratch=&cycle_monitor;	// currently cycle_monitor runs from within the butler, so will not show up here
  if (PULSES.pulses[pulse].payload == scratch) {
    MENU.out(F("cycle_monitor"));
    return;
  }

#endif

#ifdef IMPLEMENT_TUNING		// implies floating point
  scratch=&tuned_sweep_click;
  if (PULSES.pulses[pulse].payload == scratch) {
    MENU.out(F("tuned_sweep_click"));
    return;
  }

  scratch=&sweep_click;
  if (PULSES.pulses[pulse].payload == scratch) {
    MENU.out(F("sweep_click"));
    return;
  }

  scratch=&sweep_click_0;
  if (PULSES.pulses[pulse].payload == scratch) {
    MENU.out(F("sweep_click_0"));
    return;
  }
#endif	// #ifdef IMPLEMENT_TUNING	implies floating point

  scratch=&seed_icode_player;
  if (PULSES.pulses[pulse].payload == scratch) {
    MENU.out("seed_iCode ");
    char * name=array2name(iCODEs, (unsigned int *) PULSES.pulses[pulse].icode_p);
    if (name[0])
      MENU.out(name);		// display iCode name
    else {			// maybe it is a jiffle, played as iCode?
      name=array2name(JIFFLES, (unsigned int *) PULSES.pulses[pulse].icode_p);
      if (name[0])
	MENU.out(name);	// name of jiffle, played as iCode
      else
	MENU.outln(F("(none)"));
    }
    return;
  }

  scratch=&do_jiffle;
  if (PULSES.pulses[pulse].payload == scratch) {
    MENU.out(F("do_jiffle:"));
    MENU.out(array2name(JIFFLES, (unsigned int*) PULSES.pulses[pulse].data));
    return;
  }

  scratch=&do_throw_a_jiffle;
  if (PULSES.pulses[pulse].payload == scratch) {
    MENU.out(F("seed_jiff:"));
    MENU.out(array2name(JIFFLES, (unsigned int*) PULSES.pulses[pulse].data));
    return;
  }

  scratch=&pulse_info;
  if (PULSES.pulses[pulse].payload == scratch) {
    MENU.out(F("pulse_info"));
    return;
  }

  scratch=&pulse_info_1line;
  if (PULSES.pulses[pulse].payload == scratch) {
    MENU.out(F("info line"));
    MENU.tab(2);
    return;
  }
#if defined USE_MORSE && defined MORSE_OUTPUT_PIN
  scratch=&morse_feedback_d;
  if (PULSES.pulses[pulse].payload == scratch) {
    MENU.out(F("morse_feedback_d"));
    MENU.tab(2);
    return;
  }
#endif

  scratch=NULL;
  if (PULSES.pulses[pulse].payload == scratch) {
    MENU.tab();
    //    MENU.out(F("NULL"));
    return;
  }

  MENU.out(F("UNKNOWN\t"));
} // display_payload()


void flagged_pulses_info() {
  int count=0;

  for (int pulse=0; pulse<PL_MAX; ++pulse)
    if (PULSES.pulses[pulse].flags) {		// any flags set?
      pulse_info_1line(pulse);
      count++;
    }

  if (count == 0)
    MENU.outln(F("no flagged pulses"));
}


void selected_or_flagged_pulses_info_lines() {
  int count=0;
  for (int pulse=0; pulse<PL_MAX; ++pulse)
    if (PULSES.pulses[pulse].flags || (PULSES.pulse_is_selected(pulse))) { // any flags || selected
      pulse_info_1line(pulse);
      count++;
    }

  if (count == 0) {
    MENU.outln(F("no selected or flagged pulses"));
    if(PULSES.how_many_selected())
      PULSES.show_selected_mask();
  }

  MENU.ln();
}


/* **************************************************************** */
// make an existing pulse to a jiffle thrower pulse:
bool en_jiffle_thrower(int pulse, unsigned int *jiffle, gpio_pin_t pin, action_flags_t action_mask)
{
  if ((pulse > -1) && (pulse < PL_MAX) && (pulse != ILLEGAL32)) {
    PULSES.pulses[pulse].dest_action_flags |= action_mask;
    PULSES.set_payload_with_pin(pulse, &do_throw_a_jiffle, pin);
    PULSES.pulses[pulse].data = (unsigned int) jiffle;
    return true;
  }

  return false;
}


// make selected pulses jiffle throwers with gpio pin
int setup_jiffle_thrower_selected(action_flags_t action_flags) {	// FIXME: obsolete? ################
  int cnt=0;

  for (int pulse=0; pulse<PL_MAX; pulse++)
    if (PULSES.pulse_is_selected(pulse))
      if(en_jiffle_thrower(pulse, selected_in(JIFFLES), this_or_next_gpio(pulse), action_flags))
	cnt++;

  PULSES.fix_global_next();		// just in case?

  if (DO_or_maybe_display(VERBOSITY_MORE)) {
    MENU.ln();
    selected_or_flagged_pulses_info_lines();
  }

  return cnt;
};



int init_jiffle(unsigned int *jiffle, pulse_time_t when, pulse_time_t new_period, int origin_pulse)
{
  int pulse;
  pulse_time_t jiffle_period=new_period;
  PULSES.mul_time(&jiffle_period, jiffle[0]);
  PULSES.div_time(&jiffle_period, jiffle[1]);

  pulse = PULSES.setup_pulse(&do_jiffle, ACTIVE, when, jiffle_period);
  if ((pulse > -1) && (pulse < PL_MAX) && (pulse != ILLEGAL32)) {
    PULSES.pulses[pulse].action_flags |= PULSES.pulses[origin_pulse].dest_action_flags; // set actions
    PULSES.set_gpio(pulse, PULSES.pulses[origin_pulse].gpio);	// copy pin from origin pulse
    PULSES.pulses[pulse].index = 0;				// init phase 0
    PULSES.pulses[pulse].countdown = jiffle[2];			// count of first phase
    PULSES.pulses[pulse].data = (unsigned int) jiffle;
    PULSES.pulses[pulse].base_period = new_period;
#if defined USE_DACs
    PULSES.pulses[pulse].dac1_intensity = PULSES.pulses[origin_pulse].dac1_intensity;
  #if (USE_DACs > 1)
    PULSES.pulses[pulse].dac2_intensity = PULSES.pulses[origin_pulse].dac2_intensity;
  #endif
#endif
  }

  return pulse;
}


void do_throw_a_jiffle(int seeder_pulse) {		// for pulse_do
  // pulses[seeder_pulse].data	= (unsigned int) jiffle;

  // start a new jiffling pulse now (next [pulse] is not yet updated):
  unsigned int *this_jiff=(unsigned int *) PULSES.pulses[seeder_pulse].data;

  // check for empty jiffle first:
  bool has_data=true;
  for (int i=0; i<3; i++)
    if (this_jiff[i]==0)
      has_data=false;

  if (has_data)	{ // there *is* jiffle data
    int destination_pulse = init_jiffle((unsigned int *) PULSES.pulses[seeder_pulse].data,	\
	      PULSES.pulses[seeder_pulse].next, PULSES.pulses[seeder_pulse].period, seeder_pulse);
    PULSES.pulses[destination_pulse].groups |= g_SECONDARY;
    PULSES.pulses[destination_pulse].other_pulse = seeder_pulse;

#if defined USE_RGB_LED_STRIP
    if(PULSES.pulses[seeder_pulse].flags & HAS_RGB_LEDs) {
      PULSES.set_rgb_led_string(destination_pulse, PULSES.pulses[seeder_pulse].rgb_string_idx, PULSES.pulses[seeder_pulse].rgb_pixel_idx);
    }
#endif

  } else	// zero in first triplet, *invalid* jiffle table.
    MENU.outln(F("no jiffle"));
}


// old style, obsolete	// TODO: remove?
void prepare_magnets(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync) {
  if (inverse) {
    no_g_inverse();
    return;
  }

  select_in(SCALES, pentatonic_minor);
  PULSES.select_n(voices);

#define COMPATIBILITY_PERIOD_3110	// sets the period directly
#ifdef COMPATIBILITY_PERIOD_3110	// TODO: remove
  for (int pulse=0; pulse<voices; pulse++)
    if (PULSES.pulse_is_selected(pulse)) {
      PULSES.reset_and_edit_pulse(pulse, PULSES.time_unit);
      PULSES.pulses[pulse].period = PULSES.simple_time(3110);	// brute force for compatibility ;)
      en_jiffle_thrower(pulse, selected_in(JIFFLES), this_or_next_gpio(pulse), 0);
    }
  //  int selected_apply_scale_on_period(int voices, unsigned int *scale, bool octaves=true);
  selected_apply_scale_on_period(voices, selected_in(SCALES), true);	// TODO: OBSOLETE?	################
#else	// compatibility problems
  prepare_scale(false, voices, multiplier, divisor, sync, selected_in(SCALES), true);
#endif
}


/* **************************************************************** */
// pulses menu:

// what is selected?

void print_selected() {
  switch (dest) {
  case CODE_PULSES:
    PULSES.show_selected_mask();
    break;

  case CODE_TIME_UNIT:
    MENU.out_selected_();
    MENU.outln(F("time unit"));
    break;
  }
}


void info_select_destination_with(bool extended_destinations) {
  MENU.outln(F("SELECT DESTINATION for '= * / s K P n g i F j :' to work on:"));
  print_selected();
  MENU.out(F("select Pulse with "));

  MENU.out_ticked_hexs(min(PL_MAX,16));
  MENU.space();
  MENU.outln(F("select Mask with '.m<num>'"));

  MENU.outln(F("'.a'=select *all* click pulses\t'.A'=*all* pulses\t'.v'=voices\t'.L'=all alive\t'x'=none\t'.~'=invert selection"));

  if(extended_destinations) {	// FIXME: will that ever be used??? ################
    MENU.out(F("u=select "));  MENU.out(F("time unit"));
    if(dest == CODE_TIME_UNIT) {
      MENU.outln(F("\t(selected)"));
    } else
      MENU.ln();
    MENU.ln();
  }
}


// variables used to setup the experiments

#ifndef RAM_IS_SCARE	// enough RAM?
const char * experiment_names[] = {		// FIXME: const char * experiment_names would be better
      "(invalid)",				// 0
      "setup_jiffle128",			// 1
      "init_div_123456",			// 2
      "ESP8266 Frogs",				// 3
      "setup_jiffles2345",			// 4
      "init_123456",				// 5
      "init_chord_1345689a",			// 6
      "init_rhythm_1",				// 7
      "init_rhythm_2",				// 8
      "init_rhythm_3",				// 9
      "init_rhythm_4",				// 10
      "setup_jifflesNEW",			// 11
      "init_pentatonic",			// 12
      "magnets: The Harmonical Strings Christmas Evening Sounds",  // 13
      "magnets on strings 2",			// 14
      "magnets on strings 3",			// 15
      "piezzos on low strings 2016-12-28",	// 16
      "magnets on steel strings, japanese",	// 17
      "nylon stringed wooden box, piezzos",	// 18
      "tuned sweep",				// 19
      "arpeggio4096\tpentatonic",		// 20
      "arpeggio4096down\tpentatonic",		// 21
      "arpeggio_cont\tpentatonic",		// 22
      "arpeggio_and_down\tpentatonic",		// 23
      "stepping_down\tpentatonic",		// 24
      "back_to_ground\tpentatonic rhythm slowdown", // 25
      "arpeggio_and_sayling\tpentatonic",	// 26
      "simple_theme\tpentatonic",		// 27
      "peepeep4096\tpentatonic\tfor tuning",	// 28
      "KALIMBA7 tuning",			// 29
      "KALIMBA7 jiff pent minor",		// 30
      "KALIMBA7 jiff pent euro",		// 31
      "ESP32_12 pent euro",			// 32
      "minor scale",				// 33
      "major scale",				// 34
      "tetraCHORD",				// 35
      "more voices please",			// 36
      "Guitar and other Instruments",		// 37
      "Time Machine 1",				// 38
      "Time Machine 2",				// 39
      "Time Machine iCodeplayer",		// 40
//    "(invalid)",				// over
  };

  #define n_experiment_names (sizeof (experiment_names) / sizeof (const char *))
#endif // ! RAM_IS_SCARE


bool g_inverse=false;	// bottom DOWN/up GPIO click-pin mapping	// TODO: update!
/*
  'g_inverse' works when setting up an experiment creating pulses
	      other pulses aren't affected
	      some experiments do not implement that

  'reverse_gpio_pins()' as alternative:
  'reverse_gpio_pins()' works on the global gpio_pins[] array
			 the pulses won't notice but play with new pin mapping */


/* **************************************************************** */
// special menu modes, like numeric input for jiffles
#define JIFFLE_ENTRY_UNTIL_ZERO_MODE	1	// menu_mode for unsigned integer data entry, stop at zero

/* **************************************************************** */

void menu_pulses_display() {
  MENU.outln(F("http://github.com/reppr/pulses/"));

  MENU.out(F("\npulses "));
  MENU.out(PULSES.get_pl_max());
  MENU.space(2);
  MENU.out(F("GPIO "));
  MENU.outln(GPIO_PINS);
  if (MENU.verbosity > VERBOSITY_SOME)		// maybe display gpio_pins[]
    show_GPIOs();	// *does* work for GPIO_PINS==0

  MENU.ln();
  MENU.outln(F("?=help\t.=flagged info\t:=UI selections"));

  MENU.ln();
  info_select_destination_with(false);

  MENU.out(F("u=select "));
  PULSES.show_time_unit();

  MENU.ln();
  MENU.out(F("s=switch pulse on/off"));
  MENU.tab();
  MENU.out(F("M=mute ALL actions\tX=remove ALL\tK=kill\n\nCREATE PULSES\tstart with 'P'\nP=new pulse\tg=en-click\tj=en-jiffle\tN=en-noop\ti=en-info\tF=en-INFO\nS=sync\tn=sync now "));
  MENU.outln(musicBoxConf.sync);

  MENU.out(F("E=enter experiment (")); MENU.out(selected_experiment); MENU.out(')');
  MENU.out(F("\tV=voices for experiment (")); MENU.out(voices); MENU.out(F(")"));
  MENU.out(F("\tO=action_flags (")); PULSES.show_action_flags(selected_actions); MENU.outln(')');

  MENU.out(F("g~=toggle pin mapping (bottom "));
  if (g_inverse)
    MENU.out(F("up"));
  else
    MENU.out(F("down"));
  MENU.outln(F(")\tZ=reverse_gpio_pins"));

  MENU.out(F("Scale (")); MENU.out(musicBoxConf.pitch.multiplier); MENU.slash();  MENU.out(musicBoxConf.pitch.divisor);
  MENU.out(F(")\t'*!'=multiplier '/!'=divisor"));

  MENU.ln();

  stress_event_cnt = -3;	// heavy stress expected
}


// FIXME: obsolete? ################	// RETHINK:	DADA?
int setup_jiffle_thrower_synced(pulse_time_t when,
				unsigned long unit,
				unsigned long multiplier, unsigned long divisor,
				int sync, unsigned int *jiffle)
{
  pulse_time_t period = PULSES.simple_time(unit);
  PULSES.mul_time(&period, multiplier);
  PULSES.div_time(&period, divisor);
  int pulse= PULSES.setup_pulse_synced(&do_throw_a_jiffle, ACTIVE, when, period, sync);
  if ((pulse > -1) && (pulse < PL_MAX) && (pulse != ILLEGAL32)) {
    PULSES.set_gpio(pulse, this_or_next_gpio(pulse));
    PULSES.pulses[pulse].data = (unsigned int) jiffle;
  } else {
    out_noFreePulses();
  }

  return pulse;
} // setup_jiffle_thrower_synced()


/* **************************************************************** */
// jiffles:
// jiffles are (click) patterns defined in jiffletabs and based on a base period
//
// jiffletabs are unsigned int arrays
// the base period is multiplied/divided by two int values
// the following jiffletab value counts how many times the pulse will get
// woken up with this new computed period
// then continue with next jiffletab entries
// a zero multiplier ends the jiffle

// jiffletabs can define melodies:
// up to 256 triplets of {multiplier, divisor, count}
// multiplier and divisor determine period based on the starting pulses period
// a multiplier of zero indicates end of jiffle

#define JIFFLETAB_INDEX_STEP	3

void set_jiffle_RAM_value(int new_value) {
  char input;
  //  unsigned int* jiffle = selected_in(JIFFLES);

  code_RAM[jiffle_write_index++]=new_value;

  // jiffletabs *MUST* have 2 trailing zeros	// ################ FIXME: ################
  if (jiffle_write_index >= (CODE_RAM_SIZE - 2)) {	// array full?
    code_RAM[jiffle_write_index--]=0;

    // drop all remaining numbers and delimiters from input
    bool yes=true;
    while (yes) {
      input=MENU.drop_input_token();
      switch (input) {
      case ' ':  case ',':
      case '0':  case '1':  case '2':  case '3':  case '4':
      case '5':  case '6':  case '7':  case '8':  case '9':
	yes=true;
	break;

      default:
	yes=false;
      }
    }
  } else
    if (new_value==0)	// value was zero, stop input
      set_jiffle_RAM_value_0_stop();
}


// as a zero was written stop receiving further input, cleanup
void set_jiffle_RAM_value_0_stop() {
  if (jiffle_write_index >= (CODE_RAM_SIZE - 2))
    jiffle_write_index = CODE_RAM_SIZE - 2;

  code_RAM[CODE_RAM_SIZE - 1 ] = 0;		// zero out last 2 array elements (savety net)
  code_RAM[CODE_RAM_SIZE - 2 ] = 0;
  MENU.menu_mode=0;				// stop numeric data input
  //  jiffle_write_index=0;		// no, we leave write index as is

  display_jiffletab(code_RAM);		// put that here for now
}


void load_jiffle_2_code_RAM(unsigned int *source) {	// for jiffles, double zero terminated
  unsigned int data;
  unsigned int cnt=0;	// data counter

  while (jiffle_write_index < (CODE_RAM_SIZE - 2)) {
    data=source[cnt++];
    if (data==0) {
      --cnt;
      break;
    }

    set_jiffle_RAM_value(data);
  }
} // load_jiffle_2_code_RAM()


bool /*error*/ copy2_code_RAM(unsigned int *source, unsigned int size) {
  if(size > sizeof(code_RAM))
    return true;

  memcpy(code_RAM, source, size);
  return false;
} // copy2_code_RAM()


Harmonical::fraction_t jiffletab_len(unsigned int *jiffletab) {
  static Harmonical::fraction_t f;	// keep return value
  Harmonical::fraction_t scratch;
  unsigned int multiplier, divisor, count;

  f.multiplier = 0;
  f.divisor=1;
  for (int i=0;; i+=JIFFLETAB_INDEX_STEP) {
    multiplier = jiffletab[i];
    divisor    = jiffletab[i+1];
    count      = jiffletab[i+2];
    if (multiplier == 0 || divisor == 0 || count == 0)
      break;

    scratch.multiplier = multiplier * count;
    scratch.divisor = divisor;
    (*HARMONICAL).add_fraction(&scratch, &f);
  }

  return f;
}


void display_jiffletab(unsigned int *jiffle) {
  if (jiffle == NULL)	// silently ignore undefined
    return;

  Harmonical::fraction_t sum;
  sum.multiplier = 0;
  sum.divisor = 1;
  bool was_zero=false;

  int i = pointer2index(JIFFLES, jiffle);	// check DB first
  unsigned int len=0;
  if (i > 0)
    len = JIFFLES[i].len/sizeof(int);		// len is known

  // first line:
  MENU.out(JIFFLES[0].name); MENU.out(':'); MENU.space();
  MENU.out(array2name(JIFFLES, jiffle));  MENU.tab();
  MENU.out(F("editing "));
  if (MENU.menu_mode==JIFFLE_ENTRY_UNTIL_ZERO_MODE) {
    MENU.out(F("on\tclose with '}'"));
    if (MENU.verbosity >= VERBOSITY_SOME)
      MENU.out(F("\tmove cursor < >\trange borders | \""));
  } else
    MENU.out(F("off\tstart edit with '{'"));	// FIXME: in both int_edit.h and pulses.ino ################

  MENU.ln();

  // second line {data,data, ...}:
  MENU.out('{');
  for (int i=0; ; i++) {
    if ((i % JIFFLETAB_INDEX_STEP) == 0)
      MENU.space();
    if (i==jiffle_range_bottom)
      if (jiffle_range_top)	// no range, no sign
	MENU.out('|');
    if (i==jiffle_write_index)
      MENU.out("<");
    MENU.out(jiffle[i]);
    if (i==jiffle_write_index)
      MENU.out(">");
    if (i==jiffle_range_top)
      if (jiffle_range_top)	// no range, no sign
	MENU.out('"');
    MENU.out(",");
    if (jiffle[i] == 0) {
      if (was_zero)	// second zero done, stop it
	break;

      was_zero=true;	// first zero, continue *including* both zeroes
    } else
      was_zero=false;
  }
  MENU.space(); MENU.out('}');

  sum = jiffletab_len(jiffle);
  MENU.tab();
  MENU.out(F("length ")); display_fraction(&sum);// FIXME: in both int_edit.h and pulses.ino ################
  MENU.tab();
  MENU.out(F("cursor "));
  MENU.out(jiffle_write_index); MENU.slash(); MENU.out(len);
  MENU.ln();
}


void do_jiffle (int pulse) {	// to be called by pulse_do
/*
  PULSES.pulses[pulse].gpio		click pin
  PULSES.pulses[pulse].index		jiffletab index
  PULSES.pulses[pulse].countdown	count down
  PULSES.pulses[pulse].data		jiffletab[] pointer
  PULSES.pulses[pulse].base_period	base period = period of starting pulse
*/

  if(PULSES.pulses[pulse].gpio != ILLEGAL8)
    digitalWrite(PULSES.pulses[pulse].gpio, PULSES.pulses[pulse].counter & 1);	// click

  if (--PULSES.pulses[pulse].countdown > 0)				// countdown, phase endid?
    return;						//   no: return immediately

  // if we arrive here, phase endid, start next phase if any:
  unsigned int* jiffletab = (unsigned int *) PULSES.pulses[pulse].data;	// read jiffletab[]
  PULSES.pulses[pulse].index += JIFFLETAB_INDEX_STEP;
  int base_index = PULSES.pulses[pulse].index;		// readability
  long multiplier=jiffletab[base_index];
  if (multiplier == 0) {	// multiplier==0 no next phase, return

#if defined USE_RGB_LED_STRIP
    if(PULSES.pulses[pulse].flags & HAS_RGB_LEDs)
      set_rgb_led_background(pulse);
#endif

    PULSES.init_pulse(pulse);	// remove pulse
    return;			// and return
  }

  //initialize next phase, re-using the same pulse:

  // to be able to play unfinished jiffletabs while editing them
  // we check the other 2 items of the triple for zeroes
  long divisor=jiffletab[base_index+1];
  if (divisor==0) {	// no next phase, return
    MENU.outln("DADA jiff d=0");
    PULSES.init_pulse(pulse);		// remove pulse
    return;				// and return
  }

#if defined PULSES_USE_DOUBLE_TIMES
  PULSES.pulses[pulse].period = PULSES.pulses[pulse].base_period;
  PULSES.mul_time(&PULSES.pulses[pulse].period, jiffletab[base_index]);
  PULSES.div_time(&PULSES.pulses[pulse].period, jiffletab[base_index+1]);

#else // old int overflow style
  PULSES.pulses[pulse].period.time = \
    PULSES.pulses[pulse].base_period.time * jiffletab[base_index] / jiffletab[base_index+1];
#endif

  long counter=jiffletab[base_index+2];
  if (counter) {	// normal jiffle
    PULSES.pulses[pulse].countdown = jiffletab[base_index+2];		// count of next phase
  } else {	// count==0 wait
    PULSES.pulses[pulse].counter--;	// waiting is not counted as new pulse
  }
} // do_jiffle(p)


// ################################################################
// TODO: FIXME: REWORK:  setup_bass_middle_high()  used in musicBox, but not really compatible
// TODO: fix corner cases belonging to 2 groups, see: ESP32_DAC_ONLY	################
void setup_bass_middle_high(short bass_pulses, short middle_pulses, short high_pulses) {
  {
    MENU.out(F("setup_bass_middle_high("));	// ################ verbosity
    MENU.out(bass_pulses);
    MENU.out_comma_();
    MENU.out(middle_pulses);
    MENU.out_comma_();
    MENU.out(high_pulses);
    MENU.outln(')');
  }

  voices = bass_pulses + middle_pulses + high_pulses;	// init *all* primary pulses
  PULSES.select_n(voices);

  PULSES.add_selected_to_group(g_PRIMARY);

  musicBoxConf.base_pulse=0;		// a human perceived base pulse, see 'stack_sync_slices'
  if(bass_pulses && (middle_pulses || high_pulses))
    musicBoxConf.base_pulse = bass_pulses +1;	// FIXME: HACK: first pulse above bass, but should respect tuning (octave)

  // tune *all* primary pulses
#if ! defined MAGICAL_TOILET_HACK_2
  tune_2_scale(voices, multiplier, divisor, selected_in(SCALES));	// TODO: OBSOLETE?
  lower_audio_if_too_high(409600*2);	// 2 bass octaves  // TODO: adjust appropriate...
#endif

  // prepare primary pulse groups:

  // bass on DAC1 and broad angle LED lamps:
  // select_in(JIFFLES, d4096_512);

  PULSES.select_from_to(0, bass_pulses - 1);
  for(int pulse=0; pulse<bass_pulses; pulse++) {
    setup_icode_seeder(pulse, PULSES.pulses[pulse].period, (icode_t*) selected_in(iCODEs) , DACsq1 | doesICODE);

#if defined USE_MIDI
    PULSES.pulses[pulse].dest_action_flags |= sendMIDI;
#endif

#if defined USE_i2c
  #if defined USE_MCP23017
    PULSES.set_i2c_addr_pin(pulse, 0x20, pulse);	// ???
  #endif
#endif
  }
  PULSES.add_selected_to_group(g_LOW_END);

  PULSES.select_from_to(0,bass_pulses);			// TODO: do not start by 0!	pulse[bass_pulses] belongs to both output groups
  // selected_DACsq_intensity_proportional(DAx_max, 1);
  selected_share_DACsq_intensity(DAx_max, 1);		// bass DAC1 intensity

  // 2 middle octaves on 15 gpios
  PULSES.select_from_to(bass_pulses, bass_pulses + middle_pulses -1);

  for(int pulse=bass_pulses; pulse<bass_pulses+middle_pulses; pulse++) {
#if defined ICODE_INSTEAD_OF_JIFFLES
    setup_icode_seeder(pulse, PULSES.pulses[pulse].period, (icode_t*) selected_in(iCODEs), DACsq1 | doesICODE | CLICKs);
#else
    setup_icode_seeder(pulse, PULSES.pulses[pulse].period, (icode_t*) selected_in(JIFFLES), DACsq1 | doesICODE | CLICKs);
#endif

#if defined USE_MIDI
    PULSES.pulses[pulse].dest_action_flags |= sendMIDI;
#endif

    PULSES.set_gpio(pulse, next_gpio());
  }
  PULSES.add_selected_to_group(g_MIDDLE);

  // fix topmost bass pulse pulse[bass_pulses] that belongs to both groups:
  PULSES.pulses[bass_pulses].dest_action_flags |= DACsq1;

  // high octave on DAC2
  PULSES.select_from_to(bass_pulses + middle_pulses -1, bass_pulses + middle_pulses + high_pulses -1);
  for(int pulse = bass_pulses + middle_pulses; pulse<voices; pulse++) {	// pulse[21] belongs to both groups
#if defined ICODE_INSTEAD_OF_JIFFLES
    setup_icode_seeder(pulse, PULSES.pulses[pulse].period, (icode_t*) selected_in(iCODEs), DACsq2 | doesICODE);
#else
    setup_icode_seeder(pulse, PULSES.pulses[pulse].period, (icode_t*) d4096_256, DACsq2 | doesICODE);
#endif

#if defined USE_MIDI
    PULSES.pulses[pulse].dest_action_flags |= sendMIDI;
#endif
  }
  PULSES.add_selected_to_group(g_HIGH_END);

  // fix pulse[21] belonging to both output groups
  PULSES.pulses[bass_pulses + middle_pulses - 1].dest_action_flags |= DACsq2;
  selected_share_DACsq_intensity(DAx_max, 2);
  //	selected_DACsq_intensity_proportional(DAx_max, 2);

  PULSES.select_n(voices);	// select all primary voices again

#if defined USE_RGB_LED_STRIP
  int pl_max = PULSES.get_pl_max();
  for (int pulse=0; pulse<pl_max; pulse++) {
    if (PULSES.pulse_is_selected(pulse)) {
      PULSES.set_do_first(pulse, set_pulse_LED_pixel_from_counter);
      PULSES.set_rgb_led_string(pulse, 0, pulse_2_rgb_pixel(pulse));
    }
  }
#endif
} // setup_bass_middle_high()


// pre-defined jiffle pattern:
void setup_jiffles2345(bool g_inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync) {
  if (g_inverse) {
    no_g_inverse();
    return;
  }
  unsigned long unit = multiplier * PULSES.time_unit;
  unit /= divisor;

  display_name5pars("jiffles2345", g_inverse, voices, multiplier, divisor, sync);

  pulse_time_t when;
  PULSES.get_now();
  when=PULSES.now;

  divisor=1;

  multiplier=2;
  setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiffletab);

  multiplier=3;
  setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiffletab);

  multiplier=4;
  setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiffletab);

  multiplier=5;
  setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiffletab);

  // up to 8 voices
  if (voices > 5) {
    multiplier=6;
    setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiffletab);

    // skip multiplier 7

    if (voices > 6) {
      multiplier=8;
      setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiffletab);

      if (voices > 7) {
	multiplier=9;
	setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiffletab);
      }
    }

    // next would be i.e. 10, 12, 14, 15, 16
    // so 2 3 4 5 6 8 9 10 12 14 15 16
  }

  PULSES.fix_global_next();
}

// triplets {multiplier, divisor, count}
// multiplier==0 means end

unsigned int jiff0[] =
  {1,16,4, 1,24,6, 1,128,16, 1,1024,64, 1,2048,128, 1,4096,256, 1,2048,64, 1,4096,128, 1,32,2, 0,0}; // there *must* be a trailing zero.);

unsigned int jiff1[] =
  {1,512,8, 1,1024,16, 1,2048,32, 1,1024,16, 0,0};

unsigned int jiff2[] =
  {1,2096,4, 1,512,2, 1,128,2, 1,256,2, 1,512,8, 1,1024,32, 1,512,4, 1,256,3, 1,128,2, 1,64,1, 0,0};


void setup_jifflesNEW(bool g_inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync) {
  if (g_inverse) {
    no_g_inverse();
    return;
  }
  unsigned long unit = multiplier * PULSES.time_unit;
  unit /= divisor;

  display_name5pars("setup_jifflesNEW", g_inverse, voices, multiplier, divisor, sync);

  pulse_time_t when;
  PULSES.get_now();
  when=PULSES.now;

  multiplier=1;

  divisor=1;
  setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiff0);

  divisor=3;
  setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiff1);

  divisor=4;
  setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiff2);

  if (voices > 5) {
    divisor=6;
    setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiffletab0);
  }

  PULSES.fix_global_next();
}


void setup_jiffle128(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync) {
  /*
  multiplier and divisor are used twice:
  first to scale unit from PULSES.time_unit
  then reset to build the jiffle thrower pulses
    multiplier=1
    divisor = 1, 2, 3, 4, ...
  */
  unsigned long unit = multiplier * PULSES.time_unit;
  unit /= divisor;

  display_name5pars("setup_jiffle128", inverse, voices, multiplier, divisor, sync);

  PULSES.get_now();
  pulse_time_t when=PULSES.now;

  multiplier=1;
  if (!inverse) {
    for (int click=0; click<voices; click++) {
      divisor=click+1;
      setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, gling128);
    }
  } else {
    for (int click=voices-1; click>=0; click--) {
      divisor=click+1;
      setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, gling128);
    }
  }

  PULSES.fix_global_next();
}

// setup for ESP8266 Frog Orchester
void setup_jiffles0(bool g_inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync) {
  if (g_inverse) {
    no_g_inverse();
    return;
  }
  unsigned long unit = multiplier * PULSES.time_unit;
  unit /= divisor;

  display_name5pars("setup_jiffles0", g_inverse, voices, multiplier, divisor, sync);

  pulse_time_t when;
  PULSES.get_now();
  when=PULSES.now;

  divisor=3;	// this was just the default for the frogs

  multiplier=2;
  setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiffletab0);

  multiplier=3;
  setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiffletab0);

  multiplier=4;
  setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiffletab0);

  multiplier=5;
  setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiffletab0);

  multiplier=6;
  setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiffletab0);

//	  multiplier=8;
//	  setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiffletab0);
//
//	  multiplier=9;
//	  setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiffletab0);
//
//	  multiplier=10;
//	  setup_jiffle_thrower_synced(when, unit, multiplier, divisor, sync, jiffletab0);

  PULSES.fix_global_next();
}

/*
  'g_inverse' works when setting up an experiment creating pulses
	      other pulses aren't affected
	      some experiments do not implement that

  'reverse_gpio_pins)' as alternative:
  'reverse_gpio_pins()' works on the global gpio_pins[] array
			 the pulses won't notice but play with new pin mapping */


#if GPIO_PINS > 0
// TODO: move to library Pulses
bool gpio_pins_inverted=false;
void reverse_gpio_pins() {	// TODO: fix next_gpio()  ???? ################
  gpio_pin_t scratch;
  for (int i=0, j=GPIO_PINS-1; i<j; i++, j--) {
      scratch=gpio_pins[i];
      gpio_pins[i]=gpio_pins[j];
      gpio_pins[j]=scratch;
  }

  gpio_pins_inverted=!gpio_pins_inverted;
}
#endif

// ****************************************************************
// menu_pulses_reaction()

// display helper functions:
void all_pulses_info() {	// use as  do_on_other_core(&all_pulses_info);
  if (MENU.verbosity > VERBOSITY_SOME) {
    MENU.ln();
    PULSES.time_info();
  }

  if (MENU.verbosity >= VERBOSITY_SOME) {
    MENU.ln();
    print_selected();
  }

  MENU.ln();
  selected_or_flagged_pulses_info_lines();

  stress_event_cnt = -3;	// heavy stress expected
}


// helper functions to display parameters of menu functions:
void display_next_par(long parameter) {
  MENU.out_comma_();
  MENU.out(parameter);
}

void display_last_par(long parameter) {
  MENU.out_comma_();
  MENU.out(parameter);
  MENU.outln(F(")"));
}

void display_name5pars(const char* name, bool g_inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync) {
  if (MENU.verbosity) {
    MENU.out((char *) name);
    MENU.out("(");
    MENU.out(g_inverse);
    display_next_par(voices);
    display_next_par(multiplier);
    display_next_par(divisor);
    display_last_par(sync);
  }
}

// display helper function no_g_inverse()
void no_g_inverse() {
    MENU.outln(F("no 'g_inverse'\ttry 'Z' instead"));
    extern bool g_inverse;
    g_inverse=false;
}

// display factor function show_scaling();
void show_scaling() {
  MENU.out(F("multiplier/divisor "));
  MENU.out(musicBoxConf.pitch.multiplier); MENU.slash(); MENU.out(musicBoxConf.pitch.divisor);
}

void show_UI_basic_setup() {
  MENU.out(F("SYNC: "));
  MENU.out(musicBoxConf.sync);
  MENU.space(4);

  MENU.out(F("SCALE: "));
  MENU.out(selected_name(SCALES));
  MENU.space(3);

#if defined ICODE_INSTEAD_OF_JIFFLES	// TODO: REMOVE: after testing a while
  MENU.out(F("iCode: "));
  MENU.out(selected_name(iCODEs));
#else
  MENU.out(F("JIFFLE: "));
  MENU.out(selected_name(JIFFLES));
#endif
  MENU.space(3);

  MENU.out(F("SCALING: "));
  MENU.out(musicBoxConf.pitch.multiplier);
  MENU.slash();
  MENU.out(musicBoxConf.pitch.divisor);
  MENU.space(3);
  if(MagicConf.some_metric_tunings_only)
    MENU.out(F("*metric*"));
  MENU.space();
  MENU.out(metric_mnemonic);

  if (g_inverse)	// FIXME: TODO: check where that *is* used ################
    MENU.out(F("\tGPIO BOTTOM UP"));
  MENU.ln();	// maybe...
} // show_UI_basic_setup()

void show_UI_settings() {
  MENU.ln();
  show_UI_basic_setup();

  MENU.out(F("Af:"));
  PULSES.show_action_flags(selected_actions);
  MENU.tab();

  MENU.out(F("voices: "));
  MENU.out(voices);
  MENU.tab();

  MENU.verbosity_info();
  MENU.ln();

  PULSES.show_time_unit();
}

// for old style 'experiment'
void Press_toStart() {
  MENU.outln(F("Press '!' to start"));
}


void select_iCode_UI() {	// 'Q'  select iCode, maybe apply to selected pulses
  /*
    'Q'   shows registered iCode names and display data
    'QT'  tests iCode
    'Q7'  selects iCode #7 and display data
    'Q!'  copy selected iCode in code_RAM, select code_RAM, display_jiffletab(code_RAM)
    'Q9!' copy iCode #9 in code_RAM, select code_RAM, display_jiffletab(code_RAM)
  */

  unsigned int* iCode_was = selected_in(iCODEs);

  int next_token = MENU.peek();
  if (next_token != '!' && next_token != '?')	// 'Q<num>' 'Q+'=='QE' 'Q-'=='QT' select iCode
    if (UI_select_from_DB(iCODEs)) {		// select iCode UI
      icode_user_selected = true;
      not_a_preset();
    }

  bool trying=true;
  while (trying) {
    switch (MENU.peek()) {
    case '?':	// 'Q[...]?' tests a iCode
      MENU.drop_input_token();
      test_code(selected_in(iCODEs), selected_name(iCODEs), 3);
      break;

    case '!':	// 'Q[<num>]!' copies an already selected iCodetab to RAM, selects RAM
      MENU.drop_input_token();
      if(selected_in(iCODEs) != code_RAM) {
	unsigned int * source=selected_in(iCODEs);
	copy2_code_RAM(source, selected_length_in(iCODEs));
      }
      select_in(iCODEs, code_RAM);
      icode_user_selected = true;
      break;

    default:
      trying=false;
    }
  }

  if(iCode_was != selected_in(iCODEs)) {
    icode_user_selected = true;
    en_icode_seeder_selected((icode_t*) selected_in(iCODEs), selected_actions);
    not_a_preset();
  }

  if (MENU.verbosity >= VERBOSITY_SOME)
    display_icode((icode_t*) selected_in(iCODEs), selected_length_in(iCODEs));
} // select_iCode_UI()


void select_jiffle_UI() {	// 'J'  select jiffle, maybe apply to selected pulses
  /*
    'J'  shows registered jiffle names and display_jiffletab(<selected_jiffle>)
    'JT' tests jiffle
    'J7' selects jiffle #7 and display_jiffletab()
    'J!' copy selected jiffle in code_RAM, select code_RAM, display_jiffletab(code_RAM)
    'J9!' copy jiffle #9 in code_RAM, select code_RAM, display_jiffletab(code_RAM)
  */
  // some jiffles from source, some very old FIXME:	review and delete	################

  unsigned int* jiffle_was = selected_in(JIFFLES);

  int next_token = MENU.peek();
  if (next_token != '!' && next_token != '?')	// 'J<num>' 'J+'=='JE' 'J-'=='JT' select jiffle
    if (UI_select_from_DB(JIFFLES)) {		// select jiffle UI
      jiffle_user_selected = true;
      not_a_preset();
    }

  bool trying=true;
  while (trying) {
    switch (MENU.peek()) {
    case '?':	// 'J[...]?' tests a jiffle
      MENU.drop_input_token();
      test_code(selected_in(JIFFLES), selected_name(JIFFLES), 3);
      break;

    case '!':	// 'J[<num>]!' copies an already selected jiffletab to RAM, selects RAM
      MENU.drop_input_token();
      if(selected_in(JIFFLES) != code_RAM) {
	unsigned int * source=selected_in(JIFFLES);
	// jiffle_write_index=0;	// no, write starting at jiffle_write_index #### FIXME: ####
	load_jiffle_2_code_RAM(source);
      }
      select_in(JIFFLES, code_RAM);
      jiffle_user_selected = true;
      break;

    default:
      trying=false;
    }
  }

  if(jiffle_was != selected_in(JIFFLES)) {
    jiffle_user_selected = true;
    setup_jiffle_thrower_selected(selected_actions);	// was: 'j'
    not_a_preset();
  }

  if (MENU.verbosity >= VERBOSITY_SOME)
    display_jiffletab(selected_in(JIFFLES));
} // select_jiffle_UI()


void user_selected_scale(unsigned int* scale) {
  if(scale != NULL) {
    select_in(SCALES, scale);
    scale_user_selected = true;
  }
}

void select_scale__UI() {	// OBSOLETE?: only used inside oldstyle experiments...
  /*
    void select_scale__UI() examples:
    "a"		metric a=440 *minor* scale
    "E"		metric E=330 *major* scale
    "d5"	metric d *minor pentatonic* scale
    "C5"	metric C *european pentatonic* scale
    "F4"	*tetraCHORD* (as a scale) on metric F
  */

  unsigned int* scale_was = selected_in(SCALES);

  switch (MENU.peek()) {
  case EOF8:
    break;

    /*
      TODO:
      case '?':
      break;
    */
  case 'U':	// toggle harmonical time unit
    MENU.drop_input_token();

    if(PULSES.time_unit != TIME_UNIT)
      PULSES.time_unit=TIME_UNIT;	// switch to harmonical time unit
    else
      PULSES.time_unit=1000000;		// switch to metric time unit
    break;

  case 'S':	// use       if(UI_select_from_DB(SCALES))	// select scale
    MENU.drop_input_token();
    if(UI_select_from_DB(SCALES))	// select scale
      scale_user_selected = true;
    break;

  case 'c':	// c minor
    musicBoxConf.chromatic_pitch = 4;
    MENU.drop_input_token();
    user_selected_scale(minor_scale);
    PULSES.time_unit=1000000;	// switch to metric time unit
    musicBoxConf.pitch.divisor=262; // 261.63	// C4  ***not*** harmonical
    break;

  case 'C':	// c major
    musicBoxConf.chromatic_pitch = 4;
    MENU.drop_input_token();
    user_selected_scale(major_scale);
    PULSES.time_unit=1000000;	// switch to metric time unit
    musicBoxConf.pitch.divisor=262; // 261.63	// C4  ***not*** harmonical
    MENU.outln(" ok");
    break;

  case 'd':	// d minor scale
    musicBoxConf.chromatic_pitch = 6;
    MENU.drop_input_token();
    PULSES.time_unit=1000000;	// switch to metric time unit
    musicBoxConf.pitch.divisor = 294;		// 293.66 = D4
    // divisor = 147;		// 146.83 = D3
    user_selected_scale(minor_scale);
    break;

  case 'D':	// D major scale
    musicBoxConf.chromatic_pitch = 6;
    MENU.drop_input_token();
    PULSES.time_unit=1000000;	// switch to metric time unit
    musicBoxConf.pitch.divisor = 294;		// 293.66 = D4
    // divisor = 147;		// 146.83 = D3
    user_selected_scale(major_scale);
    break;

  case 'e':	// e minor scale
    musicBoxConf.chromatic_pitch = 8;
    MENU.drop_input_token();
    user_selected_scale(minor_scale);
    PULSES.time_unit=1000000;	// switch to metric time unit
    musicBoxConf.pitch.divisor=330; // 329.36	// e4  ***not*** harmonical
    break;

  case 'E':	// E major scale
    musicBoxConf.chromatic_pitch = 8;
    MENU.drop_input_token();
    user_selected_scale(major_scale);
    PULSES.time_unit=1000000;	// switch to metric time unit
    musicBoxConf.pitch.divisor=330; // 329.36	// e4  ***not*** harmonical
    break;

  case 'f':	// f minor
    musicBoxConf.chromatic_pitch = 9;
    MENU.drop_input_token();
    user_selected_scale(minor_scale);
    PULSES.time_unit=1000000;	// switch to metric time unit
    musicBoxConf.pitch.divisor=175; // 174.16	// F3  ***not*** harmonical
    break;

  case 'F':	// f major
    musicBoxConf.chromatic_pitch = 9;
    MENU.drop_input_token();
    user_selected_scale(major_scale);
    PULSES.time_unit=1000000;	// switch to metric time unit
    musicBoxConf.pitch.divisor=175; // 174.16	// F3  ***not*** harmonical
    break;

  case 'g':	// g minor
    musicBoxConf.chromatic_pitch = 11;
    MENU.drop_input_token();
    user_selected_scale(minor_scale);
    PULSES.time_unit=1000000;	// switch to metric time unit
    musicBoxConf.pitch.divisor=196; // 196.00	// G3  ***not*** harmonical
    break;

  case 'G':	// g major
    musicBoxConf.chromatic_pitch = 11;
    MENU.drop_input_token();
    user_selected_scale(major_scale);
    PULSES.time_unit=1000000;	// switch to metric time unit
    musicBoxConf.pitch.divisor=196; // 196.00	// G3  ***not*** harmonical
    break;

  case 'a':	// a minor scale
    musicBoxConf.chromatic_pitch = 1;
    MENU.drop_input_token();
    PULSES.time_unit=1000000;	// switch to metric time unit
    musicBoxConf.pitch.divisor = 440;
    user_selected_scale(minor_scale);
    break;

  case 'A':	// A major scale
    musicBoxConf.chromatic_pitch = 1;
    MENU.drop_input_token();
    PULSES.time_unit=1000000;	// switch to metric time unit
    musicBoxConf.pitch.divisor = 440;
    user_selected_scale(major_scale);
    break;

  case 'b':	// b minor
    musicBoxConf.chromatic_pitch = 3;
    MENU.drop_input_token();
    user_selected_scale(minor_scale);
    PULSES.time_unit=1000000;	// switch to metric time unit
    musicBoxConf.pitch.divisor=247; // 246.94	// B3  ***not*** harmonical
    break;

  case 'B':	// b major
    musicBoxConf.chromatic_pitch = 3;
    MENU.drop_input_token();
    user_selected_scale(major_scale);
    PULSES.time_unit=1000000;	// switch to metric time unit
    musicBoxConf.pitch.divisor=247; // 246.94	// B3  ***not*** harmonical
    break;
  }

  switch (MENU.peek()) {	// (second or) third letters for other scales
  case EOF8:
    break;

  case '0':	// europ_PENTAtonic ('0' just as it is free)	// shortcut europ_PENTAtonic
    MENU.drop_input_token();
    user_selected_scale(europ_PENTAtonic);
    break;
  case '9':	// pentaCHORD ('9' just as it is free)
    MENU.drop_input_token();
    user_selected_scale(pentaCHORD);
    break;
  case '8':	// major scale	// analog '7' (which is handy for morse input)
    MENU.drop_input_token();
    user_selected_scale(major_scale);
    break;
  case '7':	// minor scale	// avoid minuscules in morse: type 'E7' instead of 'e'
    MENU.drop_input_token();
    user_selected_scale(minor_scale);
    break;
  case '6':	// doric scale	// TODO: check 14
    MENU.drop_input_token();
    user_selected_scale(doric_scale);
    break;
  case '5':	// 5  pentatonic minor scale
    MENU.drop_input_token();
    user_selected_scale(pentatonic_minor);
    break;
  case '4':	// 4  tetraCHORD | tetrachord
    MENU.drop_input_token();
    if ((selected_in(SCALES) == minor_scale) || (selected_in(SCALES) == pentatonic_minor) || \
	(selected_in(SCALES) == doric_scale) || (selected_in(SCALES) == triad))
      user_selected_scale(tetrachord);
    else
      user_selected_scale(tetraCHORD);
    break;
  case '3':	// 3  octaves fourths fifths
    MENU.drop_input_token();
    user_selected_scale(octave_4th_5th);
    break;
  case '2':	// 2  octaves fifths
    MENU.drop_input_token();
    user_selected_scale(octaves_fifths);
    break;
  case '1':	// 1  octaves
    MENU.drop_input_token();
    user_selected_scale(octaves);
    break;
  }

  if(selected_in(SCALES) != scale_was)
    not_a_preset();
} // select_scale__UI()


void multiply_musicBox_periods(unsigned int mul) {
  if (mul >1) {
    for (int pulse=0; pulse<PL_MAX; pulse++) {
      if (PULSES.pulse_is_selected(pulse))
	PULSES.multiply_period(pulse, mul);
      else {	// not selected, test for butler (if it was *not* selected)
	if(pulse == musicBox_butler_i) {	// pulse can *not* be ILLEGAL32 here
	  if(PULSES.pulses[musicBox_butler_i].payload == &musicBox_butler) {	// butler found
	    PULSES.multiply_period(musicBox_butler_i, mul);
	    if(MENU.verbosity >= VERBOSITY_SOME)
	      MENU.outln(F("'*' butler included"));
	  }
	}
      }
    }

    PULSES.fix_global_next();
    not_a_preset();
    stress_event_cnt = -2;	// stress events possible after '*'
    stress_count = 0;

    musicBoxConf.pitch.multiplier *= mul;
    (*HARMONICAL).reduce_fraction(&musicBoxConf.pitch);
  }
} // multiply_musicBox_periods()


void divide_musicBox_periods(unsigned int div) {
  if (div > 1) {
    for (int pulse=0; pulse<PL_MAX; pulse++) {
      if (PULSES.pulse_is_selected(pulse))
	PULSES.divide_period(pulse, div);
      else {	// not selected, test for butler (if it was *not* selected)
	if(pulse == musicBox_butler_i) {	// pulse can *not* be ILLEGAL32 here
	  if(PULSES.pulses[musicBox_butler_i].payload == &musicBox_butler) {	// butler found
	    PULSES.divide_period(musicBox_butler_i, div);
	    if(MENU.verbosity >= VERBOSITY_SOME)
	      MENU.outln(F("'/' butler included"));
	  }
	}
      }
    } // pulse

    PULSES.fix_global_next();
    not_a_preset();
    stress_event_cnt = -5;	// *heavy* stress events expected after '/'
    stress_count = 0;

    musicBoxConf.pitch.divisor *= div;
    (*HARMONICAL).reduce_fraction(&musicBoxConf.pitch);
  }
} // divide_musicBox_periods()


bool menu_pulses_reaction(char menu_input) {
  static unsigned long input_value=0;	// static???	// TODO: OBSOLETE? see: new_input
  static long calc_result=0;
  long new_input;	// static ???
  pulse_time_t time_scratch;
  char next_token;	// for multichar commands

  switch (menu_input) {
  case '?':	// help, overrides common menu entry for '?'
    do_on_other_core(&menu_pulses_display);	// TODO: TEST: was: MENU.menu_display();  // menu as common
    do_on_other_core(&all_pulses_info);		// + all_pulses_info();
    break;

  case '.':	// ".xxx" select 16bit pulses masks  or  "." short info: time and flagged pulses info
    switch (MENU.peek()) {
    case ' ':
      MENU.drop_input_token();	// ' ' no 'break;'  display all_pulses_info

    case EOF8:			// '.' and '. ' (and ' ') display all_pulses_info
      do_on_other_core(&all_pulses_info);	// all_pulses_info();
      break;

    case '?':			// '.?' PULSES.show_selected_mask();
      MENU.drop_input_token();
      PULSES.show_selected_mask();
      break;

    case 'M':  case 'm':	// '.M<num>' select HEX 16bit mask
      MENU.drop_input_token();
      new_input = MENU.calculate_input(PULSES.hex_input_mask_index);
      if (new_input >= 0) {
	if (new_input < (PULSES.selection_masks() * sizeof(pulses_mask_t) / 2))	// two bytes 16bit hex masks
	  PULSES.hex_input_mask_index = new_input;
	else
	  MENU.outln_invalid();
      } else
	MENU.outln_invalid();

      PULSES.show_selected_mask();
      break;

    case '~':	// '.~' invert destination selection
      MENU.drop_input_token();
      for (int pulse=0; pulse<PL_MAX; pulse++)
	PULSES.toggle_selection(pulse);
      PULSES.show_selected_mask();
      break;

    case 'x':	// '.x' clear destination selection	also on 'x'
      MENU.drop_input_token();
      PULSES.clear_selection();
      PULSES.show_selected_mask();
      break;

    case 'a':	// '.a' select_flagged
      MENU.drop_input_token();
      select_flagged();

      PULSES.show_selected_mask();
      if (DO_or_maybe_display(VERBOSITY_LOWEST))
	selected_or_flagged_pulses_info_lines();
      break;

    case 'A':	// '.A' select destination: *all* pulses
      MENU.drop_input_token();
      select_all();
      PULSES.show_selected_mask();
      if (DO_or_maybe_display(VERBOSITY_LOWEST))
	selected_or_flagged_pulses_info_lines();
      break;

    case 'v':	// '.v' select destination: select_n(voices)
      MENU.drop_input_token();
      PULSES.select_n(voices);
      if (DO_or_maybe_display(VERBOSITY_LOWEST))
	selected_or_flagged_pulses_info_lines();
      break;

    case 'V':	// '.V' select destination: alive voices
      MENU.drop_input_token();
      PULSES.clear_selection();
      for (int pulse=0; pulse<voices; pulse++)
	if(PULSES.pulses[pulse].flags && (PULSES.pulses[pulse].flags != SCRATCH))
	  PULSES.select_pulse(pulse);

      PULSES.show_selected_mask();
      if (DO_or_maybe_display(VERBOSITY_LOWEST))
	selected_or_flagged_pulses_info_lines();
      break;

    case 'L':	// '.L' select destination: all alive pulses
      MENU.drop_input_token();
      select_alive();
      PULSES.show_selected_mask();
      if (DO_or_maybe_display(VERBOSITY_LOWEST))
	selected_or_flagged_pulses_info_lines();
      break;
    }
    break;

/*
  case ':':	// reserved for MENU_MENU_PAGES_KEY
    break;
*/

  case ',':	// in normal mode show_UI_settings and selected_mask, else data input delimiter	see: 'menu_mode'
    if (MENU.menu_mode==JIFFLE_ENTRY_UNTIL_ZERO_MODE)	// TODO: REVIEW:
      display_jiffletab(selected_in(JIFFLES));
    else {
      show_UI_settings();
      PULSES.show_selected_mask();	// if called from here (not musicBox) *do* show selected mask
    }
    break;

  case '|':	// accept as noop in normal mode. used as range bottom delimiter in arrays
    // TODO: '|' pulses vs musicbox *clash*
    if (MENU.menu_mode==JIFFLE_ENTRY_UNTIL_ZERO_MODE) {
      jiffle_range_bottom = jiffle_write_index;
      fix_jiffle_range();

      if(MENU.peek()==EOF8)
	if (MENU.verbosity)
	  display_jiffletab(selected_in(JIFFLES));
    } else
      if (MENU.verbosity >= VERBOSITY_SOME) {
	MENU.out_noop();
	MENU.ln();
      }
    break;

  case '"':	// accept as reserved noop in normal mode. used as range top delimiter in arrays
    if (MENU.menu_mode==JIFFLE_ENTRY_UNTIL_ZERO_MODE) {
      jiffle_range_top = jiffle_write_index;
      fix_jiffle_range();

      if(MENU.peek()==EOF8)
	if (MENU.verbosity)
	  display_jiffletab(selected_in(JIFFLES));
    } else
      if (MENU.verbosity >= VERBOSITY_SOME)
	MENU.outln(F("reserved"));	// reserved for string input
    break;

  // in normal mode toggle pulse selection with chiffres
  // else input data. see 'menu_mode'
  // (overwritten in musicBox menu by direct numeric preset input)
  case '0':	// toggle selection
  case '1':	// toggle selection
  case '2':	// toggle selection
  case '3':	// toggle selection
  case '4':	// toggle selection
  case '5':	// toggle selection
  case '6':	// toggle selection
  case '7':	// toggle selection
  case '8':	// toggle selection
  case '9':	// toggle selection
    switch (MENU.menu_mode) {
    case 0:	// normal input, no special menu_mode
      menu_input -= '0';
      menu_input += (PULSES.hex_input_mask_index * 16);
      if((menu_input < 0) || (menu_input >= PL_MAX))
	return false;		// *only* responsible if pulse exists

      // existing pulse:
      PULSES.toggle_selection(menu_input);

      PULSES.show_selected_mask();
      break;

    case JIFFLE_ENTRY_UNTIL_ZERO_MODE:	// first chiffre already seen
      MENU.restore_input_token();
      input_value = MENU.numeric_input(0);
      if (input_value)
	set_jiffle_RAM_value(input_value);

      else	// zero stops the input mode
	set_jiffle_RAM_value_0_stop();
      break;
    }
    break;

  case 'a':	// hex toggle selection
  case 'b':	// hex toggle selection
  case 'c':	// hex toggle selection
  case 'd':	// hex toggle selection
  case 'e':	// hex toggle selection
  case 'f':	// hex toggle selection
    switch (MENU.menu_mode) {
    case 0:	// normal input, no special menu_mode
      menu_input -= 'a';
      menu_input += 10;	// 'a' == 10
      menu_input += (PULSES.hex_input_mask_index * 16);
      if((menu_input < 0) || (menu_input >= PL_MAX))
	return false;		// *only* responsible if pulse exists

      // existing pulse:
      PULSES.toggle_selection(menu_input);

      PULSES.show_selected_mask();
      break;

    default:	// 'a' to 'f' keys not used in other menu modes
      if (MENU.verbosity >= VERBOSITY_SOME) {
	MENU.out_noop();
	MENU.ln();
      }
    }

    break;

  case '<':	// cursor left
    switch (MENU.menu_mode) {
    case JIFFLE_ENTRY_UNTIL_ZERO_MODE:
      if (jiffle_write_index)
	jiffle_write_index--;

      if(MENU.peek()==EOF8)
	if (MENU.verbosity)
	  display_jiffletab(selected_in(JIFFLES));
      break;
    default:
      if (MENU.verbosity >= VERBOSITY_SOME) {
	MENU.out_noop();
	MENU.ln();
      }
    }
    break;

  case '>':	// cursor right
    switch (MENU.menu_mode) {
    case JIFFLE_ENTRY_UNTIL_ZERO_MODE:
      if (++jiffle_write_index >= (CODE_RAM_SIZE - 2))
	jiffle_write_index = CODE_RAM_SIZE - 2;

      if(MENU.peek()==EOF8)
	if (MENU.verbosity)
	  display_jiffletab(selected_in(JIFFLES));
      break;
    default:
      if (MENU.verbosity >= VERBOSITY_SOME) {
	MENU.out_noop();
	MENU.ln();
      }
    }
    break;

  case 'u':	// PULSES.time_unit calculate or else select time_unit as destination
    {
      unsigned long input_value=PULSES.time_unit;
      if (MENU.maybe_calculate_input(&input_value)) {
	PULSES.time_unit = input_value;
	MENU.out(F("time_unit "));
	MENU.outln(PULSES.time_unit);
      }
      else
	dest = CODE_TIME_UNIT;		// FIXME: obsolete?
    }
    break;

  case 'x':	// clear destination selection  same as '.x'
    PULSES.clear_selection();
    PULSES.show_selected_mask();
    break;

  case 's':	// switch pulse ACTIVE on/off
    for (int pulse=0; pulse<PL_MAX; pulse++) {
      if (PULSES.pulse_is_selected(pulse)) {
	// special case: switching on an edited SCRATCH pulse:
	if((PULSES.pulses[pulse].flags & ACTIVE) == 0)	// was off
	  if (PULSES.pulses[pulse].flags & SCRATCH)	// SCRATCH set, like activating after edit
	    PULSES.pulses[pulse].flags &= ~SCRATCH;	// so we remove SCRATCH

	PULSES.pulses[pulse].flags ^= ACTIVE;

	if (PULSES.pulses[pulse].flags & ACTIVE) {	// DADA test ################
	  PULSES.get_now();	// ################################################################
	  PULSES.pulses[pulse].next = PULSES.now;
	  PULSES.pulses[pulse].last = PULSES.pulses[pulse].next;	// for overflow logic
	}
      }
    }
    PULSES.fix_global_next();

    if (DO_or_maybe_display(VERBOSITY_MORE)) {
      MENU.ln();
      selected_or_flagged_pulses_info_lines();
    }
    break;

  case 'S':	// enter sync
    new_input = MENU.calculate_input(musicBoxConf.sync);
    if (new_input>=0 ) {
      musicBoxConf.sync = new_input;
      sync_user_selected = true;
      not_a_preset();
    } else
      MENU.out(F("positive integer only\t"));

    if (DO_or_maybe_display(VERBOSITY_LOWEST)) {
      char txt[9]= {0};
      char* format = F("sync %i");		// TODO: | and [p] if appropriate?
      snprintf(txt, 9, format, musicBoxConf.sync);
      extern uint8_t /*next_row*/ extended_output(char* data, uint8_t col=0, uint8_t row=0, bool force=false);
      extended_output(txt, 0, 0, false);
      MENU.ln();
    }

    break;

  case 'i':	// en_info	TODO: free for '*' en_iCode_thrower
    for (int pulse=0; pulse<PL_MAX; pulse++)
      if (PULSES.pulse_is_selected(pulse))
	en_info(pulse);

    if (DO_or_maybe_display(VERBOSITY_LOWEST)) {
      MENU.ln();
      selected_or_flagged_pulses_info_lines();
    }
    break;
/*
  // RESERVED
  case 'I':	// iCode
    // icode_user_selected = true;
    break;
*/
  case 'M':	// selected_toggle_actions()	was: "mute", see 'N' as alternative
    muting_actions_UI();
    break;

  case '*':	// multiply destination
    if(MENU.peek() != '!') {		// '*' (*not* '*!<num>' set multiplier)
      switch (dest) {
      case CODE_PULSES:
	input_value = MENU.numeric_input(1);
	if (input_value > 1) {
	  multiply_musicBox_periods(input_value);

	  if (DO_or_maybe_display(VERBOSITY_MORE)) {	// TODO: '*' maybe VERBOSITY_SOME
	    MENU.ln();
	    selected_or_flagged_pulses_info_lines();
	  }
	} else
	  MENU.outln_invalid();
	break;

      case CODE_TIME_UNIT:
	input_value = MENU.numeric_input(1);
	if (input_value>0) {
	  PULSES.set_time_unit_and_inform(PULSES.time_unit * input_value);
	  not_a_preset();
	} else
	  MENU.outln_invalid();
	break;
      }
    } else {		// '*!<num>' set multiplier
      MENU.drop_input_token();
      if(MENU.peek()==EOF8)
	MENU.outln(F("multiplier"));

      input_value = MENU.numeric_input(musicBoxConf.pitch.multiplier);
      if (input_value>0 ) {
	musicBoxConf.pitch.multiplier = input_value;
      } else
	MENU.outln(F("small positive integer only"));

      if (DO_or_maybe_display(VERBOSITY_LOWEST)) {
	show_scaling();
	MENU.ln();
      }
    }
    break;

 case '/':	// '/' divide destination  '/!<num>' set divisor
   if(MENU.check_next('!')) {		// '/!<num>' set divisor
      if(MENU.peek()==EOF8)
	MENU.outln(F("divisor"));

      input_value = MENU.numeric_input(musicBoxConf.pitch.divisor);
      if (input_value>0 ) {
	musicBoxConf.pitch.divisor = input_value;
	not_a_preset();
	stress_event_cnt = -6;	// *heavy* stress events expected after '/!'	// TODO: TEST:
	stress_count = 0;
      } else
	MENU.outln(F("small positive integer only"));

      if (DO_or_maybe_display(VERBOSITY_LOWEST)) {
	show_scaling();
	MENU.ln();
      }

    } else {	// '/' divide destination
      switch (dest) {
      case CODE_PULSES:
	input_value = MENU.numeric_input(1);
	if (input_value > 1) {
	  divide_musicBox_periods(input_value);
	  if (DO_or_maybe_display(VERBOSITY_MORE)) {
	    MENU.ln();
	    selected_or_flagged_pulses_info_lines();
	  }
	} else
	  MENU.outln_invalid();
	break;

      case CODE_TIME_UNIT:
	input_value = MENU.numeric_input(1);
	if (input_value>0) {
	  PULSES.set_time_unit_and_inform(PULSES.time_unit / input_value);
	  not_a_preset();
	} else
	  MENU.outln_invalid();
	break;
      }
    }
    break;

  case '=':	// set destination to value
    switch (dest) {
    case CODE_PULSES:
      input_value = MENU.numeric_input(1);
      if (input_value>=0) {
	for (int pulse=0; pulse<PL_MAX; pulse++)
	  if (PULSES.pulse_is_selected(pulse)) {
	    time_scratch = PULSES.simple_time(PULSES.time_unit);
	    PULSES.mul_time(&time_scratch, input_value);
	    PULSES.set_new_period(pulse, time_scratch);
	  }

	PULSES.fix_global_next();
	not_a_preset();
	stress_event_cnt = -4;	// heavy stress events expected after '='
	stress_count = 0;

	if (DO_or_maybe_display(VERBOSITY_MORE)) {
	  MENU.ln();
	  selected_or_flagged_pulses_info_lines();
	}
      } else
	MENU.outln_invalid();
      break;

    case CODE_TIME_UNIT:
      input_value = MENU.numeric_input(1);
      if (input_value>0) {
	PULSES.set_time_unit_and_inform(input_value);
	not_a_preset();
      } else
	MENU.outln_invalid();
      break;
    }
    break;

  case 'K':	// kill selected pulses
    if (PULSES.how_many_selected()) {
      MENU.out(F("kill pulse "));
      for (int pulse=0; pulse<PL_MAX; pulse++)
	if (PULSES.pulse_is_selected(pulse)) {
	  PULSES.init_pulse(pulse);
	  if (MENU.maybe_display_more()) {
	    MENU.out(pulse);
	    MENU.space();
	  }
	}
      PULSES.fix_global_next();
      PULSES.check_maybe_do();	// maybe do it *first*

      MENU.ln();
    }

    if (DO_or_maybe_display(VERBOSITY_SOME)) {
      selected_or_flagged_pulses_info_lines();
      MENU.ln();
    }
    break;

  case 'P':	// pulse create and edit
    PULSES.reset_and_edit_selected();
    if (DO_or_maybe_display(VERBOSITY_LOWEST)) {
      MENU.ln();
      selected_or_flagged_pulses_info_lines();
    }
    break;

  case 'n':	// synchronize to now
    // see also '!'	one of 'n' and '!' is obsolete...
    PULSES.activate_selected_synced_now(musicBoxConf.sync);	// sync and activate

    if (DO_or_maybe_display(VERBOSITY_HIGH)) {
      MENU.ln();
      selected_or_flagged_pulses_info_lines();
    }
    break;

  case 'N':	// NULLs payload
    // we work on pulses anyway, regardless dest
    for (int pulse=0; pulse<PL_MAX; pulse++)
      if (PULSES.pulse_is_selected(pulse)) {
	if ((PULSES.pulses[pulse].flags & HAS_GPIO) && (PULSES.pulses[pulse].gpio != ILLEGAL8))
	  digitalWrite(PULSES.pulses[pulse].gpio, LOW);	// set clicks on LOW
	PULSES.set_payload(pulse, NULL);
      }

    if (DO_or_maybe_display(VERBOSITY_SOME)) {
      MENU.ln();
      selected_or_flagged_pulses_info_lines();
    }
    break;

  case 'g':	// 'g' "GPIO"	'g~' toggle up/down pin mapping
    if(MENU.check_next('~')) {	      // 'g~' toggle up/down pin mapping
      if (MENU.verbosity)
	MENU.out(F("pin mapping bottom "));

      g_inverse = !g_inverse;	// toggle bottom up/down click-pin mapping

      if (DO_or_maybe_display(VERBOSITY_LOWEST)) {
	if (g_inverse)
	  MENU.outln(F("up"));
	else
	  MENU.outln(F("down"));
      }
    } else
      en_click_selected();	// 'g' en_click  "GPIO"
    break;

#ifdef IMPLEMENT_TUNING		// implies floating point
  case 'w':	// sweep activate or toggle direction
    sweep_up *= -1;	// toggle direction up down

    if (sweep_up==0)	// start sweeping if it was disabled
      sweep_up=1;

    if (DO_or_maybe_display(VERBOSITY_LOWEST))
      sweep_info();

    break;

  case 'W':	// sweep info and control
    next_token = MENU.peek();
    if (next_token == EOF8) {	// *no* input after 'W': maybe start, info
      if (sweep_up==0)
	sweep_up=1;		//    start sweeping up if disabled

      if (DO_or_maybe_display(VERBOSITY_LOWEST))
	sweep_info();
      break;		// done
    }
    // there *is* input after 'W'

    // 'W<number>' does (calculating) positive integer input on PULSES.ticks_per_octave
    // exception: 'W0' switches sweeping off
    if (MENU.peek()!='0' && MENU.maybe_calculate_input((unsigned long*) &PULSES.ticks_per_octave)) {	// hmmm !!!
      ticks_per_octave = PULSES.ticks_per_octave;	// FIXME: obsolete
      MENU.out(PULSES.ticks_per_octave);
      MENU.outln(F(" ticks/octave"));
    } else {	// no numeric input (except '0') follows 'W'
      next_token = MENU.peek();
      switch(next_token) {	// examine following input token
      case '~': case 't':	// 'W~' 'Wt'  start sweep or toggle direction
	MENU.drop_input_token();
	if(sweep_up==0)			// start if not active
	  sweep_up = 1;
	else
	  sweep_up *= -1;		// or toggle sweep direction up down
	break;
      case '0':			// 'W0' switches sweeping off
	MENU.drop_input_token();
	sweep_up = 0;			// sweep off
	break;
      case '+': case '1':	// 'W+' 'W1"  sweep up
	MENU.drop_input_token();
	sweep_up = 1;			// sweep up
	break;
      case '-':			// 'W-' sweep down
	MENU.drop_input_token();
	sweep_up = -1;			// sweep down
	break;
      case '?':			// 'W?' info only
	MENU.drop_input_token();
	// if verbosity is too low sweep_info will not be called below,
	// so we do it here
	if (MENU.verbosity < VERBOSITY_SOME)
	  sweep_info();
	break;
      }
    }

    if (DO_or_maybe_display(VERBOSITY_LOWEST))
      sweep_info();
    break;

  case 't':	// en_sweep_click
    // we work on voices anyway, regardless dest
    for (int pulse=0; pulse<voices; pulse++)
      if (PULSES.pulse_is_selected(pulse))
	en_sweep_click(pulse);

    PULSES.fix_global_next();	// just in case?

    if (DO_or_maybe_display(VERBOSITY_MORE)) {
      MENU.ln();
      selected_or_flagged_pulses_info_lines();
    }

    break;

  case 'o':	// en_sweep_click_0
    // we work on voices anyway, regardless dest
    for (int pulse=0; pulse<voices; pulse++)
      if (PULSES.pulse_is_selected(pulse))
	en_sweep_click_0(pulse);

    PULSES.fix_global_next();	// just in case?

    if (DO_or_maybe_display(VERBOSITY_MORE)) {
      MENU.ln();
      selected_or_flagged_pulses_info_lines();
    }

    break;

  case 'p':	// en_tuned_sweep_click
    // we work on voices anyway, regardless dest
    for (int pulse=0; pulse<voices; pulse++)
      if (PULSES.pulse_is_selected(pulse))
	en_tuned_sweep_click(pulse);

    PULSES.fix_global_next();	// just in case?

    if (DO_or_maybe_display(VERBOSITY_MORE)) {
      MENU.ln();
      selected_or_flagged_pulses_info_lines();
    }

    break;

  case 'T':	// 'T<integer-number>' sets tuning, 'T' toggles TUNED	// see: Y_UI
    input_value = (unsigned long) PULSES.tuning;
    if (MENU.maybe_calculate_input(&input_value))	{	// T1 sets tuning to 1.0
      if (input_value > 0)
	PULSES.tuning = (double) input_value;	// TODO: (double) input, (double) calculations
      tuning_info();
      MENU.ln();
    } else {	// toggle TUNED on selected pulses
      // we work on voices anyway, regardless dest
      for (int pulse=0; pulse<voices; pulse++)
	if (PULSES.pulse_is_selected(pulse)) {
	  if (PULSES.pulses[pulse].flags & TUNED)
	    PULSES.stop_tuning(pulse);
	  else
	    PULSES.activate_tuning(pulse);
	}
    }

    PULSES.fix_global_next();	// just in case?

    if (DO_or_maybe_display(VERBOSITY_MORE)) {
      MENU.ln();
      selected_or_flagged_pulses_info_lines();
    }
    break;
#endif	// #ifdef IMPLEMENT_TUNING	implies floating point

  case 'j':	// en_jiffle_thrower	// maybe OBSOLETE?, as select_jiffle_UI() does this now
    setup_jiffle_thrower_selected(selected_actions);
    break;

  case 'J':	// select, edit, test, load jiffle
    select_jiffle_UI();
    break;

  case 'Q':	// select, edit, test, load iCode
    select_iCode_UI();
    break;

  // 'R' OBSOLETE????	################
  case 'R':	// scale  was: ratio	// TODO: see musicBox 'R'	make them identical? ################
    if (DO_or_maybe_display(VERBOSITY_SOME))
      MENU.out(F("scale "));

    // 'R!' tune selected pulses to a scale starting from lowest
    if (MENU.peek()=='!') {
      // tune-2-scale FIXME: *selected*  // TODO: OBSOLETE?
      tune_2_scale(voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, selected_in(SCALES));
      scale_user_selected = true;
    }
    else	// ui select a scale
      if(UI_select_from_DB(SCALES))	// select scale
	scale_user_selected = true;

    if (DO_or_maybe_display(VERBOSITY_LOWEST))
      display_arr(selected_in(SCALES), 2);

    break;

  case 'F':	// en_INFO
    // we work on pulses anyway, regardless dest
    for (int pulse=0; pulse<PL_MAX; pulse++)
      if (PULSES.pulse_is_selected(pulse))
	en_INFO(pulse);

    if (DO_or_maybe_display(VERBOSITY_MORE)) {
      MENU.ln();
      selected_or_flagged_pulses_info_lines();
    }
    break;

  case '{':	// enter_jiffletab
    MENU.menu_mode=JIFFLE_ENTRY_UNTIL_ZERO_MODE;
    select_in(JIFFLES, code_RAM);
    jiffle_write_index=0;	// ################ FIXME: ################
    select_in(JIFFLES, code_RAM);
    if(MENU.peek()==EOF8)
      if (MENU.verbosity)
	display_jiffletab(selected_in(JIFFLES));
    break;

  case '}':	// display jiffletab, stop editing jiffletab
    MENU.menu_mode=0;
    if (MENU.verbosity)
      display_jiffletab(selected_in(JIFFLES));

    jiffle_write_index=0;	// ################ FIXME: ################
    break;

  case 'C':	// Calculator simply *left to right*
    calc_result = MENU.calculate_input(calc_result);

    MENU.out(F("Calc => ")), MENU.outln(calc_result);
    if (calc_result > 1) {	// show prime factors if(result >= 2)
      int p_factors_size=6;	// for harmonics I rarely use more than three, sometimes four ;)
      unsigned int p_factors[p_factors_size];
      MENU.out(F("prime factors of ")); MENU.out(calc_result);
      int highest = (*HARMONICAL).prime_factors(p_factors, calc_result);
      for (int i=0; (*HARMONICAL).small_primes[i]<=highest; i++) {
	MENU.tab();
	MENU.out(F("("));
	MENU.out((*HARMONICAL).small_primes[i]);
	MENU.out(F(")^"));
	MENU.out(p_factors[i]);
      }
      MENU.ln();
    }
    break;

  case 'D':	// DADA reserved for temporary code   testing debugging ...
    // MENU.out_noop(); MENU.ln();
    #include "menu_D_tmp.h"
    break;

  case 'y':	// DADA reserved for temporary code   testing debugging ...
    // MENU.out_noop(); MENU.ln();
    #include "menu_y_tmp.h"
    break;

  case 'z':	// DADA reserved for temporary code   testing debugging ...
    // MENU.out_noop(); MENU.ln();
    #include "menu_z_tmp.h"
    break;

  case 'V':	// set voices	and do PULSES.select_n_voices
    if(MENU.check_next('V')) {	// 'VV' PULSES.voices
      MENU.out(F("voices "));

      new_input = MENU.calculate_input(voices);
      if (new_input>0 && new_input<=PL_MAX)
	voices = new_input;
      else
	MENU.outln_invalid();

      if (voices==0)
	voices=36;	// just a guess (was: voices=GPIO_PINS;)
      PULSES.select_n(voices);

      MENU.outln(voices);

#if defined USE_MIDI
    } else if(MENU.check_next('M')) {	// 'VM' PULSES.MIDIvolume
      float input_f = MENU.float_input(PULSES.MIDI_volume);
      if(input_f > 1.0)
	input_f = 1.0;
      if(input_f < 0)
	input_f = 0.0;
      PULSES.MIDI_volume = input_f;
      MENU.out(F("global MIDI volume "));
      MENU.outln(PULSES.MIDI_volume);
#endif

    } else { // bare 'V' and 'VE' (*NOT* 'VVx')	PULSES.volume

      if(MENU.check_next('E'))	// 'VE' (morse shortcut) reset PULSES.volume=1.0
	PULSES.volume=1.0;
      else if(MENU.peek() == 'T')// 'VT[TT]' (morse shortcut) fade volume
	while(MENU.check_next('T')) {
	  PULSES.volume *= 0.7;
	}
      else {			// 'V<nnn>' PULSES.volume (0..DAx_max)
	long inp = MENU.calculate_input((long) (PULSES.volume*DAx_max + 0.5));  // given as 0...DAx_max	// TODO: float input
	input_value = (int) inp;
	if(input_value > DAx_max)
	  input_value = DAx_max;
	if(input_value < 0)
	  input_value = 0;
	if(input_value >=0)
	  PULSES.volume = ((float) input_value) / (float) DAx_max;
      }

      MENU.out(F("global audio volume "));
      MENU.outln(PULSES.volume);
    } // PULSES.volume (or voices)
    break;

  case 'O':	// configure selected_actions
    if (MENU.peek() == EOF8) {
      MENU.out(F("action flags "));
      PULSES.show_action_flags(selected_actions);
      MENU.ln();
    } else {
      input_value = MENU.numeric_input(selected_actions);      // TODO: allow mnemonics ################
      if (input_value != selected_actions)
	selected_actions = input_value;

      if (MENU.verbosity >= VERBOSITY_SOME) {
	MENU.out(F("action flags "));
	PULSES.show_action_flags(selected_actions);
	MENU.ln();
      }
    }
    break;

  case 'A':	// A_UI()	// 'A...'  A_UI()  "A" is just a morse conveniant letter, *not* mnemonic
    A_UI();
    break;

  case 'X':	// PANIC BUTTON  reset, remove all (flagged) pulses, restart selections at none
    // reset, remove all (flagged) pulses, restart selections at none, reset selection mask
    // set MCP23017 pins low

    tabula_rasa();

    if(MENU.check_next('!')) {		// 'X!' does 'X' *and* resets time_unit
      if(PULSES.time_unit != TIME_UNIT) {
	MENU.outln(F("reset time_unit"));
	PULSES.time_unit = TIME_UNIT;	// reset time_unit
      }
    } else
      if(PULSES.time_unit != TIME_UNIT)	// info time_unit not reset
	MENU.outln(F("'X!' to reset time_unit"));

    break;

#if GPIO_PINS > 0
  case 'Z':	// reverse_gpio_pins
    reverse_gpio_pins();	// TODO: fix next_gpio()  ???? ################

    if (DO_or_maybe_display(VERBOSITY_LOWEST))
      MENU.outln(F("reverse_gpio_pins"));

    break;
#endif

  case 'E':	// experiment, setups, instruments
    if (MENU.maybe_display_more()) {
      MENU.out(F("experiment "));
    }

    input_value = MENU.numeric_input(-1);	// select experiment

    if (input_value==-1)
      display_names_OBSOLETE(experiment_names, n_experiment_names, selected_experiment);

    else if (input_value>=0 ) {	// select, initialize experiment
      selected_experiment = input_value;
      switch (selected_experiment) {
	/* some old style setups just initialise defaults,  but do not start yet, press '!'
	   others do start if you add a '!' as last char
	   you can edit and then press '!' or 'n' (== synchronise to *now*)
	   sorry, it's messy!	TODO: unmess
	*/

//      case 0:	// free setup planed
//	  break;

      case 1:	// setup_jiffle128
	musicBoxConf.pitch.multiplier=2;
	musicBoxConf.pitch.divisor=1;
	musicBoxConf.sync=15;

	if (MENU.maybe_display_more()) {
	  display_name5pars("setup_jiffle128", g_inverse, voices, \
			    musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
	  Press_toStart();
	}
	break;

      case 2:	// init_div_123456
	musicBoxConf.sync=0;
	musicBoxConf.pitch.multiplier=1;
	musicBoxConf.pitch.divisor=1;

	if (MENU.maybe_display_more()) {
	  display_name5pars("init_div_123456", g_inverse, voices, \
			    musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
	  Press_toStart();
	}
	break;

      case 3:	// setup_jiffles0
	musicBoxConf.sync=1;
	musicBoxConf.pitch.multiplier=8;
	musicBoxConf.pitch.divisor=3;
#if GPIO_PINS > 0
	reverse_gpio_pins();
#endif
	if (MENU.maybe_display_more()) {
	  MENU.out(F("setup_jiffles0("));
	  MENU.out(g_inverse);
	  display_next_par(voices);
	  display_next_par(musicBoxConf.pitch.multiplier);
	  display_next_par(musicBoxConf.pitch.divisor);
	  display_next_par(musicBoxConf.sync);
	  MENU.outln(F(")  ESP8266 Frogs"));
	  Press_toStart();
	}
	break;

      case 4:	// setup_jiffles2345
	musicBoxConf.pitch.multiplier=1;
	musicBoxConf.pitch.divisor=2;
	musicBoxConf.sync=0;
	select_in(JIFFLES, jiffletab);

	if (MENU.maybe_display_more()) {
	  display_name5pars("setup_jiffles2345", g_inverse, voices, \
			    musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
	  Press_toStart();
	}
	break;

      case 5:	// init_123456
	musicBoxConf.sync=0;		// FIXME: test and select ################
	musicBoxConf.pitch.multiplier=3;
	musicBoxConf.pitch.divisor=1;

	if (MENU.maybe_display_more()) {
	  display_name5pars("init_123456", g_inverse, voices, \
			    musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
	  Press_toStart();
	}
	break;

      case 6:	// init_chord_1345689a
	musicBoxConf.sync=0;		// FIXME: test and select ################
	musicBoxConf.pitch.multiplier=1;
	musicBoxConf.pitch.divisor=1;

	if (MENU.maybe_display_more()) {
	  display_name5pars("init_chord_1345689a", g_inverse, voices, \
			    musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
	  Press_toStart();
	}
	break;

      case 7:	// init_rhythm_1
	musicBoxConf.sync=1;
	musicBoxConf.pitch.multiplier=1;
	musicBoxConf.pitch.divisor=6*7;

	if (MENU.maybe_display_more()) {
	  display_name5pars("init_rhythm_1", g_inverse, voices, \
			    musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
	  Press_toStart();
	}
	break;

      case 8:	// init_rhythm_2
	musicBoxConf.sync=5;
	musicBoxConf.pitch.multiplier=1;
	musicBoxConf.pitch.divisor=1;

	if (MENU.maybe_display_more()) {
	  display_name5pars("init_rhythm_2", g_inverse, voices, \
			    musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
	  Press_toStart();
	}
	break;

      case 9:  // init_rhythm_3
	musicBoxConf.sync=3;
	musicBoxConf.pitch.multiplier=1;
	musicBoxConf.pitch.divisor=1;

	if (MENU.maybe_display_more()) {
	  display_name5pars("init_rhythm_3", g_inverse, voices, \
			    musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
	  Press_toStart();
	}
	break;

      case 10:	// init_rhythm_4
	musicBoxConf.sync=1;
	musicBoxConf.pitch.multiplier=1;
	musicBoxConf.pitch.divisor=7L*3L;

	if (MENU.maybe_display_more()) {
	  display_name5pars("init_rhythm_4", g_inverse, voices, \
			    musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
	  Press_toStart();
	}
	break;

      case 11:	// setup_jifflesNEW
	musicBoxConf.sync=3;
	musicBoxConf.pitch.multiplier=3;
	musicBoxConf.pitch.divisor=1;

	if (MENU.maybe_display_more()) {
	  display_name5pars("setup_jifflesNEW", g_inverse, voices, \
			    musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
	  Press_toStart();
	}
	break;

      case 12:	// init_pentatonic
	musicBoxConf.pitch.multiplier=1;
	musicBoxConf.pitch.divisor=1;

	select_in(SCALES, minor_scale);		// default e minor
	select_in(JIFFLES, piip2048);

	if (voices == 0)
	  voices = GPIO_PINS;

	PULSES.select_n(voices);
	tune_2_scale(voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, selected_in(SCALES));	// TODO: OBSOLETE?

  #ifndef USE_DACs	// TODO: review and use test code
	setup_jiffle_thrower_selected(selected_actions);
  #else // *do* use dac		// TODO: not here ################
	selected_share_DACsq_intensity(DAx_max, 1);

    #if (USE_DACs == 1)
	setup_jiffle_thrower_selected(DACsq1);
    #else
	selected_DACsq_intensity_proportional(DAx_max, 2);
//	selected_share_DACsq_intensity(DAx_max, 2);

	setup_jiffle_thrower_selected(DACsq1 | DACsq2);
    #endif
  #endif

	PULSES.activate_selected_synced_now(musicBoxConf.sync);	// sync and activate;
	MENU.ln();

	if (MENU.maybe_display_more())
	  selected_or_flagged_pulses_info_lines();

	break;


      case 13:	// prepare_magnets
	musicBoxConf.sync=1;	// or: musicBoxConf.sync=0;
	musicBoxConf.pitch.multiplier=1;
	musicBoxConf.pitch.divisor=1;
	voices=8;	//just for 'The Harmonical Strings Christmas Evening Sounds'
	g_inverse=false;
	// unsigned int harmonics4 = {1,1,1024, 1,2,1024, 1,3,1024, 1,4,1024, 0,0};
	select_in(JIFFLES, harmonics4);
	PULSES.select_n(voices);
	display_name5pars("prepare_magnets", g_inverse, voices, \
			  musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
	prepare_magnets(g_inverse, voices, \
			musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);

	if (MENU.maybe_display_more()) {
	  selected_or_flagged_pulses_info_lines();
	  Press_toStart();
	}
	break;

      case 14:	// E14
	// magnets on strings, second take
	musicBoxConf.pitch.multiplier=1;
	musicBoxConf.pitch.divisor=1;
	g_inverse=false;

	select_in(SCALES, pentatonic_minor);
	PULSES.select_n(voices);
	prepare_scale(false, voices, musicBoxConf.pitch.multiplier * 1024 , musicBoxConf.pitch.divisor * 1167, musicBoxConf.sync, selected_in(SCALES));
	select_in(JIFFLES, ting1024);
	PULSES.select_n(voices);
	display_name5pars("E14", g_inverse, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);

	if (MENU.maybe_display_more())
	  selected_or_flagged_pulses_info_lines();
	break;

      case 15:	// E15
	// magnets on strings, third take
	musicBoxConf.pitch.multiplier=1;
	musicBoxConf.pitch.divisor=1;
	g_inverse=false;

	select_in(SCALES, pentatonic_minor);
	PULSES.select_n(voices);
	prepare_scale(false, voices, musicBoxConf.pitch.multiplier * 4096 , musicBoxConf.pitch.divisor * 1167, musicBoxConf.sync, selected_in(SCALES));
	select_in(JIFFLES, ting4096);
	PULSES.select_n(voices);
	display_name5pars("E15", g_inverse, voices, \
			  musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);

	if (MENU.verbosity >= VERBOSITY_SOME)
	  selected_or_flagged_pulses_info_lines();
	break;

      case 16:	// E16 europ_PENTAtonic
	// piezzos on low strings 2016-12-28
	musicBoxConf.pitch.multiplier=4096;
	musicBoxConf.pitch.divisor=256;
	g_inverse=false;

	select_in(SCALES, europ_PENTAtonic);
	PULSES.select_n(voices);
	prepare_scale(false, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync, selected_in(SCALES));
	select_in(JIFFLES, ting4096);
	// select_in(JIFFLES, arpeggio4096);
	display_name5pars("E16 PENTAtonic", g_inverse, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);

	if (MENU.verbosity >= VERBOSITY_SOME)
	  selected_or_flagged_pulses_info_lines();
	break;

      case 17:	// E17 mimic japan
	// magnets on steel strings, "japanese"
	musicBoxConf.pitch.multiplier=1;	// click
	// multiplier=4096;	// jiffle ting4096
	musicBoxConf.pitch.divisor=256*5;

	select_in(SCALES, mimic_japan_pentatonic);
	PULSES.select_n(voices);
	prepare_scale(false, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync, selected_in(SCALES));
	select_in(JIFFLES, ting4096);
	display_name5pars("E17 mimic japan", g_inverse, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);

	if (MENU.verbosity >= VERBOSITY_SOME)
	  selected_or_flagged_pulses_info_lines();

	break;

      case 18:	// E18 pentatonic minor  nylon stringed wooden box, piezzos
	select_in(SCALES, pentatonic_minor);
	musicBoxConf.pitch.multiplier=1;	// click
	// musicBoxConf.pitch.multiplier=4096;	// jiffle ting4096
	// divisor=2048;

	// string tuning on 8/9
	//   musicBoxConf.pitch.multiplier=8;
	//   divisor=9*1024;
	// musicBoxConf.pitch.multiplier=1;
	// divisor=9*128;

	// musicBoxConf.pitch.multiplier=8*4096;	// jiffle ting4096
	// divisor=9*1024;
	musicBoxConf.pitch.multiplier=32;	// reduced
	//#if defined(ESP32)	// used as test setup with 16 clicks
	// ################ FIXME: ESP32 16 click ################
	//      multiplier *= 4;
	//#endif
	musicBoxConf.pitch.divisor=9;	// reduced
	select_in(JIFFLES, ting4096);

	PULSES.select_n(voices);
	prepare_scale(false, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync, selected_in(SCALES));
	display_name5pars("E18 pentatonic minor", g_inverse, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);

	if (MENU.verbosity >= VERBOSITY_SOME)
	  selected_or_flagged_pulses_info_lines();

	break;

#ifdef IMPLEMENT_TUNING		// implies floating point
      case 19:	// TUNING: tuned sweep
	PULSES.clear_selection();
	PULSES.select_pulse(0);
	sweep_up = 1;
	PULSES.reset_and_edit_pulse(0, PULSES.time_unit);
	PULSES.divide_period(0, 1024);
	en_tuned_sweep_click(0);

	PULSES.fix_global_next();	// just in case?
	PULSES.check_maybe_do();	// maybe do it *first*

	if (MENU.maybe_display_more()) {
	  MENU.ln();
	  selected_or_flagged_pulses_info_lines();
	}
	break;
#endif

      case 20:	// macro
	// was: MENU.play_KB_macro("X E12! .a N *8 J20-.");
	select_in(JIFFLES, arpeggio4096);		// TODO: test order
	MENU.play_KB_macro("X E12! .a N *8 -");
	select_in(JIFFLES, arpeggio4096);		// FIXME: TODO: test order
	break;

      case 21:	// macro
	// was: MENU.play_KB_macro("X E12! .a N *16 J21-.");
	select_in(JIFFLES, arpeggio4096down);		// TODO: test order
	MENU.play_KB_macro("X E12! .a N *16 -.");
	break;

      case 22:	// macro
	select_in(JIFFLES, arpeggio_cont);		// TODO: test order		// :)	with pizzs
	// was: MENU.play_KB_macro("X E12! .a N *16 J22-.");
	MENU.play_KB_macro("X E12! .a N *16 -.");
	break;

      case 23:	// macro
	select_in(JIFFLES, arpeggio_and_down);		// TODO: test order		// :) :)  arpeggio down instead pizzs
	// was: MENU.play_KB_macro("X E12! .a N *16 J23-.");
	MENU.play_KB_macro("X E12! .a N *16 -.");
	break;

      case 24:	// macro
	select_in(JIFFLES, stepping_down);		// TODO: test order		// :) :)  stepping down
	// was: MENU.play_KB_macro("X E12 S=0 ! .a N *16 J24-.");
	MENU.play_KB_macro("X E12 S=0 ! .a N *16 -.");
	break;

      case 25:	// macro
	select_in(JIFFLES, back_to_ground);		// TODO: test order		// rhythm slowdown
	// was: MENU.play_KB_macro("X E12! .a N *32 J25-.");		// :)	rhythm slowdown
	MENU.play_KB_macro("X E12! .a N *32 -.");		// :)	rhythm slowdown
	break;

      case 26:	// macro
	select_in(JIFFLES, arpeggio_and_sayling);		// TODO: test order
	// was: MENU.play_KB_macro("X E12! .a N *32 J26-.");
	MENU.play_KB_macro("X E12! .a N *32 -.");
	break;

      case 27:	// macro
	select_in(JIFFLES, simple_theme);
	MENU.play_KB_macro("X E12! .a N *2 -.");
	break;

      case 28:	// macro   for tuning
	select_in(JIFFLES, peepeep4096);
	MENU.play_KB_macro("X E12! .a N *2 -.");
	break;

      case 29:	// E29 KALIMBA7 tuning
	tabula_rasa();

#if defined KALIMBA7_v2	// ESP32 version  europ_PENTAtonic
	select_in(SCALES, europ_PENTAtonic);
	voices=7;
#else
	select_in(SCALES, pentatonic_minor);	// default, including KALIMBA7_v1
#endif
#if defined  KALIMBA7_v1
	voices=7;
#endif

	musicBoxConf.pitch.multiplier=1;
	musicBoxConf.pitch.divisor=1024;
	PULSES.select_n(voices);
	prepare_scale(false, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync, selected_in(SCALES));
	display_name5pars("E29 KALIMBA7 tuning", g_inverse, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
	en_click_selected();							// for tuning ;)
	PULSES.activate_selected_synced_now(musicBoxConf.sync);	// sync and activate
	MENU.ln();

	if (MENU.verbosity >= VERBOSITY_SOME)
	  selected_or_flagged_pulses_info_lines();

	break;

      case 30:	// KALIMBA7 jiff
	select_in(SCALES, pentatonic_minor);
	voices=7;
	// voices=8;
	musicBoxConf.pitch.multiplier=4;
	musicBoxConf.pitch.divisor=1;
	// select_in(JIFFLES, peepeep4096);
	select_in(JIFFLES, ting4096);
	// select_in(JIFFLES, tingeling4096);
	PULSES.select_n(voices);
	prepare_scale(false, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync, selected_in(SCALES));
	display_name5pars("E30 KALIMBA7 jiff", g_inverse, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
	setup_jiffle_thrower_selected(selected_actions);
	PULSES.activate_selected_synced_now(musicBoxConf.sync);	// sync and activate

	MENU.ln();

	if (MENU.verbosity >= VERBOSITY_SOME)
	  selected_or_flagged_pulses_info_lines();

	break;

      case 31:	// E31 KALIMBA7 jiff
	select_in(SCALES, europ_PENTAtonic);
	voices=7;
	musicBoxConf.pitch.multiplier=4;
	musicBoxConf.pitch.divisor=1;
	select_in(JIFFLES, ting4096);
	PULSES.select_n(voices);
	prepare_scale(false, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync, selected_in(SCALES));
	display_name5pars("E31 KALIMBA7 jiff", g_inverse, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
	setup_jiffle_thrower_selected(selected_actions);
	PULSES.activate_selected_synced_now(musicBoxConf.sync);	// sync and activate;
	MENU.ln();

	if (MENU.verbosity >= VERBOSITY_SOME)
	  selected_or_flagged_pulses_info_lines();

	break;

      case 32:	// ESP32_12 ff
	select_in(SCALES, major_scale);
	voices=GPIO_PINS;
	musicBoxConf.pitch.multiplier=4;
	musicBoxConf.pitch.divisor=1;
	// select_in(JIFFLES, ting4096);
	select_in(JIFFLES, tigg_ding4096);
	PULSES.select_n(voices);
	prepare_scale(false, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync, selected_in(SCALES));
	display_name5pars("E32 ESP32_12", g_inverse, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
	setup_jiffle_thrower_selected(selected_actions);
	PULSES.activate_selected_synced_now(musicBoxConf.sync);	// sync and activate;
	MENU.ln();

	if (MENU.verbosity >= VERBOSITY_SOME)
	  selected_or_flagged_pulses_info_lines();

	break;

      case 33:	// variation
	select_in(SCALES, minor_scale);
	voices=GPIO_PINS;
	musicBoxConf.pitch.multiplier=4;
	musicBoxConf.pitch.divisor=1;
	select_in(JIFFLES, ting4096);
	PULSES.select_n(voices);
	prepare_scale(false, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync, selected_in(SCALES));
	display_name5pars("minor", g_inverse, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
	setup_jiffle_thrower_selected(selected_actions);
	PULSES.activate_selected_synced_now(musicBoxConf.sync);	// sync and activate;
	MENU.ln();

	if (MENU.verbosity >= VERBOSITY_SOME)
	  selected_or_flagged_pulses_info_lines();

	break;

      case 34:	// same, major?
	select_in(SCALES, major_scale);
	voices=GPIO_PINS;
	musicBoxConf.pitch.multiplier=4;
	musicBoxConf.pitch.divisor=1;
	select_in(JIFFLES, ting4096);
	PULSES.select_n(voices);
	prepare_scale(false, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync, selected_in(SCALES));
	display_name5pars("major", g_inverse, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
	setup_jiffle_thrower_selected(selected_actions);
	PULSES.activate_selected_synced_now(musicBoxConf.sync);	// sync and activate;
	MENU.ln();

	if (MENU.verbosity >= VERBOSITY_SOME)
	  selected_or_flagged_pulses_info_lines();

	break;

      case 35:	// tetra
	select_in(SCALES, tetraCHORD);
	voices=GPIO_PINS;
	musicBoxConf.pitch.multiplier=4;
	musicBoxConf.pitch.divisor=1;
	select_in(JIFFLES, ting4096);
	PULSES.select_n(voices);
	prepare_scale(false, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync, selected_in(SCALES));
	display_name5pars("tetra", g_inverse, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
	setup_jiffle_thrower_selected(selected_actions);
	PULSES.activate_selected_synced_now(musicBoxConf.sync);	// sync and activate;
	MENU.ln();

	if (MENU.verbosity >= VERBOSITY_SOME)
	  selected_or_flagged_pulses_info_lines();

	break;

      case 36:	// BIG major
	select_in(SCALES, pentatonic_minor);
	voices=GPIO_PINS;
	musicBoxConf.pitch.multiplier=6;
	musicBoxConf.pitch.divisor=1;
	select_in(JIFFLES, ting4096);
	PULSES.select_n(voices);
	prepare_scale(false, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync, selected_in(SCALES));
	display_name5pars("BIG major", g_inverse, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
	setup_jiffle_thrower_selected(selected_actions);
	PULSES.activate_selected_synced_now(musicBoxConf.sync);	// sync and activate;
	MENU.ln();

	if (MENU.verbosity >= VERBOSITY_SOME)
	  selected_or_flagged_pulses_info_lines();

	break;

      case 37:	// Guitar and other instruments
	tabula_rasa();
	PULSES.time_unit=1000000;

	// default tuning e
	musicBoxConf.pitch.multiplier=4096;
	// musicBoxConf.pitch.divisor=440;			// a4
	musicBoxConf.pitch.divisor=330; // 329.36		// e4  ***not*** harmonical
	// musicBoxConf.pitch.divisor=165; // 164.81		// e3  ***not*** harmonical

	select_in(SCALES, minor_scale);	// default e minor

	select_in(JIFFLES, ting4096);	// default jiffle
	//	voices = 16;			// for DAC output
	if (voices == 0)
	  voices = 15;			// default (diatonic)	// for DAC output

	select_scale__UI();	// second/third letters choose metric scales	// OBSOLETE?:

	// select_in(JIFFLES, ting4096);
	// select_in(JIFFLES, piip2048);
	// select_in(JIFFLES, tanboura); divisor *= 2;

	// ################ FIXME: remove redundant code ################
	PULSES.select_n(voices);
//	prepare_scale(false, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, 0, selected_in(SCALES));
//	display_name5pars("GUITAR", g_inverse, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
	tune_2_scale(voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, selected_in(SCALES));	// TODO: OBSOLETE?
	lower_audio_if_too_high(409600);

  #ifndef USE_DACs	// TODO: review and use test code
	setup_jiffle_thrower_selected(selected_actions);
  #else // *do* use dac		// TODO: not here ################
	selected_share_DACsq_intensity(DAx_max, 1);

    #if (USE_DACs == 1)
	setup_jiffle_thrower_selected(DACsq1);
    #else
	selected_DACsq_intensity_proportional(DAx_max, 2);
//	selected_share_DACsq_intensity(DAx_max, 2);

	setup_jiffle_thrower_selected(DACsq1 | DACsq2);
    #endif
  #endif

	PULSES.activate_selected_synced_now(musicBoxConf.sync);	// sync and activate;
	MENU.ln();

	if (MENU.verbosity >= VERBOSITY_SOME)
	  selected_or_flagged_pulses_info_lines();

	break;

      default:
	if (MENU.verbosity >= VERBOSITY_SOME)
	  MENU.outln_invalid();

	selected_experiment=0;
	break;


      case 38:	// 'E38' time machine setup
	// ESP32_15_clicks_no_display_TIME_MACHINE1

	// if (voices == 0)	// maybe, maybe not...
	voices = 32;	// init *all* primary pulses
	PULSES.select_n(voices);

	PULSES.time_unit=1000000;	// TODO:  maybe, maybe not...

	// default tuning a
	musicBoxConf.pitch.multiplier=4096;	// bass octave added and one lower...
	//	musicBoxConf.pitch.divisor=440;		// a
	//	musicBoxConf.pitch.divisor=220;		// a
	musicBoxConf.pitch.divisor=110;		// a
	// musicBoxConf.pitch.divisor=55;		// a

	// musicBoxConf.pitch.divisor=330; // 329.36		// e4  ***not*** harmonical
	// musicBoxConf.pitch.divisor=165; // 164.81		// e3  ***not*** harmonical

	select_in(SCALES, minor_scale);		// default e minor
	select_scale__UI();	// second/third letters choose metric scales	// OBSOLETE?:

	// tune *all* primary pulses
	tune_2_scale(voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, selected_in(SCALES));	// TODO: OBSOLETE?
	lower_audio_if_too_high(409600);

	// prepare primary pulse groups:
	select_in(JIFFLES, d4096_512);

	// bass on DAC1 and planed broad angle LED lamps
	// select_in(JIFFLES, d4096_512);
	PULSES.select_from_to(0,6);
	for(int pulse=0; pulse<=6; pulse++) {
	  en_jiffle_thrower(pulse, selected_in(JIFFLES), ILLEGAL8, DACsq1);	// FIXME: use inbuilt click
	}
	PULSES.select_from_to(0,7);
	// selected_DACsq_intensity_proportional(DAx_max, 1);
	selected_share_DACsq_intensity(DAx_max, 1);

	// 2 middle octaves on 15 gpios
	// select_in(JIFFLES, d4096_512);
	//	select_in(JIFFLES, d4096_256);
	PULSES.select_from_to(7,6+15);
	setup_jiffle_thrower_selected(0);	// overwrites pulse[7]
	//	setup_jiffle_thrower_selected(DACsq2);
	//	selected_share_DACsq_intensity(DAx_max, 2);

	// fix pulse[7] that belongs to both groups:
	PULSES.pulses[7].dest_action_flags |= DACsq1;

	// high octave on DAC2
	//	select_in(JIFFLES, d4096_64);
	//select_in(JIFFLES, d4096_256);

	PULSES.select_from_to(21, 31);
	for(int pulse=22; pulse<=31; pulse++) {	// pulse[21] belongs to both groups
	  en_jiffle_thrower(pulse, selected_in(JIFFLES), ILLEGAL8, DACsq2);	// FIXME: use inbuilt click
//	  PULSES.pulses[pulse].dest_action_flags |= (DACsq2);
//	  PULSES.set_payload(pulse, &do_throw_a_jiffle);
//	  PULSES.pulses[pulse].data = (unsigned int) jiffle;
	}
	// fix pulse[21] belonging to both groups
	PULSES.pulses[21].dest_action_flags |= DACsq2;
	selected_share_DACsq_intensity(DAx_max, 2);
	//	selected_DACsq_intensity_proportional(DAx_max, 2);

	PULSES.select_n(voices);	// select all primary voices

	// maybe start?
	if(MENU.check_next('!'))		// 'E38!' starts E38
	  PULSES.activate_selected_synced_now(musicBoxConf.sync);	// sync and activate;
	break; // E38

      case 39:	// 'E39' time machine 2 setup
	// #define ESP32_15_clicks_no_display_TIME_MACHINE2
	{ // local scope only right now
	  musicBoxConf.bass_pulses=14;
	  musicBoxConf.middle_pulses=15;
	  musicBoxConf.high_pulses=7;

	  voices = musicBoxConf.bass_pulses + musicBoxConf.middle_pulses + musicBoxConf.high_pulses;	// init *all* primary pulses
	  PULSES.select_n(voices);

	  PULSES.time_unit=1000000;	// default metric
	  musicBoxConf.pitch.multiplier=4096;		// uses 1/4096 jiffles
	  musicBoxConf.pitch.divisor= 294;		// 293.66 = D4	// default tuning D4
	  // musicBoxConf.pitch.divisor= 147;	// 146.83 = D3
	  // musicBoxConf.pitch.divisor=55;	// default tuning a

	  select_in(SCALES, minor_scale);		// default e minor
	  // tune *all* primary pulses
	  tune_2_scale(voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, selected_in(SCALES));	// TODO: OBSOLETE?
	  lower_audio_if_too_high(409600*2);	// 2 bass octaves

	  // prepare primary pulse groups:
	  select_in(JIFFLES, d4096_512);		// default jiffle

	  // bass on DAC1 and planed broad angle LED lamps
	  // select_in(JIFFLES, d4096_512);
	  PULSES.select_from_to(0, musicBoxConf.bass_pulses - 1);
	  for(int pulse=0; pulse<musicBoxConf.bass_pulses; pulse++)
	    en_jiffle_thrower(pulse, selected_in(JIFFLES), ILLEGAL8, DACsq1);	// TODO: FIXME: use inbuilt click
	  PULSES.select_from_to(0,musicBoxConf.bass_pulses);			// pulse[bass_pulses] belongs to both groups
	  // selected_DACsq_intensity_proportional(DAx_max, 1);
	  selected_share_DACsq_intensity(DAx_max, 1);		// bass DAC1 intensity

	  // 2 middle octaves on 15 gpios
	  // select_in(JIFFLES, d4096_512);
	  //	select_in(JIFFLES, d4096_256);
	  PULSES.select_from_to(musicBoxConf.bass_pulses, musicBoxConf.bass_pulses + musicBoxConf.middle_pulses -1);
	  setup_jiffle_thrower_selected(selected_actions=0);		// overwrites topmost bass pulse
									// TODO: 'selected_actions=...' or '|='
	  // fix topmost bass pulse pulse[bass_pulses] that belongs to both groups:
	  PULSES.pulses[musicBoxConf.bass_pulses].dest_action_flags |= DACsq1;

	  // high octave on DAC2
	  //	select_in(JIFFLES, d4096_64);
	  //select_in(JIFFLES, d4096_256);
	  PULSES.select_from_to(musicBoxConf.bass_pulses + musicBoxConf.middle_pulses -1, musicBoxConf.bass_pulses + musicBoxConf.middle_pulses + musicBoxConf.high_pulses -1);
	  for(int pulse = musicBoxConf.bass_pulses + musicBoxConf.middle_pulses; pulse<voices; pulse++) {	// pulse[21] belongs to both groups
	    en_jiffle_thrower(pulse, selected_in(JIFFLES), ILLEGAL8, DACsq2);	// FIXME: use inbuilt click
	  }
	  // fix pulse[21] belonging to both groups
	  PULSES.pulses[musicBoxConf.bass_pulses + musicBoxConf.middle_pulses - 1].dest_action_flags |= DACsq2;
	  selected_share_DACsq_intensity(DAx_max, 2);
	  //	selected_DACsq_intensity_proportional(DAx_max, 2);

	  PULSES.select_n(voices);	// select all primary voices

	  // maybe start?
	  if(MENU.check_next('!'))		// 'E39!' starts E39
	    PULSES.activate_selected_synced_now(musicBoxConf.sync);	// sync and activate, no display
	  else
	    if (DO_or_maybe_display(VERBOSITY_LOWEST))	// maybe ok for here?
	      selected_or_flagged_pulses_info_lines();
	} // case E39 { }
	break;

      case 40:	// 'E40' time machine with icode player   *adapted to musicBox*
	// #define ESP32_15_clicks_no_display_TIME_MACHINE2
	tabula_rasa();
	{ // local scope 'E40' only right now	// TODO: factor out
	  PULSES.time_unit=1000000;	// default metric
	  musicBoxConf.pitch.multiplier=4096;		// uses 1/4096 jiffles
	  musicBoxConf.pitch.multiplier *= 8;	// TODO: adjust appropriate...
	  musicBoxConf.pitch.divisor= 294;		// 293.66 = D4	// default tuning D4
	  // musicBoxConf.pitch.divisor= 147;	// 146.83 = D3
	  // musicBoxConf.pitch.divisor=55;	// default tuning a
	  if(!scale_user_selected)	// see musicBox
	    select_in(SCALES, minor_scale);		// default e minor

	  select_scale__UI();	// second/third letters choose metric scales	// OBSOLETE?:

	  if(!jiffle_user_selected)				// see musicBox
	    select_in(JIFFLES, d4096_512);		// default jiffle

	  if(!icode_user_selected) {	// see musicBox
#if defined USE_i2c
	    select_in(iCODEs, (unsigned int*) d4096_1024_i2cLED);
#else
	    select_in(iCODEs, (unsigned int*) d4096_1024_icode_jiff);
#endif
	  }

	  setup_bass_middle_high(musicBoxConf.bass_pulses, musicBoxConf.middle_pulses, musicBoxConf.high_pulses);

	  // maybe start?
	  if(MENU.check_next('!'))		// 'E40!' starts E40
	    PULSES.activate_selected_synced_now(musicBoxConf.sync);	// sync and activate, no display
	  else
	    if (DO_or_maybe_display(VERBOSITY_LOWEST))	// maybe ok for here?
	      selected_or_flagged_pulses_info_lines();
	} // case 'E40' { }
	break;


      } // switch (selected_experiment)
    } // if experiment >= 0
    break;


  case '!':	// '!' sync and activate selected  (or: setup and start old style experiments)
    // see also 'n'
    // one of 'n' and '!' is obsolete...

    switch (selected_experiment) {	// setup some old style experiments:
    case -1:
    case 0:
      break;
    case 1:
      setup_jiffle128(g_inverse, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
      break;
    case 2:
      init_div_123456(g_inverse, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
      break;
    case 3:
      setup_jiffles0(g_inverse, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);    // ESP8266 Frog Orchester
      break;
    case 4:
      setup_jiffles2345(g_inverse, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
      break;
    case 5:
      init_123456(g_inverse, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
      break;
    case 6:
      init_chord_1345689a(g_inverse, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
      break;
    case 7:
      init_rhythm_1(g_inverse, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
      break;
    case 8:
      init_rhythm_2(g_inverse, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
      break;
    case 9:
      init_rhythm_3(g_inverse, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
      break;
    case 10:
      init_rhythm_4(g_inverse, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
      break;
    case 11:
      setup_jifflesNEW(g_inverse, voices, musicBoxConf.pitch.multiplier, musicBoxConf.pitch.divisor, musicBoxConf.sync);
      break;

    default:	// normal use case: sync and activate
      PULSES.activate_selected_synced_now(musicBoxConf.sync); // sync and activate

      if (DO_or_maybe_display(VERBOSITY_HIGH)) {
	MENU.ln();
	selected_or_flagged_pulses_info_lines();
      }
      break;
    }

    PULSES.fix_global_next();	// just in case?
    PULSES.check_maybe_do();	// maybe do it *first*

    break;

  default:
    return false;	// menu entry not found
    break;
  }

  return true;		// menu entry found
}  // menu_pulses_reaction()


/* **************************************************************** */

// extended_output(...)  output on MENU, maybe OLED, morse, ...  // TODO: morse output?
uint8_t /*next_row*/ extended_output(char* text, uint8_t col=0, uint8_t row=0, bool force=false) {
#if defined HAS_OLED
  MENU.out(text);
  if(monochrome_can_be_used() || force || morse_output_char) {
    // MC_clearLine(row);	// was too audible, better now, but removed anyway	TODO: test
    // MC_clearLine(row +1);	// was too audible, better now, but removed anyway	TODO: test
    MC_big_or_multiline(row, text);
  }

  row += 2;

#elif defined HAS_DISPLAY
  MC_display_message(text);	// also does   MENU.out(text);

#else // no display
  MENU.out(text);
#endif // HAS_OLED  |  HAS_DISPLAY

  return row;	// return row on *OLED display*
} // extended_output()


/* **************************************************************** */
/* **************************************************************** */

/* please see: README_pulses					    */

/*
   Copyright © Robert Epprecht  www.RobertEpprecht.ch   GPLv2
   http://github.com/reppr/pulses
*/

/* **************************************************************** */
/* **************************************************************** */
/*				END OF CODE			    */
/* **************************************************************** */
/* **************************************************************** */
