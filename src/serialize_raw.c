


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
  R_xlen_t length;     // Total length of the buffer
  R_xlen_t pos;        // The current position in which to write the next data
  unsigned char *data; // The actual serialized data
} static_buffer_t;


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Write a byte into the buffer at the current location.
// The actual buffer is encapsulated as part of the stream structure, so you
// have to extract it first
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void write_byte_to_static_buffer(R_outpstream_t stream, int c) {
  error("write_byte_to_static_buffer(): This function is never used in binary serialization");
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Write multiple bytes into the buffer at the current location.
// The actual buffer is encapsulated as part of the stream structure, so you
// have to extract it first
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void write_bytes_to_static_buffer(R_outpstream_t stream, void *src, int length) {
  static_buffer_t *buf = (static_buffer_t *)stream->data;
  
  // Sanity check we're not writing out-of-bounds
  while (buf->pos + length > buf->length) {
    error("write_bytes_to_static_buffer(): overflow\n");
  }
  
  // Copy the data and advance the 'pos' pointer
  memcpy(buf->data + buf->pos, src, length);
  buf->pos += length;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Read a byte from the serialized stream
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int read_byte_from_static_buffer(R_inpstream_t stream) {
  error("read_byte_from_static_buffer(): This function is never used in binary serialization");
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Read multiple bytes from the serialized stream
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void read_bytes_from_static_buffer(R_inpstream_t stream, void *dst, int length) {
  static_buffer_t *buf = (static_buffer_t *)stream->data;
  
  // Sanity check we're not reading out-of-bounts
  if (buf->pos + length > buf->length) {
    error("read_bytes_from_static_buffer(): overflow");
  }
  
  // copy the data and advance the 'pos' pointer
  memcpy(dst, buf->data + buf->pos, length);
  buf->pos += length;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Unserialize a raw vector to an R object
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP unmarshall_raw_(SEXP vec_) {

  if (TYPEOF(vec_) != RAWSXP) {
    error("unmarshall_raw_(): Argument must be a raw vector");
  }

  // In C, we want the data as a 'void *'
  void *vec = (void *)RAW(vec_);
  R_xlen_t len = Rf_xlength(vec_);

  // Create a buffer object which points to the raw data
  static_buffer_t buf;
  buf.length = len;
  buf.pos    = 0;
  buf.data   = vec;

  // Treat the data buffer as an input stream
  struct R_inpstream_st input_stream;

  R_InitInPStream(
    &input_stream,                 // Stream object wrapping data buffer
    (R_pstream_data_t) &buf,        // user data which persists during the serialization
    R_pstream_any_format,          // Unpack all serialized types
    read_byte_from_static_buffer,  // Function to read single byte from buffer
    read_bytes_from_static_buffer, // Function for reading multiple bytes from buffer
    NULL,                          // Func for special handling of reference data.
    NULL                           // Data related to reference data handling
  );

  // Unserialize the input_stream into an R object
  return R_Unserialize(&input_stream);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Serialize an R object to a raw vector
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP marshall_raw_(SEXP robj) {

  // Figure out how much memory is needed
  R_xlen_t total_size = calc_serialized_size(robj);

  // Allocate an exact-sized R RAW vector to hold the result
  SEXP res_ = PROTECT(allocVector(RAWSXP, total_size));
  
  // Create a buffer object which points to the raw data
  static_buffer_t buf;
  buf.length = total_size;
  buf.pos    = 0;
  buf.data   = RAW(res_);

  // Create the output stream structure
  struct R_outpstream_st output_stream;

  // Initialise the output stream structure
  R_InitOutPStream(
    &output_stream,               // The stream object which wraps everything
    (R_pstream_data_t) &buf,      // user data which persists during the serialization
    R_pstream_binary_format,      // Serialize as binary
    3,                            // Version = 3 for R >3.5.0 See `?base::serialize`
    write_byte_to_static_buffer,  // Function to write single byte to buffer
    write_bytes_to_static_buffer, // Function for writing multiple bytes to buffer
    NULL,                         // Func for special handling of reference data.
    R_NilValue                    // Data related to reference data handling
  );

  // Serialize the object into the output_stream
  R_Serialize(robj, &output_stream);

  // Tidy and return
  UNPROTECT(1);
  return res_;
}


