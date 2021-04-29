#ifndef _SCHEDLOCK_H_
#define _SCHEDLOCK_H_

#include <stdint.h>

#define SCHEDLOCK(ef) ef = get_eflags_and_cli();
#define SCHEDUNLOCK(ef) set_eflags_and_sti(ef);

uint32_t get_eflags_and_cli(void);
void set_eflags_and_sti(uint32_t);

#endif