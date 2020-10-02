#ifndef _BST_H_
#define _BST_H_

#include <stdint.h>
#include <ds/comparator.h>

typedef struct BST_node{
  struct BST_node* left;
  struct BST_node* right;
  void* key;
} BST_node_t;



BST_node_t* BST_create();
void BST_add(BST_node_t* root, void* key, comparator_t cmp);
BST_node_t* BST_remove(BST_node_t* root, void* key, comparator_t cmp);
BST_node_t* BST_find(BST_node_t* root, void* key, comparator_t cmp);
void BST_show(BST_node_t* root, int lvl);
void BST_destroy(BST_node_t* root);

#endif
