/*
  esp_get_infos.h

  get some esp32 specific infos:
    get_ESP32_ChipRevision()
    display_esp_versions()
    getMacAddress()
*/

#if ! defined ESP_GET_INFOS_H

// ESP32  int getChipRevision()	see: Andreas Spiess ESP32_Version.ino, register names updated
#include "soc/efuse_reg.h"
int get_ESP32_ChipRevision() {
  return (REG_READ(EFUSE_BLK0_RDATA3_REG) >> (EFUSE_RD_CHIP_VER_REV1_S)&&EFUSE_RD_CHIP_VER_REV1_V) ;
}

// #include "esp_arduino_version.h"	// OBSOLETE: some git versions needed this
void display_esp_versions() {
  MENU.out(F("ESP32 chip revision\t"));
  MENU.outln(get_ESP32_ChipRevision());

  MENU.out(F("ESP IDF version\t\t"));
  MENU.outln(esp_get_idf_version());

  MENU.out(F("ESP32 ARDUINO version\t"));
#if defined ESP_ARDUINO_VERSION_MAJOR
  MENU.out(ESP_ARDUINO_VERSION_MAJOR);
  MENU.out('.');
  MENU.out(ESP_ARDUINO_VERSION_MINOR);
  MENU.out('.');
  MENU.outln(ESP_ARDUINO_VERSION_PATCH);
#else
  MENU.outln(F("unknown (pre v2)"));
#endif
} // display_esp_versions()

// see: https://github.com/espressif/arduino-esp32/issues/932
#include "esp_system.h"
String getMacAddress() {
  uint8_t baseMac[6];
  // Get MAC address for WiFi station
  esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
  char baseMacChr[18] = {0};
  sprintf(baseMacChr, "%02X:%02X:%02X:%02X:%02X:%02X", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
  return String(baseMacChr);
}

#define ESP_GET_INFOS_H
#endif
