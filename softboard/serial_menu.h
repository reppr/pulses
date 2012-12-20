/*
  minimal include file,
  just enough to keep arduino GUI happy when building softboard
*/


typedef struct {
  int size;
  int start;
  int count;
  char *buf;
} circ_buf;

circ_buf serial_input_BUFFER;


/* no idea why i had to put these: */
void serial_menu_reaction(char menu_input);

void menu_program_display();
bool serial_menu_program_reaction(char);
