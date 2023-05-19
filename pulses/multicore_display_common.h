/*
  multicore_display_common.h
*/

#if ! defined MULTICORE_DISPLAY_COMMON_H

#include <freertos/task.h>

// testing RTOS task priority for monochrome multicore display routines:
#if ! defined MONOCHROME_PRIORITY	// see: 'pulses_engine_config.h'
  #define MONOCHROME_PRIORITY	0	// old default TODO: test&trimm
#endif

#if ! defined DO_ON_OTHER_CORE_PRIORITY	// see: 'pulses_engine_config.h'
  #define DO_ON_OTHER_CORE_PRIORITY	1
#endif

typedef struct print_descrpt_t {
  int16_t col=0;
  int16_t row=0;
  int16_t offset_y=0;	// currently only used by MC_print_1line_at()
			//   MC_display_message(), ePaper_put_run_state_symbol(),
			//   monochrome_out_morse_char()
  //uint8_t size=0;
  //uint8_t colour=0;
  //bool inverted=false;
  char* text = NULL;
} print_descrpt_t;

bool /*error*/ copy_text_to_text_buffer(const char* text, print_descrpt_t* txt_descr_p) {
  char* txt_p = (char*) malloc(strlen(text) + 1);
  if(txt_p == NULL) {
    goto malloc_error;
  }
  txt_descr_p->text = txt_p;

  char c;
  for(int i=0; ; i++) {
    c = txt_descr_p->text[i] = text[i];
    if(c==0)
      break;
  }
  return false;	// OK

 malloc_error:
  ERROR_ln(F("text buffer malloc()"));
  return true;	// ERROR
} // copy_text_to_text_buffer()

void free_text_buffer(print_descrpt_t* txt_descr_p) {
#if defined DEBUG_ePAPER	// not used on oled displays, so i leave the name as is
  MENU.outln("DEBUG_ePAPER\tfree_text_buffer() ");
  // MENU.print_free_RAM(); MENU.tab();		// deactivated	***I HAD CRASHES HERE***
#endif
#if defined DEBUG_DISPLAY_RAM_USAGE
  MENU.out(F("free_text_buffer()\t"));
  MENU.print_free_RAM(); MENU.tab();		// could crash...
#endif

  if((*txt_descr_p).text != NULL)
    free((*txt_descr_p).text);
  free(txt_descr_p);

#if defined DEBUG_DISPLAY_RAM_USAGE
  MENU.print_free_RAM();
  MENU.ln();
#endif
} // free_text_buffer()

SemaphoreHandle_t MC_mux = NULL;	// outer MUX
SemaphoreHandle_t MC_mux2 = NULL;	// inner MUX
/*
  test:  MENU.outln(uxSemaphoreGetCount(MC_mux));	// free==1	taken==0
*/

// MC_do_on_other_core() version with MC_mux and MC_DELAY_MS
TaskHandle_t MC_do_on_other_core_handle;

void MC_do_on_other_core_task(void* function_p) {
  void (*fp)() = (void (*)()) function_p;

 #if defined DEBUG_MULTICORE_DISPLAY		// used to debug errors with MONOCHROME MC_do_on_other_core()
   MENU.out(F("DEBUG_MULTICORE_DISPLAY\tstart "));
 #endif

  xSemaphoreTake(MC_mux, portMAX_DELAY);

 #if defined DEBUG_MULTICORE_DISPLAY
   MENU.out(F("taken "));
 #endif

  (*fp)();

  vTaskDelay(MC_DELAY_MS / portTICK_PERIOD_MS);
 #if defined DEBUG_MULTICORE_DISPLAY
   MENU.out(F("done delayed "));
 #endif

  xSemaphoreGive(MC_mux);
 #if defined DEBUG_MULTICORE_DISPLAY
   MENU.outln(F("given"));
 #endif

  vTaskDelete(NULL);
}

void MC_do_on_other_core(void (*function_p)(), int stack_size=4*1024) {	// create and do one shot task
  BaseType_t err = xTaskCreatePinnedToCore(MC_do_on_other_core_task,	// function
					   "other_fun",			// name
					   stack_size,			// stack size
					   (void*) function_p,		// task input parameter
					   DO_ON_OTHER_CORE_PRIORITY,	// task priority
					   &MC_do_on_other_core_handle,	// task handle
					   0);				// core 0
  if(err != pdPASS) {
    MENU.out(esp_err_to_name(err));
    MENU.tab();
    ERROR_ln(F("MC_do_on_other_core"));
#if defined ON_MULTICORE_ERRORS_SHOW_STACK_SIZES	// mild debugging help
    extern void esp_heap_and_stack_info();
    esp_heap_and_stack_info();
#endif
  }
}

#define MULTICORE_DISPLAY_COMMON_H
#endif
