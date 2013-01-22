/*
   circ_buf.cpp
*/

#include <stdlib.h>
#include <iostream>

#include "circ_buf.h"


// constructor:
Circ_buf::Circ_buf(int size=64)
{
#ifdef DEBUGGING
  std::cout << "CONSTRUCTING Circ_buf\n";
#endif
  CB.size  = size;
  CB.start = 0;
  CB.count = 0;
  CB.buf = (char *) malloc(size);	// could fail ################
}


// destructor:
Circ_buf::~Circ_buf()
{
#ifdef DEBUGGING
  std::cout << "DESTRUCTING Circ_buf\n";
#endif
  free(CB.buf);
  CB.buf   = NULL;
}


/*
  cb_write() save a byte to the buffer:
  does *not* check if buffer is full
*/
void Circ_buf::cb_write(char value) {
  int end = (CB.start + CB.count) % CB.size;
  CB.buf[end] = value;
  if (CB.count == CB.size)
    CB.start = (CB.start + 1) % CB.size;
  else
    ++CB.count;
}


/*
  cb_read() get oldest byte from the buffer:
  does *not* check if buffer is empty
*/
char Circ_buf::cb_read() {
  char value = CB.buf[CB.start];
  CB.start = (CB.start + 1) % CB.size;
  --CB.count;
  return value;
}


/*
  int cb_peek()
  return -1 if buffer is empty, else
  return next char without removing it from buffer
*/
int Circ_buf::cb_peek() const {
  if (CB.count == 0)
    return -1;

  char value = CB.buf[CB.start];
  return value;
}


/* inlined:
int Circ_buf::cb_stored() {	// returns number of buffered bytes
  return CB.count;
}

// inlined:
int Circ_buf::cb_is_full() {
  return CB.count == CB.size;
}
*/


// cb_info() debugging help
void Circ_buf::cb_info() const {
  std::cout << "\nBuffer:\t\t";
  std::cout << (long) CB.buf;

  std::cout << "\n  size:\t\t";
  std::cout << CB.size;

  std::cout << "\n  count:\t";
  std::cout << CB.count;

  std::cout << "\n  start:\t";
  std::cout << CB.start;

  int value = cb_peek();
  std::cout << "\n  char:\t\t";
  if (value != -1)
    std::cout << (char) value;
  else
    std::cout << "(none)";
  std::cout << "\n";

  std::cout << "\n";
}
