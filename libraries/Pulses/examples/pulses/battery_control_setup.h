/*
  battery_control_setup.h
*/

pinMode(BATTERY_VOLTAGE_CONTROL_PIN, INPUT);

show_battery_voltage();

/*
  *THIS CODE IS NOT USABLE YET*
  ESP32 analogRead() does not behave like i would  expect ;)
  needs some work...
*/
