/*
  menu_IO_configuration.h
*/

/*
  this version definines the menu INPUT routine int men_getchar();
  and the menu OUTPUT streams
  from the *program*
  not inside the Menu class

  so your program must #include
  #include "menu_IO_configuration.h"
*/


#ifndef MENU_CONFIGURATION_H

/*
// CONFIGURE one or more INPUTS and OUTPUTS:
#define MENU_USE_SERIAL_IN
#define MENU_USE_SERIAL_OUT

#define MENU_USE_WiFi_TELNET_IN
#define MENU_USE_WiFi_TELNET_OUT

// determine number of configured outstreams
#if defined(MENU_USE_SERIAL_OUT) + defined(MENU_USE_WiFi_TELNET_OUT) == 2
  #define MENU_OUTSTREAMS	2
#elif defined(MENU_USE_SERIAL_OUT) + defined(MENU_USE_WiFi_TELNET_OUT) == 1
  #define MENU_OUTSTREAMS	1
#elif defined(MENU_USE_SERIAL_OUT) + defined(MENU_USE_WiFi_TELNET_OUT) == 0
  #define MENU_OUTSTREAMS	0
  #error "MENU_OUTSTREAMS=0	not implemented yet"
#endif
*/

#ifdef ARDUINO
  /* ********************************
    ==> Serial menu output
  */

/*
  ################ FIXME: temporary:  use Serial unconditionally
  #if defined(MENU_USE_SERIAL_OUT) || defined(MENU_USE_SERIAL_IN)
*/

    /* BAUDRATE for Serial:	uncomment one of the following lines:	*/
    //#define BAUDRATE	250000
    //#define BAUDRATE	230400
    #define BAUDRATE	115200		// works fine here on all tested Arduinos
    //#define BAUDRATE	74880
    //#define BAUDRATE	57600
    //#define BAUDRATE	38400
    //#define BAUDRATE	19200
    //#define BAUDRATE	9600		// failsafe
    //#define BAUDRATE	31250		// MIDI

    #define MENU_OUTSTREAM	Serial
/*
    #if defined(MENU_USE_SERIAL_OUT)
      #define MENU_OUTSTREAM	Serial
    #endif
  #endif	  ################ FIXME: temporary:  use Serial unconditionally
*/

  #ifdef USE_WIFI_telnet_menu	/* ################ FIXME: #ifdef MENU_USE_WiFi_TELNET_OUT */
    /* ********************************
       ==> WiFi menu output
    */

    //  // how many clients should be able to telnet to this ESP8266
    //  #define MAX_SRV_CLIENTS 1
    //  WiFiClient server_client[MAX_SRV_CLIENTS];
    WiFiClient server_client;
    WiFiServer telnet_server(23);

    #define MENU_OUTSTREAM2	server_client
  #endif

  /* ********************************
    ==> menu input
  */
  int men_getchar() {
    if (Serial.available())
      return Serial.read();

  #ifdef USE_WIFI_telnet_menu
      if (server_client && server_client.connected() && server_client.available())
	return server_client.read();
  #endif

/*
    #ifdef MENU_USE_SERIAL_IN
      if (Serial.available())
	return Serial.read();
    #endif

    #ifdef MENU_USE_WiFi_TELNET_IN
      if (server_client && server_client.connected() && server_client.available())
        return server_client.read();
    #endif
*/
    return EOF;
  }

#else	// PC
  #warning "Linux PC test version  *NOT SUPPORTED*  out of date"

  int men_getchar() {
    return getchar();		// c++ Linux PC test version
  }

  #define MENU_OUTSTREAM	cout
#endif

  #define MENU_CONFIGURATION_H
#endif