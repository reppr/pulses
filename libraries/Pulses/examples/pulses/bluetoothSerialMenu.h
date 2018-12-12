/*
  bluetoothSerialMenu.h
*/

#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
  #error BluetoothSerialMenu.h  bluetooth is not enabled!  please run `make menuconfig` to and enable it
#endif

BluetoothSerial BluetoothSerial;

#define BLUETOOTH_NAME	TimePort	// the BT name of the esp32

void bluetooth_setup() {
  MENU.out(F("Bluetooth "));
  MENU.outln(STRINGIFY(BLUETOOTH_NAME));
  BluetoothSerial.begin("TimePort"); //Bluetooth device name
  //  BluetoothSerial.begin(STRINGIFY(BLUETOOTH_NAME)); //Bluetooth device name
}
