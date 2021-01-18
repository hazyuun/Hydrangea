/*
 *	File : vfs.c
 *	Description : TODO
 * */

#include <fs/vfs.h>
#include <mem/heap.h>
#include <stdio.h>
#include <string.h>

vfs_node_t *vfs_root;

uint32_t vfs_read(vfs_file_t *node, uint32_t offset, uint32_t size,
                  char *buffer) {
  if (node->read)
    return node->read(node, offset, size, buffer);
  return 0;
}
uint32_t vfs_write(vfs_file_t *node, uint32_t offset, uint32_t size,
                   char *buffer) {}
void vfs_open(vfs_file_t *node, uint8_t read, uint8_t write) {}
void vfs_close(vfs_file_t *node) {}
dirent_t *vfs_readdir(vfs_file_t *node, uint32_t index) {}
vfs_file_t *vfs_finddir(vfs_file_t *node, char *name) {}

vfs_node_t *vfs_create_node(const char *name, uint8_t type) {
  vfs_node_t *node = (vfs_node_t *)kmalloc(sizeof(vfs_node_t));
  memset(node, 0, sizeof(node));
  node->name = (char *)kmalloc(256 * sizeof(char));
  node->file = (vfs_file_t *)kmalloc(sizeof(vfs_file_t));
  node->file->name = node->name;
  node->file->type = type;

  strcpy(node->name, name);
  node->childs = 0;
  // if (type == VFS_DIR && strcmp(name, "/") && strcmp(name, ".") &&
  //     strcmp(name, "..")) {
  //   vfs_node_t *dot = vfs_add_child(node, ".", VFS_DIR);
  //   vfs_node_t *dotdot = vfs_add_child(node, "..", VFS_DIR);
  // }
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

  while (child->next)
    child = child->next;

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
  while (lvl--)
    printk("|  ");
  printk("+- %s\n", root->name);
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

vfs_node_t *vfs_abspath_to_node(vfs_node_t *root, char *path) {
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
      node = vfs_add_child(bkp, token, VFS_DIR);
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
vfs_node_t *vfs_get_root() { return vfs_root; }
