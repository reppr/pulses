/*
  touch.h
  ESP32 touch sensors
  see https://github.com/espressif/arduino-esp32/blob/master/tools/sdk/include/driver/driver/touch_pad.h
  or locate touch_pad.h
*/
#ifndef TOUCH_H

#define DEBUG_TOUCH_INTERRUPT		// ################ TODO: remove debugging code

gpio_pin_t test_PIN_touch = 4;		// == T0	// ################ TODO: remove debugging code
//int touch_threshold = 70;		// TODO: does not work well on some new instruments ???
//int touch_threshold = 60;		// TODO: TEST&TRIM:
int touch_threshold = 58;		// TODO: TEST&TRIM:
#if defined DEBUG_TOUCH_INTERRUPT			// ################ TODO: remove debugging code
  long touch_cnt=0;
#endif

// touchAttachInterrupt(test_PIN_touch, touch_ISR, touch_threshold);
void  IRAM_ATTR touch_ISR(void) {
  static bool is_touched=false;
  static int touch_value_isr;

#if defined DEBUG_TOUCH_INTERRUPT		// ################ TODO: remove debugging code
  MENU.out(++touch_cnt);
  MENU.tab();
  MENU.out(touch_value_isr);
  MENU.tab();
  MENU.out(test_PIN_touch);
#endif
  touch_value_isr = touchRead(test_PIN_touch);
  if(touch_value_isr < touch_threshold) {
    if(is_touched)
#if defined DEBUG_TOUCH_INTERRUPT
      MENU.out(" ON!");		// so far, I have never seen this :)
#else
      ;
#endif
    is_touched = true;
    touch_pad_set_trigger_mode(TOUCH_TRIGGER_ABOVE);
#if defined DEBUG_TOUCH_INTERRUPT
    MENU.outln(" touched");
#endif
  } else {
    if(!is_touched) {
#if defined DEBUG_TOUCH_INTERRUPT
      MENU.out(" OFF!");
#else
      ;	// ignore??? ################
#endif
    } else {
      is_touched = false;
      touch_pad_set_trigger_mode(TOUCH_TRIGGER_BELOW);
    }
#if defined DEBUG_TOUCH_INTERRUPT
    MENU.outln(" released");
#endif
  }
}

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
