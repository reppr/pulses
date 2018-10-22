/*
  peripheral_power_switch.h
*/

#ifndef PERIPHERAL_POWER_SWITCH_H

void peripheral_power_switch_ON() {
  pinMode(PERIPHERAL_POWER_SWITCH_PIN, OUTPUT);
  digitalWrite(PERIPHERAL_POWER_SWITCH_PIN, HIGH);
}

void peripheral_power_switch_OFF() {
  pinMode(PERIPHERAL_POWER_SWITCH_PIN, OUTPUT);
  digitalWrite(PERIPHERAL_POWER_SWITCH_PIN, LOW);
}

#define PERIPHERAL_POWER_SWITCH_H
#endif
