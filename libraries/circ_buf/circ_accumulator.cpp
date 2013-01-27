/*
  circ_accumulator.cpp
  Extend Circ_buf class to accumulate incoming bytes until 'end token'
  then do a programmable action that has read access to the buffer.

  First change typical line ending codes \n and \c to end tokens \0.

  If the result is not \0 = 'end token',
    then accumulate it.
  else,
    on receiving an end token,
    if the buffer is not empty,
      then trigger action.
    empty end tokens (from end of line codes translation) get ignored.
*/

#include <stdlib.h>
#include <iostream>

#include <stdio.h>
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

/* gather_then_do(void (*Action)(void))
   get input byte, translate \n and \r to \0, which is 'end token'
   check for end token:
   * accumulate data bytes
   * \0 = 'end token':
     * if there's data call 'Action()' to read and act on all data
       and return true.
     * disregard end tokens on empty buffer (left over from newline translation)
   Return true if and only if action was taken.
 */
bool Circ_accumulator::gather_then_do(void (*Action)(void)) {
  int INP;
  char c;

#ifdef DEBUGGING
  std::cout << "gathering, testing input:\n";
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

    // end token translation:
    case '\n':		// translate \n to 'end token' \0
      c = 0;
#ifdef DEBUGGING
      std::cout << "NL\n";
#endif
      break;

    case '\r':		// translate \r to 'end token' \0
      c = 0;
#ifdef DEBUGGING
      std::cout << "CR\n";
#endif
      break;

    case '\0':		// translate \r to 'end token' \0
      // c = 0;
#ifdef DEBUGGING
      std::cout << "\\0\n";
#endif
      break;
    }
    if ( c ) {		// accumulate in buffer
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
