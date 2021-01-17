/*
 *	File : vfs.h
 *	Description : TODO
 * */

#ifndef _VFS_H_
#define _VFS_H_

#include <stddef.h>
#include <stdint.h>

#define VFS_FILE 0x0
#define VFS_DIR 0x1
#define VFS_CDEV 0x2
#define VFS_BDEV 0x3
#define VFS_SYMLNK 0x4
#define VFS_PIPE 0x5
#define VFS_MTPT 0x6

typedef struct {
  char *name;
  uint32_t inode;
} dirent_t;
struct vfs_file;

typedef uint32_t (*read_fptr_t)(struct vfs_file *, uint32_t, uint32_t, char *);
typedef uint32_t (*write_fptr_t)(struct vfs_file *, uint32_t, uint32_t, char *);
typedef void (*open_fptr_t)(struct vfs_file *);
typedef void (*close_fptr_t)(struct vfs_file *);
typedef dirent_t *(*readdir_fptr_t)(struct vfs_file *, uint32_t);
typedef struct vfs_file *(*finddir_fptr_t)(struct vfs_file *, char *name);

typedef struct vfs_node {
  char *name;
  struct vfs_node *parent;
  struct vfs_node *childs;

  struct vfs_file *file;

  struct vfs_node *next;
  struct vfs_node *prev;
} vfs_node_t;

typedef struct vfs_file {
  char *name;
  uint8_t type;
  uint16_t permissions;
  uint32_t uid;
  uint32_t gid;
  uint32_t inode;
  size_t size;

  read_fptr_t read;
  write_fptr_t write;
  open_fptr_t open;
  close_fptr_t close;
  readdir_fptr_t readdir;
  finddir_fptr_t finddir;

  struct vfs_file *ptr;

} vfs_file_t;

vfs_node_t *vfs_get_root();

vfs_node_t *vfs_create_node(const char *name, uint8_t type);

vfs_node_t *vfs_add_child(vfs_node_t *parent, char *name, uint8_t type);

vfs_node_t *vfs_get_child(vfs_node_t *parent, size_t index);

vfs_node_t *vfs_get_child_by_name(vfs_node_t *parent, char *name);

void vfs_show_tree(vfs_node_t *root, size_t level);

void vfs_dummy();

char *vfs_abs_path_to(vfs_node_t *node);

vfs_node_t *vfs_abspath_to_node(vfs_node_t *root, char *path);
vfs_node_t *vfs_make_node(vfs_node_t *root, char *path, uint8_t type,
                          uint32_t inode);
uint8_t vfs_is_dir(vfs_node_t *node);

uint32_t vfs_read(vfs_file_t *node, uint32_t offset, uint32_t size,
                  char *buffer);
uint32_t vfs_write(vfs_file_t *node, uint32_t offset, uint32_t size,
                   char *buffer);
void vfs_open(vfs_file_t *node, uint8_t read, uint8_t write);
void vfs_close(vfs_file_t *node);
dirent_t *vfs_readdir(vfs_file_t *node, uint32_t index);
vfs_file_t *vfs_finddir(vfs_file_t *node, char *name);

#endif
