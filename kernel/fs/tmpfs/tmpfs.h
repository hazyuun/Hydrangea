#ifndef _TMPFS_H_
#define _TMPFS_H_

#include <fs/vfs.h>



uint8_t tmpfs_init(char *path);

uint32_t tmpfs_vfs_read(vfs_file_t *node, uint32_t offset, uint32_t size,
                       char *buffer);
uint32_t tmpfs_vfs_write(vfs_file_t *node, uint32_t offset, uint32_t size,
                        char *buffer);
uint8_t tmpfs_vfs_open(vfs_file_t *node, uint8_t read, uint8_t write);
uint8_t tmpfs_vfs_close(vfs_file_t *node);
vfs_dirent_t *tmpfs_vfs_readdir(vfs_file_t *node, uint32_t index);
vfs_file_t *tmpfs_vfs_finddir(vfs_file_t *node, char *name);

#endif

