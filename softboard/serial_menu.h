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
