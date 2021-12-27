#ifndef _DIRENT_H_
#define _DIRENT_H_

struct fs;
typedef struct fs fs_t;

struct dirent {
  uint32_t inode_number;
  uint64_t size;
  uint8_t type;
  uint16_t permissions;
  char name[256];
};

typedef struct {
  fs_t *fs;

  uint8_t *block_buffer;

  uint32_t inode_number;

  uint32_t bp_index;
  uint32_t so_far;
  uint32_t next;
  struct dirent ent;
} DIR;

#endif