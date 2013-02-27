/* **************************************************************** */
/*
  Menu.cpp
*/

/* **************************************************************** */
// Preprocessor #includes:

#include <stdio.h>
#include <stdlib.h>

#ifdef ARDUINO
  /* Keep ARDUINO GUI happy ;(		*/
  #if ARDUINO >= 100
    #include "Arduino.h"
  #else
    #include "WProgram.h"
  #endif

#else
  #include <iostream>
#endif

#include <Menu.h>


/* **************************************************************** */
// Constructor/Destructor:

#ifndef ARDUINO		// WARNING: Using Stream MACRO hack when not on ARDUINO!
  #define Stream ostream
#endif

Menu::Menu(int bufSize, int menuPages, int (*maybeInput)(void), Stream & port):
  cb_size(bufSize),
  maybe_input(maybeInput),
  port_(port),
  men_max(menuPages),
  cb_start(0),
  cb_count(0),
  men_selected(0),
  cb_buf(NULL),
  men_pages(NULL),
  echo_switch(false)
{
  cb_buf = (char *) malloc(cb_size);			    // ERROR handling ################
  men_pages = (menupage*) malloc(men_max * sizeof(menupage)); // ERROR handling ################
}

#ifndef ARDUINO		// WARNING: Using Stream MACRO hack when not on ARDUINO!
  #undef Stream
#endif


Menu::~Menu() {
  free(cb_buf);
  free(men_pages);
}


/* **************************************************************** */
// Basic circular buffer functions:

/*
  cb_write() save a byte to the buffer:
  does *not* check if buffer is full				*/
void Menu::cb_write(char value) {
  int end = (cb_start + cb_count) % cb_size;
  cb_buf[end] = value;
  if (cb_count == cb_size)
    cb_start = (cb_start + 1) % cb_size;
  else
    ++cb_count;
}


/*
  cb_read() get oldest byte from the buffer:
  does *not* check if buffer is empty				*/
char Menu::cb_read() {
  char value = cb_buf[cb_start];
  cb_start = (cb_start + 1) % cb_size;
  --cb_count;
  return value;
}


/* inlined:  see Menu.h
    // inlined:
    int Menu::cb_stored() const {	// returns number of buffered bytes
      return cb_count;
    }
    
    // inlined:
    bool Menu::cb_is_full() const {
      return cb_count == cb_size;
    }
*/


/* **************************************************************** */
// circ buf debugging:

#ifdef DEBUGGING_CIRCBUF
/* cb_info() debugging help					*/
void Menu::cb_info() const {
  out(F("\nBuffer:\t\t"));
  out(cb_buf);

  out(F("\n  size:\t\t"));
  out(cb_size);

  out(F("\n  count:\t"));
  out(cb_count);

  out(F("\n  start:\t"));
  outln(cb_start);

  int value = cb_peek();
  out(F("\n  char:\t\t"));
  if (value != -1) {
    out(value);
    out(F("\t"));
    out((char) value);
    if ( is_numeric() )
      out(F("  numeric CHIFFRE\n"));
    else
      out(F("  NAN\n"));
  } else
    out(F("(none)\n"));

  value = next_input_token();
  out(F("\n  next_input_token()\t: "));
  out(value);
  out(F("\t"));
  outln((char) value);
  ln();

  for (int i=0; i<=cb_count; i++) {
    value = cb_peek(i);
    out(F("  cb_peek("));
    out(i);
    out(F(")\t\t: "));
    out(value);
    out(F("\t"));
    outln((char) value);
  }

  ln();
}
#endif


/* **************************************************************** */
// #define I/O for ARDUINO:
#ifdef ARDUINO		// OUTPUT functions.

/*
  This version definines the menu INPUT routine int men_getchar();
  in the *program* not inside the Menu class.
*/
//	/* int men_getchar();		// ARDUINO version
//	   Read next char of menu input, if available.
//	   Does not block, returns EOF or char.			*/
//	int Menu::men_getchar() {		// ARDUINO version
//	  if (!Serial.available())
//	    return EOF;
//	
//	  return Serial.read();
//	}


#ifndef BAUDRATE
  #define BAUDRATE	115200	// works fine here
  // #define BAUDRATE	57600
  // #define BAUDRATE	38400
  // #define BAUDRATE	19200
  // #define BAUDRATE	9600	// failsafe
  // #define BAUDRATE	31250	// MIDI
#endif


/* void Menu::out(); overloaded menu output function family:	*/
// Simple versions  void Menu::out():
void Menu::out(const char c)	const	{ port_.print(c); }	// ARDUINO
void Menu::out(const int i)	const	{ port_.print(i); }
void Menu::out(const long l)	const	{ port_.print(l); }
void Menu::out(const char *str)	const	{ port_.print(str); }

#ifdef ARDUINO	// Arduino F() macro: out(F("string");
  void Menu::out(const __FlashStringHelper* str) const {
    port_.print(str);
  }
#endif


// End of line versions  void outln():
void Menu::outln(const char c)	const	{ port_.println(c); }
void Menu::outln(const int i)	const	{ port_.println(i); }
void Menu::outln(const long l)	const	{ port_.println(l); }
void Menu::outln(const char *str) const { port_.println(str); }

/* void ticked(char c)	Char output with ticks like 'A'	*/
void Menu::ticked(const char c) const {
  port_.print("'"); port_.print(c); port_.print("'");
}

/* Output a newline, tab, space, '='
   ln(), tab(), space(), equals():			*/
void Menu::ln()	    const { port_.println(); }	 // Output a newline
void Menu::tab()    const { port_.print('\t'); } // Output a tab
void Menu::space()  const { port_.print(' '); }	 // Output a space
void Menu::equals() const { port_.print('='); }	 // Output char '='


#if defined(ARDUINO) && defined(SHOW_FREE_RAM)	// Arduino: RAM usage
  /* int get_free_RAM(): determine free RAM on Arduino:		*/
  int Menu::get_free_RAM() const {
    int free;
    extern int __bss_end;
    extern void *__brkval;

    if ((int) __brkval == 0)
      return ((int) &free) - ((int) &__bss_end);
    else
      return ((int) &free) - ((int) __brkval);
  }
#endif


#else // not on ARDUINO: OUTPUT functions c++ Linux PC test version:
/* **************************************************************** */
// #define I/O for c++ Linux PC test version:

/*
  This version definines the menu INPUT routine int men_getchar();
  in the *program* not inside the Menu class.
*/
//	/* int men_getchar();
//	   Read next char of menu input, if available.
//	   Returns EOF or char.						*/
//	int Menu::men_getchar() { return getchar(); } // c++ Linux PC test version

/* void Menu::out(); overloaded menu output function family:	*/
// Simple versions  void Menu::out():
void Menu::out(const char c)	const	{ putchar(c); }
void Menu::out(const int i)	const	{ printf("%i", i); }
void Menu::out(const long l)	const	{ printf("%d", l); }
void Menu::out(const char *str)	const	{ printf("%s", str); }

// End of line versions  void outln():
void Menu::outln(const char c)	const	{ printf("%c\n", c); }
void Menu::outln(const int i)	const	{ printf("%i\n", i); }
void Menu::outln(const long l)	const	{ printf("%d\n", l); }
void Menu::outln(const char *str) const { printf("%s\n", str); }

/* Output a newline, tab, space, '='
   ln(), tab(), space(), equals():			*/
void Menu::ln()		const { putchar('\n'); } // Output a newline
void Menu::tab()	const { putchar('\t'); } // Output a tab	   
void Menu::space()	const { putchar(' '); }  // Output a space  
void Menu::equals()	const { putchar('='); }  // Output char '=' 

/* void ticked(char c)	Char output with ticks like 'A'	*/
void Menu::ticked(const char c)	const {   // prints 'c'
  printf("\'%c\'", c);
}

#endif	// [ ARDUINO else ]  c++ test version	OUTPUT functions.
/* **************************************************************** */


// String recycling:

// void OutOfRange(): output "out of range"
void Menu::OutOfRange()	const { out(F("out of range")); }

// void Error(): output " ERROR: "
void Menu::Error_()	const { out(F(" ERROR: ")); }


/* **************************************************************** */
// Buffer interface functions for parsing:

/*
  int cb_peek()
  return EOF if buffer is empty, else
  return next char without removing it from buffer		*/
int Menu::cb_peek() const {
  if (cb_count == 0)
    return EOF;

  return cb_buf[cb_start];
}


/*
  int cb_peek(int offset)
  like cb_peek() with offset>0
  return EOF if token does not exist
  return next char without removing it from buffer		*/
int Menu::cb_peek(int offset) const {
  if (cb_count <= offset)
    return EOF;

  return cb_buf[(cb_start + offset) % cb_size ];
}


/* void skip_spaces(): remove leading spaces from input buffer:	*/
void Menu::skip_spaces() {
  while (cb_peek() == ' ')  //  EOF != ' ' end of buffer case ok
    cb_read();
}


/* int next_input_token()
   return next non space input token if any
   else return EOF						*/
int Menu::next_input_token() const {
  int token;

  for (int offset=0; offset<cb_count; offset++) {
    switch ( token = cb_peek(offset) ) {
    case ' ':		// skip spaces
      break;
//  case '\0':		// currently not possible
//    return EOF;	// not a meaningful token
    default:
      return token;	// found a token
    }
  }
  return EOF;		// no meaningful token found
}


/* **************************************************************** */
// lurk_then_do() main Menu user interface:


/* bool lurk_then_do()
   get input byte, translate \n and \r to \0, which is 'END token'
   check for END token:
   * accumulate data bytes until receiving a '\0' == 'END token',
   * then:
     * if there's data call 'interpret_men_input()' to read and act on all data
       when done menu_display() and return true.
     * disregard END tokens on empty buffer (left over from newline translation)
   return true if and only if the interpreter was called on the buffer.
 */
bool Menu::lurk_then_do() {
  int INP;
  char c;

#if defined(DEBUGGING_MENU) || defined(DEBUGGING_LURKING)
  out(F("\nlurk_then_do():\n"));
#endif

  /* int maybe_input()  
     must be a function returning one byte data
     or if there is no input returning EOF		*/
  INP=(*maybe_input)();
#if defined(DEBUGGING_CIRCBUF) || defined(DEBUGGING_LURKING)
  out(F("got input\n"));
#endif

  if ( INP != EOF ) {	// there *is* input
    switch ( c = INP ) {

    // END token translation:
    case '\n':		// translate \n to 'END token' \0
      c = 0;
#if defined(DEBUGGING_CIRCBUF) || defined(DEBUGGING_LURKING)
      out(F("NL translated to END token\n"));
#endif
      break;

    case '\r':		// translate \r to 'END token' \0
      c = 0;
#if defined(DEBUGGING_CIRCBUF) || defined(DEBUGGING_LURKING)
      out(F("CR translated to END token\n"));
#endif
      break;

    case '\0':		// '\0' already is 'END token'
      // c = 0;
#if defined(DEBUGGING_MENU) || defined(DEBUGGING_CIRCBUF)
      out(F("\\0 received"));
#endif
      break;
    }
    if ( c ) {		// accumulate in buffer
      cb_write(c);
      if (echo_switch)
	out(c);

#if defined(DEBUGGING_MENU) || defined(DEBUGGING_CIRCBUF) || \
  defined(DEBUGGING_LURKING)
      out(F("accumulated "));
      ticked(c);
      ln();
#endif
      if (cb_is_full()) {
	// Inform user:
	out(F("buffer"));
	Error_();
	OutOfRange();
	ln();

	// try to recover
	// the fix would be to match message length and cb buffer size... 
#if defined(DEBUGGING_MENU) || defined(DEBUGGING_CIRCBUF) || \
  defined(DEBUGGING_LURKING)
	out(F("cb is full.  interpreting as EMERGENCY EXIT, dangerous...\n"));
#endif
	// EMERGENCY EXIT, dangerous...
	interpret_men_input();	// <<<<<<<< INTERPRET BUFFER CONTENT >>>>>>>>
	menu_display();
#if defined(DEBUGGING_MENU) || defined(DEBUGGING_LURKING)
	out(F("lurk_then_do() INTERPRETed INPUT BUFFER.\n"));
#endif
	return true;		// true means *reaction was triggered*.
      }
    } else {
#if defined(DEBUGGING_MENU) || defined(DEBUGGING_CIRCBUF) || \
  defined(DEBUGGING_LURKING)
      out(F("END token received. "));
      out(F("Buffer stored="));
      outln(cb_stored());
#endif

      /* end of line token translation can produce more then one \0 in sequence
	 only the first is meaningful, the others have no data in the buffer
	 so we treat only the first one (the one with the data).
      */
      if ( cb_stored() ) {	// disregard empty buffers
	if (echo_switch)
	  ln();

	interpret_men_input();	// <<<<<<<< INTERPRET BUFFER CONTENT >>>>>>>>
	menu_display();
#if defined(DEBUGGING_MENU) || defined(DEBUGGING_LURKING)
	out(F("lurk_then_do() INTERPRETed INPUT BUFFER.\n"));
#endif
	return true;		// true means *reaction was triggered*.

#if defined(DEBUGGING_CIRCBUF) || defined(DEBUGGING_LURKING)
      } else {
	out(F("(empty buffer ignored)\n"));
#endif
      }
    }

  } else {
#if defined(DEBUGGING_CIRCBUF) || defined(DEBUGGING_LURKING)
    out(F("EOF received\n"));
#endif
  }

#if defined(DEBUGGING_MENU) || defined(DEBUGGING_LURKING)
  out(F("lurk_then_do() lurking...\n"));
#endif
  return false;		// false means *no reaction was triggered*.
}


/* **************************************************************** */
/*
  numeric integer input from a chiffre sequence in the buffer
  call it with default_value, in most cases the old value.
  sometimes you might give an impossible value to check if there was input.
*/

/* bool is_numeric()
   true if there is a next numeric chiffre
   false on missing data or not numeric data			*/
bool Menu::is_numeric() const {
  int c = cb_peek();

  if ( c < '0' )
    return false;

  if ( c > '9' )
    return false;

  return true;
}

long Menu::numeric_input(long default_value) {
  long input, num, sign=1;

  skip_spaces();
  if (cb_count == 0)
    goto number_missing;	// no non-space input: return

  // check for sign:
  switch ( cb_peek() ) {
  case '-':			// switch sign
    sign = -1;			// then continue like '+'
  case '+':
      cb_read();		// remove sign from buffer
    break;
  }
  if (cb_count == 0)
    goto number_missing;	// no input after sign, return

  if ( ! is_numeric() )
    goto number_missing;	// NAN, give up...

  input = cb_read();		// read first chiffre after sign
  num = input - '0';

  // more numeric chiffres?
  while ( is_numeric() ) {
    input = cb_read();
    num = 10 * num + (input - '0');
  }

  // *if* we reach here there *was* numeric input:
  return sign * num;		// return new numeric value

  // number was missing, return the given default_value:
 number_missing:
  out(F("number missing\n"));
  return default_value;		// return default_value
}


/* drop leading numeric sequence from the buffer:		*/
void Menu::skip_numeric_input() {
  while ( is_numeric() )
    cb_read();
}


/* **************************************************************** */
// menu info:


/* menu_page_info(char pg)  show a known pages' info	*/
void Menu::menu_page_info(char pg) const {
  if ( pg == men_selected )
    out('*');
  else
    space();
  out(F("menupage ")); out((int) pg);
  tab(); out(("hotk ")); ticked(men_pages[pg].hotkey);
  tab(); out(F("group ")); ticked(men_pages[pg].active_group);
  tab(); out('"'); out(men_pages[pg].title); outln('"');
}


/* menu_pages_info()  show all known pages' info		*/
void Menu::menu_pages_info() const {
  for (char pg = 0; pg < men_known; pg++)
    menu_page_info(pg);
}


/* **************************************************************** */
// menu handling:

void Menu::add_page(const char *pageTitle, const char hotkey,		\
		     void (*pageDisplay)(void), bool (*pageReaction)(char), const char ActiveGroup) {
  if (men_known < men_max) {
    men_pages[men_known].title = (char *) pageTitle;
    men_pages[men_known].hotkey = hotkey;
    men_pages[men_known].display = pageDisplay;
    men_pages[men_known].interpret = pageReaction;
    men_pages[men_known].active_group = ActiveGroup;
    men_known++;

#ifdef DEBUGGING_MENU
    out(F("add_page(\""));
    out(pageTitle);
    out(F("\", "));
    out(hotkey);
    out(F(",..)\n"));
#endif
  } else {	// ERROR handling ################
    out(F("add_page"));
    Error_();
    OutOfRange();
    ln();
  } 
}


/* **************************************************************** */
// menu display:

/* Display menu	current menu page and common entries:		*/
void Menu::menu_display() const {
  char pg;

#ifdef DEBUGGING_MENU
  Menu::out(F("\nmenu_display():\n"));
#endif

  // men_selected page display:
  out(F("\n * ** *** MENU "));
  out(men_pages[men_selected].title);
  out(F(" *** ** *\n"));

  (*men_pages[men_selected].display)();

  // Display menu page key bindings:
  if ( men_known > 1 ) {
    ln();
    for (pg = 0; pg < men_known; pg++) {
      if ( pg != men_selected )		     // burried pages only
	if ( men_pages[pg].hotkey != ' ') {  // selectable pages only
	  out(men_pages[pg].hotkey);
	  equals();
	  out(men_pages[pg].title);
	  space(); space();
	}
    }
    ln();
  }

  // Display internal key bindings:
  out(F("'?' for menu  'e' toggle echo"));
  if (men_selected)
    out(F("  'q' quit page"));
  ln();

#if defined(ARDUINO) && defined(SHOW_FREE_RAM)	// Arduino: RAM usage
  out(F("RAM:")); space(); out((int) get_free_RAM()); ln();
#endif

#ifdef DEBUGGING_MENU
  ln();
#endif
}


/* **************************************************************** */
// menu input interpreter:

/* act on buffer content tokens after receiving 'END token':	*/
void Menu::interpret_men_input() {
  char token, pg, page_group, selected_group;
  bool did_something, is_active;

#ifdef DEBUGGING_MENU
  out(F("interpret_men_input(): '"));
#endif

  // interpreter loop over each token:
  // read all tokens:
  //   skip spaces
  //   search for first responsible menu entity:
  //     search selected menu page first
  //     search page 0 then if not done already
  //     search page key bindings
  //     search internal key bindings
  //   if no responsible menu entity is found say so, drop token, and continue.
  //   else:
  //     call the responsible interpret function
  //       the interpreter might read more input data,
  //       like numeric input or multibyte tokens.
  //
  while ( cb_count ) {	// INTERPRETER LOOP over each token
    token = cb_read();

    // try to find a menu entity that knows to interpret the token:
#ifdef DEBUGGING_MENU
    out(token);
    out(F("'\n"));
#endif

    // skip spaces:
    if ( token == ' ' )
      continue;


    // search selected page first:
    did_something = (*men_pages[men_selected].interpret)(token);
    if (did_something) {
#ifdef DEBUGGING_MENU
      out(F("selected page is responsible for '"));
      out(token);
      out(F("'.\n"));
#endif
      continue;
    }
#ifdef DEBUGGING_MENU
    out(F("selected page does not know '"));
    out(token);
    out(F("'\n"));
#endif
    // token not found yet...


    // check pages in same page_group and in group '+':
    selected_group = men_pages[men_selected].active_group;
#ifdef DEBUGGING_MENU
    out(F("check for selected_group '"));
    out(selected_group);
    out(F("':\n"));
    out(F("  going through the pages:\n"));
#endif

    for (pg = 0; pg < men_known; pg++) {
      if (pg == men_selected )	// we already searched men_selected
	continue;

      is_active=false;
      page_group = men_pages[pg].active_group;
#ifdef DEBUGGING_MENU
      out(F("   "));
      menu_page_info(pg);
#endif
      switch ( page_group ) {
      case '-':			// '-' means *never*
	break;
      case '+':			// '+' means always on
	is_active=true;
#ifdef DEBUGGING_MENU
	out(F("==>*  joker '+':\n"));
#endif
	break;
      default:			// else: active if in selected pages' group
	if ( page_group == selected_group ) {
	  is_active=true;
#ifdef DEBUGGING_MENU
	  out(F("==>*  page group match "));
	  menu_page_info(pg);
#endif
	}
      }
      if (is_active) {		// test active menu pages on token:
	if ( did_something = (*men_pages[pg].interpret)(token) ) {
#ifdef DEBUGGING_MENU
	  out(F("page_group '"));
	  out(page_group);
	  out(F("':\n"));
	  out(F("menu "));
	  out(men_pages[pg].title);
	  out(F(" knows '"));
	  out(token);
	  out(F("'.\n"));
#endif
	  break;
	}
      }// is_active
    }// loop over other pages

    if (did_something)
      continue;
    // token not found yet...


    // search menu page hotkeys:
#ifdef DEBUGGING_MENU
    out(F("search menu page hotkeys:\n"));
#endif
    for (pg = 0; pg < men_known; pg++) {
      if (token == men_pages[pg].hotkey) {
	(*men_pages[pg].interpret)(token);	// *might* do more, return is irrelevant
	men_selected = pg;			// switch to page
	did_something = true;			// yes, did switch
#ifdef DEBUGGING_MENU
    	out(F("==>* "));
	menu_page_info(pg);
#endif
	break;
      }
    }
    if (did_something) {
#ifdef DEBUGGING_MENU
      out(F("==>* switch to "));
      menu_page_info(men_selected);
#endif
      continue;
    }
    // token not found yet...


    // check for internal bindings next:
#ifdef DEBUGGING_MENU
    out(F("search internal key bindings:\n"));
#endif
    switch (token) {
    case '?':
      menu_pages_info();
      // menu_display();	will be called anyway...
      did_something = true;
      break;

    case 'q':
      men_selected = 0;
#ifdef DEBUGGING_MENU
      out(F("==>* switch to "));
      menu_page_info(men_selected);
#endif
      did_something = true;
      break;

    case 'e':	// toggle echo
      echo_switch = !echo_switch;
      did_something = true;
      break;

    }
#ifdef DEBUGGING_MENU
    if (did_something) {
      out(F("==>* internal hotkey '"));
      out(token);
      out(F("'.\n"));
    }
#endif

    // token still not found, give up...
    if (! did_something ) {
      out(F("unknown token "));
      ticked(token);
      tab();
      out((int) token);
      ln();
    }

  } // interpreter loop over all tokens
}
/* **************************************************************** */
