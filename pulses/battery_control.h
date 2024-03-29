/*
  battery_control.h

  #define USE_BATTERY_LEVEL_CONTROL	// triggers loading
  #define BATTERY_LEVEL_CONTROL_PIN 35	// NEW default, (was: 36)		see: MUSICBOX2_PIN_MAPPING
*/


#if ! defined BATTERY_CONTROL_H

#if ! defined BATTERY_LEVEL_CONTROL_PIN		// TODO: must be set somewhere appropriate
  #warning TODO: BATTERY_LEVEL_CONTROL_PIN should be set somewhere appropriate	set to 35
  #define BATTERY_LEVEL_CONTROL_PIN	35	// NEW default, (was: 36)	see: MUSICBOX2_PIN_MAPPING
#endif

/*
  #define ACCEPT_USB_LEVEL	400

  defaults accepts very low readings as usb level
  it assumes running from USB, so accepts it	possibly *** !!! DANGEROUS !!! ***
  UNDEFINE to make sure it will *NOT* drain a *ALREADY VERY BAD* battery...

  in *normal* use it *will* recognise a much better level as battery_low_level and save power
		 or then switch off below battery_off_level
  so probably no need to change that #define
*/
#if ! defined ACCEPT_USB_LEVEL
  #define ACCEPT_USB_LEVEL	400	// level below that are accepted assuming USB power
#endif

#if defined BATTERY_LEVELS_DOUBLED	// *NEW*	12V ~ 2400
typedef struct {
  uint8_t version=1;							// % 0
  uint8_t reserved1=0;
  uint8_t reserved2=0;
  uint8_t type=12;		// 12 = 12V lead acid battery, static charge on 13.8
				// 11 = 12V lead acid battery, static charge on 13.65
  uint16_t level_12V=2400;	// calibrated to read 1200 on 12.0V	// % 4
  uint16_t low_level=2360;	// ~ 11.8V
  uint16_t off_level=2340;	// ~ 11.7V	TODO: TEST&TRIM		// %8
  uint16_t static_13V8_level=2760;	// ~ 13.8V
  uint16_t static_13V65_level=0;	// ~ 13.65V			// %12		// TODO?: not implemented
  uint16_t accepted_USB_level=ACCEPT_USB_LEVEL;	// level below that are accepted assuming USB power
  uint16_t high_level=2480;	// ~12.4V	TODO: test&trimm	// %16
  uint16_t over_level=2780;	// ~12.9V	TODO: test&trimm
} battery_levels_conf_t;

#else	// keep old battery levels for some old instruments
  typedef struct {
    uint8_t version=1;							// % 0
    uint8_t reserved1=0;
    uint8_t reserved2=0;
    uint8_t type=12;		// 12 = 12V lead acid battery, static charge on 13.8
				// 11 = 12V lead acid battery, static charge on 13.65
    uint16_t level_12V=1200;	// calibrated to read 1200 on 12.0V	// % 4
    uint16_t low_level=1181;	// ~ 11.8V
    uint16_t off_level=1130;	// ~ 11.7V	TODO: TEST&TRIM		// %8
    uint16_t static_13V8_level=1402;	// ~ 13.8V
    uint16_t static_13V65_level=0;	// ~ 13.65V			// %12		// TODO?: not implemented
    uint16_t accepted_USB_level=ACCEPT_USB_LEVEL;	// level below that are accepted assuming USB power
    uint16_t high_level=1234;	// ~12.4V	TODO: test&trimm	// %16
    uint16_t over_level=1404;
  } battery_levels_conf_t;
#endif // Battery levels

battery_levels_conf_t BATTERY;

void battery_conf_UI_display(bool show_menu_keys=true) { // if show_menu_keys==false just show state and configuration, no UI hints
  MENU.out(BATTERY.type);
  MENU.out(F("V BATTERY levels (version "));
  MENU.out(BATTERY.version);
  MENU.out(F(")\t"));
  if(show_menu_keys)
    MENU.out(F("'B','B?'=info\t"));
  extern void show_battery_level();
  show_battery_level();

  if(show_menu_keys) {
    MENU.outln(F(" set voltage level on pin or give numeric raw value:"));
    MENU.out(F("'BO'="));
  }
  MENU.out(F("off_level=")); MENU.out(BATTERY.off_level);
  MENU.space(3);

  if(show_menu_keys)
    MENU.out(F("'BL'="));
  MENU.out(F("low_level=")); MENU.out(BATTERY.low_level);
  MENU.space(3);

  if(show_menu_keys)
    MENU.out(F("'BH'="));
  MENU.out(F("high_level=")); MENU.outln(BATTERY.high_level);


  if(show_menu_keys)
    MENU.out(F("'BC'=calibrate "));
  MENU.out(F("level_12V=")); MENU.out(BATTERY.level_12V);
  MENU.space(3);

  if(show_menu_keys)
    MENU.out(F("'BZ'="));
  MENU.out(F("static_13V8="));  MENU.out(BATTERY.static_13V8_level);
  MENU.space(3);

//if(show_menu_keys)
//  MENU.out(F("'BY'="));
//MENU.space(3);
//MENU.out(F("static_13V65=")); MENU.out(BATTERY.static_13V65_level);	// TODO?: not implemented
//MENU.space(3);

  if(show_menu_keys)
    MENU.out(F("'BX'="));
  MENU.out(F("over_level="));  MENU.out(BATTERY.over_level);
  MENU.space(3);

  if(show_menu_keys)
    MENU.out(F("'BU'="));
  MENU.out(F("accepted \"USB\"=")); MENU.out(BATTERY.accepted_USB_level);

#if defined USE_NVS
  if(show_menu_keys) {
    MENU.space(3);
    MENU.out(F("'BS'=nvs save"));
  }
#endif // USE_NVS
  MENU.ln();
} // battery_conf_UI_display(bool show_menu_keys=true)

unsigned int read_battery_level(unsigned int oversampling=15) {
  if(HARDWARE.battery_level_control_pin == ILLEGAL8)
    return 0;	// dummy

  unsigned int data=0;
  analogRead(HARDWARE.battery_level_control_pin);	// without this prior read, it reads always zero :(

  for(int i=0; i<oversampling; i++)
    data += analogRead(HARDWARE.battery_level_control_pin);

  return data / oversampling;
} // read_battery_level()

enum battery_levels {unknown,	// probably USB
		     too_LOW,	// automatic installations should better switch off
		     LOW_lv,
		     MEDIOCRE,	// low, but no danger for battery
		     GOOD,	// good working condition
		     HIGH_lv,	// very good
		     CHARGE,	// above about 13.8V (if calibrated accordingly)
		     OVER};	// >= 13.88	or max value 4095 probably pin connected to 3.3V to deactivate vu control

battery_levels check_battery_level() {
  unsigned int value = read_battery_level();

  if(value < BATTERY.accepted_USB_level)	// probably running from USB, on automatic installations this is *** !!! DANGEROUS !!! ***
    return unknown;				// 0 unknown

  if(value >= BATTERY.over_level)	// over ~13.88V  4095==*FULL* LEVEL, probably pin connected to high to deactivate vu control
    return OVER;

  if(value >= BATTERY.static_13V8_level)
    return CHARGE;			// *OVER* 13.8V

  if(value > BATTERY.high_level)
    return HIGH_lv;

  if(value >= BATTERY.level_12V)
    return GOOD;

  if(value > BATTERY.low_level)
    return MEDIOCRE;

  // else:
  MENU.out(F("BATTERY "));
  if(value <= BATTERY.off_level) {
    MENU.out(F("> OFF !!! "));
    MENU.outln(value);
    return too_LOW;		// *BATTERY LOW*	automatic installations better switch off...
  } else {
    MENU.out(F("LOW "));
    MENU.outln(value);
    return LOW_lv;
  }
} // check_battery_level()

void show_battery_level() {
  MENU.out(F("battery control pin:"));
  MENU.out(HARDWARE.battery_level_control_pin);
  MENU.out(F(" level="));
  MENU.out(read_battery_level());
  MENU.space(3);
  switch (check_battery_level()) {
  case unknown:
    MENU.outln(F("unknown, USB"));
    break;
  case too_LOW:
    MENU.outln(F("*BATTERY LOW*"));
    break;
  case LOW_lv:
    MENU.outln(F("low"));
    break;
  case MEDIOCRE:
    MENU.outln(F("LOW"));
    break;
  case GOOD:
    MENU.outln(F("GOOD"));
    break;
  case HIGH_lv:
    MENU.outln(F("HIGH"));
    break;
  case CHARGE:
    MENU.outln(F("*CHARGE*"));
    break;
  case OVER:
    MENU.outln(F("*OVER*"));	// >~13.88V	or *FULL* 4095 LEVEL, probably pin connected to high to deactivate vu control
    break;
  }
} // show_battery_level()

bool assure_battery_level() {
  if(HARDWARE.battery_level_control_pin == ILLEGAL8)	// battery level is *not* tested
    return true;

  switch(check_battery_level()) {
  case unknown:   // probably running from USB, so let that pass....     *** !!! DANGEROUS !!! ***
#if defined ACCEPT_USB_LEVEL	// probably running from USB, so let that pass...
    return true;		// *** !!! DANGEROUS !!! *** do better *not* use on automatic installations
#else
    return false;
#endif
    break;
  case too_LOW:
    return false;	// automatic installations should switch off
    break;
  case MEDIOCRE:	// LOW
    // TODO: start a battery watcher job  ################
    return true;	// user has been warned, but start the same...	################ TODO: real life test
    break;
//  case GOOD:
//  case CHARGE:
//  case OVER:
  default:
    return true;
    break;
  }
} // assure_battery_level()

uint16_t input_battery_level() {	// read from MENU.numeric_input() if exists, *or* take level on the pin
  if(MENU.is_numeric())
    return (uint16_t) MENU.numeric_input(/*dummy*/ 0);	// from MENU
  // else (no numeric MENU input, read voltage on pin)
  return (uint16_t) read_battery_level(1000 /*samples*/);	// read pin voltage
} // input_battery_level()

bool battery_UI_reaction() {	// 'B' was already received
  switch (MENU.get_next()) {
  case EOF8: // bare 'B'	battery_conf_UI_display()
  case '?':	  // 'B?'	battery_conf_UI_display()
    battery_conf_UI_display(true);
    break;

  case 'O':	// 'BO'   BATTERY.off_level
    BATTERY.off_level = input_battery_level();		// numeric MENU input or read voltage on pin
    MENU.out(F("off_level "));
    MENU.outln(BATTERY.off_level);
    break;

  case 'Z':	// 'BZ' 13.8V	BATTERY.static_13V8_level
    BATTERY.static_13V8_level = input_battery_level();	// numeric MENU input or read voltage on pin
    MENU.out(F("static_13V8 "));
    MENU.outln(BATTERY.static_13V8_level);
    break;

  case 'X':	// 'BX' 13.85V	BATTERY.over_level
    BATTERY.over_level = input_battery_level();	// numeric MENU input or read voltage on pin
    MENU.out(F("over_level "));
    MENU.outln(BATTERY.over_level);
    break;

  case 'Y':	// 'BY' 13.65V	BATTERY.static_13V65_level	// TODO?: NOT IMPLEMENTED
    BATTERY.static_13V65_level = input_battery_level();	// numeric MENU input or read voltage on pin
    MENU.out(F("static_13V65 "));
    MENU.outln(BATTERY.static_13V65_level);
    break;

  case 'U':	// 'BU' accepted USB level BATTERY.accepted_USB_level
    BATTERY.accepted_USB_level = MENU.numeric_input(ACCEPT_USB_LEVEL);  // numeric MENU input *only*
    MENU.out(F("accepted_USB_level "));
    MENU.outln(BATTERY.accepted_USB_level);
    break;

  case 'C':	// 'BC' Calibrate 12V	BATTERY.level_12V
    BATTERY.level_12V = input_battery_level();		// numeric MENU input or read voltage on pin
    MENU.out(F("level_12V "));
    MENU.outln(BATTERY.level_12V);
    break;

  case 'H':	// 'BL' high	BATTERY.high_level
    BATTERY.high_level = input_battery_level();		// numeric MENU input or read voltage on pin
    MENU.out(F("high_level "));
    MENU.outln(BATTERY.high_level);
    break;

  case 'L':	// 'BL' low	BATTERY.low_level
    BATTERY.low_level = input_battery_level();		// numeric MENU input or read voltage on pin
    MENU.out(F("low_level "));
    MENU.outln(BATTERY.low_level);
    break;

#if defined USE_NVS
  case 'S':	// 'BS' nvs save as 'BATTERY_nvs'
    extern bool /*error*/ nvs_save_blob(const char* key, void* new_blob, size_t buffer_size);
    nvs_save_blob("BATTERY_nvs", &BATTERY, sizeof(battery_levels_conf_t));
    battery_conf_UI_display(false);
    break;
#endif // USE_NVS

  default:	// unknown 'B<x>'
    MENU.restore_input_token();
    battery_conf_UI_display(true);
    MENU.ln();
  }
  return true;	// always true, as 'B' was already seen
} // battery_UI_reaction()


void battery_control_setup() {
  if(HARDWARE.battery_level_control_pin == ILLEGAL8) {	// no pin configured, battery level ignored
    MENU.outln(F("ignoring BATTERY LEVEL"));
    return;
  }

  pinMode(HARDWARE.battery_level_control_pin, INPUT);
  analogRead(HARDWARE.battery_level_control_pin);	// without this prior read, it reads always zero :(

  show_battery_level();
}


#define BATTERY_CONTROL_H
#endif // BATTERY_CONTROL_H
