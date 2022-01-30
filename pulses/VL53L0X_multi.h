/*
  VL53L0X_multi.h
*/


#if ! defined VL53L0X_MULTI_H
#include <Wire.h>

VL53L0X* L0Xp_array[USE_VL53L0X_max] = {NULL} ;

VL53L0X VL53L0X_0;
#if (USE_VL53L0X_max > 1)
VL53L0X VL53L0X_1;
#if (USE_VL53L0X_max > 2)
VL53L0X VL53L0X_2;
#if (USE_VL53L0X_max > 3)
VL53L0X VL53L0X_3;
#if (USE_VL53L0X_max > 4)
  #error 'USE_VL53L0X_max too many'
#endif
#endif
#endif
#endif


bool detect_i2c_device(uint8_t adr) {
  Wire.beginTransmission(adr);
  return ! Wire.endTransmission();
}

uint8_t setup_sequence_multi_VL53L0X() {
  MENU.out(F(">>>>>>>>>>>>>>>> setup_multi_VL53L0X()\t"));
  uint8_t cnt_index=0;	// index and count all devices, new and already existing
  uint8_t cnt_new=0;	// new setup
  uint8_t adr_new=0x29;	// next i2c address

#if defined VL53L0X_DEBUG
  MENU.out(F("start ms "));
  MENU.out(millis());
  MENU.out('\t');
#endif

  L0Xp_array[0] = &VL53L0X_0;
#if (USE_VL53L0X_max > 1)
  L0Xp_array[1] = &VL53L0X_1;
#if (USE_VL53L0X_max > 2)
  L0Xp_array[1] = &VL53L0X_2;
#if (USE_VL53L0X_max > 3)
  L0Xp_array[1] = &VL53L0X_3;
#endif
#endif
#endif

  if(detect_i2c_device(0x29)) {		// *new* device on default i2c address found
    cnt_index++;
    adr_new++;
#if defined VL53L0X_DEBUG
    MENU.out(cnt_index);
    MENU.out(':');
    MENU.out(millis());
    MENU.space();
#endif

    while(detect_i2c_device(adr_new)) {	// check planned new address
#if defined VL53L0X_DEBUG
      MENU.out(adr_new);
      MENU.out(F(" ! "));
#endif
      setup_VL53L0X(L0Xp_array[cnt_index++]);	// setup already existing sensor (again?)
      adr_new++;
    } // adr is free

    if(cnt_index < USE_VL53L0X_max) {			// if not yet all requested devices here?
      setup_VL53L0X(L0Xp_array[cnt_index]);	// setup new sensor
      (*L0Xp_array[cnt_index]).setAddress(adr_new);		// move to new address
#if defined VL53L0X_DEBUG
      MENU.out(F("ms"));
      MENU.out(millis());
      MENU.space();
#endif
      if(detect_i2c_device(adr_new))	// check if moved to new addr?
	cnt_new++;
      else
	MENU.error_ln(F(" VL53L0X *not* moved"));  // something went wrong
      //ERROR_ln(F("VL53L0X new addr"));

      if(detect_i2c_device(0x29))		// ERROR check: another device arrived early @ old addr
	MENU.error_ln(F(" ALREADY next?"));
      //ERROR_ln(F("VL53L0X @ 0x29"));
    }
  }
#if defined VL53L0X_DEBUG
  MENU.ln();
#endif

  MENU.out(cnt_index);
  MENU.out(F(" sensors found\tnew: "));
  MENU.outln(cnt_new);
  MENU.ln();

  return cnt_index;
} // setup_sequence_multi_VL53L0X()


#define VL53L0X_MULTI_H
#endif
