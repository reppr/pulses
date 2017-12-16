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

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
using namespace std;	// ESP8266 needs that

#include "pulses_systems_and_boards.h"

#ifdef USE_WIFI_telnet_menu
  #define AUTO_CONNECT_WIFI			// start wifi on booting?

// ***EITHER*** put your WLAN ssid and password in next 2 lines:
  const char* ssid     = "please_set_your_WiFi_SSID";
  const char* password = "topSecret77";

  // *** OR *** comment out above 2 lines,
  //  put that stuff in a file and #include it:
//#include "/home/you/WiFi-credentials.h"	// let the name be <something>.h


  //  // how many clients should be able to telnet to this ESP8266
  //  #define MAX_SRV_CLIENTS 1
  //  WiFiClient server_client[MAX_SRV_CLIENTS];
  WiFiClient server_client;

  WiFiServer telnet_server(23);
#endif //  USE_WIFI_telnet_menu

// class Pulses;
class Menu;


/* **************** Menu **************** */
#include <Menu.h>

//  Menu(int size, int menuPages, int (*maybeInput)(void), STREAMTYPE & port);
/*
  This version definines the menu INPUT routine int men_getchar();
  in the *program* not inside the Menu class.
*/
#ifdef ARDUINO	// FIXME: why doesn't it work from pulses_systems_and_boards.h???
  #define MENU_OUTSTREAM	Serial

  #ifdef USE_WIFI_telnet_menu
    #define MENU_OUTSTREAM2	server_client

// for another possible implementation see:
//	    // see:  http://wordaligned.org/articles/cpp-streambufs  tee
//	    #undef MENU_OUTSTREAM
//
//	#include <streambuf>
//
//	class teebuf: public std::streambuf
//	{
//	public:
//	    // Construct a streambuf which tees output to both input
//	    // streambufs.
//	    teebuf(std::streambuf * sb1, std::streambuf * sb2);
//	protected:
//	    virtual int overflow(int c);
//	private:
//	    std::streambuf * sb1;
//	    std::streambuf * sb2;
//	};
//
//	teebuf MENU_OUTSTREAM(Serial, WiFiClient);

  #endif

  #ifndef USE_WIFI_telnet_menu	// serial menu only
    int men_getchar() {
      if (!Serial.available())	// ARDUINO
        return EOF;

      return Serial.read();

    }
  #else				// serial *and* WLAN menu
    int men_getchar() {
      if (Serial.available())
        return Serial.read();

      if (server_client && server_client.connected() && server_client.available())
	return server_client.read();

      return EOF;
    }
  #endif
#endif

/* BAUDRATE for Serial:	uncomment one of the following lines:	*/
#define BAUDRATE	115200		// works fine here
//#define BAUDRATE	57600
//#define BAUDRATE	38400
//#define BAUDRATE	19200
//#define BAUDRATE	9600		// failsafe
//#define BAUDRATE	31250		// MIDI

#ifndef ARDUINO		// Linux PC test version  *NOT SUPPORTED*
  #include <Menu/Menu.h>
  #include <Pulses/Pulses.h>

  #include "Pulses/Pulses.cpp"		// why that?
  #include "Menu/Menu.cpp"		// why that?

  #define MENU_OUTSTREAM	cout

  int men_getchar() {
    return getchar();		// c++ Linux PC test version
  }
#endif

Menu MENU(32, 3, &men_getchar, MENU_OUTSTREAM, MENU_OUTSTREAM2);


/* **************** Pulses **************** */
#include <Pulses.h>

Pulses PULSES(pl_max, &MENU);


/* **************** Harmonical **************** */
#include <Harmonical.h>

Harmonical HARMONICAL(3628800uL);	// old style for a first test


/* **************************************************************** */
// some #define's:
#define ILLEGAL		-1
#define ILLEGAL_PIN	255


/* **************************************************************** */
// define uint8_t click_pin[CLICK_PULSES]
#ifdef CLICK_PULSES
  #include "pulses_CLICK_PIN_configuration.h"	// defines click_pin[]
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

    #include "ESP8266WiFi.h"  // after that min() and max() do not work any more. Say: std::min() std::max()
    extern "C" {
      #include "user_interface.h"
    }
  #endif

  #ifdef WIFI_OFF_mysensors
    // see: https://forum.mysensors.org/topic/5120/esp8266-with-wifi-off/3
    #include <ESP8266WiFi.h>  // after that min() and max() do not work any more. Say: std::min() std::max()
  #endif
#endif	// ESP8266


/* **************************************************************** */
// ratios, scales
int selected_ratio=ILLEGAL;

#ifndef RATIOS_RAM_SIZE	// ratios on small harware ressources, FIXME: test	################
  #define RATIOS_RAM_SIZE 9*2+2	// small buffer might fit on simple hardware
#endif

#ifdef RATIOS_RAM_SIZE
  // ratios:
  unsigned int ratios_RAM[RATIOS_RAM_SIZE] = {0};
  unsigned int ratios_RAM_length = RATIOS_RAM_SIZE;
  unsigned int ratios_write_index=0;
  unsigned int *ratios=ratios_RAM;
#else
  #error RATIOS_RAM_SIZE is not defined
#endif // RATIOS_RAM_SIZE

#ifndef RAM_IS_SCARE	// enough RAM?
char * ratio_names[] = {
      "ratios_RAM",		// 0
      "pentatonic_minor",	// 1
      "european_pentatonic",	// 2
      "mimic_japan_pentatonic",	// 3
      "ratios_quot",		// 4
      "ratios_int",		// 5
      "ratios_rationals",	// 6
  };

  #define n_ratio_names (sizeof (ratio_names) / sizeof (const char *))
#endif


// ratiotabs *MUST* have 2 trailing zeros


unsigned int ratios_quot[] = {1,1, 1,2, 1,3, 1,4, 1,5, 1,6, 1,7, 1,8, 0,0};  // zero terminated
unsigned int ratios_int[]  = {1,1, 2,1, 3,1, 4,1, 5,1, 6,1, 7,1, 8,1, 0,0};  // zero terminated
unsigned int ratios_rationals[]  = {1,1, 1,2, 2,3, 3,4, 5,6, 6,7, 7,8, 8,9, 9,10, 0,0};  // zero terminated

unsigned int european_pentatonic[] = {1,1, 8,9, 4,5, 2,3, 3,5, 0,0};	// scale each octave	zero terminated

unsigned int pentatonic_minor[] = {1,1, 5,6, 3,4, 2,3, 5*2,6*3, 0,0};	// scale each octave	zero terminated
// nice first try with "wrong" note:
//  unsigned int mimic_japan_pentatonic[] = {1,1, 8,9, 5,6, 2,3, 8*2,9*3, 1,2, 8,9*2, 5,12, 2,6, 8,9*3, 1,4, 0,0 };  // zero terminated
// second try:
unsigned int mimic_japan_pentatonic[] = {1,1, 8,9, 5,6, 2,3, 2*15,3*16, 0,0 };	// scale each octave	zero terminated


/* **************************************************************** */
int selected_jiffle = ILLEGAL;

// editing jiffle data
// if we have enough RAM, we work in an int array[]
// pre defined jiffletabs can be copied there before using and editing
#ifndef JIFFLE_RAM_SIZE
  // jiffletabs *MUST* have 2 trailing zeros
  #define JIFFLE_RAM_SIZE 9*3+2	// small buffer might fit on simple hardware
#endif
unsigned int jiffle_RAM[JIFFLE_RAM_SIZE] = {0};
unsigned int jiffle_write_index=0;
unsigned int jiffle_range_bottom=0;
unsigned int jiffle_range_top=0;
unsigned int *jiffle=jiffle_RAM;


void fix_jiffle_range() {
  unsigned int i;

  if (jiffle_range_top >= JIFFLE_RAM_SIZE)
    jiffle_range_top=JIFFLE_RAM_SIZE-1;

  if (jiffle_range_bottom > jiffle_range_top) {		// swap
    i = jiffle_range_bottom;
    jiffle_range_bottom = jiffle_range_top;
    jiffle_range_top = i;
  }
};


// pre defined jiffles:
unsigned int harmonics4[] = {1,1,1024, 1,2,1024, 1,3,1024, 1,4,1024, 0,0};	// magnets on strings experiments
unsigned int ting1024[] = {1,4096,64, 1,1024,128, 1,1024*3,128, 1,1024*2,128, 1,1024*8,64, 1,1024,64, 0,0}; // magnet strings experiments 2
// unsigned int ting1024[] = {1,4096,64, 1,1024,128, 0,0};			// magnet strings experiments 2
unsigned int ting4096[] = {1,4096,1024, -1,0,0};			// magnet strings experiments 2

// peepeep4096[] for tuning
unsigned int peepeep4096[] = {1,4096,2048, 1,8*4096,1, 1,4096,256, 1,8*4096,1, 1,4096,256, 1,8*4096,1, 1,4096,256, 1,8*4096,1, 0,0};
// unsigned int arpeggio4096[] = {4,4096*4,128, 4,4096*3,128, 4,4096*4,128, 4,4096*5,128, 4,4096*6,64, 0,0}; // pezzo strings E16

unsigned int arpeggio4096[] = {
  64,4096,1, 1,4096,256,  64,4096*2,1, 1,4096*2,256, 64,4096*3,1, 1,4096*3,256, 64,4096*5,1, 1,4096*5,256,
  64,4096*6,1, 1,4096*6,256, 64,4096*8,1, 1,4096*8,256, 0
  };
unsigned int arpeggio4096down[] = {
  64,4096*8,1, 1,4096*8,256, 64,4096*6,1, 1,4096*6,256, 64,4096*5,1, 1,4096*5,256, 64,4096*4,1, 1,4096*4,256,
  64,4096*3,1, 1,4096*3,256, 64,4096*2,1, 1,4096*2,256, 64,4096*1,1, 1,4096*1,256, 1,512,16, 1,128,8, 1,64,4, 1,32,2,
  0,0};

unsigned int arpeggio_cont[] = {64,4096,1, 1,4096,256,  64,4096*2,1, 1,4096*2,256, 64,4096*3,1, 1,4096*3,256,  64,4096*4,1, 1,4096*4,256, 64,4096*5,1, 1,4096*5,256, 64,4096*6,1, 1,4096*6,256, 64,4096*8,1, 1,4096*8,256, 1,4096*5,512, 1,12*6,6, 1,4096*6,24, 1,48,2, 1,4096*8,256, 1,6,1, 1,256,2, 1,12,3, 0,0};

unsigned int arpeggio_and_down0[] = {64,4096,1, 1,4096,256,  64,4096*2,1, 1,4096*2,256, 64,4096*3,1, 1,4096*3,256,  64,4096*4,1, 1,4096*4,256, 64,4096*5,1, 1,4096*5,256, 64,4096*6,1, 1,4096*6,256, 64,4096*8,1, 1,4096*8,256, 1,4096*5,512, 1,12,4, 1,2048,128, 1,4096*4,128, 0,0};

unsigned int arpeggio_and_down1[] = {64,4096,1, 1,4096,256,  64,4096*2,1, 1,4096*2,256, 64,4096*3,1, 1,4096*3,256,  64,4096*4,1, 1,4096*4,256, 64,4096*5,1, 1,4096*5,256, 64,4096*6,1, 1,4096*6,256, 64,4096*8,1, 1,4096*8,256, 1,4096*5,512, 1,12,2, 1,1024,64, 1,256,8, 1,2048,128, 1,4096*8,128, 1,4096*6,128, 1,4096*5,128, 1,4096*4,128, 1,4096*3,128, 1,4096*2,128, 1,4096,128, 0,0};

unsigned int arpeggio_and_down[] = {64,4096,1, 1,4096,256,  64,4096*2,1, 1,4096*2,256, 64,4096*3,1, 1,4096*3,256,  64,4096*4,1, 1,4096*4,256, 64,4096*5,1, 1,4096*5,256, 64,4096*6,1, 1,4096*6,256, 64,4096*8,1, 1,4096*8,256, 1,4096*5,512, 1,12,2, 1,1024,64, 1,256,8, 1,2048,128, 1,4096*8,128, 1,4096*6,128, 1,4096*5,128, 1,4096*4,128, 1,4096*3,128, 1,4096*2,128, 1,4096,128, 1,1024,64, 0,0};

unsigned int arpeggio_and_down3[] = {64,4096,1, 1,4096,256,  64,4096*2,1, 1,4096*2,256, 64,4096*3,1, 1,4096*3,256,  64,4096*4,1, 1,4096*4,256, 64,4096*5,1, 1,4096*5,256, 64,4096*6,1, 1,4096*6,256, 64,4096*8,1, 1,4096*8,256, 1,4096*5,512, 1,12,2, 1,1024,64, 1,256,8, 1,2048,128, 1,4096*8,128, 1,4096*6,128, 1,4096*5,128, 1,4096*4,128, 1,4096*3,128, 1,4096*2,128, 1,4096,128, 1,1024,128, 3,1024*2,128, 1,512,16, 1,256,16, 1,128,8, 0,0};

unsigned int arpeggio_and_sayling[] = {64,4096,1, 1,4096,256,  64,4096*2,1, 1,4096*2,256, 64,4096*3,1, 1,4096*3,256,  64,4096*4,1, 1,4096*4,256, 64,4096*5,1, 1,4096*5,256, 64,4096*6,1, 1,4096*6,256, 64,4096*8,1, 1,4096*8,256, 1,5*4096,512, 1,12,1 , 1,6*4096,1024, 6,5*6*4096,1024, 1,5*4096,1024, 1,4*4096,1024, 1,3*4096,1024, 1,2*4096,1024, 1,1024,128, 1,128,4, 1,64,4, 1,32,2, 0,0};

// unsigned int halfway[] = {1,2,1, 1,4,1, 1,8,1, 1,16,1, 1,32,1, 1,64,1, 1,128,1, 1,256,1, 1,512,1, 1,1024,1, 1,2048,1, 1,4096,1, 1,8192,1, 1,16384,1, 0,0};

// unsigned int back_to_ground[] = {3,128*2,8, 1,128,4, 1,64,3, 0,0};
unsigned int stepping_down[] = {1,4096*8,512+256, 1,4096*7,512,  1,4096*6,512,  1,4096*5,512,  1,4096*4,512,  1,4096*3,512,  1,4096*2,512,  1,4096,512, 0,0};

unsigned int back_to_ground[] = {1,2048,32, 3,1024*2,32, 1,1024,32, 1,512,32, 2,512*3,32, 1,256,16, 1,128,8, 1,64,4, 0,0};

unsigned int pentatonic_rising[] = {1,4096,256, 5,6*4096,256, 3,4*4096,256, 2,3*4096,256, 5*2,6*3*4096,256, 1,2*4096,256, 0,0};

unsigned int simple_theme[] = {1,128,8, 1,2*128,8, 1,3*128,8, 1,4*128,8, 5,6*4*128,3*8, 1,12,3,
			       3,4*2048,256*3/4, 5,6*2048,256*3/4, 2,3*2048,256*3/4, 1,2048,128,
			       1,128,3, 1,64,3, 1,32,4,
			       0,0};

#ifndef RAM_IS_SCARE	// enough RAM?
  #include "jiffles.h"
#endif


/* **************************************************************** */
/*
  void display_names(char** names, int count, int selected);
  display an array of strings like names of ratios, jiffles, experiments
  mark the selected one with an asterisk
*/
void display_names(char** names, int count, int selected) {
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


/* **************************************************************** */
// user interface variables:

int sync=1;			// syncing edges or middles of square pulses
unsigned long multiplier=1;
unsigned long divisor=1;

int selected_experiment=-1;
int voices=CLICK_PULSES;

#ifdef IMPLEMENT_TUNING		// implies floating point
  #include <math.h>

// first try, see sweep_click_0()
/* tuning *= detune;
  called detune_number times
  will rise tuning by an octave	*/
  int sweep_up=1;	// sweep_up==0 no sweep, 1 up, -1 down
  double tuning=1.0;
  double detune_number=4096.0;
  double detune=1.0 / pow(2.0, 1/detune_number);

  // second try, see sweep_click()
  // unsigned long ticks_per_octave=10000000L;		// 10 seconds/octave
  // unsigned long ticks_per_octave=60000000L;		//  1 minute/octave
  // unsigned long ticks_per_octave=60000000L*10L;	// 10 minutes/octave
  unsigned long ticks_per_octave=60000000L*60L;	//  1 hour/octave
  // unsigned long ticks_per_octave=60000000L*60L*24;	//  1 day/octave  FIXME: overflows

// re-implement, see tuned_sweep_click()
// PULSES.ticks_per_octave = ticks_per_octave;	// do that in setup
//

#endif


#ifdef USE_WIFI_telnet_menu
// see: WiFi Telnet Server for Serial Monitor use - provided #1169
//      https://github.com/esp8266/Arduino/issues/1169

// strings for WiFi status
const char *str_status[]= {
  "WL_IDLE_STATUS",
  "WL_NO_SSID_AVAIL",
  "WL_SCAN_COMPLETED",
  "WL_CONNECTED",
  "WL_CONNECT_FAILED",
  "WL_CONNECTION_LOST",
  "WL_DISCONNECTED"
};

// strings for WiFi mode
const char *str_mode[]= { "WIFI_OFF", "WIFI_STA", "WIFI_AP", "WIFI_AP_STA" };


bool connectWifi() {
  MENU.out("connecting WiFi to SSID: ");
  MENU.out(ssid); MENU.tab();

  // use in case of mode problem
  WiFi.disconnect();
  // switch to Station mode
  if (WiFi.getMode() != WIFI_STA) {
    WiFi.mode(WIFI_STA);
  }

  WiFi.begin (ssid, password);
  // WiFi.printDiag(Serial);	// debugging

  // 10 seconds to establish connection
  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000) {
    delay(100);
    #if defined(ESP32) || defined(ESP8266)
      yield();	// delay should do that, but who knows...
    #endif
    MENU.out(".");
  }

  // Check connection
  if (WiFi.status() == WL_CONNECTED) {
    MENU.outln(" connected");
    MENU.out("IP address: ");
    MENU.IPstring(WiFi.localIP());
    MENU.tab();
    return true;	// connected :)
  }

  // failed:
  MENU.outln("connect FAILED\tsee instructions in 'pulses.ino'");

  return false;
}  // connectWiFi()


bool setup_wifi_telnet() {
  if(! connectWifi())
    return false;

  if (WiFi.status() == WL_CONNECTED) {
    MENU.out("WiFi mode: ");
    MENU.out(str_mode[WiFi.getMode()]);
    MENU.out("\tstatus: " );
    MENU.outln (str_status[WiFi.status()]);

    telnet_server.begin();
    telnet_server.setNoDelay(true);
    MENU.out("\nWLAN MENU  telnet ");
    MENU.IPstring(WiFi.localIP());
    MENU.outln(" port 23");

    return true;
  } else
    MENU.outln("WiFi connect FAILED.");
  return false;
}
#endif // USE_WIFI_telnet_menu


/* **************************************************************** */
#ifdef ARDUINO
/* Arduino setup() and loop():					*/

// needed for MENU.add_page();
void softboard_display();		// defined later on
bool softboard_reaction(char token);	// defined later on
int softboard_page=-1;		// see: maybe_run_continuous()


void setup() {
  Serial.begin(BAUDRATE);	// Start serial communication.

#if defined(__AVR_ATmega32U4__) || defined(ESP8266) || defined(ESP32)	// FIXME: test ESP32  ################
  /* on ATmega32U4		Leonardo, Mini, LilyPad Arduino USB
     to be able to use Serial.print() from setup()
     we *must* do that before:
  */
  while (!Serial) { ;}		// wait for Serial to open
  MENU.ln();	// new line after possible garbage
#endif

  MENU.outln(F("http://github.com/reppr/pulses/\n"));

// ################ FIXME: not here ################
//#if defined(ESP8266) || defined(ESP32)		// show ESP chip id
//  delay(1500);	// ################ FIXME: ################
//  MENU.out("ESP chip ID: 0x");
//  Serial.println(ESP.getChipId(), HEX);
//#endif

#ifdef USE_WIFI_telnet_menu
  #ifdef AUTO_CONNECT_WIFI			// start wifi on booting?
    setup_wifi_telnet();
  #endif
#else // WiFi not in use, switch it off:

  #if defined(ESP8266) || defined(ESP32)	// ################ FIXME: test ################
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
  #endif	// ESP8266
#endif // to WiFi or not

  MENU.ln();
  MENU.out(F("sizeof(pulse_t) "));
  MENU.out(sizeof(pulse_t));
  MENU.out(F(" * "));
  MENU.out(pl_max);
  MENU.out(F(" pulses = \t"));
  MENU.outln(sizeof(pulse_t)*pl_max);


  // setting up the menu:

  // add pulses main page:
  MENU.add_page("pulses", 'P', \
		&menu_pulses_display, &menu_pulses_reaction, 'P');

  // add softboard page:
  softboard_page = MENU.add_page("Hardware Softboard", 'H',	\
		&softboard_display, &softboard_reaction, 'H');

  // display menu at startup:
  MENU.menu_display();

  #ifdef CLICK_PULSES
    init_click_pins_OutLow();		// make them OUTPUT, LOW
  #endif

  #ifdef IMPLEMENT_TUNING		// implies floating point
    PULSES.ticks_per_octave = ticks_per_octave;
  #endif

  // time and pulses *must* get initialized before setting up pulses:
  PULSES.init_time();		// start time
  PULSES.init_pulses();		// init pulses

  MENU.ln();

  /* ****************  DEMO SETUPS  **************** */
  // for a demo one of these could be called from here:

  // void setup_jiffle128(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync)
  // setup_jiffle128(false, CLICK_PULSES, 2, 1, 15);	// 12345678 slow beginning of voices, nice

  // init_div_123456(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync);
  // init_div_123456(false, CLICK_PULSES, 1, 1, 0);

  // setup_jiffles0(false, CLICK_PULSES, 2, 3, 1);	// setup for ESP8266 Frog Orchester

  //       void setup_jiffles2345(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync);
  // setup_jiffles2345(0, CLICK_PULSES, 1, 2, 0);

  // init_123456(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync);
  // init_123456(false, CLICK_PULSES, 1, 1, 0);

  // init_chord_1345689a(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync);
  // init_chord_1345689a(0, CLICK_PULSES, 1, 1, 0);

  // init_rhythm_1   (0, CLICK_PULSES, 1, 6*7, 1);
  // init_rhythm_2(0, CLICK_PULSES, 1, 1, 5);
  // init_rhythm_3(0, CLICK_PULSES, 1, 1, 3);
  // init_rhythm_4(0, CLICK_PULSES, 1, 7*3, 1);

  // void setup_jifflesNEW(int sync, unsigned int multiplier, unsigned int divisor);
  // setup_jifflesNEW(0, CLICK_PULSES, 3, 1, 3);

  // init_pentatonic(0, CLICK_PULSES, 1, 1, 1);	// Harmonical Frogs Choir	Frogs set 2
  // init_pentatonic(0, CLICK_PULSES, 1, 6, 1);	// 8 strings on piezzos 2016-12-12

  // 2 strings setup 2016-12-15 sweep and drone on piezzos
  //  multiplier=1;
  //  divisor=5800;
  //  en_click(0);
  //  en_sweep_click(1);
  //  PULSES.selected_pulses = 3;
  //  PULSES.reset_and_edit_selected();
  //  activate_selected_synced_now(sync);	// FIXME:	something's wrong :(	################


//	  // testing ratios, prepare_ratios():
//	  // void prepare_ratios(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync, unsigned int *ratios)
//	  ratios = pentatonic_minor;
//	  // ratios = european_pentatonic;
//	  // ratios = ratios_quot;
//	  // ratios = ratios_int;
//	  PULSES.selected_pulses=~0;
//	  // or: if nothing is selected all pulses with flags==0 get selected.
//	  int prepared = prepare_ratios(false, 8, 1, 1, 0, ratios);
//	  if (prepared != 8)
//	    MENU.out(F("prepared ")); MENU.out(prepared); MENU.slash(); MENU.outln(voices);
//	  //  PULSES.selected_pulses=0;


// working on:
// prepare_magnets(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync) {
// pentatonic steel strings on 8 magnets
//			  ratios = pentatonic_minor;


/* ****************  END DEMO SETUPS  **************** */


  PULSES.fix_global_next();	// we *must* call that here late in setup();

  // informations about alive pulses:
  MENU.ln();
  selected_or_flagged_pulses_info_lines();
}


// bool lower_priority_tasks();
// check lower priority tasks and do the first one that needs to be done
// return true if something was done
bool third_priority_tasks() {

#ifdef IMPLEMENT_TUNING		// tuning, sweeping priority below menu		*implies floating point*
  tuning = PULSES.tuning;	// FIXME: workaround for having all 3 sweep implementations in parallel
  if (maybe_stop_sweeping())		// low priority control sweep range
    return true;

  if(maybe_display_tuning_steps())	// low priority tuning steps info
    return true;
#endif

  if (maybe_run_continuous())	        // even lower priority: maybe display input state changes.
    return true;

  return false;
}


bool lowest_priority_tasks() {

#ifdef USE_WIFI_telnet_menu
// ################ FIXME: cleanup old code ################
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
//	      MENU.out("still dead 	");
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

  return false;		// ################ FIXME: return values ################
}



// stress_emergency:  looks like the value does not matter too much
//#if defined(ESP32)	// ################ FIXME: ESP32 stress ################
//unsigned int stress_emergency=4096*4096;	// high value seems appropriate
//#else
unsigned int stress_emergency=4096;	// high value seems appropriate
//#endif
unsigned int stress_count=0;

void loop() {	// ARDUINO
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
  stress_count=0;
  while (PULSES.check_maybe_do()) {	// in stress PULSES get's *first* priority.
    if (++stress_count >= stress_emergency) {
      // EMERGENCY
      // kill fastest pulse might do it? (i.e. fast sweeping up)

      if (MENU.maybe_display_more()) {
	MENU.out((int) PULSES.fastest_pulse());
	MENU.out(F(" deactivated  "));
      }

      PULSES.deactivate_pulse(PULSES.fastest_pulse());

      stress_count=0;				// seems best, maybe
      // stress_count = stress_emergency / 2;	// FIXME: further tests	################
    }
  }

  // descend through priorities and do first thing found
  if(! MENU.lurk_then_do())		// MENU second in priority, check if something to do,
    if (! third_priority_tasks())	// if not, check third_priority_tasks()
      lowest_priority_tasks();		// if still nothing done, check lowest_priority_tasks()

} // ARDUINO loop()

#else		// c++ Linux PC test version

int main() {

  printf("\nTesting pulses.ino\n");

  printf("\nNumber of pulses: %d\n", pl_max);

  printf("\nPULSES.init_pulses();\n");
  PULSES.init_pulses();
  printf("\n");


  printf("sizeof(pulse_t) %d * %d = \t%d\n\n",	\
	 sizeof(pulse_t), pl_max, sizeof(pulse_t)*pl_max );


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

/*
  This is the *old 'periodics' style* implementation
  copied over to the Pulses library.
  Click tasks have to be defined first to get the low index range.

  I plan to change the implementation soon.

*/

//   or connect LEDs, MOSFETs, MIDI, whatever...
//   these are just FlipFlop pins.

// Click_pulses are a sub-group of pulses that control an arduino
// digital output each.  By design they must be initiated first to get
// the low pulse indices. The pins are configured as outputs by init_click_pins_OutLow()
// and get used by clicks, jiffles and the like.

// It's best to always leave click_pulses in memory.
// You can set DO_NOT_DELETE to achieve this.
// By design click pulses *HAVE* to be defined *BEFORE* any other pulses:


void click(int pulse) {	// can be called from a pulse
  digitalWrite(PULSES.pulses[pulse].char_parameter_1, PULSES.pulses[pulse].counter & 1);
}


#ifdef IMPLEMENT_TUNING		// implies floating point
void sweep_click(int pulse) {	// can be called from a pulse
  double period = PULSES.pulses[pulse].period.time;
  double detune_number = PULSES.ticks_per_octave / PULSES.pulses[pulse].period.time;
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

  PULSES.pulses[pulse].period.time = period;
  // PULSES.pulses[pulse].period.overflow = 0;
  click(pulse);
}


void tuned_sweep_click(int pulse) {	// can be called from a pulse
  double detune_number = PULSES.ticks_per_octave / PULSES.pulses[pulse].period.time;
  double detune = pow(2.0, 1.0/detune_number);	// fails on Arduino Mega2560

  switch (sweep_up) {
  case 1:
    PULSES.tuning *= detune;
    break;
  case -1:
    PULSES.tuning /= detune;
    break;
  }

  // PULSES.pulses[pulse].period.overflow = 0;
  click(pulse);
}


// first try: octave is reached by a fixed number of steps:
void sweep_click_0(int pulse) {	// can be called from a sweeping pulse
  PULSES.pulses[pulse].period.time = PULSES.pulses[pulse].ulong_parameter_1 * tuning;
  PULSES.pulses[pulse].period.overflow = 0;
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
  struct time duration;

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
  duration.time = (unsigned long) PULSES.ticks_per_octave;
  duration.overflow=0;
  PULSES.display_realtime_sec(duration);

  MENU.tab();
  tuning_info();
  MENU.ln();
}


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
    struct time now = PULSES.get_now();

    int tuning_step = tuning;			// integer part
    int current_fraction = 1.0/(double) tuning;	// integer part
    bool tuning_up = (tuning > last_tuning);

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
}


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


void display_fraction(struct fraction *f) {
  MENU.out((*f).multiplier);
  MENU.slash();
  MENU.out((*f).divisor);
}


/* **************************************************************** */
void init_click_pins_OutLow() {		// make them OUTPUT, LOW
/* uint8_t click_pin[CLICK_PULSES];
   hardware pins for click_pulses:
   It is a bit obscure to hold them in an array indexed by [pulse]
   but it's simple and working well	*/
  for (int pulse=0; pulse<CLICK_PULSES; pulse++) {
    pinMode(click_pin[pulse], OUTPUT);
    digitalWrite(click_pin[pulse], LOW);
  }
}


////  // unused? (I use the synced version more often)
//  int setup_click_pulse(void (*pulse_do)(int), unsigned char new_flags,
//  		     struct time when, struct time new_period) {
//    int pulse = PULSES.setup_pulse(pulse_do, new_flags, when, new_period);
//    if (pulse != ILLEGAL) {
//      PULSES.pulses[pulse].char_parameter_1 = click_pin[pulse];
//      pinMode(PULSES.pulses[pulse].char_parameter_1, OUTPUT);
//      digitalWrite(PULSES.pulses[pulse].char_parameter_1, LOW);
//    } else {
//      out_noFreePulses();
//    }
//
//    return pulse;
//  }


void out_noFreePulses() {
  MENU.out(F("no free pulses"));
}


// make an existing pulse to a click pulse:
void en_click(int pulse)
{
  if (pulse != ILLEGAL) {
    PULSES.pulses[pulse].periodic_do = (void (*)(int)) &click;
    PULSES.pulses[pulse].char_parameter_1 = click_pin[pulse];
    pinMode(PULSES.pulses[pulse].char_parameter_1, OUTPUT);
    // digitalWrite(PULSES.pulses[pulse].char_parameter_1, LOW);	// ################
  }
}


#ifdef IMPLEMENT_TUNING		// implies floating point
// make an existing pulse to a sweep click pulse:
void en_sweep_click(int pulse)
{
  if (pulse != ILLEGAL) {
    en_click(pulse);
    PULSES.pulses[pulse].periodic_do = (void (*)(int)) &sweep_click;
  }
}


// make an existing pulse to a sweep_click_0 pulse:
void en_sweep_click_0(int pulse)
{
  if (pulse != ILLEGAL) {
    en_click(pulse);
    PULSES.pulses[pulse].ulong_parameter_1 = PULSES.pulses[pulse].period.time;
    PULSES.pulses[pulse].periodic_do = (void (*)(int)) &sweep_click_0;
  }
}


void en_tuned_sweep_click(int pulse)
{
  if (pulse != ILLEGAL) {
    en_click(pulse);
    PULSES.activate_tuning(pulse);
    PULSES.pulses[pulse].periodic_do = (void (*)(int)) &tuned_sweep_click;
  }
}
#endif	// #ifdef IMPLEMENT_TUNING	implies floating point


int setup_click_synced(struct time when, unsigned long unit, unsigned long multiplier,
		       unsigned long divisor, int sync) {
  int pulse= PULSES.setup_pulse_synced(&click, ACTIVE, when, unit, multiplier, divisor, sync);

  if (pulse != ILLEGAL) {
    PULSES.pulses[pulse].char_parameter_1 = click_pin[pulse];
    pinMode(PULSES.pulses[pulse].char_parameter_1, OUTPUT);
    digitalWrite(PULSES.pulses[pulse].char_parameter_1, LOW);
  } else {
    out_noFreePulses();
  }

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

  struct time now;
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

  struct time now;
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


unsigned int gling128_0[] = {1,128,16, 0,0};
unsigned int gling128_1[] = {1,256,2, 1,128,16, 0,0};
unsigned int gling128_2[] = {1,512,4, 1,256,4, 1,128,16, 0,0};
unsigned int gling128[]   = {1,512,8, 1,256,4, 1,128,16, 0,0};

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
    no_inverse();
    return;
  }
  unsigned long unit = multiplier * PULSES.time_unit;
  unit /= divisor;

  display_name5pars("init_pentatonic", inverse, voices, multiplier, divisor, sync);

  struct time now;
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


// old style, obsolete
int prepare_magnets(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync) {
  if (inverse) {
    no_inverse();
    return 0;
  }

  ratios = pentatonic_minor;
  select_n(voices);

#define COMPATIBILITY_PERIOD_3110	// sets the period directly
#ifdef COMPATIBILITY_PERIOD_3110
  for (int pulse=0; pulse<voices; pulse++)
    if (PULSES.selected_pulses & (1 << pulse)) {
      PULSES.reset_and_edit_pulse(pulse, PULSES.time_unit);
      PULSES.pulses[pulse].period.time = 3110;	// brute force for compatibility ;)
      PULSES.pulses[pulse].period.overflow = 0;	// brute force for compatibility ;)
      en_jiffle_thrower(pulse, jiffle);
    }
  int apply_ratios_on_periode(int voices, unsigned int *ratios, bool octaves=true);	// this code is obsolete anyway ################
  apply_ratios_on_periode(voices, ratios, true);
#else	// compatibility problems
  prepare_ratios(false, voices, multiplier, divisor, sync, ratios, true);
#endif

  // int apply_ratios_on_periode(int voices, unsigned int *ratios) {

  // prepare_ratios(false, voices, 4096*12, 41724, 0, ratios);
  // prepare_ratios(false, voices, multiplier, divisor, sync, ratios);
//	  for (int pulse=0; pulse<pl_max; pulse++)
//	    if (PULSES.selected_pulses & (1 << pulse)) {
//	      PULSES.reset_and_edit_pulse(pulse, PULSES.time_unit);
//	    }

  //  apply_ratios_on_periode(voices, ratios);

  // jiffle=jiff4096;
//  prepare_ratios(false, voices, 1, 1, 0, ratios);
//  apply_ratios_on_periode(voices, ratios);
//  prepare_ratios(false, 8, 32768, 41727, 0, ratios);
//	 ratios = pentatonic_minor;
//	  PULSES.selected_pulses=~0;
//	  int prepared = prepare_ratios(false, 8, 32768, 41727, 0, ratios);
//	  if (prepared != 8)
//	    MENU.out(F("prepared ")); MENU.out(prepared); MENU.slash(); MENU.outln(voices);
//	  select_flagged();
//	  //  PULSES.reset_and_edit_selected();
//	//	  for (int pulse=0; pulse<pl_max; pulse++)
//	//	    if (PULSES.selected_pulses & (1 << pulse))
//	//	      PULSES.divide_period(pulse, 41724);
//
//	  //  select_flagged();
  //jiffle=harmonics4;
  // unsigned int harmonics4 = {1,1,1024, 1,2,1024, 1,3,1024, 1,4,1024, 0,0};

//  selected_or_flagged_pulses_info_lines();
}


// ****************************************************************
int select_flagged() {
  PULSES.selected_pulses=0;
  for (int pulse=0; pulse<pl_max; pulse++)
    if (PULSES.pulses[pulse].flags)
      PULSES.selected_pulses |= (1 << pulse);

  return PULSES.selected_pulses;
}


int select_all() {
  PULSES.selected_pulses=0;
  for (int pulse=0; pulse<pl_max; pulse++)
    PULSES.selected_pulses |= (1 << pulse);

  return PULSES.selected_pulses;
}


int select_alive() {
  PULSES.selected_pulses=0;
  for (int pulse=0; pulse<pl_max; pulse++)
    if(PULSES.pulses[pulse].flags && (PULSES.pulses[pulse].flags != SCRATCH))
      PULSES.selected_pulses |= (1 << pulse);

  return PULSES.selected_pulses;
}


int select_n(unsigned int n) {
  PULSES.selected_pulses=0;
  if (n == 0)
    return PULSES.selected_pulses;

  for (int pulse=0; pulse<n; pulse++)
    PULSES.selected_pulses |= (1 << pulse);

  return PULSES.selected_pulses;
}


// ****************************************************************
/* ratios[]
/* a ratios array has elements of multiplier/divisor pairs
   each is the integer representation of a rational number
   very useful for all kind of things like scales, chords, rhythms */

int prepare_ratios(bool inverse, int voices, unsigned long multiplier, unsigned long divisor, int sync, unsigned int *ratios,
		   bool octaves=true) {
/* prepare a couple of pulses based on a ratio array.
   up to 'voices' pulses are created among the selected ones.
   return number of prepared pulses */
  if(ratios[0]==0)  return 0;	// error, no data

  if (inverse) {
    no_inverse();
    return 0;
  }

  int prepared=0;
  unsigned long unit = multiplier * PULSES.time_unit;
  unit /= divisor;

  if (octaves)
    display_name5pars("prepare_ratios fill octaves", inverse, voices, multiplier, divisor, sync);
  else
    display_name5pars("prepare_ratios", inverse, voices, multiplier, divisor, sync);

  struct time now;
  PULSES.get_now();
  now=PULSES.now;

  unsigned long this_period;
  struct time new_period;
  int pulse=0;
  int octave=1;	// 1,2,4,8,...
  for (int ratio=0; prepared<=voices; ratio++) {
    multiplier = ratios[ratio*2];
    if (multiplier==0) {
      if (octaves) {
	octave *= 2;	// one octave higher
	ratio = 0;	// restart at first ratio
	multiplier = ratios[ratio*2];
      } else
	goto global_next;		// multiplier==0, end
    }

    divisor = ratios[ratio*2+1];
    if (divisor==0)  goto global_next;	// divisor==0, error, end
    divisor *= octave;

    for (; pulse<pl_max; pulse++) {
      if (PULSES.selected_pulses & (1 << pulse)) {
	this_period = unit;
	this_period *= multiplier;
	this_period /= divisor;
	new_period.time = this_period;
	new_period.overflow = 0;
	PULSES.setup_pulse(NULL, SCRATCH, now, new_period);
	pulse++;
	prepared++;
	break;
      }
    }
    if (pulse==pl_max)	// all available pulses have been tried, give up
      break;
  }

 global_next:
  PULSES.fix_global_next();
  return prepared;
}


int apply_ratios_on_periode(int voices, unsigned int *ratios, bool octaves=true) {
  // FIXME: octaves are untested here ################
  if(ratios[0]==0)  return 0;	// error, no data

  struct time new_period;
  int applied=0;

//  int pulse=0;
  int octave=1;	// 1,2,4,8,...
  for (int ratio=0, pulse=0; applied<voices; ratio++) {
    multiplier = ratios[ratio*2];
    if (multiplier==0) {
      if (octaves) {
	octave *= 2;	// one octave higher
	ratio = 0;	// restart at first ratio
	multiplier = ratios[ratio*2];
      } else
	goto global_next;		// multiplier==0, end
    }

    divisor=ratios[ratio*2+1];
    if (divisor==0)
      goto global_next;		// divisor==0: error, end
    divisor *= octave;

    for (; pulse<pl_max; pulse++) {
      if (PULSES.selected_pulses & (1 << pulse)) {
	new_period = PULSES.pulses[pulse].period;
	PULSES.mul_time(&new_period, multiplier);
	PULSES.div_time(&new_period, divisor);
	PULSES.pulses[pulse].period = new_period;
	applied++;
	pulse++;
	break;
      }
    }
    if (pulse==pl_max)	// all available pulses have been tried, give up
      break;
  }

 global_next:
  PULSES.fix_global_next();
  return applied;
}

// ****************************************************************
void init_chord_1345689a(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync) {
  unsigned long unit = multiplier * PULSES.time_unit;
  unit /= divisor;

  display_name5pars("init_chord_1345689a", inverse, voices, multiplier, divisor, sync);

  struct time now;
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
void init_ratio_sequence(struct time when,
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
}


/* **************************************************************** */
// some pre-defined patterns:

void init_rhythm_1(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync) {
  unsigned long unit = multiplier * PULSES.time_unit;
  unit /= divisor;

  struct time now;

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

  struct time now;
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

  struct time now;
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

  struct time now;

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
void en_info(int pulse)
{
  if (pulse != ILLEGAL) {
    PULSES.pulses[pulse].periodic_do = (void (*)(int)) &pulse_info_1line;
  }
}

void pulse_info_1line(int pulse) {	// one line pulse info, short version
  unsigned long realtime=micros();		// let's take time *before* serial output

  MENU.out(F("PULSE "));
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
  MENU.space();

  MENU.out_flags_();
  MENU.outBIN(PULSES.pulses[pulse].flags, 8);

  MENU.tab();
  PULSES.print_period_in_time_units(pulse);

  MENU.tab();
  display_payload(pulse);

  MENU.tab();
  MENU.out(F("expected "));
  PULSES.display_realtime_sec(PULSES.pulses[pulse].next);

  MENU.tab();
  MENU.out(F("now "));

  PULSES.get_now();
  struct time scratch = PULSES.now;
  scratch.time = realtime;
  PULSES.display_realtime_sec(scratch);

  if (PULSES.selected_pulses & (1 << pulse)) {
    MENU.space();
    MENU.out('*');
  }

  MENU.ln();
}


// pulse_info() as paylod for pulses: print pulse info:
void pulse_info(int pulse) {

  MENU.out(F("*** PULSE info "));
  MENU.out(pulse);
  MENU.slash();
  MENU.out((unsigned int) PULSES.pulses[pulse].counter);

  MENU.tab();
  display_payload(pulse);

  MENU.out_flags_();
  MENU.outBIN(PULSES.pulses[pulse].flags, 8);
  MENU.ln();

  MENU.out(F("pin ")); MENU.out((int) PULSES.pulses[pulse].char_parameter_1);
  MENU.out(F("\tindex ")); MENU.out((int) PULSES.pulses[pulse].char_parameter_2);
  MENU.out(F("\ttimes ")); MENU.out(PULSES.pulses[pulse].int1);
  MENU.out(F("\tp1 "));	MENU.out(PULSES.pulses[pulse].parameter_1);
  MENU.out(F("\tp2 "));	MENU.out(PULSES.pulses[pulse].parameter_2);
  MENU.out(F("\tul1 ")); MENU.out(PULSES.pulses[pulse].ulong_parameter_1);

  MENU.ln();		// start next line

  MENU.out((float) PULSES.pulses[pulse].period.time / (float) PULSES.time_unit, 6);
  MENU.out(F(" time units"));

  MENU.out(F("\tpulse/ovf "));
  MENU.out((unsigned int) PULSES.pulses[pulse].period.time);
  MENU.slash();
  MENU.out(PULSES.pulses[pulse].period.overflow);

  MENU.tab();
  PULSES.display_realtime_sec(PULSES.pulses[pulse].period);
  MENU.space();
  MENU.out(F("pulse "));

  MENU.ln();		// start next line

  MENU.out(F("last/ovfl "));
  MENU.out((unsigned int) PULSES.pulses[pulse].last.time);
  MENU.slash();
  MENU.out(PULSES.pulses[pulse].last.overflow);

  MENU.out(F("   \tnext/ovfl "));
  MENU.out(PULSES.pulses[pulse].next.time);
  MENU.slash();
  MENU.out(PULSES.pulses[pulse].next.overflow);

  MENU.tab();
  MENU.out(F("expected "));
  PULSES.display_realtime_sec(PULSES.pulses[pulse].next);

  MENU.ln();		// start last line
  PULSES.time_info();

  MENU.ln();		// traling empty line
}


// make an existing pulse to display multiline pulse info:
void en_INFO(int pulse) {	// FIXME: to lib Pulses
  if (pulse != ILLEGAL) {
    PULSES.pulses[pulse].periodic_do = (void (*)(int)) &pulse_info;
  }
}


void display_payload(int pulse) {
  void (*scratch)(int);
  scratch=&click;
  if (PULSES.pulses[pulse].periodic_do == scratch) {
    MENU.out("click  ");
    MENU.out((int) PULSES.pulses[pulse].char_parameter_1);
    return;
  }

#ifdef IMPLEMENT_TUNING		// implies floating point
  scratch=&tuned_sweep_click;
  if (PULSES.pulses[pulse].periodic_do == scratch) {
    MENU.out(F("tuned_sweep_click "));
    MENU.out((int) PULSES.pulses[pulse].char_parameter_1);
    return;
  }

  scratch=&sweep_click;
  if (PULSES.pulses[pulse].periodic_do == scratch) {
    MENU.out(F("sweep_click "));
    MENU.out((int) PULSES.pulses[pulse].char_parameter_1);
    return;
  }

  scratch=&sweep_click_0;
  if (PULSES.pulses[pulse].periodic_do == scratch) {
    MENU.out(F("sweep_click_0 "));
    MENU.out((int) PULSES.pulses[pulse].char_parameter_1);
    return;
  }
#endif	// #ifdef IMPLEMENT_TUNING	implies floating point

  scratch=&do_jiffle;
  if (PULSES.pulses[pulse].periodic_do == scratch) {
    MENU.out(F("do_jiffle "));
    MENU.out((int) PULSES.pulses[pulse].char_parameter_1);
    return;
  }

  scratch=&do_throw_a_jiffle;
  if (PULSES.pulses[pulse].periodic_do == scratch) {
    MENU.out(F("seed jiffle ")); MENU.out((int) click_pin[pulse]);
    return;
  }

  scratch=&pulse_info;
  if (PULSES.pulses[pulse].periodic_do == scratch) {
    MENU.out(F("pulse_info"));
    return;
  }

  scratch=&pulse_info_1line;
  if (PULSES.pulses[pulse].periodic_do == scratch) {
    MENU.out(F("info line"));
    return;
  }

  scratch=NULL;
  if (PULSES.pulses[pulse].periodic_do == scratch) {
    MENU.out(F("NULL\t"));		// 8 char positions at least
    return;
  }

  MENU.out(F("UNKNOWN\t"));
}


void selected_pulses_info_lines() {
  int count=0;

  for (int pulse=0; pulse<pl_max; ++pulse) {
    if (PULSES.selected_pulses & (1 << pulse)) {
      pulse_info_1line(pulse);
      count++;
    }
  }

  if (count)
    MENU.ln();
}


void flagged_pulses_info() {
  int count=0;

  for (int pulse=0; pulse<pl_max; ++pulse)
    if (PULSES.pulses[pulse].flags) {		// any flags set?
//    pulse_info(pulse);	// FIXME: was like that before, check
      pulse_info_1line(pulse);
      count++;
    }

  if (count == 0)
    MENU.outln(F("no flagged pulses"));
}


void selected_or_flagged_pulses_info_lines() {
  int count=0;
  for (int pulse=0; pulse<pl_max; ++pulse)
    if (PULSES.pulses[pulse].flags || (PULSES.selected_pulses & (1 << pulse))) { // any flags || selected
      pulse_info_1line(pulse);
      count++;
    }

  if (count == 0) {
    MENU.outln(F("no selected or flagged pulses"));
    if(PULSES.selected_pulses)		// special feature ;)
      PULSES.print_selected_mask();
  }

  MENU.ln();
}


/* **************************************************************** */
// make an existing pulse to a jiffle thrower pulse:
void en_jiffle_thrower(int pulse, unsigned int *jiffletab)
{
  if (pulse != ILLEGAL) {
    PULSES.pulses[pulse].periodic_do = &do_throw_a_jiffle;
    PULSES.pulses[pulse].parameter_2 = (unsigned int) jiffletab;
  }
}


int init_jiffle(unsigned int *jiffletab, struct time when, struct time new_period, int origin_pulse)
{
  int pulse;
  struct time jiffle_period=new_period;

  jiffle_period.time = new_period.time * jiffletab[0] / jiffletab[1];

  pulse = PULSES.setup_pulse(&do_jiffle, ACTIVE, when, jiffle_period);
  if (pulse != ILLEGAL) {
    PULSES.pulses[pulse].char_parameter_1 = click_pin[origin_pulse];	// set pin
    // pinMode(click_pin[pulse], OUTPUT);				// should be ok already
    PULSES.pulses[pulse].char_parameter_2 = 0;				// init phase 0
    PULSES.pulses[pulse].parameter_1 = jiffletab[2];			// count of first phase
    PULSES.pulses[pulse].parameter_2 = (unsigned int) jiffletab;
    PULSES.pulses[pulse].ulong_parameter_1 = new_period.time;
  }

  return pulse;
}


void do_throw_a_jiffle(int pulse) {		// for pulse_do
  // pulses[pulse].parameter_2	= (unsigned int) jiffletab;

  // start a new jiffling pulse now (next [pulse] is not yet updated):
  unsigned int *this_jiff=(unsigned int *) PULSES.pulses[pulse].parameter_2;

  // check for empty jiffle first:
  bool has_data=true;
  for (int i=0; i<3; i++)
    if (this_jiff[i]==0)
      has_data=false;

  if (has_data)	// there *is* jiffle data
    init_jiffle((unsigned int *) PULSES.pulses[pulse].parameter_2,	\
	      PULSES.pulses[pulse].next, PULSES.pulses[pulse].period, pulse);
  else	// zero in first triplet, *invalid* jiffle table.
    MENU.outln(F("no jiffle"));
}


/* **************************************************************** */
// pulses menu:

// what is selected?

void print_selected() {
  switch (dest) {
  case CODE_PULSES:
    PULSES.print_selected_mask();
    break;

  case CODE_TIME_UNIT:
    MENU.out_selected_();
    MENU.outln(F("time unit"));
    break;
  }
}


void info_select_destination_with(bool extended_destinations) {
  MENU.out(F("SELECT DESTINATION for '= * / s K P n c j :' to work on:\t"));
  print_selected();
  MENU.out(F("select pulse with "));

  // FIXME: use 16 here, when reaction will be prepared for a,b,c,d,e,f too.
  MENU.out_ticked_hexs(min(pl_max,10));

  MENU.outln(F("\na=select *all* click pulses\tA=*all* pulses\tl=alive click voices\tL=all alive\tx=none\t~=invert selection"));

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
char * experiment_names[] = {		// FIXME: const char * experiment_names would be better
      "(invalid)",				// 0
      "setup_jiffle128",				// 1
      "init_div_123456",				// 2
      "ESP8266 Frogs",				// 3
      "setup_jiffles2345",			// 4
      "init_123456",				// 5
      "init_chord_1345689a",			// 6
      "init_rhythm_1",				// 7
      "init_rhythm_2",				// 8
      "init_rhythm_3",				// 9
      "init_rhythm_4",				// 10
      "setup_jifflesNEW",				// 11
      "init_pentatonic",				// 12
      "magnets: The Harmonical Strings Christmas Evening Sounds",	// 13
      "magnets on strings 2",			// 14
      "magnets on strings 3",			// 15
      "piezzos on low strings 2016-12-28",	// 16
      "magnets on steel strings, japanese",	// 17
      "nylon stringed wooden box, piezzos",	// 18
      "TUNING",					// 19
      "arpeggio4096\tpentatonic",			// 20
      "arpeggio4096down\tpentatonic",		// 21
      "arpeggio_cont\tpentatonic",		// 22
      "arpeggio_and_down\tpentatonic",		// 23
      "stepping_down\tpentatonic",		// 24
      "back_to_ground\tpentatonic rhythm slowdown", // 25
      "arpeggio_and_sayling\tpentatonic",		// 26
      "simple_theme\tpentatonic",			// 27
      "peepeep4096\tpentatonic\tfor tuning",	// 28
      "(invalid)",				// 29
  };

  #define n_experiment_names (sizeof (experiment_names) / sizeof (const char *))
#endif // ! RAM_IS_SCARE


bool inverse=false;	// bottom DOWN/up click-pin mapping
/*
  'inverse' works when setting up an experiment creating pulses
 	    other pulses aren't affected
	    some experiments do not implement that

  'reverse_click_pins()' as alternative:
  'reverse_click_pins()' works on the global click_pin[] array
 			 the pulses won't notice but play with new pin mapping */


/* **************************************************************** */
// special menu modes, like numeric input for jiffletabs
int menu_mode=0;
#define JIFFLE_ENTRY_UNTIL_ZERO_MODE	1	// menu_mode for unsigned integer data entry, stop at zero

/* **************************************************************** */
void menu_pulses_display() {
  MENU.outln(F("http://github.com/reppr/pulses/\n"));

  MENU.out(F("pulses "));
  MENU.out(PULSES.get_pl_max());
  MENU.tab();
  MENU.outln(F("?=help\ti=info\t.=flagged info\t:=selected info"));

  MENU.ln();
  info_select_destination_with(false);

  MENU.out(F("u=select "));  MENU.out(F("time unit"));
  MENU.out("  (");
  MENU.out(PULSES.time_unit);
  MENU.out(F(" microseconds"));
  MENU.out(F(" = "));
  MENU.out((float) (1000000.0 / (float) PULSES.time_unit), 6);
  MENU.outln(F(" per second)"));

  MENU.ln();
  MENU.out(F("s=switch pulse on/off"));
  MENU.tab();
  MENU.out(F("M=deactivate ALL\tX=remove ALL\tK=kill\n\nCREATE PULSES\tstart with 'P'\nP=new pulse\tc=en-click\tj=en-jiffle\tN=en-noop\tf=en-info\tF=en-INFO\nS=sync\tn=sync now "));
  MENU.outln(sync);

  MENU.out(F("E=enter experiment (")); MENU.out(selected_experiment); MENU.out(F(")"));
  MENU.out(F("\t\tV=voices for experiment (")); MENU.out(voices); MENU.outln(F(")"));
   MENU.out(F("b=toggle pin mapping (bottom "));
  if (inverse)
    MENU.out(F("up"));
  else
    MENU.out(F("down"));
  MENU.outln(F(")\tZ=reverse_click_pins"));

  MENU.out(F("Scale (")); MENU.out(multiplier);MENU.slash();  MENU.out(divisor);
  MENU.out(F(")\tm=multiplier d=divisor"));
}


int setup_jiffle_thrower_synced(struct time when,
				unsigned long unit,
				unsigned long multiplier, unsigned long divisor,
				int sync, unsigned int *jiffletab)
{
  int pulse= PULSES.setup_pulse_synced(&do_throw_a_jiffle, ACTIVE,
			       when, unit, multiplier, divisor, sync);
  if (pulse != ILLEGAL) {
    PULSES.pulses[pulse].parameter_2 = (unsigned int) jiffletab;
  } else {
    out_noFreePulses();
  }

  return pulse;
}


/* **************************************************************** */
// jiffles:
// jiffles are (click) patterns defined in jiffletabs and based on a base period
//
// the base period is multiplied/divided by two int values
// the following jiffletab value counts how many times the pulse will get
// woken up with this new computed period
// then continue with next jiffletab entries
// a zero multiplier ends the jiffle

// jiffletabs define melody:
// up to 256 triplets of {multiplier, divisor, count}
// multiplier and divisor determine period based on the starting pulses period
// a multiplier of zero indicates end of jiffle

#define JIFFLETAB_INDEX_STEP	3

// jiffletabs *MUST* have 2 trailing zeros
unsigned int jiffletab[] =
  {1,16,2, 1,256,32, 1,128,8, 1,64,2, 1,32,1, 1,16,1, 1,8,2, 0,0};

unsigned int jiffletab_december[] =
  {1,1024,4, 1,64,4, 1,28,16, 1,512,8, 1,1024,128, 0,0};

unsigned int jiffletab_december128[] =
  {1,1024,4, 1,64,4, 1,128,16, 1,512,8, 1,1024,128, 0,0};

unsigned int jiffletab_december_pizzicato[] =
  {1,1024,4, 1,64,4, 1,28,16, 1,512,8, 1,1024,128, 1,2048,8, 0,0};


void set_jiffle_RAM_value(int new_value) {
  char input;
  jiffle[jiffle_write_index++]=new_value;

  // jiffletabs *MUST* have 2 trailing zeros	// ################ FIXME: ################
  if (jiffle_write_index >= (JIFFLE_RAM_SIZE - 2)) {	// array full?
    jiffle[jiffle_write_index--]=0;

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
  if (jiffle_write_index >= (JIFFLE_RAM_SIZE - 2))
    jiffle_write_index = JIFFLE_RAM_SIZE - 2;
//  jiffle[jiffle_write_index] = 0;	// store a trailing zero

  jiffle[JIFFLE_RAM_SIZE - 1 ] = 0;	// zero out last 2 array elements (savety net)
  jiffle[JIFFLE_RAM_SIZE - 2 ] = 0;
  menu_mode=0;				// stop numeric data input
  //  jiffle_write_index=0;		// no, we leave write index as is

  display_jiffletab(jiffle);		// put that here for now
}


void load2_jiffle_RAM(unsigned int *source) {	// double zero terminated
  unsigned int data;
  unsigned int cnt=0;	// data counter

  while (jiffle_write_index < (JIFFLE_RAM_SIZE - 2)) {
    data=source[cnt++];
    if (data==0) {
      --cnt;
      break;
    }

    set_jiffle_RAM_value(data);
  }

  //  jiffle_write_index += cnt;
}


struct fraction jiffletab_len(unsigned int *jiffletab) {
  static struct fraction f;	// keep return value
  struct fraction scratch;
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
    HARMONICAL.add_fraction(&scratch, &f);
  }

  return f;
}


void display_jiffletab(unsigned int *jiffletab) {
  struct fraction sum;
  sum.multiplier = 0;
  sum.divisor = 1;
  bool was_zero=false;

  // first line:
#ifndef RAM_IS_SCARE
  MENU.out(jiffle_names[selected_jiffle]); MENU.tab(); MENU.out(F("ID: ")); MENU.out(selected_jiffle); MENU.tab();
#endif
  MENU.out(F("editing "));
  if (menu_mode==JIFFLE_ENTRY_UNTIL_ZERO_MODE) {
    MENU.out(F("on\tclose with '}'"));
    if (MENU.verbosity >= VERBOSITY_SOME)
      MENU.out(F("\tmove cursor < >\trange borders | \""));
  } else
    MENU.out(F("off\tstart edit with '{'"));

  MENU.ln();

  // second line {data,data, ...}:
  MENU.out("{");
  for (int i=0; ; i++) {
    if ((i % JIFFLETAB_INDEX_STEP) == 0)
      MENU.space();
    if (i==jiffle_range_bottom)
      if (jiffle_range_top)	// no range, no sign
	MENU.out('|');
    if (i==jiffle_write_index)
      MENU.out("<");
    MENU.out(jiffletab[i]);
    if (i==jiffle_write_index)
      MENU.out(">");
    if (i==jiffle_range_top)
      if (jiffle_range_top)	// no range, no sign
	MENU.out('"');
    MENU.out(",");
    if (jiffletab[i] == 0) {
      if (was_zero)	// second zero done, stop it
	break;

      was_zero=true;	// first zero, continue *including* both zeroes
    } else
      was_zero=false;
  }

  MENU.out(F(" }  cursor "));
  MENU.out(jiffle_write_index); MENU.slash(); MENU.out(JIFFLE_RAM_SIZE);	// ################ FIXME: ################

  sum = jiffletab_len(jiffletab);
  MENU.tab();
  MENU.out(F("length ")); display_fraction(&sum);
  MENU.ln();
}



unsigned int jiffletab01[] = {1,512,8, 1,1024,16, 1,2048,32, 1,1024,16, 0,0};
unsigned int jiffletab02[] = {1,128,2, 1,256,6, 1,512,10, 1,1024,32, 1,3*128,20, 1,64,8, 0,0};
unsigned int jiffletab03[] = {1,32,4, 1,64,8, 1,128,16, 1,256,32, 1,512,64, 1,1024,128, 0,0};	// testing octaves
unsigned int jiffletab04[] =
  {1,2096,4, 1,512,2, 1,128,2, 1,256,2, 1,512,8, 1,1024,32, 1,512,4, 1,256,3, 1,128,2, 1,64,1, 0,0};
unsigned int jiffletab05[] = {2,1024*3,4, 1,1024,64, 1,2048,64, 1,512,2, 1,64,1, 1,32,1, 1,16,2, 0,0};
unsigned int jiffletab06[] = {1,32,2, 0,0};	// doubleclick
unsigned int jiffletab1[]  = {1,1024,64, 1,512,4, 1,128,2, 1,64,1, 1,32,1, 1,16,1, 0,0};


void do_jiffle (int pulse) {	// to be called by pulse_do
  // PULSES.pulses[pulse].char_parameter_1	click pin
  // PULSES.pulses[pulse].char_parameter_2	jiffletab index
  // PULSES.pulses[pulse].parameter_1		count down
  // PULSES.pulses[pulse].parameter_2		jiffletab[] pointer
  // PULSES.pulses[pulse].ulong_parameter_1	base period = period of starting pulse

  digitalWrite(PULSES.pulses[pulse].char_parameter_1, PULSES.pulses[pulse].counter & 1);	// click

  if (--PULSES.pulses[pulse].parameter_1 > 0)				// countdown, phase endid?
    return;						//   no: return immediately

  // if we arrive here, phase endid, start next phase if any:
  unsigned int* jiffletab = (unsigned int *) PULSES.pulses[pulse].parameter_2;	// read jiffletab[]
  PULSES.pulses[pulse].char_parameter_2 += JIFFLETAB_INDEX_STEP;
  int base_index = PULSES.pulses[pulse].char_parameter_2;		// readability
  long multiplier=jiffletab[base_index];
  if (multiplier == 0) {	// multiplier==0 no next phase, return
    PULSES.init_pulse(pulse);	// remove pulse
    return;			// and return
  }

  //initialize next phase, re-using the same pulse:

  // to be able to play unfinished jiffletabs while editing them
  // we check the other 2 items of the triple for zeroes
  long divisor=jiffletab[base_index+1];
  long counter=jiffletab[base_index+2];
  if (divisor==0 || counter==0 ) {	// no next phase, return
    PULSES.init_pulse(pulse);		// remove pulse
    return;				// and return
  }
  PULSES.pulses[pulse].period.time =
    PULSES.pulses[pulse].ulong_parameter_1 * jiffletab[base_index] / jiffletab[base_index+1];
  PULSES.pulses[pulse].parameter_1 = jiffletab[base_index+2];		// count of next phase
}


// huch, unused?	FIXME: unused?
void setup_jiffle_thrower(unsigned int *jiffletab, unsigned char new_flags, struct time when, struct time new_period) {
  int pulse = PULSES.setup_pulse(&do_throw_a_jiffle, new_flags, when, new_period);

  if (pulse != ILLEGAL)
    PULSES.pulses[pulse].parameter_2 = (unsigned int) jiffletab;
  else
    out_noFreePulses();
}


// pre-defined jiffle pattern:
void setup_jiffles2345(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync) {
  if (inverse) {
    no_inverse();
    return;
  }
  unsigned long unit = multiplier * PULSES.time_unit;
  unit /= divisor;

  display_name5pars("jiffles2345", inverse, voices, multiplier, divisor, sync);

  struct time when;
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

// jiffletab0 is obsolete	DADA ################
unsigned int jiffletab0[] =
  {2,1024*3,4, 1,1024,64, 1,2048,64, 1,512,4, 1,64,3, 1,32,1, 1,16,2, 0,0};	// nice short jiffy

unsigned int jiff0[] =
  {1,16,4, 1,24,6, 1,128,16, 1,1024,64, 1,2048,128, 1,4096,256, 1,2048,64, 1,4096,128, 1,32,2, 0,0}; // there *must* be a trailing zero.);

unsigned int jiff1[] =
  {1,512,8, 1,1024,16, 1,2048,32, 1,1024,16, 0,0};

unsigned int jiff2[] =
  {1,2096,4, 1,512,2, 1,128,2, 1,256,2, 1,512,8, 1,1024,32, 1,512,4, 1,256,3, 1,128,2, 1,64,1, 0,0};


void setup_jifflesNEW(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync) {
  if (inverse) {
    no_inverse();
    return;
  }
  unsigned long unit = multiplier * PULSES.time_unit;
  unit /= divisor;

  display_name5pars("setup_jifflesNEW", inverse, voices, multiplier, divisor, sync);

  struct time when;
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
  struct time when=PULSES.now;

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
void setup_jiffles0(bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync) {
  if (inverse) {
    no_inverse();
    return;
  }
  unsigned long unit = multiplier * PULSES.time_unit;
  unit /= divisor;

  display_name5pars("setup_jiffles0", inverse, voices, multiplier, divisor, sync);

  struct time when;
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
  'inverse' works when setting up an experiment creating pulses
 	    other pulses aren't affected
	    some experiments do not implement that

  'reverse_click_pins()' as alternative:
  'reverse_click_pins()' works on the global click_pin[] array
 			 the pulses won't notice but play with new pin mapping */

bool click_pins_inverted=false;
void reverse_click_pins() {
  uint8_t scratch;
  for (int i=0, j=CLICK_PULSES-1; i<j; i++, j--) {
      scratch=click_pin[i];
      click_pin[i]=click_pin[j];
      click_pin[j]=scratch;
  }

  click_pins_inverted=!click_pins_inverted;
}

// ****************************************************************
// menu_serial_program_reaction()


// display helper functions:
void short_info() {
  MENU.ln();
  PULSES.time_info();

  MENU.ln();
  selected_or_flagged_pulses_info_lines();
}


// helper functions to display parameters of menu functions:
void display_next_par(long parameter) {
  MENU.out(F(", "));
  MENU.out(parameter);
}

void display_last_par(long parameter) {
  MENU.out(F(", "));
  MENU.out(parameter);
  MENU.outln(F(")"));
}

void display_name5pars(char* name, bool inverse, int voices, unsigned int multiplier, unsigned int divisor, int sync) {
  if (MENU.verbosity) {
    MENU.out((char *) name);
    MENU.out("(");
    MENU.out(inverse);
    display_next_par(voices);
    display_next_par(multiplier);
    display_next_par(divisor);
    display_last_par(sync);
  }
}

// display helper function no_inverse()
void no_inverse() {
    MENU.outln(F("no 'inverse'\ttry 'Z' instead"));
    extern bool inverse;
    inverse=false;
}

// display factor function show_scale();
void show_scale() {
  MENU.out(F("multiplier/divisor "));
  MENU.out(multiplier); MENU.slash(); MENU.outln(divisor);
}

// for old style 'experiment'
void Press_toStart() {
  MENU.outln(F("Press '!' to start"));
}


bool menu_pulses_reaction(char menu_input) {
  static unsigned long input_value=0;
  static unsigned long calc_result=0;
  struct time now, time_scratch;
  unsigned long bitmask;
  char next_token;	// for multichar commands

  switch (menu_input) {
  case '?':	// help, overrides common menu entry for '?'
    MENU.menu_display();	// as common
    short_info();		// + short info
    break;

  case '.':	// time and flagged pulses info
    short_info();
    break;

  case ':':
    MENU.ln();
    PULSES.time_info();
    MENU.ln();
    PULSES.maybe_show_selected_mask();
    selected_pulses_info_lines();
    break;

  case ',':	// accept as noop in normal mode. used as delimiter to input data, displaying info. see 'menu_mode'
    if (menu_mode==JIFFLE_ENTRY_UNTIL_ZERO_MODE)
	display_jiffletab(jiffle);
    else
      if (MENU.verbosity >= VERBOSITY_SOME) {
	MENU.out_noop();
	MENU.ln();
      }
    break;

  case '|':	// accept as noop in normal mode. used as range bottom delimiter in arrays
    if (menu_mode==JIFFLE_ENTRY_UNTIL_ZERO_MODE) {
      jiffle_range_bottom = jiffle_write_index;
      fix_jiffle_range();

      if(MENU.cb_peek()==EOF)
	if (MENU.verbosity)
	  display_jiffletab(jiffle);
    } else
      if (MENU.verbosity >= VERBOSITY_SOME) {
	MENU.out_noop();
	MENU.ln();
      }
    break;

  case '"':	// accept as reserved noop in normal mode. used as range top delimiter in arrays
    if (menu_mode==JIFFLE_ENTRY_UNTIL_ZERO_MODE) {
      jiffle_range_top = jiffle_write_index;
      fix_jiffle_range();

      if(MENU.cb_peek()==EOF)
	if (MENU.verbosity)
	  display_jiffletab(jiffle);
    } else
      if (MENU.verbosity >= VERBOSITY_SOME)
	MENU.outln(F("reserved"));	// reserved for string input
    break;

  // in normal mode toggle pulse selection with chiffres
  // else input data. see 'menu_mode'
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
    switch (menu_mode) {
    case 0:	// normal input, no special menu_mode
      if((menu_input -'0') >= pl_max)
	return false;		// *only* responsible if pulse exists

      // existing pulse:
      PULSES.selected_pulses ^= (1 << (menu_input - '0'));

      PULSES.maybe_show_selected_mask();
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

  case '<':
    switch (menu_mode) {
    case JIFFLE_ENTRY_UNTIL_ZERO_MODE:
      if (jiffle_write_index)
	jiffle_write_index--;

      if(MENU.cb_peek()==EOF)
	if (MENU.verbosity)
	  display_jiffletab(jiffle);
      break;
    default:
      if (MENU.verbosity >= VERBOSITY_SOME) {
	MENU.out_noop();
	MENU.ln();
      }
    }
    break;

  case '>':
    switch (menu_mode) {
    case JIFFLE_ENTRY_UNTIL_ZERO_MODE:
      if (++jiffle_write_index >= (JIFFLE_RAM_SIZE - 2))
	jiffle_write_index = JIFFLE_RAM_SIZE - 2;

      if(MENU.cb_peek()==EOF)
	if (MENU.verbosity)
	  display_jiffletab(jiffle);
      break;
    default:
      if (MENU.verbosity >= VERBOSITY_SOME) {
	MENU.out_noop();
	MENU.ln();
      }
    }
    break;

  case 'u':	// calculating or select destination: PULSES.time_unit
    {
      unsigned long input_value=PULSES.time_unit;
      if (MENU.maybe_calculate_input(&input_value)) {
	MENU.out("==> "), MENU.outln(input_value);
	PULSES.time_unit=input_value;
      }
      else
	dest = CODE_TIME_UNIT;		// FIXME: obsolete?
    }
    break;

  case 'a':	// select destination: all pulses with flags
    select_flagged();
    PULSES.maybe_show_selected_mask();

    if (MENU.maybe_display_more())
      selected_or_flagged_pulses_info_lines();

    break;

  case 'A':	// select destination: *all* pulses
    select_all();
    PULSES.maybe_show_selected_mask();
    break;

  case 'l':	// select destination: alive voices
    PULSES.selected_pulses=0;
    for (int pulse=0; pulse<voices; pulse++)
      if(PULSES.pulses[pulse].flags && (PULSES.pulses[pulse].flags != SCRATCH))
	PULSES.selected_pulses |= (1 << pulse);

    PULSES.maybe_show_selected_mask();
    break;

  case 'L':	// select destination: all alive pulses
    select_alive();
    PULSES.maybe_show_selected_mask();
    break;

  case '~':	// invert destination selection
    PULSES.selected_pulses = ~PULSES.selected_pulses;
    bitmask=0;
    for (int pulse=0; pulse<pl_max; pulse++)
      bitmask |= (1 << pulse);
    PULSES.selected_pulses &= bitmask;
    PULSES.maybe_show_selected_mask();
    break;

  case 'x':	// clear destination selection
    PULSES.selected_pulses = 0;
    PULSES.maybe_show_selected_mask();
    break;

  case 's':	// switch pulse ACTIVE on/off
    for (int pulse=0; pulse<pl_max; pulse++) {
      if (PULSES.selected_pulses & (1 << pulse)) {
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
    PULSES.check_maybe_do();			// maybe do it *first*

    if (MENU.maybe_display_more()) {
      MENU.ln();
      selected_or_flagged_pulses_info_lines();			// *then* info ;)
    }
    // info_select_destination_with(false);	// DADA ################
    break;

  case 'S':	// enter sync
    if (MENU.maybe_calculate_input(&input_value)) {
      if (input_value>=0 )
	sync = input_value;
      else
	MENU.out(F("positive integer only"));
    }

    if (MENU.maybe_display_more()) {
      MENU.out(F("sync "));
      MENU.outln(sync);
    }

    break;

  case 'i':	// info
    MENU.ln();
    PULSES.time_info();
    MENU.ln();	// ################################################################
    flagged_pulses_info();
    break;

  case 'M':	// "mute", no deactivate all clicks, see 'N'
    PULSES.deactivate_all_clicks();
    PULSES.check_maybe_do();	// maybe do it *first*

    if (MENU.verbosity)
      MENU.outln(F("deactivated all pulses"));
    break;

  case '*':	// multiply destination
    switch (dest) {
    case CODE_PULSES:
      input_value = MENU.numeric_input(1);
      if (input_value>=0) {
	for (int pulse=0; pulse<pl_max; pulse++)
	  if (PULSES.selected_pulses & (1 << pulse))
	    PULSES.multiply_period(pulse, input_value);

	PULSES.fix_global_next();
	PULSES.check_maybe_do();	// maybe do it *first*

	if (MENU.maybe_display_more()) {
	  MENU.ln();
	  selected_or_flagged_pulses_info_lines();
	}
      } else
	MENU.outln_invalid();
      break;

    case CODE_TIME_UNIT:
      input_value = MENU.numeric_input(1);
      if (input_value>0)
	PULSES.set_time_unit_and_inform(PULSES.time_unit * input_value);
      else
	MENU.outln_invalid();
      break;
    }
    break;

  case '/':	// divide destination
    switch (dest) {
    case CODE_PULSES:
      input_value = MENU.numeric_input(1);
      if (input_value>=0) {
	for (int pulse=0; pulse<pl_max; pulse++)
	  if (PULSES.selected_pulses & (1 << pulse))
	    PULSES.divide_period(pulse, input_value);

	PULSES.fix_global_next();
	PULSES.check_maybe_do();	// maybe do it *first*

	if (MENU.maybe_display_more()) {
	  MENU.ln();
	  selected_or_flagged_pulses_info_lines();
	}
      } else
	MENU.outln_invalid();
      break;

    case CODE_TIME_UNIT:
      input_value = MENU.numeric_input(1);
      if (input_value>0)
	PULSES.set_time_unit_and_inform(PULSES.time_unit / input_value);
      else
	MENU.outln_invalid();
      break;
    }
    break;

  case '=':	// set destination to value
    switch (dest) {
    case CODE_PULSES:
      input_value = MENU.numeric_input(1);
      if (input_value>=0) {
	for (int pulse=0; pulse<pl_max; pulse++)
	  if (PULSES.selected_pulses & (1 << pulse)) {
	    time_scratch.time = PULSES.time_unit;
	    time_scratch.overflow = 0;
	    PULSES.mul_time(&time_scratch, input_value);
	    PULSES.set_new_period(pulse, time_scratch);
	  }

	PULSES.fix_global_next();
	PULSES.check_maybe_do();	// maybe do it *first*

	if (MENU.maybe_display_more()) {
	  MENU.ln();
	  selected_or_flagged_pulses_info_lines();
	}
      } else
	MENU.outln_invalid();
      break;

    case CODE_TIME_UNIT:
      input_value = MENU.numeric_input(1);
      if (input_value>0)
	PULSES.set_time_unit_and_inform(input_value);
      else
	MENU.outln_invalid();
      break;
    }
    break;

  case 'K':	// kill selected pulses
    if (PULSES.selected_pulses) {
      MENU.out(F("kill pulse "));
      for (int pulse=0; pulse<pl_max; pulse++)
	if (PULSES.selected_pulses & (1 << pulse)) {
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

    if (MENU.maybe_display_more()) {
      selected_or_flagged_pulses_info_lines();
      MENU.ln();
    }
    break;

  case 'P':	// pulse create and edit
    PULSES.reset_and_edit_selected();
    if (MENU.maybe_display_more()) {
      MENU.ln();
      selected_or_flagged_pulses_info_lines();
    }
    break;

  case 'n':	// synchronize to now
    // we work on pulses anyway, regardless dest
    PULSES.activate_selected_synced_now(sync, PULSES.selected_pulses);	// sync and activate

    if (MENU.maybe_display_more()) {	// *then* info ;)
      MENU.ln();
      selected_or_flagged_pulses_info_lines();
    }
    break;

  case 'N':	// NULLs payload
    // we work on pulses anyway, regardless dest
    for (int pulse=0; pulse<voices; pulse++)
      if (PULSES.selected_pulses & (1 << pulse)) {
	PULSES.pulses[pulse].periodic_do = NULL;
	if (pulse<CLICK_PULSES)		// set clicks on LOW
	  digitalWrite(click_pin[pulse], LOW);
      }

    PULSES.fix_global_next();	// just in case?
    PULSES.check_maybe_do();	// maybe do it *first*

    if (MENU.maybe_display_more()) {	// *then* info ;)
      MENU.ln();
      selected_or_flagged_pulses_info_lines();
    }
    break;

  case 'c':	// en_click
    // we work on voices anyway, regardless dest
    for (int pulse=0; pulse<voices; pulse++)
      if (PULSES.selected_pulses & (1 << pulse))
	en_click(pulse);

    PULSES.check_maybe_do();	// maybe do it *first*

    if (MENU.maybe_display_more()) {
      MENU.ln();
      selected_pulses_info_lines();
    }

    break;

#ifdef IMPLEMENT_TUNING		// implies floating point
  case 'w':
    sweep_up *= -1;	// toggle direction up down

    if (sweep_up==0)	// start sweeping if it was disabled
      sweep_up=1;

    PULSES.check_maybe_do();	// maybe do it *first*

    if (MENU.maybe_display_more())
      sweep_info();

    break;

  case 'W':	// sweep info and control
    next_token = MENU.cb_peek();
    if (next_token == (char) EOF) {	// *no* input after 'W': maybe start, info
      if (sweep_up==0)
	sweep_up=1;		//    start sweeping up if disabled

      if (MENU.verbosity >= VERBOSITY_SOME)
	sweep_info();
      break;		// done
    }
    // there *is* input after 'W'

    // 'W<number>' does (calculating) positive integer input on PULSES.ticks_per_octave
    // exception: 'W0' switches sweeping off
    if (MENU.cb_peek()!='0' && MENU.maybe_calculate_input((unsigned long*) &PULSES.ticks_per_octave)) {	// hmmm !!!
      ticks_per_octave = PULSES.ticks_per_octave;	// FIXME: obsolete
      MENU.out(PULSES.ticks_per_octave);
      MENU.outln(F(" ticks/octave"));
    } else {	// no numeric input (except '0') follows 'W'
      next_token = MENU.cb_peek();
      switch(next_token) {	// examine following input token
      case '~': case 't':
	MENU.drop_input_token();
	if(sweep_up==0)			// start if not active
	  sweep_up = 1;
	else
	  sweep_up *= -1;		// or toggle sweep direction up down
	break;
      case '0':				// 'W0' switches sweeping off
	MENU.drop_input_token();
	sweep_up = 0;		// sweep off
	break;
      case '+': case '1':
	MENU.drop_input_token();
	sweep_up = 1;		// sweep up
	break;
      case '-':
	MENU.drop_input_token();
	sweep_up = -1;		// sweep down
	break;
      case '?':			// info only
	MENU.drop_input_token();
	// if verbosity is too low sweep_info will not be called below,
	// so we do it here
	if (MENU.verbosity < VERBOSITY_SOME)
	  sweep_info();
	break;
      }
    }

    if (MENU.verbosity >= VERBOSITY_SOME)
      sweep_info();
    break;

  case 't':	// en_sweep_click
    // we work on voices anyway, regardless dest
    for (int pulse=0; pulse<voices; pulse++)
      if (PULSES.selected_pulses & (1 << pulse))
	en_sweep_click(pulse);

    PULSES.fix_global_next();	// just in case?
    PULSES.check_maybe_do();	// maybe do it *first*

    if (MENU.maybe_display_more()) {
      MENU.ln();
      selected_or_flagged_pulses_info_lines();
    }

    break;

  case 'o':	// en_sweep_click_0
    // we work on voices anyway, regardless dest
    for (int pulse=0; pulse<voices; pulse++)
      if (PULSES.selected_pulses & (1 << pulse))
	en_sweep_click_0(pulse);

    PULSES.fix_global_next();	// just in case?
    PULSES.check_maybe_do();	// maybe do it *first*

    if (MENU.maybe_display_more()) {
      MENU.ln();
      selected_or_flagged_pulses_info_lines();
    }

    break;

  case 'p':	// en_tuned_sweep_click
    // we work on voices anyway, regardless dest
    for (int pulse=0; pulse<voices; pulse++)
      if (PULSES.selected_pulses & (1 << pulse))
	en_tuned_sweep_click(pulse);

    PULSES.fix_global_next();	// just in case?
    PULSES.check_maybe_do();	// maybe do it *first*

    if (MENU.maybe_display_more()) {
      MENU.ln();
      selected_or_flagged_pulses_info_lines();
    }

    break;

  case 'T':	// 'T<integer-number>' sets tuning, 'T' toggles TUNED
    input_value = (unsigned long) PULSES.tuning;
    if (MENU.maybe_calculate_input(&input_value))	{	// T1 sets tuning to 1.0
      if (input_value > 0)
	PULSES.tuning = (double) input_value;
      tuning_info();
      MENU.ln();
    } else {	// toggle TUNED on selected pulses
      // we work on voices anyway, regardless dest
      for (int pulse=0; pulse<voices; pulse++)
	if (PULSES.selected_pulses & (1 << pulse))
	  if (PULSES.pulses[pulse].flags & TUNED)
	    PULSES.stop_tuning(pulse);
	  else
	    PULSES.activate_tuning(pulse);
    }

    PULSES.fix_global_next();	// just in case?
    PULSES.check_maybe_do();	// maybe do it *first*

    if (MENU.maybe_display_more()) {
      MENU.ln();
      selected_or_flagged_pulses_info_lines();
    }
    break;
#endif	// #ifdef IMPLEMENT_TUNING	implies floating point

  case 'j':	// en_jiffle_thrower
    // we work on voices anyway, regardless dest
    for (int pulse=0; pulse<voices; pulse++)
      if (PULSES.selected_pulses & (1 << pulse))
	en_jiffle_thrower(pulse, jiffle);

    PULSES.fix_global_next();	// just in case?
    PULSES.check_maybe_do();	// maybe do it *first*

    if (MENU.maybe_display_more()) {
      MENU.ln();
      selected_or_flagged_pulses_info_lines();
    }
    break;

  case 'J':	// select, edit, load jiffle
    /*
      'J'  shows jiffle_names and display_jiffletab(jiffle) selected jiffle
      'J7' selects jiffle #7 and display_jiffletab(7)
      'J!' loads selected jiffle in jiffle_RAM and display_jiffletab(jiffle_RAM)
    */
    // some jiffles from source, some very old FIXME:	review and delete	################
    if (MENU.cb_peek() == '!') {  // 'J!' copies an already selected jiffletab to RAM
      MENU.drop_input_token();
      if(jiffle != jiffle_RAM) {
	unsigned int * source=jiffle;
	// jiffle_write_index=0;	// no, write starting at jiffle_write_index #### FIXME: ####

	jiffle = jiffle_RAM;
	selected_jiffle = 0;
	load2_jiffle_RAM(source);
      }
    } else {	// select jiffle
      if (MENU.maybe_display_more()) {
	MENU.out(F("jiffle "));

#ifndef RAM_IS_SCARE	// enough RAM?	display jiffle names
	display_names(jiffle_names, n_jiffle_names, selected_jiffle);
#endif
      }

      // some jiffles in the source are very old   FIXME: check
      selected_jiffle=MENU.numeric_input(selected_jiffle);	// remember as index for jiffle_names[selected_jiffle]
      switch (selected_jiffle) {
      case 0:
	jiffle = jiffle_RAM;
	break;
      case 1:
	jiffle = gling128;
	break;
      case 2:
	jiffle = jiffletab;
	break;
      case 3:
	jiffle = jiffletab_december;
	break;
      case 4:
	jiffle = jiffletab_december128;
	break;
      case 5:
	jiffle = jiffletab_december_pizzicato;
	break;
      case 6:
	jiffle = jiffletab01;
	break;
      case 7:
	jiffle = jiffletab01;
	break;
      case 8:
	jiffle = jiffletab02;
	break;
      case 9:
	jiffle = jiffletab03;
	break;
      case 10:
	jiffle = jiffletab04;
	break;
      case 11:
	jiffle = jiffletab05;
	break;
      case 12:
	jiffle = jiffletab06;
	break;
      case 13:
	jiffle = jiffletab06;
	break;
      case 14:
	jiffle = gling128_0;
	break;
      case 15:
	jiffle = gling128_1;
	break;
      case 16:
	jiffle = gling128_2;
	break;
      case 17:
	jiffle = harmonics4;
	break;
      case 18:
	jiffle = ting1024;
	break;
      case 19:
	jiffle = ting4096;
	break;
      case 20:
	jiffle = arpeggio4096;
	break;
      case 21:
	jiffle = arpeggio4096down;
	break;
	//    case 21:
	//      jiffle = mimic_japan_pentatonic;
	//      break;

      case 22:
	jiffle = arpeggio_cont;
	break;
      case 23:
	jiffle = arpeggio_and_down;
	break;
      case 24:
	jiffle = stepping_down;
	break;
      case 25:
	jiffle = back_to_ground;
	break;
      case 26:
	jiffle = arpeggio_and_sayling;
	break;
      case 27:
	jiffle = simple_theme;
	break;
      case 28:
	jiffle = pentatonic_rising;
	break;
      default:
	selected_jiffle=0;	// as jiffle_names[selected_jiffle] index

	if (MENU.verbosity >= VERBOSITY_SOME)
	  MENU.outln_invalid();
      }
    }
    display_jiffletab(jiffle);
    break;

  case 'R':	// ratios
    if (MENU.maybe_display_more()) {
      MENU.out(F("ratio "));

#ifndef RAM_IS_SCARE	// enough RAM?	display jiffle names
      display_names(ratio_names, n_ratio_names, selected_ratio);
#endif
    }
    selected_ratio=MENU.numeric_input(selected_ratio);	// remember as index for ratio_names[selected_ratio]
    switch (selected_ratio) {
    case 0:
      ratios = ratios_RAM;
      break;
    default:
      if (selected_ratio >= n_ratio_names) {
	selected_ratio=0;

	if (MENU.verbosity >= VERBOSITY_SOME)
	  MENU.outln_invalid();
      }
      break;
    }
//    display_ratio(selected_ratio);
    break;

  case 'f':	// en_info
    // we work on pulses anyway, regardless dest
    for (int pulse=0; pulse<pl_max; pulse++)
      if (PULSES.selected_pulses & (1 << pulse))
	en_info(pulse);

    if (MENU.maybe_display_more()) {
      MENU.ln();
      selected_or_flagged_pulses_info_lines();
    }
    break;

  case 'F':	// en_INFO
    // we work on pulses anyway, regardless dest
    for (int pulse=0; pulse<pl_max; pulse++)
      if (PULSES.selected_pulses & (1 << pulse))
	en_INFO(pulse);

    if (MENU.maybe_display_more()) {
      MENU.ln();
      selected_or_flagged_pulses_info_lines();
    }
    break;

  case '{':	// enter_jiffletab
    menu_mode=JIFFLE_ENTRY_UNTIL_ZERO_MODE;
    jiffle = jiffle_RAM;
    jiffle_write_index=0;	// ################ FIXME: ################
    if(MENU.cb_peek()==EOF)
      if (MENU.verbosity)
	display_jiffletab(jiffle);
    break;

  case '}':	// display jiffletab, stop editing jiffletab
    if (MENU.verbosity)
      display_jiffletab(jiffle);

    menu_mode=0;
    jiffle_write_index=0;
    break;

  case 'd':	// divisor
    if(MENU.cb_peek()==EOF)
      MENU.outln(F("divisor"));

    input_value = MENU.numeric_input(divisor);
    if (input_value>0 )
      divisor = input_value;
    else
      MENU.outln(F("small positive integer only"));

    if (MENU.maybe_display_more())
      show_scale();
    break;

  case 'C':	// Calculator simply *left to right*	*positive integers only*
    MENU.maybe_calculate_input(&calc_result);

    if (MENU.maybe_display_more()) {
      MENU.out("==> "), MENU.outln(calc_result);

      if (calc_result > 1) {   	// show prime factors if(result >= 2)
	int p_factors_size=6;	// for harmonics I rarely use more than three, sometimes four ;)
	unsigned int p_factors[p_factors_size];
	MENU.out(F("prime factors of ")); MENU.out(calc_result);
	int highest = HARMONICAL.prime_factors(p_factors, p_factors_size, calc_result);
	for (int i=0; HARMONICAL.small_primes[i]<=highest; i++) {
	  MENU.tab();
	  MENU.out(F("("));
	  MENU.out(HARMONICAL.small_primes[i]);
	  MENU.out(F(")^"));
	  MENU.out(p_factors[i]);
	}
	MENU.ln();
      }
    }

    break;

  case 'D':	// DADA reserved for temporary code   testing debugging ...
    MENU.out_noop(); MENU.ln();
    break;

  case 'y':	// DADA reserved for temporary code   testing debugging ...
    {
      // temporary least-common-multiple  test code, unfinished...	// ################ FIXME: ################
      unsigned long lcm=1L;
      for (int pulse=0; pulse<pl_max; pulse++) {
	if (PULSES.selected_pulses & (1 << pulse)) {
	  MENU.out(pulse); MENU.tab(); PULSES.pulses[pulse].period.time; MENU.tab();
	  lcm = HARMONICAL.LCM(lcm, PULSES.pulses[pulse].period.time);
	  MENU.outln(lcm);
	}
      }

      MENU.out(F("==> lcm ")); MENU.out(lcm);
      struct time length;
      length.time = lcm;
      length.overflow = 0;
      PULSES.display_realtime_sec(length);
      MENU.ln();

      for (int pulse=0; pulse<pl_max; pulse++)
	if ((PULSES.selected_pulses & (1 << pulse)) && PULSES.pulses[pulse].period.time) {
	  MENU.out(pulse);
	  MENU.tab();
	  MENU.outln(lcm/PULSES.pulses[pulse].period.time);
	}
    }
  break;

  case 'm':	// multiplier
    if(MENU.cb_peek()==EOF)
      MENU.outln(F("multiplier"));

    input_value = MENU.numeric_input(multiplier);
    if (input_value>0 )
      multiplier = input_value;
    else
      MENU.outln(F("small positive integer only"));

    if (MENU.maybe_display_more())
      show_scale();

    break;

  case 'V':	// set voices
    if(MENU.cb_peek()==EOF)
      MENU.out(F("voices "));

    input_value = MENU.numeric_input(voices);
    if (input_value>0 && input_value<=CLICK_PULSES)
      voices = input_value;
    else
      MENU.outln_invalid();

    if (MENU.maybe_display_more())
      MENU.outln(voices);

    break;

  case 'b':	// toggle bottom down/up click-pin mapping bottom up/down
    if (MENU.verbosity)
      MENU.out(F("pin mapping bottom "));

    inverse = !inverse;	// toggle bottom up/down click-pin mapping

    if (MENU.maybe_display_more()) {
      if (MENU.verbosity) {
	if (inverse)
	  MENU.outln(F("up"));
	else
	  MENU.outln(F("down"));
      }
    }

    break;

  case 'X':	// PANIC BUTTON  reset, remove all (flagged) pulses, restart selections at none
/* DEACTIVATED 'X!' reboot ESP8266
// not very useful, you cannot continue on the same serial line, deactivated
#ifdef ESP8266	// 'X!' reboot ESP8266		hope it works on all ESP8266 boards, FIXME: test
    // 'X!' reboots ESP8266
    next_token = MENU.cb_peek();
    switch(next_token) {	// examine following input token
    case '!':	// 'X!' reboots ESP8266
      MENU.drop_input_token();	// ;)
      MENU.outln(F("HARDWARE RESET"));
      pinMode(7, OUTPUT);	// setting pin 7 to output reboots my board
      break;
    }
#endif
*/
    // reset, remove all (flagged) pulses, restart selections at none
    { int cnt=0;
      for (int pulse=0; pulse<pl_max; pulse++) {  // tabula rasa
	if (PULSES.pulses[pulse].flags) {
	  PULSES.init_pulse(pulse);
	  cnt++;
	}
      }

      // By design click pulses *HAVE* to be defined *BEFORE* any other pulses:
      PULSES.init_click_pulses();
      init_click_pins_OutLow();		// switch them on LOW, output	current off, i.e. magnets
      PULSES.selected_pulses=0L;		// restart selections at none

      PULSES.fix_global_next();

      if (MENU.verbosity) {
	MENU.out(F("\nremoved all pulses ")); MENU.outln(cnt);
	MENU.outln(F("switched pins off."));
      }
    }
    break;

  case 'Z':
    reverse_click_pins();

    if (MENU.maybe_display_more())
      MENU.outln(F("reverse_click_pins"));

    break;

  case 'E':	// enter experiment
    if (MENU.maybe_display_more()) {
      MENU.out(F("experiment "));

#ifndef RAM_IS_SCARE	// enough RAM?
      display_names(experiment_names, n_experiment_names, selected_experiment);
#endif
    }

    input_value = MENU.numeric_input(selected_experiment);
    if (input_value>=0 )
      selected_experiment = input_value;
    else
      MENU.outln_invalid();

    switch (selected_experiment) {	// initialize defaults, but do not start yet
    case 1:
      multiplier=2;
      divisor=1;
      sync=15;

      if (MENU.maybe_display_more()) {
	display_name5pars("setup_jiffle128", inverse, voices, multiplier, divisor, sync);
	Press_toStart();
      }
      break;

    case 2:
      sync=0;
      multiplier=1;
      divisor=1;

      if (MENU.maybe_display_more()) {
	display_name5pars("init_div_123456", inverse, voices, multiplier, divisor, sync);
	Press_toStart();
      }
      break;

    case 3:
      sync=1;
      multiplier=8;
      divisor=3;
      reverse_click_pins();	// ################ FIXME: not here ################

      if (MENU.maybe_display_more()) {
	// display_name5pars("setup_jiffles0", inverse, voices, multiplier, divisor, sync);
	MENU.out(F("setup_jiffles0("));
	MENU.out(inverse);
	display_next_par(voices);
	display_next_par(multiplier);
	display_next_par(divisor);
	display_next_par(sync);
	MENU.outln(F(")  ESP8266 Frogs"));
	Press_toStart();
      }
      break;

      case 4:
      multiplier=1;
      divisor=2;
      sync=0;
      jiffle=jiffletab;

      if (MENU.maybe_display_more()) {
	display_name5pars("setup_jiffles2345", inverse, voices, multiplier, divisor, sync);
	Press_toStart();
      }
      break;

    case 5:
      sync=0;		// FIXME: test and select ################
      multiplier=3;
      divisor=1;

      if (MENU.maybe_display_more()) {
	display_name5pars("init_123456", inverse, voices, multiplier, divisor, sync);
	Press_toStart();
      }
      break;

    case 6:
      sync=0;		// FIXME: test and select ################
      multiplier=1;
      divisor=1;

      if (MENU.maybe_display_more()) {
	display_name5pars("init_chord_1345689a", inverse, voices, multiplier, divisor, sync);
	Press_toStart();
      }
      break;

    case 7:
      sync=1;
      multiplier=1;
      divisor=6*7;

      if (MENU.maybe_display_more()) {
	display_name5pars("init_rhythm_1", inverse, voices, multiplier, divisor, sync);
	Press_toStart();
      }
      break;

    case 8:
      sync=5;
      multiplier=1;
      divisor=1;

      if (MENU.maybe_display_more()) {
	display_name5pars("init_rhythm_2", inverse, voices, multiplier, divisor, sync);
	Press_toStart();
      }
      break;

    case 9:
      sync=3;
      multiplier=1;
      divisor=1;

      if (MENU.maybe_display_more()) {
	display_name5pars("init_rhythm_3", inverse, voices, multiplier, divisor, sync);
	Press_toStart();
      }
      break;

    case 10:
      sync=1;
      multiplier=1;
      divisor=7L*3L;

      if (MENU.maybe_display_more()) {
	display_name5pars("init_rhythm_4", inverse, voices, multiplier, divisor, sync);
	Press_toStart();
      }
      break;

    case 11:
      sync=3;
      multiplier=3;
      divisor=1;

      if (MENU.maybe_display_more()) {
	display_name5pars("setup_jifflesNEW", inverse, voices, multiplier, divisor, sync);
	Press_toStart();
      }
      break;

    case 12:
      sync=1;
      multiplier=1;
      divisor=1;

      if (MENU.maybe_display_more()) {
	display_name5pars("init_pentatonic", inverse, voices, multiplier, divisor, sync);
	Press_toStart();
      }
      break;

    case 13:
      sync=1;	// or: sync=0;
      multiplier=1;
      divisor=1;
      voices=8;	//just for 'The Harmonical Strings Christmas Evening Sounds'
      inverse=false;
      // unsigned int harmonics4 = {1,1,1024, 1,2,1024, 1,3,1024, 1,4,1024, 0,0};
      jiffle=harmonics4;
      select_n(voices);
      display_name5pars("prepare_magnets", inverse, voices, multiplier, divisor, sync);
      prepare_magnets(inverse, voices, multiplier, divisor, sync);

      if (MENU.maybe_display_more()) {
	selected_or_flagged_pulses_info_lines();
	Press_toStart();
      }
      break;

    case 14:
      // magnets on strings, second take
      multiplier=1;
      divisor=1;
      inverse=false;

      ratios = pentatonic_minor;
      select_n(voices);
      prepare_ratios(false, voices, multiplier * 1024 , divisor * 1167, sync, ratios);
      jiffle=ting1024;
      select_n(voices);
      display_name5pars("E14", inverse, voices, multiplier, divisor, sync);

      if (MENU.maybe_display_more())
	selected_or_flagged_pulses_info_lines();
      break;

    case 15:
      // magnets on strings, third take
      multiplier=1;
      divisor=1;
      inverse=false;

      ratios = pentatonic_minor;
      select_n(voices);
      prepare_ratios(false, voices, multiplier * 4096 , divisor * 1167, sync, ratios);
      jiffle=ting4096;
      select_n(voices);
      display_name5pars("E15", inverse, voices, multiplier, divisor, sync);

      if (MENU.verbosity >= VERBOSITY_SOME)
	selected_or_flagged_pulses_info_lines();
      break;

    case 16:
      // piezzos on low strings 2016-12-28
      multiplier=4096;
      divisor=256;
      inverse=false;

      ratios = european_pentatonic;
      select_n(voices);
      prepare_ratios(false, voices, multiplier, divisor, sync, ratios);
      jiffle=ting4096;
      // jiffle = arpeggio4096;
      display_name5pars("E16 european_pent", inverse, voices, multiplier, divisor, sync);

      if (MENU.verbosity >= VERBOSITY_SOME)
	selected_or_flagged_pulses_info_lines();
      break;

    case 17:
      // magnets on steel strings, "japanese"
      multiplier=1;	// click
      // multiplier=4096;	// jiffle ting4096
      divisor=256*5;

      ratios = mimic_japan_pentatonic;
      select_n(voices);
      prepare_ratios(false, voices, multiplier, divisor, sync, ratios);
      jiffle=ting4096;
      display_name5pars("E17 mimic japan", inverse, voices, multiplier, divisor, sync);

      if (MENU.verbosity >= VERBOSITY_SOME)
	selected_or_flagged_pulses_info_lines();

      break;

    case 18:	// nylon stringed wooden box, piezzos
      ratios = pentatonic_minor;
      multiplier=1;	// click
      // multiplier=4096;	// jiffle ting4096
      // divisor=2048;

      // string tuning on 8/9
      //   multiplier=8;
      //   divisor=9*1024;
      // multiplier=1;
      // divisor=9*128;

      // multiplier=8*4096;	// jiffle ting4096
      // divisor=9*1024;
      multiplier=32;	// reduced
//#if defined(ESP32)	// used as test setup with 16 clicks
// ################ FIXME: ESP32 16 click ################
//      multiplier *= 4;
//#endif
      divisor=9;	// reduced
      jiffle=ting4096;

      select_n(voices);
      prepare_ratios(false, voices, multiplier, divisor, sync, ratios);
      display_name5pars("E18 pentatonic minor", inverse, voices, multiplier, divisor, sync);

      if (MENU.verbosity >= VERBOSITY_SOME)
	selected_or_flagged_pulses_info_lines();

      break;

#ifdef IMPLEMENT_TUNING		// implies floating point
    case 19:	// TUNING
      PULSES.selected_pulses=1;
      sweep_up=1;
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

    case 20:
      jiffle = arpeggio4096;
      MENU.play_KB_macro("X E12!aN *8 J20-.");
      break;

    case 21:
      jiffle = arpeggio4096down;
      MENU.play_KB_macro("X E12!aN *16 J21-.");
      break;

    case 22:
      jiffle = arpeggio_cont;				// :)	with pizzs
      MENU.play_KB_macro("X E12!aN *16 J22-.");
      break;

    case 23:
      jiffle = arpeggio_and_down;			// :) :)  arpeggio down instead pizzs
      MENU.play_KB_macro("X E12!aN *16 J23-.");
      break;

    case 24:
      jiffle = stepping_down;				// :) :)  stepping down
      MENU.play_KB_macro("X E12 S=0 !aN *16 J24-.");
      break;

    case 25:
      jiffle = back_to_ground;		// rhythm slowdown
      MENU.play_KB_macro("X E12!aN *32 J25-.");		// :)	rhythm slowdown
      break;

    case 26:
      jiffle = arpeggio_and_sayling;
      MENU.play_KB_macro("X E12!aN *32 J26-.");
      break;

    case 27:
      jiffle = simple_theme;
      MENU.play_KB_macro("X E12!aN *2 -.");
      break;

    case 28:				// for tuning
      jiffle = peepeep4096;
      MENU.play_KB_macro("X E12!aN *2 -.");

      break;

    default:
      if (MENU.verbosity >= VERBOSITY_SOME)
	MENU.outln_invalid();

      selected_experiment=0;
      break;
    }
    break;

  case '!':			// '!' setup and start experiment
    switch (selected_experiment) {
    case -1:
    case 0:
      break;
    case 1:
      setup_jiffle128(inverse, voices, multiplier, divisor, sync);
      break;
    case 2:
      init_div_123456(inverse, voices, multiplier, divisor, sync);
      break;
    case 3:
      setup_jiffles0(inverse, voices, multiplier, divisor, sync);    // ESP8266 Frog Orchester
      break;
    case 4:
      setup_jiffles2345(inverse, voices, multiplier, divisor, sync);
      break;
    case 5:
      init_123456(inverse, voices, multiplier, divisor, sync);
      break;
    case 6:
      init_chord_1345689a(inverse, voices, multiplier, divisor, sync);
      break;
    case 7:
      init_rhythm_1(inverse, voices, multiplier, divisor, sync);
      break;
    case 8:
      init_rhythm_2(inverse, voices, multiplier, divisor, sync);
      break;
    case 9:
      init_rhythm_3(inverse, voices, multiplier, divisor, sync);
      break;
    case 10:
      init_rhythm_4(inverse, voices, multiplier, divisor, sync);
      break;
    case 11:
      setup_jifflesNEW(inverse, voices, multiplier, divisor, sync);
      break;
    case 12:
      init_pentatonic(inverse, voices, multiplier, divisor, sync);
      break;
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
      // FIXME:	maybe make that default?
      PULSES.activate_selected_synced_now(sync, PULSES.selected_pulses); // sync and activate

      if (MENU.maybe_display_more()) {		// *then* maybe info ;)
	MENU.ln();
	selected_or_flagged_pulses_info_lines();
      }
      break;

    default:	// invalid
      selected_experiment=0;

      if (MENU.verbosity)
	MENU.outln_invalid();
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
}


/* **************************************************************** */
// include softboard on arduinos:
#ifdef ARDUINO
//  #include "libraries/Pulses/examples/pulses/softboard_page.ino"
//  #include <examples/softboard/softboard_page.ino>
#else
  #warning "Not compiling softboard_page on PC."
#endif

/* **************************************************************** */


/* **************************************************************** */
/* **************************************************************** */

/* please se README_pulses					    */

/*
   Copyright © Robert Epprecht  www.RobertEpprecht.ch   GPLv2
   http://github.com/reppr/pulses
*/

/* **************************************************************** */
/* **************************************************************** */
/*				END OF CODE			    */
/* **************************************************************** */
/* **************************************************************** */
