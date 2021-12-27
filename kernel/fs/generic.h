#ifndef _FS_GENERIC_H_
#define _FS_GENERIC_H_

typedef enum { FS_EXT2 } fs_name_t;

#include <fs/dirent.h>

struct fs;
typedef struct fs fs_t;

typedef DIR *(*fs_opendir)(fs_t *fs, uint32_t inode_number);
typedef void (*fs_closedir)(DIR *dir);
typedef struct dirent *(*fs_readdir)(DIR *dir);

#include <drivers/ata.h>
#include <misc/mbr.h>

struct fs {

  fs_name_t *type;

  /* Physical location */
  /*
    TODO: It might be on a non ATA support, so..
    it is a good idea to generalize this
  */
  ATA_drive_t *drv;
  uint8_t partition_number;
  uint64_t p_offset;

  /* Depends of the filesystem type */
  void *fs_specific;

  fs_opendir opendir;
  fs_closedir closedir;
  fs_readdir readdir;
};

#endif
