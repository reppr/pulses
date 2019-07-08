/*
  esp_now_pulses.h
*/

#include <esp_now.h>
#include <WiFi.h>


#define CHANNEL	3

bool esp_now_pulses_setup() {
  WiFi.mode(WIFI_STA);
  if (esp_now_init() == ESP_OK) {
    return false;	// false == ok
  } // else

  return true;		// ERROR
}

uint8_t macSlaves[][6] = {
  //To send to specific Slaves
  //{0x24, 0x0A, 0xC4, 0x0E, 0x3F, 0xD1}, {0x24, 0x0A, 0xC4, 0x0E, 0x4E, 0xC3}
  //Or to send to all Slaves
  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}
};


int esp_now_pulses_send() {
  uint8_t broadcast[] = {0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF};
  char * text[] = {"Hey You?       "};
  return esp_now_send(broadcast, (uint8_t*) &text, sizeof(text));
}
