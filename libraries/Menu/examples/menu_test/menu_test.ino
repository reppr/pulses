/* **************************************************************** */
/*
		menu_test.ino

	  A simple test example for library Menu. 
*/

/*
  This version definines the menu INPUT routine int men_getchar();
  in the *program* not inside the Menu class.
*/
/* **************************************************************** */

#include <stdio.h>
#include <stdlib.h>

#ifdef ARDUINO
  /* Keep ARDUINO GUI happy ;(		*/
  #if ARDUINO >= 100
    #include "Arduino.h"
  #else
    #include "WProgram.h"
  #endif
#else			// c++ Linux PC test version
  #include <iostream>
  using namespace std;
#endif

#include <Menu.h>
#ifndef ARDUINO		// Does not work without that!  WHAT IS HAPPENING HERE???
  #include <Menu.cpp>
#endif

#define BAUDRATE	115200		// works fine here


/* **************************************************************** */
/*
  This version definines the menu INPUT routine int men_getchar();
  in the *program* not inside the Menu class.
*/
#ifdef ARDUINO

  int men_getchar() {
    if (!Serial.available())	// ARDUINO
      return EOF;

    return Serial.read();
  }

  Menu MENU(32, 5, &men_getchar, Serial);

#else

  int men_getchar() {
    return getchar();		// c++ Linux PC test version
  }

  Menu MENU(32, 5, &men_getchar, cout);

#endif

const char guguseli[] MAYBE_PROGMEM = "\tdada...\n";

/* **************************************************************** */
/* I use this for both: Arduino example sketch and c++ PC test version.
   Compile setup() and loop() on ARDUINO
		or:
   compile main() on c++ Linux PC test version.
*/ 
/* **************************************************************** */
#ifdef ARDUINO		// Compile setup() and loop() on Arduino:
// Compile setup() and loop() for Arduino:


void setup() {	// ARDUINO
  /*
    This version definines the menu INPUT routine int men_getchar();
    in the *program* not inside the Menu class.
    Reverted to passing &men_getchar to the Menu class constructor.
  */
  Serial.begin(BAUDRATE);	// Start serial communication.

  MENU.out("For me this is a test for visability of menu pages:\n");
  MENU.out("depending the page different menu keys are active.\n");
  MENU.out("For you it's a very simple joke game to find out how it works...\n");
  MENU.ln();

  Menu_setup();		// Tell the menu what to do.

  // display menu at startup:
  MENU.menu_display();
}


void loop() {	// ARDUINO
  /*
    All you have to from your Arduino sketch loop() is to call:
    MENU.lurk_then_do();
    This will *not* block the loop.

    It will lurk on menu input, but return immediately if there's none.
    On input it will accumulate one next input byte and return until it
    will see an 'END token'.

    When seeing one of the usual line endings it takes it as 'END token' \0.

    On receiving an 'END token' it will interpret the accumulated tokens and return.
  */
  MENU.lurk_then_do();
}


/* **************************************************************** */
#else	// Compile main() on c++ PC test version:
// Compile main() for c++ Linux PC test version:

int main() {	// c++ Linux PC test version

  MENU.ln();
  MENU.out("For me this is a test for visability of menu pages:\n");
  MENU.out("depending the page different menu keys are active.\n");
  MENU.out("For you it's a very simple joke game to find out how it works...\n");

  MENU.ln();
  MENU.out("Menu_visability_test  ==> ETHERNAL LOOP <==:\n");

  Menu_setup();		// add program menu pages
  MENU.menu_display();  // display menu at startup


  while(true)		// ==> ETHERNAL LOOP <==
    MENU.lurk_then_do();

  return 0;
}

#endif // Arduino or 'c++ Linux PC test version'


/* **************************************************************** */
/* Building menu pages:

   The program defines what the menu pages display and how they react.

   The menu can have multiple pages, the selected one gets displayed.
   On start up the first one added will be selected.

   The menu pages define keys ("tokens") and the corresponding actions.

   Below the tokens of the selected/displayed page tokens of other pages
   can be configured to remain visible in groups of related pages.

   For each menu page we will need:

     hotkey		selects this menu page.
     title		identifies page to the user.
     display function	displays the menu page.
     action function	checks if it is responsible for a token,
     	    		if so do its trick and return true,
			else return false.
*/

/* **************************************************************** */
/* Defining page A:

   Page A knows token 'a' and says so,
   but it also suggests to try 'y', which is not visible from here...

   // page A will only be visible when selected, never when burried:
   see Menu_setup();
   MENU.add_page(menuTitleA, menuHotkeyA, &program_displayA, &program_actionA, '-');
*/

const char menuTitleA[] = "'A' PAGE";


/* **************************************************************** */
void program_displayA() {
  MENU.out("\n'a' is active,\t");
  MENU.out("hmm... try 'y' maybe?\n");
}


/* **************************************************************** */
bool program_actionA(char token) {
#ifdef DEBUGGING_MENU
  MENU.out("testing program_actionA(");
  MENU.out(token);
  MENU.out("):\n");
#endif
  switch (token) {
  case 'a':
    MENU.out("\nYES, I DO KNOW TOKEN 'a' :)\n");
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
/* Defining page B:

   Page B knows token 'b'.
   Visability will be set to see the 'Y' group from here,
   this opens "the backdoor" to use 'y', as it is recognized by the
   Y menu, belowing to the same visability group.

   // page B belongs to 'Y' group:
   see Menu_setup();
   MENU.add_page(menuTitleB, menuHotkeyB, &program_displayB, &program_actionB, 'Y');
*/

const char menuTitleB[] = "Back Page";


/* **************************************************************** */
void program_displayB() {
  MENU.out("\nHere we say 'b'\n");
}


/* **************************************************************** */
bool program_actionB(char token) {
#ifdef DEBUGGING_MENU
  MENU.out("testing program_actionB(");
  MENU.out(token);
  MENU.out("):\n");
#endif
  switch (token) {
  case 'b':
    MENU.out("\nnow, I *DO* KNOW token 'b' :)\n");
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
/* Defining page Y:

   Page Y knows token 'y', but (as a joke) will never tell you ;)
   You cannot select it, the hotkey ' ' prevents this.
   You can use a dummy for the display function or a NULL pointer.
   Visability will be set to be visible for the 'Y'group members.
   This opens the "backdoor" to use 'y' for all members of 'Y'.
 
   /* page Y adds actions active for the 'Y' group
      this group cannot get selected, the page hot key ' ' prevents that.
   // see Menu_setup();
   // MENU.add_page(menuTitleY, ' ', &program_displayY, &program_actionY, 'Y');
*/

const char menuTitleY[] = "YYYYY";

/* **************************************************************** */
void program_displayY() {
  MENU.out("\nYOU SHOULD NEVER SEE THIS...\n");
}

/* **************************************************************** */
bool program_actionY(char token) {
#ifdef DEBUGGING_MENU
  MENU.out("testing program_actionY(");
  MENU.out(token);
  MENU.out("):\n");
#endif
  switch (token) {
  case 'y':
    MENU.ln();
    MENU.out("/******************************\\ \n");	// ecape \ ...
    MENU.out("*      you have found the      *\n");
    MENU.out("*                              *\n");
    MENU.out("*    **********************    *\n");
    MENU.out("*    *** *  BACKDOOR  * ***    *\n");
    MENU.out("*    **********************    *\n");
    MENU.out("*    ***/              \\***    *\n");	// ecape \ ...
    MENU.out("*    ***                ***    *\n");
    MENU.out("*    ***   YY      YY   ***    *\n");
    MENU.out("*    ***    YY    YY    ***    *\n");
    MENU.out("*    ***     YY  YY     ***    *\n");
    MENU.out("*    ***      Y\\/Y      ***    *\n");	// ecape \ ...        
    MENU.out("*    ***       YY       ***    *\n");
    MENU.out("*    ***       YY       ***    *\n");
    MENU.out("*    ***       YY       ***    *\n");
    MENU.out("*    ***       YY       ***    *\n");
    MENU.out("*    ***       YY       ***    *\n");
    MENU.out("*    ***                ***    *\n");
    MENU.out("*    ***\\.  .******.  ./***    *\n");	// ecape \ ...
    MENU.out("*    **********************    *\n");
    MENU.out("*    *                    *    *\n");
    MENU.out("*            (: :)             *\n");
    MENU.out("\\******************************/\n");
    MENU.ln();

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
/* Defining page X:

   Page X knows token 'x', but (as a joke) will never tell you ;)
   You cannot select it, the hotkey ' ' prevents this.
   You can use a dummy for the display function or a NULL pointer.
   Visability will be set to be visible for the 'Y'group members.
   This opens the "backdoor" to use 'y' for all members of 'Y'.
 
   // page X is always visible
   see Menu_setup();
   MENU.add_page(menuTitleX, menuHotkeyX, &program_displayX, &program_actionX, '+');
*/

const char menuTitleX[] = "XXX";


/* **************************************************************** */
void program_displayX() {
  MENU.out("\nthere's nothing here to see\n");
}


/* **************************************************************** */
bool program_actionX(char token) {
#ifdef DEBUGGING_MENU
  MENU.out("testing program_actionX(");
    MENU.out(token);
    MENU.out("):\n");
#endif
  switch (token) {
  case 'x':
    MENU.out("\n XX       XX");
    MENU.out("\n  XX     XX");
    MENU.out("\n   XX   XX");
    MENU.out("\n    XX XX");
    MENU.out("\n     XXX");
    MENU.out("\n    XX XX");
    MENU.out("\n   XX   XX");
    MENU.out("\n  XX     XX");
    MENU.out("\n XX       XX\n");

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
/* Defining page N,  testing numeric input:

   Check and change value of a (long) variable defined elsewhere in your program.

   // page N testing numeric input
   see Menu_setup();
   MENU.add_page(menuTitleN, menuHotkeyN, &program_displayN, &program_actionN, '-');
*/

/* the variable must be defined somewhere. Let's just call it 'value'.	*/
long value = 42;

const char menuTitleN[] = "NUMBERS";


/* **************************************************************** */

void program_displayN() {
  MENU.out("\nValue = ");
  MENU.out(value);
  MENU.out("\tv=change value.\n");
}


/* **************************************************************** */
bool program_actionN(char token) {
#ifdef DEBUGGING_MENU
  MENU.out("testing program_actionN(");
  MENU.out(token);
  MENU.out("):\n");
#endif
  switch (token) {
  case 'v':
    MENU.out("Enter new value : ");
    value = MENU.numeric_input(value);
    MENU.out("Set value to ");
    MENU.outln(value);

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
// set the program menu up:

void Menu_setup() {
  // add menu pages:

  // page A will only be visible when selected, never when burried:
  MENU.add_page(menuTitleA, 'A', &program_displayA, &program_actionA, '-');

  // page B belongs to 'Y' group:
  MENU.add_page(menuTitleB, 'B', &program_displayB, &program_actionB, 'Y');

  // page X is always visible
  MENU.add_page(menuTitleX, 'X', &program_displayX, &program_actionX, '+');

  /* page Y adds actions active for the 'Y' group
     this group cannot get selected, the page hot key ' ' prevents that.	*/
  MENU.add_page(menuTitleY, ' ', &program_displayY, &program_actionY, 'Y');

  // page N testing numeric input
  MENU.add_page(menuTitleN, 'N', &program_displayN, &program_actionN, '-');
}

/* **************************************************************** */
