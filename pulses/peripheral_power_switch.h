/*
  peripheral_power_switch.h
*/

#ifndef PERIPHERAL_POWER_SWITCH_H

bool peripheral_power_on=false;

void peripheral_power_switch_ON() {
  if(HARDWARE.periph_power_switch_pin != ILLEGAL8) {
    pinMode(HARDWARE.periph_power_switch_pin, OUTPUT);
    digitalWrite(HARDWARE.periph_power_switch_pin, HIGH);
    peripheral_power_on=true;
  }
}

void peripheral_power_switch_OFF() {
  if(HARDWARE.periph_power_switch_pin != ILLEGAL8) {
    pinMode(HARDWARE.periph_power_switch_pin, OUTPUT);
    digitalWrite(HARDWARE.periph_power_switch_pin, LOW);
    peripheral_power_on=false;
  }
}

#define PERIPHERAL_POWER_SWITCH_H
#endif
