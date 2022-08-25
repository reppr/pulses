/*
  simple_do_from_other_core.h
  *very simple minded method* to do *one* lengthy  void (void) function from another core

  do a 'void xxxx()' task
  from another core (if exists)

  stupid, no mutex
  only to do slowly one by one
*/

#if defined ESP32	// has a second core?

#if ! defined DO_ON_OTHER_CORE_PRIORITY		// see: 'pulses_engine_config.h'
  #define DO_ON_OTHER_CORE_PRIORITY	1
#endif

TaskHandle_t do_on_other_core_handle;

void do_on_other_core_task(void* function_p) {
  void (*fp)() = (void (*)()) function_p;
  (*fp)();
  vTaskDelete(NULL);
}

void do_on_other_core(void (*function_p)(), int stack_size=8*1024) {	// create and do one shot task
  BaseType_t err = xTaskCreatePinnedToCore(do_on_other_core_task,	// function
					   "other_fun",			// name
					   stack_size,			// stack size
					   (void*) function_p,		// task input parameter
					   DO_ON_OTHER_CORE_PRIORITY,	// task priority
					   &do_on_other_core_handle,	// task handle
					   0);				// core 0
  if(err != pdPASS) {
    MENU.out(err);
    MENU.space();
    ERROR_ln(F("do_on_other_core"));
  }
}

#else
  #error NO OTHER CORE just do on this one...
#endif // MULTICORE?
