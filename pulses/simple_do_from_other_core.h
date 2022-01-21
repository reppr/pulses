/*
  simple_do_from_other_core.h
  *very simple minded method* to do *one* lengthy  void (void) function from another core

  do a 'void xxxx()' task
  from another core (if exists)

  stupid, no mutex
  only to do slowly one by one
*/

#if defined ESP32	// has a second core?
TaskHandle_t do_on_other_core_handle;

void do_on_other_core_task(void* function_p) {
  void (*fp)() = (void (*)()) function_p;
  (*fp)();
  vTaskDelete(NULL);
}

void do_on_other_core(void (*function_p)()) {	// create and do one shot task
  BaseType_t err = xTaskCreatePinnedToCore(do_on_other_core_task,	// function
					   "other_fun",			// name
					   4000,				// stack size
					   (void*) function_p,			// task input parameter
					   0,					// task priority
					   &do_on_other_core_handle,		// task handle
					   0);					// core 0
  if(err != pdPASS) {
    MENU.out(err);
    MENU.space();
    ERROR_ln(F("do_on_other_core"));
  }
}

#else
  #error NO OTHER CORE just do on this one...
#endif // MULTICORE?
