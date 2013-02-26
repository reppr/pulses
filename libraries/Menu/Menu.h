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
// Preprocessor macro logic:

/* Debugging macros:		*/
#ifdef DEBUGGING_ALL
  #define DEBUGGING_CLASS
  #define DEBUGGING_CIRCBUF
  #define DEBUGGING_LURKING
  #define DEBUGGING_MENU
#endif

// Preprocessor magic to compile on ARDUINO:
/* **************** ARDUINO **************** */
#if defined(ARDUINO)

/* MAYBE_PROGMEM  *MUST* be #defined,
   either as 'PROGMEM' to save RAM on ARDUINO
   or empty for a PC test run.			*/
#ifndef MAYBE_PROGMEM		// commandline?
  // #define MAYBE_PROGMEM	PROGMEM		################ DEACTIVATED
  #define MAYBE_PROGMEM
#endif

/* Keep ARDUINO GUI happy ;(				*/
#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include <avr/pgmspace.h>


#else /* **************** LINUX **************** */
/*	  for c++ development on a Linux PC	 */

/* MAYBE_PROGMEM  *MUST* be #defined,
   either as 'PROGMEM' to save RAM on ARDUINO
   or empty for a PC test run.			*/
#ifndef MAYBE_PROGMEM		// commandline?
  #define MAYBE_PROGMEM
#endif

#endif // [ARDUINO else] LINUX preprocessor stuff.
/* **************************************************************** */


// Declare these early to make live easier:
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
  Menu(int size, int menuPages, int (*maybeInput)(void), STREAMTYPE & port);
  ~Menu();

  // high level API:
  bool lurk_then_do(void);
  void add_page(const char *pageTitle, const char hotkey, \
		void (*pageDisplay)(), bool (*pageReaction)(char), \
		const char ActiveGroup);
  //-  void menu_pages_info() const; ################
  void menu_pages_info();			// show all known pages' info

  // out(any );	  Overloaded menu output function family:
  // Simple versions  void Menu::out():
  void out(const char c);	// char output
  void out(const int i);	// int output
  void out(const long l);	// long output
  void out(const char *str);	// string

  // End of line versions  void outln():
  void outln(const char c);
  void outln(const int i);
  void outln(const long l);
  void outln(const char *str);

/* Second parameter versions with a trailing char:
  Like Menu::out(xxx, char c)
  A char as a second parameter get's printed after first argument.
  Like:  Menu::out(string, '\t');  or  Menu::out(string, ' '); */
  void out(const char c, const char x);
  void out(const int i,  const char x);
  void out(const long l, const char x);
  void out(const char *st, const char x);

/* Output a newline, tab, space, '='
  ln(), tab(), space(), equals():				*/
  void ln();			// Output a newline
  void tab();			// Output a tab
  void space();			// Output a space
  void equals();		// Output char '='

  /* Output for ARDUINO PROGMEM strings.
     (Fake on Linux Pc c++ test version.)			*/
  void out_progmem(const char *str);
  void ticked(const char c);	// Output a ticked char token like 'A'

  // int men_getchar();		// Basic menu char input function

  int cb_peek() const;			// peek at next if any, else return EOL
  int cb_peek(int offset) const;	// peek at next, overnext... if any, else EOL
  void skip_spaces();			// skip leading spaces from the buffer
  int next_input_token() const;		// next non space input token if any, else EOL
  bool is_numeric() const;		// test if next token will be a numeric chiffre
  long numeric_input(long default_value);  // read a number from the buffer
  void skip_numeric_input();		// drop leading numeric sequence from the buffer

  void menu_display();			// display the menu:
					//   selected menu page,
					//   page hot keys,
					//   internal hot keys.

  // Act on buffer content tokens after seeing 'end token':
  void interpret_men_input();		// Menu input interpreter


 protected:
  //-  void menu_page_info(char pg) const; ################
  void menu_page_info(char pg);		// show a menu pages' info
  bool cb_is_full() const { return cb_count == cb_size; } // inlined: buffer full?
#ifdef DEBUGGING_CIRCBUF
  void cb_info() const;			// Debugging help
#endif
  int (*maybe_input)(void);	// maybe_input()  Must return EOF or next char
  bool (*action)(void);		// Will be called on receiving an end token

  int cb_stored() { return cb_count; }   // inlined: number of accumulated bytes?

 private:
  int cb_start;
  int cb_size;
  int cb_count;
  char * cb_buf;
  void cb_write(char value);	// Save a byte in buffer,  no checks
  char cb_read();		// Get oldest byte from the buffer.
				// Does *not* check if buffer is empty.
  STREAMTYPE & port_;		// output stream
  bool echo_switch;		// switch echo of input to output

  // Menu pages:
  char men_max;
  char men_known;
  char men_selected;
  menupage *men_pages;
};

/* **************************************************************** */
#endif
