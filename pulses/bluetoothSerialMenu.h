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
  #define BLUETOOTH_NAME	ESP alpha 0.014		// the BT name of esp32
#endif


#define BLUETOOTH_ENABLE_PIN	35	// check pin to see if bluetooth is enabled

#if defined BLUETOOTH_ENABLE_PIN
  bool bluetooth_switch_() {
    bool on;
    pinMode(BLUETOOTH_ENABLE_PIN, INPUT);
    digitalRead(BLUETOOTH_ENABLE_PIN);		// i do not trust the very first reading...
    on = digitalRead(BLUETOOTH_ENABLE_PIN);	// read again
    if(MENU.verbosity >= VERBOSITY_LOWEST) {
      MENU.out(F("bluetooth is switched"));
      MENU.out_ON_off(on);
      if(on)
	MENU.tab();
      else
	MENU.ln(2);
    }
    return on;
  }
#endif

esp_bt_controller_status_t bt_status_before_sleeping = (esp_bt_controller_status_t) 0;	// debugging only

esp_bt_controller_status_t show_bt_controller_status() {
  MENU.out(F("esp_bt_controller_get_status() "));
  esp_bt_controller_status_t bt_c_status = esp_bt_controller_get_status();
  MENU.outln(bt_c_status);
  return bt_c_status;
}

bool bluetoothSerialBEGIN() {
  bool ok;

#if defined BLUETOOTH_ENABLE_PIN
  if(! bluetooth_switch_()) {	// savety net, should not happen...
    MENU.error_ln(F("BT is disabled"));
    return false;
  }
#endif

  MENU.out(F("BLUEtoothSerial.begin("));
  MENU.out(F(STRINGIFY(BLUETOOTH_NAME)));
  MENU.out(F(") "));

  ok = BLUEtoothSerial.begin(STRINGIFY(BLUETOOTH_NAME));	// Bluetooth device name
  yield();
  if(ok)
    MENU.outln(F("ok"));
  else {
    MENU.outln(F("FAILED"));
  }

  return ok;
}

void bluetooth_setup() {
  MENU.out(F("BLUETOOTH: "));
  MENU.outln(STRINGIFY(BLUETOOTH_NAME));

  /* debugging
  show_bt_controller_status();	// reboot: zero, then 2 forever...
  MENU.tab();
  */

  /*
  MENU.ok_or_error_ln(F("esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT)"), \
		      esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT));
  */

  /*	does not help anyway
  MENU.ok_or_error_ln(F("esp_bt_controller_enable(ESP_BT_MODE_BTDM)"), \
		      esp_bt_controller_enable(ESP_BT_MODE_BTDM));	// ERROR: *only* first time
  */

  /* not even that helps
  MENU.ok_or_error_ln(F("esp_bt_controller_enable((esp_bt_mode_t) 0x02)"), \
		      esp_bt_controller_enable((esp_bt_mode_t) 0x02));
  */

  bluetoothSerialBEGIN();
  yield();

  if(MENU.verbosity >= VERBOSITY_SOME)
    show_bt_controller_status();

  MENU.ln();
}