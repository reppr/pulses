/*
  peripheral_power_switch.h
*/

#ifndef PERIPHERAL_POWER_SWITCH_H

bool peripheral_power_is_on=false;

void peripheral_power_switching(bool on) {
  if(HARDWARE.periph_power_switch_pin != ILLEGAL8) {
    MENU.out(F("peripheral POWER "));
    peripheral_power_is_on=on;
    if(on)
      MENU.out(F("ON "));
    else
      MENU.out(F("OFF "));

    MENU.out(F("(pin "));
    MENU.out(HARDWARE.periph_power_switch_pin);
    MENU.out(')');
#if defined BATTERY_LEVEL_CONTROL_PIN
    MENU.out(F("\tbattery level (pin "));
    MENU.out(BATTERY_LEVEL_CONTROL_PIN);		// pin *may* be incorrect
    MENU.out(F(") was "));
    MENU.out(analogRead(BATTERY_LEVEL_CONTROL_PIN));	// pin *may* be incorrect	measure *before* switching
#endif
    MENU.ln();

    pinMode(HARDWARE.periph_power_switch_pin, OUTPUT);
    digitalWrite(HARDWARE.periph_power_switch_pin, peripheral_power_is_on);
  }
}

void peripheral_power_switch_ON() {
  peripheral_power_switching(true);
}


void peripheral_power_switch_OFF() {
  peripheral_power_switching(false);
}

#define PERIPHERAL_POWER_SWITCH_H
#endif
