
#include <fs/initrd/initrd.h>
#include <mem/pmm.h>
#include <stdio.h>
#include <string.h>

uint8_t initrd_init(multiboot_info_t *mbi) {
  size_t count = mbi->mods_count;
  if (count == 0)
    return 0;
  printk("\n[initrd] Found %d module(s) ", count);
  if (count > 10) {
    vesa_term_use_color(NICE_YELLOW);
    printk("\n\t<!> [initrd] Can't load more than 10 modules ");
    printk("\n\t<!> [initrd] %d module(s) will be ignored ", count - 10);
    count = 10;
  }

  vfs_node_t *initrd = vfs_add_child(vfs_get_root(), "initrd", VFS_DIR);

  multiboot_module_t *module = (multiboot_module_t *)(mbi->mods_addr);
  char num[2] = "\0\0";
  uint8_t i = count;
  while (i-- > 0) {

    size_t mod_addr = module->mod_start;

    num[0] = '0' + (uint8_t)i;

    vfs_node_t *current_module = vfs_add_child(initrd, num, VFS_DIR);

    tar_parse(current_module, mod_addr);
    ++module;
  }
  return count;
}

uint32_t initrd_read(vfs_file_t *node, uint32_t offset, uint32_t size,
                     char *buffer) {
  if (offset >= node->size)
    return 0;
  if (offset + size > node->size)
    size = node->size - offset;
  memcpy(buffer, (uint8_t *)node->inode, size);
  return size;
}

uint32_t initrd_write(vfs_file_t *node, uint32_t offset, uint32_t size,
                      char *buffer) {}
void initrd_open(vfs_file_t *node, uint8_t read, uint8_t write) {}
void initrd_close(vfs_file_t *node) {}

dirent_t *initrd_readdir(vfs_file_t *node, uint32_t index) {}
vfs_file_t *initrd_finddir(vfs_file_t *node, char *name) {}

size_t tar_get_size(const char *input) {
  size_t size = 0;
  for (size_t j = 11, count = 1; j > 0; j--, count *= 8)
    size += ((input[j - 1] - '0') * count);

  return size;
}

size_t tar_parse(vfs_node_t *node, size_t addr) {
  uint32_t i = 0;
  while (1) {

    tar_header_t *header = (tar_header_t *)addr;
    if (!strlen(header->name))
      break;
    uint8_t *offset = (uint8_t *)addr;
    offset += 512;

    vfs_node_t *n =
        vfs_make_node(node, header->name + 2,
                      header->typeflag[0] == 53 ? VFS_DIR : VFS_FILE, offset);
    n->file->read = &initrd_read;

    size_t size = tar_get_size(header->size);

    n->file->size = size;
    header->offset = addr;

    addr += ((size / 512) + 1) * 512;
    if (size % 512)
      addr += 512;
    ++i;
  }
  return i;
}
