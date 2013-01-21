/*
 something.cpp  Passing a function pointer to a function to do something:
*/

#include <iostream>

bool something() {
  std::cout << "Something in the way she moves...\n";
  return true;
}

void do_it(bool (*do_something)(void)) {
  (*do_something)();
}

int main() {
  std::cout << "something.cpp\n";
  do_it(&something);
  std::cout << "done\n";
  return 99;	// test only ;)
}
