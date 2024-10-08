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

#if ! defined GET_6_BYTES_MAC_IS_COMPILED
  void get_6_bytes_mac(uint8_t* c6) {
    String mac = WiFi.macAddress();	// Get MAC address for WiFi station
    const char * c = mac.c_str();
    for(int i=0; i<6; i++) {		// 6 bytes
      c6[i] = *c++;
    }
  } // get_6_bytes_mac()
#define  GET_6_BYTES_MAC_IS_COMPILED
#endif

String getMacAddress() {
  uint8_t baseMac[6];
  get_6_bytes_mac(baseMac);	// Get MAC address for WiFi station
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
