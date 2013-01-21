/*
  circ_accumulator
*/

#include <stdlib.h>
#include <iostream>
#include "circ_buf.cpp"
#include "circ_accumulator.h"


Circ_accumulator::Circ_accumulator(int size) {
  std::cout << "CONSTRUCTING\n";
  Circ_buf CB(size);
}


Circ_accumulator::~Circ_accumulator() {
  std::cout << "DESTRUCTING\n";
    // CB.free();
}


bool Circ_accumulator::gather_then_do() {
  std::cout << "gathering\n";
  return false;		// nothing was done
}
