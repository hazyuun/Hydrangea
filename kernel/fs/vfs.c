/*
 *	File : vfs.c
 *	Description : TODO
 * */

#include <fs/vfs.h>
#include <mem/heap.h>
#include <stdio.h>
#include <string.h>

vfs_node_t *vfs_root;


vfs_node_t *vfs_create_node(const char *name) {
  vfs_node_t *node = (vfs_node_t *)kmalloc(sizeof(vfs_node_t));
  memset(node, 0, sizeof(node));
  node->name = (char *)kmalloc(256 * sizeof(char));

  strcpy(node->name, name);
  node->childs = 0;
  return node;
}

vfs_node_t *vfs_add_child(vfs_node_t *parent, char *name) {
  if (!parent) {

    printk("\nParent is NULL");
    return NULL;
  }
  if (parent->childs == 0) {
    parent->childs = vfs_create_node(name);
    parent->childs->parent = parent;
    return parent->childs;
  }
  vfs_node_t *child = parent->childs;
  
  while (child->next) 
    child = child->next;

  child->next = vfs_create_node(name);
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
  return child;
}

vfs_node_t *vfs_get_child_by_name(vfs_node_t *parent, char *name) {
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
  vfs_root = vfs_create_node("/");
  vfs_root->parent = vfs_root;
  vfs_add_child(vfs_root, "bin");
  vfs_add_child(vfs_root, "etc");
  vfs_add_child(vfs_root, "home");
  vfs_add_child(vfs_root, "dev");
  

  vfs_node_t *home = vfs_get_child(vfs_root, 2);
  vfs_add_child(home, "yuun");
  vfs_add_child(home, "yuusuf");

  vfs_node_t *ys = vfs_get_child(home, 0);
  vfs_add_child(ys, "document.txt");
  vfs_add_child(ys, "program.c");

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
  vfs_node_t *node = root;
  while (token) {
    node = vfs_get_child_by_name(node, token);
    if (!node)
      return NULL;
    token = strtok(NULL, " ");
  }
  return node;
}

vfs_node_t *vfs_get_root() { return vfs_root; }
