/*
  battery_control_setup.h
*/

pinMode(BATTERY_LEVEL_CONTROL_PIN, INPUT);
analogRead(BATTERY_LEVEL_CONTROL_PIN);	// without this prior read, it reads always zero :(

show_battery_level();
