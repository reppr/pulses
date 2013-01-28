/*
  simple_menu_main.cpp
*/

#include <iostream>
#include <stdio.h>

#define DEBUGGING
#include "simple_menu.cpp"
#include "simple_menu_main.h"


Simple_menu MENU(64, &getchar);		// getchar()


void menu_action() {
  char c;

  std::cout << "MENU ACTION on : ";
  while ( MENU.cb_stored() ) {
    c = MENU.cb_read();
    std::cout << c;
  }
  std::cout << "\n";
}


int main() {
  std::cout << "testing simple_menu main  ==> ETHERNAL LOOP <== \n";

  while(true)
    MENU.gather_then_do(&menu_action);
  // MENU.lurk();

  std::cout << "done\n";
  return 0;
}
