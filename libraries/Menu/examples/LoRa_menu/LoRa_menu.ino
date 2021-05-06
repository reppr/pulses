/*
  LoRa_menu.ino
  uses: https://github.com/sandeepmistry/arduino-LoRa
*/

#define USE_LoRa_EXPLORING

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#if ! defined EOF32
  #define EOF32		0xffffffff
#endif

/* **************************************************************** */
/* BAUDRATE for Serial:	uncomment one of the following lines:	*/
#define BAUDRATE	500000		// works fine here
//#define BAUDRATE	115200		// works fine here
//#define BAUDRATE	57600
//#define BAUDRATE	38400
//#define BAUDRATE	19200
//#define BAUDRATE	9600		// failsafe
//#define BAUDRATE	31250		// MIDI


/* **************************************************************** */
#define LoRa_RECEIVE_BUF_SIZE	256	// TODO: very big, just for testing (and LoRa chat...)
/* **************************************************************** */

#if ! defined MENU_OUTSTREAM2
  // see: https://stackoverflow.com/questions/11826554/standard-no-op-output-stream
  #include <iostream>
  class NullBuffer :  public std::streambuf
  {
  public:
    int overflow(int c) { return c; }
    // streambuf::overflow is the function called when the buffer has to output data to the actual destination of the stream.
    // The NullBuffer class above does nothing when overflow is called so any stream using it will not produce any output.
  };

  NullBuffer null_buffer;
  //#define MENU_OUTSTREAM2		std::ostream null_stream(&null_buffer)
  #define MENU_OUTSTREAM2	(Stream &) null_buffer
#endif

/*
  This version definines the menu INPUT routine int men_getchar();
  in the *program* not inside the Menu class.
*/
int men_getchar() {
  if (!Serial.available())	// ARDUINO
    return EOF32;

  return Serial.read();
}


#include <Menu.h>

#define CB_SIZE		128	// a big menu input buffer size allows LoRa chat with long lines
				// if you are short on RAM something like 32 is big enough

Menu MENU(CB_SIZE, 1, &men_getchar, Serial, MENU_OUTSTREAM2);

#include "LoRa_pulses.h"


void setup() {
  Serial.begin(BAUDRATE);	// Start serial communication.
  while (!Serial) { ;}		// wait for Serial to open

  extern void LoRa_menu_display();
  extern bool LoRa_menu_reaction(char token);
  MENU.add_page("LoRa menu", 'L', \
		&LoRa_menu_display, &LoRa_menu_reaction, '+');

  MENU.ln();
  MENU.outln(F("LoRa menu\thttp://github.com/reppr/pulses/"));
  MENU.ln();

  setup_LoRa_default();

  MENU.menu_display();		// display menu at startup
  MENU.ln();
} // setup()


/*
  Call MENU.lurk_then_do(); from Arduino loop();
  This will *not* block the loop.

  It will lurk on menu input, but return immediately if there's none.
  On input it will accumulate one next input byte and return until it
  will see an 'END token'.

  When seeing one of the usual line endings it takes it as 'END token' \0.

  On receiving an 'END token' it will interpret the accumulated tokens and return.
*/

void loop() {	// ARDUINO
  MENU.lurk_then_do();
  check_for_LoRa_jobs();

  // Insert your own code here.
  // Do not let your program block program flow,
  // always return to the main loop as soon as possible.
} // loop()

#include "LoRa_menu_page.h"
