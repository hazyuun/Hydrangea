#include <ds/ordered_list.h>
#include <mem/heap.h>
#include <mem/pmm.h>
#include <stdio.h>
#include <string.h>
#include <tty/tty.h>
ordered_list_t ol_create(int max_size) {
  ordered_list_t ol;
  ol.items = (void *)pmalloc(max_size * sizeof(void *));
  memset(ol.items, 0, max_size * sizeof(void *));
  ol.size = 0;
  ol.max_size = max_size;
  return ol;
}

ordered_list_t ol_place_at(void *addr, uint32_t max_size) {
  ordered_list_t ol;
  ol.items = (void **)addr;
  memset(ol.items, 0, max_size * sizeof(void *));
  ol.size = 0;
  ol.max_size = max_size;
  return ol;
}

void ol_insert(ordered_list_t *ol, void *item, comparator_t cmp) {
  uint32_t i = 0;

  while (cmp(item, ol->items[i]) == 1 && i < ol->size)
    i++;
  if (i == ol->size)
    ol->items[ol->size++] = item;
  else {
    void *tmp = ol->items[i];
    ol->items[i] = item;
    while (i < ol->size) {
      void *tmp_ = ol->items[++i];
      ol->items[i] = tmp;
      tmp = tmp_;
    }
    ol->size++;
  }
}
void *ol_find(ordered_list_t *ol, uint32_t index) {
  if (index >= ol->size)
    return NULL;
  return ol->items[index];
}
void ol_remove(ordered_list_t *ol, uint32_t index) {
  while (index < ol->size) {
    ol->items[index] = ol->items[index + 1];
    index++;
  }
  ol->size--;
}
