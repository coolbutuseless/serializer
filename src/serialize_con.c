


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
  SEXP con_ = (SEXP)stream->data;
  
  // Modelled after jsonlite/src/push_parser.c  
  SEXP call = PROTECT(
    Rf_lang4(
      PROTECT(Rf_install("readBin")),
      con_,                                 // con
      PROTECT(Rf_allocVector(RAWSXP, 0)),   // what = raw()
      PROTECT(Rf_ScalarInteger(length))     // n
    ));
  
  SEXP out = PROTECT(Rf_eval(call, R_BaseEnv));
  int len = Rf_length(out);
  if(len <= 0) {
    // Connection did not return any data!
    error("read_bytes_from_connection() returned %i bytes read", len);
  }
  
  if (len != length) {
    // Why did the connection not give the number of bytes I asked for?
    error("read_bytes_from_connection(). Expected %i bytes to read. Got: %i", length, len);
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
  SEXP con_ = (SEXP)stream->data;
  
  SEXP raw_vec_ = PROTECT(allocVector(RAWSXP, length));
  
  memcpy(RAW(raw_vec_), src, length);
  
  // Modelled after jsonlite/src/push_parser.c  
  SEXP call = PROTECT(
    Rf_lang3(
      PROTECT(Rf_install("writeBin")), 
      raw_vec_, // OBJECT
      con_     // con
    )
  );
  // SEXP out = PROTECT(Rf_eval(call, R_BaseEnv));
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
    (R_pstream_data_t) con_,        // Actual data buffer
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
    (R_pstream_data_t) con_,       // The actual data
    R_pstream_binary_format,      // Store as binary
    3,                            // Version = 3 for R >3.5.0 See `?base::serialize`
    write_byte_to_connection,    // Function to write single byte to buffer
    write_bytes_to_connection,   // Function for writing multiple bytes to buffer
    NULL,                         // Func for special handling of reference data.
    R_NilValue                    // Data related to reference data handling
  );
  
  // Serialize the object into the output_stream
  R_Serialize(robj, &output_stream);
  
  return R_NilValue;
}
