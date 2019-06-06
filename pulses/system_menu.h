/*
  system_menu.h
*/

void system_menu_display() {
  //MENU.outln(F("Pulses System Menu\n"));

#if defined ESP32
  display_esp_versions();
#endif

  display_program_version();	// display program versions, maybe prename. menu output only

  MENU.ln();
}

bool system_menu_reaction(char token) {
  int input_value;

  switch(token) {
  case '?':
    MENU.menu_display();
    break;

  default:
    return false;
  }

  return true;
}
