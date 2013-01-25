/*
  testing circ_accumulator
*/

#include <iostream>

#include <stdio.h>
#include <termios.h>

#define DEBUGGING
#include "circ_accumulator.cpp"

char scratch[80];

Circ_accumulator ACC(64, &getchar);		// getchar(), getchar_unlocked()


/*
int maybe_input() {
  return getchar_unlocked();
}
*/


void do_it() {
  char c;

  std::cout << "program ACTION will be taken on: ";

  while ( ACC.cb_stored() ) {
    c = ACC.cb_read();
    std::cout << c;
  }
  std::cout << "\n";
}


int main() {
  std::cout << "testing circ_accumulator looped main:\n";

  while(true)
    ACC.gather_then_do(&do_it);

  std::cout << "done\n";
  return 0;
}
