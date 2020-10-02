#ifndef _BST_H_
#define _BST_H_

#include <stdint.h>

typedef struct {
  BST_node_t* left;
  BST_node_t* right;
  uint32_t value;   
} BST_node_t;

BST_node_t* BST_create();
void BST_add(BST_node_t* root, uint32_t value);
void BST_remove(BST_node_t* root, uint32_t value);
void BST_find(BST_node_t* root, uint32_t value);
void BST_destroy(BST_node_t* root);

#endif
