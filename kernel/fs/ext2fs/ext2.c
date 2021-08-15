#include <fs/ext2fs/ext2.h>
#include <fs/generic.h>

#include <kernel.h>
#include <mem/heap.h>
#include <util/logger.h>
#include <stdio.h>
#include <string.h>

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
  log_info(NICE_CYAN_0, "EXT2", "EXT2 filesystem detected");
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
      log_f(ERROR, "EXT2", "Filesystem not clean");
      panic("EXT2 filesystem not clean");
      break;
    /* TODO: handle readonly properly */
    case EXT2_ERR_READONLY:
    case EXT2_ERR_IGNORE:
      log_f(WARN, "EXT2", "Filesystem not clean");
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

static void ext2_make_dirent(DIR *dir, ext2_directory_entry_t *e) {
  fs_t *fs = dir->fs;
  ext2_t *ext2_infos = (ext2_t *)(fs->fs_specific);

  dir->next = (uint32_t)((uint8_t *)e + e->entry_size);
  dir->so_far += e->entry_size;

  memcpy(dir->ent.name, e->name, e->name_length_lo);
  dir->ent.name[e->name_length_lo] = '\0';

  dir->ent.inode_number = e->inode_number;
  dir->ent.type = e->type_indicator__name_length_hi;

  ext2_inode_t *inode = ext2_kmalloc_inode(ext2_infos);

  ext2_get_inode(fs, e->inode_number, inode);
  dir->ent.size = inode->size;
  dir->ent.permissions = inode->type_permissions;
  ext2_kfree_inode(inode);
}

/* Returns the offset to the data block pointed to by a singly indirect block
 * pointer */
static uint64_t ext2_get_sbptr_offset(fs_t *fs, uint32_t inode_number,
                                      uint32_t index) {
  ext2_t *ext2_infos = (ext2_t *)(fs->fs_specific);

  ext2_inode_t *inode = ext2_kmalloc_inode(ext2_infos);
  ext2_get_inode(fs, inode_number, inode);

  uint64_t offset = fs->p_offset;
  offset +=
      inode->singly_indirect_block_pointer * ext2_infos->block_size + index;

  uint32_t block_ptr;
  ATA_read_b(fs->drv, offset, sizeof(uint32_t), (uint8_t *)&block_ptr);

  offset = fs->p_offset;
  offset += block_ptr * ext2_infos->block_size;

  ext2_kfree_inode(inode);
  return offset;
}

/* Returns the offset to the data block pointed to by a doubly indirect block
 * pointer */
static uint64_t ext2_get_dbptr_offset(fs_t *fs, uint32_t inode_number,
                                      uint32_t index_1, uint32_t index_2) {
  ext2_t *ext2_infos = (ext2_t *)(fs->fs_specific);

  /* First, get the first pointer */
  uint32_t block_ptr_1;
  uint64_t offset = ext2_get_sbptr_offset(fs, inode_number, index_1);
  ATA_read_b(fs->drv, offset, sizeof(uint32_t), (uint8_t *)&block_ptr_1);

  /* The first pointer points to the second pointer */
  uint32_t block_ptr_2;
  offset = block_ptr_1 * ext2_infos->block_size + index_2;
  ATA_read_b(fs->drv, offset, sizeof(uint32_t), (uint8_t *)&block_ptr_2);

  offset = fs->p_offset;
  offset += block_ptr_2 * ext2_infos->block_size;
  return offset;
}

/* Returns the offset to the data block pointed to by a triply indirect block
 * pointer */
static uint64_t ext2_get_tbptr_offset(fs_t *fs, uint32_t inode_number,
                                      uint32_t index_1, uint32_t index_2,
                                      uint32_t index_3) {
  ext2_t *ext2_infos = (ext2_t *)(fs->fs_specific);

  /* First, get the second pointer */
  uint32_t block_ptr_1;
  uint64_t offset = ext2_get_dbptr_offset(fs, inode_number, index_1, index_2);
  ATA_read_b(fs->drv, offset, sizeof(uint32_t), (uint8_t *)&block_ptr_1);

  /* The second pointer points to the third pointer */
  uint32_t block_ptr_2;
  offset = block_ptr_1 * ext2_infos->block_size + index_3;
  ATA_read_b(fs->drv, offset, sizeof(uint32_t), (uint8_t *)&block_ptr_2);

  offset = fs->p_offset;
  offset += block_ptr_2 * ext2_infos->block_size;
  return offset;
}

/* TODO : Test this function with large files */
static uint64_t ext2_get_block_offset(fs_t *fs, uint32_t inode_number,
                                      uint64_t block_nbr) {
  ext2_t *ext2_infos = (ext2_t *)(fs->fs_specific);

  uint32_t ppb = ext2_infos->block_size / sizeof(uint32_t);
  uint32_t offset = 0;

  ext2_inode_t *inode = ext2_kmalloc_inode(ext2_infos);
  ext2_get_inode(fs, inode_number, inode);

  /* Direct Block Pointer */
  if (block_nbr < 12) {
    offset = inode->direct_block_pointer[block_nbr] * ext2_infos->block_size;
  }
  /* Singly Indirect Block Pointer */
  else if (block_nbr < 12 + ppb) {
    uint32_t index = (block_nbr - 12) * sizeof(uint32_t);
    offset = ext2_get_sbptr_offset(fs, inode_number, index);
  }
  /* Doubly Indirect Block Pointer */
  else if (block_nbr < 12 + ppb + ppb * ppb) {
    uint32_t index = block_nbr - 12 - ppb;
    uint32_t index_1 = index / ppb;
    uint32_t index_2 = index % ppb;
    offset = ext2_get_dbptr_offset(fs, inode_number, index_1, index_2);
  }
  /* Triply Indirect Block Pointer */
  else if (block_nbr < 12 + ppb + ppb * ppb + ppb * ppb * ppb) {
    uint32_t index = block_nbr - 12 - ppb - ppb * ppb;

    uint32_t index_1 = index / (ppb * ppb);
    uint32_t tmp = index % (ppb * ppb);
    uint32_t index_2 = tmp / ppb;
    uint32_t index_3 = tmp % ppb;
    /* Pretty sure this shit above is wrong somewhere */

    offset = ext2_get_tbptr_offset(fs, inode_number, index_1, index_2, index_3);
  }

  ext2_kfree_inode(inode);
  return offset;
}

struct dirent *ext2_readdir(DIR *dir) {
  fs_t *fs = dir->fs;
  ext2_t *ext2_infos = (ext2_t *)(fs->fs_specific);

  if (dir->so_far < ext2_infos->block_size) {
    ext2_directory_entry_t *e = (ext2_directory_entry_t *)dir->next;
    if (e->inode_number == 0)
      return 0;

    ext2_make_dirent(dir, e);
    return &dir->ent;
  }
  /* Read the next block */
  ++(dir->bp_index);

  uint64_t b_offset =
      ext2_get_block_offset(fs, dir->inode_number, dir->bp_index);
  if (!b_offset)
    return 0;

  uint64_t size = ext2_infos->block_size;
  uint8_t *buffer = dir->block_buffer;
  ATA_read_b(dir->fs->drv, fs->p_offset + b_offset, size, buffer);

  dir->next = (uint32_t)(dir->block_buffer);
  dir->so_far = 0;

  return ext2_readdir(dir);
}

void ext2_closedir(DIR *dir) {
  if (!dir)
    return;
  kfree(dir->block_buffer);
  kfree(dir);
}

uint32_t ext2_readfile(fs_t *fs, uint32_t inode_number, uint64_t offset,
                       uint64_t size, uint8_t *buffer) {
  if (!size)
    return 0;
  ext2_t *ext2_infos = (ext2_t *)(fs->fs_specific);

  ext2_inode_t *inode = ext2_kmalloc_inode(ext2_infos);
  ext2_get_inode(fs, inode_number, inode);

  if ((inode->type_permissions & EXT2_INODE_TYPE_FILE) == 0) {
    ext2_kfree_inode(inode);
    return 0;
  }

  if (!(inode->size) || (size > inode->size)) {
    ext2_kfree_inode(inode);
    return 0;
  }

  uint64_t end = offset + size;

  if (end > inode->size)
    end = inode->size;

  uint64_t block_start = offset / ext2_infos->block_size;
  uint64_t block_end = end / ext2_infos->block_size;

  uint32_t start_offset = offset % ext2_infos->block_size;
  uint32_t end_offset = end % ext2_infos->block_size;

  uint64_t b_offset;

  if (block_start == block_end) {
    b_offset = ext2_get_block_offset(fs, inode_number, block_start);
    ATA_read_b(fs->drv, fs->p_offset + b_offset + start_offset, size, buffer);
    return size;
  }

  for (uint64_t blk = block_start; blk < block_end; blk++) {
    b_offset = ext2_get_block_offset(fs, inode_number, blk);
    if (!b_offset)
      return 0;

    uint64_t b_size = ext2_infos->block_size;

    if (blk == block_start) {
      ATA_read_b(fs->drv, fs->p_offset + b_offset + start_offset,
                 b_size - start_offset, buffer);

    } else if (blk == block_end) {
      ATA_read_b(fs->drv, fs->p_offset + b_offset + end_offset, end_offset,
                 buffer);
    }
  }
  buffer[inode->size] = 0;
  ext2_kfree_inode(inode);
  return size;
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

uint32_t ext2_vfs_read(vfs_file_t *node, uint32_t offset, uint32_t size,
                       char *buffer) {
  return ext2_readfile(node->fs, node->inode, offset, size, (uint8_t *)buffer);
}
/*
uint32_t ext2_vfs_write(vfs_file_t *node, uint32_t offset, uint32_t size,
                   char *buffer){

                   }
uint8_t ext2_vfs_open(vfs_file_t *node, uint8_t read, uint8_t write){

}
uint8_t ext2_vfs_close(vfs_file_t *node){

}
vfs_dirent_t *ext2_vfs_readdir(vfs_file_t *node, uint32_t index){

}
vfs_file_t *ext2_vfs_finddir(vfs_file_t *node, char *name){

}
*/
