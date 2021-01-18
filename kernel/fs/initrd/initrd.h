#ifndef _INITRD_H_
#define _INITRD_H_

#include <boot/multiboot.h>
#include <fs/vfs.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
  const char name[100];
  char mode[8];
  char uid[8];
  char gid[8];
  char size[12];
  char mtime[12];
  char chksum[8];
  char typeflag[1];
  size_t offset;
} tar_header_t;

uint8_t initrd_init(multiboot_info_t *mbi); /* Haha what a name, lol */

uint32_t initrd_read(vfs_file_t *node, uint32_t offset, uint32_t size,
                     char *buffer);
dirent_t *initrd_readdir(vfs_file_t *node, uint32_t index);
vfs_file_t *initrd_finddir(vfs_file_t *node, char *name);

size_t tar_get_size(const char *input);
size_t tar_parse(vfs_node_t *node, size_t addr);

#endif