#ifndef _PIPE_H_
#define _PIPE_H_

#include <stddef.h>
#include <stdint.h>

#include <fs/vfs.h>
typedef struct {
  uint8_t *buffer;
  uint8_t read_index;
  uint8_t write_index;
  size_t size;
  size_t valid_data_size;
} ring_buffer_t;

ring_buffer_t *make_rb(size_t size);
size_t rb_read(ring_buffer_t *rb, uint8_t *buffer, size_t size);
size_t rb_write(ring_buffer_t *rb, uint8_t *buffer, size_t size);
vfs_node_t *rb_node(ring_buffer_t *rb);

int rb_open(ring_buffer_t *rb, uint32_t flags);
#endif
