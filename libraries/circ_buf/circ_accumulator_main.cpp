/*
  circ_accumulator_main.cpp	testing circ_accumulator

  Extend Circ_buf class to accumulate incoming bytes until 'end token'
  then do a programmable action that has read access to the buffer.

  First change typical line ending codes \n and \c to end tokens \0.

  If the result is not \0 = 'end token',
    then accumulate it.
  else,
    on receiving an end token,
    if the buffer is not empty,
      then trigger action.
    empty end tokens (from end of line codes translation) get ignored.
*/

#include <iostream>

#define DEBUGGING
#include "circ_accumulator.cpp"

char scratch[80];

Circ_accumulator ACC(64, &getchar);		// getchar(), getchar_unlocked()

/* void do_it()
   Do anything when an end token is received and
   the buffer is not empty.
   This is just a stub showing buffered content.		*/
void do_it() {
  char c;

  std::cout << "program ACTION HAPPENS on: ";

  while ( ACC.cb_stored() ) {
    c = ACC.cb_read();
    std::cout << c;
  }
  std::cout << "\n";
}


int main() {
  std::cout << "testing circ_accumulator main:  ==>  LOOPS FOREVER... <==\n";

  while(true)		// ==>  LOOPS FOREVER... <==
    ACC.gather_then_do(&do_it);

  std::cout << "done\n";
  return 0;
}
