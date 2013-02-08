/* **************************************************************** */
/*
  menu2.cpp
  menu2 as monolithic base class
*/

/* **************************************************************** */
// Preprocessor #includes:

#include <stdio.h>
#include <stdlib.h>

#ifndef ARDUINO
  #include <iostream>
#endif

#include <menu2.h>


/* **************************************************************** */
// #define I/O for Arduino:
#ifdef ARDUINO

/* int men_getchar();	// Arduino version
   Read next char of menu input, if available.
   Does not block, returns EOF or char.			*/
int men_getchar() {		// Arduino version
  if (!Serial.available())
    return EOF;

  return Serial.read();
}


#ifndef BAUDRATE
  #define BAUDRATE	115200	// works fine here
  // #define BAUDRATE	57600
  // #define BAUDRATE	38400
  // #define BAUDRATE	19200
  // #define BAUDRATE	9600	// failsafe
  // #define BAUDRATE	31250	// MIDI
#endif


/* void out(); overloaded menu output function family:		*/
// Simple versions  void out():
void out(char c)	{ Serial.print(c); }
void out(int i)		{ Serial.print(i); }
void out(long l)	{ Serial.print(l); }
void out(const char *str) { Serial.print(str); }

// End of line versions  void outln():
void outln(char c)	{ Serial.println(c); }
void outln(int i)	{ Serial.println(i); }
void outln(long l)	{ Serial.println(l); }
void outln(const char *str) { Serial.println(str); }

/* Second parameter versions with a trailing char:
  out(xxx, char c)
  A char as a second parameter get's printed after first argument.
  like:  out(string, '\t');  or  out(string, ' ');		*/
void out(char c, char x)	{ Serial.print(c); Serial.print(x); }
void out(int i, char x)		{ Serial.print(i); Serial.print(x); }
void out(long l, char x)	{ Serial.print(l); Serial.print(x); }
void out(const char *str, char x) { Serial.print(str); Serial.print(x); }

/* Output a newline, tab, space  nl(), tab(), space():		*/
void nl()		{ Serial.println(); }
void tab()		{ Serial.print('\t'); }
void space()		{ Serial.print(' '); }

/* void tokenized(char c)	Char output with ticks like 'A'	*/
void tokenized(char c) {
  Serial.print("'"); Serial.print(c); Serial.print("'");
}


// PROGMEM strings 	#define out_str() for Arduino:
/*
  Serial.print() for PROGMEM strings:
  // void serial_print_progmem(const prog_uchar *str)	// does not work :(
*/
void serial_print_progmem(const unsigned char *str) {
  unsigned char c;
  while((c = pgm_read_byte(str++)))
    Serial.write(c);
}
// On Arduino out_str() is #defined as a macro.
#define out_str(str)	( serial_print_progmem((str)) )


#else	// c++ test version
/* **************************************************************** */
// #define I/O for c++ Linux PC test version:

/* int men_getchar();
   Read next char of menu input, if available.
   Returns EOF or char.						*/
int men_getchar() {
  return getchar();
}


/* void out(); overloaded menu output function family:		*/
// Simple versions  void out():
void out(char c)	{ putchar(c); }
void out(int i)		{ printf("%i", i); }
void out(long l)	{ printf("%d", l); }
void out(const char *str) { printf("%s", str); }

// End of line versions  void outln():
void outln(char c)	{ printf("%c\n", c); }
void outln(int i)	{ printf("%i\n", i); }
void outln(long l)	{ printf("%d\n", l); }
void outln(const char *str) { printf("%s\n", str); }

/* Second parameter versions with a trailing char:
  out(xxx, char c)
  A char as a second parameter get's printed after first argument.
  like:  out(string, '\t');  or  out(string, ' ');		*/
void out(char c, char x)	{ printf("%c%c", c, x); }
void out(int i, char x)		{ printf("%i%c", i, x); }
void out(long l, char x)	{ printf("%d%c", l, x); }
void out(const char *str, char x) { printf("%s%c", str, x); }

/* Output a newline, tab, space  nl(), tab(), space():		*/
void nl()		{ putchar('\n'); }
void tab()		{ putchar('\t'); }
void space()		{ putchar(' '); }

/* void tokenized(char c)	Char output with ticks like 'A'	*/
void tokenized(char c)	{ printf("\'%c\'", c); }	// prints 'c'


/* Fake PROGMEM string output on PC:	*/
void out_str(const unsigned char *str) {	// Fake PROGMEM string output on PC
  char c;

  while(c = *str++)
    out(c);
}

#endif	// [ Arduino else ]  c++ test version
/* **************************************************************** */


/* **************************************************************** */
// Some early definitions:

/* char constants for output having the arduino as target in mind:
   alternative to PROGMEM one char strings...			*/	
const char _space = ' ';
const char _tab  = '\t';
const char _tick = '\'';
const char _quote = '"';
const char _star = '*';
const char _open  = '(';	// not used yet ################
const char _close = ')';	// not used yet ################
const char _column = ':';	// not used yet ################


/* **************************************************************** */
/* strings for output having the arduino as target in mind:
   MAYBE_PROGMEM is either empty or has 'PROGMEM' in it.
   so we can compile them in program memory to save RAM on arduino,
   but still test on c++ Linux PC.
								*/	
const unsigned char outOfRange[] MAYBE_PROGMEM = "out of range";
const unsigned char error_[] MAYBE_PROGMEM = " ERROR: ";


/* **************************************************************** */
// Constructor/Destructors:

Menu2::Menu2(int bufSize, int menuPages, int (*maybeInput)(void)) {
#ifdef DEBUGGING_CLASS
  out("Menu2 CONSTRUCTOR: cb_size=");
  outln(bufSize);
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
  out("Menu2 DESTRUCTOR\n");
#endif

  free(cb_buf);
  free(men_pages);
}


/* **************************************************************** */
// Basic circular buffer functions:

/*
  cb_write() save a byte to the buffer:
  does *not* check if buffer is full				*/
void Menu2::cb_write(char value) {	// ERROR handling where? ################
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
    bool Menu2::cb_is_full() {
      return cb_count == cb_size;
    }
*/


/* **************************************************************** */
// Buffer interface functions for parsing:

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
  out("\nBuffer:\t\t");
  out(cb_buf);

  out("\n  size:\t\t");
  out(cb_size);

  out("\n  count:\t");
  out(cb_count);

  out("\n  start:\t");
  outln(cb_start);

  int value = cb_peek();
  out("\n  char:\t\t");
  if (value != -1) {
    out(value);
    out("\t");
    out((char) value;
    if ( is_numeric() )
      out("  numeric CHIFFRE\n");
    else
      out("  NAN\n");
  } else
    out("(none)\n");

  value = next_input_token();
  out("\n  next_input_token()\t: ");
  out(value);
  out("\t");
  outln((char) value);
  outln();

  for (int i=0; i<=cb_count; i++) {
    value = cb_peek(i);
    out("  cb_peek(");
    out(i);
    out(")\t\t: ");
    out(value);
    out("\t");
    outln((char) value);
  }

  outln();
}
#endif


/* **************************************************************** */
// lurk_then_do() main menu2 user interface:


const unsigned char buffer_[] MAYBE_PROGMEM = "buffer";

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
bool Menu2::lurk_then_do() {
  int INP;
  char c;

#if defined(DEBUGGING_MENU) || defined(DEBUGGING_LURKING)
  out("\nlurk_then_do():\n");
#endif

  /* int maybe_input()  
     must be a function returning one byte data
     or if there is no input returning EOF		*/
  INP=(*maybe_input)();

#if defined(DEBUGGING_CIRCBUF) || defined(DEBUGGING_LURKING)
  out("got input\n");
#endif

  if ( INP != EOF ) {	// there *is* input
    switch ( c = INP ) {

    // END token translation:
    case '\n':		// translate \n to 'END token' \0
      c = 0;
#if defined(DEBUGGING_CIRCBUF) || defined(DEBUGGING_LURKING)
      out("NL translated to END token\n");
#endif
      break;

    case '\r':		// translate \r to 'END token' \0
      c = 0;
#if defined(DEBUGGING_CIRCBUF) || defined(DEBUGGING_LURKING)
      out("CR translated to END token\n");
#endif
      break;

    case '\0':		// '\0' already is 'END token'
      // c = 0;
#if defined(DEBUGGING_MENU) || defined(DEBUGGING_CIRCBUF)
      out("\\0 received");
#endif
      break;
    }
    if ( c ) {		// accumulate in buffer
      cb_write(c);
#if defined(DEBUGGING_MENU) || defined(DEBUGGING_CIRCBUF) || \
  defined(DEBUGGING_LURKING)
      out("accumulated ");
      tokenized(c);
      nl();
#endif
      if (cb_is_full()) {
	// inform user:
	out_str(buffer_);
	out_str(error_);
	out_str(outOfRange);
	nl();

	// try to recover
	// the fix would be to match message length and cb buffer size... 
#if defined(DEBUGGING_MENU) || defined(DEBUGGING_CIRCBUF) || \
  defined(DEBUGGING_LURKING)
      out("cb is full.  interpreting as EMERGENCY EXIT, dangerous...\n");
#endif
	// EMERGENCY EXIT, dangerous...
	interpret_men_input();	// <<<<<<<< INTERPRET BUFFER CONTENT >>>>>>>>
	menu_display();
#if defined(DEBUGGING_MENU) || defined(DEBUGGING_LURKING)
    out("lurk_then_do() INTERPRETed INPUT BUFFER.\n");
#endif
	return true;		// true means *reaction was triggered*.
      }
    } else {
#if defined(DEBUGGING_MENU) || defined(DEBUGGING_CIRCBUF) || \
  defined(DEBUGGING_LURKING)
      out("END token received. ");
      out("Buffer stored=");
      outln(cb_stored());
#endif

      /* end of line token translation can produce more then one \0 in sequence
	 only the first is meaningful, the others have no data in the buffer
	 so we treat only the first one (the one with the data).
      */
      if ( cb_stored() ) {	// disregard empty buffers
	interpret_men_input();	// <<<<<<<< INTERPRET BUFFER CONTENT >>>>>>>>
	menu_display();
#if defined(DEBUGGING_MENU) || defined(DEBUGGING_LURKING)
    out("lurk_then_do() INTERPRETed INPUT BUFFER.\n");
#endif
	return true;		// true means *reaction was triggered*.

#if defined(DEBUGGING_CIRCBUF) || defined(DEBUGGING_LURKING)
      } else {
	out("(empty buffer ignored)\n");
#endif
      }
    }

  } else {
#if defined(DEBUGGING_CIRCBUF) || defined(DEBUGGING_LURKING)
      out("EOF received\n");
#endif
  }

#if defined(DEBUGGING_MENU) || defined(DEBUGGING_LURKING)
    out("lurk_then_do() lurking...\n");
#endif
  return false;		// false means *no reaction was triggered*.
}


/* **************************************************************** */
/*
  numeric integer input from a chiffre sequence in the buffer
  call it with default_value, in most cases the old value.
  sometimes you might give an impossible value to check if there was input.
*/

const unsigned char numberMissing_[] MAYBE_PROGMEM = "number missing\n";

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
  out_str(numberMissing_);
  return default_value;		// return default_value
}


/* drop leading numeric sequence from the buffer:		*/
void Menu2::skip_numeric_input() {
  while ( is_numeric() )
    cb_read();
}


/* **************************************************************** */
// menu info:

const unsigned char menuPage_[] MAYBE_PROGMEM = "menupage ";
const unsigned char hotk_[] MAYBE_PROGMEM = "hotk '";
const unsigned char group_[] MAYBE_PROGMEM = "group '";

/* menu_page_info(char pg)  show a known pages' info	*/
void Menu2::menu_page_info(char pg) const {
  if ( pg == men_selected )
    out(_star);
  else
    out(_space);

  out_str(menuPage_);
  out((int) pg);
  out(_tab);
  out_str(hotk_);
  out(men_pages[pg].hotkey);
  out(_tick);
  out(_tab);
  out_str(group_);
  out(men_pages[pg].active_group);
  out(_tick);
  out(_tab);
  out(_quote);
  out(men_pages[pg].title);
  out(_quote);
  nl();
}


/* menu_pages_info()  show all known pages' info		*/
void Menu2::menu_pages_info() const {
  for (char pg = 0; pg < men_known; pg++)
    menu_page_info(pg);
}


/* **************************************************************** */
// menu handling:

const unsigned char addPg[] MAYBE_PROGMEM = "add_page";

void Menu2::add_page(char *pageTitle, char hotkey,		\
		     void (*pageDisplay)(void), bool (*pageReaction)(char), const char ActiveGroup) {
  if (men_known < men_max) {
    men_pages[men_known].title = pageTitle;
    men_pages[men_known].hotkey = hotkey;
    men_pages[men_known].display = pageDisplay;
    men_pages[men_known].interpret = pageReaction;
    men_pages[men_known].active_group = ActiveGroup;
    men_known++;

#ifdef DEBUGGING_MENU
    out_str(addPg);
    out("(\"");
    out(pageTitle);
    out("\", ");
    out(hotkey);
    out(",..)\n");
#endif
  } else {	// ERROR handling ################
    out_str(addPg);
    out_str(error_);
    out_str(outOfRange);
    nl();
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
  out("\nmenu_display():\n");
#endif

  // men_selected page display:
  out_str(deco_);
  out_str(men_);
  out(men_pages[men_selected].title);
  out_str(_deco);

  (*men_pages[men_selected].display)();

  // display menu page key bindings:
  if ( men_known > 1 ) {
    nl();
    for (pg = 0; pg < men_known; pg++) {
      if ( pg != men_selected ) {	// omit selected pages' hot key display, even if active.
	out(men_pages[pg].hotkey);
	out("=");
	out(men_pages[pg].title);
	out(_space);
	out(_space);
      }
    }
    nl();
  }

  // display internal key bindings:
  out_str(internalKeys);
  out_str(qQuit);
  nl();

#ifdef DEBUGGING_MENU
  nl();
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
  out("interpret_men_input(): '");
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
    out("'\n");
#endif

    // skip spaces:
    if ( token == ' ' )
      continue;


    // search selected page first:
    did_something = (*men_pages[men_selected].interpret)(token);
    if (did_something) {
#ifdef DEBUGGING_MENU
      out("selected page is responsible for '");
      out(token);
      out("'.\n");
#endif
      continue;
    }
#ifdef DEBUGGING_MENU
    out("selected page does not know '");
    out(token);
    out("'\n");
#endif
    // token not found yet...


    // check pages in same page_group and in group '+':
    selected_group = men_pages[men_selected].active_group;
#ifdef DEBUGGING_MENU
    out("check for selected_group '");
    out(selected_group);
    out("':\n");
    out("  going through the pages:\n");
#endif

    for (pg = 0; pg < men_known; pg++) {
      if (pg == men_selected )	// we already searched men_selected
	continue;

      is_active=false;
      page_group = men_pages[pg].active_group;
#ifdef DEBUGGING_MENU
      out("   ");
      menu_page_info(pg);
#endif
      switch ( page_group ) {
      case '-':			// '-' means *never*
	break;
      case '+':			// '+' means always on
	is_active=true;
#ifdef DEBUGGING_MENU
	out("==>*  joker '+':\n");
#endif
	break;
      default:			// else: active if in selected pages' group
	if ( page_group == selected_group ) {
	  is_active=true;
#ifdef DEBUGGING_MENU
	  out("==>*  page group match ");
	  menu_page_info(pg);
#endif
	}
      }
      if (is_active) {		// test active menu pages on token:
	if ( did_something = (*men_pages[pg].interpret)(token) ) {
#ifdef DEBUGGING_MENU
	  out("page_group '");
	  out(page_group);
	  out("':\n");
	  out("menu ");
	  out(men_pages[pg].title;
	  out(" knows '");
	      out(token);
	  out("'.\n");
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
    out("search menu page hotkeys:\n");
#endif
    for (pg = 0; pg < men_known; pg++) {
      if (token == men_pages[pg].hotkey) {
	(*men_pages[pg].interpret)(token);	// *might* do more, return is irrelevant
	men_selected = pg;			// switch to page
	did_something = true;			// yes, did switch
#ifdef DEBUGGING_MENU
    	out("==>* ");
	menu_page_info(pg);
#endif
	break;
      }
    }
    if (did_something) {
#ifdef DEBUGGING_MENU
      out("==>* switch to ");
      menu_page_info(men_selected);
#endif
      continue;
    }
    // token not found yet...


    // check for internal bindings next:
#ifdef DEBUGGING_MENU
    out("search internal key bindings:\n");
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
      out("==>* switch to ");
      menu_page_info(men_selected);
#endif
      did_something = true;
      break;
    }
#ifdef DEBUGGING_MENU
    if (did_something) {
      out("==>* internal hotkey '");
      out(token);
      out("'.\n");
    }
#endif

    // token still not found, give up...
    if (! did_something ) {
      out_str(unknownToken);
      out(token);
      nl();
    }

  } // interpreter loop over all tokens
}
/* **************************************************************** */
