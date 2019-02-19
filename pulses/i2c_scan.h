#ifndef I2C_SCAN_H

#include <Wire.h>

int i2c_scan() {
  int found=0;
  int error;

  for(int adr = 1; adr < 127; adr++ ) {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the adr.
    Wire.beginTransmission(adr);
    error = Wire.endTransmission();

    if (error == 0) {
      MENU.out("I2C device at 0x");
      if (adr<16)
	MENU.out('0');
      MENU.out_hex(adr);
      MENU.outln("  !");
      found++;
    } else if (error==4) {
      MENU.out("Unknown error at address 0x");
      if (adr<16)
	MENU.out("0");
      MENU.out_hex(adr);
      MENU.ln();
    }
  }

  MENU.out(found);
  MENU.outln(F(" I2C devices found"));

  return found;
}

#define I2C_SCAN_H
#endif
