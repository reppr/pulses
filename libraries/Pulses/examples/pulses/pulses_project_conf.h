/* **************************************************************** */
/*
  project_configuration.ino

  prepare compiling for special projects like the kalimbas
  (for default just leave this file empty)
*/
/* **************************************************************** */


/* **************************************************************** */
// testing KALIMBA7 on ESP devices:
#if defined ESP32	// KALIMBA7_v2 ESP32 version
  //  #define KALIMBA7_v0	// ESP32 version, obsolete
  #define KALIMBA7_v2	// ESP32 version
  #define AUTOSTART	MENU.play_KB_macro("E31 -"); selected_experiment=-1;

#elif defined ESP8266	// KALIMBA7_v1 ESP8266 nodemcu version
  #define KALIMBA7_v1	// ESP8266 nodemcu version
  #define AUTOSTART	MENU.play_KB_macro("E30 -"); selected_experiment=-1;
#endif
