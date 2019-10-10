/*
  pulses_sanity_checks.h

  check some pp macros
*/

#include "pulses_boards.h"

// DACs
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
#if defined(USE_MCP23017) || defined(USE_RTC_MODULE) || defined(USE_i2c_SCANNER) || defined(USE_MPU6050) || defined(USE_MCP23017)
    #define USE_i2c
  #endif
#endif


// OLED boards
#if defined BOARD_OLED_LIPO && defined BOARD_HELTEC_OLED
  #error BOARD_HELTEC_OLED and BOARD_OLED_LIPO can *not both* be configured
#endif

#if defined USE_BLUETOOTH_SERIAL_MENU && defined USE_WIFI_telnet_menu
  #undef USE_WIFI_telnet_menu
  #warning USE_BLUETOOTH_SERIAL_MENU so #undef USE_WIFI_telnet_menu
#endif


// ESP-NOW
#if defined USE_ESP_NOW
/*
  Sketch uses 1447262 bytes (110%) of program storage space. Maximum is 1310720 bytes.
  Global variables use 96048 bytes (29%) of dynamic memory, leaving 231632 bytes for local variables. Maximum is 327680 bytes.
*/

  #if defined USE_WIFI_telnet_menu
    #warning undefining USE_WIFI_telnet_menu
    #undef USE_WIFI_telnet_menu
  #endif
/*
  Sketch uses 1447262 bytes (110%) of program storage space. Maximum is 1310720 bytes.
  Global variables use 96048 bytes (29%) of dynamic memory, leaving 231632 bytes for local variables. Maximum is 327680 bytes.
  hmm?
*/

  #if defined USE_BLUETOOTH_SERIAL_MENU
    #warning undefining USE_BLUETOOTH_SERIAL_MENU
    #undef USE_BLUETOOTH_SERIAL_MENU
  #endif
/*
  'MENU_OUTSTREAM2' was not declared in this scope	fixed
  some more troubles,	fixed

  Sketch uses 1013648 bytes (77%) of program storage space. Maximum is 1310720 bytes.
  Global variables use 71348 bytes (21%) of dynamic memory, leaving 256332 bytes for local variables. Maximum is 327680 bytes.
*/
#endif


#if defined USE_MPU6050 && defined USE_RTC_MODULE	// clash i2c address
  #warning MPU6050 and RTC_MODULE use the same i2c address, USE_RTC_MODULE *deactivated*
  #undef USE_RTC_MODULE
#endif

#if defined INCLUDE_IMU_ZERO && ! defined USE_MPU6050
  #warning no MPU6050, no IMU ZERO
  #undef INCLUDE_IMU_ZERO
#endif

/*
  TODO:	 *conflict* UART2  MIDI <> OLED		and UART2 gpio_pins 16, 17

  #define USE_UART2

  GPIO16	OLED_RST, ePaper	UART2 RX  // *CONFLICT* UART2 MIDI <> OLED
  GPIO17				UART2 TX

  also check gpio_pins	and possible *other PIN 16, 17 USAGE*
*/
