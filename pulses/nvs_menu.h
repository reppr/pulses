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
  else
    MENU.out(F("\tsay 'XH!' to delete"));
  MENU.ln();

  MENU.outln(F("\n\n'H'=HARDWARE 'HR'=read 'HS'=save '?'=info"));
  MENU.ln();

  MENU.outln(F("'I'=IDENTITY 'IS'=save 'IR'=read 'IT'=time slice 'IP<preName>'"));
  MENU.ln();

  MENU.outln(F("'MH'=monochrome_type heltec  'MO'=type LiPO  'ML'=type lilygo eInk  'M0'=type off"));
  MENU.ln();

  MENU.outln(F("'Px' pins:  'PT'=morse Touch  'PO'=morse Output  'PP'=peripheral power  'T'=touch Threshold"));
  MENU.ln();

  MENU.out(F("'S'=SYSTEM"));
#if defined USE_MPU6050_at_ADDR
  MENU.out(F(" 'SZ'=run IMU_Zero\t'SU'=set mpu6050 offsets by hand"));
#endif
  MENU.ln();

#if defined USE_RGB_LED_STRIP
  RGB_led_string_UI_display();
  MENU.ln();
#endif

  MENU.out(F("'Axxx'=autostart KB macro"));
  if(nvs_AUTOSTART_kb_macro && strlen(nvs_AUTOSTART_kb_macro)) {
    MENU.out(F(" (\""));
    MENU.out(nvs_AUTOSTART_kb_macro);
    MENU.out(F("\")\t'XA!'=delete autostart"));
  }

  MENU.outln(F("\t'XX!'=DELETE ALL KEYS!"));

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
      extern void ERROR_ln(const char* text);
      ERROR_ln(F("say HR HS H?"));	// error feedback
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
    else if(MENU.check_next('O'))  // 'MO' LiPO
      HARDWARE.monochrome_type = monochrome_type_LiPO;
    else if(MENU.check_next('L'))  // 'ML' ePaper LILYGO  2.13" eInk display
      HARDWARE.monochrome_type = monochrome_type_LILYGO_T5;

    show_monochrome_type(HARDWARE.monochrome_type);
    break;

  case 'L':	 // 'L' RGB LED STRING
#if defined USE_RGB_LED_STRIP
    RGB_led_string_UI();
#else
    MENU.outln(F("*NO* rgb led string code"));
    MENU.skip_input();
#endif
    break;

  case 'S':	// 'S' system	in construction
    switch(next_token) { // second letter after 'S'

#if defined USE_MPU6050_at_ADDR		// MPU-6050 6d accelero/gyro
    case 'U':	// 'SU' set MPU offsets by hand
      MENU.drop_input_token();
      set_accGyro_offsets_UI();
      break;
    case 'Z':	// 'SZ' ':NSZ' run IMU_Zero
      MENU.drop_input_token();
      tabula_rasa();	// sound alters mpu readings...
#if defined HAS_DISPLAY
      monochrome_clear();
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

  case 'T':	// 'T' touch TRESHOLD
      MENU.out(F("touch THRESHOLD "));
      input_value = MENU.calculate_input(HARDWARE.touch_threshold);
      HARDWARE.touch_threshold = (uint16_t) input_value;
      MENU.outln(HARDWARE.touch_threshold);
    break;

  case 'P':	// 'Px' PINS
    if(MENU.check_next('T')) {		// 'PT' set morse touch pin
      MENU.out(F("morse touch pin "));
      input_value = MENU.numeric_input(ILLEGAL8);
      if((input_value < ILLEGAL8) && (input_value >= 0)) {
	HARDWARE.morse_touch_input_pin = input_value;
	MENU.out(F("do 'HS' and reboot "));
      }
      MENU.outln(HARDWARE.morse_touch_input_pin);

    } else if(MENU.check_next('O')) {	// 'PO' set morse output pin
      MENU.out(F("morse output pin "));
      input_value = MENU.numeric_input(ILLEGAL8);
      if((input_value < ILLEGAL8) && (input_value >= 0)) {
	HARDWARE.morse_output_pin = input_value;
	pinMode(HARDWARE.morse_output_pin, OUTPUT);
      }
      MENU.outln(HARDWARE.morse_output_pin);
    } else if(MENU.check_next('P')) {	// 'PP' set peripheral power pin
      MENU.out(F("peripheral power pin "));
      input_value = MENU.numeric_input(ILLEGAL8);
      if((input_value <= ILLEGAL8) && (input_value >= 0)) {
	HARDWARE.periph_power_switch_pin = input_value;
	pinMode(HARDWARE.periph_power_switch_pin, OUTPUT);
      }
      MENU.outln(HARDWARE.periph_power_switch_pin);
    }
    break;

  case 'X':	// must be 'XH!' or 'XA!' or 'XX!'
    if(MENU.string_match("X!")) {	// 'XX!'=delete *ALL* nvs keys
      nvs_clear_all_keys();
      yield();
    } else
      if(MENU.string_match("H!")) {	// 'XH!'=delete HARDWARE_nvs
	MENU.outln(F("delete HARDWARE_nvs"));
	nvs_delete_key("HARDWARE_nvs");
      } else
	if(MENU.string_match("A!")) {	// 'XA!=delete nvs_AUTOSTART
	  MENU.out(F("remove nvs_AUTOSTART\t"));
	  nvs_delete_key("nvs_AUTOSTART");
	  set_nvs_autostart_kb_macro(NULL);
	  MENU.ln();
	} else
	  MENU.outln_invalid();
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

  default:
    return false;	// not found
  }

  return true;		// found
} // nvs_menu_reaction()
