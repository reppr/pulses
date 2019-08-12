/*
  nvs_menu.h
*/

void nvs_menu_display() {
  MENU.outln(F("Pulses NVS Menu\n"));

  MENU.out(F("free entries:\t"));
  MENU.outln(nvs_free_entries());
  MENU.ln();

  MENU.out(F("'H'=HARDWARE 'HR'=read 'HS'=save"));
  MENU.ln();

  MENU.outln(F("\n'P' nvs_PRENAME"));

  MENU.ln();
}


bool nvs_menu_reaction(char token) {
  int input_value;
  String s;
  char next_token=MENU.peek();

  switch(token) {
  case '?':
    MENU.menu_display();
    break;

  case 'H':
    MENU.out(F("HARDWARE\t"));
    switch(next_token) {	// next token after 'H'
    case '?':	// 'H?'
      MENU.drop_input_token();	// continue...
    case EOF:	// bare 'H', 'H?'
      // info
      break;

    case 'R':
      MENU.drop_input_token();
      {
	pulses_hardware_conf_t hardware_from_nvs;
	nvs_read_blob("HARDWARE_nvs", &hardware_from_nvs, sizeof(pulses_hardware_conf_t));
	show_hardware_conf(&hardware_from_nvs);
      }
      show_hardware_conf(&HARDWARE_Conf);
      break;

    case 'S':
      MENU.drop_input_token();
      nvs_save_blob("HARDWARE_nvs", &HARDWARE_Conf, sizeof(pulses_hardware_conf_t));
      break;

    default:
      // info DADA
      return false;
    } // switch(next_token) after 'H'

    MENU.ln();
    break;

  case 'P':
    s = nvs_getString(F("nvs_PRENAME"));
    MENU.out(F("nvs_PRENAME:\t"));
    if (s)
      MENU.out(s);
    MENU.ln();
    break;

  case 'X':
    MENU.drop_input_token();
    nvs_clear_all_keys();
    MENU.outln("DADA TODO: DEBUG CRASH");	// menu CRASH AFTER THAT
    yield();
    break;

  default:
    return false;	// not found
  }

  return true;		// found
}
