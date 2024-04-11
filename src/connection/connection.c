#include "connection.h"

size_t read_connection(Rconnection con, void* buf, size_t n) {
  return R_ReadConnection(con, buf, n);
}

size_t write_connection(Rconnection con, void* buf, size_t n) {
  return R_WriteConnection(con, buf, n);
}

