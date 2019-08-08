/*
  system_menu.h
*/

void display_type_sizes() {
  MENU.out(F("pulses_hardware_conf_t\t"));
  MENU.outln(sizeof(pulses_hardware_conf_t));

  MENU.out(F("musicBox_conf_t\t"));
  MENU.outln(sizeof(musicBox_conf_t));

  MENU.out(F("cycles_conf_t\t"));
  MENU.outln(sizeof(cycles_conf_t));

  MENU.out(F("magical_conf_t\t"));
  MENU.outln(sizeof(magical_conf_t));

  MENU.out(F("ui_conf_t\t"));
  MENU.outln(sizeof(ui_conf_t));
}

void system_menu_display() {
  //MENU.outln(F("Pulses System Menu\n"));

#if defined ESP32
  display_esp_versions();
  MENU.ln();
#endif

  display_program_version();	// display program versions, maybe preName. menu output only

  display_type_sizes();
  MENU.ln();

  MENU.outln(F("\n'P' nvs_PRENAME"));

#if defined BATTERY_LEVEL_CONTROL_PIN
  show_battery_level();
#endif

  MENU.ln();
}

bool system_menu_reaction(char token) {
  int input_value;
  String s;

  switch(token) {
  case '?':
    MENU.menu_display();
    break;

  case 'P':
    s = nvs_getString(F("nvs_PRENAME"));
    MENU.out(F("nvs_PRENAME:\t"));
    if (s)
      MENU.out(s);
    MENU.ln();
    break;

  default:
    return false;
  }

  return true;
}
