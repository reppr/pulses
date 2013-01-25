/*
  circ_accumulator
*/

#include <stdlib.h>
#include <iostream>

#include <stdio.h>
#include <termios.h>
#include "circ_buf.cpp"
#include "circ_accumulator.h"


Circ_accumulator::Circ_accumulator(int size, bool (*input_test)(void), int (*getChar)(void)):
  Circ_buf(size)
{
#ifdef DEBUGGING
  std::cout << "inner CONSTRUCTOR Circ_accumulator\n";
#endif
  is_input = input_test;
  get_char = getChar;

  /* did not work
  if (setvbuf(stdin, NULL, _IONBF, 0) == 0)
    printf("Set stream to unbuffered mode\n");
  */
}


Circ_accumulator::~Circ_accumulator() {
#ifdef DEBUGGING
  std::cout << "DESTRUCTING Circ_accumulator\n";
#endif

  /* did not work
  if (setvbuf(stdin, NULL, _IOLBF, 0) == 0) {
#ifdef DEBUGGING
    printf("Set stream back to line buffered mode\n");
#endif
  }
  */
}


bool Circ_accumulator::gather_then_do() {
  int INP;
  char c;

  std::cout << "gathering\n";

  if ( (*is_input)() ) {
    std::cout << "tested\n";

    INP = (*get_char)();
    std::cout << "getted\n";

    c=INP;

    switch ( INP ) {

    case EOF:
#ifdef DEBUGGING
      std::cout << "EOF";
#endif
      break;

    // end token translation:
    case '\n':
      c = 0;
#ifdef DEBUGGING
      std::cout << "NL";
#endif
      break;

    case '\r':
      c = 0;
#ifdef DEBUGGING
      std::cout << "CR";
#endif
      break;

    case '\0':
      // c = 0;
#ifdef DEBUGGING
      std::cout << "\\0";
#endif
      break;
    }
    if ( c ) {
      cb_write(c);

#ifdef DEBUGGING
      std::cout << "accumulated " << c << "\n";
#endif

    } else {

#ifdef DEBUGGING
      std::cout << "END TOKEN\n";
#endif

    }
  } else {

#ifdef DEBUGGING
    std::cout << "no input\n";
#endif

  }

  return false;		// nothing was done
}
