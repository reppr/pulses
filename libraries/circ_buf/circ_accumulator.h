/*
  circ_accumulater.h
*/

#ifndef CIRC_ACCUMULATOR
#define CIRC_ACCUMULATOR


#include "circ_buf.h"

class Circ_accumulator : public Circ_buf
{
 public:
  Circ_accumulator(int size);
  ~Circ_accumulator();

  bool gather_then_do();
 private:
};

#endif
