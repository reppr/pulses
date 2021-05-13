/*
  system_menu.h
*/

void display_type_sizes() {
  MENU.out(F("pulses_hardware_conf_t\t"));
  MENU.outln(sizeof(pulses_hardware_conf_t));

#if defined USE_BATTERY_LEVEL_CONTROL
  MENU.out(F("battery_levels_conf_t\t"));
  MENU.outln(sizeof(battery_levels_conf_t));
#endif

  MENU.out(F("musicBox_conf_t\t"));
  MENU.outln(sizeof(musicBox_conf_t));

  MENU.out(F("cycles_conf_t\t"));
  MENU.outln(sizeof(cycles_conf_t));

  MENU.out(F("magical_conf_t\t"));
  MENU.outln(sizeof(magical_conf_t));

  MENU.out(F("ui_conf_t\t"));
  MENU.outln(sizeof(ui_conf_t));

#if defined USE_ESP_NOW
  MENU.out(F("peer_ID_t\t"));
  MENU.outln(sizeof(peer_ID_t));
#endif

#if defined USE_LORA
  MENU.out(F("pulses_LoRa_conf_t\t"));
  MENU.outln(sizeof(pulses_LoRa_conf_t));
#endif

  MENU.out(F("pulse_t\t\t"));
  MENU.outln(sizeof(pulse_t));

  MENU.out(F("pulse_time_t\t"));
  MENU.outln(sizeof(pulse_time_t));

} // display_type_sizes()

void cpp_info_display() {
//  bool dummybool[8];
//  MENU.out(F("sizeof(bool[8])\t"));	MENU.outln(sizeof(dummybool));

  MENU.out(F("\ncpp_info:  sizeof(int*) "));
  MENU.outln(sizeof(int*));

  MENU.out(F("(char) "));
  MENU.out(sizeof(char));

  MENU.out(F("\t(short) "));
  MENU.out(sizeof(short));

  MENU.out(F("\t(int) "));
  MENU.out(sizeof(int));

  MENU.out(F("\t\t(long) "));
  MENU.out(sizeof(long));

  MENU.out(F("\t(int64_t) "));
  MENU.outln(sizeof(int64_t));

//  MENU.out(F("\t(void*) "));
//  MENU.outln(sizeof(void*));

  MENU.out(F("(float) "));
  MENU.out(sizeof(float));

  MENU.out(F("\t(double) "));
  MENU.out(sizeof(double));

  MENU.out(F("\t(long double) "));
  MENU.outln(sizeof(long double));
} // cpp_info_display()


void system_menu_display() {
  //MENU.outln(F("Pulses System Menu\n"));

#if defined ESP32
  display_esp_versions();
  MENU.ln();
#endif

  display_program_version();	// display program versions, maybe preName. menu output only
  MENU.ln();

  display_type_sizes();		// sizeof pulses data types
  MENU.ln();

  cpp_info_display();		// sizeof compiler specific types
  MENU.ln();

#if defined USE_BATTERY_LEVEL_CONTROL
  MENU.ln();
  extern void battery_UI_display();
  battery_UI_display();
  MENU.ln();
#endif
} // system_menu_display()


bool system_menu_reaction(char token) {
  int input_value;
  String s;

  switch(token) {
  case '?':
    MENU.menu_display();
    break;

#if defined USE_BATTERY_LEVEL_CONTROL
  case 'B':
    extern bool battery_UI_reaction();
    battery_UI_reaction();		// 'B' was already received, always returns true
    break;
#endif

  default:
    return false;
  }

  return true;
} // system_menu_reaction()
