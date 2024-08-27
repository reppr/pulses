/*
  peer_ID.h
*/

#if ! defined PEER_ID_H

// peer_ID_t
typedef struct peer_ID_t {
  uint8_t mac_addr[6]={0};
  char preName[16]={0};
  uint8_t esp_now_time_slice=ILLEGAL8;	// react on broadcast or all-known-peers messages in an individual time slice
  uint8_t version=0;
} peer_ID_t;

void show_peer_id(peer_ID_t* this_peer_ID_p) {	// TODO: move?
  MENU.out(F("IDENTITY\tpreName "));
  MENU.out_IstrI(this_peer_ID_p->preName);

  extern char* MAC_str(const uint8_t* mac);
  MENU.out(F("\tMAC "));
  MENU.out(MAC_str(this_peer_ID_p->mac_addr));

  MENU.out(F("\ttime slice  "));
  MENU.out((int) this_peer_ID_p->esp_now_time_slice);

  MENU.out(F("\tversion\t"));
  MENU.outln((int) this_peer_ID_p->version);
} // show_peer_id()


// #if defined USE_ESP_NOW || defined USE_NVS	// proves easier to compile that always
  peer_ID_t my_IDENTITY;

  // MAC as string
  char* MAC_str(const uint8_t* mac) {		// TODO: TEST: mac==NULL case
    if(mac == NULL)
      return (char*) ">*< ALL KNOWN >*<";	// for esp_now
    // else

    static char MACstr[18];
    snprintf(MACstr, sizeof(MACstr), "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    return MACstr;
  } // MAC_str(const uint8_t* mac)

  uint8_t my_MAC[] = {0,0,0,0,0,0};
// #endif // defined USE_ESP_NOW || defined USE_NVS


#define PEER_ID_H
#endif
