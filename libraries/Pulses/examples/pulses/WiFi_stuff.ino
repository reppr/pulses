/*
  WiFi_stuff.ino
*/

#ifndef WiFi_STUFF_INO
#ifdef USE_WIFI_telnet_menu	// do we need WiFi at all?


/* **************************************************************** */
/* **************************************************************** */

// ***EITHER*** put your WLAN ssid and password in next 2 lines:
char ssid[32] = "please_set_your_WiFi_SSID";
char password[32] = "LetItBeSTRONG";

// *** OR *** comment out above 2 lines,  put that stuff in a file and #include it:
//#include "/home/you/WiFi-credentials.h"	// let the name be <something>.h

/* **************************************************************** */
/* **************************************************************** */

  #define AUTO_CONNECT_WIFI			// start WiFi on booting?


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


bool connectWifi() {
  MENU.out("connecting WiFi to SSID: ");
  MENU.out(ssid); MENU.tab();

  // use in case of mode problem
  WiFi.disconnect();
  // switch to Station mode
  if (WiFi.getMode() != WIFI_STA) {
    WiFi.mode(WIFI_STA);
  }

  WiFi.begin (ssid, password);
  // WiFi.printDiag(Serial);	// debugging

  // 10 seconds to establish connection
  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000) {
    delay(100);
    #if defined(ESP32) || defined(ESP8266)
      yield();	// delay should do that, but who knows...
    #endif
    MENU.out(".");
  }

  // Check connection
  if (WiFi.status() == WL_CONNECTED) {
    MENU.outln(" connected");
    MENU.out("IP address: ");
    MENU.IPstring(WiFi.localIP());
    MENU.tab();
    return true;	// connected :)
  }

  // failed:
  MENU.outln("\nconnect FAILED\tsee file 'WiFi_stuff.ino'");

  return false;
}  // connectWiFi()


bool setup_wifi_telnet() {
  if(! connectWifi())
    return false;

  if (WiFi.status() == WL_CONNECTED) {
    MENU.out("WiFi mode: ");
    MENU.out(str_mode[WiFi.getMode()]);
    MENU.out("\tstatus: " );
    MENU.outln (str_status[WiFi.status()]);

    telnet_server.begin();
    telnet_server.setNoDelay(true);
    MENU.out("\nWLAN MENU  telnet ");
    MENU.IPstring(WiFi.localIP());
    MENU.outln(" port 23");

    return true;
  } else
    MENU.outln("WiFi connect FAILED.");
  return false;
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
      MENU.outln((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*");
    }
  }
}


/* **************************************************************** */
// WiFi menu:
// display function for the WiFi_menu page:
void WiFi_menu_display() {
  MENU.outln(F("WiFi menu"));

  MENU.out(F("S=ssid  P=passwd  c=connect  x=disconnect  X=stop  s=scan"));

  //  WiFi_info();
}


// reaction function for the WiFi_menu page:
bool WiFi_menu_reaction(char token) {
  switch (token) {
  case 'S':	// enter SSID
    break;

  case 'P':	// enter password
    break;

  case 'c':	// connect
    setup_wifi_telnet();

    if (MENU.verbosity >= VERBOSITY_SOME)
      ;
//      WiFi_info();
    break;

  case 'x':	// disconnect
    WiFi.disconnect();
    break;

  case 'X':	// stop
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
    WiFi.forceSleepBegin();
    break;

  case 's':	// scan
    WiFi_scan();
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
