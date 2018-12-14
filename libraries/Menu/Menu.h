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

  #ifndef STREAMTYPE		// could have been #define'd in Pulses.h
    using namespace std;
    ostream & Serial=cout;	// nice trick from johncc
  #endif
  #define STREAMTYPE	ostream
#endif


/* **************************************************************** */
// verbosity:	Levels of menu feedback:
#define VERBOSITY_LOWEST	1
#define VERBOSITY_SOME		2
#define VERBOSITY_MORE		3
#define VERBOSITY_HIGH		4

#define MENU_MENU_PAGES_KEY	':'	// menu token for menupage switch
/* **************************************************************** */
class Pulses;

/* **************************************************************** */
/* GET_FREE_RAM  and  USE_F_MACRO
 * compile a working Menu::get_free_RAM() function or just a dummy?
 * use Arduino F() macro to save RAM or a noop?
 */
#ifdef ARDUINO

  // GET_FREE_RAM
  // compile a working Menu::get_free_RAM() function?
  #ifdef __SAM3X8E__
    #warning "not defining get_free_RAM() on Arduino Due."
    #undef GET_FREE_RAM
  #else
    #define GET_FREE_RAM
  #endif

  // USE_F_MACRO
  // normally we *do* use F() macro on Arduino to save RAM:
  #define USE_F_MACRO	// comment out for RAM tests	// FIXME: ???, really?

  #if defined(ESP32)	// FIXME: F() on ESP32	################
    #undef USE_F_MACRO
  #endif

#else	// NO GET_FREE_RAM on PC	(*ARDUINO ONLY*)

  // GET_FREE_RAM
  #ifdef GET_FREE_RAM
    #undef GET_FREE_RAM
    #warning GET_FREE_RAM *ARDUINO ONLY* #undef
  #endif

#endif	// ARDUINO or PC


// use Arduino F() macro to save RAM or just a NOOP?
#ifndef USE_F_MACRO	// NOOP fake
  // For tests and on PC:  Fake Arduino F("string") macro as NOOP:
  #undef F
  #define F(s)	(s)
  #warning "using a NOOP fake F() macro."
#endif

/* **************************************************************** */
// Preprocessor macro logic:

/* DEBUGGING macros:		*/
#ifdef DEBUGGING_ALL
  #define DEBUGGING_CIRCBUF
  #define DEBUGGING_LURKING
  #define DEBUGGING_MENU
#endif

#ifdef DEBUGGING_CIRCBUF
  #define DEBUGGING_LURKING
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
/* struct menupage for Menu::menupage *men_pages[]:		*/

struct menupage {
  void (*display)(void);
  bool (*interpret)(char token);
  char *title;
  char page_key;
  // Configure which pages burried below men_selected remain active:
  char active_group;	// tokens on pages with the same active_group
			// as men_selected remain active below page actions
			// '+' and  '-' have special meanings:
			// '+' means *always* active,  '-' *never* active
};


/* **************************************************************** */
/* class Menu {}							*/

class Menu {
 public:
  ~Menu();

/* high level API:							*/

  // construct a Menu:
  Menu(int size, int menuPages, int (*maybeInput)(void), STREAMTYPE & port, STREAMTYPE & port2);

  // "run-through" routine for main loop:
  bool lurk_then_do(void);

  // add a menu page, return page number or -1
  int add_page(const char *pageTitle, const char page_key, \
		void (*pageDisplay)(), bool (*pageReaction)(char), \
		const char ActiveGroup);

  char men_selected;			// selected menu page

  void flush() const { port_.flush(); }	// flush menu output, inlined.

  int cb_peek() const;			// peek at next if any, else return EOL
  int cb_peek(int offset) const;	// peek at next, overnext... if any, else EOL
  unsigned int skip_spaces();			// skip leading spaces from the buffer
  int next_input_token() const;		// next non space input token if any, else EOL
  bool is_numeric() const;		// test if next token will be a numeric chiffre
  long numeric_input(long default_value);  // read a number from the buffer
  void skip_numeric_input();		// drop leading numeric sequence from the buffer

  void menu_page_info(char pg)	const;	// show a menu pages' info
  void menu_pages_info()	const;	// show all known pages' info

  void menu_display() const;		// display the menu:
					//   selected menu page,
					//   page hot keys,
					//   internal hot keys.

  // out(any );	  overloaded menu output function family:
  // Simple versions  void Menu::out():
  void out(const char c)	const;	// char output
  void out(const int i)		const;	// int output
  void out(const long l)	const;	// long output
  void out(const char *str)	const;	// c-style character string
  void out(const String s)	const;	// c++ String

#ifndef INTEGER_ONLY
  void out(const double d)	const;	// double float
#endif

  // unsigned versions:
  void out(const unsigned char c)	const; // unsigned char output
  void out(const unsigned int i)	const; // i.e. pointers on Arduino
  void out(const unsigned long l)	const; // i.e. pointers on Linux

  // End of line versions  void outln():
  void outln(const char c)	const;	// char output and newline
  void outln(const int i)	const;	// int output  and newline
  void outln(const long l)	const;	// long output and newline
  void outln(const char *str)	const;	// c-style character string and newline
  void outln(const String s)	const;	// c++ String

  void outln(void) { ln(); }		// outln(void), inlined

#ifndef INTEGER_ONLY
  void outln(const double d)	const;	// double float
#endif

  // unsigned versions:
  void outln(const unsigned char c)	const;	// unsigned char and newline
  void outln(const unsigned int i)	const;	// unsigend int  and newline
  void outln(const unsigned long l)	const;	// unsigned long and newline

  // Print a value and pad with spaces to field width 'pad'.
  // At least one space will always be printed.
  void pad(const long value, unsigned int len) const;

#ifdef USE_F_MACRO
  // *DO* use Arduino F() MACRO for string output to save RAM:

  void out(const __FlashStringHelper*) const; // Arduino macro: F("string")

  // End of line version:
  void outln(const __FlashStringHelper*) const; // Arduino macro: F("string")
#endif

  void out(const float f, int places)	const;	// formatted float output

  void ticked(const char c) const;  // output a ticked char token like 'A'

  /* Do i want this?
  // show char sequence ticked, like 'A' 'B' 'C' (ending in a space)
  void ticked_chars(const char * chars) const;
  */

  // Print binary numbers with leading zeroes and a trailing space:
  void outBIN(unsigned long l, int bits ) const; // binary output

  // show one letter mnemonics for flag state
  void show_flag_mnemonics(unsigned long l, int bits, char* ON_chars, char* OFF_chars) const;

  // Print a hex chiffre:
  void out_hex_chiffre(unsigned char chiffre) const; // output 1 hex chiffre

  // Print a uint32_t in hex:
  void out_hex(uint32_t n) const;		// uint32_t  hex output

  /*
  void out_ticked_hexs(unsigned int count) const;
  Print a sequence of ticked ascending hex chiffres: '0' '1' ... '9' 'a' 'b'
  Ends with a space.
  */
  void out_ticked_hexs(unsigned int count) const;

  // Print a bar_graph line:
  void bar_graph(long value, unsigned int scale, char c);

  /* String recycling:						*/
  void OutOfRange() const;	// output "out of range\n"
  void out_Error_() const;	// output "ERROR: "
  void error_ln(const char * str) const; // output "ERROR: xxxxxxxx"
  void out_selected_() const;	// output "selected "
  void out_flags_() const;	// output "\tflags ";

/* Output a newline, tab, space, '='
  ln(), tab(), space(), equals():				*/
  void ln()	const;		// output a newline
  void tab()	const;		// output a tab
  void tab(unsigned int n) const;  // output n tabs
  void space()	const;		// output a space
  void space(unsigned int n)	const;		// output n spaces
  void equals()	const;		// output char '='
  void slash()	const;		// output char '/'

  void IPstring(int ip) const;	// output an ip like "192.168.0.1"
  void out_ON_off(bool flag);	// output either "ON" or "off" depending flag

  /* Output Control						*/
  bool maybe_display_more(unsigned char verbosity_level=VERBOSITY_SOME); // avoid too much output

/* Input							*/
  int is_chiffre(char token);			// is token a chiffre?
  char is_operator(char token);			// known operator?
  bool get_numeric_input(unsigned long *result);	// if there's a number, read it
  bool maybe_calculate_input(unsigned long *result);	// check for & calculate positive integer input
							// calculates simply left to right

  bool string_match(const char *test_str);			// c-style character string
//  bool string_match(const String test_str);			// c++ String
//  bool string_match(const __FlashStringHelper* test_str);	// Arduino macro: F("string")

/* int get_free_RAM(): determine RAM usage, maybe only a dummy	*/
  int get_free_RAM() const;
  void print_free_RAM() const;

  // Levels of menu feedback:
  unsigned char verbosity;
  /* verbosity codes:
    #define VERBOSITY_LOWEST	1
       errors and *requested* informations
    #define VERBOSITY_SOME	2
       include feedback, i.e. new values
    #define VERBOSITY_MORE	3
       some more infos, for novice users
    #define VERBOSITY_HIGH	4
       more than you want to see
  */
  void verbosity_info();	// helper function
  void outln_invalid();		// displays '(invalid)'
  void out_noop();		// displays 'noop'

  int drop_input_token();	// drops and return next input token, only use if you must
  int restore_input_token();	// restore and return last input token, only use if you *really must*
				// *dangerous* no checks, you *can* restore a just-read-token

  void play_KB_macro(char *macro, bool newline=true);	//  keyboard input from a string
				// most often you might do  'men_selected=0;'  before

  int menu_mode;		// currently only used to edit data arrays
  friend class Pulses;

 protected:
  // Act on buffer content tokens after seeing 'end token':
  bool try_page_reaction(char pg, char token);	// interpreter factor
  void interpret_men_input();		// Menu input interpreter

  bool cb_is_full() const { return cb_count == cb_size; } // inlined: buffer full?
#ifdef DEBUGGING_CIRCBUF
  void cb_info() const;			// debugging help
#endif
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
  STREAMTYPE & port_;		// output stream	Serial
  STREAMTYPE & port2_;		// output stream2	BLUEtoothSerial	   WiFi telnet client
  bool echo_switch;		// switch echo of input to output

  // Menu pages:
  char men_max;			// maximal number of menu pages
  char men_known;		// initialized pages
  menupage *men_pages;		// pages' data
};

/* **************************************************************** */
#endif
