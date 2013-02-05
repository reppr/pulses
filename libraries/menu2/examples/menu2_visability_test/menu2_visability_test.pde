/* **************************************************************** */
/*
  menu2_visability_test
  Simple example for menu2 library. 
*/
/* **************************************************************** */

#include <stdio.h>


#include <menu2.h>
// #include <menu2.cpp>


#define BAUDRATE	115200		// works fine here


/* **************************************************************** */

Menu2 MENU(32, 5, &getcharMACRO);


/* **************************************************************** */
/* main loop(); For both versions: Arduino and c++ PC test version.
void loop() {
  /*
    All you have to from your Arduino sketch loop() is to call:
    MENU.lurk_then_do();

    This will *not* block the loop.
    It will lurk on menu input, return immediately if there's none.
    On input if will just accumulate next input byte and return, *if*
    it was not an END token.

    When seeing one of the usual line endings it takes it as 'END token' \0.

    On receiving an 'END token' it will interpret the accumulated tokens and return.

    So normally just the following line should do fine:
    MENU.lurk_then_do();
    should do fine.
    This version does the same but can also generate debugging output for me...
  */
  if ( MENU.lurk_then_do() ) {
#if defined(DEBUGGING_MENU) || defined(DEBUGGING_LURKING)
    outMACRO << "lurk_then_do() interpreted buffer content, return TRUE\n";
  } else {
    outMACRO << "lurk_then_do() just lurking...\n";
#endif
  }
}


#ifdef ARDUINO
/* **************************************************************** */
// Arduino setup():


void setup() {
  Serial.begin(BAUDRATE);	// Start serial communication.
  Menu2_setup();		// Tell the menu what to do.
}

// Already defined, see above:
// void loop() {
//   MENU.lurk_then_do();
// }
/* **************************************************************** */


#else // c++ PC test version
/* **************************************************************** */
// c++ PC test version main():
int main() {
  outMACRO << "menu2_visability_test  ==> ETHERNAL LOOP <==:\n";

  menu2_setup();

  loop_body();		// ==> ETHERNAL LOOP <==

  return 0;
}
/* **************************************************************** */
#endif // Arduino or 'c++ PC test'


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
   see menu2_setup();
   MENU.add_page(menuTitleA, menuHotkeyA, &program_displayA, &program_actionA, '-');
*/

char menuTitleA[] = "'A' PAGE";
char menuHotkeyA = 'A';


/* **************************************************************** */
void program_displayA() {
  outMACRO << "\n'a' is active,\t";
  outMACRO << "hmm... try 'y' maybe?\n";
}


/* **************************************************************** */
bool program_actionA(char token) {
#ifdef DEBUGGING_MENU
  outMACRO << "testing program_actionA(" << token << "):\n";
#endif
  switch (token) {
  case 'a':
    outMACRO << "\nYES, I DO KNOW TOKEN 'a' :)\n";
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
   see menu2_setup();
   MENU.add_page(menuTitleB, menuHotkeyB, &program_displayB, &program_actionB, 'Y');
*/

char menuTitleB[] = "Back Page";
char menuHotkeyB = 'B';


/* **************************************************************** */
void program_displayB() {
  outMACRO << "\nHere we say 'b'\n";
}


/* **************************************************************** */
bool program_actionB(char token) {
#ifdef DEBUGGING_MENU
  outMACRO << "testing program_actionB(" << token << "):\n";
#endif
  switch (token) {
  case 'b':
    outMACRO << "\nnow, I *DO* KNOW token 'b' :)\n";
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
      this group cannot get selected, the page hot key ' ' prevents that.	*/
   see menu2_setup();
   MENU.add_page(menuTitleY, ' ', &program_displayY, &program_actionY, 'Y');
*/

char menuTitleY[] = "YYYYY";
char menuHotkeyY = ' ';

/* **************************************************************** */
void program_displayY() {
  outMACRO << "\nYOU SHOULD NEVER SEE THIS...\n";
}

/* **************************************************************** */
bool program_actionY(char token) {
#ifdef DEBUGGING_MENU
  outMACRO << "testing program_actionY(" << token << "):\n";
#endif
  switch (token) {
  case 'y':
    outMACRO << "\n";
    outMACRO << "/°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°\\ \n";	// ecape \ ...
    outMACRO << "°      you have found the      °\n";
    outMACRO << "°                              °\n";
    outMACRO << "°    °********************°    °\n";
    outMACRO << "°    °** *  BACKDOOR  * **°    °\n";
    outMACRO << "°    °********************°    °\n";
    outMACRO << "°    °**/              \\**°    °\n";	// ecape \ ...
    outMACRO << "°    °**                **°    °\n";
    outMACRO << "°    °**   YY      YY   **°    °\n";
    outMACRO << "°    °**    YY    YY    **°    °\n";
    outMACRO << "°    °**     YY  YY     **°    °\n";
    outMACRO << "°    °**      Y\\/Y      **°    °\n";	// ecape \ ...        
    outMACRO << "°    °**       YY       **°    °\n";
    outMACRO << "°    °**       YY       **°    °\n";
    outMACRO << "°    °**       YY       **°    °\n";
    outMACRO << "°    °**       YY       **°    °\n";
    outMACRO << "°    °**       YY       **°    °\n";
    outMACRO << "°    °**                **°    °\n";
    outMACRO << "°    °**\\.  .******.  ./**°    °\n";	// ecape \ ...
    outMACRO << "°    °********************°    °\n";
    outMACRO << "°    °                    °    °\n";
    outMACRO << "°            (: :)             °\n";
    outMACRO << "°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°\n";
    outMACRO << "\n";

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
   see menu2_setup();
   MENU.add_page(menuTitleX, menuHotkeyX, &program_displayX, &program_actionX, '+');
*/

char menuTitleX[] = "XXX";
char menuHotkeyX = 'X';


/* **************************************************************** */
void program_displayX() {
  outMACRO << "\nthere's nothing here to see\n";
}


/* **************************************************************** */
bool program_actionX(char token) {
#ifdef DEBUGGING_MENU
  outMACRO << "testing program_actionX(" << token << "):\n";
#endif
  switch (token) {
  case 'x':
    outMACRO << "\n XX       XX";
    outMACRO << "\n  XX     XX";
    outMACRO << "\n   XX   XX";
    outMACRO << "\n    XX XX";
    outMACRO << "\n     XXX";
    outMACRO << "\n    XX XX";
    outMACRO << "\n   XX   XX";
    outMACRO << "\n  XX     XX";
    outMACRO << "\n XX       XX\n";

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
   see menu2_setup();
   MENU.add_page(menuTitleN, menuHotkeyN, &program_displayN, &program_actionN, '-');
*/

/* the variable must be defined somewhere. Let's just call it 'value'.	*/
long value = 42;

char menuTitleN[] = "NUMBERS";
char menuHotkeyN = 'N';


/* **************************************************************** */

void program_displayN() {
  outMACRO << "\nValue = " << value << "\tv=change value.\n";
}


/* **************************************************************** */
bool program_actionN(char token) {
#ifdef DEBUGGING_MENU
  outMACRO << "testing program_actionN(" << token << "):\n";
#endif
  switch (token) {
  case 'v':
    outMACRO << "Enter new value : ";
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
// set the program menu up:

void menu2_setup() {
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

/* **************************************************************** */
