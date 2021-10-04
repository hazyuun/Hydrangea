#ifndef _LIST_H_
#define _LIST_H_
#include <stdint.h>

typedef struct {
  void **data;
  uint32_t used;
  uint32_t size;
} list_t;

list_t *make_list(uint32_t size);
void list_init(list_t *list, uint32_t size);
uint32_t list_push(list_t *list, void *element);
uint8_t list_delete(list_t *list, uint32_t index);
void *list_get(list_t *list, uint32_t index);

#endif
