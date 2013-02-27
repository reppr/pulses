/* **************************************************************** */
/*
   Menu.h
*/
/* **************************************************************** */
#ifndef MENU_h
#define MENU_h

#include <stdio.h>
#include <stdlib.h>

#ifdef ARDUINO
  #define STREAMTYPE	Stream
#else
  #include <iostream>

  using namespace std;
  ostream & Serial=cout;	// nice trick from johncc
  #define STREAMTYPE	ostream
#endif

/* **************************************************************** */
#ifdef ARDUINO
  #define SHOW_FREE_RAM

  // comment/uncomment for RAM tests:
  #define USE_F_MACRO
#endif

#ifndef USE_F_MACRO
  // For tests and on PC:  Fake Arduino F("string") macro as noop:
  #define F(s)	(s)
#endif


/* **************************************************************** */
// Preprocessor macro logic:

/* Debugging macros:		*/
#ifdef DEBUGGING_ALL
  #define DEBUGGING_CIRCBUF
  #define DEBUGGING_LURKING
  #define DEBUGGING_MENU
#endif


#if defined(ARDUINO)
/* Keep ARDUINO GUI happy ;(				*/
  #if ARDUINO >= 100
    #include "Arduino.h"
  #else
    #include "WProgram.h"
  #endif
#endif // ARDUINO
/* **************************************************************** */


// Declare early to make live easier:
void Menu_setup();


/* **************************************************************** */
/* struct menupage for Menu::menupage *men_pages[]:		*/

struct menupage {
  void (*display)(void);
  bool (*interpret)(char token);
  char *title;
  char hotkey;
  // Configure which pages burried below men_selected remain active:
  char active_group;	// tokens on pages with the same active_group
			// as men_selected remain active below page actions
			// '+' and  '-' have special meanings:
			// '+' means *always* active,  '-' *never* active
};


/* **************************************************************** */
/* class Menu {}						*/

class Menu {
 public:
  ~Menu();

  // high level API:
  Menu(int size, int menuPages, int (*maybeInput)(void), STREAMTYPE & port);
  bool lurk_then_do(void);
  void add_page(const char *pageTitle, const char hotkey, \
		void (*pageDisplay)(), bool (*pageReaction)(char), \
		const char ActiveGroup);
 

  void flush() const { port_.flush(); }

  int cb_peek() const;			// peek at next if any, else return EOL
  int cb_peek(int offset) const;	// peek at next, overnext... if any, else EOL

  void skip_spaces();			// skip leading spaces from the buffer
  int next_input_token() const;		// next non space input token if any, else EOL
  bool is_numeric() const;		// test if next token will be a numeric chiffre
  long numeric_input(long default_value);  // read a number from the buffer
  void skip_numeric_input();		// drop leading numeric sequence from the buffer

  void menu_display() const;		// display the menu:
					//   selected menu page,
					//   page hot keys,
					//   internal hot keys.

  // out(any );	  overloaded menu output function family:
  // Simple versions  void Menu::out():
  void out(const char c)    const;	// char output
  void out(const int i)     const;	// int output
  void out(const long l)    const;	// long output
  void out(const char *str) const;	// string

#ifdef ARDUINO
  void out(const __FlashStringHelper*) const; // Arduino macro: F("string")
#endif

  // End of line versions  void outln():
  void outln(const char c)    const;	// char output and newline
  void outln(const int i)     const;	// int output  and newline
  void outln(const long l)    const;	// long output and newline
  void outln(const char *str) const;	// string and newline

  void ticked(const char c) const;  // output a ticked char token like 'A'


  /* String recycling:						*/
  void OutOfRange() const;	// output "out of range"
  void Error_() const;		// output " ERROR: "

/* Output a newline, tab, space, '='
  ln(), tab(), space(), equals():				*/
  void ln()	const;		// output a newline
  void tab()	const;		// output a tab
  void space()	const;		// output a space
  void equals()	const;		// output char '='


#if defined(ARDUINO) && defined(SHOW_FREE_RAM)	// Arduino: RAM usage
/* int get_free_RAM(): determine RAM usage			*/
  int get_free_RAM() const;
#endif


 protected:
  // Act on buffer content tokens after seeing 'end token':
  void interpret_men_input();		// Menu input interpreter

  bool cb_is_full() const { return cb_count == cb_size; } // inlined: buffer full?
#ifdef DEBUGGING_CIRCBUF
  void cb_info() const;			// debugging help
#endif
  void menu_page_info(char pg)	const;	// show a menu pages' info
  void menu_pages_info()	const;	// show all known pages' info

  int (*maybe_input)(void);	// maybe_input()  Must return EOF or next char
  bool (*action)(void);		// will be called on receiving an end token

  int cb_stored() const { return cb_count; }   // inlined: number of accumulated bytes

 private:
  int cb_start;
  int cb_size;
  int cb_count;
  char * cb_buf;
  void cb_write(char value);	// save a byte in buffer,  no checks
  char cb_read();		// get oldest byte from the buffer.
				//   does *not* check if buffer is empty.
  STREAMTYPE & port_;		// output stream
  bool echo_switch;		// switch echo of input to output

  // Menu pages:
  char men_max;			// maximal number of menu pages
  char men_known;		// initialized pages
  char men_selected;		// selected page
  menupage *men_pages;		// pages' data
};

/* **************************************************************** */
#endif
