/*
  battery_control.h
*/

#if ! defined BATTERY_LEVEL_CONTROL_PIN
  #define BATTERY_LEVEL_CONTROL_PIN	36
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
unsigned int battery_off_level=1140;	// ~ 11.5V
unsigned int battery_high_level=1402;	// ~ 13.8V

unsigned int read_battery_level(unsigned int oversampling=15) {
  unsigned int data=0;
  analogRead(BATTERY_LEVEL_CONTROL_PIN);	// without this prior read, it reads always zero :(

  for(int i=0; i<oversampling; i++)
    data += analogRead(BATTERY_LEVEL_CONTROL_PIN);

  return data / oversampling;
}

// TODO: enum battery_levels ################
int check_battery_level() {
  unsigned int value = read_battery_level();

#if defined ACCEPT_USB_LEVEL
  if(value < ACCEPT_USB_LEVEL)	// probably running from USB, so let that pass.... *** !!! DANGEROUS !!! ***
    return 0;		// 0 unknown
#endif

  if(value == 4095)	// *FULL* LEVEL, probably pin connected to high to deactivate vu control
    return 5;

  if(value > battery_high_level)
    return 4;		// 4 *OVER* 13.8V

  if(value > battery_low_level)
    return 1;		// 1 looks OK

  // else:
  MENU.out(F("BATTERY "));
  if(value <= battery_off_level) {
    MENU.out(F("OFF "));
    MENU.outln(value);
    return 3;		// 3 SWITCH OFF
  } else {
    MENU.out(F("LOW "));
    MENU.outln(value);
    return 2;		// 2 LOW
  }
}

void show_battery_level() {
  MENU.out(F("battery control pin: "));
  MENU.out(BATTERY_LEVEL_CONTROL_PIN);
  MENU.tab();
  MENU.out(read_battery_level());
  MENU.tab();
  switch (check_battery_level()) {
  case 0:
    MENU.out(F("?? USB ??"));
    break;
  case 1:
    MENU.out(F("GOOD"));
    break;
  case 2:
    MENU.out(F("LOW"));
    break;
  case 3:
    MENU.out(F("*NO POWER*"));
    break;
  case 4:
    MENU.out(F("*HIGH*"));
    break;
  case 5:
    MENU.out(F("*4095*"));	// *FULL* LEVEL, probably pin connected to high to deactivate vu control
    break;
  }

  MENU.out(F("\t(off <= "));
  MENU.out(battery_off_level);
  MENU.out(F("  low < "));
  MENU.out(battery_low_level);
  MENU.out(F("  high >= "));
  MENU.out(battery_high_level);
  MENU.out(F("  HIGH = 4095"));
  MENU.outln(')');
}

bool assure_battery_level() {
  switch(check_battery_level()) {
  case 0:   // probably running from USB, so let that pass....     *** !!! DANGEROUS !!! ***
    return true;						// *** !!! DANGEROUS !!! ***
    break;
  case 1:
    return true;	// OK
    break;
  case 2:		// LOW
    // TODO: start a battery watcher job  ################
    return true;	// user has been warned, but start the same...	################ TODO: real life test
    break;
  case 3:
    return false;
    break;
  case 4:
    return true;
    break;
  }
}

void battery_control_setup() {
  pinMode(BATTERY_LEVEL_CONTROL_PIN, INPUT);
  analogRead(BATTERY_LEVEL_CONTROL_PIN);	// without this prior read, it reads always zero :(

  show_battery_level();
}
