#ifndef _DEVFS_H_
#define _DEVFS_H_

#include <fs/vfs.h>

typedef struct {
  char *name;
  uint8_t type;
  open_fptr_t open;
  read_fptr_t read;
  write_fptr_t write;
  close_fptr_t close;
  uint32_t device_specific;
} devfs_node_t;

uint8_t devfs_init(char *path);
uint8_t devfs_register_dev(devfs_node_t *dev);

uint32_t devfs_vfs_read(vfs_file_t *node, uint32_t offset, uint32_t size,
                        char *buffer);
uint32_t devfs_vfs_write(vfs_file_t *node, uint32_t offset, uint32_t size,
                         char *buffer);
uint8_t devfs_vfs_open(vfs_file_t *node, uint8_t read, uint8_t write);
uint8_t devfs_vfs_close(vfs_file_t *node);
vfs_dirent_t *devfs_vfs_readdir(vfs_file_t *node, uint32_t index);
vfs_file_t *devfs_vfs_finddir(vfs_file_t *node, char *name);

#endif
