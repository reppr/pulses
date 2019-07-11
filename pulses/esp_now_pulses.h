/*
  esp_now_pulses.h

  see:	https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/network/esp_now.html#
	https://www.instructables.com/id/ESP32-With-ESP-Now-Protocol/
	https://hackaday.io/project/164132-hello-world-for-esp-now/log/160570-esp-now-introduction
*/

#include <esp_now.h>
#include <WiFi.h>


#define ESP_NOW_CHANNEL	3


// buffers for data to send or receive

uint8_t esp_now_send_data[ESP_NOW_MAX_DATA_LEN] = {0};
uint8_t esp_now_data_to_send_cnt=0;	// bytes

uint8_t esp_now_received_data[ESP_NOW_MAX_DATA_LEN] = {0};
uint8_t esp_now_data_received_cnt=0;	// bytes
uint8_t esp_now_data_received_read=0;	// bytes

void esp_now_store_to_send(int di) {			// (int) version
  if(esp_now_data_to_send_cnt > ESP_NOW_MAX_DATA_LEN) {
    MENU.error_ln(F("send buffer full"));
    return;
  }

  int* ip = (int*) esp_now_send_data + esp_now_data_to_send_cnt;
  *ip = di;

  esp_now_data_to_send_cnt += sizeof(int);
}

void esp_now_store_to_send(short ds) {			// (short version)
  if(esp_now_data_to_send_cnt > ESP_NOW_MAX_DATA_LEN) {
    MENU.error_ln(F("send buffer full"));
  }

  short* sp = (short*) esp_now_send_data + esp_now_data_to_send_cnt;
  *sp = ds;

  esp_now_data_to_send_cnt += sizeof(short);
}

void esp_now_read_received(int* pi) {			// (int) version
  // SIZE ################
  int* ip = (int*) esp_now_received_data + esp_now_data_received_read;
  *pi = *ip;

  esp_now_data_received_read += sizeof(int);
}

void esp_now_read_received(short* ps) {			// (short version)
  // SIZE ################
  short* sp = (short*)  esp_now_received_data + esp_now_data_received_read;
  MENU.out("DADA sees ");
  MENU.outln(*sp);
  *ps = *sp;

  esp_now_data_received_read += sizeof(short);
}

uint8_t broadcast_mac[] = {0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF};

void esp_now_data_sent_callback(const uint8_t *mac_addr, esp_now_send_status_t status) {
  if(MENU.maybe_display_more(VERBOSITY_LOWEST)) {	// display MAC?
    char macStr[18];	// receiver MAC as string
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
	     mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    MENU.out(F("sent to: "));
    MENU.out(macStr);
    MENU.tab();
  }

  if(status == ESP_NOW_SEND_SUCCESS) {
    esp_now_data_to_send_cnt=0;
    if(MENU.maybe_display_more(VERBOSITY_LOWEST))
      MENU.outln(F("ok"));
  } else {
    if(MENU.maybe_display_more(VERBOSITY_LOWEST))
      MENU.error_ln(esp_err_to_name(status));
  }
}

void esp_now_send(icode_t meaning) {
  esp_now_store_to_send(meaning);

  switch(meaning) {
  case PRES:
    extern short preset;
    esp_now_store_to_send(preset);
    if(MENU.maybe_display_more(VERBOSITY_LOWEST)) {
      MENU.out(F("sent: "));
      MENU.out(F("PRES "));
      MENU.out(preset);
      MENU.space(2);
    }
    break;

  default:
    MENU.error_ln(F("unknown meaning"));
    return;
  } // switch(meaning)

  if(MENU.maybe_display_more(VERBOSITY_LOWEST)) {
    MENU.out(esp_now_data_to_send_cnt);
    MENU.out(F(" bytes\t\t"));
  }

  esp_err_t status = esp_now_register_send_cb(esp_now_data_sent_callback);
  if(status != ESP_OK) {
    MENU.error_ln(esp_err_to_name(status));
    // return;	// maybe, maybe not
  }

  status = esp_now_send(broadcast_mac, esp_now_send_data, esp_now_data_to_send_cnt);
  if(MENU.maybe_display_more(VERBOSITY_LOWEST)) {
    if (status == ESP_OK)
      MENU.outln(F("ok"));
    else
      MENU.error_ln(esp_err_to_name(status));
  }
}

static void esp_now_pulses_reaction() {
  MENU.outln("DADA esp_now_pulses_reaction()");
  if(MENU.maybe_display_more(VERBOSITY_LOWEST))
    MENU.out(F("received: "));
  int meaning;
  esp_now_read_received(&meaning);	// (int) version
  switch (meaning) {
  case PRES:
    {
      MENU.out(F("PRES "));
      short preset;
      esp_now_read_received(&preset);	// (short) version
      MENU.outln(preset);
      //     short * preset_p = (short *) esp_now_received_data;
      //      short * preset_p = (short *) &esp_now_received_data;
      //      MENU.outln((short) *preset_p);
    }
    break;

  default:
    MENU.out_Error_();
    MENU.out(F("unknown meaning\t"));
    MENU.outln(meaning);
  } // switch meaning
}


static void pulses_data_received_callback(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  MENU.outln("DADA pulses_data_received_callback()");
  esp_now_data_received_read = 0;

  if(MENU.maybe_display_more(VERBOSITY_LOWEST)) {
    char macStr[18];	// senders MAC string representation
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
	     mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    MENU.out("\nESP-NOW from: ");
    MENU.out(macStr);
    MENU.out(F("\t\tbytes "));
    MENU.outln(data_len);
  }

  esp_now_data_received_cnt = data_len;
  memcpy(esp_now_received_data, data, data_len);
  esp_now_pulses_reaction();
}

esp_now_peer_info_t peer_info;

esp_err_t esp_now_pulses_setup() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();	// just in case

  esp_err_t status;
  status = esp_now_init();
  if(status != ESP_OK)
    return status;

  status = esp_now_register_recv_cb(pulses_data_received_callback);
  if(status != ESP_OK)
    return status;

  peer_info.channel = ESP_NOW_CHANNEL;
  memcpy(peer_info.peer_addr, broadcast_mac, 6);
  peer_info.ifidx = ESP_IF_WIFI_STA;
  peer_info.encrypt = false;
  status = esp_now_add_peer(&peer_info);

  return status;
}

// DADA

//	// pulses_esp_now_t	as type of pulses esp_now messages:
//	// see: https://hackaday.io/project/164132-hello-world-for-esp-now/log/160570-esp-now-introduction
//	typedef struct __attribute__((packed)) pulses_esp_now_t {
//	  icode_t meaning;
//	//  uint8_t data_bytes;
//	//  uint8_t flags1_UNUSED;
//	//  uint8_t flags2_UNUSED;
//	//  uint8_t flags3_UNUSED;
//	  // data
//	} pulses_esp_now_t;
//
//
//	static void send_msg(pulses_esp_now_t * msg) {
//	  // Pack
//	  uint16_t packet_size = sizeof(pulses_esp_now_t);
//	  uint8_t msg_data[packet_size];
//	  memcpy(&msg_data[0], msg, sizeof(pulses_esp_now_t));
//
//	  esp_err_t status = esp_now_send(broadcast_mac, msg_data, packet_size);
//	  if (ESP_OK != status) {
//	    MENU.out(F("send_msg "));
//	    MENU.outln(esp_err_to_name(status));
//	  }
//	}
//
//	esp_err_t esp_now_pulses_broadcast(icode_t code) {
//	  int data_len = 4;	// default for code==meaning
//	  int * ip;
//
//	  switch(code) {
//	  case PRES:
//	    break;
//	  default:
//	    MENU.error_ln(F("data_len, unknown code"));	// TODO: ##################################
//	  }
//
//	  pulses_esp_now_t * msg_buf = (pulses_esp_now_t *) malloc(data_len + 0);	// TODO: determine right size
//	  //  pulses_esp_now_t message;
//
//	  switch(code) {
//	  case PRES:
//	    extern short preset;
//	    ip = (int *) &msg_buf;
//	    *ip = preset;
//	    break;
//	  default:
//	    MENU.error_ln(F("not implemented code"));	// TODO: ##################################
//	  }
//
//
//	  esp_now_register_send_cb(esp_now_data_sent_callback);
//
//	//  return esp_now_send(broadcast_mac, (uint8_t*) msg_buf, sizeof(msg_buf));
//	  esp_err_t status=esp_now_send(broadcast_mac, (uint8_t*) msg_buf, sizeof(msg_buf));
//	  return status;
//	}
//
//	static void msg_recv_cb(const uint8_t *mac_addr, const uint8_t *data, int len) {
//
//	  if (len == sizeof(pulses_esp_now_t)) {
//	    char macStr[18];	// buffer for sender MAC string representation
//
//	    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
//		     mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
//
//	    MENU.out("\nESP-NOW from: ");
//	    MENU.out(macStr);
//	    MENU.tab();
//	    MENU.outln(len);
//
//	    pulses_esp_now_t * msg_buf = (pulses_esp_now_t *) malloc(len + 16);	// TODO: determine right size
//	    memcpy(msg_buf, data, len);
//
//	//    MENU.out(F("esp_now_pulses_reaction_old "));
//	//    MENU.outln((*msg_buf).meaning);
//	    esp_now_pulses_reaction_old(*msg_buf, (uint8_t*) data);
//
//	    free(msg_buf);
//	  } // if(len)
//	}
//
//	//Callback function that tells us when data from Master is received
//	static void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
//	  char macStr[18];	// senders MAC string representation
//	  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
//		   mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
//	  MENU.out("\nESP-NOW from: ");
//	  MENU.out(macStr);
//	  MENU.out(F("\t\tbytes "));
//	  MENU.outln(data_len);
//	//  MENU.outln((int) *data);
//	  pulses_esp_now_t * msg_buf = (pulses_esp_now_t *) malloc(data_len + 16);	// TODO: determine right size
//	  memcpy(msg_buf, data, data_len);
//	  esp_now_pulses_reaction_old(*msg_buf, (uint8_t*) data);
//
//	  free(msg_buf);
//	}
//
//
//
//
