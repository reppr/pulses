/*
  WiFi_stuff.ino
*/

#ifndef WiFi_STUFF_INO
#ifdef USE_WIFI_telnet_menu	// do we need WiFi at all?

#define WIFI_DEBUG	// ################ FIXME: remove ################

/* **************************************************************** */
/* **************************************************************** */

// if the harmonical connects to your wavelan as STATION these are the credentials:
// ***EITHER*** put your WLAN ssid and password in next 2 lines:
//char STA_ssid[32] = "please_set_your_WiFi_SSID";
//char password[32] = "LetItBeSTRONG";

// *** OR *** comment out above 2 lines,  put that stuff in a file and #include it:
#include "/home/dada/WiFi-credentials.h"	// let the name be <something>.h


/* **************************************************************** */
// if the harmonical acts as ACCESS POINT these are the credentials:
char AP_ssid[32] = "HARMONICAL generic";
char AP_password[32] = "open";

/* **************************************************************** */
/* **************************************************************** */

#define WIFI_DEFAULT_MODE	WIFI_AP		// WORK IN PROGRESS ################ FIXME: ################

//#define WIFI_DEFAULT_MODE	WIFI_STA	// working


//  #define AUTOSTART_WIFI	// start WiFi when booting?  ***configure the following line***
//#define AUTOSTART_WIFI	// start WiFi when booting


/* **************************************************************** */
// see: WiFi Telnet Server for Serial Monitor use - provided #1169
//      https://github.com/esp8266/Arduino/issues/1169

// strings for WiFi status
const char *str_status[]= {
  "WL_IDLE_STATUS",
  "WL_NO_SSID_AVAIL",
  "WL_SCAN_COMPLETED",
  "WL_CONNECTED",
  "WL_CONNECT_FAILED",
  "WL_CONNECTION_LOST",
  "WL_DISCONNECTED"
};


// strings for WiFi mode
const char *str_mode[]= { "WIFI_OFF", "WIFI_STA", "WIFI_AP", "WIFI_AP_STA" };


WiFiMode_t selected_wifi_mode=WIFI_DEFAULT_MODE;

bool connectWifi(WiFiMode_t selected_wifi_mode, const char* ssid, const char* password) {
  switch (selected_wifi_mode) {
  case WIFI_AP:
MENU.outln("DADA AP FIXME: ################");
    // IP 0x0707A8C0	// 192.168.7.7
    break;
  case WIFI_STA:
    break;

  default:
    MENU.outln(F("ERROR: selected_wifi_mode"));	// ################ FIXME: ################
  }

  MENU.out(F("connecting to SSID: "));
  MENU.out(ssid); MENU.tab();

  // use in case of mode problem
  WiFi.disconnect();
  // switch to selected_wifi_mode
  if (WiFi.getMode() != selected_wifi_mode) {
    WiFi.mode(selected_wifi_mode);
  }

  WiFi.begin (ssid, password);
  yield();
  if (WiFi.getMode() != selected_wifi_mode) {
    WiFi.mode(selected_wifi_mode);
    yield();
  }

#ifdef WIFI_DEBUG	// ################ FIXME: remove ################
  MENU.outln("WIFI_DEBUG remove ################");
  WiFi.printDiag(Serial);
#endif

  // 20 seconds to establish connection
  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 20000) {
    delay(100);
    MENU.out(".");
    yield();	// delay should do that, but who knows...
  }

  // Check connection
  if (WiFi.status() == WL_CONNECTED) {
    MENU.outln(" connected");

    MENU.out(F("WiFi: "));
    MENU.out(ssid); MENU.tab();
    MENU.out("IP address: ");
    MENU.IPstring(WiFi.localIP());
    MENU.tab();

    return true;	// connected :)
  }

  // failed:
  MENU.outln("\nconnect FAILED\tsee file 'WiFi_stuff.ino'");

#if defined WIFI_DEBUG
  MENU.outln("WIFI_DEBUG remove ################");
  WiFi.printDiag(Serial);	// debugging	// ################ FIXME: ################
#endif

  return false;
}  // connectWiFi(...)


// setup_wifi_telnet(?)  might be called (void) from AUTOSTART_WIFI
bool setup_wifi_telnet(WiFiMode_t selected_wifi_mode=WIFI_DEFAULT_MODE) {
  switch_wifi_mode(selected_wifi_mode);

  switch (selected_wifi_mode) {
  case WIFI_AP:
//	    if (! WiFi.softAP(AP_ssid, AP_password))
//	      return false;
    return WiFi.softAP("AP_ssid", "AP_password");
    return WiFi.softAP(AP_ssid, AP_password);
    break;

  case WIFI_STA:
    if(! connectWifi(selected_wifi_mode, STA_ssid, password))
      return false;
    break;

  default:
    MENU.outln("setup_wifi_telnet ERROR: wifi_mode");	// ################ FIXME:	################
  }

  if (WiFi.status() == WL_CONNECTED) {

    if (MENU.verbosity >= VERBOSITY_SOME)
      MENU.outln(F("starting telnet server"));

    telnet_server.begin();
    telnet_server.setNoDelay(true);

    if (MENU.verbosity >= VERBOSITY_SOME)
      WiFi_info();

    return true;

  } else {
    MENU.outln("WiFi connect FAILED.");
    WiFi_info();
  }

  return false;
}

// #include <esp_system.h>
#include "esp_wifi.h"
#include "WiFi.h"
#include "WiFiAP.h"	// softap
#include <lwip/sockets.h>
/*
int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)

*/

bool setup_AP() {
  selected_wifi_mode=WIFI_MODE_AP;	// ################ FIXME: OBSOLETE: ################
  // see:  viewtopic.php?f=13&t=1317&p=5942&hilit=c+initializer#p5942
  // ################ FIXME: VERBOSITY ################
  MENU.out(F("start WiFi AP\t"));
  MENU.out(AP_ssid);
  MENU.tab();
  MENU.outln(AP_password);

  wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&config);
  esp_wifi_set_mode(WIFI_MODE_AP);
  yield();

  wifi_config_t ap_config = { };
  strcpy((char*) ap_config.ap.ssid, AP_ssid);
  ap_config.ap.ssid_len=0;
  strcpy((char*) ap_config.ap.password, AP_password);
  ap_config.ap.channel=0;
  ap_config.ap.authmode=WIFI_AUTH_OPEN;
  ap_config.ap.ssid_hidden=0;
  ap_config.ap.max_connection=4;
  ap_config.ap.beacon_interval=100;
  esp_wifi_set_config(WIFI_IF_AP, (wifi_config_t *)&ap_config);

/*
MENU.outln(F("tcpip_adapter_set_ip_info()"));
  tcpip_adapter_ip_info_t ipInfo;
  inet_pton(AF_INET, "192.168.7.7", &ipInfo.ip);
  inet_pton(AF_INET, "192.168.7.1", &ipInfo.gw);
  inet_pton(AF_INET, "255.255.255.0", &ipInfo.netmask);
  tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_STA, &ipInfo);
*/

MENU.outln(F("esp_wifi_start()"));
  esp_wifi_start();
  yield();

MENU.outln(F("telnet_server.begin()"));
  telnet_server.begin();
  telnet_server.setNoDelay(true);
  yield();
/*
  MENU.outln(F("wifi_softap_dhcps_start()"));
  #include <apps/dhcpserver.h>
  wifi_softap_dhcps_start();
  yield();
*/

  // MENU.outln(F("WiFiClient.connect()"));
  // WiFi.connect(0x0707A8C0, 23); // 192.168.7.7
  MENU.outln(F("esp_wifi_connect()"));
  // Connect to the given host at the given port using TCP.
  // int connect(const char* host, uint16_t port)
  // int connect(IPAddress ip, uint16_t port)
  esp_wifi_connect();
  yield();
  // ################ FIXME: VERBOSITY ################
  MENU.outln(F("done"));
}


bool switch_wifi_mode(WiFiMode_t selected_wifi_mode) {
  if (WiFi.getMode() == selected_wifi_mode)
    return true;

  WiFi.mode(selected_wifi_mode);
  if (WiFi.getMode() == selected_wifi_mode)
    return true;

  // insist in case of mode problems:
  WiFi.disconnect();
  WiFi.mode(selected_wifi_mode);

  return(WiFi.getMode() == selected_wifi_mode);
}


void WiFi_scan() {
  MENU.outln(F("scanning WiFi"));

  int n = WiFi.scanNetworks();
  if (n == 0)
    MENU.outln(F("no networks found"));
  else {
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      MENU.out(i + 1);
      MENU.out(": ");
      MENU.out(WiFi.SSID(i));
      MENU.out("       \t(");
      MENU.out(WiFi.RSSI(i));
      MENU.out(")");
#ifndef ESP32	// ################ FIXME: ESP32 ################
      MENU.out((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*");
#endif
      MENU.ln();
    }
  }
}


void WiFi_info() {
  int status;

  MENU.out(F("\nwifi_info:  mode "));
  yield();
  int mode=WiFi.getMode();
  MENU.out(mode);
  MENU.tab();
  MENU.outln(str_mode[mode]);
  if (mode) {	// mode==0 would crash
    MENU.out(F("status"));
    MENU.tab();
    yield();
    status=WiFi.status();
    MENU.out(status);

    MENU.tab();
    if (status == 255)
      MENU.outln(F("(none)"));
    else {
      MENU.outln(str_status[status]);
    }

    // MENU.IPstring(WiFi.localIP()); MENU.ln();

    // IPAddress softAPIP();
    MENU.IPstring(WiFi.softAPIP()); MENU.ln();
//	MENU.outln(" port 23");

  MENU.ln();
  }

#if defined WIFI_DEBUG_REMOVED
  MENU.outln("WIFI_DEBUG ################ remove!");
  WiFi.printDiag(Serial);	// debugging	// ################ FIXME: ################
  MENU.outln("WIFI_DEBUG ################");
#endif
}


/* **************************************************************** */
// WiFi menu:

// display function for the WiFi_menu page:
void WiFi_menu_display() {
  MENU.ln();
  WiFi_info();
  MENU.ln();

  MENU.out(F("selected mode: "));
  MENU.out(str_mode[selected_wifi_mode]);
  MENU.tab();
  MENU.ln();

  MENU.outln(F(".=info A=API  b=STA  c=connect  x=disconnect  X=stop  s=scan"));
  //  MENU.outln(F(".=info a=API  b=STA  c=connect  x=disconnect  X=stop  s=scan  S=ssid  p=passwd"));
}


// reaction function for the WiFi_menu page:
bool WiFi_menu_reaction(char token) {
  switch (token) {

  case 'A':
    setup_AP();
    WiFi_info();
    break;

  case 'D':	// debugging
    WiFi.softAP("GUGUS", "dadadeli88");
    WiFi_info();
    break;

  case '.':
    WiFi_info();
    break;

  case 'a':
    selected_wifi_mode=WIFI_AP;
    switch_wifi_mode(selected_wifi_mode);
    WiFi_info();
    break;

  case 'b':
    selected_wifi_mode=WIFI_STA;
    switch_wifi_mode(selected_wifi_mode);
    WiFi_info();
    break;

  case 'c':	// connect
    setup_wifi_telnet(selected_wifi_mode);
    break;

  case 'x':	// disconnect
    WiFi.disconnect();
    if (MENU.verbosity >= VERBOSITY_SOME)
      WiFi_info();
    break;

  case 'X':	// stop!
    if (MENU.verbosity >= VERBOSITY_SOME)
      MENU.outln(F("STOP wifi"));

    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
#ifndef ESP32	// ################ FIXME: ESP32 ################
    WiFi.forceSleepBegin();
#endif

    if (MENU.verbosity >= VERBOSITY_SOME)
      WiFi_info();
    break;

  case 's':	// scan
    WiFi_scan();
    break;

  case 'S':	// enter SSID
    MENU.outln(F("not implemented yet"));	// ################ FIXME:  ################
    break;

  case 'p':	// enter password
    MENU.outln(F("not implemented yet"));	// ################ FIXME:  ################
    break;

  default:
    return false;	// token not found in this menu
  }

  return true;		// token found in this menu
}

/* **************************************************************** */
#endif // USE_WIFI_telnet_menu	// do we need WiFi at all?
#endif // WiFi_STUFF_INO

#define WiFi_STUFF_INO
