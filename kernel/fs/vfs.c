/*
 *	File : vfs.c
 *	Description : TODO
 * */

#include <fs/vfs.h>
#include <mem/heap.h>
#include <stdio.h>
#include <string.h>
#include <term/term.h>

vfs_node_t *vfs_root;

uint32_t vfs_read(vfs_file_t *node, uint32_t offset, uint32_t size,
                  char *buffer) {
  if (node->read)
    return node->read(node, offset, size, buffer);
  return 0;
}
uint32_t vfs_write(vfs_file_t *node, uint32_t offset, uint32_t size,
                   char *buffer) {
  if (node->write)
    return node->write(node, offset, size, buffer);
  return 0;
}
uint8_t vfs_open(vfs_file_t *node, uint8_t read, uint8_t write) {
  (void)read;
  (void)write;
  if (node->open)
    return node->open(node);
  return 0;
}
uint8_t vfs_close(vfs_file_t *node) {
  if (node->close)
    return node->close(node);
  return 0;
}
vfs_dirent_t *vfs_readdir(vfs_file_t *node, uint32_t index) {
  if (node->readdir)
    return node->readdir(node, index);
  return 0;
}
vfs_file_t *vfs_finddir(vfs_file_t *node, char *name) {
  if (node->finddir)
    return node->finddir(node, name);
  return 0;
}

vfs_node_t *vfs_create_node(char *name, uint8_t type) {
  vfs_node_t *node = (vfs_node_t *)kmalloc(sizeof(vfs_node_t));
  memset((char *)node, 0, sizeof(node));
  node->name = (char *)kmalloc(256 * sizeof(char));
  node->file = (vfs_file_t *)kmalloc(sizeof(vfs_file_t));
  node->file->name = node->name;
  node->file->type = type;

  strcpy(node->name, name);
  node->childs = 0;
  node->next = 0;
  node->file->permissions = 0;
  if (type == VFS_DIR) {
    node->file->permissions |= (VFS_DIR << 14);
    node->file->permissions |= VFS_PERMISSIONS_USER_X;
    node->file->permissions |= VFS_PERMISSIONS_GROUP_X;
    node->file->permissions |= VFS_PERMISSIONS_OTHER_X;
  }
  node->file->permissions |= VFS_PERMISSIONS_USER_R;
  node->file->permissions |= VFS_PERMISSIONS_USER_W;
  node->file->permissions |= VFS_PERMISSIONS_GROUP_R;
  node->file->permissions |= VFS_PERMISSIONS_OTHER_R;

  return node;
}

void vfs_free_child_nodes(vfs_node_t *node) {
  vfs_node_t *n = node->childs;
  while (n) {
    vfs_free_node(n);
    n = n->next;
  }
  node->childs = 0;
}

void vfs_free_node(vfs_node_t *node) {
  kfree(node->name);
  kfree(node->file);

  vfs_free_child_nodes(node);

  kfree(node);
}

vfs_node_t *vfs_add_child(vfs_node_t *parent, char *name, uint8_t type) {
  if (!parent) {

    printk("\nCan't create %s node, parent is NULL", name);
    return NULL;
  }
  if (parent->childs == 0) {
    parent->childs = vfs_create_node(name, type);
    parent->childs->parent = parent;
    return parent->childs;
  }
  vfs_node_t *child = parent->childs;

  while (child->next) {
    child = child->next;
  }

  child->next = vfs_create_node(name, type);
  child->next->parent = parent;
  child->next->prev = child;

  return child->next;
}

vfs_node_t *vfs_get_child(vfs_node_t *parent, size_t index) {
  vfs_node_t *child = parent->childs;
  if (!child)
    return NULL;
  for (size_t i = 0; i < index; i++) {
    child = child->next;
    if (child == NULL)
      break;
  }
  if (!strcmp(child->name, "."))
    return child->parent;
  if (!strcmp(child->name, ".."))
    return child->parent->parent;
  return child;
}

vfs_node_t *vfs_get_child_by_name(vfs_node_t *parent, char *name) {
  if (!strcmp(name, "."))
    return parent;
  if (!strcmp(name, ".."))
    return parent->parent;

  vfs_node_t *child = parent->childs;
  if (!child)
    return NULL;
  while (strcmp(child->name, name) && child->next) {
    child = child->next;
    if (child == NULL)
      break;
  }
  return strcmp(child->name, name) ? NULL : child;
}

void vfs_show_tree(vfs_node_t *root, size_t level) {
  if (!strcmp(root->name, ".") || !strcmp(root->name, ".."))
    return;
  size_t lvl = level;
  term_use_color(NICE_WHITE);
  while (lvl--)
    printk("|  ");

  if (vfs_is_dir(root))
    term_use_color(NICE_YELLOW);
  else if (vfs_is_mtpt(root))
    term_use_color(NICE_RED);
  else
    term_use_color(NICE_WHITE);

  printk("+- %s\n", root->name);

  term_use_color(NICE_WHITE);
  if (root->childs) {
    vfs_node_t *child = root->childs;
    while (child) {

      vfs_show_tree(child, level + 1);
      child = child->next;
    }
  }
}

void vfs_dummy() {
  vfs_root = vfs_create_node("/", VFS_DIR);
  vfs_root->parent = vfs_root;
  vfs_add_child(vfs_root, "bin", VFS_DIR);
  vfs_add_child(vfs_root, "etc", VFS_DIR);
  vfs_add_child(vfs_root, "mnt", VFS_DIR);
  vfs_node_t *home = vfs_add_child(vfs_root, "home", VFS_DIR);
  vfs_add_child(vfs_root, "dev", VFS_DIR);

  vfs_node_t *ys = vfs_add_child(home, "yuun", VFS_DIR);
  vfs_add_child(home, "yuusuf", VFS_DIR);

  vfs_add_child(ys, "document.txt", VFS_FILE);
  vfs_add_child(ys, "program.c", VFS_FILE);
}

char *vfs_abs_path_to(vfs_node_t *node) {
  if (!node) {
    printk("node is null");
    return NULL;
  }
  char *path = (char *)kmalloc(256);
  strcpy(path, node->name);
  while (node->parent != node) {
    node = node->parent;
    char *bak = (char *)kmalloc(256);
    strcpy(bak, path);
    strcpy(path, node->name);

    if (strcmp(path, "/"))
      strcat(path, "/");
    strcat(path, bak);
    kfree(bak);
  }
  return path;
}

vfs_node_t *vfs_node_from_path(vfs_node_t *root, char *path) {
  char *ap = (char *)kmalloc(256);
  strcpy(ap, path);
  char *token = strtok(ap, "/");
  vfs_node_t *node;

  if (path[0] == '/')
    node = vfs_get_root();
  else
    node = root;

  while (token) {
    node = vfs_get_child_by_name(node, token);
    if (!node)
      return NULL;
    token = strtok(NULL, "/");
  }
  return node;
}

vfs_node_t *vfs_make_node(vfs_node_t *root, char *path, uint8_t type,
                          uint32_t inode) {
  char *ap = (char *)kmalloc(256);
  strcpy(ap, path);
  char *token = strtok(ap, "/");

  vfs_node_t *node = root;

  vfs_node_t *bkp = node;
  while (token) {
    node = vfs_get_child_by_name(node, token);
    if (!node)
      node = vfs_add_child(bkp, token, type);
    bkp = node;
    token = strtok(NULL, " ");
  }
  node->file->type = type;
  node->file->inode = inode;
  if (type == VFS_FILE)
    vfs_free_child_nodes(node);
  return node;
}

uint8_t vfs_is_dir(vfs_node_t *node) { return node->file->type == VFS_DIR; }
uint8_t vfs_is_mtpt(vfs_node_t *node) { return node->file->type == VFS_MTPT; }

vfs_node_t *vfs_get_root() { return vfs_root; }

#include <fs/dirent.h>
#include <fs/ext2fs/ext2.h>
#include <fs/generic.h>
static void vfs_ext2_populate(vfs_node_t *root, DIR *dir) {
  struct dirent *entry;
  do {
    entry = ext2_readdir(dir);
    if (!entry)
      break;

    /* In my design, . and .. directories are VFS things */
    /* so I will just skip them here */
    if (!strcmp(".", entry->name) || !strcmp("..", entry->name))
      continue;

    uint8_t type = ext2_type_translate(entry->type);

    vfs_node_t *n = vfs_add_child(root, entry->name, type);
    n->file->permissions = entry->permissions;
    n->file->fs = dir->fs;
    n->file->inode = entry->inode_number;
    n->file->read = &ext2_vfs_read;
    n->file->size = entry->size;
    
    /* Recursively populate every sub-directory */
    if (type == VFS_DIR) {
      DIR *sub_dir = ext2_opendir(dir->fs, entry->inode_number);
      vfs_ext2_populate(n, sub_dir);
      ext2_closedir(sub_dir);
    }

  } while (entry);
}

uint8_t vfs_mount_ext2(fs_t *fs, vfs_node_t *mtpt) {

  DIR *dir = ext2_opendir(fs, 2);
  vfs_ext2_populate(mtpt, dir);
  ext2_closedir(dir);
  return 0;
}

#include <misc/mbr.h>
uint8_t vfs_mount_partition(ATA_drive_t *drv, uint8_t partition_num, char *path,
                            vfs_node_t *rel) {

  if (drv->mtpts[partition_num])
    return 255;

  mbr_t mbr;
  mbr_parse(drv, &mbr);

  /* Only EXT2 for now */
  if (mbr.partitions[partition_num].type != MBR_PART_TYPE_LINUX)
    return 1;

  fs_t *fs = ext2_init(drv, partition_num);
  if (!fs)
    return 1;

  vfs_node_t *mtpt = vfs_node_from_path(rel, path);
  if (!mtpt)
    return 2;

  if (mtpt->childs)
    return 3;

  uint8_t result = vfs_mount_ext2(fs, mtpt);
  if (result)
    return result;

  mtpt->file->type = VFS_MTPT;
  drv->mtpts[partition_num] = (void *)mtpt;

  return result;
}

uint8_t vfs_umount_partition(ATA_drive_t *drv, uint8_t partition_num) {
  if (!drv->mtpts[partition_num])
    return 1;

  vfs_node_t *mtpt = (vfs_node_t*) (drv->mtpts[partition_num]);
  vfs_free_child_nodes(mtpt);
  mtpt->file->type = VFS_DIR;
  drv->mtpts[partition_num] = 0;
  return 0;
}

void vfs_drwxrwxrwx(char *out, uint16_t permissions) {
  out[0] = (permissions & (VFS_DIR << 14)) ? 'd' : '-';
  out[1] = (permissions & VFS_PERMISSIONS_USER_R) ? 'r' : '-';
  out[2] = (permissions & VFS_PERMISSIONS_USER_W) ? 'w' : '-';
  out[3] = (permissions & VFS_PERMISSIONS_USER_X) ? 'x' : '-';
  out[4] = (permissions & VFS_PERMISSIONS_GROUP_R) ? 'r' : '-';
  out[5] = (permissions & VFS_PERMISSIONS_GROUP_W) ? 'w' : '-';
  out[6] = (permissions & VFS_PERMISSIONS_GROUP_X) ? 'x' : '-';
  out[7] = (permissions & VFS_PERMISSIONS_OTHER_R) ? 'r' : '-';
  out[8] = (permissions & VFS_PERMISSIONS_OTHER_W) ? 'w' : '-';
  out[9] = (permissions & VFS_PERMISSIONS_OTHER_X) ? 'x' : '-';
}
