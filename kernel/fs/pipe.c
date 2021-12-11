#include <fs/pipe.h>
#include <mem/heap.h>
#include <fs/vfs.h>
#include <fs/file_descriptor.h>
#include <multitasking/scheduler.h>
#include <util/logger.h>

ring_buffer_t *make_rb(size_t size){
  ring_buffer_t *rb;
  rb = (ring_buffer_t *) kmalloc(sizeof(ring_buffer_t));
  
  rb->size = size;
  rb->buffer = (uint8_t *) kmalloc(rb->size);
  rb->read_index = 0;
  rb->write_index = 0;
  
  return rb;
}

static uint8_t rb_read_byte(ring_buffer_t *rb){
  while(rb->valid_data_size == 0) {
    asm volatile("hlt");
  }
  uint8_t byte = rb->buffer[rb->read_index];
  
  ++(rb->read_index);
  --(rb->valid_data_size);
  rb->read_index %= rb->size;
  
  return byte;
}

static void rb_write_byte(ring_buffer_t *rb, uint8_t byte){
  rb->buffer[rb->write_index] = byte;
  
  ++(rb->write_index);
  ++(rb->valid_data_size);
    
  rb->write_index %= rb->size;
   
  if(rb->valid_data_size > rb->size)
    rb->valid_data_size = rb->size;
  
}

size_t rb_read(ring_buffer_t *rb, uint8_t *buffer, size_t size){
  if(size == 0) return 0;
  
  //if(size > rb->valid_data_size) size = rb->valid_data_size;
  
  for(uint32_t i = 0; i < size; i++){
    buffer[i] = rb_read_byte(rb);
  }
  return size;
}

size_t rb_write(ring_buffer_t *rb, uint8_t *buffer, size_t size){
  if(size == 0) return 0;
  
  for(uint32_t i = 0; i < size; i++){
    rb_write_byte(rb, buffer[i]);
  }
  
  return size;
}

uint32_t rb_vfs_read(vfs_file_t *file, uint32_t offset, uint32_t size,
                   char *buffer){
  (void) offset;
  return rb_read((ring_buffer_t *)(file->inode), (uint8_t *) buffer, size);
  
}

uint32_t rb_vfs_write(vfs_file_t *file, uint32_t offset, uint32_t size,
                   char *buffer){
  (void) offset;
  return rb_write((ring_buffer_t *)(file->inode), (uint8_t *) buffer, size);
}

vfs_node_t *rb_node(ring_buffer_t *rb){
  vfs_node_t *node;
  node = vfs_create_node("", VFS_PIPE);
  node->file->read = &rb_vfs_read;
  node->file->write = &rb_vfs_write;
  node->file->inode = (uint32_t) rb;
  return node;
}

int rb_open(ring_buffer_t *rb, uint32_t flags){
  uint32_t ef;
  ef = get_eflags_and_cli();
  vfs_node_t *node = rb_node(rb);
  
  file_descriptor_t *fd = fd_open(node, flags);
  
  int i = list_push(mt_get_current_task()->file_descriptors, fd);
  set_eflags(ef);
  return i;
}
