/*
  circ_accumulator.h
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

#ifndef CIRC_ACCUMULATOR
#define CIRC_ACCUMULATOR

#include "circ_buf.h"

class Circ_accumulator : public Circ_buf
{
 public:
  // Circ_accumulator(int size, int (*maybe_input)(void), void (*action)(void));
  Circ_accumulator(int size, int (*maybe_input)(void));
  Circ_accumulator();	// trying to keep c++ constructors happy
  ~Circ_accumulator();

  bool gather_then_do(void (*action)(void));

 protected:
  int (*maybe_input)(void);	// maybe_input() must return EOF or next char
  void (*action)(void);		// will be called on receiving an end token
};

#endif
