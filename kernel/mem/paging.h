/*
 *	File : pg.h
 *	Description : TODO
 * */

#ifndef _pg_H_
#define _pg_H_

#include <stdint.h>

#define PG_SIZE 0x1000

#define PG_PRESENT (1 << 0)
#define PG_RW (1 << 1)
#define PG_USER (1 << 2)

#define PG_FRAME 0xFFFFF000
#define PG_FLAGS 0x00000FFF

#define IS_ALIGNED(addr) (!(addr % 0x1000))

void pg_init();

uint32_t *pg_get_ker_dir();
uint32_t *pg_get_current_dir();

uint8_t pg_is_mapped(uint32_t *dir, uint32_t *virt);
void pg_map_pages(uint32_t *dir, uint32_t *phys, uint32_t *virt, uint32_t num,
                  uint32_t flags);
void pg_unmap_pages(uint32_t *dir, uint32_t *virt, uint32_t num);

void pg_switch_page_dir(uint32_t *dir);

void pg_invalidate_cache();
void pg_invalidate_page(uint32_t *virt);

void pg_page_fault(uint8_t code);

uint32_t *pg_virt_to_phys(uint32_t *virt);

#endif
