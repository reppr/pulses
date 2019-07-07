/*
  system_menu.h
*/

void system_menu_display() {
  //MENU.outln(F("Pulses System Menu\n"));

#if defined ESP32
  display_esp_versions();
  MENU.ln();
#endif

  display_program_version();	// display program versions, maybe preName. menu output only

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
