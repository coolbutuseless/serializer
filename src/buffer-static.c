


#include <R.h>
#include <Rinternals.h>
#include <Rdefines.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "buffer-static.h"


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Initialise an empty buffer to hold 'nbytes'
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static_buffer_t *init_static_buffer(int nbytes, unsigned char *data) {
  static_buffer_t *buf = (static_buffer_t *)malloc(sizeof(static_buffer_t));
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
  static_buffer_t *buf = (static_buffer_t *)stream->data;

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
  static_buffer_t *buf = (static_buffer_t *)stream->data;

  // Expand the buffer if it's out space
  while (buf->pos + length > buf->length) {
    error("write bytes static buffer failed\n");
  }

  memcpy(buf->data + buf->pos, src, length);

  buf->pos += length;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Read a byte from the serialized stream
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int read_byte_from_static_buffer(R_inpstream_t stream) {
  static_buffer_t *buf = (static_buffer_t *)stream->data;

  if (buf->pos >= buf->length) {
    error("read_byte(): overflow");
  }

  return buf->data[buf->pos++];
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Read multiple bytes from the serialized stream
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void read_bytes_from_static_buffer(R_inpstream_t stream, void *dst, int length) {
  static_buffer_t *buf = (static_buffer_t *)stream->data;

  if (buf->pos + length > buf->length) {
    error("read_bytes(): overflow");
  }

  memcpy(dst, buf->data + buf->pos, length);

  buf->pos += length;
}
