#include <fs/pipe.h>
#include <mem/heap.h>


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
  if(rb->valid_data_size == 0) return 0;
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
  
  if(size > rb->valid_data_size) size = rb->valid_data_size;
  
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


