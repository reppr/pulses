/*
  nvs_menu.h
*/

void store_early_hardware_conf() {
  pulses_hardware_conf_t hardware;

#if defined USE_RGB_LED_STRIP
  hardware.rgb_strings = 1;
  hardware.rgb_led_cnt[0] = RGB_STRING_LED_CNT;
  hardware.rgb_pin[0] = RGB_LED_STRIP_DATA_PIN;
#endif

#if defined USE_MPU6050
  hardware.mpu6050_addr = 0x68;
#endif

}

void nvs_menu_display() {
  MENU.outln(F("Pulses NVS Menu\n"));

  MENU.out("free entries:\t");
  MENU.outln(nvs_free_entries());
  MENU.ln();

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
