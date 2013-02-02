/* **************************************************************** */
/*
  menu2_main.cpp
  menu2 as monolithic base class
*/
/* **************************************************************** */

#include <iostream>
#include <stdio.h>

/*	==> does not work <==
// unbuffeered getch() for testing on linux pc
// compile with -lncurses
// #include <ncurses.h>
*/


//#define DEBUGGING_ALL
//#define DEBUGGING_CLASS
//#define DEBUGGING_CIRCBUF
//#define DEBUGGING_LURKING
//#define DEBUGGING_MENU

#include "menu2.cpp"


/* **************************************************************** */
Menu2 MENU(32, 4, &getchar);


/* **************************************************************** */
void program_displayA() {
  std::cout << "\n'a' is active\n";
}

/* **************************************************************** */
void program_displayB() {
  std::cout << "\n'b'\n";
}


/* **************************************************************** */
void program_displayX() {
  std::cout << "\nthere's nothing here to see\n";
}


/* **************************************************************** */
void program_displayY() {
  std::cout << "\nYOU SHOULD NEVER SEE THIS...\n";
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
  default:
    return false;	// return false; means *no* action was taken here
			// the menu page is *not* responsible for the token    
  }
}


/* **************************************************************** */
bool program_actionB(char token) {
#ifdef DEBUGGING_MENU
  std::cout << "program_actionB(" << token << "): ";
#endif
  switch (token) {
  case 'b':
    std::cout << "\nnow, I *DO* KNOW token 'b' :)\n";
    return true;	// return true;  means there *was* action,
			// it's  *not* the exit status of the action
			// the menu page *is responsible* for this token
    break;
  default:
    return false;	// return false; means *no* action was taken here
			// the menu page is *not* responsible for the token    
  }
}


/* **************************************************************** */
bool program_actionX(char token) {
#ifdef DEBUGGING_MENU
  std::cout << "program_actionB(" << token << "): ";
#endif
  switch (token) {
  case 'x':
    std::cout << "\n XX       XX";
    std::cout << "\n  XX     XX";
    std::cout << "\n   XX   XX";
    std::cout << "\n    XX XX";
    std::cout << "\n     XXX";
    std::cout << "\n    XX XX";
    std::cout << "\n   XX   XX";
    std::cout << "\n  XX     XX";
    std::cout << "\n XX       XX\n";

    return true;	// return true;  means there *was* action,
			// it's  *not* the exit status of the action
			// the menu page *is responsible* for this token
    break;
  default:
    return false;	// return false; means *no* action was taken here
			// the menu page is *not* responsible for the token    
  }
}


/* **************************************************************** */
bool program_actionY(char token) {
#ifdef DEBUGGING_MENU
  std::cout << "program_actionY(" << token << "): ";
#endif
  switch (token) {
  case 'y':
    std::cout << "\nYou found the BACKDOOR!\n";
    return true;	// return true;  means there *was* action,
			// it's  *not* the exit status of the action
			// the menu page *is responsible* for this token
    break;
  default:
    return false;	// return false; means *no* action was taken here
			// the menu page is *not* responsible for the token    
  }
}


/* **************************************************************** */
char menuTitleA[] = "The 'A' Page";
char menuTokenA = 'A';

char menuTitleB[] = "Back Page";
char menuTokenB = 'B';

char menuTitleX[] = "XXX";
char menuTokenX = 'X';

char menuTitleY[] = "YYYYY";
// char menuTokenY = '\0';



/* **************************************************************** */
int main() {
  std::cout << "running menu2_main.cpp  ==> ETHERNAL LOOP <==\n";

  /* 	does not work
  // unbuffered ncurses keyboard test input:
  std::cout << "unbuffered ncurses keyboard test input\n";
  initscr();
  cbreak();
  */

  MENU.add_page(menuTitleA, menuTokenA, &program_displayA, &program_actionA, '-');
  MENU.add_page(menuTitleB, menuTokenB, &program_displayB, &program_actionB, 'Y');
  MENU.add_page(menuTitleX, menuTokenX, &program_displayX, &program_actionX, '+');
  MENU.add_page(menuTitleY, '\0', &program_displayY, &program_actionY, 'Y');

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
