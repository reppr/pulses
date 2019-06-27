/*
  battery_control.h

  #define BATTERY_LEVEL_CONTROL_PIN 36		triggers loading
*/


#if ! defined BATTERY_CONTROL_H

#if ! defined BATTERY_LEVEL_CONTROL_PIN		// TODO: must be set somewhere appropriate
  #error TODO: BATTERY_LEVEL_CONTROL_PIN must be set somewhere appropriate
//  #define BATTERY_LEVEL_CONTROL_PIN	36	// TODO: must be set somewhere appropriate
#endif

/*
  #define ACCEPT_USB_LEVEL	150

  defaults accepts very low readings as usb level
  it assumes running from USB, so accepts it	possibly *** !!! DANGEROUS !!! ***
  UNDEFINE to make sure it will *NOT* drain a *ALREADY VERY BAD* battery...

  in *normal* use it *will* recognise a much better level as battery_low_level and save power
		 or then switch off below battery_off_level
  so probably no need to change that #define
*/
#define ACCEPT_USB_LEVEL	200

const unsigned int battery_12V_level=1200;	// calibrated reading for 12.00V
//float battery_level_scaling = battery_12V_level / 12.00 ;	// NO, it's too far from being linear...

unsigned int battery_low_level=1175;	// ~ 11.8V
//unsigned int battery_off_level=1140;	// ~ 11.5V
unsigned int battery_off_level=1130;	// ~ ????? justaTEST ################
unsigned int battery_high_level=1402;	// ~ 13.8V

unsigned int read_battery_level(unsigned int oversampling=15) {
  unsigned int data=0;
  analogRead(BATTERY_LEVEL_CONTROL_PIN);	// without this prior read, it reads always zero :(

  for(int i=0; i<oversampling; i++)
    data += analogRead(BATTERY_LEVEL_CONTROL_PIN);

  return data / oversampling;
}

enum battery_levels {unknown,	// probably USB
		     too_LOW,	// automatic installations should better switch off
		     MEDIOCRE,	// low, but no danger for battery
		     GOOD,	// good working condition
		     CHARGE,	// above about 13.8V (if calibrated accordingly)
		     OVER};	// reads max value 4095 probably pin connected to 3.3V to deactivate vu control

battery_levels check_battery_level() {
  unsigned int value = read_battery_level();

  if(value < ACCEPT_USB_LEVEL)	// probably running from USB, on automatic installations this is *** !!! DANGEROUS !!! ***
    return unknown;		// 0 unknown

  if(value == 4095)	// *FULL* LEVEL, probably pin connected to high to deactivate vu control
    return OVER;

  if(value > battery_high_level)
    return CHARGE;		// 4 *OVER* 13.8V

  if(value > battery_low_level)
    return GOOD;

  // else:
  MENU.out(F("BATTERY "));
  if(value <= battery_off_level) {
    MENU.out(F("> OFF !!! "));
    MENU.outln(value);
    return too_LOW;		// *BATTERY LOW*	automatic installations better switch off...
  } else {
    MENU.out(F("LOW "));
    MENU.outln(value);
    return MEDIOCRE;
  }
}

void show_battery_level() {
  MENU.out(F("battery control pin: "));
  MENU.out(BATTERY_LEVEL_CONTROL_PIN);
  MENU.tab();
  MENU.out(read_battery_level());
  MENU.tab();
  switch (check_battery_level()) {
  case unknown:
    MENU.out(F("?USB?"));
    break;
  case GOOD:
    MENU.out(F("GOOD"));
    break;
  case MEDIOCRE:
    MENU.out(F("LOW"));
    break;
  case too_LOW:
    MENU.out(F("*BATTERY LOW*"));
    break;
  case CHARGE:
    MENU.out(F("*HIGH*"));
    break;
  case OVER:
    MENU.out(F("*4095*"));	// *FULL* LEVEL, probably pin connected to high to deactivate vu control
    break;
  }

  if (MENU.verbosity > VERBOSITY_SOME) {
    MENU.out(F("\t(off <= "));
    MENU.out(battery_off_level);
    MENU.out(F("  low < "));
    MENU.out(battery_low_level);
    MENU.out(F("  high >= "));
    MENU.out(battery_high_level);
    MENU.out(F("  HIGH = 4095"));
    MENU.outln(')');
  } else
    MENU.ln();
}

bool assure_battery_level() {
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
  case GOOD:
  case CHARGE:
  case OVER:
    return true;
    break;
  }
}

void battery_control_setup() {
  pinMode(BATTERY_LEVEL_CONTROL_PIN, INPUT);
  analogRead(BATTERY_LEVEL_CONTROL_PIN);	// without this prior read, it reads always zero :(

  show_battery_level();
}


#define BATTERY_CONTROL_H
#endif // BATTERY_CONTROL_H
