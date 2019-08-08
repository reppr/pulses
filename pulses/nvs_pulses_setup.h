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

#if defined NVS_PUT_PRENAME_HACK	// 1 shot HACK to put a PRENAME		TODO: UI
  MENU.out(F("NVS_PUT_PRENAME_HACK: "));
  MENU.out(STRINGIFY(NVS_PUT_PRENAME_HACK));
  MENU.tab();
  // avoid writing the same string over and over		// TODO: needs testing
  if(CONF_nvs.getString("nvs_PRENAME") != STRINGIFY(NVS_PUT_PRENAME_HACK)) {
    CONF_nvs.putString("nvs_PRENAME", STRINGIFY(NVS_PUT_PRENAME_HACK));
    MENU.outln(F("ok"));
   } else
    MENU.outln(F("was already stored"));
#endif

MENU.out("nvs_PRENAME:\t");
{
  String name = CONF_nvs.getString("nvs_PRENAME");
  if(name) {
    my_ID.preName = name;
    MENU.out(my_ID.preName);
    // bluetooth_name?
  }
  MENU.ln();
}

/*
MENU.out("nvs_GPIO:\t");
MENU.outln(CONF_nvs.getString("nvs_GPIO"));

MENU.out("nvs_AUDIO:\t");
MENU.outln(CONF_nvs.getString("nvs_AUDIO"));

MENU.out("nvs_MORSE:\t");
MENU.outln(CONF_nvs.getString("nvs_MORSE"));

MENU.out("nvs_AUTOSTART:\t");
MENU.outln(CONF_nvs.getString("nvs_AUTOSTART"));
*/

MENU.out("nvs freeEntries():\t");
MENU.outln(CONF_nvs.freeEntries());

/*
  nvs_DATE
*/

//if (coded_NAME == 0)	// ???, remove
//  MENU.outln("NULL");

//CONF_nvs.putString("nvs_NAME", "Rumpelstilzchen");
//CONF_nvs.putString("nvs_NAME", (char *) *coded_NAME);
//MENU.out(*coded_NAME);

// CONF_nvs.putString("nvs_GPIO", "{32, 33, 14, 13, 23, 5, 17, 16}");

// MORSE_TOUCH_INPUT_PIN,  MORSE_GPIO_INPUT_PIN, MORSE_OUTPUT_PIN
// CONF_nvs.putString("nvs_MORSE", "mTI=13 mGI=34 mO=2");


#if defined AUTOSTART
//  CONF_nvs.putString("nvs_AUTOSTART", STRINGIFY(AUTOSTART));
#endif

CONF_nvs.end();
MENU.ln();
