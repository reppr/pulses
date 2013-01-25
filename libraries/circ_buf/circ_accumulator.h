/*
  circ_accumulater.h
*/

#ifndef CIRC_ACCUMULATOR
#define CIRC_ACCUMULATOR

#include "circ_buf.h"

class Circ_accumulator : public Circ_buf
{
 public:
  // Circ_accumulator(int size, int (*maybe_input)(void), void (*action)(void));
  Circ_accumulator(int size, int (*maybe_input)(void));
  ~Circ_accumulator();

  bool gather_then_do(void (*action)(void));

 protected:
  int (*maybe_input)(void);	// maybe_input() must return EOF or next char
  void (*action)(void);		// will be called on receiving an end token
};

#endif
