#include <fs/file_ops.h>
#include <fs/file_descriptor.h>
#include <multitasking/schedlock.h>
#include <multitasking/scheduler.h>


int open(char *path, uint32_t flags, uint32_t mode){
  uint32_t ef;
  ef = get_eflags_and_cli();
  
  /* TODO: add support for O_CREAT */
  (void) mode;
  
  vfs_node_t *node = vfs_node_from_path(vfs_get_root(), path);
  
  file_descriptor_t *fd = fd_open(node, flags);
  
  int i = list_push(mt_get_current_task()->file_descriptors, fd);
  
  set_eflags(ef);
  return i;
}

int close(int fd){
  uint32_t ef;
  ef = get_eflags_and_cli();
  
  list_t *file_descriptors = mt_get_current_task()->file_descriptors;
  
  file_descriptor_t *d = list_get(file_descriptors, fd);
  if(!d) return -1;
  
  fd_close(d);
  list_delete(file_descriptors, fd);
  
  set_eflags(ef);
  return 0;
}


int read(int fd, void *buffer, size_t size){
  uint32_t ef;
  /* 
    Disabling interrupts and waiting for data from keyboard 
    makes no sense (specifically if the ring buffer is empty)
  */
  if(fd)
    ef = get_eflags_and_cli();
  
  /*
    Note : by using if(fd) I am assuming that stdin is always
    file descriptor 0, what if the program actually reopened it
    with another file descriptor number ? Anyway
  */
  
  size_t r = 0;
  
  list_t *file_descriptors = mt_get_current_task()->file_descriptors;
  file_descriptor_t *d = list_get(file_descriptors, fd);
  
  if(!d) goto fail;
  if(!d->read) goto fail;
  
  vfs_node_t *node = fd_to_node(d);
  r = vfs_read(node->file, d->offset, size, buffer);

fail:
  if(fd)
    set_eflags(ef);
  return r;
}

int write(int fd, const void *buffer, size_t size){
  uint32_t ef;
  ef = get_eflags_and_cli();
  size_t w = 0;
  
  list_t *file_descriptors = mt_get_current_task()->file_descriptors;
  file_descriptor_t *d = list_get(file_descriptors, fd);
  
  if(!d) goto fail;
  if(!d->write) goto fail;
  
  vfs_node_t *node = fd_to_node(d);
  w = vfs_write(node->file, d->offset, size, buffer);
  
fail:
  set_eflags(ef);
  return w;
}


