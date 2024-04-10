


#include <R.h>
#include <Rinternals.h>
#include <Rdefines.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "calc-serialized-size.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Read a byte from the serialized stream
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int read_byte_from_connection(R_inpstream_t stream) {
  error("read_byte_from_connection() never used for binary serializing");
  return 0;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Read multiple bytes from the connection
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void read_bytes_from_connection(R_inpstream_t stream, void *dst, int length) {
  
  // Get the connection from the user data
  SEXP con_ = (SEXP)stream->data;
  
  // Call 'readBin(con, raw(), length)' in R
  // Modelled after jsonlite/src/push_parser.c  
  SEXP call = PROTECT(
    Rf_lang4(
      PROTECT(Rf_install("readBin")),
      con_,                                 // con
      PROTECT(Rf_allocVector(RAWSXP, 0)),   // what = raw()
      PROTECT(Rf_ScalarInteger(length))     // n
    ));
  
  // Actually evaluate the call
  SEXP out = PROTECT(Rf_eval(call, R_BaseEnv));
  
  R_xlen_t len = Rf_xlength(out);
  
  // Sanity check to see if any data returned
  if(len <= 0) {
    error("read_bytes_from_connection() returned %ld bytes read", len);
  }
  
  // Sanity check that we read the requested number of bytes from the connection
  if (len != length) {
    error("read_bytes_from_connection(). Expected %i bytes to be read, but actually read %ld", length, len);
  }
  
  memcpy(dst, RAW(out), length);
  UNPROTECT(5);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Read a byte from the serialized stream
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void write_byte_to_connection(R_outpstream_t stream, int c) {
  error("write_byte_to_connection() never used for binary serializing");
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Read multiple bytes from the connection
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void write_bytes_to_connection(R_outpstream_t stream, void *src, int length) {
  
  // Get the connection from the user data
  SEXP con_ = (SEXP)stream->data;
  
  // Create an R raw vector from the 'src' data in order to pass it
  // to an R function.
  SEXP raw_vec_ = PROTECT(allocVector(RAWSXP, length));
  memcpy(RAW(raw_vec_), src, length);
  
  // Create a call to:  writeBin(raw_vec, con)
  // Modelled after jsonlite/src/push_parser.c  
  SEXP call = PROTECT(
    Rf_lang3(
      PROTECT(Rf_install("writeBin")), 
      raw_vec_, // raw vector
      con_      // con
    )
  );
  
  // Evaluate the call to write the data
  Rf_eval(call, R_BaseEnv);
  
  UNPROTECT(3);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Unpack a raw vector to an R object
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP unmarshall_con_(SEXP con_) {

  // Treat the data buffer as an input stream
  struct R_inpstream_st input_stream;

  R_InitInPStream(
    &input_stream,                 // Stream object wrapping data buffer
    (R_pstream_data_t) con_,       // the user data is just a (void *) to the connection
    R_pstream_any_format,          // Unpack all serialized types
    read_byte_from_connection,     // Function to read single byte from buffer
    read_bytes_from_connection,    // Function for reading multiple bytes from buffer
    NULL,                          // Func for special handling of reference data.
    NULL                           // Data related to reference data handling
  );

  // Unserialize the input_stream into an R object
  return R_Unserialize(&input_stream);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Serialize an R object. Precalculate the resulting size so that
// the number of memory allocations can be minimised
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP marshall_con_(SEXP robj, SEXP con_) {
  
  // Create the output stream structure
  struct R_outpstream_st output_stream;
  
  // Initialise the output stream structure
  R_InitOutPStream(
    &output_stream,               // The stream object which wraps everything
    (R_pstream_data_t) con_,      // the user data is just a (void *) to the connection
    R_pstream_binary_format,      // Store as binary
    3,                            // Version = 3 for R >3.5.0 See `?base::serialize`
    write_byte_to_connection,     // Function to write single byte to buffer
    write_bytes_to_connection,    // Function for writing multiple bytes to buffer
    NULL,                         // Func for special handling of reference data.
    R_NilValue                    // Data related to reference data handling
  );
  
  // Serialize the object into the output_stream
  R_Serialize(robj, &output_stream);
  
  return R_NilValue;
}
