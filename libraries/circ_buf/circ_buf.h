// circ_buf.h

#ifndef CIRC_BUF
#define CIRC_BUF

class Circ_buf
{
 public:
  Circ_buf(int size);
  ~Circ_buf();
  int cb_stored();
  void cb_write(char value);
  char cb_read();
  int cb_is_full();
  void cb_recover_last();	// don't think we need both
  int cb_peek();		// don't think we need both
  void cb_info();
 private:
  int _size;		// maximum number of elements
  int _start;		// index of oldest element, read index
  int _count;		// buffered bytes
  char * _buf;		// *buffer
};

#endif
