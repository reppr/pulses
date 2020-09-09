/*
  nvs_menu.h
*/

void nvs_menu_display() {
  MENU.out_IstrI(my_IDENTITY.preName);
  MENU.space(2);
  nvs_free_entries();
  MENU.ln();

  if (! nvs_test_key("IDENTITY_nvs"))
    MENU.out(F("\t\tsay 'IS' to create"));
  MENU.ln();

  if (! nvs_test_key("HARDWARE_nvs"))
    MENU.out(F("\t\tsay 'HS' to create"));
  MENU.ln();

  MENU.outln(F("\n\n'H'=HARDWARE 'HR'=read 'HS'=save '?'=info"));
  MENU.ln();

  MENU.outln(F("'I'=IDENTITY 'IS'=save 'IR'=read 'IT'=time slice 'IP<preName>'"));
  MENU.ln();

  MENU.outln(F("'MH'=monochrome_type heltec  'MO'=type LiPO  'M0'=type off"));
  MENU.ln();

  MENU.out(F("'S'=SYSTEM"));
#if defined USE_MPU6050
  MENU.out(F(" 'SZ'=run IMU_Zero\t'SU'=set mpu6050 offsets by hand"));
#endif
  MENU.ln();

#if defined USE_RGB_LED_STRIP
  rgb_led_string_UI_display();
  MENU.ln();
#endif

  MENU.out(F("'Axxx'=autostart KB macro"));
  if(nvs_AUTOSTART_kb_macro && strlen(nvs_AUTOSTART_kb_macro)) {
    MENU.out(F(" (\""));
    MENU.out(nvs_AUTOSTART_kb_macro);
    MENU.out(F("\")\t'D'=delete autostart"));
  }
  MENU.ln();

  MENU.ln();
} // nvs_menu_display()


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
	nvs_read_blob("IDENTITY_nvs", &ID_from_nvs, sizeof(peer_ID_t));	// read again
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

	MENU.out(F("preName\t"));
	MENU.out_IstrI(my_IDENTITY.preName);
	MENU.ln();
	break;
      } // switch next token after 'I'

      // display_ID_and_SYSTEM(&ID_from_nvs);	// TODO: factor out and use in menu display
      extern void show_peer_id(peer_ID_t* this_peer_ID_p);

      if(ID_from_nvs.version != ILLEGAL8) {
	MENU.out(F("NVS\t"));
	show_peer_id(&ID_from_nvs);
      }

      MENU.out(F("SYSTEM\t"));
      show_peer_id(&my_IDENTITY);
      MENU.ln();
      // TODO: factor out and use in menu display
    }
    break;

  case 'M': // 'Mx' monochrome type;
    if(MENU.check_next('0'))	   // 'M0' monochrome_type_off
      HARDWARE.monochrome_type = monochrome_type_off;
    else if(MENU.check_next('H'))  // 'MH' heltec
      HARDWARE.monochrome_type = monochrome_type_heltec;
    else if(MENU.check_next('L'))  // 'Ml' LiPO
      HARDWARE.monochrome_type = monochrome_type_LiPO;

    show_monochrome_type(HARDWARE.monochrome_type);
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
    case 'U':	// 'SU' set MPU offsets by hand
      MENU.drop_input_token();
      set_accGyro_offsets_UI();
      break;
    case 'Z':	// 'SZ' ':NSZ' run IMU_Zero
      MENU.drop_input_token();
      tabula_rasa();	// sound alters mpu readings...
#if defined USE_MONOCHROME_DISPLAY
      (*u8x8_p).clear();
#endif
      extended_output(F("calibrating MPU"), 0, 1, true);
      MENU.ln();
      determine_imu_zero_offsets(1000);
      extended_output(F("done say 'HS'"), 0, 3, true);
      MENU.ln();
      set_IMU_Zero_offsets();
      break;
#endif

    case EOF8:
      break;

    default:
      return false;
    } // second letter after 'S'
    break;

  case 'X':	// works, but crashes menu	// TODO: fix menu crash after nvs_clear_all_keys()
    MENU.drop_input_token();
    nvs_clear_all_keys();
    MENU.outln("DADA TODO: DEBUG CRASH");	// menu CRASH AFTER THAT
    yield();
    break;

  case 'A': // 'Axxx' nvs_autostart()
    if(MENU.peek() != EOF8) {
      size_t max = 100;
      char buffer[max];
      int len=0;
      char c=1;
      while (len < max && (c != EOF8))
	{
	  c = MENU.peek();
	  if (c != EOF8) {
	    buffer[len++] = c;
	    MENU.drop_input_token();
	  } else
	    buffer[len] = '\0';
	}
      if(! nvs_save_blob("nvs_AUTOSTART", buffer, len + 1))
	set_nvs_autostart_kb_macro(buffer);
    }
    break;

  case 'D': // 'D' delete nvs_AUTOSTART
    MENU.out(F("remove nvs_AUTOSTART\t"));
    nvs_delete_key("nvs_AUTOSTART");
    set_nvs_autostart_kb_macro(NULL);
    MENU.ln();
    break;

  default:
    return false;	// not found
  }

  return true;		// found
}
