/*
  bluetoothSerialMenu.h
*/

#include "BluetoothSerial.h"
#include <esp_bt.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
  #error BluetoothSerialMenu.h  bluetooth is not enabled!  please run `make menuconfig` to and enable it
#endif

// BluetoothSerial BLUEtoothSerial;

#if defined MUSICBOX_VERSION
  #define BLUETOOTH_NAME	MUSICBOX_VERSION	// the BT name of esp32
#else
  #define BLUETOOTH_NAME	ESP 0.007		// the BT name of esp32
#endif

esp_bt_controller_status_t bt_status_before_sleeping = (esp_bt_controller_status_t) 0;

esp_bt_controller_status_t show_bt_status() {
  MENU.out(F("esp_bt_controller_get_status() "));
  esp_bt_controller_status_t bt_c_status = esp_bt_controller_get_status();
  MENU.outln(bt_c_status);
  return bt_c_status;
}

void bluetooth_setup() {
  MENU.out(F("BLUETOOTH: "));
  MENU.outln(STRINGIFY(BLUETOOTH_NAME));

  /*
  if(bt_status_before_sleeping) {
    MENU.ok_or_error_ln(F("esp_bt_controller_enable(bt_status_before_sleeping)"), \
			esp_bt_controller_enable(bt_status_before_sleeping));
  }
  */

  /*
    MENU.ok_or_error_ln(F("esp_bt_controller_enable(ESP_BT_MODE_BTDM)"), \
		      esp_bt_controller_enable(ESP_BT_MODE_BTDM));
  */

  /*	################ comment?
  MENU.ok_or_error_ln(F("esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT)"), \
		      esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT));
  */

  /* not even that helps
  MENU.ok_or_error_ln(F("esp_bt_controller_enable((esp_bt_mode_t) 0x02)"), \
		      esp_bt_controller_enable((esp_bt_mode_t) 0x02));
  */

  /*	does not help anyway
  MENU.ok_or_error_ln(F("esp_bt_controller_enable(ESP_BT_MODE_BTDM)"), \
		      esp_bt_controller_enable(ESP_BT_MODE_BTDM));	// ERROR: *only* first time
  */

  MENU.out(F("BLUEtoothSerial.begin("));
  MENU.out(F(STRINGIFY(BLUETOOTH_NAME)));
  MENU.out(F(") "));
  if(BLUEtoothSerial.begin(STRINGIFY(BLUETOOTH_NAME))) // Bluetooth device name
    MENU.outln(F("ok"));
  else {
    MENU.outln(F("FAILED"));
  }

  show_bt_status();
  MENU.ln();
}
