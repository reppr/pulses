/*
  esp_now_pulses.h

  see:	https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/network/esp_now.html#
	https://www.instructables.com/id/ESP32-With-ESP-Now-Protocol/
	https://hackaday.io/project/164132-hello-world-for-esp-now/log/160570-esp-now-introduction
*/

#include <esp_now.h>
#include <WiFi.h>
#include "pulses_esp_err.h"		// esp_err_t ERROR reporting

//#define ESP_NOW_SETUP_CALL_PARTICIPANTS	// TESTING: DEACTIVATED setup participants call

/*
  DEBUG_ESP_NOW_b		_b for bool, true or false

  a lot of menu output we'll be skipped by high stress levels in everyday use, so
  define DEBUG_ESP_NOW_b to force more output on esp now processes
  BEWARE:  *this could easily damage the audio with CLICKS*
  mainly used in  if(MENU.maybe_display_more() || DEBUG_ESP_NOW_b)
*/
//#define DEBUG_ESP_NOW_b	true	// switches ESP_NOW debugging on  BEWARE:  *this could easily damage the audio with CLICKS*
#if ! defined DEBUG_ESP_NOW_b		// default: ESP_NOW debugging off
  #define DEBUG_ESP_NOW_b	false	// switches ESP_NOW debugging off
#endif

//#define DEBUG_ESP_NOW_NETWORKING

#define ESP_NOW_CHANNEL	4		// TODO: UI

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
// typedef uint8_t mac_addr_t;	// *DEPRECIATED*	use uint8_t
uint8_t broadcast_mac[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
uint8_t* known_peers_mac_p = NULL;	// this is a *PSEUDO MAC POINTER* to NULL
					//   esp_now_send() will *send to ALL KNOWN PEERS*
uint8_t time_sliced_sent_to_mac[6]={0};

/*
  uint8_t*  esp_now_send2_mac_p

  points to the mac esp_now_send() will send to
  if NULL: send to *ALL KNOWN* peers
*/
uint8_t* esp_now_send2_mac_p = broadcast_mac; // DEFAULT: *first* message will be BROADCASTed

// esp_now_reaction_timer  give reactions on broadcast or all_known_peers messages in an individual time slice
hw_timer_t * esp_now_reaction_timer = NULL;

extern void show_peer_id(peer_ID_t* this_peer_ID_p);

void set_my_IDENTITY() {
  esp_read_mac(my_IDENTITY.mac_addr, ESP_MAC_WIFI_STA);

// extern String nvs_getString(char * key);
// my_IDENTITY.esp_now_time_slice  set from nvs

#if defined DEBUG_ESP_NOW_NETWORKING
  MENU.outln("\nset_my_IDENTITY()\tset ID");
  show_peer_id(&my_IDENTITY);
  MENU.ln();
#endif
} // set_my_IDENTITY()


// sending:
void esp_now_data_sent_callback(const uint8_t *mac_addr, esp_now_send_status_t status) {
  if(MENU.maybe_display_more(VERBOSITY_SOME) || DEBUG_ESP_NOW_b) {	// display MAC?
    MENU.out(F("esp now sent to:  "));
    MENU.out(MAC_str(mac_addr));
    MENU.tab();
    esp_err_info(status);
  }
} // esp_now_data_sent_callback()

esp_err_t esp_now_pulses_send(const uint8_t *mac_addr) {	// send data stored in esp_now_send_buffer
  esp_err_t status = esp_now_send(mac_addr, esp_now_send_buffer, esp_now_send_buffer_cnt);

  if(MENU.maybe_display_more(VERBOSITY_SOME) || DEBUG_ESP_NOW_b) {
    MENU.out(F("esp_now_pulses_send() "));
    esp_err_info(status);
  }

  return status;
} // esp_now_pulses_send()


// some messages contain only the meaning code, no additional parameters:
esp_err_t esp_now_send_bare(uint8_t* mac_addr, icode_t meaning) {
#if DEBUG_ESP_NOW_b
  MENU.out(F("esp_now_send_bare() "));
#endif

  icode_t* i_data = (icode_t*) esp_now_send_buffer;
  *i_data = meaning;
  esp_now_send_buffer_cnt = sizeof(icode_t);	// icode_t meaning

  return esp_now_pulses_send(mac_addr);
} // esp_now_send_bare()


esp_err_t esp_now_send_preset(uint8_t* mac_addr, short preset) {	// do we need that?
  icode_t* i_data = (icode_t*) esp_now_send_buffer;
  *i_data++ = PRES;
  esp_now_send_buffer_cnt = sizeof(icode_t);	// icode_t meaning

  short* short_p = (short *) &esp_now_send_buffer[esp_now_send_buffer_cnt];
  extern short preset;
  memcpy(short_p, &preset, sizeof(short));
  esp_now_send_buffer_cnt += sizeof(short);

  bool do_display = (MENU.maybe_display_more(VERBOSITY_SOME) || DEBUG_ESP_NOW_b);
  if(do_display) {
    MENU.out(F("ESP-NOW sent PRES "));
    MENU.out(preset);
    MENU.tab();
  }

  esp_err_t status = esp_now_send(mac_addr, esp_now_send_buffer, esp_now_send_buffer_cnt);

  if(do_display)
    esp_err_info(status);

  return status;
} //esp_now_send_preset()


void esp_now_add_identity() {	// adds identity data to the esp_now_send_buffer[]
#if DEBUG_ESP_NOW_b
  MENU.out(F("esp_now_add_identity() "));
#endif

  peer_ID_t* ID_p = (peer_ID_t *) &esp_now_send_buffer[esp_now_send_buffer_cnt];
  memcpy(ID_p, &my_IDENTITY, sizeof(peer_ID_t));

#if defined DEBUG_ESP_NOW_NETWORKING
  MENU.outln("\nesp_now_add_identity()\tadded ID to be sent");
  show_peer_id((peer_ID_t*) &esp_now_send_buffer[esp_now_send_buffer_cnt]);
  MENU.ln();
#endif

  esp_now_send_buffer_cnt += sizeof(peer_ID_t);
} // esp_now_add_identity()


void esp_now_read_received_identity(peer_ID_t* receive_ID_p) {	// reads identity data from esp_now_received_data
  uint8_t* data_read_p = esp_now_received_data + esp_now_received_data_read;

  memcpy(receive_ID_p, data_read_p, sizeof(peer_ID_t));
  //esp_now_received_data_read += sizeof(peer_ID_t) ;	// not needed here

#if defined DEBUG_ESP_NOW_NETWORKING
  MENU.outln("\nesp_now_read_received_identity()\tgot ID ");
  show_peer_id(receive_ID_p);
  MENU.ln();
#endif
} // esp_now_read_received_identity()


esp_err_t esp_now_send_who(uint8_t* mac_addr) {
  esp_err_t status;
  bool do_display;
  if(do_display = (MENU.maybe_display_more(VERBOSITY_SOME) || DEBUG_ESP_NOW_b))
    MENU.out(F("ESP-NOW send N_WHO\t"));

  status = esp_now_send_bare(mac_addr, N_WHO);
  if(do_display || (status != ESP_OK))
    esp_err_info(status);

  return status;
} // esp_now_send_who()


esp_err_t  esp_now_send_macro(uint8_t* mac_addr, char * macro) {
  icode_t* i_data = (icode_t*) esp_now_send_buffer;
  uint8_t len = strlen(macro) + 1;

  bool do_display = (MENU.maybe_display_more(VERBOSITY_SOME) || DEBUG_ESP_NOW_b);
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
} // esp_now_send_macro()


unsigned long esp_now_send_HI_time=0L;	// timing ping pong  N_HI - N_HO
#if defined PULSES_USE_DOUBLE_TIMES
esp_err_t  esp_now_send_HI(uint8_t* mac_addr) {
  esp_now_send_HI_time = (unsigned long) PULSES.get_now();
  return esp_now_send_bare(mac_addr, N_HI);
}

#else
esp_err_t  esp_now_send_HI(uint8_t* mac_addr) {
  pulse_time_t n = PULSES.get_now();
  esp_now_send_HI_time = n.time;
  return esp_now_send_bare(mac_addr, N_HI);
}
#endif

// peers

// typedef peer_ID_t	see above
esp_now_peer_info_t peer_info;

bool mac_is_non_zero(uint8_t* mac_addr) {
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
  MENU.out_IstrI(my_IDENTITY.preName);
  MENU.outln(F(" known peers:"));
  int i;
  for(i=0; i<ESP_NOW_MAX_TOTAL_PEER_NUM; i++) {
    if(mac_is_non_zero(esp_now_pulses_known_peers[i].mac_addr)) {
      MENU.out(i + 1);	// start counting with 1
      if(i<9)
	MENU.space();
      MENU.space(2);
      MENU.out(MAC_str(esp_now_pulses_known_peers[i].mac_addr));
      MENU.space(2);
      MENU.out_IstrI(esp_now_pulses_known_peers[i].preName);
      MENU.tab();
      MENU.outln(esp_now_pulses_known_peers[i].esp_now_time_slice);
    }
  }
  MENU.ln();
} // display_peer_ID_list()


void esp_now_ID_2_list(peer_ID_t* ID_p) {
#if DEBUG_ESP_NOW_b
  MENU.outln(F("esp_now_ID_2_list()"));
#endif

  if((*ID_p).mac_addr != NULL) {
    uint8_t peer_is_known=ILLEGAL8;
    int peer; // index
    for(peer=0; peer<ESP_NOW_MAX_TOTAL_PEER_NUM; peer++) {	// search if peer is known

      for(int m=0; m<6; m++) {	// compare MAC
	if((*ID_p).mac_addr[m] != esp_now_pulses_known_peers[peer].mac_addr[m])
	  break;
	if(m==5)
	  peer_is_known = peer;	// all 6 mac bytes are identical
      }

      if(peer_is_known != ILLEGAL8) {	// known peer?
	if((*ID_p).preName != '\0') {	// preName could have been empty or changed ;)
	  for(int b=0; b<16; b++)
	    esp_now_pulses_known_peers[peer].preName[b] = (*ID_p).mac_addr[b];
	  esp_now_pulses_known_peers[peer].preName[15] = '\0';	// savety net
	}
#if DEBUG_ESP_NOW_b || defined DEBUG_ESP_NOW_NETWORKING
	MENU.out(F(" peer is known "));
	MENU.out((int) peer_is_known);
	MENU.tab();
	MENU.out_IstrI((*ID_p).preName);
#endif
	return;			// *EARLY EXIT*, keep it quick
      }
    }

    // *PEER WAS UNKNOWN*
#if DEBUG_ESP_NOW_b
    MENU.out(F("esp now NEW member "));
#endif
    int free_entry=ILLEGAL32;
    for(int i=0; i<ESP_NOW_MAX_TOTAL_PEER_NUM; i++) {	// search for free mac field
      if(esp_now_pulses_known_peers[i].mac_addr == NULL) {	// was never set
	MENU.out(" MAC*NULL .");
	MENU.out(i);
	MENU.space(2);

	free_entry = i;
	break;	// we found an empty virgine slot
      } else {
	//	bool mac_is_non_zero(uint8_t* mac_addr)
	if(! mac_is_non_zero(esp_now_pulses_known_peers[i].mac_addr)) {	// search empty mac enty
	  free_entry = i;
#if DEBUG_ESP_NOW_b
	  MENU.out(" free_entry ");
	  MENU.out(free_entry);
#endif
	  break;	// we found an empty, reusable slot
	}
      }
    } // for all peer slots

    if(free_entry != ILLEGAL32) {	// free_entry at index i	now COPY DATA to pulses own KNOWN PEER LIST
#if DEBUG_ESP_NOW_b
      MENU.out(F("->["));
      MENU.out(free_entry);
      MENU.out(F("] "));
#endif
      esp_now_pulses_known_peers[free_entry] = *ID_p;
      esp_now_pulses_known_peers[free_entry].preName[15] = '\0';	// savety net

      if(MENU.verbosity >= VERBOSITY_LOWEST) {
	MENU.out_IstrI(my_IDENTITY.preName);
	MENU.out(F(" connected to "));
	show_peer_id(&esp_now_pulses_known_peers[free_entry]);
      }
      // TODO: new connected peer maybe show on monochrome?
    } else		// no free entry
      MENU.error_ln(F("too many peers"));
  } // mac_addr != NULL
} // esp_now_ID_2_list()


esp_err_t esp_now_pulses_add_peer(peer_ID_t* ID_new_p) {	// might give feedback
  bool do_display = (MENU.maybe_display_more(VERBOSITY_SOME) || DEBUG_ESP_NOW_b);
  if(do_display)
    MENU.out(F("esp_now_pulses_add_peer(), esp_now_add_peer() "));

  peer_info.channel = ESP_NOW_CHANNEL;
  memcpy(peer_info.peer_addr, (*ID_new_p).mac_addr, 6);	// if((*ID_new_p).mac_addr != NULL)
  peer_info.ifidx = ESP_IF_WIFI_STA;
  peer_info.encrypt = false;

  esp_err_t status = esp_now_add_peer(&peer_info);	// try to add peer
  switch (status) {
  case ESP_OK:						// *new* peer added
    //    *ID_new_p.preName[15] = '\0';			// savety net
    esp_now_ID_2_list(ID_new_p);
    if(do_display) {
      MENU.out(F("ok\t"));
      MENU.out_IstrI((*ID_new_p).preName);
      MENU.out(F(" added"));
    }
    break;
  case ESP_ERR_ESPNOW_EXIST:	// peer was already seen
    if(do_display) {
      MENU.out(F(" peer existed "));
      MENU.out_IstrI((*ID_new_p).preName);
    }
    if((*ID_new_p).preName != '\0') {	// if there *is* a preName, copy prename into list again:
      (*ID_new_p).preName[15] = '\0';			// savety net
      ; //      DADA search index
    }
    if(do_display)
      MENU.ln();
    break;
  default:
    MENU.error_ln(esp_err_to_name(status));
  }
  return status;
} // esp_now_pulses_add_peer()


esp_err_t esp_now_add_peer_mac_only(const uint8_t *mac_addr) {	// might give feedback
  peer_ID_t fake_id;
  memcpy(fake_id.mac_addr, mac_addr, 6);
  if(MENU.maybe_display_more(VERBOSITY_SOME) || DEBUG_ESP_NOW_b)
    MENU.out(F(" add mac only "));

  return esp_now_pulses_add_peer(&fake_id);
}

void send_IDENTITY_time_sliced() {	// send data stored in esp_now_send_buffer
  esp_err_t status = esp_now_pulses_send(time_sliced_sent_to_mac);

  if(MENU.maybe_display_more(VERBOSITY_SOME) || DEBUG_ESP_NOW_b)
    esp_err_info(status);

//	#if defined DEBUG_ESP_NOW_NETWORKING
//	  MENU.outln("\nsend_IDENTITY_time_sliced() sent previously prepared data");
//	#endif
}


void esp_now_send_identity(uint8_t* to_mac) {
#if ! defined DEBUG_ESP_NOW_NETWORKING	// else print anyway
  if(MENU.verbosity > VERBOSITY_SOME)
#endif
    MENU.out(F("ESP-NOW send N_ID\t"));

  // prepare data to send
  icode_t* i_data = (icode_t*) esp_now_send_buffer;
  *i_data++ = N_ID;
  esp_now_send_buffer_cnt = sizeof(icode_t);	// icode_t meaning
  esp_now_add_identity();			// my_IDENTITY

#if defined DEBUG_ESP_NOW_NETWORKING
  MENU.outln("\nesp_now_send_identity()\tread N_ID from send buffer ");
  show_peer_id((peer_ID_t*) i_data);
  MENU.ln();
#endif

  esp_err_t status = esp_now_send(to_mac, esp_now_send_buffer, esp_now_send_buffer_cnt);
  if(MENU.verbosity > VERBOSITY_SOME) {
    esp_err_info(status);
    MENU.out(F("sent "));
  }
} // esp_now_send_identity()


// prepare N_ID IDENTITY message to be sent in it's time slice
void esp_now_prepare_N_ID(uint8_t* to_mac) {
  if(MENU.verbosity > VERBOSITY_SOME)
    MENU.out(F("esp_now_prepare_N_ID()\tms "));

  if(my_IDENTITY.esp_now_time_slice == ILLEGAL8) {
    MENU.error_ln(F("no time slice defined, will *NOT* send"));
    return;
  }

  MENU.outln(my_IDENTITY.esp_now_time_slice * ESP_NOW_TIME_SLICE_MS);

  // save mac
  for (int i=0; i<6; i++)
    time_sliced_sent_to_mac[i] = to_mac[i];

  // prepare data to send
  icode_t* i_data = (icode_t*) esp_now_send_buffer;
  *i_data++ = N_ID;
  esp_now_send_buffer_cnt = sizeof(icode_t);	// icode_t meaning
  esp_now_add_identity();			// my_IDENTITY

  // setup time sliced reaction
  esp_now_reaction_timer = timerBegin(0, 80, true /* count upwards */);
  timerAttachInterrupt(esp_now_reaction_timer, &send_IDENTITY_time_sliced, true /* edge */);
  timerAlarmWrite(esp_now_reaction_timer, (my_IDENTITY.esp_now_time_slice * ESP_NOW_TIME_SLICE_MS * 1000), false /* only once */);
  timerAlarmEnable(esp_now_reaction_timer);
} // esp_now_prepare_N_ID()


// receiving:
static void esp_now_pulses_reaction(const uint8_t *mac_addr) {
#if DEBUG_ESP_NOW_b != false
  MENU.outln("esp_now_pulses_reaction()");
#endif
  bool do_display = (MENU.maybe_display_more(VERBOSITY_SOME) || DEBUG_ESP_NOW_b);
  if(do_display)
    MENU.out(F("\nesp_now_pulses_reaction()  received: "));

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
    esp_now_add_peer_mac_only(mac_addr);		// might give feedback
    break;

  case N_HO:
    if(do_display) {
      MENU.space(2);

#if defined PULSES_USE_DOUBLE_TIMES
      MENU.out((unsigned long) PULSES.get_now() - esp_now_send_HI_time);
#else
      pulse_time_t n = PULSES.get_now();
      MENU.out(n.time - esp_now_send_HI_time);
#endif
      MENU.out(F(" microsec  "));

      MENU.out(MAC_str(mac_addr));
      MENU.out(F("  I AM here  "));
    }

    esp_now_add_peer_mac_only(mac_addr);		// might give feedback
    break;

  case N_WHO:
    {
      if(do_display) {
	MENU.out(MAC_str(mac_addr));
	MENU.space(2);
	MENU.outln(F("N_WHO"));
      }
      esp_now_add_peer_mac_only(mac_addr);		// might give feedback

      esp_now_prepare_N_ID((uint8_t*) mac_addr);
    }
    break;

  case N_ID:
    {
      peer_ID_t received_ID;
      esp_now_read_received_identity(&received_ID);

#if defined DEBUG_ESP_NOW_NETWORKING
      MENU.outln("\nesp_now_pulses_reaction()\tread N_ID");
      show_peer_id(&received_ID);
      MENU.ln();
#endif

      esp_now_pulses_add_peer(&received_ID);	// might give feedback
    }
    break;

  case N_ST:
    extern void sync_landscape_time_sliced();
    sync_landscape_time_sliced();
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
      esp_now_add_peer_mac_only(mac_addr);

      extern bool load_preset_and_start(short preset, bool start=true);
      load_preset_and_start(new_preset);
    }
    break;

  case MACRO_NOW:
    {
      char* macro = (char*) esp_now_received_data + sizeof(icode_t);
      MENU.out(F("react on MACRO_NOW  "));
      MENU.outln(macro);
      MENU.play_KB_macro(macro);
      esp_now_add_peer_mac_only(mac_addr);	// might give feedback
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


static void pulses_data_received_callback(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  esp_now_received_data_cnt = data_len;
  memcpy(esp_now_received_data, data, data_len);
  esp_now_received_data_read = 0;

  if(MENU.maybe_display_more(VERBOSITY_SOME) || DEBUG_ESP_NOW_b) {
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
void esp_now_send_maybe_do_macro(uint8_t* mac_addr, char* macro) {
  if(MENU.maybe_display_more(VERBOSITY_SOME) || DEBUG_ESP_NOW_b) {
    MENU.out(F("esp_now_send_maybe_do_macro() "));
    MENU.outln(macro);
  }

  esp_err_t status = esp_now_send_macro(mac_addr, macro);
  if(status == ESP_OK) {	// if sent, maybe DO it locally now?
    MENU.out(F("ok, sent\t"));

    if(known_peers_mac_p == NULL || known_peers_mac_p == (uint8_t*) &broadcast_mac) {  // to play or not to play?
      MENU.out(F("and *do* locally "));
      MENU.play_KB_macro(macro);		// only play locally if the recipent is *NOT* an individual
    } else
      MENU.outln(F("*not* done locally"));

  } else						// sending failed
    if(MENU.maybe_display_more(VERBOSITY_LOWEST/* sic! */) || DEBUG_ESP_NOW_b)	// *do* display that
      MENU.error_ln(esp_err_to_name(status));
} // esp_now_send_maybe_do_macro()


void esp_now_call_participants() {	// kickstart network connections
  if(MENU.maybe_display_more(VERBOSITY_SOME) || DEBUG_ESP_NOW_b)
    MENU.outln(F("esp_now_call_participants()"));

  esp_now_send_identity(broadcast_mac);
  yield();
  delay(100); // ################################################################
  esp_now_send_who(broadcast_mac);
}


//#define ESP_NOW_IDLE_ID_SEND	// if ever used again, make it switchable...
#if defined ESP_NOW_IDLE_ID_SEND
  // if the engine considers it appropriate send identification from time to time
  int esp_now_idle_ms = 20000;	// ms	all 20"
  hw_timer_t * esp_now_idle_identity_timer = NULL; // ms
  bool volatile esp_now_send_idle_identity=false;

  void trigger_idle_identification() {
    esp_now_send_idle_identity=true;
  }

  bool esp_now_idle_identification() {
    if(! esp_now_send_idle_identity) {
      esp_now_send_idle_identity=true;
      return false;
    }

    extern bool musicbox_is_awake();
    extern bool musicbox_is_idle();
    if(musicbox_is_idle()) {
      esp_now_send_identity(broadcast_mac);
      esp_now_send_idle_identity = false;
      return true;
    } // else

    esp_now_send_idle_identity = false;
    if(MENU.maybe_display_more(VERBOSITY_SOME))
      MENU.outln("MusicBox is playing\tID not sent");	// TODO: REMOVE?:
    return false;
  }
#endif	// ESP_NOW_IDLE_ID_SEND

esp_err_t add_broascast_2_ESP_peer_list() {
  peer_ID_t broadcast_ID;
  memcpy(&broadcast_ID.mac_addr, &broadcast_mac, 6);
  char preName[16] = "broadcast\0\0\0\0\0\0";
  memcpy(&broadcast_ID.preName, preName, 16);
  return esp_now_pulses_add_peer(&broadcast_ID);	// add broadcast as peer to ESP list
}

// setup:
esp_err_t esp_now_pulses_setup() {
  MENU.outln(F("esp_now_pulses_setup()"));

  MENU.outln(F("  WiFi.mode(WIFI_OFF)"));
  WiFi.mode(WIFI_OFF);
  yield();

  MENU.outln(F("  WiFi.mode(WIFI_STA)"));
  WiFi.mode(WIFI_STA);				// TODO: BUG: does not returm on PSRAM boards!
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

  status = add_broascast_2_ESP_peer_list();		// add broadcast as ESP peer

  set_my_IDENTITY();

#if defined ESP_NOW_IDLE_ID_SEND
  // TODO: UI switch! ################
  // if "idle" broadcast IDENTITY from time to time
  MENU.out(F("  if idle broadcast identity all "));
  MENU.out((float) esp_now_idle_ms / 1000.0);
  MENU.outln(F("\""));

  esp_now_idle_identity_timer = timerBegin(0, 80, true /* count upwards */);	// ms
  if(esp_now_idle_identity_timer==NULL)
    MENU.error_ln(F("timerBegin"));
  else {
    timerAttachInterrupt(esp_now_idle_identity_timer, &trigger_idle_identification, true /* edge */);
    timerAlarmWrite(esp_now_idle_identity_timer, esp_now_idle_ms*1000, true /* repeat */);
    timerAlarmEnable(esp_now_idle_identity_timer);
  }
#endif

return status;
} // esp_now_pulses_setup()
