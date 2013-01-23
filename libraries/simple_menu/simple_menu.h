/*
  simple_menu.h
*/

#ifndef SIMPLE_MENU
#define SIMPLE_MENU

class Simple_menu : public Circ_accumulator
{
 public:
  Simple_menu(int size, bool (*is_input)(void), int (*get_char)(void));
  ~Simple_menu();
  void lurk();

 private:
  void display();
  int react();
};

#endif
