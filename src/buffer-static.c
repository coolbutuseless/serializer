


#include <R.h>
#include <Rinternals.h>
#include <Rdefines.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "buffer-dynamic.h"


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Initialise an empty buffer to hold 'nbytes'
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
dynamic_buffer_t *init_static_buffer(int nbytes, unsigned char *data) {
  dynamic_buffer_t *buf = (dynamic_buffer_t *)malloc(sizeof(dynamic_buffer_t));
  if (buf == NULL) {
    error("init_buffer(): cannot malloc buffer");
  }

  buf->data = data;

  buf->length = nbytes;
  buf->pos = 0;

  return buf;
}




//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Write a byte into the buffer at the current location.
// The actual buffer is encapsulated as part of the stream structure, so you
// have to extract it first
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void write_byte_to_static_buffer(R_outpstream_t stream, int c) {
  dynamic_buffer_t *buf = (dynamic_buffer_t *)stream->data;

  // Expand the buffer if it's out space
  while (buf->pos >= buf->length) {
    error("write byte static buffer failed.\n");
  }

  buf->data[buf->pos++] = (unsigned char)c;
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Write multiple bytes into the buffer at the current location.
// The actual buffer is encapsulated as part of the stream structure, so you
// have to extract it first
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void write_bytes_to_static_buffer(R_outpstream_t stream, void *src, int length) {
  dynamic_buffer_t *buf = (dynamic_buffer_t *)stream->data;

  // Expand the buffer if it's out space
  while (buf->pos + length > buf->length) {
    error("write bytes static buffer failed\n");
  }

  memcpy(buf->data + buf->pos, src, length);

  buf->pos += length;
}


