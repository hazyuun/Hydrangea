#include <ds/BST.h>
#include <mem/pmm.h>
#include <stdio.h>

void BST_add(BST_node_t *root, void *key, comparator_t cmp) {
  if (!root) {
    root = (BST_node_t *)pmalloc(sizeof(BST_node_t));
    root->left = NULL;
    root->right = NULL;
    root->key = key;
    return;
  }
  if (cmp(key, root->key) == 1) {
    if (root->right)
      BST_add(root->right, key, cmp);
    else {
      root->right = (BST_node_t *)pmalloc(sizeof(BST_node_t));
      root->right->left = NULL;
      root->right->right = NULL;
      root->right->key = key;
    }
  } else if (cmp(key, root->key) == -1) {
    if (root->left)
      BST_add(root->left, key, cmp);
    else {
      root->left = (BST_node_t *)pmalloc(sizeof(BST_node_t));
      root->left->left = NULL;
      root->left->right = NULL;
      root->left->key = key;
    }
  } else
    return;
}
BST_node_t *BST_remove(BST_node_t *root, void *key, comparator_t cmp) {
  if (!root)
    return root;
  if (cmp(key, root->key) == 1) {
    if (root->right)
      root->right = BST_remove(root->right, key, cmp);
    else
      return root;
  } else if (cmp(key, root->key) == -1) {
    if (root->left)
      root->left = BST_remove(root->left, key, cmp);
    else
      return root;
  } else {
    if (root->right && root->left) { /* Both */
      BST_node_t *inorder_s = root->right;
      while (inorder_s->left)
        inorder_s = inorder_s->left;
      root->key = inorder_s->key;
      root->right = BST_remove(root->right, key, cmp);
      return root;
    } else if (root->right) { /* Only right */
      /* I need kfree here */
      /* TODO: free this when I have a kfree */
      return root->left;
    } else { /* Only left */
      /* I need kfree here as well */
      return root->right;
    }
  }
  return root;
}
BST_node_t *BST_find(BST_node_t *root, void *key, comparator_t cmp) {
  if (!root)
    return NULL;
  if (cmp(key, root->key) == 1) {
    if (root->right)
      return BST_find(root->right, key, cmp);
    else
      return NULL;
  } else if (cmp(key, root->key) == -1) {
    if (root->left)
      return BST_find(root->left, key, cmp);
    else
      return NULL;
  } else
    return root;
}

void BST_show(BST_node_t *root, int lvl) {
  if (!root)
    return;
  printk("%d\n", root->key);
  int i;
  if (root->right) {
    i = lvl;
    while (i-- > 0)
      printk("|   ");
    printk("R-- ");
    BST_show(root->right, lvl + 1);
  }
  if (root->left) {
    i = lvl;
    while (i-- > 0)
      printk("|   ");
    printk("L-- ");
    BST_show(root->left, lvl + 1);
  }
}