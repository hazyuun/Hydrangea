#ifndef _EXT2_H_
#define _EXT2_H_

#include <stdint.h>

#define EXT2_SIGNATURE 0xEF53

#define EXT2_FS_STATE_CLEAN 1
#define EXT2_FS_STATE_ERROR 2

#define EXT2_ERR_IGNORE 1 /* Kalm */
#define EXT2_ERR_READONLY 2
#define EXT2_ERR_PANIC 3 /* Panik */

#define EXT2_CREATOR_OS_LINUX 0
#define EXT2_CREATOR_OS_GNU_HURD 1
#define EXT2_CREATOR_OS_MASIX 2
#define EXT2_CREATOR_OS_FREEBSD 3
#define EXT2_CREATOR_OS_OTHER 4

#define EXT2_REQ_FEAT_COMPRESSION (1 << 0)
#define EXT2_REQ_FEAT_DIR_TYPE (1 << 1)
#define EXT2_REQ_FEAT_REPLAY_JOURNAL (1 << 2)
#define EXT2_REQ_FEAT_JOURNAL_DEVICE (1 << 3)

#define EXT2_OPT_FEAT_PREALLOCATE (1 << 0)
#define EXT2_OPT_FEAT_AFS_SERVER_INODES (1 << 1)
#define EXT2_OPT_FEAT_JOURNAL_EXISTS (1 << 2)
#define EXT2_OPT_FEAT_EXT_INODE_ATTR (1 << 3)
#define EXT2_OPT_FEAT_RESIZABLE (1 << 4)
#define EXT2_OPT_FEAT_HASH_INDEX (1 << 5)

#define EXT2_RO_FEAT_SPARSE (1 << 0)
#define EXT2_RO_FEAT_64BITS (1 << 1)
#define EXT2_RO_FEAT_BIN_TREE (1 << 2)

#define EXT2_INODE_TYPE_FIFO 0x1000
#define EXT2_INODE_TYPE_CHAR_DEV 0x2000
#define EXT2_INODE_TYPE_DIR 0x4000
#define EXT2_INODE_TYPE_BLOCK_DEV 0x6000
#define EXT2_INODE_TYPE_FILE 0x8000
#define EXT2_INODE_TYPE_SYMLINK 0xA000
#define EXT2_INODE_TYPE_UNIX_SOCKET 0xC000

#define EXT2_PERMISSIONS_OTHER_X 0x001
#define EXT2_PERMISSIONS_OTHER_W 0x002
#define EXT2_PERMISSIONS_OTHER_R 0x004
#define EXT2_PERMISSIONS_GROUP_X 0x008
#define EXT2_PERMISSIONS_GROUP_W 0x010
#define EXT2_PERMISSIONS_GROUP_R 0x020
#define EXT2_PERMISSIONS_USER_X 0x040
#define EXT2_PERMISSIONS_USER_W 0x080
#define EXT2_PERMISSIONS_USER_R 0x100
#define EXT2_PERMISSIONS_STICKY_BIT 0x200
#define EXT2_PERMISSIONS_GID 0x400
#define EXT2_PERMISSIONS_UID 0x800

#define EXT2_INODE_FLAG_SECURE_DELETION 0x1
#define EXT2_INODE_FLAG_KEEP_COPY 0x2
#define EXT2_INODE_FLAG_COMPRESSION 0x4
#define EXT2_INODE_FLAG_SYNC_IMMEDIATE 0x8
#define EXT2_INODE_FLAG_IMMUTABLE 0x10
#define EXT2_INODE_FLAG_APPEND_ONLY 0x20
#define EXT2_INODE_FLAG_NO_DUMP 0x40
#define EXT2_INODE_FLAG_NO_LAST_ACCESS 0x80
#define EXT2_INODE_FLAG_HASH_INDEXED_DIR 0x10000
#define EXT2_INODE_FLAG_AFS 0x20000
#define EXT2_INODE_FLAG_JOURNAL_FILE_DATA 0x40000

#define EXT2_DIR_ENTRY_TYPE_UNKNOWN 0
#define EXT2_DIR_ENTRY_TYPE_FILE 1
#define EXT2_DIR_ENTRY_TYPE_DIR 2
#define EXT2_DIR_ENTRY_TYPE_CHAR_DEV 3
#define EXT2_DIR_ENTRY_TYPE_BLOCK_DEV 4
#define EXT2_DIR_ENTRY_TYPE_FIFO 5
#define EXT2_DIR_ENTRY_TYPE_SOCKET 6
#define EXT2_DIR_ENTRY_TYPE_SYMLINK 7

typedef struct {
  uint32_t inodes_count;
  uint32_t blocks_count;
  uint32_t superuser_blocks_count;
  uint32_t unallocated_blocks_count;

  uint32_t unallocated_inodes_count;
  uint32_t superblock_block;
  uint32_t log2_block_size_minus_10;
  uint32_t log2_frag_size_minus_10;
  uint32_t blocks_per_group;

  uint32_t frags_per_group;
  uint32_t inodes_per_group;
  uint32_t last_mount_POSIX_time;
  uint32_t last_write_POSIX_time;

  uint16_t mounts_since_last_fsck;
  uint16_t mounts_until_next_fsck;
  uint16_t ext2_signature;
  uint16_t fs_state;
  uint16_t error_handling_method;
  uint16_t version_min;

  uint32_t last_fsc_POSIX_time;
  uint32_t forced_fsck_POSIX_period;
  uint32_t fs_creator_OS;
  uint32_t version_maj;

  uint16_t uid;
  uint16_t gid;

  /* Extended fields */
  uint32_t first_non_reserved_inode;
  uint16_t inode_size;
  uint16_t block_group;

  uint32_t optional_features;
  uint32_t required_features;
  uint32_t crucial_required_features_flag;

  uint8_t fs_id[16];
  uint8_t volume_name[16];
  uint8_t last_mount_path[64];

  uint32_t compression_algos;

  uint8_t files_preallocated_blocks;
  uint8_t dirs_preallocated_blocks;

  uint16_t unused;

  uint8_t journal_id[16];
  uint32_t journal_inode;
  uint32_t journal_device;
  uint32_t orphan_inode_list_head;
} __attribute__((packed)) ext2_superblock_t;

typedef struct {
  uint32_t block_bitmap_addr;
  uint32_t inode_bitmap_addr;
  uint32_t inode_table_starting_block_addr;
  uint16_t unallocated_blocks_count;
  uint16_t unallocated_inodes_count;
  uint16_t directories_count;
  uint8_t unused[14];
} __attribute__((packed)) ext2_block_group_dt_t;

typedef struct {
  uint16_t type_permissions;
  uint16_t uid;
  uint32_t size;
  uint32_t last_access_ptime;
  uint32_t creation_ptime;
  uint32_t last_modified_ptime;
  uint32_t deleted_ptime;

  uint16_t gid;

  uint16_t hard_links_count;
  uint32_t disk_sectors;
  uint32_t flags;
  uint32_t os_specific_0;

  uint32_t direct_block_pointer[12];
  uint32_t singly_indirect_block_pointer;
  uint32_t doubly_indirect_block_pointer;
  uint32_t triply_indirect_block_pointer;

  uint32_t generation_number;

  uint32_t __extended_attr_block;
  uint32_t __file_size_hi___dir_ACL;

  uint32_t frag_block_addr;
  uint32_t os_specific_1[3];
} __attribute__((packed)) ext2_inode_t;

typedef struct {
  uint32_t inode_number;
  uint16_t entry_size;
  uint8_t name_length_lo;
  uint8_t type_indicator__name_length_hi;
  char name[];
} __attribute__((packed)) ext2_directory_entry_t;

typedef struct {
  ext2_superblock_t *superblock;
  ext2_block_group_dt_t *gd;
  uint32_t groups_count;
  uint32_t block_size;
} ext2_t;

struct fs;
typedef struct fs fs_t;
#include <fs/dirent.h>
#include <misc/mbr.h>

fs_t *ext2_init(ATA_drive_t *drv, uint8_t partition_number);
void ext2_get_inode(fs_t *fs, uint32_t inode_number, ext2_inode_t *inode);

DIR *ext2_opendir(fs_t *fs, uint32_t inode_number);
struct dirent *ext2_readdir(DIR *dir);
void ext2_closedir(DIR *dir);
uint32_t ext2_readfile(fs_t *fs, uint32_t inode_number, uint64_t offset,
                       uint64_t size, uint8_t *buffer);

/* VFS related */
#include <fs/vfs.h>
uint8_t ext2_type_translate(uint8_t type);

uint32_t ext2_vfs_read(vfs_file_t *node, uint32_t offset, uint32_t size,
                       char *buffer);
uint32_t ext2_vfs_write(vfs_file_t *node, uint32_t offset, uint32_t size,
                        char *buffer);
uint8_t ext2_vfs_open(vfs_file_t *node, uint8_t read, uint8_t write);
uint8_t ext2_vfs_close(vfs_file_t *node);
vfs_dirent_t *ext2_vfs_readdir(vfs_file_t *node, uint32_t index);
vfs_file_t *ext2_vfs_finddir(vfs_file_t *node, char *name);

#endif
