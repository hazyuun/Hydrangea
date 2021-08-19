#ifndef _IRQ_H_
#define _IRQ_H_

#include <stdint.h>
#include <cpu/registers.h>

typedef void (*irq_handler_t)(registers_t*);

void irq_common_handler(registers_t *r);
void irq_register(uint8_t irq, irq_handler_t handler);

#endif
