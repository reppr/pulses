/*
  esp_now_pulses.h

  see:	https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/network/esp_now.html#
	https://www.instructables.com/id/ESP32-With-ESP-Now-Protocol/
	https://hackaday.io/project/164132-hello-world-for-esp-now/log/160570-esp-now-introduction
*/

#include <esp_now.h>
#include <WiFi.h>

/*
  DEBUG_ESP_NOW

  a lot of menu output we'll be skipped by high stress levels in everyday use, so
  define DEBUG_ESP_NOW to force more output on esp now processes
  BEWARE:  *this could easily damage the audio with CLICKS*
  mainly used in  if(MENU.maybe_display_more() || DEBUG_ESP_NOW)
*/
#define DEBUG_ESP_NOW		true	// switches ESP_NOW debugging on  BEWARE:  *this could easily damage the audio with CLICKS*
#if ! defined DEBUG_ESP_NOW		// default: ESP_NOW debugging off
  #define DEBUG_ESP_NOW		false	// switches ESP_NOW debugging pff
#endif

#define ESP_NOW_CHANNEL	3

// buffers for data to send or receive
uint8_t esp_now_send_buffer[ESP_NOW_MAX_DATA_LEN] = {0};
uint8_t esp_now_send_buffer_cnt=0;	// bytes

uint8_t esp_now_received_data[ESP_NOW_MAX_DATA_LEN] = {0};
uint8_t esp_now_received_data_cnt=0;	// bytes
uint8_t esp_now_received_data_read=0;	// bytes
uint8_t broadcast_mac[] = {0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF};


// info and debugging
void esp_now_show_raw_data(uint8_t * read_p, uint8_t len) {
  MENU.ln();
  MENU.out(len);
  MENU.outln(F(" BYTES raw data\tu8,\tint"));
  for(int i=0; i<len; i++) {
    MENU.out('|');
    MENU.out(*(read_p + i));
    MENU.out("| ");
  }
  MENU.ln();

  if(len >= sizeof(icode_t)) {
    icode_t * ip = (icode_t*) read_p;

    MENU.outln((int) *ip);
  }
  MENU.outln(F("raw data end\n"));
}


// MAC as string
char MACstr[18];
char* setMACstr(const uint8_t* mac) {
  snprintf(MACstr, sizeof(MACstr), "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return MACstr;
}


// error reporting
bool /* error */ esp_err_info(esp_err_t status) {
  if(status == ESP_OK) {	// ok
    if(MENU.maybe_display_more(VERBOSITY_LOWEST) || DEBUG_ESP_NOW)
      MENU.outln(F("ok"));
    return false;	// OK
  } else			// not ok
    if(MENU.maybe_display_more(VERBOSITY_LOWEST/* sic! */) || DEBUG_ESP_NOW)	// *do* display that
      MENU.error_ln(esp_err_to_name(status));
  return true;		// ERROR
}


// sending:
void esp_now_data_sent_callback(const uint8_t *mac_addr, esp_now_send_status_t status) {
  if(MENU.maybe_display_more(VERBOSITY_LOWEST) || DEBUG_ESP_NOW) {	// display MAC?
    MENU.out(F("sent to:  "));
    MENU.out(setMACstr(mac_addr));
    MENU.tab();
    esp_err_info(status);
  }
}

esp_err_t esp_now_pulses_send() {	// send data stored in esp_now_send_buffer
  esp_err_t status = esp_now_send(broadcast_mac, esp_now_send_buffer, esp_now_send_buffer_cnt);

  if(MENU.maybe_display_more(VERBOSITY_LOWEST) || DEBUG_ESP_NOW)
    esp_err_info(status);

  return status;
}

esp_err_t esp_now_send_preset(/* recipient */ short preset) {
  icode_t* i_data = (icode_t*) esp_now_send_buffer;
  *i_data++ = PRES;
  esp_now_send_buffer_cnt = sizeof(icode_t);	// icode_t meaning

  short* short_p = (short *) &esp_now_send_buffer[esp_now_send_buffer_cnt];
  extern short preset;
  memcpy(short_p, &preset, sizeof(short));
  esp_now_send_buffer_cnt += sizeof(short);

  if(MENU.maybe_display_more(VERBOSITY_LOWEST) || DEBUG_ESP_NOW) {
    MENU.out(F("ESP-NOW sent PRES "));
    MENU.out(preset);
    MENU.tab();
  }

  esp_err_t status = esp_now_send(broadcast_mac, esp_now_send_buffer, esp_now_send_buffer_cnt);

  if(MENU.maybe_display_more(VERBOSITY_LOWEST) || DEBUG_ESP_NOW)
    esp_err_info(status);

  return status;
}

esp_err_t  esp_now_send_macro(/* recipient */ char * macro) {
  icode_t* i_data = (icode_t*) esp_now_send_buffer;
  uint8_t len = strlen(macro) + 1;
  if(MENU.maybe_display_more(VERBOSITY_LOWEST) || DEBUG_ESP_NOW) {
    MENU.out(F("esp_now_send_macro()  "));
    MENU.outln(macro);
  }

  *i_data++ = MACRO_NOW;
  esp_now_send_buffer_cnt = sizeof(icode_t);	// icode_t meaning

  memcpy(i_data, macro, len);
  esp_now_send_buffer_cnt += len;

  esp_err_t status = esp_now_send(broadcast_mac, esp_now_send_buffer, esp_now_send_buffer_cnt);

  if(MENU.maybe_display_more(VERBOSITY_LOWEST) || DEBUG_ESP_NOW)
    esp_err_info(status);

  return status;
}


// receiving:
static void esp_now_pulses_reaction() {
#if DEBUG_ESP_NOW != false
  MENU.outln("esp_now_pulses_reaction()");
#endif
  if(MENU.maybe_display_more(VERBOSITY_LOWEST) || DEBUG_ESP_NOW)
    MENU.out(F("received: "));

  esp_now_received_data_read=0;
  // get meaning:
  icode_t* i_data = (icode_t*) esp_now_received_data;
  icode_t meaning = *i_data;
  esp_now_received_data_read += sizeof(icode);
  i_data += sizeof(icode);
  short * sp;

  switch (meaning) {
  case PRES:
    // read preset number:
    extern short preset;
    //    sp = (short*) i_data;
    sp = (short*) &esp_now_received_data[esp_now_received_data_read];
    preset = *sp;
    esp_now_received_data_read += sizeof(short);
    i_data += sizeof(short);
    // i_data += sizeof(short);	// not needed on last item

    if(MENU.maybe_display_more(VERBOSITY_LOWEST) || DEBUG_ESP_NOW) {
      MENU.out(F("PRES "));
      MENU.out(preset);
      MENU.tab();
    }

    extern bool load_preset_and_start(short preset);
    load_preset_and_start(preset);
    break;

  case MACRO_NOW:
    {
      char* macro = (char*) esp_now_received_data + sizeof(icode_t);
      MENU.out(F("react on MACRO_NOW  "));
      MENU.outln(macro);
      MENU.play_KB_macro(macro);
    }
    break;

  default:
    MENU.out((int) meaning);
    MENU.space(2);
    MENU.error_ln(F("what?"));
    esp_now_show_raw_data(esp_now_received_data, esp_now_received_data_cnt);
  } // switch meaning
}

static void pulses_data_received_callback(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  esp_now_received_data_cnt = data_len;
  memcpy(esp_now_received_data, data, data_len);
  esp_now_received_data_read = 0;

  if(MENU.maybe_display_more(VERBOSITY_LOWEST) || DEBUG_ESP_NOW) {
    MENU.out("\npulses_data_received_callback()  from: ");
    MENU.out(setMACstr(mac_addr));
    MENU.out(F("\t\tbytes "));
    MENU.outln(data_len);
  }

  esp_now_pulses_reaction();
}

void esp_now_send_and_do_macro(/* recipient, */ char * macro) {
  if(MENU.maybe_display_more(VERBOSITY_LOWEST) || DEBUG_ESP_NOW) {
    MENU.out(F("esp_now_send_and_do_macro() "));
    MENU.outln(macro);
  }

  esp_err_t status = esp_now_send_macro(macro);
  if(status == ESP_OK)	// if sent, do it locally now:
    MENU.play_KB_macro(macro);
  else						// sending failed
    if(MENU.maybe_display_more(VERBOSITY_LOWEST/* sic! */) || DEBUG_ESP_NOW)	// *do* display that
      MENU.error_ln(esp_err_to_name(status));
}


// setup:
esp_now_peer_info_t peer_info;

esp_err_t esp_now_pulses_setup() {
  MENU.outln(F("esp_now_pulses_setup()"));

  MENU.outln(F("  WiFi.mode(WIFI_OFF)"));
  WiFi.mode(WIFI_OFF);
  yield();

  MENU.outln(F("  WiFi.mode(WIFI_STA)"));
  WiFi.mode(WIFI_STA);
  yield();

  MENU.outln(F("  WiFi.disconnect()"));
  WiFi.disconnect();	// just in case
  yield();

  esp_err_t status;
  MENU.outln(F("  esp_now_init()"));
  yield();
  status = esp_now_init();
  if(status != ESP_OK)
    return status;

  MENU.outln(F("  esp_now_register_recv_cb()"));
  yield();
  status = esp_now_register_recv_cb(pulses_data_received_callback);
  if(status != ESP_OK)
    return status;

  yield();
  MENU.outln(F("  esp_now_add_peer()"));
  peer_info.channel = ESP_NOW_CHANNEL;
  memcpy(peer_info.peer_addr, broadcast_mac, 6);
  peer_info.ifidx = ESP_IF_WIFI_STA;
  peer_info.encrypt = false;

  status = esp_now_add_peer(&peer_info);

  return status;
}
