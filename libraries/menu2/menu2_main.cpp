/* **************************************************************** */
/*
  menu2_main.cpp
  menu2 as monolithic base class
*/
/* **************************************************************** */

#include <iostream>
#include <stdio.h>

//#define DEBUGGING_ALL
//#define DEBUGGING_CLASS
//#define DEBUGGING_CIRCBUF
//#define DEBUGGING_LURKING
#define DEBUGGING_MENU

#include "menu2.cpp"


/* **************************************************************** */
Menu2 MENU(32, &getchar);


/* **************************************************************** */
bool program_action() {
  char c;

  std::cout << "\nPROGRAM ACTION sees: ";
  while ( MENU.cb_stored() ) {
    c = MENU.cb_read();
    std::cout << c;
  }
  std::cout << "\n";

  return false;			// false means *no* action was taken here
}

/* **************************************************************** */
int main() {
  std::cout << "running menu2_main.cpp  ==> ETHERNAL LOOP <==\n";

  while (true)		// ==> ETHERNAL LOOP <==
    if ( MENU.lurk_and_do(&program_action) ) {
      #if defined(DEBUGGING_MENU) ||  defined(DEBUGGING_LURKING)
        std::cout << "lurk_and_do() returned TRUE\n";
      #endif
    } else {
      #ifdef DEBUGGING_MENU
        std::cout << "lurk_and_do() returned false\n";
      #endif
    }

  return 0;
}

/* **************************************************************** */
