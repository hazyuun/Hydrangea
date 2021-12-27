#include <cpu/irq.h>
#include <cpu/pic.h>
#include <kernel.h>

irq_handler_t irq_handlers[16] = {0};

void irq_common_handler(registers_t *r) {
  uint8_t irq = r->int_num - 32;

  if (irq == 7 && pic_is_irq7_sus())
    return;

  if (irq == 15 && pic_is_irq15_sus())
    return;

  pic_eoi(irq);

  if (irq_handlers[irq])
    irq_handlers[irq](r);
}

void irq_register(uint8_t irq, irq_handler_t handler) {
  irq_handlers[irq] = handler;
}