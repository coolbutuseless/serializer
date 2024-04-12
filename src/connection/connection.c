#include "connection.h"

size_t read_connection(SEXP connection, void* buf, size_t n) {
  return R_ReadConnection(R_GetConnection(connection), buf, n);
}

