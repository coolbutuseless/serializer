


#include <R.h>
#include <Rinternals.h>
#include <Rdefines.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "calc-serialized-size.h"


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// The data buffer.
// Needs total length and pos to keep track of how much data it contains
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
typedef struct {
  R_xlen_t length;
  R_xlen_t pos;
  unsigned char *data;
} static_buffer_t;


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


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Unpack a raw vector to an R object
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP unmarshall_(SEXP vec_) {

  if (TYPEOF(vec_) != RAWSXP) {
    error("unpack(): Only raw vectors can be unserialized");
  }

  // Unpack the raw vector into a C void *
  void *vec = RAW(vec_);
  R_xlen_t len = XLENGTH(vec_);

  // Create a buffer object which points to the raw data
  static_buffer_t *buf = malloc(sizeof(static_buffer_t));
  if (buf == NULL) {
    error("'buf' malloc failed!");
  }
  buf->length = len;
  buf->pos    = 0;
  buf->data   = vec;

  // Treat the data buffer as an input stream
  struct R_inpstream_st input_stream;

  R_InitInPStream(
    &input_stream,                 // Stream object wrapping data buffer
    (R_pstream_data_t) buf,        // Actual data buffer
    R_pstream_any_format,          // Unpack all serialized types
    read_byte_from_static_buffer,  // Function to read single byte from buffer
    read_bytes_from_static_buffer, // Function for reading multiple bytes from buffer
    NULL,                          // Func for special handling of reference data.
    NULL                           // Data related to reference data handling
  );

  // Unserialize the input_stream into an R object
  SEXP res_  = PROTECT(R_Unserialize(&input_stream));

  free(buf);
  UNPROTECT(1);
  return res_;
}





//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Serialize an R object. Precalculate the resulting size so that
// the number of memory allocations can be minimised
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP marshall_(SEXP robj) {

  // Figure out how much memory is needed
  int total_size;
  total_size = calc_serialized_size(robj);

  // Allocate an exact-sized R RAW vector to hold the result
  SEXP res_ = PROTECT(allocVector(RAWSXP, total_size));
  static_buffer_t *buf = init_static_buffer(total_size, RAW(res_));

  // Create the output stream structure
  struct R_outpstream_st output_stream;

  // Initialise the output stream structure
  R_InitOutPStream(
    &output_stream,               // The stream object which wraps everything
    (R_pstream_data_t) buf,       // The actual data
    R_pstream_binary_format,      // Store as binary
    3,                            // Version = 3 for R >3.5.0 See `?base::serialize`
    write_byte_to_static_buffer,  // Function to write single byte to buffer
    write_bytes_to_static_buffer, // Function for writing multiple bytes to buffer
    NULL,                         // Func for special handling of reference data.
    R_NilValue                    // Data related to reference data handling
  );

  // Serialize the object into the output_stream
  R_Serialize(robj, &output_stream);

  // Free all the memory
  free(buf);
  UNPROTECT(1);
  return res_;
}


