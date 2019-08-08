/*
  nvs_pulses.h

  do not use MENU from here
*/
#ifndef NVS_PULSES_H

#include <Preferences.h>

String nvs_getString(char * key) {
  String s;

  Preferences CONF_nvs;
  CONF_nvs.begin("CONFIG", false);
  s = CONF_nvs.getString(key);
  CONF_nvs.end();

  return s;
}

int nvs_free_entries() {
  Preferences CONF_nvs;
  CONF_nvs.begin("CONFIG", false);
  int free_entries = CONF_nvs.freeEntries();
  CONF_nvs.end();
  return free_entries;
}

void nvs_save_hardware() {
  //  HARDWARE_conf
}

esp_err_t configure_hardware_from_nvs() {
  esp_err_t esp_err;
  pulses_hardware_conf_t read_config;

  return esp_err;
}

#define NVS_PULSES_H
#endif
