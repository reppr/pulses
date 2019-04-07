/*
  stress_managment.h
*/


#if ! defined STRESS_MANAGMENT_H

// TODO: stress configuration struct conf_stress_t
/* did not really help...	// instant_stress_release DEACTIVATED
void instant_stress_release() {
  MENU.out("&");
  int cnt=0;

  for(int pulse = (PL_MAX -1); pulse>=0; pulse--)
    if(PULSES.pulses[pulse].groups & g_SECONDARY) {
      PULSES.init_pulse(pulse);
      if(++cnt == 2)
	return;
    }
}
*/

#define STRESS_MANAGMENT_H
#endif
