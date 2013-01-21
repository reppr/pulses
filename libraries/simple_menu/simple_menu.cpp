/*
  simple_menu.cpp
*/

#include <stdlib.h>
#include <iostream>

// #include "circ_buf.h"
#include "../circ_buf/circ_buf.h"	// omg! ################################

#include "simple_menu.h"

Simple_menu::Simple_menu(int size, bool (*input_test)(void), int (*getChar)(void)) {
  Circ_buf CB(size);
  is_input = input_test;
  get_char = getChar;
  //  _cb = CB
  std::cout << "Simple_menu CONSTRUCTED\n";
}

Simple_menu::~Simple_menu() {
  // CB.~Circ_buf();
  std::cout << "Simple_menu DESTRUCTED\n";
}

void Simple_menu::lurk() {
  int INP;
  char c;

  std::cout << "lurking...\n";
  if ( (*is_input)() ) {
    std::cout << "there is input:  ";
    INP=(*get_char)();
    c=(char) INP;
    std::cout << c << "\t" << INP << "\n";
  } else
    std::cout << "waiting for input\n";
}
