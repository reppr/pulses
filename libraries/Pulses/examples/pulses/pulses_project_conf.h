/* **************************************************************** */
/*
  project_configuration.ino

  prepare compiling for special projects like the kalimbas
  (for default just leave this file empty)
*/
/* **************************************************************** */



/* **************************************************************** */


// testing ESP32_12 or KALIMBA7 on ESP devices:
#if defined ESP32	// KALIMBA7_v2 ESP32 version
  #if true	// ESP32_12	||	KALIMBA7_v2 ?
    #define ESP32_12_v0
    #define AUTOSTART	MENU.play_KB_macro("E32 -"); selected_experiment=-1;
  #else	// KALIMBAS
    #define KALIMBA7_v2	// ESP32 version
    #define AUTOSTART	MENU.play_KB_macro("E31 -"); selected_experiment=-1;
  #endif
#elif defined ESP8266	// KALIMBA7_v1 ESP8266 nodemcu version
  #define KALIMBA7_v1	// ESP8266 nodemcu version
  #define AUTOSTART	MENU.play_KB_macro("E30 -"); selected_experiment=-1;
#endif


/* **************************************************************** */
#if defined ESP8266 || defined ESP32
  // autostart wifi?
  //#define AUTOSTART_WIFI	setup_wifi_telnet();
#endif
