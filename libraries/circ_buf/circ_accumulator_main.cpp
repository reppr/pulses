/*
  testing circ_accumulator
*/

#include <iostream>
#include <stdio.h>


#define DEBUGGING
#include "circ_accumulator.cpp"

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
  std::cout << "testing circ_accumulator main:\n";

  Circ_accumulator ACC(64, &input_test, &getchar_unlocked);	// getchar_unlocked()

  ACC.gather_then_do();

/*
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
*/

  std::cout << "done\n";
  return 0;
}
