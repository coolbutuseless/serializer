

dynamic_buffer_t *init_static_buffer(int nbytes, unsigned char *data);
void write_byte_to_static_buffer(R_outpstream_t stream, int c);
void write_bytes_to_static_buffer(R_outpstream_t stream, void *src, int length);
