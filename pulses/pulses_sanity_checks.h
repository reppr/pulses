/*
  pulses_sanity_checks.h

  check some pp macros
*/

#include "pulses_boards.h"

#if defined USE_DACs
  #if (USE_DACs == 0)	// deactivate
    #undef USE_DACs
    #warning USE_DACs == 0:  undefined USE_DACs
  #elif (USE_DACs > 2)	// not supported
    #error USE_DACs: only 1 or 2 DACs supported
  #else
    #if defined BOARD_has_DACs
      #if (USE_DACs > BOARD_has_DACs)
	#error BOARD_has_DACs:  looks like your board has not enough DACs
      #endif
    #else	// BOARD_has_DACs is not defined
      #error BOARD_has_DACs is not defined
    #endif
  #endif
#endif // USE_DACs

// USE_MORSE
#if (defined(MORSE_GPIO_INPUT_PIN) || defined(MORSE_OUTPUT_PIN) || defined(MORSE_TOUCH_INPUT_PIN))
  #define USE_MORSE
#endif

#if (defined(MORSE_TOUCH_INPUT_PIN) && ! defined(BOARD_has_TOUCH))
  #error MORSE_TOUCH_INPUT_PIN:  undefined BOARD_has_TOUCH
#endif

// i2c
#if ! defined USE_i2c
#if defined(USE_MCP23017) || defined(USE_RTC_MODULE) || defined(USE_i2c_SCANNER)
    #define USE_i2c
  #endif
#endif

#if defined USE_BLUETOOTH_SERIAL_MENU && defined USE_WIFI_telnet_menu
  #undef USE_WIFI_telnet_menu
  #warning USE_BLUETOOTH_SERIAL_MENU so #undef USE_WIFI_telnet_menu
#endif
