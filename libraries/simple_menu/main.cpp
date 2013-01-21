// test only

#include <iostream>
#include <stdio.h>

// #include "circ_buf.cpp"
#include "../circ_buf/circ_buf.cpp"	// omg! ################################
#include "simple_menu.cpp"

char scratch[80];

bool input_test() {
  int INP;
  std::cout << "testing on input\n";

  if ( (INP = getchar_unlocked()) != EOF ) {
    putchar_unlocked(INP);
    return true;
  }

  return false;
}


int main() {
  std::cout << "testing main:\n";

  //  Simple_menu MENU(64, &input_test, &getchar);	// getchar()
  Simple_menu MENU(64, &input_test, &getchar_unlocked);	// getchar_unlocked()

  MENU.lurk();
  std::cout << "done\n";

  return 0;
}
