#include <fs/ext2fs/ext2.h>
#include <fs/generic.h>

#include <mem/heap.h>
#include <stdio.h>
#include <string.h>
#include <kernel.h>

/* Returns the offset (bytes) of an inode, given its number */
uint64_t ext2_get_inode_offset(ext2_t *ext2_infos, uint32_t inode_number) {

  uint16_t inode_size = ext2_infos->superblock->inode_size;

  uint64_t group =
      (inode_number - 1) / ext2_infos->superblock->inodes_per_group;
  uint64_t index =
      (inode_number - 1) % ext2_infos->superblock->inodes_per_group;

  uint64_t offset = ext2_infos->gd[group].inode_table_starting_block_addr *
                    ext2_infos->block_size;
  offset += index * inode_size;

  return offset;
}

/* Initializes an ext2 partition, returns NULL if bad signature */
fs_t *ext2_init(ATA_drive_t *drv, uint8_t partition_number) {

  mbr_t mbr;
  mbr_parse(drv, &mbr);
  uint64_t offset = mbr.partitions[partition_number].LBA_start * 512;

  ext2_t *ext2_infos;
  ext2_infos = (ext2_t *)kmalloc(sizeof(ext2_t));
  ext2_infos->superblock =
      (ext2_superblock_t *)kmalloc(sizeof(ext2_superblock_t));

  ATA_read_b(drv, offset + 1024, sizeof(ext2_superblock_t),
             (uint8_t *)(ext2_infos->superblock));

  if (ext2_infos->superblock->ext2_signature != EXT2_SIGNATURE) {
    kfree(ext2_infos->superblock);
    kfree(ext2_infos);
    return NULL;
  }

  if (ext2_infos->superblock->fs_state == EXT2_FS_STATE_ERROR) {
    switch (ext2_infos->superblock->error_handling_method) {
    case EXT2_ERR_PANIC:
      panic("EXT2 filesystem not clean");
      break;
    /* TODO: handle readonly properly */
    case EXT2_ERR_IGNORE:
    case EXT2_ERR_READONLY:
      printk("[EXT2] Warning : Filesystem not clean");
    }
  }

  ext2_infos->block_size = 1024
                           << ext2_infos->superblock->log2_block_size_minus_10;

  uint32_t a = ext2_infos->superblock->blocks_count;
  a /= ext2_infos->superblock->blocks_per_group;
  a += (ext2_infos->superblock->blocks_count %
        ext2_infos->superblock->blocks_per_group)
           ? 1
           : 0;

  uint32_t b = ext2_infos->superblock->inodes_count;
  b /= ext2_infos->superblock->inodes_per_group;
  b += (ext2_infos->superblock->inodes_count %
        ext2_infos->superblock->inodes_per_group)
           ? 1
           : 0;

  ext2_infos->groups_count = (a > b) ? a : b;

  ext2_infos->gd = (ext2_block_group_dt_t *)kmalloc(
      ext2_infos->groups_count * sizeof(ext2_block_group_dt_t));

  uint64_t gd_offset =
      (ext2_infos->block_size == 1024) ? 2048 : ext2_infos->block_size;

  ATA_read_b(drv, offset + gd_offset,
             ext2_infos->groups_count * sizeof(ext2_block_group_dt_t),
             (uint8_t *)(ext2_infos->gd));

  fs_t *result = (fs_t *)kmalloc(sizeof(fs_t));
  result->drv = drv;
  result->p_offset = mbr.partitions[partition_number].LBA_start * 512;
  result->fs_specific = (void *)ext2_infos;

  result->opendir = &ext2_opendir;
  result->readdir = &ext2_readdir;
  result->closedir = &ext2_closedir;

  result->partition_number = partition_number;
  result->type = FS_EXT2;

  return result;
}

static inline ext2_inode_t *ext2_kmalloc_inode(ext2_t *ext2_infos) {
  return (ext2_inode_t *)kmalloc(ext2_infos->superblock->inode_size);
}

static inline void ext2_kfree_inode(ext2_inode_t *inode) { kfree(inode); }

void ext2_get_inode(fs_t *fs, uint32_t inode_number, ext2_inode_t *inode) {
  ext2_t *ext2_infos = (ext2_t *)(fs->fs_specific);
  uint64_t offset = ext2_get_inode_offset(ext2_infos, inode_number);
  ATA_read_b(fs->drv, fs->p_offset + offset, ext2_infos->superblock->inode_size,
             (uint8_t *)inode);
}

#include <fs/dirent.h>
DIR *ext2_opendir(fs_t *fs, uint32_t inode_number) {
  ext2_t *ext2_infos = (ext2_t *)(fs->fs_specific);

  ext2_inode_t *inode = ext2_kmalloc_inode(ext2_infos);
  ext2_get_inode(fs, inode_number, inode);

  if ((inode->type_permissions & EXT2_INODE_TYPE_DIR) == 0) {
    ext2_kfree_inode(inode);
    return NULL;
  }

  DIR *dir = (DIR *)kmalloc(sizeof(DIR));
  memset((char *)dir, 0, sizeof(dir));

  dir->fs = fs;
  dir->inode_number = inode_number;
  dir->block_buffer = (uint8_t *)kmalloc(ext2_infos->block_size);

  ATA_drive_t *drv = fs->drv;
  uint64_t offset = fs->p_offset;
  offset += inode->direct_block_pointer[0] * ext2_infos->block_size;
  uint64_t size = ext2_infos->block_size;
  uint8_t *buffer = dir->block_buffer;

  ATA_read_b(drv, offset, size, buffer);

  dir->next = (uint32_t)(dir->block_buffer);
  dir->so_far = 0;
  ext2_kfree_inode(inode);
  return dir;
}

struct dirent *ext2_readdir(DIR *dir) {
  fs_t *fs = dir->fs;
  ext2_t *ext2_infos = (ext2_t *)(fs->fs_specific);

  if (dir->so_far < ext2_infos->block_size) {
    ext2_directory_entry_t *e = (ext2_directory_entry_t *)dir->next;
    if (e->inode_number == 0)
      return 0;

    dir->next = (uint32_t)((uint8_t *)e + e->entry_size);
    dir->so_far += e->entry_size;

    memcpy(dir->ent.name, e->name, e->name_length_lo);
    dir->ent.name[e->name_length_lo] = '\0';

    dir->ent.inode_number = e->inode_number;
    dir->ent.type = e->type_indicator__name_length_hi;

    ext2_inode_t *inode = ext2_kmalloc_inode(ext2_infos);
    ;
    ext2_get_inode(fs, e->inode_number, inode);

    dir->ent.permissions = inode->type_permissions;
    ext2_kfree_inode(inode);
    return &dir->ent;
  }

  return 0;
}

void ext2_closedir(DIR *dir) {
  if (!dir)
    return;
  kfree(dir->block_buffer);
  kfree(dir);
}

#include <fs/vfs.h>
uint8_t ext2_type_translate(uint8_t type) {
  switch (type) {
  case EXT2_DIR_ENTRY_TYPE_DIR:
    return VFS_DIR;
  case EXT2_DIR_ENTRY_TYPE_CHAR_DEV:
    return VFS_CDEV;
  case EXT2_DIR_ENTRY_TYPE_BLOCK_DEV:
    return VFS_BDEV;
  case EXT2_DIR_ENTRY_TYPE_FIFO:
    return VFS_PIPE;
  case EXT2_DIR_ENTRY_TYPE_SOCKET:
    return VFS_SOCKET;
  case EXT2_DIR_ENTRY_TYPE_SYMLINK:
    return VFS_SYMLNK;
  case EXT2_DIR_ENTRY_TYPE_UNKNOWN:
  case EXT2_DIR_ENTRY_TYPE_FILE:
  default:
    return VFS_FILE;
  }
}