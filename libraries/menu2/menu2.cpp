/* **************************************************************** */
/*
  menu2.cpp
  menu2 as monolithic base class
*/

/* **************************************************************** */
#include <stdlib.h>
#include <iostream>


// switch prepared to compile Arduino sketches
#define MAYBE_PROGMEM
// #define MAYBE_PROGMEM	PROGMEM

/* outMACRO  macro for stream output:	*/
#ifndef outMACRO
  /* outMACRO for testing on c++		*/
  #define outMACRO	std::cout

  /* outMACRO for Arduino		*/	// TODO ################
  // #include ################
  // #define outMACRO	Serial::cout
#endif

#include "menu2.h"


/* **************************************************************** */
// constructor/destructors:

Menu2::Menu2(int bufSize, int menuPages, int (*maybeInput)(void)) {
#ifdef DEBUGGING_CLASS
  outMACRO << "Menu2 CONSTRUCTOR: cb_size=";
  outMACRO << bufSize << "\n";
#endif

  // initialize circular input buffer:
  cb_start = 0;
  cb_size  = bufSize;
  cb_buf = (char *) malloc(cb_size);	    // ERROR handling ################

  cb_count = 0;

  maybe_input = maybeInput;

  men_selected = 0;
  men_max = menuPages;

  men_pages = (menupage*) malloc(men_max * sizeof(menupage)); // ERROR handling ################
}


Menu2::~Menu2() {
#ifdef DEBUGGING_CLASS
  outMACRO << "Menu2 DESTRUCTOR\n";
#endif

  free(cb_buf);
  free(men_pages);
}


/* **************************************************************** */
// basic circular buffer functions:

/*
  cb_write() save a byte to the buffer:
  does *not* check if buffer is full				*/
void Menu2::cb_write(char value) {
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
char Menu2::cb_read() {
  char value = cb_buf[cb_start];
  cb_start = (cb_start + 1) % cb_size;
  --cb_count;
  return value;
}


/* inlined:  see menu2.h
    // inlined:
    int Menu2::cb_stored() {	// returns number of buffered bytes
      return cb_count;
    }
    
    // inlined:
    int Menu2::cb_is_full() {
      return cb_count == cb_size;
    }
*/


/* **************************************************************** */
// buffer interface functions for parsing:

/*
  int cb_peek()
  return EOF if buffer is empty, else
  return next char without removing it from buffer		*/
int Menu2::cb_peek() const {
  if (cb_count == 0)
    return EOF;

  return cb_buf[cb_start];
}


/*
  int cb_peek(int offset)
  like cb_peek() with offset>0
  return EOF if token does not exist
  return next char without removing it from buffer		*/
int Menu2::cb_peek(int offset) const {
  if (cb_count <= offset)
    return EOF;

  return cb_buf[(cb_start + offset) % cb_size ];
}


/* void skip_spaces(): remove leading spaces from input buffer:	*/
void Menu2::skip_spaces() {
  while (cb_peek() == ' ')  //  EOF != ' ' end of buffer case ok
    cb_read();
}


/* int next_input_token()
   return next non space input token if any
   else return EOF						*/
int Menu2::next_input_token() const {
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
// circ buf debugging:

#ifdef DEBUGGING_CIRCBUF
/* cb_info() debugging help					*/
void Menu2::cb_info() const {
  outMACRO << "\nBuffer:\t\t";
  outMACRO << (long) cb_buf;

  outMACRO << "\n  size:\t\t";
  outMACRO << cb_size;

  outMACRO << "\n  count:\t";
  outMACRO << cb_count;

  outMACRO << "\n  start:\t";
  outMACRO << cb_start;
  outMACRO << "\n";

  int value = cb_peek();
  outMACRO << "\n  char:\t\t";
  if (value != -1) {
    outMACRO << value << "\t" << (char) value;
    if ( is_numeric() )
      outMACRO << "  numeric CHIFFRE\n";
    else
      outMACRO << "  NAN\n";
  } else
    outMACRO << "(none)\n";

  value = next_input_token();
  outMACRO << "\n  next_input_token()\t: " << value << "\t" << (char) value << "\n";
  outMACRO << "\n";

  for (int i=0; i<=cb_count; i++) {
    value = cb_peek(i);
    outMACRO << "  cb_peek(" << i << ")\t\t: " << value << "\t" << (char) value << "\n";
  }

  outMACRO << "\n";
}
#endif


/* **************************************************************** */
// lurk_and_do() main menu2 user interface:

/* bool lurk_and_do()
   get input byte, translate \n and \r to \0, which is 'END token'
   check for END token:
   * accumulate data bytes until receiving a '\0' == 'END token',
   * then:
     * if there's data call 'interpret_men_input()' to read and act on all data
       when done menu_display() and return true.
     * disregard END tokens on empty buffer (left over from newline translation)
   return true if and only if the interpreter was called on the buffer.
 */
bool Menu2::lurk_and_do() {
  int INP;
  char c;

#if defined(DEBUGGING_MENU) || defined(DEBUGGING_LURKING)
  outMACRO << "\nrunning lurk_and_do()\n";
#endif

  /* int maybe_input()  
     must be a function returning one byte data
     or if there is no input returning EOF		*/
  INP=(*maybe_input)();

#if defined(DEBUGGING_CIRCBUF) || defined(DEBUGGING_LURKING)
  outMACRO << "got input\n";
#endif

  if ( INP != EOF ) {	// there *is* input
    switch ( c = INP ) {

    // END token translation:
    case '\n':		// translate \n to 'END token' \0
      c = 0;
#if defined(DEBUGGING_CIRCBUF) || defined(DEBUGGING_LURKING)
      outMACRO << "NL translated to END token\n";
#endif
      break;

    case '\r':		// translate \r to 'END token' \0
      c = 0;
#if defined(DEBUGGING_CIRCBUF) || defined(DEBUGGING_LURKING)
      outMACRO << "CR translated to END token\n";
#endif
      break;

    case '\0':		// '\0' already is 'END token'
      // c = 0;
#if defined(DEBUGGING_MENU) || defined(DEBUGGING_CIRCBUF)
      outMACRO << "\\0 received";
#endif
      break;
    }
    if ( c ) {		// accumulate in buffer
      cb_write(c);
#if defined(DEBUGGING_MENU) || defined(DEBUGGING_CIRCBUF) || \
  defined(DEBUGGING_LURKING)
      outMACRO << "accumulated '" << c << "'\n";
#endif

    } else {

#if defined(DEBUGGING_MENU) || defined(DEBUGGING_CIRCBUF) || \
  defined(DEBUGGING_LURKING)
      outMACRO << "END token received. ";
      outMACRO << "Buffer stored=" << cb_stored() << "\n";
#endif

      /* end of line token translation can produce more then one \0 in sequence
	 only the first is meaningful, the others have no data in the buffer
	 so we treat only the first one (the one with the data).
      */
      if ( cb_stored() ) {	// disregard empty buffers
	interpret_men_input();	// <<<<<<<< INTERPRET BUFFER CONTENT >>>>>>>>
	menu_display();
	return true;		// true means *reaction was triggered*.

#if defined(DEBUGGING_CIRCBUF) || defined(DEBUGGING_LURKING)
      } else {
	outMACRO << "(empty buffer ignored)\n";
#endif
      }
    }

  } else {
#if defined(DEBUGGING_CIRCBUF) || defined(DEBUGGING_LURKING)
      outMACRO << "EOF received\n";
#endif
  }

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
bool Menu2::is_numeric() const {
  int c = cb_peek();

  if ( c < '0' )
    return false;

  if ( c > '9' )
    return false;

  return true;
}

const unsigned char numberMissing[] MAYBE_PROGMEM = \
  "number missing";

long Menu2::numeric_input(long default_value) {
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
  num = input;

  // more numeric chiffres?
  while ( is_numeric() ) {
    input = cb_read();
    num = 10 * num + (input - '0');
  }

  // *if* we reach here there *was* numeric input:
  return sign * num;		// return new numeric value

  // number was missing, return the given default_value:
 number_missing:
  outMACRO << numberMissing;
  return default_value;		// return default_value
}


/* drop leading numeric sequence from the buffer:		*/
void Menu2::skip_numeric_input() {
  while ( is_numeric() )
    cb_read();
}


/* **************************************************************** */
// menu handling:

const unsigned char addPg[] MAYBE_PROGMEM = "add_page";
const unsigned char outOfRange[] MAYBE_PROGMEM = "out of range";
const unsigned char error_[] MAYBE_PROGMEM = " ERROR: ";

void Menu2::add_page(char *pageTitle, char ptoken,		\
		     void (*pageDisplay)(void), bool (*pageReaction)(char), const char ActiveGroup) {
  if (men_known < men_max) {
    men_pages[men_known].title = pageTitle;
    men_pages[men_known].ptoken = ptoken;
    men_pages[men_known].display = pageDisplay;
    men_pages[men_known].interpret = pageReaction;
    men_pages[men_known].active_group = ActiveGroup;
    men_known++;

#ifdef DEBUGGING_MENU
    outMACRO << addPg << "(\"" << pageTitle << "\", " << ptoken << ",..)\n";
#endif
  } else {
    outMACRO << addPg << error_ << outOfRange << "\n";	// ERROR handling ################
  } 
}


/* **************************************************************** */
// menu display:

const unsigned char internalKeys[] MAYBE_PROGMEM = "\n'?' for menu";
const unsigned char qQuit[] MAYBE_PROGMEM = "  'q' quit page";
const unsigned char deco_[] MAYBE_PROGMEM = "\n * ** *** ";
const unsigned char _deco[] MAYBE_PROGMEM = " *** ** *\n";
const unsigned char men_[] MAYBE_PROGMEM = "MENU ";

/* display menu	current menu page and common entries:		*/
void Menu2::menu_display() {
  char pg;

#ifdef DEBUGGING_MENU
  outMACRO << "\nmenu_display():\n";
#endif

  // men_selected page display:
  outMACRO << deco_;
  outMACRO << men_ << men_pages[men_selected].title;
  outMACRO << _deco;

  (*men_pages[men_selected].display)();

  // display menu page key bindings:
  if ( men_known > 1 ) {
    outMACRO << "\n";
    for (pg = 0; pg < men_known; pg++) {
      if ( pg != men_selected )	// omit selected pages' hot key display, even if active.
	outMACRO << men_pages[pg].ptoken << "=" << men_pages[pg].title << "  ";
    }
    outMACRO << "\n";
  }

  // display internal key bindings:
  outMACRO << internalKeys << qQuit << "\n";

#ifdef DEBUGGING_MENU
  outMACRO << "\n";
#endif
}


/* **************************************************************** */
// menu input interpreter:

const unsigned char unknownToken[] MAYBE_PROGMEM = "unkown token ";

/* act on buffer content tokens after receiving 'END token':	*/
void Menu2::interpret_men_input() {
  char token, pg, page_group, selected_group;
  bool did_something, is_active;

#ifdef DEBUGGING_MENU
  outMACRO << "\ninterpret_men_input(): '";
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
    outMACRO << token << "'\n";
#endif

    // skip spaces:
    if ( token == ' ' )
      continue;


    // search selected page first:
    did_something = (*men_pages[men_selected].interpret)(token);
    if (did_something) {
#ifdef DEBUGGING_MENU
      outMACRO << "selected page is responsible for '" << token << "'\n";
#endif
      continue;
    }
#ifdef DEBUGGING_MENU
    outMACRO << "selected page does not know '" << token << "'\n";
#endif
    // token not found yet...


    // check pages in same page_group and in group '+':
    selected_group = men_pages[men_selected].active_group;
#ifdef DEBUGGING_MENU
    outMACRO << "check selected_group '" << selected_group << "':\n";
#endif
    //  if ( selected_group != '-' ) {
    if (true) {
      for (pg = 0; pg < men_known; pg++) {
	if (pg == men_selected )	// we already searched men_selected
	  continue;

	is_active=false;
	page_group = men_pages[pg].active_group;
#ifdef DEBUGGING_MENU
	outMACRO << "checking page " << men_pages[pg].title \
		  << " group '" << page_group << "'\n";
#endif
	switch ( page_group ) {
	case '-':			// '-' means *never*
	  break;
	case '+':			// '+' means always on
	  is_active=true;
#ifdef DEBUGGING_MENU
	  outMACRO << men_pages[pg].title << " joker '+' matches\n";
#endif
	  break;
	default:			// else: active if in selected pages' group
	  if ( page_group == selected_group ) {
	    is_active=true;
#ifdef DEBUGGING_MENU
	    outMACRO << "page " << men_pages[pg].title << " matches\n";
#endif
	  }
	}
	if (is_active) {		// test active menu pages on token:
	  if ( did_something = (*men_pages[pg].interpret)(token) ) {
#ifdef DEBUGGING_MENU
	    outMACRO << "page_group '" << page_group << "':\n";
	    outMACRO << "menu " << men_pages[pg].title << " knows '" << token << "'\n";
#endif
	    break;
	  }
	}// is_active
      }// loop over other pages
      if (did_something)
	continue;
    }// selected_group != '-'
    // token not found yet...


    // search menu page tokens:
#ifdef DEBUGGING_MENU
    outMACRO << "search menu page tokens:\t";
#endif
    for (pg = 0; pg < men_known; pg++) {
      if (token == men_pages[pg].ptoken) {
	(*men_pages[pg].interpret)(token);	// *might* do more, return is irrelevant
	men_selected = pg;			// switch to page
	did_something = true;			// yes, did switch
	break;
      }
    }
    if (did_something) {
#ifdef DEBUGGING_MENU
      outMACRO << "switched to page " << men_pages[men_selected].title << "\n";
#endif
      continue;
    }
#ifdef DEBUGGING_MENU
    else 
      outMACRO << "(none)\n";
#endif
    // token not found yet...


    // check for internal bindings next:
#ifdef DEBUGGING_MENU
    outMACRO << "search internal key bindings:\t";
#endif
    switch (token) {
    case '?':
      // menu_display();	will be called anyway...
      did_something = true;
      break;

    case 'q':
      men_selected = 0;
      did_something = true;
      break;
    }

#ifdef DEBUGGING_MENU
    if (did_something)
      outMACRO << "found internal binding '" << token << "'\n";
    else
      outMACRO << "(nope)\n";
#endif

    // token still not found, give up...
    if (! did_something ) {
      outMACRO << unknownToken << token << "\n";
    }

  } // interpreter loop over all tokens
}
/* **************************************************************** */
