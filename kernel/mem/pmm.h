/*
 *	File : pmm.h
 *	Description : TODO
 * */

#ifndef _PMM_H_
#define _PMM_H_

#include <stdint.h>
#include <stddef.h>
#include <mem/paging.h>
/* Placement malloc */
void* pmalloc(size_t size);

/* Placement malloc with alignment */
void* pmalloc_a(size_t size, size_t align);

/* Placement malloc with alignment that also gives physical address of allocated memory */
void* pmalloc_ap(size_t size, size_t align, size_t* physical_addr);

/* Frames bitmap related functions */
void frame_bmp_set(uint32_t addr);
void frame_bmp_reset(uint32_t addr);
uint32_t frame_bmp_test(uint32_t addr);
uint32_t frame_bmp_get_first();

/* Frame allocator */
void* frame_alloc();
void frame_free(void* addr);


#endif
