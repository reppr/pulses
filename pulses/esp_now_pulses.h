/*
  esp_now_pulses.h

  see:	https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/network/esp_now.html#
	https://www.instructables.com/id/ESP32-With-ESP-Now-Protocol/
	https://hackaday.io/project/164132-hello-world-for-esp-now/log/160570-esp-now-introduction
*/

#include <esp_now.h>
#include <WiFi.h>


#define ESP_NOW_CHANNEL	3

//Callback function that gives us feedback about the sent data
esp_err_t esp_now_pulses_broadcast();	// pre declaration
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  //Copies the receiver Mac Address to a string
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
	   mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

  Serial.print("sent to: ");
  Serial.print(macStr);

  Serial.print("\tstatus: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "ok" : "Fail");
}

uint8_t broadcast_mac[] = {0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF};
esp_err_t esp_now_pulses_broadcast() {
  char * text[] = {"Hey You?       "};	// TODO: REMOVE: justatest
  esp_now_register_send_cb(OnDataSent);
  return esp_now_send(broadcast_mac, (uint8_t*) &text, sizeof(text));
}

//Callback function that tells us when data from Master is received
extern void start_musicBox();	// TODO: do not do that from here, JUSTATEST
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  char macStr[18];
  //Copies the sender Mac Address to a string
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
	   mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print("\nESP-NOW from: ");
  Serial.println(macStr);

  start_musicBox();	// TODO: do not do that from here, JUSTATEST
}


esp_now_peer_info_t peer_info;

esp_err_t esp_now_pulses_setup() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();	// just in case

  esp_err_t status;
  status = esp_now_init();
  if(status != ESP_OK)
    return status;

  status = esp_now_register_recv_cb(OnDataRecv);
  if(status != ESP_OK)
    return status;

  peer_info.channel = ESP_NOW_CHANNEL;
  memcpy(peer_info.peer_addr, broadcast_mac, 6);
  peer_info.ifidx = ESP_IF_WIFI_STA;
  peer_info.encrypt = false;
  status = esp_now_add_peer(&peer_info);

  return status;
}

// see: https://hackaday.io/project/164132-hello-world-for-esp-now/log/160570-esp-now-introduction
typedef struct __attribute__((packed)) esp_now_msg_t
{
  uint32_t version;
  uint32_t type;
  uint8_t size;
  // ...
} esp_now_msg_t;


// TODO: unused ???
uint8_t macSlaves[][6] = {
  //To send to specific Slaves
  //{0x24, 0x0A, 0xC4, 0x0E, 0x3F, 0xD1}, {0x24, 0x0A, 0xC4, 0x0E, 0x4E, 0xC3}
  //Or to send to all Slaves
  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}
};
