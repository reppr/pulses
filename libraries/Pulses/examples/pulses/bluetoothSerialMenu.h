/*
  bluetoothSerialMenu.h
*/

#include "BluetoothSerial.h"
#include <esp_bt.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
  #error BluetoothSerialMenu.h  bluetooth is not enabled!  please run `make menuconfig` to and enable it
#endif

//BluetoothSerial BLUEtoothSerial;

#define BLUETOOTH_NAME	alpha 0.007	// the BT name of the esp32

void bluetooth_setup() {
  MENU.out(F("BLUETOOTH: "));
  MENU.outln(STRINGIFY(BLUETOOTH_NAME));

  //
  if(esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT))
    MENU.error_ln(F("esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT)"));
  //

  /*
  if(esp_bt_controller_enable(ESP_BT_MODE_BTDM))
    MENU.error_ln(F("esp_bt_controller_enable(ESP_BT_MODE_BTDM)"));	// ERROR: *only* first time
  */

  if(BLUEtoothSerial.begin(STRINGIFY(BLUETOOTH_NAME))) //Bluetooth device name
    MENU.outln("DADA BEGIN");
  else
    MENU.outln("DADA begin");
}
