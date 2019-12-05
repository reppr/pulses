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

  MENU.outln(F("'I'=IDENTITY 'IS'=save 'IR'=read 'IT'=time slice 'IP<preName>'"));
  MENU.ln();

  MENU.out(F("'S'=SYSTEM"));

#if defined USE_MPU6050
  MENU.out(F(" 'SU'=set mpu6050 offsets"));
#endif
  MENU.ln();

#if defined USE_RGB_LED_STRIP
  rgb_led_string_UI_display();
#endif

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

  case 'I': //	'I' identity my_IDENTITY
    MENU.outln(F("\nIDENTITY"));
    {
      peer_ID_t ID_from_nvs;
      ID_from_nvs.version=ILLEGAL8;
      nvs_read_blob("IDENTITY_nvs", &ID_from_nvs, sizeof(peer_ID_t));	// always read on 'I...'

      switch(MENU.peek()) { // 'I...' second letter after 'I'
      case 'S':	// 'IS' nvs save identity
	nvs_save_blob("IDENTITY_nvs", &my_IDENTITY, sizeof(peer_ID_t));
	// read back
	ID_from_nvs.version=ILLEGAL8;
	nvs_read_blob("IDENTITY_nvs", &ID_from_nvs, sizeof(peer_ID_t));	// always read on 'I...'
      case 'R':	// 'IR' nvs read identity, noop as we read always
	MENU.drop_input_token(); // drop 'S' 'R'
      case EOF8:
	break;

      case 'T':	// 'IT' time slice
	MENU.drop_input_token();
	if(MENU.is_numeric()) {	// 'IT<numeric>'
	  input_value = MENU.numeric_input(-1);
	  if(input_value >= -1 && input_value <= 256)	// -1 aka 'ILLEGAL8' deactivates time slice
	    my_IDENTITY.esp_now_time_slice = input_value;
	  else
	    MENU.outln_invalid();
	}
	MENU.out(F("esp-now time slice\t"));	// obsolete, see below
	MENU.outln(my_IDENTITY.esp_now_time_slice);
	break;

      case 'P':	// 'IP<preName>' preName
	MENU.drop_input_token();
	int b;
	char c;
	for(b=0; b<16; b++) {
	  if(MENU.peek() == EOF8) {
	    my_IDENTITY.preName[b] = '\0';
	    break;
	  }
	  c = MENU.drop_input_token();
	  my_IDENTITY.preName[b] = c;
	}
	my_IDENTITY.preName[15] = '\0';	// security net

	MENU.out(F("preName\t|"));
	MENU.out(my_IDENTITY.preName);
	MENU.outln('|');
	break;
      } // switch next token after 'I'

      extern void show_peer_id(peer_ID_t* this_peer_ID_p);

      if(ID_from_nvs.version != ILLEGAL8) {
	MENU.out(F("NVS\t"));
	show_peer_id(&ID_from_nvs);
      }

      MENU.out(F("SYSTEM\t"));
      show_peer_id(&my_IDENTITY);
      MENU.ln();
    }
    break;

  case 'L':	 // 'L' RGB LED STRING
#if defined USE_RGB_LED_STRIP
    rgb_led_string_UI();
#else
    MENU.outln(F("*NO* rgb led string code"));
#endif
    break;

  case 'S':	// 'S' system	in construction
    switch(next_token) { // second letter after 'S'

#if defined USE_MPU6050		// MPU-6050 6d accelero/gyro
    case 'U':	// 'SU'
      MENU.drop_input_token();
      set_accGyro_offsets_UI();
      break;
#endif

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
