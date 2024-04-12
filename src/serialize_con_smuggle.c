


#include <R.h>
#include <Rinternals.h>
#include <Rdefines.h>
#include <R_ext/Connections.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "calc-serialized-size.h"
#include "connection/connection.h"

SEXP read_connection_xptr;
SEXP write_connection_xptr;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Smuggle in the pointers to the 'read_connection()' and 'write_connection()'
// functions which are compiled into 'inst/lib/libconnections.so' from
// 'src/connection/connection.*'
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP init_smuggle_(SEXP rc_xptr, SEXP wc_xptr) {
  read_connection_xptr = rc_xptr;
  R_PreserveObject(rc_xptr);
  
  write_connection_xptr = wc_xptr;
  R_PreserveObject(wc_xptr);
  
  return R_NilValue;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Recreate a local copy of read_connection() that internally
//   1. reconstructs a 'read_connection()' function from the pointer 
//      'read_connection_xptr' which is the address from the dynamically loaded
//      inst/lib/libconnections.so
//   2. Call this function
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
size_t read_connection(SEXP connection, void* buf, size_t n) {
  size_t (*read_connection)(SEXP, void*, size_t);
  read_connection = (size_t (*)(SEXP, void *, size_t))R_ExternalPtrAddr(read_connection_xptr);
  return read_connection(connection, buf, n);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Create a function from the external pointer we got when loading the
// library 'inst/lib/libconnections.so'
// I.e.
//    This function below
//    Calls the function in 'inst/lib/libconnections.so'
//    Which calls the non-API R function 'R_WriteConnection()'
//
// But because the call to this non-API function is wrapped in a library
// outside of the main package '.so' file, 'R CMD check' doesn't see it,
// and won't give a "NOTE" during the check.
//
// I.e. the 'smuggle' method would work on CRAN
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
size_t write_connection(SEXP connection, void* buf, size_t n) {
  size_t (*write_connection)(SEXP, void*, size_t);
  write_connection = (size_t (*)(SEXP, void *, size_t))R_ExternalPtrAddr(write_connection_xptr);
  return write_connection(connection, buf, n);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Read a byte from the serialized stream
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int read_byte_from_connection_smuggle(R_inpstream_t stream) {
  error("read_byte_from_connection_smuggle() never used for binary serializing");
  return 0;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Read multiple bytes from the connection
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void read_bytes_from_connection_smuggle(R_inpstream_t stream, void *dst, int length) {
  size_t nread = read_connection(stream->data, dst, length);

  // Sanity check that we read the requested number of bytes from the connection
  if (nread != length) {
    error("read_bytes_from_connection_smuggle(). Expected %i bytes to be read, but actually read %ld", length, nread);
  }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Read a byte from the serialized stream
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void write_byte_to_connection_smuggle(R_outpstream_t stream, int c) {
  error("write_byte_to_connection() never used for binary serializing");
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Read multiple bytes from the connection
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void write_bytes_to_connection_smuggle(R_outpstream_t stream, void *src, int length) {
  write_connection(stream->data, src, length);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Unpack a raw vector to an R object
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP unmarshall_con_smuggle_(SEXP con_) {
  
  // Treat the data buffer as an input stream
  struct R_inpstream_st input_stream;

  R_InitInPStream(
    &input_stream,                 // Stream object wrapping data buffer
    (R_pstream_data_t) con_,       // the user data is just a (void *) to the connection
    R_pstream_any_format,          // Unpack all serialized types
    read_byte_from_connection_smuggle,     // Function to read single byte from buffer
    read_bytes_from_connection_smuggle,    // Function for reading multiple bytes from buffer
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
SEXP marshall_con_smuggle_(SEXP robj, SEXP con_) {
  
  // Create the output stream structure
  struct R_outpstream_st output_stream;
  
  // Initialise the output stream structure
  R_InitOutPStream(
    &output_stream,               // The stream object which wraps everything
    (R_pstream_data_t) con_,      // the user data is just a (void *) to the connection
    R_pstream_binary_format,      // Store as binary
    3,                            // Version = 3 for R >3.5.0 See `?base::serialize`
    write_byte_to_connection_smuggle,     // Function to write single byte to buffer
    write_bytes_to_connection_smuggle,    // Function for writing multiple bytes to buffer
    NULL,                         // Func for special handling of reference data.
    R_NilValue                    // Data related to reference data handling
  );
  
  // Serialize the object into the output_stream
  R_Serialize(robj, &output_stream);
  
  return R_NilValue;
}
