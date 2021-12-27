#include <ds/list.h>
#include <mem/heap.h>
#include <stddef.h>
#include <stdint.h>

void list_init(list_t *list, uint32_t size) {
  list->size = size;
  list->used = 0;
  list->data = (void **)kmalloc(size * sizeof(void *));
}

list_t *make_list(uint32_t size) {
  list_t *list = (list_t *)kmalloc(sizeof(list_t));
  list_init(list, size);
  return list;
}

uint32_t list_push(list_t *list, void *element) {
  if (list->used == list->size) {
    list->size *= 2;
    list->data = (void **)krealloc(list->data, list->size * sizeof(void *));
  }

  list->data[list->used++] = element;
  return list->used - 1;
}

uint8_t list_delete(list_t *list, uint32_t index) {
  if (index > list->used)
    return 1;

  for (uint32_t i = index; i < list->used; i++)
    list->data[i] = list->data[i + 1];
  --list->used;
  return 0;
}

void *list_get(list_t *list, uint32_t index) {
  if (index > list->used)
    return NULL;
  return list->data[index];
}
