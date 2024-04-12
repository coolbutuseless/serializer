


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
int read_byte_from_file(R_inpstream_t stream) {
  error("read_byte_from_connection() never used for binary serializing");
  return 0;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Read multiple bytes from the connection
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void read_bytes_from_file(R_inpstream_t stream, void *dst, int length) {
  
  // Get the connection from the user data
  FILE *fp = (FILE *)stream->data;
  
  unsigned long nread = fread(dst, 1, length, fp);
  
  // Sanity check that we read the requested number of bytes from the connection
  if (nread != length) {
    error("read_bytes_from_file(). Expected %i bytes to be read, but actually read %ld", length, nread);
  }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Read a byte from the serialized stream
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void write_byte_to_file(R_outpstream_t stream, int c) {
  error("write_byte_to_connection() never used for binary serializing");
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Read multiple bytes from the connection
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void write_bytes_to_file(R_outpstream_t stream, void *src, int length) {
  
  FILE *fp = (FILE *)stream->data;
  fwrite(src, 1, length, fp);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Unpack a raw vector to an R object
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP unmarshall_file_(SEXP filename_) {

  FILE *fp = fopen(CHAR(STRING_ELT(filename_, 0)), "rb");
  if (fp == NULL) {
    error("unmarshall_file(): Error opening '%s'", CHAR(STRING_ELT(filename_, 0)));
  }
  
  // Treat the data buffer as an input stream
  struct R_inpstream_st input_stream;

  R_InitInPStream(
    &input_stream,                 // Stream object wrapping data buffer
    (R_pstream_data_t) fp,         // the user data is just a (void *) to the connection
    R_pstream_any_format,          // Unpack all serialized types
    read_byte_from_file,           // Function to read single byte from buffer
    read_bytes_from_file,          // Function for reading multiple bytes from buffer
    NULL,                          // Func for special handling of reference data.
    NULL                           // Data related to reference data handling
  );

  // Unserialize the input_stream into an R object
  SEXP res_ = PROTECT(R_Unserialize(&input_stream));
  fclose(fp);
  UNPROTECT(1);
  return res_;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Serialize an R object. Precalculate the resulting size so that
// the number of memory allocations can be minimised
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP marshall_file_(SEXP robj, SEXP filename_) {
  
  FILE *fp = fopen(CHAR(STRING_ELT(filename_, 0)), "wb");
  if (fp == NULL) {
    error("unmarshall_file(): Error opening '%s'", CHAR(STRING_ELT(filename_, 0)));
  }
  
  // Create the output stream structure
  struct R_outpstream_st output_stream;
  
  // Initialise the output stream structure
  R_InitOutPStream(
    &output_stream,               // The stream object which wraps everything
    (R_pstream_data_t) fp,        // the user data is just a (void *) to the connection
    R_pstream_binary_format,      // Store as binary
    3,                            // Version = 3 for R >3.5.0 See `?base::serialize`
    write_byte_to_file,           // Function to write single byte to buffer
    write_bytes_to_file,          // Function for writing multiple bytes to buffer
    NULL,                         // Func for special handling of reference data.
    R_NilValue                    // Data related to reference data handling
  );
  
  // Serialize the object into the output_stream
  R_Serialize(robj, &output_stream);
  
  fclose(fp);
  return R_NilValue;
}
