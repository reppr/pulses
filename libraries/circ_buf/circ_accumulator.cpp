/*
  circ_accumulator
*/

#include <stdlib.h>
#include <iostream>

#include "circ_buf.cpp"
#include "circ_accumulator.h"


Circ_accumulator::Circ_accumulator(int size, bool (*input_test)(void), int (*getChar)(void)) {

#ifdef DEBUGGING
  std::cout << "CONSTRUCTING Circ_accumulator\n";
#endif

  is_input = input_test;
  get_char = getChar;
}

Circ_accumulator::Circ_accumulator(void) {

#ifdef DEBUGGING
  std::cout << "CONSTRUCTING Circ_accumulator using default values\n";
#endif

  Circ_accumulator(64, NULL, NULL);
}


Circ_accumulator::~Circ_accumulator() {
#ifdef DEBUGGING
  std::cout << "DESTRUCTING Circ_accumulator\n";
#endif
}


bool Circ_accumulator::gather_then_do() {
#ifdef DEBUGGING
  std::cout << "gathering\n";
#endif

  return false;		// nothing was done
}
