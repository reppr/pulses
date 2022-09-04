/*
  bluetooth_menu_page.h
*/

#include <esp_bt_main.h>
#include <esp_bt.h>

void bluetooth_menu_display() {
  MENU.outln(F("simple bluetooth debugging  *test interface*\thttp://github.com/reppr/pulses/\n"));

  MENU.outln(STRINGIFY(BLUETOOTH_NAME));
  MENU.outln(bluetooth_name);

  #if defined BLUETOOTH_ENABLE_PIN
    MENU.out(F("bluetooth enable pin "));
    MENU.outln(HARDWARE.bluetooth_enable_pin);
    bluetooth_switch_info();
  #endif

  MENU.out(F("BLUEtoothSerial.hasClient() "));
  if(BLUEtoothSerial.hasClient())
    MENU.outln(F("YES"));
  else
    MENU.outln(F("no"));

  show_bt_controller_status();	// reboot: zero, then 2 forever...
  MENU.ln();

  MENU.outln(F("btSerial begin 'S' end 's' flush 'f'	bluedroid disable 'b' enable 'B'"));
  MENU.outln(F("bt controller enable 'CI' 'CE' 'CB' 'CD'"));

  MENU.ln();
}

bool bluetooth_menu_reaction(char token) {
  switch(token) {
  case '?':	// hide pulses '?' and restore common reaction
    MENU.menu_display();

  case 'S':
    bluetoothSerialBEGIN();
    yield();
    break;
  case 's':
    MENU.outln(F("BLUEtoothSerial.end()"));
    BLUEtoothSerial.end();
    yield();
    break;
  case 'f':	// flush();
    BLUEtoothSerial.flush();
    break;

  case 'b':
    MENU.ok_or_error_ln(F("esp_bluedroid_disable()"), esp_bluedroid_disable());
    yield();
    break;
  case 'B':
    MENU.ok_or_error_ln(F("esp_bluedroid_enable()"), esp_bluedroid_enable());
    yield();
    break;

  case 'C':
    switch (MENU.peek()) {
    case 'I':
      MENU.drop_input_token();
      MENU.ok_or_error_ln(F("esp_bt_controller_enable(ESP_BT_MODE_IDLE)"), \
			  esp_bt_controller_enable(ESP_BT_MODE_IDLE));
      yield();
      break;

    case 'E':
      MENU.drop_input_token();
      MENU.ok_or_error_ln(F("esp_bt_controller_enable(ESP_BT_MODE_BLE)"), \
			  esp_bt_controller_enable(ESP_BT_MODE_BLE));
      yield();
      break;

    case 'B':
      MENU.drop_input_token();
      MENU.ok_or_error_ln(F("esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT)"), \
			  esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT));
      yield();
      break;

    case 'D':
      MENU.drop_input_token();
      MENU.ok_or_error_ln(F("esp_bt_controller_enable(ESP_BT_MODE_BTDM)"), \
			  esp_bt_controller_enable(ESP_BT_MODE_BTDM));
      yield();
      break;

//    case '+':
//      MENU.drop_input_token();
//      MENU.ok_or_error_ln(F("esp_bt_controller_enable(BTDM)"), esp_bt_controller_enable(BTDM));
//      yield();
//      break;

    default:
      MENU.restore_input_token();
      return false;
    } // Cx compound commands, switch(x) second lettere
    break;

  default:
    return false;
  }

  return true;
}

/*
  MENU.ok_or_error_ln(F("esp_bt_controller_enable((esp_bt_mode_t) 0x02)"), \
		      esp_bt_controller_enable((esp_bt_mode_t) 0x02));
*/
