/*
  VL53L0X_pulses_Adafruit.h

  USE_VL53L0X_Adafruit
*/

#if ! defined USE_VL53L0X_Adafruit_H

#include "Adafruit_VL53L0X.h"
Adafruit_VL53L0X VL53L0X_1;


bool VL53lox_usable=false;

int16_t VL53L0X_read_mm() {	// negative values ERROR	-1 out of range 	-2 not initialized
  if(! VL53lox_usable)
    return -2;		// ERROR -2 not initialized

  VL53L0X_RangingMeasurementData_t measure;
  VL53L0X_1.rangingTest(&measure, false); // pass in 'true' to get debug data printout!
  if (measure.RangeStatus == 4)		// phase failures have incorrect data
    return -1;				// ERROR -1 out of range
  else
    return measure.RangeMilliMeter;	// OK
} // VL53L0X_read_mm()

void setup_VL53L0X() {
  MENU.out(F("\n################ VL53L0X init\t"));
  VL53lox_usable=false;


#if defined VL53L0X_DEBUG
  MENU.out(F("@ms "));
  MENU.out(millis());
  MENU.out('\t');
#endif

  if(VL53L0X_1.begin()) {	// ok
    VL53lox_usable=true;
    MENU.out(F("ok\tmm "));
    MENU.outln(VL53L0X_read_mm());
    return;			// OK :)
  } // else

  //ERROR_ln(F("VL53L0X_1.begin()"));	// ERROR
  MENU.error_ln(F("VL53L0X_1.begin()"));	// ERROR
} // setup_VL53L0X()

#define USE_VL53L0X_Adafruit_H
#endif
