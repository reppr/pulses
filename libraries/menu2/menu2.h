/*
  menu2.h
  menu2 as monolithic base class
*/

#ifndef MENU2
#define MENU2

class Menu2
{
 public:
  Menu2(int size);
  ~Menu2();

  bool lurk_then_react();
  int cb_count;
  // protected:
 private:
  int cb_start;
  int cb_size;
  char * cb_buf;
};

#endif
