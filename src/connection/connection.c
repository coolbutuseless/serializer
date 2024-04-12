#include "connection.h"

size_t read_connection(SEXP connection, void* buf, size_t n) {
  return R_ReadConnection(R_GetConnection(connection), buf, n);
}


size_t write_connection(SEXP connection, void* buf, size_t n) {
  return R_WriteConnection(R_GetConnection(connection), buf, n);
}

