/*
  VL53L0X_multi.h
*/

#define VL53L0X_DEBUG

#if (USE_VL53L0X_max > 1)
  #warning 'multiple VL53L0X do not work with this version, needs also hardware changes'
  #define VL53L0X_DEBUG
#endif


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

bool L0X_usable[USE_VL53L0X_max]={false};


int16_t VL53L0X_get_mm_from_sensor(uint8_t index) {	// sensor by index
#if defined VL53L0X_DEBUG
  MENU.out(F("VL53L0X_get_mm_from_sensor("));
  MENU.out(index);
  MENU.out(F(")\t"));
#endif

  if(index < 0 || index >= USE_VL53L0X_max) {
#if defined VL53L0X_DEBUG
    MENU.out_Error_();
    MENU.out(F("index "));
    MENU.outln(index);
#endif
    return -4;		// ERROR -4 invalid index
  }

  if(! L0X_usable[index]) {
#if defined VL53L0X_DEBUG
    MENU.out_Error_();
    MENU.outln(F("not initialised"));
#endif
    return -2;		// ERROR -2 not initialized
  }

  bool usability_was=VL53lox_usable;	// WORKAROUND: to keep *single sensor* version usable...
  VL53lox_usable = L0X_usable[index];	// WORKAROUND: to keep *single sensor* version usable...
  int16_t mm = VL53L0X_read_mm(L0Xp_array[index]);
  VL53lox_usable = usability_was;

#if defined VL53L0X_DEBUG
  MENU.out(F("raw "));
  MENU.out(mm);
  MENU.space(2);
#endif

  if((*L0Xp_array[index]).timeoutOccurred()) {
#if defined VL53L0X_DEBUG
    MENU.out_Error_();
    MENU.outln(F("timeout"));
#endif
    return -3;		// ERROR -3 timeout
  }

  if(mm == 8190) {
#if defined VL53L0X_DEBUG
    MENU.out_Error_();
    MENU.outln(F("out of range"));
#endif
    return -1;		// ERROR -1 out of range
  }

#if defined VL53L0X_DEBUG
  MENU.out(F("mm= "));
  MENU.outln(mm);
#endif

  return mm;
} // VL53L0X_get_mm_from_sensor()


bool detect_i2c_device(uint8_t adr) {
  Wire.beginTransmission(adr);
  return (0 == Wire.endTransmission());
}

/*
  This version explored an approach without the need for GPIOs or i2c port expander to
  setup multiple sensors one after the other on different addresses. I tried to delay
  each sensor (but the first) with RC. Never worked reliable.
*/
uint8_t setup_sequence_multi_VL53L0X() {
  MENU.out(F("setup_sequence_multi_VL53L0X()\t"));
  uint8_t cnt_index=0;	// index and count all devices, new and already existing
  uint8_t cnt_new=0;	// new setup
  uint8_t adr_new=0x29;	// next i2c address

#if defined VL53L0X_DEBUG
  uint32_t start_time = millis();	// DEBUG: to be able to show this when done
  MENU.out(F("start ms "));
  MENU.outln(start_time);
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

#if ! defined L0X_TESTS
  #define L0X_TESTS	20	// DEFAULT: 20*50ms = 1 second
#endif

  for(int test_nr=0; (test_nr < L0X_TESTS) && (cnt_index < USE_VL53L0X_max); test_nr++) {	// testloop, waiting for new devices to appear
    if(detect_i2c_device(0x29)) {		// *new* device on default i2c address found
      //cnt_index++;
      adr_new++;
#if defined VL53L0X_DEBUG
      MENU.out(F("new #"));
      MENU.out(cnt_index);
      MENU.out(':');
      MENU.out(millis());
      MENU.space();

      if(false == detect_i2c_device(adr_new)) {	// DEBUG only
	MENU.out(F("free addr @ "));
	MENU.out_hex((uint32_t) adr_new);
	MENU.ln();
      }
#endif

      while(detect_i2c_device(adr_new)) {	// check planned new address
#if defined VL53L0X_DEBUG
	MENU.out_hex(adr_new);
	MENU.out(F(" in use\t"));
#endif
	if(setup_VL53L0X(L0Xp_array[cnt_index]))	// setup already existing sensor (again?)
	  L0X_usable[cnt_index] = true;			// OK

	adr_new++;
	cnt_index++;
      } // adr is free

      if(cnt_index <= USE_VL53L0X_max) {			// if not yet all requested devices here?
#if defined VL53L0X_DEBUG
	MENU.out(F(" new addr "));
	MENU.out_hex(adr_new);
	MENU.out(F(" init "));
#endif
	MENU.out('#');
	MENU.out(cnt_index);
	MENU.space();
	if(setup_VL53L0X(L0Xp_array[cnt_index])) {		// setup new sensor
	  L0X_usable[cnt_index] = true;				// OK
#if defined VL53L0X_DEBUG
	  MENU.out(F("ok"));
#endif
	  ;
	}
#if defined VL53L0X_DEBUG
	else
	  MENU.out(F("FAILED"));
	MENU.out(F("\tset new address"));
#endif
	(*L0Xp_array[cnt_index]).setAddress(adr_new);		// move to new address

	if(detect_i2c_device(adr_new)) {	// check if moved to new addr?
	  cnt_new++;
#if defined VL53L0X_DEBUG
	  MENU.out(F("\tmoved:"));
	  MENU.out(millis());
	  MENU.space();
#endif
	} else
	  MENU.error_ln(F(" VL53L0X *not* moved"));  // something went wrong
	//ERROR_ln(F("VL53L0X new addr"));

	if(detect_i2c_device(0x29))		// ERROR check: another device arrived early @ old addr
	  MENU.error_ln(F(" ALREADY next?"));	//	change hardware setup (increase resistor)
	//ERROR_ln(F("VL53L0X @ 0x29"));
      } // if(cnt_index <= USE_VL53L0X_max)
#if defined VL53L0X_DEBUG
      else {
	MENU.out("DADA nonew ");
	MENU.out(cnt_index);
	MENU.space();
	MENU.out(USE_VL53L0X_max);
      }
#endif
    } // detected new @ 0x29
    else {
      delay(50);
    }
#if defined VL53L0X_DEBUG	// MAYBE: always? ################################
    MENU.out('.');
#endif
  } // testloop, waiting for new devices to appear
#if defined VL53L0X_DEBUG
  MENU.ln();
#endif

  MENU.out(cnt_index);
  MENU.out(F(" sensors found\tnew: "));
  MENU.outln(cnt_new);

  if(cnt_index < USE_VL53L0X_max) {	// second run: checking for pre existing devices
    cnt_index=0;	// counting again
#if defined VL53L0X_DEBUG
    MENU.outln(F("searching pre existing sensors:"));
#endif
    for(int i=0, adr=0x29; i < USE_VL53L0X_max; i++) {
      adr++;
#if defined VL53L0X_DEBUG
      MENU.out('#');
      MENU.out(i);
      MENU.out(F(" @"));
      MENU.out_hex(adr);
      MENU.space();
#endif

      if(detect_i2c_device(adr)) {
#if defined VL53L0X_DEBUG
	MENU.out(F("yes, adr "));
#endif
	(*L0Xp_array[cnt_index]).setAddress(adr);		// I don't know why this is needed...

#if defined VL53L0X_DEBUG
	MENU.out_hex((*L0Xp_array[cnt_index]).getAddress());	//	I get 0x29 without ^^
	MENU.tab();
#endif
	if(setup_VL53L0X(L0Xp_array[cnt_index])) {		// setup new sensor
	  L0X_usable[cnt_index] = true;				// OK
#if defined VL53L0X_DEBUG
	  MENU.outln(F("ok"));
#endif
	} else {
	  L0X_usable[cnt_index] = false;			// *NOT* usable?
#if defined VL53L0X_DEBUG
	  MENU.outln(F("FAILED"));
	}
#endif
	cnt_index++;
      } else {	// detected or not
	L0X_usable[cnt_index] = false;			// *NOT* usable?
#if defined VL53L0X_DEBUG
	MENU.out_Error_();
	MENU.outln(F("missing"));
#endif
      }
    } // loop over devices

    MENU.out(cnt_index);
    MENU.out(F(" sensors found\tnew: "));
    MENU.outln(cnt_new);

  } // second run: checking for pre existing devices

#if defined VL53L0X_DEBUG
  MENU.out(F("start time "));
  MENU.outln(start_time);
#endif

  MENU.ln();

  return cnt_index;
} // setup_sequence_multi_VL53L0X()


#define VL53L0X_MULTI_H
#endif
