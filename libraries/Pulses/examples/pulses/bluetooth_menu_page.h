/*
  bluetooth_menu_page.h
*/

#include <esp_bt_main.h>
#include <esp_bt.h>

void bluetooth_menu_display() {
  MENU.outln(F("bluetooth debugging *test interface*\thttp://github.com/reppr/pulses/\n"));

  MENU.outln(STRINGIFY(BLUETOOTH_NAME));

  show_bt_status();	// reboot: zero, then 2 forever...

  MENU.outln(F("btSerial begin 'S' end 's'\tbluedroid disable 'b' enable 'B'"));
  MENU.outln(F("bt controller enable 'CI' 'CE' 'CB' 'CD'"));

  MENU.ln();
}

bool bluetooth_menu_reaction(char token) {
  switch(token) {
  case 'S':
    bluetoothSerialBEGIN();
    yield();
    break;
  case 's':
    MENU.outln(F("BLUEtoothSerial.end()"));
    BLUEtoothSerial.end();
    yield();
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
    switch (MENU.cb_peek()) {
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
//      MENU.ok_or_error_ln(F("esp_bt_controller_enable(BTDM)"), \
//			  esp_bt_controller_enable(BTDM));
//      yield();
//      break;

    default:
      MENU.restore_input_token();
      return false;
    }
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
