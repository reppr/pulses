/*
  menu2.cpp
  menu2 as monolithic base class
*/

#include <iostream>

#include "menu2.h"


Menu2::Menu2() {
#ifdef DEBUGGING
  std::cout << "Menu2 CONSTRUCTER\n";
#endif
}


Menu2::~Menu2() {
#ifdef DEBUGGING
  std::cout << "Menu2 DESTRUCTER\n";
#endif
}


bool Menu2::lurk_then_react() {
#ifdef DEBUGGING
  std::cout << "running lurk_then_react()\n";
#endif

  // return true;		// true means *reaction was triggered*.
  return false;		// false means *no reaction was triggered*.
}
