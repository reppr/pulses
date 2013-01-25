/*
  circ_accumulator
*/

#include <stdlib.h>
#include <iostream>

#include <stdio.h>
#include <termios.h>
#include "circ_buf.cpp"
#include "circ_accumulator.h"

/*
  size is size of circ_buf
  int maybe_input() must return EOF or next char
*/
Circ_accumulator::Circ_accumulator(int size, int (*maybeInput)(void)):
  Circ_buf(size)
{
#ifdef DEBUGGING
  std::cout << "inner CONSTRUCTOR Circ_accumulator\n";
#endif

  maybe_input = maybeInput;
}


Circ_accumulator::~Circ_accumulator() {
#ifdef DEBUGGING
  std::cout << "DESTRUCTING Circ_accumulator\n";
#endif
}

bool Circ_accumulator::gather_then_do(void (*Action)(void)) {
  int INP;
  char c;

#ifdef DEBUGGING
  std::cout << "gathering, testing input:\n";
#endif

  INP=(*maybe_input)();

#ifdef DEBUGGING
  std::cout << "got input\n";
#endif

  if ( INP != EOF ) {
    switch ( c = INP ) {

    // end token translation:
    case '\n':
      c = 0;
#ifdef DEBUGGING
      std::cout << "NL\n";
#endif
      break;

    case '\r':
      c = 0;
#ifdef DEBUGGING
      std::cout << "CR\n";
#endif
      break;

    case '\0':
      // c = 0;
#ifdef DEBUGGING
      std::cout << "\\0\n";
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
      std::cout << "END TOKEN received\n";
      std::cout << "stored " << cb_stored() << "\n";
#endif

      /* end of line token translation can produce more then one \0 in sequence
	 only the first is meaningful, the others have no data in the buffer
	 so we treat only the first one (the one with the data).
      */
      if ( cb_stored() ) {	// disregard empty buffers
	(*Action)();
	return true;		// there *was* action
      }
    }

  } else {
#ifdef DEBUGGING
      std::cout << "EOF received\n";
#endif
  }

  return false;			// no action
}
