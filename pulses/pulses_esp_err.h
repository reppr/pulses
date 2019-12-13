/*
  pulses_esp_err.h

  esp error reporting
*/

#if ! defined PULSES_ESP_ERR_INFO_H

#include "esp_err.h"
bool /* error */ esp_err_info(esp_err_t status) {
  if(status == ESP_OK) {	// ok
    if(MENU.maybe_display_more(VERBOSITY_LOWEST))
      // was: if(MENU.maybe_display_more(VERBOSITY_LOWEST) || DEBUG_ESP_NOW)
      MENU.outln(F("ok"));
    return false;	// OK
  } else			// not ok
    if(MENU.maybe_display_more(VERBOSITY_LOWEST/* sic! */))	// *do* display that
      // was: if(MENU.maybe_display_more(VERBOSITY_LOWEST/* sic! */) || DEBUG_ESP_NOW)	// *do* display that
      MENU.error_ln(esp_err_to_name(status));
  return true;		// ERROR
}

#define PULSES_ESP_ERR_INFO_H
#endif
