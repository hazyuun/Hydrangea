#include <cpu/pic.h>
#include <io/io.h>

void pic_init() {
  /* ICW1: Initialize PIC and expect an ICW4 */
  io_outb(PIC_1_CMD, 0x11);
  io_outb(PIC_2_CMD, 0x11);

  /* ICW2: Remap IRQ0 to 0x20 and IRQ8 to 0x28*/
  io_outb(PIC_1_DATA, 0x20);
  io_outb(PIC_2_DATA, 0x28);

  /* ICW3: Specify how PICs are connected together            */
  /*  For PIC 1, each bit is an IRQ, so for IRQ2 we send 0x04 */
  /*  For PIC 2, we just send 0x02 for IRQ2                   */
  io_outb(PIC_1_DATA, 0x04);
  io_outb(PIC_2_DATA, 0x02);

  /* ICW4: Operate in x86 mode */
  io_outb(PIC_1_DATA, 0x01);
  io_outb(PIC_2_DATA, 0x01);

  /* Mask all interrupts */
  io_outb(PIC_1_MASK, 0xFB);
  io_outb(PIC_2_MASK, 0xFF);
}

uint8_t pic_get_mask(uint8_t pic) {
  if (pic != 1 && pic != 2)
    return 0;

  uint8_t mask;
  uint16_t port;

  port = (pic == 1) ? PIC_1_MASK : PIC_2_MASK;
  mask = io_inb(port);

  return mask;
}

void pic_set_mask(uint8_t pic, uint8_t mask) {
  if (pic != 1 && pic != 2)
    return;

  uint16_t port;
  port = (pic == 1) ? PIC_1_MASK : PIC_2_MASK;

  io_outb(port, mask);
}

void pic_mask(uint8_t irq) {
  uint8_t pic;

  pic = (irq < 8) ? 1 : 2;
  irq -= (irq < 8) ? 0 : 8;

  uint8_t mask = pic_get_mask(pic);
  mask |= (1 << irq);

  pic_set_mask(pic, mask);
}

void pic_unmask(uint8_t irq) {
  uint8_t pic;

  pic = (irq < 8) ? 1 : 2;
  irq -= (irq < 8) ? 0 : 8;

  uint8_t mask = pic_get_mask(pic);
  mask &= ~(1 << irq);

  pic_set_mask(pic, mask);
}

void pic_eoi(uint8_t irq) {
  if (irq >= 8) {
    io_outb(PIC_2_CMD, 0x20);
  }
  io_outb(PIC_1_CMD, 0x20);
}

/* When the IRQ7 is sus ! (spurious interrupt lol) */
uint8_t pic_is_irq7_sus() {
  io_outb(PIC_1_CMD, 0x0B);
  uint8_t sus = io_inb(PIC_1_CMD) & 0x80;
  return sus;
}

uint8_t pic_is_irq15_sus() {
  io_outb(PIC_2_CMD, 0x0B);
  uint8_t sus = io_inb(PIC_2_CMD) & 0x80;

  if (sus)
    io_outb(PIC_1_CMD, 0x20);

  return sus;
}
