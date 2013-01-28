/*
  menu2.cpp
  menu2 as monolithic base class
*/

#include <stdlib.h>
#include <iostream>

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
  return -1 if buffer is empty, else
  return next char without removing it from buffer
*/
int Menu2::cb_peek() const {
  if (cb_count == 0)
    return -1;

  char value = cb_buf[cb_start];
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

  int value = cb_peek();
  std::cout << "\n  char:\t\t";
  if (value != -1)
    std::cout << (char) value;
  else
    std::cout << "(none)";
  std::cout << "\n";

  std::cout << "\n";
}
#endif


/* lurk_and_do(void (*Action)(void))
   get input byte, translate \n and \r to \0, which is 'end token'
   check for end token:
   * accumulate data bytes
   * \0 = 'end token':
     * if there's data call 'Action()' to read and act on all data
       and return true.
     * disregard end tokens on empty buffer (left over from newline translation)
   Return true if and only if action was taken.
 */
bool Menu2::lurk_and_do(void (*Action)(void)) {
  int INP;
  char c;

#ifdef DEBUGGING
  std::cout << "running lurk_and_do()\n";
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
      std::cout << "NL translated\n";
#endif
      break;

    case '\r':		// translate \r to 'end token' \0
      c = 0;
#ifdef DEBUGGING
      std::cout << "CR translated\n";
#endif
      break;

    case '\0':		// translate \r to 'end token' \0
      // c = 0;
#ifdef DEBUGGING
      std::cout << "\\0 received";
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
      std::cout << "END token received. ";
      std::cout << "Buffer stored=" << cb_stored() << "\n";
#endif

      /* end of line token translation can produce more then one \0 in sequence
	 only the first is meaningful, the others have no data in the buffer
	 so we treat only the first one (the one with the data).
      */
      if ( cb_stored() ) {	// disregard empty buffers
	(*Action)();
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
