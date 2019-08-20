/*
  nvs_menu.h
*/

void nvs_menu_display() {
  MENU.outln(F("Pulses NVS Menu\n"));

  MENU.out(F("free entries:\t"));
  MENU.outln(nvs_free_entries());
  MENU.ln();

  MENU.outln(F("'H'=HARDWARE 'HR'=read 'HS'=save '?'=info"));
  MENU.ln();

  MENU.outln(F("'I'=IDENTITY 'IS'=save 'IR'=read 'IT'=time slice"));
  MENU.ln();

  MENU.outln(F("'S'=SYSTEM 'SS'=set rgb string cnt"));

  MENU.outln(F("\n'P' nvs_PRENAME"));

  MENU.ln();
}


bool nvs_menu_reaction(char token) {
  int input_value;
  char next_token=MENU.peek();

  switch(token) {
  case '?':
    MENU.menu_display();
    break;

  case 'H':
    MENU.out(F("HARDWARE\t"));
    switch(next_token) {	// next token after 'H'
    case 'R':	// 'HR'	= read hardware and info nvs hardware and engine hardware
    case '?':	// 'H?'	= read hardware and info nvs hardware and engine hardware
      MENU.drop_input_token();	// continue...

    case ILLEGAL8:	// bare 'H' = read hardware and info nvs hardware and engine hardware
      nvs_show_HW_both();
      break;

    case 'S':	// 'HS' save HARDWARE_nvs blob, read back, compare
      MENU.drop_input_token();
      nvs_save_blob("HARDWARE_nvs", &HARDWARE, sizeof(pulses_hardware_conf_t));

      MENU.ln();
      nvs_show_HW_both();
      //show_current_hardware_conf();
      break;

    default:
      MENU.error_ln(F("say HR HS H?"));	// error feedback
      return false;
    } // switch(next_token) after 'H'

    MENU.ln();
    break;

  case 'I': //	'I' identity my_ID
    MENU.outln(F("\nIDENTITY"));
    {
      peer_ID_t ID_from_nvs;
      ID_from_nvs.version=ILLEGAL8;
      nvs_read_blob("IDENTITY_nvs", &ID_from_nvs, sizeof(peer_ID_t));	// always read on 'I...'

      switch(MENU.peek()) { // 'I...' second letter after 'I'
      case 'S':	// 'IS' nvs save identity
	nvs_save_blob("IDENTITY_nvs", &my_ID, sizeof(peer_ID_t));
	// read back
	ID_from_nvs.version=ILLEGAL8;
	nvs_read_blob("IDENTITY_nvs", &ID_from_nvs, sizeof(peer_ID_t));	// always read on 'I...'
      case 'R':	// 'IR' nvs read identity, noop as we read always
	MENU.drop_input_token(); // drop 'S' 'R'
      case EOF8:
	break;
      case 'T':	// 'IT'
	MENU.drop_input_token();
	if(MENU.is_numeric()) {	// 'IT<numeric>'
	  input_value = MENU.numeric_input(-1);
	  if(input_value > 0 && input_value < 256)
	    my_ID.esp_now_time_slice = input_value;
	  else
	    MENU.outln_invalid();
	}
	MENU.out(F("esp-now time slice\t"));	// obsolete, see below
	MENU.outln(my_ID.esp_now_time_slice);
	break;
      } // switch next token after 'I'

      if(ID_from_nvs.version != ILLEGAL8) {
	MENU.out(F("NVS\t"));
	show_peer_id(&ID_from_nvs);
      }

      MENU.out(F("SYSTEM\t"));
      show_peer_id(&my_ID);
      MENU.ln();
    }
    break;

  case 'S':	// 'S' system	in construction
    switch(next_token) { // second letter after 'S'
    case 'S':	// 'SS'
      MENU.drop_input_token();
      input_value = MENU.numeric_input(HARDWARE.rgb_strings);
      if((input_value >= 0) && (input_value < RGB_STRINGS_MAX))
	HARDWARE.rgb_strings = input_value;

      MENU.out(F("LED strings "));
      MENU.outln(HARDWARE.rgb_strings);

      rgb_strings_available = (HARDWARE.rgb_strings > 0); // false, if there's no string
      MENU.out(F("LED strings switched"));
      MENU.out_ON_off(rgb_strings_available);
      MENU.ln();
      break;

    case EOF8:
      break;

    default:
      return false;
    } // second letter after 'S'
    break;

  case 'P':
    {
      String s="";
      s = nvs_getString(F("nvs_PRENAME"));
      MENU.out(F("nvs_PRENAME:\t"));
      if (s)
	MENU.out(s);
      MENU.ln();
    }
    break;

  case 'X':	// works, but crashes menu	// TODO: fix menu crash after nvs_clear_all_keys()
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
