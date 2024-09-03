/*
 * ****************************************************************
 * softboard_page.ino
 * ****************************************************************
 */


/* **************************************************************** */
/* ****  CODE TO INSERT TO THE START OF YOUR PROGRAM follows:  **** */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#ifdef ARDUINO
  #if ARDUINO >= 100
    #include "Arduino.h"
  #else
    #include "WProgram.h"
  #endif
#else	// #include's for Linux PC test version
  #include <iostream>
#endif


/* ****   CODE TO INSERT TO THE START OF YOUR PROGRAM ends:    **** */
/* **************************************************************** */



/* **************************************************************** */
/* CODE TO INSERT INTO YOUR PROGRAM setup() follows: ************** */
/* Arduino setup()						*/

/*
  >>>>>>>>>>>>>>>> PUT this *BEFORE*  Arduino setup() <<<<<<<<<<<<<<<<
  // needed for MENU.add_page();
  void softboard_display();		// defined later on
  bool softboard_reaction(char token);	// defined later on
  >>>>>>>>>>>>>>>> PUT THESE BEFORE  Arduino setup() <<<<<<<<<<<<<<<<
*/

/*
  >>>>>>>>>>>>>>>> PUT this *IN*  Arduino setup() <<<<<<<<<<<<<<<<
  // add softboard to MENU:
  MENU.add_page("Arduino Softboard", 'H',	\
		&softboard_display, &softboard_reaction, 'H');

  >>>>>>>>>>>>>>>> PUT THIS IN Arduino setup() <<<<<<<<<<<<<<<<
*/

/* CODE TO INSERT INTO YOUR PROGRAM setup() ends. ***************** */
/* **************************************************************** */



/* **************************************************************** */
/* CODE TO INSERT INTO YOUR PROGRAM loop() follows: *************** */
/* Arduino loop()						*/

/*
  >>>>>>>>>>>>>>>> PUT this *BEFORE*  Arduino loop() <<<<<<<<<<<<<<<<
  bool maybe_run_continuous();	// defined later on
  >>>>>>>>>>>>>>>> PUT this *BEFORE*  Arduino loop() <<<<<<<<<<<<<<<<
*/

/* Arduino loop() template:
//	void loop() {	// ARDUINO
//	  MENU.lurk_then_do();
//	  maybe_run_continuous();	// maybe display input pin state changes
//
//	  // Insert your own code here.
//	  // Do not let your program block program flow,
//	  // always return to the main loop as soon as possible.
//	}
*/

/* CODE TO INSERT INTO YOUR PROGRAM loop() ends. ****************** */
/* **************************************************************** */



/* **************************************************************** */
/* ARDUINO BOARD SPECIFIC THINGS  try to use ARDUINO MACROS: */
#include <pins_arduino.h>

typedef int8_t gpio_pin_t;		// negative values might be used for pin extensions


// NodeMCU ESP8266 board
/*
  // on NodeMCU ESP8266 board Arduino defines digital pins
  static const gpio_pin_t D0   = 16;
  static const gpio_pin_t D1   = 5;
  static const gpio_pin_t D2   = 4;
  static const gpio_pin_t D3   = 0;
  static const gpio_pin_t D4   = 2;
  static const gpio_pin_t D5   = 14;
  static const gpio_pin_t D6   = 12;
  static const gpio_pin_t D7   = 13;
  static const gpio_pin_t D8   = 15;
  static const gpio_pin_t D9   = 3;
  static const gpio_pin_t D10  = 1;
 */

#ifndef NUM_DIGITAL_PINS		// try harder... ?
  #warning "#define NUM_DIGITAL_PINS	// FIXME: ################"

  #if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) // mega boards
    #define NUM_DIGITAL_PINS	70
  #elif defined(__SAM3X8E__)
    #ifdef PINS_COUNT	// on Arduino DUE	// FIXME: ################
      #define NUM_DIGITAL_PINS	PINS_COUNT	// FIXME: ################
    #else
      #warning "#define MISING NUM_DIGITAL_PINS	// FIXME: ################"
      #warning "#define NUM_DIGITAL_PINS 79	// FIXME: ################"
      #define NUM_DIGITAL_PINS	79		// FIXME: DUE ################
    #endif
  #else						// FIXME: 168/328 boards ???
    #define NUM_DIGITAL_PINS	20
  #endif

  #ifndef NUM_DIGITAL_PINS
    #error "#define NUM_DIGITAL_PINS		// FIXME: ################"
  #endif
#endif


#ifndef NUM_ANALOG_INPUTS		// try harder... ?
  #warning "#define NUM_ANALOG_INPUTS	// FIXME: ################"

  #if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) // mega boards
    #define NUM_ANALOG_INPUTS	16
  #elif defined(__SAM3X8E__)
     #define NUM_ANALOG_INPUTS	16		// FIXME: DUE ################
  #else						// FIXME: 168/328 boards ???
    #define NUM_ANALOG_INPUTS	6
  #endif

  #ifndef NUM_ANALOG_INPUTS
    #error "#define NUM_ANALOG_INPUTS		// FIXME: ################"
  #endif
#endif

#if defined ESP32	// it's a hack, does not check if the pin *can* do that...
  #undef  NUM_ANALOG_INPUTS
  #define NUM_ANALOG_INPUTS	NUM_DIGITAL_PINS
#endif

#ifndef digitalPinHasPWM	// ################
  #ifdef __SAM3X8E__		// FIXME: ################
    #warning "#define MISSING digitalPinHasPWM(p)"
    #define digitalPinHasPWM(p)         ((p) >= 2 && (p) <= 13)
  #else
    #error #define digitalPinHasPWM
  #endif
#endif


/*
  DIGITAL_IOs
  number of arduino pins configured for digital I/O
  *not* counting analog inputs:
*/
#if defined ESP8266
  #define DIGITAL_IOs	28	// ??? ################ FIXME: ################
#elif defined ESP32
  #define DIGITAL_IOs	40	// ??? ################ FIXME: ################
#else
  #define DIGITAL_IOs	(NUM_DIGITAL_PINS - NUM_ANALOG_INPUTS)	// FIXME: ################
#endif

/* **************************************************************** */
/* define some things early: */

#if ! defined ILLEGAL8
  #define ILLEGAL8	255
#endif
#if ! defined ILLEGAL16
  #define ILLEGAL16	0xffff
#endif
#if ! defined ILLEGAL32
  #define ILLEGAL32	0xffffffff
#endif

uint8_t PIN_digital = ILLEGAL8;
uint8_t PIN_analog = 0;		// 0 is save as default for analog pins

// Comment next line out if you do not want the analog2tone functionality:
#define has_ARDUINO_TONE

#if defined(__SAM3X8E__) || defined(ESP32)
  #undef has_ARDUINO_TONE
#endif

#ifdef has_ARDUINO_TONE
  uint8_t PIN_tone = ILLEGAL8;	// pin for tone output on a piezzo
#endif

// The following defaults *must* match with each other, choose one pair.
// either:
  bool extra_switch=false;	// extra functionality on digital pins
  int visible_digital_pins=DIGITAL_IOs;		// extra_switch==false
// or:
  // bool extra_switch=true;	// extra functionality on digital pins
  // int visible_digital_pins=NUM_DIGITAL_PINS;	// extra_switch==true



/* ****************  info on DIGITAL pins:  **************** */

#ifdef __SAM3X8E__
  #ifndef portModeRegister
    #warning "#define *MISSING* portModeRegister(P)."
    #define portModeRegister(P) ( (volatile RwReg*)( pgm_read_word( port_to_mode_PGM + (P))) )
  #endif
#endif

/*
  pin_info_digital()
  display configuration and state of a pin:
*/
const char pin_[] = "pin ";
const char high_[] = "HIGH";
const char low_[] = "LOW";

#if defined PULSES_SYSTEM
  extern bool show_pulses_pin_usage(gpio_pin_t pin);
#endif

extern void show_esp32_pin_capabilities(gpio_pin_t pin);
void pin_info_digital(gpio_pin_t pin) {
  //uint8_t mask = digitalPinToBitMask(pin);
#ifdef __SAM3X8E__	// FIXME: ################
  #warning "softboard does not run on Arduino Due yet! ################"
  Pio* const port = digitalPinToPort(pin);
#else
  uint8_t port = digitalPinToPort(pin);
#endif

#ifdef __SAM3X8E__	// FIXME: ################
  volatile RwReg* reg;

  if (port == NULL) return;

#else
  //volatile uint8_t *reg;

  if (port == NOT_A_PIN) return;
#endif

  // selected sign * and pin:
  if (pin == PIN_digital )
    MENU.out('*');
  else
    MENU.space();
  MENU.out(pin_);
  MENU.out((int) pin);
  MENU.tab();
#if defined(ESP32)
    #warning "I/O pin configuration info *not implemented on ESP32*."
    // MENU.out(F("(pin_info_digital() not implemented on ESP32)"));
    if (digitalRead(pin))
     MENU.out(high_);
    else
      MENU.out(low_);

#elif defined(ESP8266)		// FIXME: ################
    #warning "I/O pin configuration info *not implemented on ESP8266*."
    // MENU.out(F("(pin_info_digital() not implemented on ESP8266)"));
    if (digitalRead(pin))
     MENU.out(high_);
    else
      MENU.out(low_);

#elif defined(__SAM3X8E__)	// FIXME: !!! ################
    #warning "I/O pin configuration info *not implemented on Arduino DUE yet*."
    MENU.out(F("(pin_info_digital() not implemented on DUE yet)"));
    if (digitalRead(pin))
     MENU.out(high_);
    else
      MENU.out(low_);

#else		// old style Arduino hardware
  // see: <Arduino.h>

  // input or output?
  reg = portModeRegister(port);
  // uint8_t oldSREG = SREG;	// let interrupt ACTIVE ;)
  // cli();
  if (*reg & mask) {		// digital OUTPUTS
    // SREG = oldSREG;
    MENU.out(F("O  "));

    // high or low?
    reg = portOutputRegister(port);
    // oldSREG = SREG;		// let interrupt ACTIVE ;)
    // cli();
    if (*reg & mask) {		    // HIGH
      // SREG = oldSREG;
      MENU.out(high_);
    } else {			    // LOW
      // SREG = oldSREG;
      MENU.out(low_);
    }
  } else {			// digital INPUTS
    // SREG = oldSREG;
    MENU.out(F("I  "));

    // pullup, tristate?
    reg = portOutputRegister(port);
    // oldSREG = SREG;		// let interrupt ACTIVE ;)
    // cli();
    if (*reg & mask) {		    // pull up resistor
      // SREG = oldSREG;
      MENU.out(F("pullup"));
    } else {			    // tri state high-Z
      // SREG = oldSREG;
      MENU.out(F("floating"));
    }
  }
#endif	// hardware type

#if defined PULSES_SYSTEM
  MENU.tab();
  if(! show_pulses_pin_usage((gpio_pin_t) pin))
    MENU.tab(2);

  #if defined ESP32
  MENU.tab();
  show_esp32_pin_capabilities((gpio_pin_t) pin);
  #endif // ESP32
#endif // PULSES_SYSTEM

  MENU.ln();
} // pin_info_digital()

// display configuration and state of all digital pins:
void pins_info_digital() {
  for (int pin=0; pin<visible_digital_pins; pin++)
    pin_info_digital((gpio_pin_t) pin);
}


/* continuously watch a digital pins readings: */
/*
  watch_digital_input(pin)
  continuously display digital input readings,
  whenever the input changes:
*/

#define IMPOSSIBLE	-9785	// FIXME:  just a value not possible on analog input

int watch_seen=IMPOSSIBLE;
void watch_digital_start(gpio_pin_t pin) {
  watch_seen=IMPOSSIBLE;

  MENU.out(F("watching pin D"));
  MENU.out((int) pin);
  MENU.outln(F("\t\tr=stop"));
}


bool watch_digital_input(int pin) {	// return if there was *output*
  int value=digitalRead(PIN_digital);

  if (value != watch_seen) {
    watch_seen = value;

    MENU.out(F("*D"));  MENU.out((int) pin);
    MENU.tab();
    if (value)
      MENU.outln(high_);
    else
      MENU.outln(low_);

    return true;	// there was *output*
  }

  return false;		// only mandatory digital read, but *no output*
}


bool run_watch_dI=false;

// toggle_watch()  toggle continuous digital watch display on/off.
void toggle_watch() {
  run_watch_dI = !run_watch_dI;
  if (run_watch_dI)
    watch_digital_start(PIN_digital);
  else
    MENU.ln();
}


/* ****************  info on ANALOG pins:  **************** */
/*
  bar_graph(value)
  print one value & bar graph line:
*/
const char value_[] = "value ";


bool touch_VU=false;	// show touchRead(pin) instead of analogRead(pin)
void bar_graph(int value, bool newline=true) {
  int i, scale, length=64;

  #if defined ESP32
    if(touch_VU)
      scale=160;	// read touch sensor
    else
      scale=4095;	// normal analogRead()
  #else
    #if defined ESP8266
      scale=1024;	// on ESP8266 it really is ;)
    #else
      scale=1023;
    #endif
  #endif

  int stars = ((long) value * (long) length) / scale + 1 ;

  MENU.out(value); MENU.tab();
  if (value >=0) {
    for (i=0; i<stars; i++) {
      if (i == 0 && value == 0)		// zero?
	MENU.out('0');
					// middle or top?
      else if								\
	((i == length/2 && value == 512) || (i == length && value == scale))
	MENU.out('|');
      else
	MENU.out('*');			// all other values '*'
    }
    if(newline)
      MENU.ln();
  } else {
    MENU.out(F("negative value "));
    MENU.out(value);
    MENU.OutOfRange();
  }
} // bar_graph() (softboard)


// Display analog pin name and value as number and bar graph:
void pin_info_analog(gpio_pin_t pin) {
  if (pin == PIN_analog)
    MENU.out(F("*A"));	// DADA
  else
    MENU.out(F(" A"));
  MENU.out((int) pin);
  MENU.tab();
  bar_graph(analogRead(pin), false /* no newline */);
#if defined PULSES_SYSTEM
  MENU.tab();
  show_pulses_pin_usage((gpio_pin_t) pin);
#endif
  MENU.ln();
}


/*
  pins_info_analog()
  Display analog snapshot read values and bar graphs, a line each analog input:
*/
const char bar_graph_header[] =	\
  "\npin\tvalue\t|\t\t\t\t|\t\t\t\t|";

void pins_info_analog() {
  MENU.outln(bar_graph_header);

  for (int i=0; i<NUM_ANALOG_INPUTS; i++)
    pin_info_analog((gpio_pin_t) i);

  MENU.ln();
}

#if defined ESP32	// touch interface
  void pin_info_touch(int pin) {
    bool VU_type_was=touch_VU;
    MENU.out(F(" T "));
    MENU.out(pin);
    MENU.tab();
    touch_VU=true;
    bar_graph(touchRead(pin));
    touch_VU=VU_type_was;
  }

  void pins_info_touch() {
    int i;

    MENU.outln(bar_graph_header);

    for (i=0; i<40; i++)
      pin_info_touch(i);

    MENU.ln();
  }
#endif

/* **************************************************************** */
/*
  Running 'continuous' display types or similar.
  Implemented without waiting allows 'run-through' programming.

  bar_graph_VU(pin):
  Continuous display changes on an analogue input.
  Display a scrolling bar graph over the readings.
  A new line is displayed as soon as the reading changes for more
  than +/- tolerance.

  tolerance can be changed by sending '+' or '-'

  run-through, don't wait...
*/
bool run_VU=false;


int VU_last=IMPOSSIBLE;

// tolerance default 0. Let the user *see* the noise...
int bar_graph_tolerance=0;

void feedback_tolerance(unsigned int tolerance) {
  MENU.out(F("tolerance "));
  MENU.outln(tolerance);
}


void VU_init(int pin) {
  VU_last=IMPOSSIBLE;	// just an impossible value

  MENU.out(F("pin\tval\t'v'=stop\t'+' '-' '*' '/' set "));
  feedback_tolerance(bar_graph_tolerance);
}


// toggle_VU()  toggle continuous VU display on/off.
void toggle_VU() {
  run_VU = !run_VU;
  if (run_VU)
    VU_init(PIN_analog);
  else {
    MENU.outln(F("VU off"));
    MENU.ln();
    touch_VU = false;	// reset touch_VU mode when switching off
  }
}

/*
  bool bar_graph_VU(pin):
  Continuous display changes exceeding a tolerance on an analogue input.
  Display a scrolling bar graph over the readings.
  A new line is displayed as soon as the reading changes for more
  than +/- tolerance.

  tolerance can be changed by sending '+' or '-'

  run-through, don't wait...
  returns true, if there was *output*
*/
//bool touch_VU=false;	// show touchRead(pin) instead of analogRead(pin)
bool bar_graph_VU(int pin) {	// return true, if there was output
  int value;

  if(touch_VU)
    value =  touchRead(pin);
  else
    value =  analogRead(pin);

  if (abs(value - VU_last) > bar_graph_tolerance) {
    MENU.out('*');
    if(touch_VU)
      MENU.out('T');
    else
      MENU.out('A');
    MENU.out((int) pin);
    MENU.tab();
    bar_graph(value);
    VU_last = value;

    return true; // there was *output*				 return true;
  }

  return false;  // analog read *had to be done*, but no output	 return false;
} // bar_graph_VU() (softboard)


#ifdef has_ARDUINO_TONE
/*
  bool analog2tone(int analogPIN, int PIN_tone):
  Simple acoustical feedback of analog readings using arduino tone()

  This very simple version is probably not compatible with pulses.

  Arduino tone() gives me some garbage below 32hz and is unusable below 16hz
  So analog2tone switches tone off for analog reading 0 and
  adds an offset of 31hz starting from analog reading 1.

  returns true  if an analog read had to be taken which might trigger tone change and output
*/
bool run_analog2tone = false;

//unsigned int analog2tone_tolerance = 1;		// tolerance regarding analog reading
unsigned int analog2tone_tolerance = 0;		// tolerance regarding analog reading
unsigned long analog2tone_timeInterval = 36;	// minimal time between measurements
int lastToneReading = -1;

const int unusable_tones = 31;		// tone() plays some garbage below 32hz :(
// const int unusable_tones = 15;	// tone() plays garbage below 16hz :(

bool analog2tone(int analogPIN, int PIN_tone) {
  if (PIN_tone == ILLEGAL8)		// ILLEGAL *pin*	return false;
    return false;

  static unsigned long lastToneTime = 0;
  unsigned long now = millis();

  if (! now - lastToneTime >= analog2tone_timeInterval)
      return false;		// not the right *time* yet	return false;

  // time to take a reading and check
  lastToneTime = now;
  int ToneReading=analogRead(analogPIN);
  if (!abs(ToneReading - lastToneReading) > analog2tone_tolerance)
    return false;		// inside *tolerance*		return false;

  lastToneReading = ToneReading;

  // unsigned int hz = ToneReading*1;	// arbitrary factor
  unsigned int hz = ToneReading;		// arbitrary factor==1

  // ################ FIXME: make Arduino tone mending a compile time option ################
  // arduino tone() delivers garbage below 16hz,
  // so we mend that a bit:
  if(hz) {
    hz += unusable_tones;	// start at lowest ok tone=16hz)
    tone(PIN_tone, hz);
  } else			// switch tone off for zero
    noTone(PIN_tone);

  if(MENU.verbosity >= VERBOSITY_CHATTY) {
    MENU.out(F("tone hz="));
    MENU.outln(hz);
  }

  return true;			// something done (at least an analog reading)
}


// toggle_tone()  toggle continuous sound pitch follower on/off.
void toggle_tone() {
  run_analog2tone = !run_analog2tone;
  if (run_analog2tone)
    lastToneReading = -1;
  else
    noTone(PIN_tone);

  if(MENU.verbosity >= VERBOSITY_SOME) {
    MENU.out(F("tone "));
    if (run_analog2tone)
      MENU.outln(F("ON"));
    else
      MENU.outln(F("off"));
  }
}
#endif // has_ARDUINO_TONE


/*
  bool maybe_run_continuous():
  Check if to continuously show/sound analog/digital input changes

  return true  if any time consuming action like menu output was done
*/
bool maybe_run_continuous() {
  if (run_VU) {
    if (touch_VU) {	// touchRead()
      if (bar_graph_VU(PIN_digital))
	return true;		// there was *output*
    } else		// analogRead()
    if (bar_graph_VU(PIN_analog))
      return true;		// there was *output*
  }

#ifdef has_ARDUINO_TONE
  if(run_analog2tone)
    if(analog2tone(PIN_analog, PIN_tone))
      return true;		// *tone* changed and *output*
#endif

  if (run_watch_dI)
    if (watch_digital_input(PIN_digital))
      return true;		// there was *output*

  return false;
}


void stop_continuous() {	// unused
  run_VU=false;
  run_watch_dI=false;
#ifdef has_ARDUINO_TONE
  if(run_analog2tone)
    toggle_tone();
#endif
}


/* **************************************************************** */
// *very* simple i2c scanner
#if defined USE_i2c
#include <Wire.h>

int i2c_scan() {
  int found=0;
  int error;

  for(int adr = 1; adr < 127; adr++ ) {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the adr.
    Wire.beginTransmission(adr);
    error = Wire.endTransmission();

    if (error == 0) {
      found++;
      MENU.out("I2C device at ");
      MENU.out_hex(adr);
      MENU.tab();
      switch(adr) {
      case 0x20:
      case 0x21:
      case 0x22:
      case 0x23:
      case 0x24:
      case 0x25:
      case 0x26:
      case 0x27:
	MENU.outln(F("MCP23008  8-bit port expander"));
	break;

      case 0x30:	// could be VL53L0X after moving?
      case 0x31:	// could be VL53L0X after moving?
      case 0x32:	// could be VL53L0X after moving?
      case 0x33:	// could be VL53L0X after moving?
	MENU.out(F("? "));
      case 0x29:	// VL53L0X on default address
	MENU.outln(F("GY-530  VL53L0X TOF distance sensor"));
	break;

      case 0x39:
	MENU.outln(F("APDS9960  proximity, gesture, RGB sensor"));
	break;

      case 0x40:
	MENU.outln(F("SHT21  humidity, temperature sensor"));
	break;

      case 0x50:
      case 0x51:
      case 0x52:
      case 0x53:
      case 0x54:
      case 0x55:
      case 0x56:
      case 0x57:
	MENU.outln(F("EEPROM AT24CX"));	// probably on a DS3231 RTC module
	break;
      // case 0X57:	// ??? at24cx OR 24c32 ???

      case 0x5A:	// MPR121 or 24C32
      case 0x5B:	// MPR121 or 24C32
      case 0x5C:	// MPR121 or 24C32
      case 0x5D:	// MPR121 or 24C32
	MENU.out(F("MPR121 touch sensor ("));
	switch(adr) {	// ADR address selecting
	case 0x5A:
	  MENU.out(F("ADR:nc"));
	    break;
	case 0x5B:
	  MENU.out(F("ADR:+3V"));
	    break;
	case 0x5C:
	  MENU.out(F("ADR:SDA"));
	    break;
	case 0x5D:
	  MENU.out(F("ADR:SCL"));
	    break;
	}
	MENU.out(F("\t)"));
      case 0x58:
      case 0x59:
	// 0x5A, 0x5B, 0x5C, 0x5D see above	(can be either MPR121 or 24C32)
      case 0x5E:
	MENU.outln(F("EEPROM 24C32"));	// maybe on a DS3231 RTC module ???
	break;

      case 0x68:
	MENU.outln(F("MPU6050 / DS1307 / DS3231 *????*"));	// they all can use the same address :(
	break;
      case 0x69:
	MENU.outln(F("MPU6050"));	// other address
	break;

      default:
	MENU.outln('!');
      }
    } else if (error==4) {
      MENU.out("Unknown error at address ");
      MENU.out_hex(adr);
      MENU.ln();
    }
  }

  MENU.out(found);
  MENU.outln(F(" I2C devices found"));

  return found;
} //i2c_scan()
#endif

/* **************************************************************** */
/*
   Menu softboard display and reaction:
*/

/* **************************************************************** */
/* Building menu pages:

   The program defines what the menu pages display and how they react.

   The menu can have multiple pages, the selected one gets displayed.
   On start up the first one added will be selected.

   The menu pages define keys ("tokens") and the corresponding actions.

   Below the tokens of the selected/displayed page tokens of other pages
   can be configured to remain visible in groups of related pages.

   For each menu page we will need:

     hotkey		selects this menu page.
     title		identifies page to the user.
     display function	displays the menu page.
     action function	checks if it is responsible for a token,
			if so do its trick and return true,
			else return false.
*/


/* ****************  softboard menu display:  **************** */
const char select_[] = "select ";
const char pinFor[] = " pin for ";
const char toWork_[] = " to work on:\t";
const char pin__[] = "pin (";


// Factored out display functions:

void _select_digital(bool key) {
  if (key)
    MENU.out(F("D="));
  MENU.out(select_);
  MENU.out(F("digital"));
  MENU.out(pinFor);
  MENU.out(F("'d, r, I, O, H, L, W"));
#if defined ESP32	// touch interface
  MENU.out(F(", t"));
#endif
  MENU.outln('\'');
}


void _select_analog(bool key) {
  if (key)
    MENU.out(F("\nA="));
  MENU.out(select_);
  MENU.out(F("analog"));
  MENU.out(pinFor);
#ifdef has_ARDUINO_TONE
  MENU.out(F("'a, v, t'"));
#else
  MENU.out(F("'a, v'"));
#endif
}

#if defined ESP32
  #include "esp_get_infos.h"
#endif

void softboard_display() {
  MENU.outln(F("Arduino Softboard  http://github.com/reppr/pulses/\n"));

#if defined ESP32
  display_esp_versions();
  MENU.ln();
  MENU.out(F("MAC: "));
  MENU.outln(getMacAddress());

#elif defined ESP8266
  MENU.out("ESP chip ID: ");
  MENU.out_hex(ESP.getChipId());
  MENU.ln();
#endif
  MENU.ln();

#if defined PULSES_SYSTEM
  MENU.outln(F("'P'=show PULSES pin usage"));
  MENU.ln();
#endif

  _select_digital(true);
  MENU.out(toWork_);
  MENU.out(pin__);
  if (PIN_digital == ILLEGAL8)
    MENU.out(F("no"));
  else
    MENU.out((int) PIN_digital);
  MENU.outln(')');
  MENU.out(F("O=OUTPUT\tI=INPUT\t\tH=HIGH\tL=LOW\tPWM: W=WRITE\td=pin info"));
  MENU.ln();

  _select_analog(true);
  MENU.out(toWork_);
  MENU.out(pin__);
  MENU.out((int) PIN_analog);
  MENU.outln(')');

#ifdef has_ARDUINO_TONE
  MENU.out(F("watch over time:\tv=VU bar  r=read\tt=tone\tT=TonePIN ("));
  MENU.out(( int) PIN_tone);
  MENU.outln(F(")"));
#else
  MENU.outln(F("watch over time:\tv=VU bar\tr=read"));
#endif

#if defined ESP32	// touch interface?
  MENU.outln(F("\nT=touch<nn>"));
#endif

  MENU.out(F("\n.=all digital\t,=all analog\t;=both\tf=touch\tx=extra\t"));
#if defined USE_i2c
  MENU.out(F("\tC=i2c scan"));
#endif
#if defined USE_MPU6050_at_ADDR	// MPU-6050 6d accelero/gyro
  MENU.out(F("\tG=accelero/gyro"));
#endif
  MENU.ln();

#if defined USE_RTC_MODULE	// in PULSES_SYSTEM (only)
  MENU.outln(F("t=time\tt! <year> <month> <day> <hour> <minute> =set time"));
#endif
}


/* ****************  softboard reactions:  **************** */

/*
  Toggle extra functionality
  * Visability of analog inputs as general digital I/O pins
    (only this one item implemented)
*/
void toggle_extra() {
  extra_switch = !extra_switch;
  if (extra_switch)
    visible_digital_pins=NUM_DIGITAL_PINS;
  else
    visible_digital_pins=DIGITAL_IOs;
}


// Factored out helper function
// digital_pin_ok()  Checks if PIN_digital has been set
bool digital_pin_ok() {
  // testing on ILLEGAL8 is enough in this context
  if (PIN_digital == ILLEGAL8) {
    _select_digital(true);
    MENU.ln();
    return false;
  } // else
  return true;
} // digital_pin_ok()


// bool softboard_reaction(char token)
// Try to react on token, return success flag.
const char pwm_[] = "PWM ";
const char noHw_[] = "no hardware ";
const char analogWrite_[] = "\tanalogWrite(";

bool softboard_reaction(char token) {
  long newValue;
#ifdef ESP8266	// FIXME: ESP32	################
  const int pwm_maX=1024;
#else
  //const int pwm_maX=256;
#endif

  switch (token) {
  case 'A':
    _select_analog(false);
    MENU.tab();

    newValue = MENU.numeric_input(PIN_analog);
    if (newValue>=0 && newValue<NUM_ANALOG_INPUTS)
      PIN_analog = newValue;
    else
      MENU.OutOfRange();

    MENU.out(pin_);
    MENU.outln((int) PIN_analog);
    break;

  case 'D':
    _select_digital(false);
    MENU.tab();

    newValue = MENU.numeric_input(PIN_digital);
    if (newValue>=0 && newValue<visible_digital_pins) {
      PIN_digital = newValue;
      pin_info_digital(PIN_digital);
    } else
      if (newValue != ILLEGAL8)
	MENU.OutOfRange();

    touch_VU=false;	// maybe not?
    break;

  case 'O':
    if (digital_pin_ok()) {
      pinMode(PIN_digital, OUTPUT);
      pin_info_digital(PIN_digital);
    }
    break;

  case 'I':
    if (digital_pin_ok()) {
      pinMode(PIN_digital, INPUT);
      pin_info_digital(PIN_digital);
    }
    break;

  case 'H':  // no problem to use H here, same as the menu code.
    if (digital_pin_ok()) {
      digitalWrite(PIN_digital, HIGH);
      pin_info_digital(PIN_digital);
    }
    break;

  case 'L':
    if (digital_pin_ok()) {
      digitalWrite(PIN_digital, LOW);
      pin_info_digital(PIN_digital);
    }
    break;

#ifndef ESP32	// FIXME: ESP32
  case 'W':
    if (digital_pin_ok()) {

      // can the pin do hardware PWM?
      if (!digitalPinHasPWM(PIN_digital)) {	// *no PWM* on this pin
	MENU.out(noHw_); MENU.out(pwm_);
	MENU.out(F("on ")); MENU.out(pin_);
	MENU.outln((int) PIN_digital);

	MENU.skip_numeric_input();
      } else {					// pin *can* do PWM
	MENU.out(pin_); MENU.out((int) PIN_digital);
	MENU.tab();
	MENU.out(pwm_); MENU.out(F("write "));
	newValue = MENU.numeric_input(ILLEGAL32);
	if (newValue>=0 && newValue<pwm_maX) {
	  MENU.out(newValue);

	  analogWrite(PIN_digital, newValue);
	  MENU.out(F("\tanalogWrite(")); MENU.out((int) PIN_digital);
	  MENU.out_comma_();
	  MENU.out(newValue);
	  MENU.outln(')');
	} else
	  MENU.OutOfRange();
      }
    }
    break;
#endif	// ! ESP32

#if defined PULSES_SYSTEM
  case 'P': // 'P' show PULSES pin usage
    extern void show_pulses_all_pins_usage();
    show_pulses_all_pins_usage();	// TODO: keep 'P'?	capabilities?	verbosity
    break;
#endif

  case 'a':
    MENU.outln(bar_graph_header);
    pin_info_analog(PIN_analog);
    break;

  case 'd':
    if (digital_pin_ok())
      pin_info_digital(PIN_digital);
    break;

  case 'v':
    toggle_VU();
    break;

  case '+':
#ifdef has_ARDUINO_TONE
    if (!run_VU && !run_analog2tone)
      return false;    // *only* responsible if (run_VU || run_analog2tone)

    if (run_VU)
      feedback_tolerance(++bar_graph_tolerance);
    if (run_analog2tone)
      feedback_tolerance(++analog2tone_tolerance);
#else
    if (!run_VU)
      return false;    // *only* responsible if (run_VU)

    feedback_tolerance(++bar_graph_tolerance);
#endif
    break;

  case '*':	// increase tolerance
    if (!run_VU)
      return false;    // *only* responsible if (run_VU)

    bar_graph_tolerance++;
    bar_graph_tolerance *= 10;
    bar_graph_tolerance /= 9;
    MENU.tab(); MENU.outln(bar_graph_tolerance);
    feedback_tolerance(bar_graph_tolerance);
    break;

  case '/':	// decrease tolerance
    if (!run_VU)
      return false;    // *only* responsible if (run_VU)

    bar_graph_tolerance *= 9;
    bar_graph_tolerance /= 10;

    feedback_tolerance(bar_graph_tolerance);
    break;

  case '-':
#ifdef has_ARDUINO_TONE
    if (!run_VU && !run_analog2tone)
      return false;    // *only* responsible if (run_VU || run_analog2tone)

    if (run_VU)
      if (bar_graph_tolerance)		// only if not already zero
	feedback_tolerance(--bar_graph_tolerance);
    if (run_analog2tone)
      if (analog2tone_tolerance)	// only if not already zero
	feedback_tolerance(--analog2tone_tolerance);
#else
    if (! run_VU)
      return false;    // *only* responsible if (run_VU)

    if (bar_graph_tolerance)		// only if not already zero
      feedback_tolerance(--bar_graph_tolerance);
#endif

    break;

  case 'r':
    if (digital_pin_ok()) {
      toggle_watch();
      /*
	I do *not* call	pinMode(PIN_digital, INPUT) from here.
	Looks more flexible to me to let the user do that,
	if she wants.
      */
    }
    break;

#if defined USE_RTC_MODULE	// in PULSES_SYSTEM (only)
  case 't':
    show_DS1307_time_stamp();	// DS3231 or DS1307
    MENU.ln();
    if(MENU.check_next('!')) {
      uint8_t year=21;	// year 2021
      uint8_t month=11;
      uint8_t day=11;
      uint8_t hour=11;
      uint8_t minute=11;
      uint8_t second=0;
      //uint8_t dayOfWeek=0;	// invalid

      // year
      newValue = MENU.numeric_input(year);
      if ( newValue >= 2100 || newValue < 0 || (newValue > 99 && newValue < 2000))
	MENU.outln_invalid();		// *all* invalid cases...
      else {
	if (newValue < 100)
	  year = newValue;
	else
	  year = newValue - 2000;

	// month
	newValue = MENU.numeric_input(month);
	if ( newValue > 12 || newValue < 1)
	  MENU.outln_invalid();		// *all* invalid cases...
	else {
	  month = newValue;

	  // day
	  newValue = MENU.numeric_input(day);
	  if ( newValue < 1 || newValue > 31)
	    MENU.outln_invalid();		// invalid cases, but no check 31...
	  else {
	    day = newValue;

	    // hour
	    newValue = MENU.numeric_input(hour);
	    if ( newValue > 23 || newValue < 1)
	      MENU.outln_invalid();		// *all* invalid cases...
	    else {
	      hour = newValue;

	      // minute
	      newValue = MENU.numeric_input(minute);
	      if ( newValue > 60 || newValue < 0)
		MENU.outln_invalid();		// *all* invalid cases...
	      else {
		minute = newValue;
		second = 0;

		// void set_DS1307_time(byte second, byte minute, byte hour, byte month, byte year)
		set_DS1307_time(second, minute, hour, day, month, year);
		show_DS1307_time_stamp();	// DS3231 or DS1307
		MENU.ln();
	      }
	    }
	  }
	}
      }
    }
    break;
#endif

#if defined ESP32	// touch interface?
  case 'T':	// ################ TODO: clashes with 'T' for Arduino style tone();
    newValue = MENU.numeric_input(PIN_digital);
    if (newValue>=0 && newValue<visible_digital_pins) {
      PIN_digital = newValue;
    } else {
      if (newValue != ILLEGAL8)
	MENU.OutOfRange();
    }

    touch_VU=true;
    run_VU=true;
#endif

#if defined PULSES_SYSTEM
    if(PIN_digital == ILLEGAL8)
      PIN_digital = HARDWARE.morse_touch_input_pin;
#endif

    VU_init(PIN_digital);
    break;

#ifdef has_ARDUINO_TONE
  case 't':
    toggle_tone();
    break;

  case 'T':	// ################ TODO: clashes with 'T' for touch interface
    if(run_analog2tone)
      toggle_tone();	// switch old tone off

    newValue = MENU.numeric_input(PIN_tone);
    if (newValue>=0 && newValue<NUM_DIGITAL_PINS) {
      PIN_tone = newValue;
      MENU.out(pin_);
      MENU.outln((int) PIN_tone);
      toggle_tone();	// switch tone ON
    } else {
      MENU.OutOfRange();
    }
    break;
#endif

  case '.':	// all digital
    MENU.ln();
    pins_info_digital();
    MENU.ln();
    break;

  case ',':	// all analog
    pins_info_analog();
    break;

#if defined ESP32
  case 'f':	// all touch
    pins_info_touch();
    break;
#endif

  case ';':	// all ;)
    MENU.ln();
    pins_info_analog();
    pins_info_digital();
#if defined ESP32
    pins_info_touch();
#endif
    MENU.ln();
    break;

  case 'x':	// toggle extended
    toggle_extra();
    if(MENU.verbosity >= VERBOSITY_SOME) {
      MENU.out(F("Show pins: "));
      MENU.outln(visible_digital_pins);
    }
    break;

#if defined USE_i2c
  case 'C':	// i2c_scan()	#define USE_i2c to activate
    i2c_scan();
    break;
#endif

#if defined USE_MPU6050_at_ADDR	// MPU-6050 6d accelero/gyro
  case 'G':	// MPU-6050 6d accelero/gyro
    accGyro_data_display();
    break;
#endif

  default:
    return 0;		// token not found in this menu
  }
  return 1;		// token found in this menu
}


/* **************************************************************** */
/* README

README softboard

   softboard  http://github.com/reppr/pulses

   Arduino hardware/software developing/testing tool.

   This version is implemented as an example for the Menu library.
   Using a library makes it easy to include in your own sketches.
   You can add more menu pages with other functionality to your sketch.


Description

   Softboard is a kind of an Arduino software breadboard.

   Simple Arduino hardware Menu interface over a serial line like USBserial.
   Softboard gives you an interface to the Arduino hardware
   by sending simple commands:
   * show infos about arduino pins and RAM usage
   * set pin mode
   * write pin states
   * read analog and digital inputs
   * write PWM values
   * continuously display changes on analog or digital inputs as bar graph
   * play analog readings back as the pitch of a tone


   Use it to test hardware like sensors, motors, things you want
   to run quick tests after setting something up on a (real) breadboard.

   Then you can use it to test software parts of your program while
   you write it and fit parts together.

   Later, when everything is working you can throw out the Menu code
   from your sketch, or keep it as an interface to your program.


   Send a sequence of one letter commands and numeric chiffres
   followed by a linebreak over a serial line (say from your computer)
   to the arduino to trigger menu actions and get infos.

   The arduino will buffer serial input byte by byte (without waiting)
   until a terminating linefeed is received as an end token.
   Any sequence of one or more '\n'  '\c'  '\0' is accepted as end token.


   Commands can read and set I/O pin configuration and states,
   switch pins on/off, read and write digital and analog values,
   switch pullup/high-z, continuously watch changing inputs over time.
   You can also connect a piezzo to a spare digital pin and listen
   to the changes on an analog input coded as pitch of a tone.


   Send '?' (and a linefeed) over serial line to see the menu.
   It displays some basic infos and shows some one-letter commands.


'run-through' cpu time friendly implementation:

   Softboard tries not to block the processor longer then needed and
   to let as much cpu time as possible to any other code possibly running.
   Menu will just run through it's code and return immediately.


Integrate your own code:

   It is easy to define Menu pages as an interface to your own code.
   Please have a look at the Menu/examples/ to see how to do this.

   You can define multiple Menu pages and define visability groups
   for pages sharing the same commands.

   Softboard is just a definition of a Menu page, you can add pages
   or delete them when compiling your program as you like.


Installation:

   Get it from http://github.com/reppr/pulses
   Put contents of the pulses/libraries/ folder into sketchbook/libraries/

   Softboard is implemented as an example of libraries/Menu/
   After a restart the Arduino GUI shows softboard under
   File >> Sketchbook >> libraries >> Menu >> softboard

	Arduino versions older than 1.0 need a fix:
	  see this thread:
	  http://code.google.com/p/arduino/issues/detail?id=604&start=200

	    short version (adapt to your arduino version)
	      edit file arduino-0022/hardware/arduino/cores/arduino/wiring.h
	      comment out line 79 (round macro)
	      #define round(x)     ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))
	      tested on arduino-0023

	For older arduino versions you also have to rename the sketch file
	from softboard.ino to softboard.pde


How it works:

   You communicate with the arduino over a serial connection
   that could be a real serial line or an usb cable.
   The library should be able to deal with other stream interfaces, btw.

   The minimalistic menu shows you one letter commands and listens to
   your input. Serial inputs are buffered until you send a linefeed.
   Then your inputs (commands and numbers as sequences of chiffres)
   will be read and acted upon.


Configure your terminal program:

   Do configure your terminal emulation program to send some sort of
   line ending code, usual culprits should work.

   Set arduino baud rate by editing the line starting with
   #define BAUDRATE in menu_IO_configuration.h
   and set it to the same value in your terminal software.

   So if for example you use the Arduino 'Serial Monitor' window
   check that it *does* send 'Newline' (in the bottom window frame)
   and set baud rate to the same value as BAUDRATE on the arduino.


   Send '?' (and a linefeed) over serial line to see the menu.
   It displays some basic infos and shows some one-letter commands.

   'e' toggle serial echo.
   'x' toggle visibility of analog inputs as extra digital I/O pins.



Some examples:

       Always let your terminal send a linefeed after the example string
       to trigger the menu actions.

       Spaces in the strings can be omitted.



   Example 1:  'D13 OH' switch LED on   (D select pin, O output, H high)
	       'L'      off again       (L low)


   Example 2:  writing high to an input activates internal pullup resistor

	       'D13 IH' pullup LED pin  (D select pin, I input, H high)

					LED glows at low level
	       'O'      LED as OUTPUT   now the LED is on


   Example 3:  Watch an analog input like a VU meter, changing over time.
	       See electric noise on unconnected floating A0 input
	       scrolling over your serial terminal.
	       (Touch the input if there is no visible signal.)

	       Or connect a sensor to the input and explore its data...

	       'A0 v'      A=select pin (both analog or digital)
			   v=display analog read values and bar graphs

			   Stop it by pressing 'v' again.


*A0	77	*****
*A0	63	****
*A0	74	*****
*A0	84	******
*A0	115	********
*A0	165	***********
*A0	237	***************
*A0	289	*******************
*A0	285	******************
*A0	241	****************
*A0	159	**********
*A0	97	*******
*A0	70	*****
*A0	63	****
*A0	76	*****
*A0	86	******
*A0	123	********
*A0	171	***********
*A0	248	****************
*A0	294	*******************
*A0	280	******************
*A0	227	***************
*A0	144	**********
*A0	87	******
*A0	66	*****



   Example 4:  Listen to an analog input value changing the pitch of a piezzo tone.
	       This example does a similar thing as #3 playing an analog value back
	       as the pitch of a sound on a Piezzo connected from pin 8 to ground:

	       'A0 T8'	Select analog input and the pin for the piezzo.
	       't'	toggles the tone on and off.

	       You will hear a sound on the piezzo now. The pitch will change
	       according to the reading on the analog pin.


   Example 5:  ','  Display snapshot values of all analog inputs.

			[fixed font only]

pin     value   |                               |                               |
*A0     609     ***************************************
 A1     504     ********************************
 A2     451     *****************************
 A3     398     *************************
 A4     383     ************************
 A5     409     **************************



   Example 6:  '.'  Info on all digital pin's I/O configuration and state.

 pin 0	I  floating
 pin 1	I  floating
 pin 2	O  LOW
 pin 3	O  HIGH
 pin 4	I  floating
*pin 5	I  pullup       // * indicates the selected pin
 pin 6	I  floating
 pin 7	I  floating
 pin 8	I  floating
 pin 9	I  floating
 pin 10	I  floating
 pin 11	I  floating
 pin 12	I  floating
 pin 13	I  floating

   btw: The example output was generated after 'D2OLD3OHD5IH'.
	'D2OL D3OH D5IH' is the same, but easier to read.



   Copyright © Robert Epprecht  www.RobertEpprecht.ch   GPLv2

   http://github.com/reppr/pulses

*/
/* **************************************************************** */
