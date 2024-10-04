/*
  bluetoothSerialMenu.h
*/

#include "BluetoothSerial.h"
#include <esp_bt.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
  #error BluetoothSerialMenu.h  bluetooth is not enabled!  please run `make menuconfig` and enable it
#endif

String bluetooth_name;

//#define BLUETOOTH_ENABLE_PIN	35	// check pin to see if bluetooth is enabled?  used by some OLD instruments, probably OBSOLETE

#if defined BLUETOOTH_ENABLE_PIN
  bool bluetooth_switch_info() {
    bool on;
    pinMode(HARDWARE.bluetooth_enable_pin, INPUT);
    digitalRead(HARDWARE.bluetooth_enable_pin);		// i do not trust the very first reading...
    on = digitalRead(HARDWARE.bluetooth_enable_pin);	// read again
    if(MENU.verbosity >= VERBOSITY_LOWEST) {
      MENU.out(F("bluetooth "));
      MENU.out(HARDWARE.bluetooth_enable_pin);
      MENU.out(F(" is switched"));
      MENU.out_ON_off(on);
      if(on)
	MENU.tab();
      else
	MENU.ln();
    }
    return on;
  }
#endif

esp_bt_controller_status_t bt_status_before_sleeping = (esp_bt_controller_status_t) 0;	// debugging only

esp_bt_controller_status_t show_bt_controller_status() {
  MENU.out(F("esp_bt_controller_get_status() "));
  esp_bt_controller_status_t bt_c_status = esp_bt_controller_get_status();
  switch(bt_c_status) {
  case ESP_BT_CONTROLLER_STATUS_IDLE:
    MENU.outln(F("IDLE"));
    break;
  case ESP_BT_CONTROLLER_STATUS_INITED:
    MENU.outln(F("INITED"));
    break;
  case ESP_BT_CONTROLLER_STATUS_ENABLED:
    MENU.outln(F("ENABLED"));
    break;
  default:
    MENU.outln_invalid();
  }
  return bt_c_status;
} // show_bt_controller_status()

bool bluetoothSerialBEGIN() {
  bool ok;

#if defined BLUETOOTH_ENABLE_PIN
  if(! bluetooth_switch_info()) {	// savety net, should not happen...
    ERROR_ln(F("BT is disabled"));
    return false;
  }
#endif

  MENU.out(F("BLUEtoothSerial.begin("));
  MENU.out(bluetooth_name);
  MENU.out(F(") "));

  ok = BLUEtoothSerial.begin(bluetooth_name);	// Bluetooth device name
  yield();
  if(ok)
    MENU.outln(F("ok"));
  else {
    MENU.outln(F("FAILED"));
  }

  return ok;
}

void bluetooth_setup() {			// start bluetooth
  MENU.out(F("BLUETOOTH: "));

    // get correct bluetooth_name
  if(my_IDENTITY.preName)	// TODO: TEST: ################
    bluetooth_name = my_IDENTITY.preName;
  else
#if defined PROGRAM_VERSION
    bluetooth_name = STRINGIFY(PROGRAM_VERSION);
#else
    bluetooth_name = "ESP32 alpha";
#endif


  MENU.outln(bluetooth_name);

  /* debugging		//	FIXME: TODO: *FAILED BLUETOOTH WAKEUP AFTER light_sleep()*
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
} // bluetooth_setup()

void bluetooth_end() {				// end bluetooth
  if(MENU.verbosity >= VERBOSITY_SOME)
    MENU.out(F("BT end"));
  BLUEtoothSerial.end();
  // yield();

  if(MENU.verbosity >= VERBOSITY_SOME) {
    show_bt_controller_status();
    MENU.ln();
  }
}

#if defined BLUETOOTH_ENABLE_PIN
void set_bluetooth_according_switch() {
  pinMode(HARDWARE.bluetooth_enable_pin, INPUT);
  digitalRead(HARDWARE.bluetooth_enable_pin);		// don't trust the very first reading...
  if(digitalRead(HARDWARE.bluetooth_enable_pin))	// read again
    bluetoothSerialBEGIN();				// START bluetooth
  else							//   or
    bluetooth_end();					// END bluetooth
}
#endif
