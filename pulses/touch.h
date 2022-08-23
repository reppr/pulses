/*
  touch.h
  ESP32 touch sensors
  see https://github.com/espressif/arduino-esp32/blob/master/tools/sdk/include/driver/driver/touch_pad.h
  or locate touch_pad.h

  file maybe obsolete, see: morse.h
*/
#ifndef TOUCH_H

// TODO: touch_threshold should go to HARDWARE
//int touch_threshold = 70;		// TODO: does not work well on some new instruments ???
//int touch_threshold = 60;		// TODO: TEST&TRIM:
//int touch_threshold = 58;		// TODO: TEST&TRIM:

#if defined ePaper213B73_BOARD_LILYGO_T5 || defined ePaper213BN_BOARD_LILYGO_BN
  int touch_threshold = 40;		// TODO: TEST&TRIM:	// touch_threshold should go to HARDWARE
#else
  int touch_threshold = 61;		// TODO: TEST&TRIM:	// touch_threshold should go to HARDWARE
#endif

/* see also:
   touch_pad_init();
   touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER);

   // touch_pad_set_trigger_mode(touch_trigger_mode_t mode);
   touch_pad_set_trigger_mode(TOUCH_TRIGGER_BELOW);
   touch_pad_set_trigger_mode(TOUCH_TRIGGER_ABOVE);
   touch_pad_get_status();
*/
#define  TOUCH_H
#endif // TOUCH_H
