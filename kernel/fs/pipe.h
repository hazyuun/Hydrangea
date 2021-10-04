#ifndef _PIPE_H_
#define _PIPE_H_

#include <stdint.h>
#include <stddef.h>

typedef struct {
    uint8_t *buffer;
    uint8_t  read_index;
    uint8_t  write_index;
    size_t   size;
    size_t   valid_data_size;
} ring_buffer_t;


ring_buffer_t *make_rb(size_t size);
size_t rb_read(ring_buffer_t *rb, uint8_t *buffer, size_t size);
size_t rb_write(ring_buffer_t *rb, uint8_t *buffer, size_t size);

#endif
