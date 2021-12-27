#include <cpu/idt.h>
#include <cpu/pic.h>

#include <cpu/interrupts.h>

void interrupts_init() {
  pic_init();
  idt_init();
}
