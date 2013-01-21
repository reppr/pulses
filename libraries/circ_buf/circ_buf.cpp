#include <stdlib.h>
#include <iostream>
#include "circ_buf.h"


// constructor:
Circ_buf::Circ_buf(int size)
{
  _size  = size;
  _start = 0;
  _count = 0;
  _buf = (char *) malloc(size);	// could fail ################
}


// destructor:
Circ_buf::~Circ_buf()
{
  free(_buf);
  _buf   = NULL;
  _size  = 0;
  _start = 0;
  _count = 0;
}


int Circ_buf::cb_is_full() {
  return _count == _size;
}


int Circ_buf::cb_stored() {	// returns number of buffered bytes
  return _count;
}


/*
  cb_write() save a byte to the buffer:
  does *not* check if buffer is full
*/
void Circ_buf::cb_write(char value) {
  int end = (_start + _count) % _size;
  _buf[end] = value;
  if (_count == _size)
    _start = (_start + 1) % _size;
  else
    ++_count;
}


/*
  cb_read() get oldest byte from the buffer:
  does *not* check if buffer is empty
*/
char Circ_buf::cb_read() {
  char value = _buf[_start];
  _start = (_start + 1) % _size;
  --_count;
  return value;
}


/*
  cb_recover_last()  recover one byte *read immediately before*
  *no checks inside*
*/
// cb_recover_last() OR cb_peek()    don't think we need both ################
void Circ_buf::cb_recover_last() {
  _start = (_start - 1 + _size) % _size;	// safety net ;)
  ++_count;
}


/*
  int cb_peek()
  return -1 if buffer is empty, else
  return next char without removing it from buffer
*/
// cb_recover_last() OR cb_peek()    don't think we need both ################
int Circ_buf::cb_peek() {
  if (_count == 0)
    return -1;

  char value = _buf[_start];
  return value;
}


// for debugging
void Circ_buf::cb_info() {
  std::cout << "\nBuffer:\t\t";
  std::cout << (int) _buf;

  std::cout << "\n  size:\t\t";
  std::cout << _size;

  std::cout << "\n  count:\t";
  std::cout << _count;

  std::cout << "\n  start:\t";
  std::cout << _start;

  int value = cb_peek();
  std::cout << "\n  char:\t\t";
  if (value != -1)
    std::cout << (char) value;
  else
    std::cout << "(none)";
  std::cout << "\n";

  std::cout << "\n";
}
