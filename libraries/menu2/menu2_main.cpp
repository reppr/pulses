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
//#define DEBUGGING_MENU

#include "menu2.cpp"


/* **************************************************************** */
Menu2 MENU(32, &getchar);


/* **************************************************************** */
void program_display() {
  std::cout << "\n  say 'a'\n";
}

/* **************************************************************** */
bool program_action(char token) {
#ifdef DEBUGGING_MENU
  std::cout << "program_action2(" << token << "): ";
#endif
  switch (token) {
  case 'a':
    std::cout << "\nYES, I DO KNOW TOKEN 'a' :)\n";
    return true;	// return true;  means there *was* action,
			// it's  *not* the exit status of the action
			// the menu page *is responsible* for this token	
//  break;

  default:
    return false;	// return false; means *no* action was taken here
			// the menu page is *not* responsible for the token    
  }
}


char menuTitle[] = "The 'a' Test";
char menuToken = 'P';


/* **************************************************************** */
int main() {
  std::cout << "running menu2_main.cpp  ==> ETHERNAL LOOP <==\n";

  MENU.add_page(menuTitle, menuToken, &program_display, &program_action);

  MENU.menu_display();

  while (true)		// ==> ETHERNAL LOOP <==
    if ( MENU.lurk_and_do() ) {
      #if defined(DEBUGGING_MENU) || defined(DEBUGGING_LURKING)
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
