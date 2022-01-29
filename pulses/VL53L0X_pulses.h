/*
  VL53L0X_pulses.h
*/

#if ! defined VL53L0X_H

#include "VL53L0X.h"
VL53L0X VL53L0X_1;

bool VL53lox_usable=false;


int16_t VL53L0X_read_mm() {	// negative values ERROR	-1 out of range 	-2 not initialized
  if(! VL53lox_usable)
    return -2;		// ERROR -2 not initialized

  int16_t mm = VL53L0X_1.readRangeSingleMillimeters();
  if(VL53L0X_1.timeoutOccurred()) {
    MENU.out(mm);
    ERROR_ln(" VL53L0X timeout");
    mm = -3;	// mm = -1;
  }

  return mm;
} // VL53L0X_read_mm()


void set_VL53L0X_long_range() {
  // lower the return signal rate limit (default is 0.25 MCPS)
  VL53L0X_1.setSignalRateLimit(0.1);
  // increase laser pulse periods (defaults are 14 and 10 PCLKs)
  VL53L0X_1.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
  VL53L0X_1.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);
} // set_VL53L0X_long_range()

bool set_VL53L0X_high_speed() {
  // reduce timing budget to 20 ms (default is about 33 ms)
  return VL53L0X_1.setMeasurementTimingBudget(20000);
} // set_VL53L0X_high_speed()

bool set_VL53L0X_high_accuracy() {
  // increase timing budget to 200 ms
  return VL53L0X_1.setMeasurementTimingBudget(200000);
} // set_VL53L0X_high_accuracy()


void setup_VL53L0X() {
  MENU.out(F("\n################ VL53L0X init\t"));
  VL53lox_usable=false;

  VL53L0X_1.setTimeout(500);
  if(VL53L0X_1.init()) {	// ok
    VL53lox_usable=true;
    MENU.out(F("ok\tmm "));
    //VL53L0X_1.setMeasurementTimingBudget(20000);
    MENU.outln(VL53L0X_read_mm());
    return;			// OK :)
  } // else

  ERROR_ln(F("VL53L0X_1.begin()"));	// ERROR
} // setup_VL53L0X()

#define VL53L0X_H
#endif
