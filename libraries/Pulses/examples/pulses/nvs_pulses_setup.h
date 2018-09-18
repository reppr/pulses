/*
  nvs_pulses_setup.h
*/

#ifndef STRINGIFY2
  // see:  https://stackoverflow.com/questions/20631922/expand-macro-inside-string-literal
  #define STRINGIFY2(X) #X
  #define STRINGIFY(X) STRINGIFY2(X)
#endif


/* Start a namespace "CONFIG"
  in Read-Write mode: set second parameter to false 
  Note: Namespace name is limited to 15 chars */
Preferences CONF_nvs;
CONF_nvs.begin("CONFIG", false);
MENU.out("nvs_NAME:\t");
MENU.outln(CONF_nvs.getString("nvs_NAME", ""));

MENU.out("nvs_GPIO:\t");
MENU.outln(CONF_nvs.getString("nvs_GPIO", ""));

MENU.out("nvs_AUTOSTART:\t");
MENU.outln(CONF_nvs.getString("nvs_AUTOSTART", ""));

MENU.out("NVS freeEntries():\t");
MENU.outln(CONF_nvs.freeEntries());

if (coded_NAME == 0)
  MENU.outln("NULL");

//CONF_nvs.putString("nvs_NAME", "Rumpelstilzchen");
//CONF_nvs.putString("nvs_NAME", (char *) *coded_NAME);
//MENU.out(*coded_NAME);

// CONF_nvs.putString("nvs_GPIO", "{32, 33, 14, 13, 23, 5, 17, 16}");

#if defined AUTOSTART
//  CONF_nvs.putString("nvs_AUTOSTART", STRINGIFY(AUTOSTART));
#endif
/*
  nvs_NAME
  nvs_GPIO
  nvs_AUTOSTART
  nvs_DATE
*/

CONF_nvs.end();
MENU.ln();
