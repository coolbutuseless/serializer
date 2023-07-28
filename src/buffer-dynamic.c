


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
dynamic_buffer_t *init_buffer(int nbytes) {
  dynamic_buffer_t *buf = (dynamic_buffer_t *)malloc(sizeof(dynamic_buffer_t));
  if (buf == NULL) {
    error("init_buffer(): cannot malloc buffer");
  }

  buf->data = (unsigned char *)malloc(nbytes * sizeof(unsigned char));
  if (buf->data == NULL) {
    error("init_buffer(): cannot malloc buffer data");
  }

  buf->length = nbytes;
  buf->pos = 0;

  return buf;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Naive buffer expansion - double it every time space runs out
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void expand_buffer(dynamic_buffer_t *buf, R_xlen_t length) {

  double factor = 2;

  do {
    buf->length = (R_xlen_t)(factor * buf->length);
  } while (buf->length < buf->pos + length);

  if (buf->length > R_XLEN_T_MAX) {
    error("Requested buffer expandsion too large: %td\n", buf->length);
  }

  unsigned char *new_data = (unsigned char *)realloc((void *)buf->data, buf->length);

  if (new_data == NULL) {
    free(buf->data);
    free(buf);
    error("Couldn't expand buffer to reallocate: %td\n", buf->length);
  }

  buf->data = new_data;

}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Write a byte into the buffer at the current location.
// The actual buffer is encapsulated as part of the stream structure, so you
// have to extract it first
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void write_byte(R_outpstream_t stream, int c) {
  dynamic_buffer_t *buf = (dynamic_buffer_t *)stream->data;

  // Expand the buffer if it's out space
  if (buf->pos >= buf->length) {
    expand_buffer(buf, 1);
  }

  buf->data[buf->pos++] = (unsigned char)c;
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Write multiple bytes into the buffer at the current location.
// The actual buffer is encapsulated as part of the stream structure, so you
// have to extract it first
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void write_bytes(R_outpstream_t stream, void *src, int length) {
  dynamic_buffer_t *buf = (dynamic_buffer_t *)stream->data;

  // Expand the buffer if it's out space
  if (buf->pos + length > buf->length) {
    expand_buffer(buf, length);
  }

  memcpy(buf->data + buf->pos, src, length);

  buf->pos += length;
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Read a byte from the serialized stream
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int read_byte(R_inpstream_t stream) {
  dynamic_buffer_t *buf = (dynamic_buffer_t *)stream->data;

  if (buf->pos >= buf->length) {
    error("read_byte(): overflow");
  }

  return buf->data[buf->pos++];
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Read multiple bytes from the serialized stream
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void read_bytes(R_inpstream_t stream, void *dst, int length) {
  dynamic_buffer_t *buf = (dynamic_buffer_t *)stream->data;

  if (buf->pos + length > buf->length) {
    error("read_bytes(): overflow");
  }

  memcpy(dst, buf->data + buf->pos, length);

  buf->pos += length;
}
