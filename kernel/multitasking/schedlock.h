#ifndef _SCHEDLOCK_H_
#define _SCHEDLOCK_H_

#include <stdint.h>

uint32_t get_eflags_and_cli(void);
void set_eflags(uint32_t);

#endif