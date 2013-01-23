/*
  simple_menu.cpp
*/

#include <stdlib.h>
#include <iostream>

#include "../circ_buf/circ_accumulator.cpp"	// omg! ################################
#include "simple_menu.h"


Simple_menu::Simple_menu(int size, bool (*input_test)(void), int (*getChar)(void)) {
  // Circ_accumulator(int size, bool (*is_input)(void), int (*get_char)(void));
#ifdef DEBUGGING
  std::cout << "Simple_menu constructing...\n";
#endif

  Circ_accumulator ACC(size, input_test, getChar);

#ifdef DEBUGGING
  std::cout << "Simple_menu CONSTRUCTED\n";
#endif
}


Simple_menu::~Simple_menu() {
#ifdef DEBUGGING
  std::cout << "Simple_menu DESTRUCTED\n";
#endif
}


void Simple_menu::lurk() {
  int INP;
  char c;

#ifdef DEBUGGING
  std::cout << "lurking...\n";
#endif


  if ( (*is_input)() ) {
#ifdef DEBUGGING
    std::cout << "there is input:  ";
#endif
    INP=(*get_char)();
    c=(char) INP;
#ifdef DEBUGGING
    std::cout << c << "\t" << INP << "\n";
#endif

  } else {
#ifdef DEBUGGING
    std::cout << "waiting for input\n";
#endif
  }

}
