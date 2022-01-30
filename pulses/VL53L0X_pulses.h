/*
  VL53L0X_pulses.h
*/

#define VL53L0X_DEBUG

#if ! defined VL53L0X_H

#include "VL53L0X.h"

VL53L0X* vl53l0x_p[USE_VL53L0X_max] = {NULL} ;

VL53L0X VL53L0X_1;
#if (USE_VL53L0X_max > 1)
VL53L0X VL53L0X_2;
#if (USE_VL53L0X_max > 2)
VL53L0X VL53L0X_3;
#if (USE_VL53L0X_max > 3)
VL53L0X VL53L0X_4;
#if (USE_VL53L0X_max > 4)
  #error 'USE_VL53L0X_max too many'
#endif
#endif
#endif
#endif

bool VL53lox_usable=false;


int16_t VL53L0X_read_mm(VL53L0X* L0X) {	// negative values ERROR	-1 out of range 	-2 not initialized
  if(! VL53lox_usable)
    return -2;		// ERROR -2 not initialized

  int16_t mm = (*L0X).readRangeSingleMillimeters();
  if((*L0X).timeoutOccurred()) {
    MENU.out(mm);
    //ERROR_ln(" VL53L0X timeout");
    MENU.error_ln(" VL53L0X timeout");
    mm = -3;	// mm = -1;
  }

  return mm;
} // VL53L0X_read_mm()


void set_VL53L0X_long_range(VL53L0X* L0X) {
  // lower the return signal rate limit (default is 0.25 MCPS)
  (*L0X).setSignalRateLimit(0.1);
  // increase laser pulse periods (defaults are 14 and 10 PCLKs)
  (*L0X).setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
  (*L0X).setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);
} // set_VL53L0X_long_range()

bool set_VL53L0X_high_speed(VL53L0X* L0X) {
  // reduce timing budget to 20 ms (default is about 33 ms)
  return (*L0X).setMeasurementTimingBudget(20000);
} // set_VL53L0X_high_speed()

bool set_VL53L0X_high_accuracy(VL53L0X* L0X) {
  // increase timing budget to 200 ms
  return (*L0X).setMeasurementTimingBudget(200000);
} // set_VL53L0X_high_accuracy()


void setup_VL53L0X(VL53L0X* L0X) {
  MENU.out(F("\n################ VL53L0X init\t"));
  VL53lox_usable=false;

#if defined VL53L0X_DEBUG
  MENU.out(F("@ms "));
  MENU.out(millis());
  MENU.out('\t');
#endif

  (*L0X).setTimeout(500);
  if((*L0X).init()) {	// ok
    VL53lox_usable=true;
    MENU.out(F("ok\tmm "));
    //(*L0X).setMeasurementTimingBudget(20000);
    MENU.outln(VL53L0X_read_mm(L0X));
    return;			// OK :)
  } // else

  //ERROR_ln(F("(*L0X).init()"));	// ERROR
  MENU.error_ln(F("(*L0X).init()"));	// ERROR
} // setup_VL53L0X()

#define VL53L0X_H
#endif
