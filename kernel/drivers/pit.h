#ifndef _PIT_H_
#define _PIT_H_

#include <stdint.h>
#include <cpu/registers.h>

typedef struct {
  uint64_t timer_value;
  uint32_t freq;
} pit_infos_t;

void pit_init(uint32_t freq);
void pit_sleep(uint64_t t);
void pit_event(registers_t *r);

#endif
