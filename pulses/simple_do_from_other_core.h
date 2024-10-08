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

#if ! defined DO_ON_OTHER_CORE_STACK_SIZE
  #define DO_ON_OTHER_CORE_STACK_SIZE	2*1024	// DEFAULT	(was: 8k)
#endif

TaskHandle_t do_on_other_core_handle;

//#define DO_ON_OTHER_CORE_SHOW_STACK_USE	// from inside task,	DEBUGGING ONLY
void do_on_other_core_task(void* function_p) {
  void (*fp)() = (void (*)()) function_p;
  (*fp)();
#if defined DO_ON_OTHER_CORE_SHOW_STACK_USE	// from inside task,	DEBUGGING ONLY
  MENU.out(F("do on other core FREE STACK "));
  MENU.outln(uxTaskGetStackHighWaterMark(NULL));
#endif
  vTaskDelete(NULL);
}

//#define ON_MULTICORE_ERRORS_SHOW_STACK_SIZES	// mild debugging help
void do_on_other_core(void (*function_p)(), int stack_size=DO_ON_OTHER_CORE_STACK_SIZE) {  // create and do one shot task
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
#if defined ON_MULTICORE_ERRORS_SHOW_STACK_SIZES	// mild debugging help
    extern void esp_heap_and_stack_info();
    esp_heap_and_stack_info();
    MENU.ln();
#endif
    // vTaskDelete(do_on_other_core_handle);	// FREEZE...
  }
}

#else
  #error NO OTHER CORE just do on this one...
#endif // MULTICORE?
