/*
 *	File : paging.h
 *	Description : TODO
 * */

#ifndef _PAGING_H_
#define _PAGING_H_

#include <stdint.h>

#define PG_FRAME_ADDR     0xFFFFF000
#define PG_AVAILABLE      (7 << 9)
#define PG_RESERVED_H     (3 << 7)
#define PG_DIRTY          (1 << 6)
#define PG_ACCESSED       (1 << 5)
#define PG_RESERVED_L     (3 << 2)
#define PG_SUPERVISOR     (1 << 2)
#define PG_READ_WRITE     (1 << 1)
#define PG_PRESENT        (1 << 0)

#define PG_SIZE           0x1000

#define VIRT_TO_PG_DIR(addr)  (addr & 0xFFF)
#define VIRT_TO_PG_TBL(addr)  ((addr & 0x3FF000) >> 12)
#define VIRT_TO_PG_OFF(addr)  ((addr & 0xFFC00000) >> 22)




void pg_init();
void pg_switch_page_dir(uint32_t*);
void* pg_get_page(uint32_t addr, uint8_t make);
void pg_page_fault(uint8_t code);

#endif
