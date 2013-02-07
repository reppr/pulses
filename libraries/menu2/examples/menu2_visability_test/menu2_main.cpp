/* **************************************************************** */
/*
  menu2_main.cpp
  menu2 as monolithic base class
*/
/* **************************************************************** */

#ifndef ARDUINO
  #include <iostream>
#endif

#include <stdio.h>

/*	==> does not work <==
// unbuffeered getch() for testing on linux pc
// compile with -lncurses
// #include <ncurses.h>
*/

/* macros for debugging:		*/ 
//#define DEBUGGING_ALL
//#define DEBUGGING_CLASS
//#define DEBUGGING_CIRCBUF
//#define DEBUGGING_LURKING
//#define DEBUGGING_MENU


#include "menu2.cpp"


/* **************************************************************** */
Menu2 MENU(32, 5, &getcharMACRO);



/* **************************************************************** */
void program_displayA() {
  OUT("\n'a' is active,\t");
  OUT("hmm... try 'y' maybe?\n");
}

/* **************************************************************** */
void program_displayB() {
  OUT("\nHere we say 'b'\n");
}


/* **************************************************************** */
void program_displayX() {
  OUT("\nthere's nothing here to see\n");
}


/* **************************************************************** */
void program_displayY() {
  OUT("\nYOU SHOULD NEVER SEE THIS...\n");
}


/* **************************************************************** */
long value = 42;

void program_displayN() {
  OUT("\nValue = ");
  OUT(value);
  OUT("\tv=change value.\n");
}


/* **************************************************************** */
bool program_actionA(char token) {
#ifdef DEBUGGING_MENU
  OUT("testing program_actionA(");
  OUT(token);
  OUT("):\n");
#endif
  switch (token) {
  case 'a':
    OUT("\nYES, I DO KNOW TOKEN 'a' :)\n");
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
  OUT("testing program_actionB(");
  OUT(token);
  OUT("):\n");
#endif
  switch (token) {
  case 'b':
    OUT("\nnow, I *DO* KNOW token 'b' :)\n");
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
  OUT("testing program_actionX(");
  OUT(token);
  OUT("):\n");
#endif
  switch (token) {
  case 'x':
    OUT("\n XX       XX");
    OUT("\n  XX     XX");
    OUT("\n   XX   XX");
    OUT("\n    XX XX");
    OUT("\n     XXX");
    OUT("\n    XX XX");
    OUT("\n   XX   XX");
    OUT("\n  XX     XX");
    OUT("\n XX       XX\n");

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
  OUT("testing program_actionY(");
  OUT(token);
  OUT("):\n");
#endif
  switch (token) {
  case 'y':
    OUTln()
    OUT("/°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°\\ \n");	// ecape \ ...
    OUT("°      you have found the      °\n");
    OUT("°                              °\n");
    OUT("°    °********************°    °\n");
    OUT("°    °** *  BACKDOOR  * **°    °\n");
    OUT("°    °********************°    °\n");
    OUT("°    °**/              \\**°    °\n");	// ecape \ ...
    OUT("°    °**                **°    °\n");
    OUT("°    °**   YY      YY   **°    °\n");
    OUT("°    °**    YY    YY    **°    °\n");
    OUT("°    °**     YY  YY     **°    °\n");
    OUT("°    °**      Y\\/Y      **°    °\n");	// ecape \ ...        
    OUT("°    °**       YY       **°    °\n");
    OUT("°    °**       YY       **°    °\n");
    OUT("°    °**       YY       **°    °\n");
    OUT("°    °**       YY       **°    °\n");
    OUT("°    °**       YY       **°    °\n");
    OUT("°    °**                **°    °\n");
    OUT("°    °**\\.  .******.  ./**°    °\n");	// ecape \ ...
    OUT("°    °********************°    °\n");
    OUT("°    °                    °    °\n");
    OUT("°            (: :)             °\n");
    OUT("°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°\n");
      OUTln();

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
bool program_actionN(char token) {
#ifdef DEBUGGING_MENU
  OUT("testing program_actionN(");
  OUT(token);
  OUT("):\n");
#endif
  switch (token) {
  case 'v':
    OUT("Enter new value : ");
    value=MENU.numeric_input(value);

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
char menuTitleA[] = "'A' PAGE";
char menuHotkeyA = 'A';

char menuTitleB[] = "Back Page";
char menuHotkeyB = 'B';

char menuTitleX[] = "XXX";
char menuHotkeyX = 'X';

char menuTitleY[] = "YYYYY";
// char menuHotkeyY = ' ';

char menuTitleN[] = "NUMBERS";
char menuHotkeyN = 'N';



void my_setup() {
  // add menu pages:

  // page A will only be visible when selected, never when burried:
  MENU.add_page(menuTitleA, menuHotkeyA, &program_displayA, &program_actionA, '-');

  // page B belongs to 'Y' group:
  MENU.add_page(menuTitleB, menuHotkeyB, &program_displayB, &program_actionB, 'Y');

  // page X is always visible
  MENU.add_page(menuTitleX, menuHotkeyX, &program_displayX, &program_actionX, '+');

  /* page Y adds actions active for the 'Y' group
     this group cannot get selected, the page hot key ' ' prevents that.	*/
  MENU.add_page(menuTitleY, ' ', &program_displayY, &program_actionY, 'Y');

  // page N testing numeric input
  MENU.add_page(menuTitleN, menuHotkeyN, &program_displayN, &program_actionN, '-');


  // display menu at startup:
  MENU.menu_display();
}


void my_loop_body() {
  if ( MENU.lurk_then_do() ) {
#if defined(DEBUGGING_MENU) || defined(DEBUGGING_LURKING)
    OUT("lurk_then_do() returned TRUE\n");
#endif
  } else {
#ifdef DEBUGGING_MENU
    OUT("lurk_then_do() returned false\n");
#endif
  }
}


#ifndef ARDUINO
/* **************************************************************** */
// PC test version:
int main() {
  OUT("menu2_main.cpp  ==> ETHERNAL LOOP <==:\n");

  /* 	does not work
  // unbuffered ncurses keyboard test input:
  OUT("unbuffered ncurses keyboard test input\n");
  initscr();
  cbreak();
  */

  my_setup();

  while (true)		// ==> ETHERNAL LOOP <==
    my_loop_body();

  return 0;
}
#endif


#ifdef ARDUINO
/* **************************************************************** */
// Arduino setup() and main loop:

void setup() {
  Serial.begin(BAUDRATE);
  my_setup();
}


void loop() {
  MENU.lurk_then_do();
}
#endif


/* **************************************************************** */
