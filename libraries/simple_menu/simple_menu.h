/*
  simple_menu.h
*/

#ifndef SIMPLE_MENU
#define SIMPLE_MENU

class Simple_menu
{
 public:
  Simple_menu(int size, bool (*is_input)(void), int (*get_char)(void));
  ~Simple_menu();
  void lurk();

 private:
  void display();
  int react();
  bool (*is_input)(void);
  int (*get_char)(void);
  //  circ_buf * _cb;
};

#endif
