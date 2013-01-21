/*
  circ_accumulator
*/

#include <stdlib.h>
#include <iostream>
#include "circ_accumulator.h"


Circ_accumulator::Circ_accumulator(int size) {
  std::cout << "CONSTRUCTING\n";
  Circ_buf CB(size);
}


Circ_accumulator::~Circ_accumulator() {
  std::cout << "DESTRUCTING\n";
    // CB.free();
}

//  bool accumulate_then_do();
