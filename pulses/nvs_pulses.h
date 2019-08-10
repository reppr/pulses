/*
  nvs_pulses.h
*/

#ifndef NVS_PULSES_H

#include <nvs.h>
#include <Preferences.h>


String nvs_getString(char * key) {
  String s;

  Preferences CONF_nvs;
  CONF_nvs.begin("CONFIG", /* readonly is */ false);
  s = CONF_nvs.getString(key);
  CONF_nvs.end();

  return s;
}


int nvs_free_entries() {
  Preferences CONF_nvs;
  CONF_nvs.begin("CONFIG", /* readonly is */ false);
  int free_entries = CONF_nvs.freeEntries();
  CONF_nvs.end();
  return free_entries;
}


//	void nvs_save_blob(char* key, void* blob, size_t length) { // 2nd version using nvs_getString()
//	// see: https://github.com/espressif/arduino-esp32/blob/master/libraries/Preferences/examples/Prefs2Struct/Prefs2Struct.ino
//
//	  MENU.out(F("nvs_save_blob 2nd version"));
//	  MENU.out(key);
//	  MENU.tab();
//
//	  Preferences CONF_nvs;
//	  if(CONF_nvs.begin("CONFIG", /* readonly is */ false))
//	    MENU.error_ln(F("CONF_nvs.begin()"));
//
//	  size_t did_put_bytes = CONF_nvs.putBytes("CONFIG", blob, length);
//	  MENU.out(did_put_bytes);
//
//	  CONF_nvs.end();
//	}


void nvs_save_blob(char* key, void* blob, size_t length) {	// 1st version using CONF_nvs.putBytes
// see: https://github.com/espressif/arduino-esp32/blob/master/libraries/Preferences/examples/Prefs2Struct/Prefs2Struct.ino

  MENU.out(F("nvs_save_blob "));
  MENU.out(key);
  MENU.tab();

  Preferences CONF_nvs;
  if(CONF_nvs.begin("CONFIG", /* readonly is */ false))
    MENU.error_ln(F("CONF_nvs.begin()"));

  size_t did_put_bytes = CONF_nvs.putBytes("CONFIG", blob, length);
  MENU.out(did_put_bytes);
  MENU.outln(F(" bytes saved"));

  CONF_nvs.end();
}


void nvs_read_blob(char* key, void* new_blob, size_t buffer_size) {	// 1st version using CONF_nvs.getBytes
  MENU.out(F("nvs_read_blob\t"));
  MENU.outln(key);

  Preferences CONF_nvs;
  CONF_nvs.begin("CONFIG", /* readonly is */ false);
  size_t read_len = CONF_nvs.getBytes((const char*) key, new_blob, buffer_size);
  MENU.out(read_len);
  MENU.outln(F(" bytes"));
  if(read_len > buffer_size) {
    MENU.error_ln(F("buffer too small\n"));
    CONF_nvs.end();
    return;
  }

  size_t read_bytes = CONF_nvs.getBytes(key, new_blob, buffer_size);
  MENU.out(read_bytes);
  MENU.outln(F(" bytes read"));

  CONF_nvs.end();
}


esp_err_t configure_hardware_from_nvs() {
  esp_err_t esp_err;
  pulses_hardware_conf_t read_HARDWARE;

  return esp_err;
}


void nvs_clear_all_keys() {
  MENU.out(F(">>> CLEARED ALL KEYS in CONF_nvs <<<  "));
  Preferences CONF_nvs;
  CONF_nvs.begin("CONFIG", /* readonly is */ false);
  int was_free = CONF_nvs.freeEntries();
  CONF_nvs.clear();

  MENU.out(CONF_nvs.freeEntries() - was_free);
  MENU.out(F("\tfree entries now "));
  MENU.outln(CONF_nvs.freeEntries());
  CONF_nvs.end();
  MENU.outln("DADA");
}


#define NVS_PULSES_H
#endif
