/*
  menu2.cpp
  menu2 as monolithic base class
*/

#include <iostream>
#include <stdlib.h>

#include "menu2.h"


Menu2::Menu2(int bufSize) {
#ifdef DEBUGGING
  std::cout << "Menu2 CONSTRUCTOR\n";
#endif

  cb_start = 0;
  cb_size  = bufSize;
  cb_buf = (char *) malloc(cb_size);	// could fail ################
  cb_count = 0;
}


Menu2::~Menu2() {
#ifdef DEBUGGING
  std::cout << "Menu2 DESTRUCTOR\n";
#endif

  free(cb_buf);
}


bool Menu2::lurk_then_react() {
#ifdef DEBUGGING
  std::cout << "running lurk_then_react()\n";
#endif

  // return true;		// true means *reaction was triggered*.
  return false;		// false means *no reaction was triggered*.
}
