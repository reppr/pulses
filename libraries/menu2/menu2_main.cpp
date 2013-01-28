/*
  menu2_main.cpp
  menu2 as monolithic base class
*/

#include <iostream>
#include <stdio.h>

#define DEBUGGING
#include "menu2.cpp"

Menu2 MENU(32);

int main() {
  std::cout << "running menu2_main.cpp\n";


  if ( MENU.lurk_then_react() )
    std::cout << "lurk_then_react returned TRUE\n";
  else
    std::cout << "lurk_then_react returned false\n";

  return 0;
}
