/*
  menu2.cpp
  menu2 as monolithic base class
*/

#include <stdlib.h>
#include <iostream>

// switch prepared to compile Arduino sketches
#define MAYBE_PROGMEM
// #define MAYBE_PROGMEM	PROGMEM

#include "menu2.h"


Menu2::Menu2(int bufSize, int (*maybeInput)(void)) {
#ifdef DEBUGGING
  std::cout << "Menu2 CONSTRUCTOR: cb_size=";
  std::cout << bufSize << "\n";
#endif

  // initialize circular input buffer:
  cb_start = 0;
  cb_size  = bufSize;
  cb_buf = (char *) malloc(cb_size);	// could fail ################
  cb_count = 0;

  maybe_input = maybeInput;

  menu_display();			// display menu on startup
}


Menu2::~Menu2() {
#ifdef DEBUGGING
  std::cout << "Menu2 DESTRUCTOR\n";
#endif

  free(cb_buf);
}


/*
  cb_write() save a byte to the buffer:
  does *not* check if buffer is full
*/
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
  does *not* check if buffer is empty
*/
char Menu2::cb_read() {
  char value = cb_buf[cb_start];
  cb_start = (cb_start + 1) % cb_size;
  --cb_count;
  return value;
}


/*
  int cb_peek()
  return EOF if buffer is empty, else
  return next char without removing it from buffer
*/
int Menu2::cb_peek() const {
  if (cb_count == 0)
    return EOF;

  return cb_buf[cb_start];
}


/*
  int cb_peek(int offset)
  like cb_peek() with offset>0
  return EOF if token does not exist
  return next char without removing it from buffer
*/
int Menu2::cb_peek(int offset) const {
  if (cb_count <= offset)
    return EOF;

  return cb_buf[(cb_start + offset) % cb_size ];
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


// remove leading spaces from the input buffer:
void Menu2::skip_spaces() {
  while (cb_peek() == ' ')	//  EOF != ' '  so end of buffer case is ok
    cb_read();
}


/* int next_input_token()
   return next non space input token if any
   else return EOF					*/
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


#ifdef DEBUGGING
// cb_info() debugging help
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


/* bool lurk_and_do(bool (*ProgramAction)(void))
   get input byte, translate \n and \r to \0, which is 'END token'
   check for END token:
   * accumulate data bytes
   * \0 = 'END token':
     * if there's data call 'ProgramAction()' to read and act on all data
       and return true.
     * disregard END tokens on empty buffer (left over from newline translation)
   Return true if and only if action was taken.
 */
bool Menu2::lurk_and_do(bool (*ProgramAction)(void)) {
  int INP;
  char c;

#ifdef DEBUGGING
  std::cout << "\nrunning lurk_and_do()\n";
#endif

  /* int maybe_input()  
     must be a function returning one byte data
     or if there is no input returning EOF		*/
  INP=(*maybe_input)();

#ifdef DEBUGGING
  std::cout << "got input\n";
#endif

  if ( INP != EOF ) {	// there *is* input
    switch ( c = INP ) {

    // END token translation:
    case '\n':		// translate \n to 'END token' \0
      c = 0;
#ifdef DEBUGGING
      std::cout << "NL translated to END token\n";
#endif
      break;

    case '\r':		// translate \r to 'END token' \0
      c = 0;
#ifdef DEBUGGING
      std::cout << "CR translated to END token\n";
#endif
      break;

    case '\0':		// '\0' already is 'END token'
      // c = 0;
#ifdef DEBUGGING
      std::cout << "\\0 received";
#endif
      break;
    }
    if ( c ) {		// accumulate in buffer
      cb_write(c);
cb_info();
#ifdef DEBUGGING
      std::cout << "accumulated " << c << "\n";
#endif

    } else {

#ifdef DEBUGGING
      std::cout << "END token received. ";
      std::cout << "Buffer stored=" << cb_stored() << "\n";
#endif

      /* end of line token translation can produce more then one \0 in sequence
	 only the first is meaningful, the others have no data in the buffer
	 so we treat only the first one (the one with the data).
      */
      if ( cb_stored() ) {	// disregard empty buffers
	do_menu_actions(ProgramAction);		// act on buffer content
	menu_display();
	return true;		// true means *reaction was triggered*.

#ifdef DEBUGGING
      } else {
	std::cout << "(empty buffer ignored)\n";
#endif
      }
    }

  } else {
#ifdef DEBUGGING
      std::cout << "EOF received\n";
#endif
  }

  return false;		// false means *no reaction was triggered*.
}


/* bool is_numeric()
   true if there is a next numeric chiffre
   false on missing data or not numeric data		*/
bool Menu2::is_numeric() const {
  int c = cb_peek();

  if ( c < '0' )
    return false;

  if ( c > '9' )
    return false;

  return true;
}


/*
  numeric integer input from a chiffre sequence in the buffer
  call it with default_value, in most cases the old value.
  sometimes you might give an impossible value to check if there was input.
*/

const unsigned char number_missing[] MAYBE_PROGMEM = "number missing";

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


/* drop leading numeric sequence from the buffer:	*/
void Menu2::skip_numeric_input() {
  while ( is_numeric() )
    cb_read();
}

/* void common_display()
   display menu items common to all menus:		*/

const unsigned char common_[] MAYBE_PROGMEM = \
  "\nPress 'm' or '?' for menu  'e' toggle echo";

const unsigned char _quit[] MAYBE_PROGMEM = \
  "  'q' quit this menu";

/*
  const unsigned char program_[] MAYBE_PROGMEM = \
    " 'P' program menu ";
*/


/* display common menu entries:				*/
void Menu2::common_display() {
#ifdef DEBUGGING
  std::cout << "Program common display will go here\n";
#endif

  std::cout << common_;
//if (menu != MENU_CODE_UNDECIDED)
    std::cout << _quit;

  std::cout << "\n";
}


/* display menu	current state and common entries:	*/
void Menu2::menu_display() {
#ifdef DEBUGGING
  std::cout << "\nMENU DISPLAY menu_display()\n";
#endif

  common_display();
}


/* act on buffer content tokens after receiving 'END token':	*/
void Menu2::do_menu_actions(bool (*ProgramAction)(void)) {
#ifdef DEBUGGING
  std::cout << "\ndo_menu_actions()\n";
#endif

  // program menu first:
  bool did_something = (*ProgramAction)();

  if ( ! did_something ) {
#ifdef DEBUGGING
    std::cout << "ProgramAction() did not do anything...\n";
#endif

  } else {
#ifdef DEBUGGING
    std::cout << "ProgramAction() did do something ;)\n";
#endif

  }
}

