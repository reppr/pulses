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

#include "menu2.h"


/* **************************************************************** */
// constructor/destructors:

Menu2::Menu2(int bufSize, int (*maybeInput)(void)) {
#ifdef DEBUGGING_CLASS
  std::cout << "Menu2 CONSTRUCTOR: cb_size=";
  std::cout << bufSize << "\n";
#endif

  // initialize circular input buffer:
  cb_start = 0;
  cb_size  = bufSize;
  cb_buf = (char *) malloc(cb_size);	// could fail ################
  cb_count = 0;

  maybe_input = maybeInput;

  //  menu_display();			// display menu on startup
}


Menu2::~Menu2() {
#ifdef DEBUGGING_CLASS
  std::cout << "Menu2 DESTRUCTOR\n";
#endif

  free(cb_buf);
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
  std::cout << "\nBuffer:\t\t";
  std::cout << (long) cb_buf;

  std::cout << "\n  size:\t\t";
  std::cout << cb_size;

  std::cout << "\n  count:\t";
  std::cout << cb_count;

  std::cout << "\n  start:\t";
  std::cout << cb_start;
  std::cout << "\n";

  int value = cb_peek();
  std::cout << "\n  char:\t\t";
  if (value != -1) {
    std::cout << value << "\t" << (char) value;
    if ( is_numeric() )
      std::cout << "  numeric CHIFFRE\n";
    else
      std::cout << "  NAN\n";
  } else
    std::cout << "(none)\n";

  value = next_input_token();
  std::cout << "\n  next_input_token()\t: " << value << "\t" << (char) value << "\n";
  std::cout << "\n";

  for (int i=0; i<=cb_count; i++) {
    value = cb_peek(i);
    std::cout << "  cb_peek(" << i << ")\t\t: " << value << "\t" << (char) value << "\n";
  }

  std::cout << "\n";
}
#endif


/* **************************************************************** */
// lurk_and_do() main menu2 user interface:

/* bool lurk_and_do(bool (void)
   get input byte, translate \n and \r to \0, which is 'END token'
   check for END token:
   * accumulate data bytes
   * \0 = 'END token':
################################################################
     * if there's data call 'ProgramAction()' to read and act on all data
       and return true.
     * disregard END tokens on empty buffer (left over from newline translation)
   Return true if and only if action was taken.
 */
bool Menu2::lurk_and_do() {
  int INP;
  char c;

#if defined(DEBUGGING_MENU) || defined(DEBUGGING_LURKING)
  std::cout << "\nrunning lurk_and_do()\n";
#endif

  /* int maybe_input()  
     must be a function returning one byte data
     or if there is no input returning EOF		*/
  INP=(*maybe_input)();

#if defined(DEBUGGING_CIRCBUF) || defined(DEBUGGING_LURKING)
  std::cout << "got input\n";
#endif

  if ( INP != EOF ) {	// there *is* input
    switch ( c = INP ) {

    // END token translation:
    case '\n':		// translate \n to 'END token' \0
      c = 0;
#if defined(DEBUGGING_CIRCBUF) || defined(DEBUGGING_LURKING)
      std::cout << "NL translated to END token\n";
#endif
      break;

    case '\r':		// translate \r to 'END token' \0
      c = 0;
#if defined(DEBUGGING_CIRCBUF) || defined(DEBUGGING_LURKING)
      std::cout << "CR translated to END token\n";
#endif
      break;

    case '\0':		// '\0' already is 'END token'
      // c = 0;
#if defined(DEBUGGING_MENU) || defined(DEBUGGING_CIRCBUF)
      std::cout << "\\0 received";
#endif
      break;
    }
    if ( c ) {		// accumulate in buffer
      cb_write(c);
#if defined(DEBUGGING_CIRCBUF) || defined(DEBUGGING_LURKING)
      std::cout << "accumulated " << c << "\n";
#endif

    } else {

#if defined(DEBUGGING_MENU) || defined(DEBUGGING_CIRCBUF) || \
  defined(DEBUGGING_LURKING)
      std::cout << "END token received. ";
      std::cout << "Buffer stored=" << cb_stored() << "\n";
#endif

      /* end of line token translation can produce more then one \0 in sequence
	 only the first is meaningful, the others have no data in the buffer
	 so we treat only the first one (the one with the data).
      */
      if ( cb_stored() ) {	// disregard empty buffers
	do_menu_actions();	// <<<<<<<< ACT ON BUFFER CONTENT >>>>>>>>
	menu_display();
	return true;		// true means *reaction was triggered*.

#if defined(DEBUGGING_CIRCBUF) || defined(DEBUGGING_LURKING)
      } else {
	std::cout << "(empty buffer ignored)\n";
#endif
      }
    }

  } else {
#if defined(DEBUGGING_CIRCBUF) || defined(DEBUGGING_LURKING)
      std::cout << "EOF received\n";
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

const unsigned char number_missing[] MAYBE_PROGMEM = \
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
  std::cout << number_missing;
  return default_value;		// return default_value
}


/* drop leading numeric sequence from the buffer:		*/
void Menu2::skip_numeric_input() {
  while ( is_numeric() )
    cb_read();
}


/* **************************************************************** */
// menu handling:

void Menu2::add_page(char *pageTitle, char token,		\
	      void (*pageDisplay)(void), bool (*pageReaction)(char)) {
  page_title = pageTitle;
  page_token = token;
  display_page = pageDisplay;
  page_reaction = pageReaction;

#ifdef DEBUGGING_MENU
  std::cout << "add_page(\"" << pageTitle << "\", " << token << ",..)\n";
  std::cout << "________________________________________________\n";
  (*display_page)();
  std::cout << "________________________________________________\n";
#endif
}

/* **************************************************************** */
// menu display:

/* void common_display()
   display menu items common to all menus:			*/

const unsigned char common_[] MAYBE_PROGMEM = \
  "\nPress 'm' or '?' for menu  'e' toggle echo";

const unsigned char _quit[] MAYBE_PROGMEM = \
  "  'q' quit this menu";

/*
  const unsigned char program_[] MAYBE_PROGMEM = \
    " 'P' program menu ";
*/


/* display common menu entries:					*/
void Menu2::common_display() {
  std::cout << common_;
//if (menu != MENU_CODE_UNDECIDED)
    std::cout << _quit;

  std::cout << "\n";
}


const unsigned char internalKeys[] MAYBE_PROGMEM = "\n'?' for menu";
const unsigned char qQuit[] MAYBE_PROGMEM = "  'q' quit page";

/* display menu	current menu page and common entries:		*/
void Menu2::menu_display() {
#ifdef DEBUGGING_MENU
  std::cout << "\nmenu_display():\n";
#endif

  // page_display();
  std::cout << "\nMENU " << page_title << "\n";
  (*display_page)();

  // internal key bindings:
  std::cout << internalKeys << qQuit << "\n";
  
#ifdef DEBUGGING_MENU
  std::cout << "\n";
#endif
}


/* **************************************************************** */
// menu action:

const unsigned char unknownToken[] MAYBE_PROGMEM = "unkown token ";

/* act on buffer content tokens after receiving 'END token':	*/
void Menu2::do_menu_actions() {
  char token;
  bool did_something;

#ifdef DEBUGGING_MENU
  std::cout << "\ndo_menu_actions(" << token << "):\n";
#endif

  while ( cb_count ) {
    token = cb_read();
#ifdef DEBUGGING_MENU
    std::cout << token;
#endif

    // skip spaces:
    if ( token == ' ' )
      continue;

    // program menu first:
    did_something = (*page_reaction)(token);

    if ( ! did_something ) {
#ifdef DEBUGGING_MENU
      std::cout << "program menu page does not know '" << token << "'\n";
#endif

      // check for internal bindings:
        switch (token) {
	case '?':
	  menu_display();
	  did_something=true;
	  break;

	case 'q':
	  did_something=true;
	  break;
	}


    } else {
#ifdef DEBUGGING_MENU
      std::cout << "program menu page responsible for '" << token << "'\n";
#endif
    }
  }

  if (! did_something ) {
    std::cout << unknownToken << token << "\n";
  }

}
/* **************************************************************** */
