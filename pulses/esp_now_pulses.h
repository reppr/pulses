/*
  esp_now_pulses.h

  see:	https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/network/esp_now.html#
	https://www.instructables.com/id/ESP32-With-ESP-Now-Protocol/
	https://hackaday.io/project/164132-hello-world-for-esp-now/log/160570-esp-now-introduction
*/

#include <esp_now.h>
#include <WiFi.h>


#define ESP_NOW_CHANNEL	3

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  MENU.outln("DADA OnDataSent");
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

// pulses_esp_now_t	as type of pulses esp_now messages:
// see: https://hackaday.io/project/164132-hello-world-for-esp-now/log/160570-esp-now-introduction
typedef struct __attribute__((packed)) pulses_esp_now_t {
  icode_t meaning;
//  uint8_t data_bytes;
//  uint8_t flags1_UNUSED;
//  uint8_t flags2_UNUSED;
//  uint8_t flags3_UNUSED;
  // data
} pulses_esp_now_t;


static void send_msg(pulses_esp_now_t * msg) {
  MENU.outln("DADA send_msg");
  // Pack
  uint16_t packet_size = sizeof(pulses_esp_now_t);
  uint8_t msg_data[packet_size];
  memcpy(&msg_data[0], msg, sizeof(pulses_esp_now_t));

  esp_err_t status = esp_now_send(broadcast_mac, msg_data, packet_size);
  if (ESP_OK != status) {
    MENU.out(F("send_msg "));
    MENU.outln(esp_err_to_name(status));
  }
}

esp_err_t esp_now_pulses_broadcast(icode_t code) {
  MENU.outln("DADA esp_now_pulses_broadcast");
  int data_len = 4;	// default for code==meaning
  int * ip;

  switch(code) {
  case PRES:
    break;
  default:
    MENU.error_ln(F("data_len, unknown code"));	// TODO: ##################################
  }

  pulses_esp_now_t * msg_buf = (pulses_esp_now_t *) malloc(data_len + 0);	// TODO: determine right size
  //  pulses_esp_now_t message;

  switch(code) {
  case PRES:
    extern short preset;
    ip = (int *) &msg_buf;
    *ip = preset;
MENU.outln("DADA sent preset "); MENU.outln(preset);
    break;
  default:
    MENU.error_ln(F("not implemented code"));	// TODO: ##################################
  }


  esp_now_register_send_cb(OnDataSent);
  return esp_now_send(broadcast_mac, (uint8_t*) msg_buf, sizeof(msg_buf));
}

static void esp_now_pulses_reaction(pulses_esp_now_t msg, uint8_t *data) {
  MENU.outln("DADA esp_now_pulses_reaction");
  switch (msg.meaning) {
  case PRES:
    MENU.outln(F("PRES "));
    break;

  default:
    MENU.out_Error_();
    MENU.out(F("unknown reaction code\t"));
    MENU.outln(msg.meaning);
    MENU.outln((int) *data);
  } // switch meaning
}

static void msg_recv_cb(const uint8_t *mac_addr, const uint8_t *data, int len) {
  MENU.outln("DADA msg_recv_cb");

  if (len == sizeof(pulses_esp_now_t)) {
    char macStr[18];	// buffer for sender MAC string representation

    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
	     mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

    MENU.out("\nESP-NOW from: ");
    MENU.out(macStr);
    MENU.tab();
    MENU.outln(len);

    pulses_esp_now_t * msg_buf = (pulses_esp_now_t *) malloc(len + 16);	// TODO: determine right size
    memcpy(msg_buf, data, len);

//    MENU.out(F("esp_now_pulses_reaction "));
//    MENU.outln((*msg_buf).meaning);
    esp_now_pulses_reaction(*msg_buf, (uint8_t*) data);

    free(msg_buf);
  } // if(len)
}

//Callback function that tells us when data from Master is received
static void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  MENU.outln("DADA OnDataRecv");
  char macStr[18];	// senders MAC string representation
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
	   mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  MENU.out("\nESP-NOW from: ");
  MENU.out(macStr);
  MENU.out(F("\t\tbytes "));
  MENU.outln(data_len);
//  MENU.outln((int) *data);
  pulses_esp_now_t * msg_buf = (pulses_esp_now_t *) malloc(data_len + 16);	// TODO: determine right size
  memcpy(msg_buf, data, data_len);
  esp_now_pulses_reaction(*msg_buf, (uint8_t*) data);

  free(msg_buf);
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
