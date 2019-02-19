/*
  peripheral_power_switch.h
*/

#ifndef PERIPHERAL_POWER_SWITCH_H

bool peripheral_power_on=false;

void peripheral_power_switch_ON() {
  pinMode(PERIPHERAL_POWER_SWITCH_PIN, OUTPUT);
  digitalWrite(PERIPHERAL_POWER_SWITCH_PIN, HIGH);
  peripheral_power_on=true;
}

void peripheral_power_switch_OFF() {
  pinMode(PERIPHERAL_POWER_SWITCH_PIN, OUTPUT);
  digitalWrite(PERIPHERAL_POWER_SWITCH_PIN, LOW);
  peripheral_power_on=false;
}

#define PERIPHERAL_POWER_SWITCH_H
#endif
