
#include <fs/initrd/initrd.h>
#include <mem/pmm.h>
#include <stdio.h>
#include <string.h>

void initrd_init(multiboot_module_t *mod) {
  vfs_node_t *initrd = vfs_add_child(vfs_get_root(), "initrd");
  tar_parse(initrd, mod->mod_start);
}

/*
uint32_t initrd_read(vfs_node_t *node, uint32_t offset, uint32_t size,
                     char *buffer) {}

uint32_t initrd_write(vfs_node_t *node, uint32_t offset, uint32_t size,
                      char *buffer) {}
void initrd_open(vfs_node_t *node, uint8_t read, uint8_t write) {}
void initrd_close(vfs_node_t *node) {}

dirent_t *initrd_readdir(vfs_node_t *node, uint32_t index) {}
vfs_node_t *initrd_finddir(vfs_node_t *node, char *name) {}
*/

size_t tar_get_size(const char *input) {
  size_t size = 0;
  for (size_t j = 11, count = 1; j > 0; j--, count *= 8)
    size += ((input[j - 1] - '0') * count);
  return size;
}

size_t tar_parse(vfs_node_t *node, size_t addr) {
  uint32_t i = 0;
  while(1) {
    tar_header_t *header = (tar_header_t *)addr;
    if (!strlen(header->name)) break;
    
    vfs_add_child(node, header->name);

    size_t size = tar_get_size(header->size);
    header->offset = addr;
    
    addr += ((size / 512) + 1) * 512;
    if (size % 512)
      addr += 512;
    ++i;
  }
  return i;
}
