#ifndef _FILE_DESCRIPTOR_H
#define _FILE_DESCRIPTOR_H

#include <fs/vfs.h>

/*
  Note : gft = global file table 
*/

typedef struct {
  vfs_node_t *node;
  uint32_t locked_by;
  /* 
    locked_by = pid of the process that locked the file 
                or 0 if the file is not locked.
                (PID 0 is guarenteed not to lock the file) 
  */
  uint32_t ref_count;
} gft_entry_t;

typedef struct {
  uint32_t gft_index;
  uint32_t offset;
  uint8_t read;
  uint8_t write;
} file_descriptor_t;

uint8_t gft_init();
uint32_t gft_add(vfs_node_t *node);
uint8_t gft_delete(uint32_t index);
uint8_t gft_lock(uint32_t index);
uint8_t gft_unlock(uint32_t index);


/* TODO: I might move those defines somewhere else */
#define O_RDONLY (1 << 0)
#define O_WRONLY (1 << 1)
#define O_RDWR   (1 << 2)

file_descriptor_t *fd_open(vfs_node_t *node, uint32_t flags);
uint8_t fd_close(file_descriptor_t *fd);
uint8_t fd_seek(file_descriptor_t *fd, uint32_t offset);
uint8_t fd_lock(file_descriptor_t *fd);
uint8_t fd_unlock(file_descriptor_t *fd);
uint32_t fd_locked_by(file_descriptor_t *fd);
vfs_node_t *fd_to_node(file_descriptor_t *fd);

#endif

