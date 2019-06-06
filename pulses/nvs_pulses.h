/*
  nvs_pulses.h
*/
#ifndef NVS_PULSES_H

#include <Preferences.h>

#define USE_NVS		// very first tests, not integrated yet

String nvs_getString(char * key) {
  String s;

  Preferences CONF_nvs;
  CONF_nvs.begin("CONFIG", false);
  s = CONF_nvs.getString("nvs_PRENAME");
  CONF_nvs.end();

  return s;
}

#define NVS_PULSES_H
#endif
