/*
  circ_buf.h
*/

#ifndef CIRC_BUF
#define CIRC_BUF

struct circ_buf {
  int size;
  int start;
  int count;
  char * buf;
};

class Circ_buf
{
 public:
  Circ_buf(int size);
  ~Circ_buf();
  void cb_write(char value);			// save a byte in buffer,  no checks
  char cb_read();				// get a byte from buffer, no checks
  int cb_peek() const;				// peek at next if any, else return error
  int cb_stored() const { return CB.count; }			// inline:  # stored bytes
  bool cb_is_full() const { return CB.count == CB.size; }	// inlined: buffer full?
  void cb_info() const;						// debugging help

 protected:
  circ_buf CB;
};

#endif
