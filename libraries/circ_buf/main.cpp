// test only

#include <iostream>
#include "circ_buf.cpp"

char scratch[80];

int main() {
  std::cout << "testing main:\n";

  std::cout << "\ttesting circ_buf\n";
  Circ_buf CB(64);
  CB.cb_info();

  std::cout << "enter test string:\t";
  std::cin.get(scratch, 79);

  char * p = scratch;
  char c;
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
