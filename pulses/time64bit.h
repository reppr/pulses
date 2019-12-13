/*
  time64bit.h
*/

#include "driver/timer.h"

// esp_err_t ERROR reporting
#include "pulses_esp_err.h"

timer_config_t TIME64_conf;

timer_group_t timer64Group = (timer_group_t) 1;	// omg i love c
timer_idx_t timer64Num = (timer_idx_t) 1;

void test_timer64() {
  uint64_t value64;
  timer_get_counter_value(timer64Group, timer64Num, &value64);
  MENU.out((unsigned int) value64);

  MENU.tab(1);
  double value_f = value64;
  MENU.out(value_f, 15);
  MENU.tab();

  uint64_t d = value_f;
  d -= value64;
  MENU.out(F("d= "));
  MENU.out((unsigned int) d);
  MENU.tab();

  MENU.out(value_f/1000000.0);
  MENU.outln(F(" s"));
}

void setup_timer64() {
  MENU.out(F("setup_timer64()  G"));
  MENU.out(timer64Group);
  MENU.out(F(" T"));
  MENU.out(timer64Num);
  MENU.tab();

  TIME64_conf.alarm_en = TIMER_ALARM_DIS;
  TIME64_conf.counter_en = true;
  //TIME64_conf.intr_type
  TIME64_conf.counter_dir = TIMER_COUNT_UP;
  //TIME64_conf.auto_reload
  TIME64_conf.divider=80;

  esp_err_info(timer_init(timer64Group, timer64Num, &TIME64_conf));
  MENU.ln();
  /*
  for(int g=0; g<2; g++) {	// just a test ;)
    for(int t=0; t<2; t++) {
      MENU.out('G');
      MENU.out(g);
      MENU.space(2);
      MENU.out('T');
      MENU.out(t);
      MENU.space(2);
      err=timer_init((timer_group_t) g, (timer_idx_t) t, &TIME64_conf);
      esp_err_info(err);
    }
  }
  */
}
