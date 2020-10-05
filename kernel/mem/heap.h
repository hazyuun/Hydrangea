/*
 *	File : heap.h
 *	Description : TODO
 *   Note : My heap implementation is bad, very bad,
 *        and I am aware of that, so if you are trying
 *        to learn from this code, it is a bad idea. 
 * */

#ifndef _HEAP_H_
#define _HEAP_H_

#include <stdint.h>
#include <ds/ordered_list.h>

#define HEAP_START            0xC0000000
#define HEAP_MAX_SIZE         0x100000
#define HEAP_INDEX_SIZE       0x20000
#define HEAP_INITIAL_SIZE     0x10000

typedef struct free_ll{
     void* addr;
     size_t size;
     struct free_ll* prev;
     struct free_ll* next;
} free_ll_t; /* Linked list of free blocks */

typedef struct free_ll alloc_ll_t;

typedef struct heap{
     void* start;
     size_t size;
     size_t max_size;
     uint8_t usr;
     free_ll_t* free_ll;
     alloc_ll_t* alloc_ll;
} heap_t;

heap_t* heap_create(void* start, size_t size, size_t max_size, uint8_t usr);

void heap_save_allocated_block(heap_t* heap, void* addr, size_t size);
size_t heap_forget_allocated_block(heap_t* heap, void* addr);

uint8_t heap_expand(heap_t* heap, size_t size);
uint8_t heap_contract(heap_t* heap, size_t size);

free_ll_t* heap_first_fit(heap_t* heap, size_t size);

void* heap_alloc(heap_t* heap, size_t size);
void heap_free(heap_t* heap, void* ptr);

void* kmalloc(size_t size);
void kfree(void* ptr);

#endif
