#include "circ_buf.h"
#include <stdlib.h>

void cb_init(circ_buf *cb, int size) {
  cb->size  = size;
  cb->start = 0;
  cb->count = 0;
  cb->buf = (char *) malloc(cb->size);
}


// not used
void cb_free(circ_buf *cb) {
  free(cb->buf);
  cb->buf = NULL;
}


int cb_is_full(circ_buf *cb) {
  return cb->count == cb->size;
}


int cb_stored(circ_buf *cb) {	// returns number of buffered bytes
  return cb->count;
}


/*
  cb_write() save a byte to the buffer:
  does *not* check if buffer is full
*/
void cb_write(circ_buf *cb, char value) {
  int end = (cb->start + cb->count) % cb->size;
  cb->buf[end] = value;
  if (cb->count == cb->size)
    cb->start = (cb->start + 1) % cb->size;
  else
    ++ cb->count;
}


/*
  cb_read() get oldest byte from the buffer:
  does *not* check if buffer is empty
*/
char cb_read(circ_buf *cb) {
  char value = cb->buf[cb->start];
  cb->start = (cb->start + 1) % cb->size;
  --cb->count;
  return value;
}


/*
  cb_recover_last(): recover one byte *read immediately before*
  *no checks inside*
*/
void cb_recover_last(circ_buf *cb) {
  cb->start = (cb->start - 1 + cb->size) % cb->size;	// safety net ;)
  ++cb->count;
}


/*
  int cb_peek():
  return -1 if buffer is empty, else
  return next char without removing it from buffer
*/
int cb_peek(circ_buf *cb) {
  if (cb->count == 0)
    return -1;

  char value = cb->buf[cb->start];
  return value;
}
