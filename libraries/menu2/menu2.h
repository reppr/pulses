/*
  menu2.h
  menu2 as monolithic base class
*/

#ifndef MENU2
#define MENU2

class Menu2
{
 public:
  Menu2(int size, int (*maybeInput)(void));
  ~Menu2();

  bool lurk_and_do(void (*Action)(void));
  int cb_stored() const { return cb_count; }	// inline:  # stored bytes
  char cb_read();				// get a byte from buffer, no checks

  int cb_peek() const;				// peek at next if any, else return EOL
  int cb_peek(int offset) const;		// peek at next, overnext... if any, else EOL
  void skip_spaces();				// skip leading spaces from the buffer
  int next_input_token();			// next non space input token if any, else EOL
  bool is_numeric();				// test if next token will be a numeric chiffre
  long numeric_input(long default_value);	// read a number from the buffer
  void skip_numeric_input(void);		// drop leading numeric sequence from the buffer

  void menu_display();				// display menu
  void common_display();			// display common to all menu pages

 protected:
  bool cb_is_full() const { return cb_count == cb_size; }	// inlined: buffer full?
#ifdef DEBUGGING
  void cb_info() const;						// debugging help
#endif
  int (*maybe_input)(void);	// maybe_input() must return EOF or next char
  void (*action)(void);		// will be called on receiving an end token

 private:
  int cb_start;
  int cb_size;
  int cb_count;
  char * cb_buf;
  void cb_write(char value);			// save a byte in buffer,  no checks
};

#endif
