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
  #define DEBUG_ESP_NOW		false	// switches ESP_NOW debugging off
#endif

#define ESP_NOW_CHANNEL	4

// react on broadcast or all-known-peers messages in an individual time slice
// defines the length of *one* slice in milliseconds
#define ESP_NOW_TIME_SLICE_MS	12	// TODO: TEST&TRIMM:

// buffers for data to send or receive
uint8_t esp_now_send_buffer[ESP_NOW_MAX_DATA_LEN] = {0};
uint8_t esp_now_send_buffer_cnt=0;	// bytes

uint8_t esp_now_received_data[ESP_NOW_MAX_DATA_LEN] = {0};
uint8_t esp_now_received_data_cnt=0;	// bytes
uint8_t esp_now_received_data_read=0;	// bytes

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


// MAC address
typedef uint8_t mac_addr_t;
mac_addr_t broadcast_mac[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
mac_addr_t* known_peers_mac_p = NULL;	// this is a *PSEUDO MAC POINTER* to NULL
					//   esp_now_send() will *send to ALL KNOWN PEERS*
mac_addr_t time_sliced_sent_to_mac[6]={0};

/*
  mac_addr_t*  esp_now_send2_mac_p

  points to the mac esp_now_send() will send to
  if NULL: send to *ALL KNOWN* peers
*/
mac_addr_t* esp_now_send2_mac_p = broadcast_mac; // DEFAULT: *first* message will be BROADCASTed

// MAC as string
char* MAC_str(const mac_addr_t* mac) {	// TODO: TEST: mac==NULL case
  if(mac == NULL)
    return ">*< ALL KNOWN >*<";
  // else

  static char MACstr[18];
  snprintf(MACstr, sizeof(MACstr), "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  return MACstr;
}

mac_addr_t my_MAC[] = {0,0,0,0,0,0};


// esp_now_reaction_timer  give reactions on broadcast or all_known_peers messages in an individual time slice
hw_timer_t * esp_now_reaction_timer = NULL;


// peer_ID_t
typedef struct peer_ID_t {
  String preName="";
  mac_addr_t mac_addr[6]={0};
  uint8_t esp_now_time_slice=0;	// react on broadcast or all-known-peers messages in an individual time slice
  uint8_t version=0;
} peer_ID_t;

peer_ID_t my_IDENTITY;


void show_peer_id(peer_ID_t* this_peer_ID_p) {
  MENU.out(F("IDENTITY\t"));
  MENU.out(this_peer_ID_p->preName);

  MENU.out(F("\tMAC\t"));
  MENU.out(MAC_str(this_peer_ID_p->mac_addr));

  MENU.out(F("\ttime slice  "));
  MENU.out((int) this_peer_ID_p->esp_now_time_slice);

  MENU.out(F("\tversion\t"));
  MENU.outln((int) this_peer_ID_p->version);
}


void set_my_IDENTITY() {
  esp_read_mac(my_IDENTITY.mac_addr, ESP_MAC_WIFI_STA);
  extern String nvs_getString(char * key);
  my_IDENTITY.preName = nvs_getString(F("nvs_PRENAME"));

  // my_IDENTITY.esp_now_time_slice
}

// esp_err_t ERROR reporting
#if ! defined ESP_ERR_INFO_DEFINED
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
  #define ESP_ERR_INFO_DEFINED
#endif


// sending:
void esp_now_data_sent_callback(const mac_addr_t *mac_addr, esp_now_send_status_t status) {
  if(MENU.maybe_display_more(VERBOSITY_LOWEST) || DEBUG_ESP_NOW) {	// display MAC?
    MENU.out(F("esp now sent to:  "));
    MENU.out(MAC_str(mac_addr));
    MENU.tab();
    esp_err_info(status);
  }
}

esp_err_t esp_now_pulses_send(const mac_addr_t *mac_addr) {	// send data stored in esp_now_send_buffer
  esp_err_t status = esp_now_send(mac_addr, esp_now_send_buffer, esp_now_send_buffer_cnt);

  if(MENU.maybe_display_more(VERBOSITY_LOWEST) || DEBUG_ESP_NOW)
    esp_err_info(status);

  return status;
}

// some messages contain only the meaning code, no additional parameters:
esp_err_t esp_now_send_bare(mac_addr_t* mac_addr, icode_t meaning) {
#if defined DEBUG_ESP_NOW
  MENU.out(F("esp_now_send_bare() "));
#endif

  icode_t* i_data = (icode_t*) esp_now_send_buffer;
  *i_data = meaning;
  esp_now_send_buffer_cnt = sizeof(icode_t);	// icode_t meaning

  return esp_now_pulses_send(mac_addr);
}

esp_err_t esp_now_send_preset(mac_addr_t* mac_addr, short preset) {
  icode_t* i_data = (icode_t*) esp_now_send_buffer;
  *i_data++ = PRES;
  esp_now_send_buffer_cnt = sizeof(icode_t);	// icode_t meaning

  short* short_p = (short *) &esp_now_send_buffer[esp_now_send_buffer_cnt];
  extern short preset;
  memcpy(short_p, &preset, sizeof(short));
  esp_now_send_buffer_cnt += sizeof(short);
  bool do_display = (MENU.maybe_display_more(VERBOSITY_LOWEST) || DEBUG_ESP_NOW);
  if(do_display) {
    MENU.out(F("ESP-NOW sent PRES "));
    MENU.out(preset);
    MENU.tab();
  }

  esp_err_t status = esp_now_send(mac_addr, esp_now_send_buffer, esp_now_send_buffer_cnt);

  if(do_display)
    esp_err_info(status);

  return status;
}

void esp_now_add_mine() {	// adds identity data to the esp_now_send_buffer[]
#if defined DEBUG_ESP_NOW
  MENU.out(F(" esp_now_add_mine() "));
#endif

  peer_ID_t* ID_p = (peer_ID_t *) &esp_now_send_buffer[esp_now_send_buffer_cnt];
  memcpy(ID_p, &my_IDENTITY, sizeof(peer_ID_t));
  esp_now_send_buffer_cnt += sizeof(peer_ID_t);
}

peer_ID_t esp_now_read_mine() {	// reads identity data from esp_now_received_data
  peer_ID_t read_peer_ID;
  uint8_t* data_read_p = esp_now_received_data + esp_now_received_data_read;
  for(int b=0; b<6; b++)
    read_peer_ID.mac_addr[b] = *data_read_p++;
  esp_now_received_data_read += 6;

  char c;
  for(int i=0; i<16; i++) {
    read_peer_ID.preName[i] = c = *data_read_p++;
    if(c == '\0')
      break;
  }

  return read_peer_ID;
}

esp_err_t esp_now_send_who(mac_addr_t* mac_addr) {
  bool do_display = (MENU.maybe_display_more(VERBOSITY_LOWEST) || DEBUG_ESP_NOW);

  icode_t* i_data = (icode_t*) esp_now_send_buffer + esp_now_send_buffer_cnt;
  *i_data++ = N_WHO;
  esp_now_send_buffer_cnt = sizeof(icode_t);	// icode_t meaning

  esp_now_add_mine();

  if(do_display)
    MENU.out(F("ESP-NOW send N_WHO\t"));

  esp_err_t status = esp_now_send(mac_addr, esp_now_send_buffer, esp_now_send_buffer_cnt);
  if(do_display)
    esp_err_info(status);

  return status;
}


esp_err_t  esp_now_send_macro(mac_addr_t* mac_addr, char * macro) {
  icode_t* i_data = (icode_t*) esp_now_send_buffer;
  uint8_t len = strlen(macro) + 1;

  bool do_display = (MENU.maybe_display_more(VERBOSITY_LOWEST) || DEBUG_ESP_NOW);
  if(do_display) {
    MENU.out(F("esp_now_send_macro()  "));
    MENU.outln(macro);
  }

  *i_data++ = MACRO_NOW;
  esp_now_send_buffer_cnt = sizeof(icode_t);	// icode_t meaning

  memcpy(i_data, macro, len);
  esp_now_send_buffer_cnt += len;

  esp_err_t status = esp_now_send(mac_addr, esp_now_send_buffer, esp_now_send_buffer_cnt);

  if(do_display)
    esp_err_info(status);

  return status;
}

unsigned long esp_now_send_HI_time=0L;	// timing ping pong  N_HI - N_HO
esp_err_t  esp_now_send_HI(mac_addr_t* mac_addr) {
  pulse_time_t n = PULSES.get_now();
  esp_now_send_HI_time = n.time;
  return esp_now_send_bare(mac_addr, N_HI);
}


// peers

// typedef peer_ID_t	see above
esp_now_peer_info_t peer_info;

bool mac_is_non_zero(mac_addr_t* mac_addr) {
  if(*mac_addr == NULL) {
    // MENU.out(F("MAC*NULL "));
    return false;	// *not* planed to be used like this...   meanwhile it *is* ;)
  }

  for(int m=0; m<6; m++) {	// check MAC bytes
    if(*(mac_addr + m))
      return true;
  }
  return false;
}

peer_ID_t esp_now_pulses_known_peers[ESP_NOW_MAX_TOTAL_PEER_NUM];

void display_peer_ID_list() {
  MENU.outln(F("known peers:"));
  int i;
  for(i=0; i<ESP_NOW_MAX_TOTAL_PEER_NUM; i++) {
    if(mac_is_non_zero(esp_now_pulses_known_peers[i].mac_addr)) {
      MENU.out(i + 1);	// start counting with 1
      if(i<9)
	MENU.space();
      MENU.space(2);
      MENU.out(MAC_str(esp_now_pulses_known_peers[i].mac_addr));
      MENU.space(2);
      MENU.out('"');
      MENU.out(esp_now_pulses_known_peers[i].preName);
      MENU.outln('"');
    }
  }
  MENU.ln();
} // display_peer_ID_list()

void esp_now_2_ID_list(uint8_t* mac_addr, String preName /*hardware*/) {
  bool do_display = (MENU.maybe_display_more(VERBOSITY_LOWEST) || DEBUG_ESP_NOW);
#if defined DEBUG_ESP_NOW
  MENU.out(F("esp_now_2_ID_list() "));
#endif

  if(mac_addr != NULL) {
    bool peer_is_known=false;
    int i; // index
    for(i=0; i<ESP_NOW_MAX_TOTAL_PEER_NUM; i++) {	// search if peer is known
      for(int m=0; m<6; m++) {	// compare MAC
	if(*(mac_addr + m) != esp_now_pulses_known_peers[i].mac_addr[m])
	  break;
	if(m==5)
	  peer_is_known=true;	// all 6 mac bytes are identical
      }
      if(peer_is_known) {
#if defined DEBUG_ESP_NOW
	MENU.outln(F("peer is known"));
#endif
	return;			// *EARLY EXIT*, keep it quick
      }
    }

    // *PEER WAS UNKNOWN*
#if defined DEBUG_ESP_NOW
    MENU.out(F("esp now new member "));
    MENU.outln(MAC_str(mac_addr));
#endif
    bool free_entry=false;
    for(i=0; i<ESP_NOW_MAX_TOTAL_PEER_NUM; i++) {	// search for free mac field
      if(*mac_addr == NULL) {	// was never set
	MENU.out(" MAC*NULL .");
	MENU.out(i);
	MENU.space(2);

	free_entry = true;
	break;	// we found an empty virgine slot
      } else {
	//	bool mac_is_non_zero(mac_addr_t* mac_addr)
	if(! mac_is_non_zero(esp_now_pulses_known_peers[i].mac_addr)) {	// search empty mac enty
	  free_entry = true;
	  MENU.out(" free_entry ");
	  MENU.out(i);
	  break;	// we found an empty, reusable slot
	}
      }
    } // for all peer slots

    if(free_entry) {	// free_entry at index i	now COPY DATA to pulses own KNOWN PEER LIST
#if defined DEBUG_ESP_NOW
      MENU.out(F("->["));
      MENU.out(i);
      MENU.out(F("] "));
#endif
      // mac
      mac_addr_t* mp = esp_now_pulses_known_peers[i].mac_addr;
      memcpy(mp, mac_addr, 6);

      // preName
      if(preName) {	// TODO: TEST:
	const char* new_preName = preName.c_str();
	int len = strlen(new_preName);
	for(int b=0; b<16; b++) {
	  if(b < len)
	    esp_now_pulses_known_peers[i].preName[b] = new_preName[b];
	  else
	    esp_now_pulses_known_peers[i].preName[b] = '\0';
	}
	MENU.outln(esp_now_pulses_known_peers[i].preName);
      } // else

      // hardware planed
      if(do_display) {
	MENU.out(F("\n\nconnected to "));
	MENU.out(MAC_str(mac_addr));
	MENU.space(2);
	MENU.outln(preName);
      }
    } else		// no free entry
      MENU.error_ln(F("too many peers"));
  } // mac_addr != NULL
} // esp_now_2_ID_list()


// TODO: parameter including preName	peer_ID_t ################
esp_err_t esp_now_pulses_add_peer(const mac_addr_t *mac_addr) {	// might give feedback
  bool do_display = (MENU.maybe_display_more(VERBOSITY_LOWEST) || DEBUG_ESP_NOW);
  if(do_display)
    MENU.out(F("  esp_now_pulses_add_peer()  esp_now_add_peer() "));

  peer_info.channel = ESP_NOW_CHANNEL;
  memcpy(peer_info.peer_addr, mac_addr, 6);
  peer_info.ifidx = ESP_IF_WIFI_STA;
  peer_info.encrypt = false;

  //  esp_err_t status = esp_now_add_peer(&peer_info);
  esp_err_t status = esp_now_add_peer(&peer_info);
  switch (status) {
  case ESP_OK:
    esp_now_2_ID_list((mac_addr_t*) mac_addr, ""); // DADA ""
    if(do_display)
      MENU.outln(F("ok"));
    break;
  case ESP_ERR_ESPNOW_EXIST:
    if(do_display)
      MENU.outln(F("peer existed"));
    break;
  default:
    MENU.error_ln(esp_err_to_name(status));
  }
  return status;
} // esp_now_pulses_add_peer()


void send_response_time_sliced() {
  // N_ME
  ;
}

// time_sliced_sent_to_mac
bool prepare_time_sliced_reaction(mac_addr_t* to_mac) {
  // debug output ################################################################
  MENU.outln(F("prepare_time_sliced_reaction()\tms"));
MENU.outln(my_IDENTITY.esp_now_time_slice * ESP_NOW_TIME_SLICE_MS);

  // save mac
  for (int i=0; i<6; i++)
    time_sliced_sent_to_mac[i] = to_mac[i];


  // prepare data to send


  // setup time sliced reaction
  esp_now_reaction_timer = timerBegin(0, 80, true /* count upwards */);
  timerAttachInterrupt(esp_now_reaction_timer, &send_response_time_sliced, true /* edge */);
  timerAlarmWrite(esp_now_reaction_timer, (my_IDENTITY.esp_now_time_slice * ESP_NOW_TIME_SLICE_MS * 1000), false /* only once */);
  timerAlarmEnable(esp_now_reaction_timer);
  return true; // ################################################################
} // prepare_time_sliced_reaction()


// receiving:
static void esp_now_pulses_reaction(const mac_addr_t *mac_addr) {
#if DEBUG_ESP_NOW != false
  MENU.outln("esp_now_pulses_reaction()");
#endif
  bool do_display = (MENU.maybe_display_more(VERBOSITY_LOWEST) || DEBUG_ESP_NOW);
  if(do_display)
    MENU.out(F("received: "));

  esp_now_received_data_read=0;
  // get meaning:
  icode_t* i_data = (icode_t*) esp_now_received_data;
  icode_t meaning = *i_data;
  esp_now_received_data_read += sizeof(icode);
  i_data += sizeof(icode);
  short * sp;

  switch (meaning) {
  case N_HI:
    esp_now_send_bare(broadcast_mac, N_HO); // FIRST: answer HO,  broadcast (building up peer lists ;)
    // TODO: ERROR check

    if(do_display) {
      MENU.out(MAC_str(mac_addr));
      MENU.out(F("  HI anybody "));
    }
    esp_now_pulses_add_peer(mac_addr);		// might give feedback
    break;

  case N_HO:
    if(do_display) {
      pulse_time_t n = PULSES.get_now();
      MENU.space(2);
      MENU.out(n.time - esp_now_send_HI_time);
      MENU.out(F(" microsec  "));
      MENU.out(MAC_str(mac_addr));
      MENU.out(F("  I AM here  "));
    }

    esp_now_pulses_add_peer(mac_addr);		// might give feedback
    break;

  case N_WHO:
    {
      if(do_display) {
	MENU.out(MAC_str(mac_addr));
	MENU.space(2);
	MENU.outln(F("N_WHO"));
      }

      peer_ID_t received_ID = esp_now_read_mine();
      esp_now_2_ID_list((mac_addr_t*) mac_addr, received_ID.preName);

      prepare_time_sliced_reaction((mac_addr_t*) mac_addr);
    }
    break;

  case N_ME:
    {
      peer_ID_t received_ID = esp_now_read_mine();
      esp_now_2_ID_list((mac_addr_t*) mac_addr, received_ID.preName);	// building up peer info lists
    }
    break;

  case PRES:
    {
      // read preset number:
      sp = (short*) &esp_now_received_data[esp_now_received_data_read];
      //    extern short (musicBoxConf.preset);
      short new_preset = *sp;
      esp_now_received_data_read += sizeof(short);
      i_data += sizeof(short);
      // i_data += sizeof(short);	// not needed on last item

      if(do_display) {
	MENU.out(F("PRES "));
	MENU.out(new_preset);
	MENU.tab();
      }
      esp_now_pulses_add_peer(mac_addr);		// might give feedback

      extern bool load_preset_and_start(short preset);
      load_preset_and_start(new_preset);
    }
    break;

  case MACRO_NOW:
    {
      char* macro = (char*) esp_now_received_data + sizeof(icode_t);
      MENU.out(F("react on MACRO_NOW  "));
      MENU.outln(macro);
      MENU.play_KB_macro(macro);
      esp_now_pulses_add_peer(mac_addr);	// might give feedback
    }
    break;

  default:
    MENU.out((int) meaning);
    MENU.space(2);
    MENU.error_ln(F("what?"));
    esp_now_show_raw_data(esp_now_received_data, esp_now_received_data_cnt);
    // the peer is *not* added
  } // switch meaning
} //  esp_now_pulses_reaction()


static void pulses_data_received_callback(const mac_addr_t *mac_addr, const uint8_t *data, int data_len) {
  esp_now_received_data_cnt = data_len;
  memcpy(esp_now_received_data, data, data_len);
  esp_now_received_data_read = 0;

  if(MENU.maybe_display_more(VERBOSITY_LOWEST) || DEBUG_ESP_NOW) {
    MENU.out("\npulses_data_received_callback()  from: ");
    MENU.out(MAC_str(mac_addr));
    MENU.out(F("\t\tbytes "));
    MENU.outln(data_len);
  }

  esp_now_pulses_reaction(mac_addr);
} // pulses_data_received_callback()


/*
  esp_now_send_maybe_do_macro

  sends a menu KB macro to the configured  esp_now_send2_mac_p*

  IF the mac* == NULL (alias known_peers_mac_p*) || BROADCAST
     THEN *DO* play the macro locally

  else if sending to an INDIVIDUAL
     ONLY send *without playing it locally*	
*/
void esp_now_send_maybe_do_macro(mac_addr_t* mac_addr, char* macro) {
  if(MENU.maybe_display_more(VERBOSITY_LOWEST) || DEBUG_ESP_NOW) {
    MENU.out(F("esp_now_send_maybe_do_macro() "));
    MENU.outln(macro);
  }

  esp_err_t status = esp_now_send_macro(mac_addr, macro);
  if(status == ESP_OK) {	// if sent, maybe DO it locally now?

    if(known_peers_mac_p == NULL || known_peers_mac_p == (mac_addr_t*) &broadcast_mac) {  // to play or not to play?
      MENU.out(F("send AND locally DO "));
      MENU.play_KB_macro(macro);		// only play locally if the recipent is *NOT* an individual
    }

  } else						// sending failed
    if(MENU.maybe_display_more(VERBOSITY_LOWEST/* sic! */) || DEBUG_ESP_NOW)	// *do* display that
      MENU.error_ln(esp_err_to_name(status));
} // esp_now_send_maybe_do_macro()


// setup:
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
  MENU.out(F("  esp_now_init() "));

  status = esp_now_init();
  if(status != ESP_OK)
    return status;
  yield();

  uint32_t esp_now_version=0;
  if(esp_now_get_version(&esp_now_version) == ESP_OK) {
    MENU.out(F("  version "));
    MENU.outln(esp_now_version);
  } // else *ignore* errors here

  MENU.outln(F("  esp_now_register_send_cb()"));
  yield();
  status = esp_now_register_send_cb(esp_now_data_sent_callback);
  if(status != ESP_OK)
    return status;

  MENU.outln(F("  esp_now_register_recv_cb()"));
  yield();
  status = esp_now_register_recv_cb(pulses_data_received_callback);
  if(status != ESP_OK)
    return status;

  yield();
  MENU.out(F("  broadcast ch="));
  MENU.outln((int) ESP_NOW_CHANNEL);

  peer_info.channel = ESP_NOW_CHANNEL;
  memcpy(peer_info.peer_addr, broadcast_mac, 6);
  peer_info.ifidx = ESP_IF_WIFI_STA;
  peer_info.encrypt = false;

  status = esp_now_pulses_add_peer(broadcast_mac);	// add broadcast as peer

  set_my_IDENTITY();

  return status;
} // esp_now_pulses_setup()
