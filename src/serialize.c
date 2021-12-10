


#include <R.h>
#include <Rinternals.h>
#include <Rdefines.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#include "buffer-dynamic.h"
#include "buffer-static.h"
#include "calc-size-robust.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Serialize an R object
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP marshall_(SEXP robj) {

  // Create the buffer for the serialized representation
  // See also: `expand_buffer()` which re-allocates the memory buffer if
  // it runs out of space
  dynamic_buffer_t *buf = init_buffer(16384);

  // Create the output stream structure
  struct R_outpstream_st output_stream;

  // Initialise the output stream structure
  R_InitOutPStream(
    &output_stream,          // The stream object which wraps everything
    (R_pstream_data_t) buf,  // The actual data
    R_pstream_binary_format, // Store as binary
    3,                       // Version = 3 for R >3.5.0 See `?base::serialize`
    write_byte,              // Function to write single byte to buffer
    write_bytes,             // Function for writing multiple bytes to buffer
    NULL,                    // Func for special handling of reference data.
    R_NilValue               // Data related to reference data handling
  );

  // Serialize the object into the output_stream
  R_Serialize(robj, &output_stream);

  // Copy just the valid bytes to return to the user
  SEXP res_ = PROTECT(allocVector(RAWSXP, buf->pos));
  memcpy(RAW(res_), buf->data, buf->pos);

  // Free all the memory
  free(buf->data);
  free(buf);
  UNPROTECT(1);
  return res_;
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
SEXP marshall_fast_(SEXP robj) {

  // Figure out how much memory is needed
  int total_size;
  total_size = calc_size_robust(robj);

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
