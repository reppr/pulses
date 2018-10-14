/*
  battery_control.h
*/

/*
  *THIS CODE IS NOT USABLE YET*
  ESP32 analogRead() does not behave like i would  expect ;)
  needs some work...
*/

#if ! defined BATTERY_VOLTAGE_CONTROL_PIN
  #define BATTERY_VOLTAGE_CONTROL_PIN	36
#endif

//unsigned int battery_low_level=1474;	// about 11.5 volts on 2.7M/470K voltage divider
unsigned int battery_low_level=1150;	// by accident about 11.5 volts on 2.7M/470K voltage divider
float battery_level_scaling = battery_low_level / 11.5;	// TODO: calibrate

void show_battery_voltage(unsigned int oversampling=6) {
  analogRead(BATTERY_VOLTAGE_CONTROL_PIN);	// without this prior read, it reads always zero :(

  MENU.out(F("\nBattery control pin: "));
  MENU.out(BATTERY_VOLTAGE_CONTROL_PIN);
  MENU.tab();
  //  MENU.outln(analogRead(BATTERY_VOLTAGE_CONTROL_PIN));
  unsigned int sum=0;
  for(int i=0, v=0; i<oversampling; i++) {
    //sum += analogRead(BATTERY_VOLTAGE_CONTROL_PIN);
    v = analogRead(BATTERY_VOLTAGE_CONTROL_PIN);	// TODO: REMOVE: debugging code
    sum += v;
    MENU.outln(v);
    // delay(100);
  }
  sum /= oversampling;

  MENU.out(sum / battery_level_scaling);
  MENU.out(F(" volts\tlow: "));
  MENU.outln(battery_low_level / battery_level_scaling);
}
