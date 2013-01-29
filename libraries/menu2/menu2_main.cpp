/*
  menu2_main.cpp
  menu2 as monolithic base class
*/

#include <iostream>
#include <stdio.h>

#define DEBUGGING
#include "menu2.cpp"


Menu2 MENU(32, &getchar);

void program_action() {
  char c;

  std::cout << "\nPROGRAM ACTION sees: ";
  while ( MENU.cb_stored() ) {
    c = MENU.cb_read();
    std::cout << c;
  }
  std::cout << "\n";
}

int main() {
  std::cout << "running menu2_main.cpp  ==> ETHERNAL LOOP <==\n";

  while (true)		// ==> ETHERNAL LOOP <==
    if ( MENU.lurk_and_do(&program_action) )
      std::cout << "lurk_and_do() returned TRUE\n";
    else
      std::cout << "lurk_and_do() returned false\n";

  return 0;
}
