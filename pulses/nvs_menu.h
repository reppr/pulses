/*
  nvs_menu.h
*/

void nvs_menu_display() {
  MENU.outln(F("Pulses NVS Menu\n"));

  MENU.outln(F("\n'P' nvs_PRENAME"));

  MENU.ln();
}

bool nvs_menu_reaction(char token) {
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
    return false;	// not found
  }

  return true;		// found
}
