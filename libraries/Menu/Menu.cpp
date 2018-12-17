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

  #include <Menu.h>

#else
  #include <iostream>

  #include <Menu/Menu.h>
#endif


/* **************************************************************** */
// Constructor/Destructor:

#ifndef ARDUINO		// WARNING: Using Stream MACRO hack when not on ARDUINO!
  #define Stream ostream
  #warning Using Stream MACRO HACK when not on ARDUINO!
#endif

Menu::Menu(int bufSize, int menuPages, int (*maybeInput)(void), Stream & port, Stream & port2):
  cb_size(bufSize),
  maybe_input(maybeInput),
  port_(port),
  port2_(port2),
  men_max(menuPages),
  cb_start(0),
  cb_count(0),
  men_selected(0),
  cb_buf(NULL),
  men_pages(NULL),
  echo_switch(true),
  verbosity(VERBOSITY_SOME),
  menu_mode(0)
{
  cb_buf = (char *) malloc(cb_size);
  men_pages = (menupage*) malloc(men_max * sizeof(menupage));
  /*
    Checking for RAM ERRORs delayed from Menu constructor to add_page()
    so the user can see the error message.
    Looks save enough as a menu is not very usable without adding menupages.
  */
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


#ifdef DEBUGGING_CIRCBUF
  // circ buf debugging:

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
	outln(F("  numeric CHIFFRE"));
      else
	outln(F("  NAN"));
    } else
      outln(F("(none)"));

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
#ifdef ARDUINO
/* void Menu::out(); overloaded menu output function family:	*/
/*
  #define MEN_OUT(X)  ( port_.print((X)); port2_.print((X)) )	//  embedded ';' does not work
  see: https://wiki.sei.cmu.edu/confluence/display/c/PRE10-C.+Wrap+multistatement+macros+in+a+do-while+loop
    see: Compliant Solution
  do { ... } while (0)     keep preprocessor happy ;)
*/
#define MENU_OUT(X) \
  do { \
    port_.print((X)); \
    port2_.print((X)); \
  } while (0)

#define MENU_OUTln(X) \
  do { \
    port_.println((X)); \
    port2_.println((X)); \
  } while (0)

  // Simple versions  void Menu::out():
  void Menu::out(const char c)		const	{ MENU_OUT(c); }	// ARDUINO
  void Menu::out(const int i)		const	{ MENU_OUT(i); }
  void Menu::out(const long l)		const	{ MENU_OUT(l); }
  void Menu::out(const char *str)	const	{ MENU_OUT(str); }	// c-style character string
  void Menu::out(const String s)	const	{ MENU_OUT(s); }	// c++ String

#ifndef INTEGER_ONLY
  void Menu::out(const double d)	const	{ MENU_OUT(d); }
#endif

  // unsigned versions:
  void Menu::out(const unsigned char c)	const	{ MENU_OUT(c); }
  void Menu::out(const unsigned int i)	const	{ MENU_OUT(i); }
  void Menu::out(const unsigned long l)	const	{ MENU_OUT(l); }

  // End of line versions  void outln():
  void Menu::outln(const char c)	const	{ MENU_OUTln(c); }
  void Menu::outln(const int i)		const	{ MENU_OUTln(i); }
  void Menu::outln(const long l)	const	{ MENU_OUTln(l); }
  void Menu::outln(const char *str)	const	{ MENU_OUTln(str); }	// c-style character string and newline
  void Menu::outln(const String s)	const	{ MENU_OUTln(s); }	// c++ String and newline

#ifndef INTEGER_ONLY
  void Menu::outln(const double d)	const	{ MENU_OUTln(d); }
#endif

  // unsigned versions:
  void Menu::outln(const unsigned char c)	const	{ MENU_OUTln(c); }
  void Menu::outln(const unsigned int i)	const	{ MENU_OUTln(i); }
  void Menu::outln(const unsigned long l)	const	{ MENU_OUTln(l); }

  // Print a value and pad with spaces to field width 'pad'.
  // At least one space will always be printed.
  void Menu::pad(long value, unsigned int pad) const {
    out(value);
    pad--;		// we need at least one chiffre
    if (value<0) {
      value = -value;
      pad--;		// '-' sign
    }
    long scratch=10;
    while (value >= scratch) {
      pad--;	// another chiffre
      scratch *= 10;
    }
    if (pad < 1)	// one space at least
      pad=1;

    for(; pad; pad--)	// pad with spaces
      space();
  }

#ifdef USE_F_MACRO
  // *DO* use Arduino F() MACRO for string output to save RAM:
  void Menu::out(const __FlashStringHelper* str) const {
    MENU_OUT(str);
  }

  // End of line version:
  // Arduino F() macro: outln(F("string");
  void Menu::outln(const __FlashStringHelper* str) const {
    MENU_OUTln(str);
  }
#endif

  void Menu::out(const float f, int places)	const {	// formatted float output
//    char c[16];
//    sprintf(c, "%g", places);  // as the macro takes one argument convert to a string first
//    MENU_OUT(c);
//    MENU_OUT((f, places));

    // ################ FIXME: use macro MENU_OUT(); ################
    port_.print(f, places);
    port2_.print(f, places);
  }


  /* void ticked(char c)	Char output with ticks like 'A'	*/
  void Menu::ticked(const char c) const {
    MENU_OUT((char) 39); MENU_OUT(c); MENU_OUT((char) 39);
  }


  /* Do i want this?
  // show char sequence ticked, like 'A' 'B' 'C' (ending in a space)
  void Menu::ticked_chars(const char * chars) const {
    char c;
    while (c = *chars++) {
      ticked(c);
      space();
    }
  }
  */

  /* Output a newline, tab, space, '='
     ln(), tab(), space(), equals():			*/
  void Menu::ln()     const { MENU_OUT('\n'); }	// output a newline
  void Menu::tab()    const { MENU_OUT('\t'); }	// output a tab
  void Menu::tab(unsigned int n)  const {	// output n tabs
    for (; n; n--)
      tab();
  }
  void Menu::space()  const { MENU_OUT(' '); }	// output a space	// but telnet seems to eat some of them...
  void Menu::space(unsigned int n)  const {	// output n spaces
    for (; n; n--)
      space();
  }

  void Menu::equals() const { MENU_OUT('='); }	// Output char '='
  void Menu::slash()  const { MENU_OUT('/'); }	// Output char '/'[1~

  void Menu::IPstring(int ip) const {			// Output an IP as 192.168.1.2
    for(int i=0; i<4; i++) {
      MENU_OUT(ip & 0xff);
      ip >>= 8;
      if (i<3)
	MENU_OUT('.');
    }
  }


void Menu::out_ON_off(bool flag) {	// output either "ON" or "off" depending flag
  if(flag)
    MENU_OUT(F("ON"));
  else
    MENU_OUT(F("off"));
}


bool Menu::maybe_display_more(unsigned char verbosity_level) {	// avoid too much output
  /* output control, avoid errors caused by serial menu output
     looks at verbosity *and* input buffer
     can be fooled to skip output by a terminating ',' on most input lines	*/

   if (verbosity < verbosity_level)
     return false;

   if (verbosity > verbosity_level)
     return true;

   // if verbosity is == verbosity_level: display only if no other input is waiting
   if(cb_peek()==EOF)
     return true;
   return false;		// there is more input, do not display info yet
}



/* numeric input, calculating (left to right)				*/
  int Menu::is_chiffre(char token) {		// is token a chiffre?
    if (token > '9' || token < '0')
      return false;
    return token;
  }


  char Menu::is_operator(char token) {
    switch (token) {
    case '*':
    case '/':
    case '+':
    case '-':
    case '%':
    case '&':
    case '|':
    case '^':
    case '!':
      return token;
      break;
    }
    return 0;
  }


  bool Menu::get_numeric_input(unsigned long *result) {	// if there's a number, read it
    char token = cb_peek();
    if (!is_chiffre(token))	// no numeric input, return false
      return false;

    drop_input_token();
    *result = token - '0';	// start with first chiffre

    while (token=cb_peek(), is_chiffre(token)) {
      drop_input_token();
      *result *= 10;
      *result += token -'0';
    }
    return true;		// all input was read
  }


  bool Menu::maybe_calculate_input(unsigned long *result) {
    /* check for & calculate integer input
       calculates simply left to right  */
    unsigned long scratch;	// see recursion
    char token = cb_peek();
    bool is_result = false;

    if (get_numeric_input(result)) {
      is_result = true;

      token = cb_peek();
      if (token == -1)
	return true;
    }

    if (is_operator(token)) {	// known operator?
      drop_input_token();

      scratch = *result;		// save for recursion
      switch (token) {
      case '*':
	if (get_numeric_input(result)) {
	  scratch *= *result;
	  *result = scratch;
	}
	if (cb_peek()==-1)
	  return true;
	else
	  return maybe_calculate_input(result);	// recurse and return
	break;
      case '/':
	if (get_numeric_input(result)) {
	  scratch /= *result;
	  *result = scratch;
	}
	if (cb_peek()==-1)
	  return true;
	else
	  return maybe_calculate_input(result);	// recurse and return
	break;
      case '+':
	if (get_numeric_input(result)) {
	  scratch += *result;
	  *result = scratch;
	}
	if (cb_peek()==-1)
	  return true;
	else
	  return maybe_calculate_input(result);	// recurse and return
	break;
      case '-':
	if (get_numeric_input(result)) {
	  scratch -= *result;
	  *result = scratch;
	}
	if (cb_peek()==-1)
	  return true;
	else
	  return maybe_calculate_input(result);	// recurse and return
	break;
      case '%':
	if (maybe_calculate_input(result)) {
	  scratch %= *result;
	  *result = scratch;
	  return true;
	}
	return false;
	break;
      case '&':
	if (maybe_calculate_input(result)) {
	  scratch &= *result;
	  *result = scratch;
	  return true;
	}
	return false;
	break;
      case '|':
	if (maybe_calculate_input(result)) {
	  scratch |= *result;
	  *result = scratch;
	  return true;
	}
	return false;
	break;
      case '^':
	if (maybe_calculate_input(result)) {
	  scratch ^= *result;
	  *result = scratch;
	  return true;
	}
	return false;
	break;
      case '!':	// factorial
	scratch = 1;
	for (long i=*result; i>0; i--)
	  scratch *= i;
	*result = scratch;
	break;

      default:	// should not happen, see is_operator()
	return false;
      }
    }

    return is_result;
  }

/*
  bool Menu::string_match(const char * teststring)
    check for a leading string match (skipping leading spaces) in the input buffer
    on a match  remove string (and leading spaces) from input and return true

    else  restore any leading spaces and return false

  the teststring itself can not begin with a string ;)
*/
bool Menu::string_match(const char * teststring) {
  int len;
  unsigned int leading_spaces = skip_spaces();	// remove leading spaces, can be restored

  if(len = strlen(teststring)) { // *if* there is a string
    if(cb_count >= len) {		// compare strings
      for(int i=0; i<len; i++)
	if(cb_peek(i) != *(teststring +i)) {
	  // restore leading spaces, as they could be *essential* for the menu parser
	  while (leading_spaces--)  restore_input_token();	// restore leading spaces
	  return false;			// strings are different
	}
      // teststring *matches* input buffer
      while(len--) drop_input_token();	// *remove* string from input buffer
					// (leading spaces remain skipped too)
      return true;			// OK	*matches* input buffer
    }
  }

  // restore leading spaces, as they could be *essential* for the menu parser
  while (leading_spaces--)  restore_input_token();	// restore leading spaces
  return false;		// no string, no match
}

  /*
   * Menu tries to determine free RAM and to display free RAM in the title line:
   * To keep the change simple I decided to define the RAM functions as noops
   * when the menu does not know how to determine RAM properly.
   */

  #if defined(GET_FREE_RAM)		// REAL free RAM functions:
    #if defined(ESP8266) || defined(ESP32)
      /* int get_free_RAM(): determine free RAM on ESP8266 and ESP32:	*/
      int Menu::get_free_RAM() const {
	return ESP.getFreeHeap();
      }
    #else // ARDUINO, but not ESPxx
      /* int get_free_RAM(): determine free RAM on ARDUINO:		*/
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

    void Menu::print_free_RAM() const {
      out(F("free RAM:")); space(); out((int) get_free_RAM());
    }

  #else				// NOOP free RAM fakes only:

    int Menu::get_free_RAM() const {		// noop fake
      return -1;
    }

    void Menu::print_free_RAM() const {		// noop fake
    }
  #endif
#else // OUTPUT functions  out() family for c++ Linux PC test version:
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
  void Menu::out(const char c)		const	{ putchar(c); }
  void Menu::out(const int i)		const	{ printf("%i", i); }
  void Menu::out(const long l)		const	{ printf("%d", l); }
  void Menu::out(const char *str)	const	{ printf("%s", str); }

  // unsigned versions:
  void Menu::out(const unsigned char c)	const	{ putchar(c); }
  void Menu::out(const unsigned int i)	const	{ printf("%u", i); }
  void Menu::out(const unsigned long l)	const	{ printf("%u", l); }

  // End of line versions  void outln():
  void Menu::outln(const char c)	const	{ printf("%c\n", c); }
  void Menu::outln(const int i)		const	{ printf("%i\n", i); }
  void Menu::outln(const long l)	const	{ printf("%d\n", l); }
  void Menu::outln(const char *str)	const   { printf("%s\n", str); }

  // unsigned versions:
  void Menu::outln(const unsigned char c)	const { printf("%c\n", c); }
  void Menu::outln(const unsigned int i)	const { printf("%u\n", i); }
  void Menu::outln(const unsigned long l)	const { printf("%u\n", l); }


  // float output:
  void Menu::out(const float f, int places)	const {	// formatted float output
    printf("%f", places);
  }

  /* Output a newline, tab, space, '='
     ln(), tab(), space(), equals():			*/
  void Menu::ln()	const { putchar('\n'); } // Output a newline
  void Menu::tab()	const { putchar('\t'); } // Output a tab
  void Menu::space()	const { putchar(' '); }  // Output a space
  void Menu::equals()	const { putchar('='); }  // Output char '='

  /* void ticked(char c)	Char output with ticks like 'A'	*/
  void Menu::ticked(const char c)	const {   // prints 'c'
    printf("\'%c\'", c);
  }


  /* NOOP free RAM fakes only:				*/
  int Menu::get_free_RAM() const {		// noop fake
    return -1;
  }

  void Menu::print_free_RAM() const {		// noop fake
  }

#endif	// [ ARDUINO else ]  c++ test version	OUTPUT functions.


/* **************************************************************** */
/* void out_BIN(unsigned long value, int bits)
   Print binary numbers with leading zeroes and a trailing space:   */
void Menu::outBIN(unsigned long value, int bits ) const {
  int i;
  unsigned long mask=0;

  for (i = bits - 1; i >= 0; i--) {
    mask = (1 << i);
    if (value & mask)
      out('1');
    else
      out('0');
  }
  space();
}

// show one letter mnemonics for flag state and a trailing space
void Menu::show_flag_mnemonics(unsigned long flags, int bits, char* ON_chars, char* OFF_chars) const {
  for (int i = bits - 1; i >= 0; i--) {
    if (flags & (1 << i))
      out(ON_chars[i]);
    else
      out(OFF_chars[i]);
  }
  space();
}

void Menu::out_hex_chiffre(unsigned char chiffre) const { // output 1 hex chiffre
  char c='?';

  if (chiffre < 10)
    c = chiffre + '0';
  else
    if (chiffre < 16 )
      c = chiffre - 10 + 'a';

  out(c);
}

// Print a uint32_t in hex:
void Menu::out_hex(uint32_t n) const {			// uint32_t  hex output
  char hex[9];
  sprintf(hex, "%x", n);
  out(hex);
}

/*
  void out_ticked_hexs(unsigned int count) const;
  Print a sequence of ticked ascending hex chiffres: '0' '1' ... '9' 'a' 'b'
  Ends with a space.
*/
void Menu::out_ticked_hexs(unsigned int count) const {
  if (count>16)
    count=16;

  for(char i=0; i<count; i++) {
    if (i<10)
      ticked(i + '0');
    else
      ticked(i -10 + 'a');
    space();				// ends with a space
  }
}



/*
  void bar_graph(long value, unsigned int scale, char c);
  print one value & bar graph line.

  example output:  MENU.bar_graph(value, 1023, '*');

value -1112	---------------------------------------------------------------XXXXXX
value -1040	---------------------------------------------------------------XX
value -1039	---------------------------------------------------------------X
value -1024	---------------------------------------------------------------X
value -1023	---------------------------------------------------------------|
value -1022	---------------------------------------------------------------
value -319	-------------------
value -32	--
value -31	-
value -16	-
value -15	!
value -1	!
value 0		0
value 1		.
value 15	.
value 16	*
value 32	**
value 256	****************
value 368	***********************
value 496	*******************************
value 608	**************************************
value 832	****************************************************
value 1008	***************************************************************
value 1022	***************************************************************
value 1023	**************************************************************|
value 1024	***************************************************************X
value 1072	***************************************************************XXXX
value 1104	***************************************************************XXXXXX
*/
void Menu::bar_graph(long value, unsigned int scale, char c) {
  const long length=64;
  int stars = (value * length) / (scale + 1) ;

  out(F("value "));
  pad(value, 5);
  tab();

  if (value==0) {
    outln('0');
    return;
  }
  if (value < 0) {
    c='-';
    value=-value;
    stars=-stars;
    if(stars==0) {	// negative, but too little for a star ;)
      c='!';
      stars=1;
    }
  } else		// positive, non zero value
    if (value==scale)	// full scale: last sign will be '|'
      --stars;

  if (stars)
    for (int i=0; i<stars; i++) {
      if (i==(length - 1))
	c='X';
      out(c);
    }
  else
    out('.');	// not zero, but too little for a star ;)

//	  if (value > scale)		//
//	    out('X');
//	  else
  if (value==scale)		// positive full scale:
    out('|');			// last sign '|'

  ln();
}



/* **************************************************************** */
// String recycling:

// void OutOfRange(): output "out of range\n"
void Menu::OutOfRange()	const { out(F("out of range\n")); }

void Menu::out_Error_()	const { out(F("ERROR: ")); }

// void error_ln(char * str): output "ERROR: xxxxxxxx"
void Menu::error_ln(const char * str)	const {
  out_Error_();
  outln(str);
}

// void ok_or_error_ln(char * str, errorflag): output "ERROR: xxxxxxxx nn" or "xxxxxxxx ok"
void Menu::ok_or_error_ln(const char * str, int error) const {
  if(error) {
    out_Error_();
    out(str);
    space();
#if defined ESP32
    outln(esp_err_to_name(error));	// display ESP32 ERROR NAMES
#else
    outln(error);
#endif
  } else {
    out(str);
    outln(" ok");
  }
}

// void out_selected_(): output "selected "
void Menu::out_selected_() const { out(F("selected ")); }

// void out_flags_(): output "\tflags "
void Menu::out_flags_()	const { out(F("\tflags ")); }

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
unsigned int Menu::skip_spaces() {
  unsigned int space_count=0;
  while (cb_peek() == ' ') {  //  EOF != ' ' end of buffer case ok
    space_count++;
    cb_read();
  }
  return space_count;
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


/* int drop_input_token()
   drop next input token from the input and return it
   no checks
   only use if you must...					*/
int Menu::drop_input_token() {
  return cb_read();
}


/* int restore_input_token()
   restores last input token and return it
   no checks
   only use if you *really must*				*/
int Menu::restore_input_token() {	// restores last input token	only use if you *really must*
  cb_count++;				// *dangerous*	but you *can* restore a just-read-token
  cb_start = (cb_size + cb_start - 1) % cb_size;

  return cb_buf[cb_start];
}


void Menu::play_KB_macro(char *macro, bool newline) {	// most often you might do  'men_selected=0;'  before
  out("play_KB_macro  ");

  for (char c=*macro++; c; c=*macro++) {
    if (cb_is_full()) {
      // Inform user:
      if (verbosity) {
	out(F("buffer "));
	out_Error_();
	OutOfRange();
      }
    } else {
      cb_write(c);

      if (echo_switch)		// echo input back to ouput?
	out(c);			// *not* depending verbosity
    }
  }

  space();			// separe macro from its output

  interpret_men_input();	// Menu input interpreter
  if(newline)
    ln();
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

  /* int maybe_input()
     must be a function returning one byte data
     or if there is no input returning EOF		*/
  INP=(*maybe_input)();

  if ( INP == EOF )	// no input?  done
    return false;	// false: *no program reaction triggered*

  // there *is* input
#if defined(DEBUGGING_LURKING)
  outln(F("got input"));
#endif

  // END token translation:
  switch ( c = INP ) {
  case '\0':		// '\0' already *is* 'END token'
#if defined(DEBUGGING_LURKING) || defined(DEBUGGING_MENU)
    out(F("\\0 received"));
#endif
    break;
  case '\n':		// translate \n to 'END token' \0
  case '\r':		// translate \r to 'END token' \0
#if defined(DEBUGGING_LURKING)
    out((int) c);  outln(F(" translated to END token"));
#endif
    c = 0;
    break;
  } // END token translation

  if ( c ) {		// accumulate in buffer
    cb_write(c);
    if (echo_switch)	// echo input back to ouput?
      out(c);		// *not* depending verbosity

#if defined(DEBUGGING_LURKING) || defined(DEBUGGING_MENU)
    out(F("accumulated "));
    ticked(c);
    ln();
#endif
    if (cb_is_full()) {
      // Inform user:
      if (verbosity) {
	out(F("buffer "));
	out_Error_();
	OutOfRange();
      }

      // try to recover
      // the fix would be to match message length and cb buffer size...
#if defined(DEBUGGING_LURKING) || defined(DEBUGGING_MENU)
      outln(F("cb is full.  interpreting as EMERGENCY EXIT, dangerous..."));
#endif

      // EMERGENCY EXIT, dangerous...
      interpret_men_input();	// <<<<<<<< INTERPRET BUFFER CONTENT >>>>>>>>
#if defined(DEBUGGING_LURKING) || defined(DEBUGGING_MENU)
      outln(F("lurk_then_do() INTERPRETed INPUT BUFFER."));
#endif

      if (verbosity == VERBOSITY_HIGH)
	menu_display();

      return true;		// true means *reaction was triggered*.
    }
  } else { // token==0
#if defined(DEBUGGING_LURKING) || defined(DEBUGGING_MENU)
    out(F("END token received. ")); out(F("Buffer stored=")); outln(cb_stored());
#endif
    /* end of line token translation can produce more then one \0 in sequence
       only the first is meaningful, the others have no data in the buffer
       so we treat only the first one (the one with the data).
    */
    if ( cb_stored() ) {	// disregard empty buffers
      if (echo_switch)
	ln();

      interpret_men_input();	// <<<<<<<< INTERPRET BUFFER CONTENT >>>>>>>>
#if defined(DEBUGGING_LURKING) || defined(DEBUGGING_MENU)
      outln(F("lurk_then_do() INTERPRETed INPUT BUFFER."));
#endif

      if (verbosity == VERBOSITY_HIGH)
	menu_display();

      return true;	// true means *reaction was triggered*.
#if defined(DEBUGGING_LURKING)
    } else {
      outln(F("(empty buffer ignored)"));
#endif
    }
  }

  return false;		// false means *no reaction was triggered*.
}


/* **************************************************************** */
/*
  numeric integer input from a chiffre sequence in the buffer.
  call it with default_value, in most cases the old value.
  sometimes you might give an impossible value to check if there was input.
*/

/* bool is_numeric()
   true if there is a next numeric chiffre
   false on missing data or not numeric data			*/
bool Menu::is_numeric() const {
  int c = cb_peek();

  if (c > '9' || c < '0')
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
  if (verbosity) {
    outln(F("number missing"));
  }
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
  space(2); out('\''); out((char) MENU_MENU_PAGES_KEY); out(men_pages[pg].page_key); out('\'');
  out(F(" (sees ")); ticked(men_pages[pg].active_group); out(')');
  tab(); out('"'); out(men_pages[pg].title); outln('"');
}


/* menu_pages_info()  show all known pages' info		*/
void Menu::menu_pages_info() const {
  for (char pg = 0; pg < men_known; pg++)
    menu_page_info(pg);
}


/* **************************************************************** */
// menu handling:

int Menu::add_page(const char *pageTitle, const char page_key,		\
		     void (*pageDisplay)(void), bool (*pageReaction)(char), const char ActiveGroup) {

  // Delayed MALLOC ERROR CHECKING from constructor:
  if ((cb_buf == NULL) || (men_pages == NULL)) {
  /*
    Checking for RAM ERRORs delayed from Menu constructor to add_page()
    so the user can see the error message.
    Looks save enough as a menu is not very usable without adding menupages.
  */
    out_Error_();
    print_free_RAM();	// real or fake ;)
    ln();
    flush();		// we *do* need to flush here
#ifndef ESP8266
    exit(1);		// give up, STOP!	// FIXME: DOES *NOT* WORK WITH ESP8266 ################
#endif
  }

  if (men_known < men_max) {
    men_pages[men_known].title = (char *) pageTitle;
    men_pages[men_known].page_key = page_key;
    men_pages[men_known].display = pageDisplay;
    men_pages[men_known].interpret = pageReaction;
    men_pages[men_known].active_group = ActiveGroup;
    men_known++;
#ifdef DEBUGGING_MENU
    out(F("add_page(\""));  out(pageTitle);
    out(F("\", "));  ticked(page_key);  out(F(",..) \t"));
    outln((int) men_known - 1);
#endif
    return (men_known - 1);
  } else {	// ERROR handling ################
    if (verbosity) {
      out(F("add_page "));
      out_Error_();
      OutOfRange();
    }
  }
  return -1;
}


/* **************************************************************** */
// menu display:

/* Display menu	current menu page and common entries:		*/
void Menu::menu_display() const {
  char pg;
#ifdef DEBUGGING_MENU
  Menu::outln(F("\nmenu_display()"));
#endif

  ln();

  // start with a line for each menu page:
  menu_pages_info();

  // men_selected page display:
  out(F("\n * ** *** MENU "));
  out(men_pages[men_selected].title);
  out(F(" *** ** *\t\t"));
  print_free_RAM();	// real or fake ;)
  ln();

  (*men_pages[men_selected].display)();

  /*	keep that code for possible re-use
  // Display menu page key bindings:
  if ( men_known > 1 ) {
    ln();
    for (pg = 0; pg < men_known; pg++) {
      if ( pg != men_selected )		     // burried pages only
	if ( men_pages[pg].page_key != ' ') {  // selectable pages only
	  out(men_pages[pg].page_key);
	  equals();
	  out(men_pages[pg].title);
	  space(); space();
	}
    }
    ln();
  }
  */

  // Display internal key bindings:
  //   on a PC i use a broken configuration sends 'ß' instead of '?'
  //   just accept that as '?' without telling anybody ;)
  out(F("\n'?' for menu  '_' toggle echo  '+-' verbosity"));
  if (men_selected)
    out(F("  'q' quit page"));
  ln();

#ifdef DEBUGGING_MENU
  ln();
#endif
}


/* **************************************************************** */
void Menu::verbosity_info() {
  if (verbosity) {
    out(F("verbosity "));
    switch (verbosity) {
    case VERBOSITY_LOWEST:
      out(F("lowest"));
      break;
    case VERBOSITY_SOME:
      out(F("some"));
      break;
    case VERBOSITY_MORE:
      out(F("more"));
      break;
    case VERBOSITY_HIGH:
      out(F("high"));
      break;
    }
  }
}


void Menu::outln_invalid() {
  outln(F("(invalid)"));
}


void Menu::out_noop() {
  out(F("noop"));
}


/* **************************************************************** */
// menu input interpreter:

/* Act on buffer content tokens after receiving 'END token':	*/

/* factored out:
   Not needed without debugging code.				*/
bool Menu::try_page_reaction(char pg, char token) {
  bool did_something=(*men_pages[pg].interpret)(token);

#ifdef DEBUGGING_MENU
  space(); space();	// indent
  out(F("page \"")); out(men_pages[pg].title); out('"'); tab();
  if (did_something)
    out(F("KNEW"));
  else
    out(F("unknown"));
  out(F(" token ")); ticked(token); ln();
#endif
  return did_something;
}


void Menu::interpret_men_input() {
  char token, pg, page_group, selected_group;
  bool did_something, is_active;

#ifdef DEBUGGING_MENU
  out(F("\ninterpret_men_input(): "));
#endif
  // interpreter loop over each token:
  // read all tokens:
  //   skip spaces
  //   check for MENU_MENU_PAGES_KEY and react
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
    ticked(token); ln();
#endif

    // skip spaces:
    if ( token == ' ' )
      continue;

    if (token==MENU_MENU_PAGES_KEY) {	// ':'
#ifdef DEBUGGING_MENU
      outln(F("* MENU PAGES KEY"));
#endif
      switch(token = cb_peek()) {	// read next token
      case 255:		// 255 is EOF as char	':' only	display menu pages
	menu_pages_info();
	return;
	break;
      case '?':		// maybe also on ":?" too?  drop one of 2 tokens
      case 'ß':		// some broken configuration sends 'ß' instead of '?', silently accept
	drop_input_token();
	menu_pages_info();	// then info, like ':' only
	continue;
	break;
      default:	// search if it is a menupage page_key
    // search menu page page_keys:
#ifdef DEBUGGING_MENU
	out(F("* search page_keys\t"));
	out(token);
	tab();
	outln((int) token);
#endif
	for (pg = 0; pg < men_known; pg++) {	// search menu page page_keys
	  if (token == men_pages[pg].page_key) {
	    drop_input_token();
	    men_selected = pg;			// switch to page
	    did_something = true;		// yes, did switch
#ifdef DEBUGGING_MENU
	    out(F("   found page "));
	    out((int) pg); tab(); ticked(token);
	    tab(); ticked(men_pages[pg].page_key); ln();
#endif
	    // often menu_display() will be called anyway, depending verbosity
	    // if verbosity is too low, (but still not zero,)  we do it from here
	    if (verbosity <= VERBOSITY_MORE)
	      if (maybe_display_more(VERBOSITY_LOWEST))
		menu_display();

#ifdef DEBUGGING_MENU
	    out(F("FOUND ")); menu_page_info(pg);
#endif
	    break;
	  }
	}	// search menu page page_keys
	if (did_something) {
#ifdef DEBUGGING_MENU
	  out(F("SWITCH to ")); menu_page_info(men_selected);
#endif
	  continue;
	}
      } // check token *after* MENU_MENU_PAGES_KEY
    }
    // token not found yet...
    // search selected page first:
#ifdef DEBUGGING_MENU
    out(F("* try selected"));
#endif
    did_something = try_page_reaction(men_selected, token);
    if (did_something) {
      continue;
    }
    // token not found yet...


    // check pages in same page_group and in group '+':
    selected_group = men_pages[men_selected].active_group;
#ifdef DEBUGGING_MENU
    out(F("* check visability group ")); ticked(selected_group); ln();
    outln(F("  * going through the pages"));
#endif
    for (pg = 0; pg < men_known; pg++) {
      if (pg == men_selected )	// we already searched men_selected
	continue;

      is_active=false;
      page_group = men_pages[pg].active_group;
#ifdef DEBUGGING_MENU
      space(); space(); space(); menu_page_info(pg);
#endif
      switch ( page_group ) {
      case '-':			// '-' means *never*
	break;
      case '+':			// '+' means always on
	is_active=true;
#ifdef DEBUGGING_MENU
	outln(F("==> * joker '+'"));
#endif
	break;
      default:			// else: active if in selected pages' group
	if ( page_group == selected_group ) {
	  is_active=true;
#ifdef DEBUGGING_MENU
	  space(); space(); space(); space();
	  out(F("* visability match"));
#endif
	}
      }
      if (is_active)		// test active menu pages on token:
	if ( (did_something = try_page_reaction(pg, token)) )
	  break;
    }// loop over hidden pages

    if (did_something)
      continue;
    // token not found yet...

    // check for internal bindings next:
#ifdef DEBUGGING_MENU
    outln(F("* search internal key bindings"));
#endif
    switch (token) {
    case '?':
    case 'ß':	// some broken configuration sends 'ß' instead of '?'
      if (verbosity <= VERBOSITY_MORE)	// if verbosity is too low	// TODO: test!
	menu_display();			//   we do it from here

      did_something = true;
      break;

    case 'q':
#if ! defined(ARDUINO) && ! defined(ESP8266)	// on PC quit when on page 0
      if (men_selected == 0) {
	exit(0);
      }
#endif

      men_selected = 0;
#ifdef DEBUGGING_MENU
      out(F("==>* switch to ")); menu_page_info(men_selected);
#endif
      if (verbosity <= VERBOSITY_MORE)	// if verbosity is too low	// TODO: test!
	menu_display();			//   we do it from here

      did_something = true;
      break;

    case '+':
      if (++verbosity > VERBOSITY_HIGH)
	verbosity = VERBOSITY_HIGH;
      verbosity_info();
      ln();

      did_something = true;
      break;

    case '-':
      if (--verbosity < VERBOSITY_LOWEST) // no UI access of verbosity=0
	verbosity = VERBOSITY_LOWEST;
      verbosity_info();
      ln();

      did_something = true;
      break;

    case '_':	// toggle echo		// underscore toggles echo
      echo_switch = !echo_switch;
      did_something = true;
      break;

    }
#ifdef DEBUGGING_MENU
    if (did_something) {
      out(F("INTERNAL HOTKEY ")); ticked(token); outln('.');
    }
#endif

    // token still not found, give up...
    if (! did_something ) {
      // maybe display menu and *then* the "UNKNOWN TOKEN line"
      if(verbosity == VERBOSITY_HIGH)
	menu_display();

      if (verbosity >= VERBOSITY_LOWEST) {
	out(F("\nUNKNOWN TOKEN "));
	ticked(token);
	tab();
	out((int) (token & 0xFF));
	ln();
      }
      if (cb_peek() != EOF) {
	out(F("skipping "));
	out(token);
	while (cb_peek() != EOF) {
	  out((char) drop_input_token());
	}
	ln();
      }
    }

  } // interpreter loop over all tokens
}
/* **************************************************************** */
