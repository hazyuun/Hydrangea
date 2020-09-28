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

void pg_init();
void pg_switch_page_dir(uint32_t*);
void pg_page_fault(uint8_t code);

#endif
