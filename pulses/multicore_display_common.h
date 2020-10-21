/*
  multicore_display_common.h
*/

#if ! defined MULTICORE_DISPLAY_COMMON_H

#include <freertos/task.h>

// testing RTOS task priority for monochrome display routines:
#define MONOCHROME_PRIORITY	0	// seems best


typedef struct print_descrpt_t {
  int16_t col=0;
  int16_t row=0;
  int16_t offset_y=0;
  //uint8_t size=0;
  //uint8_t colour=0;
  //bool inverted=false;
  char* text = NULL;
} print_descrpt_t;

bool /*error*/ copy_text_to_text_buffer(char* text, print_descrpt_t* txt_descr_p) {
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
  MENU.error_ln(F("text buffer malloc()"));
  return true;	// ERROR
} // copy_text_to_text_buffer()

void free_text_buffer(print_descrpt_t* txt_descr_p) {
#if defined DEBUG_ePAPER	// not used on oled displays, so i leave the name as is
  MENU.outln("DEBUG_ePAPER\tfree_text_buffer() ");
  // MENU.print_free_RAM(); MENU.tab();		// deactivated	***I HAD CRASHES HERE***
#endif
  if((*txt_descr_p).text != NULL)
    free((*txt_descr_p).text);
  free(txt_descr_p);
}

SemaphoreHandle_t MC_mux = NULL;

// MC_do_on_other_core() version with MC_mux and MC_DELAY_MS
TaskHandle_t MC_do_on_other_core_handle;

void MC_do_on_other_core_task(void* function_p) {
  void (*fp)() = (void (*)()) function_p;

  xSemaphoreTake(MC_mux, portMAX_DELAY);

  (*fp)();

  vTaskDelay(MC_DELAY_MS / portTICK_PERIOD_MS);
  xSemaphoreGive(MC_mux);
  vTaskDelete(NULL);
}

void MC_do_on_other_core(void (*function_p)()) {	// create and do one shot task
  BaseType_t err = xTaskCreatePinnedToCore(MC_do_on_other_core_task,	// function
					   "other_fun",			// name
					   4000,				// stack size
					   (void*) function_p,			// task input parameter
					   0,					// task priority
					   &MC_do_on_other_core_handle,		// task handle
					   0);					// core 0
  if(err != pdPASS) {
    MENU.out(err);
    MENU.space();
    MENU.error_ln(F("MC_do_on_other_core"));
  }
}

#define MULTICORE_DISPLAY_COMMON_H
#endif
