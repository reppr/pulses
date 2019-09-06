/*
  morse_setup.h
*/

//limit_debounce		= 0.5;
limit_debounce		= 0.25;	// *LOWERED* as a test	// TODO: test&trim
limit_dot_dash		= (float) (dotTim + dashTim + 1)/2;
limit_dot_dash = 2.0;	// maybe better?	TODO: TEST: ################
limit_dash_loong	= (float) (dashTim + loongTim + 1 )/2;
limit_loong_overlong	= (float) (loongTim + overlongTim + 1)/2;

/*	float mean(int a, int b) is (a+b)/2	*/

#ifdef MORSE_GPIO_INPUT_PIN	// use GPIO with pulldown as morse input
  MENU.out(F("MORSE GPIO input pin "));
  MENU.outln(MORSE_GPIO_INPUT_PIN);

  pinMode(MORSE_GPIO_INPUT_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(MORSE_GPIO_INPUT_PIN), morse_GPIO_ISR_rising, RISING);
#endif

#ifdef MORSE_TOUCH_INPUT_PIN	// use ESP32 touch sensor as morse input
  MENU.out(F("MORSE touch pin "));
  MENU.outln(MORSE_TOUCH_INPUT_PIN);
  // touchAttachInterrupt(MORSE_TOUCH_INPUT_PIN, touch_morse_ISR, touch_threshold);	// do that last
#endif

#ifdef MORSE_OUTPUT_PIN
  MENU.out(F("MORSE output pin "));
  MENU.outln(MORSE_OUTPUT_PIN);

  pinMode(MORSE_OUTPUT_PIN, OUTPUT);
  digitalWrite(MORSE_OUTPUT_PIN, LOW);
#endif

#if defined(MORSE_GPIO_INPUT_PIN) || defined(MORSE_TOUCH_INPUT_PIN)
  // morse_show_limits();	// code left for debugging
#endif

// initialize a timer used to check for letter separation pause:
morse_separation_timer = timerBegin(0, 80, true);	// prescaler 80 for microseconds
timerAttachInterrupt(morse_separation_timer, &morse_endOfLetter, true);

#ifdef MORSE_TOUCH_INPUT_PIN	// use ESP32 touch sensor as morse input
  // touch_morse_ISR_v2();		// call ISR once from setup for initialisation?
  touchAttachInterrupt(MORSE_TOUCH_INPUT_PIN, touch_morse_ISR_v2, touch_threshold);
#endif
