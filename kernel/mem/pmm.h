/*
 *	File : pmm.h
 *	Description : TODO
 * */

#ifndef _PMM_H_
#define _PMM_H_

#include <mem/paging.h>
#include <stddef.h>
#include <stdint.h>
/* Placement malloc */
void *pmalloc(size_t size);

/* Placement malloc with alignment */
void *pmalloc_a(size_t size, size_t align);

/* Placement malloc with alignment that also gives physical address of allocated
 * memory */
void *pmalloc_ap(size_t size, size_t align, size_t *physical_addr);

void pmm_init();

/* Frames bitmap related functions */
void frame_bmp_set(uint32_t addr);
void frame_bmp_reset(uint32_t addr);
uint32_t frame_bmp_test(uint32_t addr);
uint32_t frame_bmp_get_first();

/* Frame allocator */

uint32_t frame_alloc();
/*uint8_t frame_realloc(page_t* p, uint8_t usr, uint8_t rw);
uint8_t frame_map(page_t* p, uint32_t addr, uint8_t remap, uint8_t usr, uint8_t
rw);
*/
void frame_free(uint32_t addr);

#endif
