/* **************************************************************** */
/*
  menu2_main.cpp
  menu2 as monolithic base class
*/
/* **************************************************************** */

#include <iostream>
#include <stdio.h>

/* unbuffeered getch() for testing on linux pc
   compile with -lncurses	does not work	*/
// #include <ncurses.h>


//#define DEBUGGING_ALL
//#define DEBUGGING_CLASS
//#define DEBUGGING_CIRCBUF
//#define DEBUGGING_LURKING
//#define DEBUGGING_MENU

#include "menu2.cpp"


/* **************************************************************** */
Menu2 MENU(32, 2, &getchar);
// Menu2 MENU(32, 2, &getch);

/* **************************************************************** */
void program_displayA() {
  std::cout << "\n  say 'a'\n";
}

/* **************************************************************** */
void program_displayB() {
  std::cout << "\n  say 'b'...\n";
}

/* **************************************************************** */
bool program_actionA(char token) {
#ifdef DEBUGGING_MENU
  std::cout << "program_actionA(" << token << "): ";
#endif
  switch (token) {
  case 'a':
    std::cout << "\nYES, I DO KNOW TOKEN 'a' :)\n";
    return true;	// return true;  means there *was* action,
			// it's  *not* the exit status of the action
			// the menu page *is responsible* for this token
    break;

  case 'b':
    std::cout << "\nno, ...give me an 'a'\n";
    return true;
    break;

  default:
    return false;	// return false; means *no* action was taken here
			// the menu page is *not* responsible for the token    
  }
}


bool program_actionB(char token) {
#ifdef DEBUGGING_MENU
  std::cout << "program_actionB(" << token << "): ";
#endif
  switch (token) {
  case 'a' :
    std::cout << "\nSORRY, you failed!\n";
    return true;
    break;

  case 'b':
    std::cout << "\nYES, 'b' :)\n";
    return true;	// return true;  means there *was* action,
			// it's  *not* the exit status of the action
			// the menu page *is responsible* for this token
    break;

  default:
    return false;	// return false; means *no* action was taken here
			// the menu page is *not* responsible for the token    
  }
}


char menuTitleA[] = "The 'ab' Test";
char menuTitleB[] = "'b' page";
char menuTokenA = 'A';
char menuTokenB = 'B';


/* **************************************************************** */
int main() {
  std::cout << "running menu2_main.cpp  ==> ETHERNAL LOOP <==\n";

  /* 	does not work
  // unbuffered ncurses keyboard test input:
  std::cout << "unbuffered ncurses keyboard test input\n";
  initscr();
  cbreak();
  */

  MENU.add_page(menuTitleA, menuTokenA, &program_displayA, &program_actionA);
  MENU.add_page(menuTitleB, menuTokenB, &program_displayB, &program_actionB);

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
