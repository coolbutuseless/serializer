
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// The data buffer.
// Needs total length and pos to keep track of how much data it contains
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
typedef struct {
  R_xlen_t length;
  R_xlen_t pos;
  unsigned char *data;
} static_buffer_t;

static_buffer_t *init_static_buffer(int nbytes, unsigned char *data);
void write_byte_to_static_buffer(R_outpstream_t stream, int c);
void write_bytes_to_static_buffer(R_outpstream_t stream, void *src, int length);
int read_byte_from_static_buffer(R_inpstream_t stream);
void read_bytes_from_static_buffer(R_inpstream_t stream, void *dst, int length);
