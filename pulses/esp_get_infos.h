/*
  esp_get_infos.h

  get some esp32 specific infos:
    get_ESP32_ChipRevision()
    display_esp_versions()
    getMacAddress()
*/

#if ! defined ESP_GET_INFOS_H

#include "esp_chip_info.h"
#include "hal/efuse_hal.h"
void display_esp_versions() {
  MENU.out(F("ESP32 chip revision\t"));
  MENU.out(efuse_hal_get_major_chip_version());
  MENU.out('.');
  MENU.outln(efuse_hal_get_major_chip_version());

  MENU.out(F("ESP IDF version\t\t"));
  MENU.outln(esp_get_idf_version());

#if defined ESP_ARDUINO_VERSION_MAJOR
  MENU.out(F("ESP32 ARDUINO VERSION\t"));
  MENU.out(ESP_ARDUINO_VERSION_MAJOR);
  MENU.out('.');
  MENU.out(ESP_ARDUINO_VERSION_MINOR);
  MENU.out('.');
  MENU.outln(ESP_ARDUINO_VERSION_PATCH);
#endif
} // display_esp_versions()

// see: https://github.com/espressif/arduino-esp32/issues/932
#include "esp_system.h"
#include "esp_mac.h"

String getMacAddress() {
  uint8_t baseMac[6];
  // Get MAC address for WiFi station
  esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
  char baseMacChr[18] = {0};
  sprintf(baseMacChr, "%02X:%02X:%02X:%02X:%02X:%02X", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
  return String(baseMacChr);
} // getMacAddress()

// see: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/heap_debug.html#heap-information
#include "esp_heap_caps.h"
void esp_heap_and_stack_info() {
  MENU.out(F("heap_caps_get_free_size(0)\t\t"));
  MENU.outln(heap_caps_get_free_size(0));

  MENU.out(F("xPortGetFreeHeapSize()\t\t\t"));
  MENU.outln(xPortGetFreeHeapSize());

  MENU.out(F("uxTaskGetStackHighWaterMark(NULL)\t"));
  MENU.outln(uxTaskGetStackHighWaterMark(NULL));
} // esp_heap_and_stack_info()

#define ESP_GET_INFOS_H
#endif
