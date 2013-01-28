/*
  simple_menu.cpp
*/

#include <stdlib.h>
#include <iostream>

#include "../circ_buf/circ_accumulator.cpp"	// omg! ################################
#include "simple_menu.h"


Simple_menu::Simple_menu(int size, int (*maybeInput)(void)):
  Circ_accumulator(size, maybeInput)
{
#ifdef DEBUGGING
  std::cout << "Simple_menu inner CONSTRUCTER\n";
#endif
}


Simple_menu::~Simple_menu() {
#ifdef DEBUGGING
  std::cout << "Simple_menu DESTRUCTED\n";
#endif
}


//void Simple_menu::lurk() {
//  int INP;
//  char c;
//
//#ifdef DEBUGGING
//  std::cout << "lurking...\t";
//#endif
//
//  // immediately return if there's no input:
//  if ( INP=(*maybe_input)() == EOF ) {
//#ifdef DEBUGGING
//    std::cout << "(no input, done)\n";
//#endif
//
//    return;
//  }
//
//  c=(char) INP;
//#ifdef DEBUGGING
//  std::cout << "input: "  << c << " | " << INP;
//#endif
//}
