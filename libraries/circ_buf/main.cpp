// test only

#include <iostream>
#include <stdio.h>
#include "circ_buf.cpp"
#include "simple_menu.cpp"

char scratch[80];

bool input_test() {
  int INP;
  std::cout << "testing on input\n";

  if ( (INP = getchar_unlocked()) != EOF ) {
    putchar_unlocked(INP);
    return true;
  }

  return false;
}


int main() {
  std::cout << "testing main:\n";

  //  Simple_menu MENU(64, &input_test, &getchar);
  Simple_menu MENU(64, &input_test, &getchar_unlocked);

  MENU.lurk();
  std::cout << "done\n";
return 99;

  std::cout << "\ttesting circ_buf\n";
  Circ_buf CB(64);
  CB.cb_info();

  char c;
  char * p;

  p = scratch;
  *p=0;
  std::cout << "enter test string:\t";
  std::cin.get(scratch, 79);

  while (c = *p++)
    CB.cb_write(c);
  CB.cb_info();

  std::cout << "read from buffer:\t";
  while (CB.cb_stored())
    std::cout << CB.cb_read();
  std::cout << "\n";
  CB.cb_info();


  return 0;
}
