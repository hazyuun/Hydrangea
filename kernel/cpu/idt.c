/*
 *	File : idt.c
 *	Description : TODO
 *
 * */

#include <cpu/idt.h>
#include <cpu/pic.h>

#include <io/io.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

static IDT_ptr idt;
static IDT_entry idt_entries[256];

void idt_init() {
  idt.base = (uint32_t)&idt_entries;
  idt.size = (256 * sizeof(IDT_entry)) - 1;
  memset((uint8_t *)&idt_entries, 0, sizeof(idt_entries));

  /* Code duplication time ! */
  idt_set_entry(0, (uint32_t)isr0, 0x08, I_PRESENT | I_GATETYPE_INT32);
  idt_set_entry(1, (uint32_t)isr1, 0x08, I_PRESENT | I_GATETYPE_INT32);
  idt_set_entry(2, (uint32_t)isr2, 0x08, I_PRESENT | I_GATETYPE_INT32);
  idt_set_entry(3, (uint32_t)isr3, 0x08, I_PRESENT | I_GATETYPE_INT32);
  idt_set_entry(4, (uint32_t)isr4, 0x08, I_PRESENT | I_GATETYPE_INT32);
  idt_set_entry(5, (uint32_t)isr5, 0x08, I_PRESENT | I_GATETYPE_INT32);
  idt_set_entry(6, (uint32_t)isr6, 0x08, I_PRESENT | I_GATETYPE_INT32);
  idt_set_entry(7, (uint32_t)isr7, 0x08, I_PRESENT | I_GATETYPE_INT32);
  /* What a drag */
  idt_set_entry(8, (uint32_t)isr8, 0x08, I_PRESENT | I_GATETYPE_INT32);
  idt_set_entry(9, (uint32_t)isr9, 0x08, I_PRESENT | I_GATETYPE_INT32);
  idt_set_entry(10, (uint32_t)isr10, 0x08, I_PRESENT | I_GATETYPE_INT32);
  idt_set_entry(11, (uint32_t)isr11, 0x08, I_PRESENT | I_GATETYPE_INT32);
  idt_set_entry(12, (uint32_t)isr12, 0x08, I_PRESENT | I_GATETYPE_INT32);
  idt_set_entry(13, (uint32_t)isr13, 0x08, I_PRESENT | I_GATETYPE_INT32);
  idt_set_entry(14, (uint32_t)isr14, 0x08, I_PRESENT | I_GATETYPE_INT32);
  idt_set_entry(15, (uint32_t)isr15, 0x08, I_PRESENT | I_GATETYPE_INT32);
  /* This is boring */
  idt_set_entry(16, (uint32_t)isr16, 0x08, I_PRESENT | I_GATETYPE_INT32);
  idt_set_entry(17, (uint32_t)isr17, 0x08, I_PRESENT | I_GATETYPE_INT32);
  idt_set_entry(18, (uint32_t)isr18, 0x08, I_PRESENT | I_GATETYPE_INT32);
  idt_set_entry(19, (uint32_t)isr19, 0x08, I_PRESENT | I_GATETYPE_INT32);
  idt_set_entry(20, (uint32_t)isr20, 0x08, I_PRESENT | I_GATETYPE_INT32);
  idt_set_entry(21, (uint32_t)isr21, 0x08, I_PRESENT | I_GATETYPE_INT32);
  idt_set_entry(22, (uint32_t)isr22, 0x08, I_PRESENT | I_GATETYPE_INT32);
  idt_set_entry(23, (uint32_t)isr23, 0x08, I_PRESENT | I_GATETYPE_INT32);
  /* Imagine copy paste doesn't exist */
  idt_set_entry(24, (uint32_t)isr24, 0x08, I_PRESENT | I_GATETYPE_INT32);
  idt_set_entry(25, (uint32_t)isr25, 0x08, I_PRESENT | I_GATETYPE_INT32);
  idt_set_entry(26, (uint32_t)isr26, 0x08, I_PRESENT | I_GATETYPE_INT32);
  idt_set_entry(27, (uint32_t)isr27, 0x08, I_PRESENT | I_GATETYPE_INT32);
  idt_set_entry(28, (uint32_t)isr28, 0x08, I_PRESENT | I_GATETYPE_INT32);
  idt_set_entry(29, (uint32_t)isr29, 0x08, I_PRESENT | I_GATETYPE_INT32);
  idt_set_entry(30, (uint32_t)isr30, 0x08, I_PRESENT | I_GATETYPE_INT32);
  idt_set_entry(31, (uint32_t)isr31, 0x08, I_PRESENT | I_GATETYPE_INT32);
  /* Finally ! */
  /* TODO: find a better way and clean that mess */

  /* Code duplication time 2.0 */
  idt_set_entry(32, (uint32_t)irq0, 0x08, I_PRESENT | I_GATETYPE_INT32);
  idt_set_entry(33, (uint32_t)irq1, 0x08, I_PRESENT | I_GATETYPE_INT32);
  idt_set_entry(34, (uint32_t)irq2, 0x08, I_PRESENT | I_GATETYPE_INT32);
  idt_set_entry(35, (uint32_t)irq3, 0x08, I_PRESENT | I_GATETYPE_INT32);
  idt_set_entry(36, (uint32_t)irq4, 0x08, I_PRESENT | I_GATETYPE_INT32);
  idt_set_entry(37, (uint32_t)irq5, 0x08, I_PRESENT | I_GATETYPE_INT32);
  idt_set_entry(38, (uint32_t)irq6, 0x08, I_PRESENT | I_GATETYPE_INT32);
  idt_set_entry(39, (uint32_t)irq7, 0x08, I_PRESENT | I_GATETYPE_INT32);
  /* One more chunk ! */
  idt_set_entry(40, (uint32_t)irq8, 0x08, I_PRESENT | I_GATETYPE_INT32);
  idt_set_entry(41, (uint32_t)irq9, 0x08, I_PRESENT | I_GATETYPE_INT32);
  idt_set_entry(42, (uint32_t)irq10, 0x08, I_PRESENT | I_GATETYPE_INT32);
  idt_set_entry(43, (uint32_t)irq11, 0x08, I_PRESENT | I_GATETYPE_INT32);
  idt_set_entry(44, (uint32_t)irq12, 0x08, I_PRESENT | I_GATETYPE_INT32);
  idt_set_entry(45, (uint32_t)irq13, 0x08, I_PRESENT | I_GATETYPE_INT32);
  idt_set_entry(46, (uint32_t)irq14, 0x08, I_PRESENT | I_GATETYPE_INT32);
  idt_set_entry(47, (uint32_t)irq15, 0x08, I_PRESENT | I_GATETYPE_INT32);
  /* Finally ! 2.0 */

  /* syscalls : int $0x80 */
  idt_set_entry(128, (uint32_t)isr32, 0x08, I_PRESENT | I_DPL(3) | I_GATETYPE_TRAP32);
  
  __asm__ __volatile__("lidt %0" ::"m"(idt));
  __asm__ __volatile__("sti");  
}

void idt_set_entry(uint8_t index, uint32_t base, uint16_t segsel,
                   uint8_t flags) {
  idt_entries[index].base_lo = base & 0xFFFF;
  idt_entries[index].base_hi = (base >> 16) & 0xFFFF;

  idt_entries[index].segsel = segsel;
  idt_entries[index].zero = 0;
  idt_entries[index].flags = flags;
}

