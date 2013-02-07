/* **************************************************************** */
/*
   menu2.h
*/
/* **************************************************************** */
#ifndef MENU2
#define MENU2


#include <stdio.h>
#include <stdlib.h>

/* **************************************************************** */
// preprocessor macro logic:

/* debugging macros:		*/
#ifdef DEBUGGING_ALL
  #define DEBUGGING_CLASS
  #define DEBUGGING_CIRCBUF
  #define DEBUGGING_LURKING
  #define DEBUGGING_MENU
#endif

// Preprocessor magic to compile on Arduino:
/* **************** ARDUINO **************** */
#if defined(ARDUINO)
  /* MAYBE_PROGMEM  *MUST* be #defined,
     either as 'PROGMEM' to save RAM on Arduino
     or empty for a PC test run.			*/
  #ifndef MAYBE_PROGMEM		// commandline?
//  #define MAYBE_PROGMEM	PROGMEM		################ DEACTIVATED
    #define MAYBE_PROGMEM
  #endif


  /* I/O MACROs  ARDUINO: */

  /* streaming output on the Arduino
     http://playground.arduino.cc/Main/StreamingOutput	*/
  // #include <Streaming.h> did not work for me
  // #include "Streaming.h"

/*
  #include <Stream.h>
  template<class T> inline Print &operator <<(Print &obj, T arg) { obj.print(arg); return obj; }
/*
  /* outMACRO  macro for stream output:	ARDUINO		*/
  #ifndef outMACRO		// commandline?
    #define outMACRO	Serial

    #define OUTch(c)		Serial.print((char) (c))
    #define OUT_str(str)	serial_print_progmem((str))
    #define OUT(X)		Serial.print(X)
    #define OUTln(X)		Serial.println(X)
    #define OUTln		Serial.println()

  #endif


  /* getcharMACRO  macro for char input: ARDUINO	*/
  /* returns EOF or char				*/
  /* see menu2.cpp
  int getchar_serial() {
    if (!Serial.available())
      return EOF;

    return Serial.read();
  }
  */
  #define getcharMACRO	getchar_serial	// on Arduino


  /* keep Arduino GUI happy ;(				*/
  #if ARDUINO >= 100
    #include "Arduino.h"
  #else
    #include "WProgram.h"
  #endif

  #include <avr/pgmspace.h>


#else /* **************** LINUX **************** */
/*	    for development on a Linux PC	 */

/* MAYBE_PROGMEM  *MUST* be #defined,
   either as 'PROGMEM' to save RAM on Arduino
   or empty for a PC test run.			*/
  #ifndef MAYBE_PROGMEM		// commandline?
    #define MAYBE_PROGMEM
  #endif


  /* I/O MACROs for LINUX: */

  /* outMACRO  macro for stream output:		*/
  #ifndef outMACRO		// commandline?
    /* outMACRO for testing on c++		*/
    #define outMACRO	std::cout

    #define OUTch(c)		printf("%c", (c))
    #define OUT_str(str)	printf("%p", (str))
    #define OUT(X)		printf("%d", (X))
    #define OUTln(X)		printf("%d\n", (X))
    #define OUTln		printf("\n")
  #endif


  /* getcharMACRO  macro for char input: LINUX	*/
  /* returns EOF or char			*/
  #define getcharMACRO	getchar	/* on Linux	*/


#endif // [Arduino else] LINUX preprocessor stuff.
/* **************************************************************** */

// declare this early:
void menu2_setup();


// struct menupage for pages[]
struct menupage {
  void (*display)(void);
  bool (*interpret)(char token);
  char *title;
  char hotkey;
  // configure which pages burried below men_selected remain active:
  char active_group;	// tokens on pages with the same active_group
			// as men_selected remain active below page actions
			// '+' and  '-' have special meanings:
			// '+' means *always* active,  '-' *never* active
};


class Menu2 {
 public:
  Menu2(int size, int menuPages, int (*maybeInput)(void));
  ~Menu2();

  // high level API:
  bool lurk_then_do(void);
  void add_page(char *pageTitle, char token, \
		void (*pageDisplay)(), bool (*pageReaction)(char), char ActiveGroup);
  void menu_pages_info() const;			// show all known pages' info


  int cb_stored() const { return cb_count; }	// inline:  # stored bytes
  char cb_read();				// get a byte from buffer, no checks

  int cb_peek() const;				// peek at next if any, else return EOL
  int cb_peek(int offset) const;		// peek at next, overnext... if any, else EOL
  void skip_spaces();				// skip leading spaces from the buffer
  int next_input_token() const;			// next non space input token if any, else EOL
  bool is_numeric() const;				// test if next token will be a numeric chiffre
  long numeric_input(long default_value);	// read a number from the buffer
  void skip_numeric_input();			// drop leading numeric sequence from the buffer

  void menu_display();				// display the menu:
						//   selected menu page,
						//   page hot keys,
						//   internal hot keys.

  // act on buffer content tokens after seeing 'end token':
  void interpret_men_input();			// menu input interpreter

 protected:
  void menu_page_info(char pg) const;		// show a menu pages' info
  bool cb_is_full() const { return cb_count == cb_size; }	// inlined: buffer full?
#ifdef DEBUGGING_CIRCBUF
  void cb_info() const;						// debugging help
#endif
  int (*maybe_input)(void);	// maybe_input() must return EOF or next char
  bool (*action)(void);		// will be called on receiving an end token

 private:
  int cb_start;
  int cb_size;
  int cb_count;
  char * cb_buf;
  void cb_write(char value);			// save a byte in buffer,  no checks

  // menu pages:
  char men_max;
  char men_known;
  char men_selected;
  menupage *men_pages;
};

/* **************************************************************** */
#endif
