/*
 *	File : heap.c
 *	Description : TODO
 *   Note : My heap implementation is bad, very bad,
 *        and I am aware of that, so if you are trying
 *        to learn from this code, it is a bad idea.
 *  TODO : implement a better heap.
 * */

#include <assert.h>
#include <stdio.h>

#include <mem/heap.h>
#include <mem/paging.h>
#include <mem/pmm.h>

extern heap_t* kheap;

heap_t* heap_create(void* start, size_t size, size_t max_size, uint8_t usr){
     heap_t* heap = (heap_t*) pmalloc(sizeof(heap_t));
     heap->start    = start;
     heap->size     = size;
     heap->max_size = max_size;
     heap->usr      = usr;

     heap->free_ll  = (free_ll_t*) pmalloc(sizeof(free_ll_t));
     heap->free_ll->addr = start;
     heap->free_ll->size = size;
     heap->free_ll->prev = NULL;
     heap->free_ll->next = NULL;

     heap->alloc_ll  = NULL;
     return heap;
}

/*
uint8_t heap_expand(heap_t* heap, size_t size){
}
uint8_t heap_contract(heap_t* heap, size_t size){
}
*/

void heap_save_allocated_block(heap_t* heap, void* addr, size_t size){
     if(!heap->alloc_ll){
          heap->alloc_ll  = (alloc_ll_t*) pmalloc(sizeof(alloc_ll_t));
          heap->alloc_ll->addr = addr;
          heap->alloc_ll->size = size;
          heap->alloc_ll->prev = NULL;
          heap->alloc_ll->next = NULL;
          return;
     }
     alloc_ll_t* alloc_ll = heap->alloc_ll;

     while(alloc_ll->next != NULL){
          alloc_ll = alloc_ll->next;
     }
     alloc_ll->next  = (alloc_ll_t*) pmalloc(sizeof(alloc_ll_t));
     alloc_ll->next->addr = addr;
     alloc_ll->next->size = size;
     alloc_ll->next->prev = alloc_ll;
     alloc_ll->next->next = NULL;
}

size_t heap_forget_allocated_block(heap_t* heap, void* addr){
     
     alloc_ll_t* alloc_ll = heap->alloc_ll;

     while(alloc_ll->addr != addr && alloc_ll->next != NULL){
          alloc_ll = alloc_ll->next;
     }
     if(alloc_ll->addr != addr) return -1;
     
     alloc_ll->next->prev = alloc_ll->prev;
     alloc_ll->prev->next = alloc_ll->next;
     return alloc_ll->size;
}


free_ll_t* heap_first_fit(heap_t* heap, size_t size){
     free_ll_t* free_ll = heap->free_ll;  
     
     while(free_ll && free_ll->size < size){
          
          free_ll = free_ll->next;
          if(!free_ll) return NULL;
     }
     return free_ll;
}

void* heap_alloc(heap_t* heap, size_t size){
     free_ll_t* free_ll = heap_first_fit(heap, size);
     if(!free_ll) return NULL; /* TODO: expand the heap */
     void* addr = free_ll->addr;
     heap_save_allocated_block(heap, addr, size);
     if(size < free_ll->size){
          free_ll->addr += size;
          free_ll->size -= size;
          
          return addr;
     }
     if(free_ll->prev)
          free_ll->prev->next = free_ll->next;
     else
          heap->free_ll = free_ll->next;
     if(free_ll->next)
          free_ll->next->prev = free_ll->prev;
     
     return addr;
}

void heap_free(heap_t* heap, void* ptr){   
     free_ll_t* free_ll = heap->free_ll;
     while(ptr > free_ll->addr)
          free_ll = free_ll->next;
     free_ll_t* new_free_node = (free_ll_t*) pmalloc(sizeof(free_ll_t));
     new_free_node->addr = ptr;
     new_free_node->size = heap_forget_allocated_block(heap, ptr);
     new_free_node->next = free_ll;
     new_free_node->prev = free_ll->prev;
     if(free_ll->prev){
          free_ll->prev->next = new_free_node;
     }else{
          heap->free_ll = new_free_node;     
     }
     free_ll->prev = new_free_node;
     if(new_free_node->addr + new_free_node->size == new_free_node->next->addr){
          new_free_node->size += new_free_node->next->size;
          new_free_node->next = new_free_node->next->next;
     }
     if(new_free_node->prev->addr + new_free_node->prev->size == new_free_node->addr){
          new_free_node->prev->size += new_free_node->size;
          new_free_node->prev->next = new_free_node->next;
     }
}

void* kmalloc(size_t size){
     return heap_alloc(kheap, size);
}

#include <string.h>
void *krealloc(void *ptr, size_t new_size){
     if(!ptr) return NULL;

     void *new_ptr = kmalloc(new_size);
     if(!new_ptr) return NULL;

     memcpy(new_ptr, ptr, new_size);
     kfree(ptr);
     return new_ptr;
}

void kfree(void* ptr){
     heap_free(kheap, ptr);
}
