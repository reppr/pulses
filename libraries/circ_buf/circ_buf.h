// circ_buf.h

#ifndef CIRC_BUF
#define CIRC_BUF

class Circ_buf
{
 public:
  Circ_buf(int size);
  ~Circ_buf();
  void cb_write(char value);			// save a byte in buffer,  no checks
  char cb_read();				// get a byte from buffer, no checks
  int cb_peek() const;				// peek at next if any, else return error
  int cb_stored() const { return _count; }		// inlined: number of stored bytes
  bool cb_is_full() const { return _count == _size; }	// inlined: is the buffer full?
  void cb_info() const;				// debugging help

 private:
  int _size;					// maximum number of elements
  int _start;					// index of oldest element, read index
  int _count;					// buffered bytes
  char * _buf;					// *buffer
};

#endif
