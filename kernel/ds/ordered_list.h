#ifndef _ORDERERD_LIST_
#define _ORDERERD_LIST_

#include <stdint.h>
#include <stddef.h>

#include <ds/comparator.h>

typedef struct{
     void** items;
     uint32_t size;
     uint32_t max_size;
} ordered_list_t;

ordered_list_t ol_create(int max_size);
ordered_list_t ol_place_at(void* addr, uint32_t max_size);
void ol_insert(ordered_list_t* ol, void* item, comparator_t cmp);
void* ol_find(ordered_list_t* ol, uint32_t index);
void ol_remove(ordered_list_t* ol, uint32_t index);


#endif
