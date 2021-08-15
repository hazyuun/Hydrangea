#ifndef _TSS_H_
#define _TSS_H_

#include <stdint.h>

struct tss_ent {
  uint32_t prev_tss;
  uint32_t esp0;
  uint32_t ss0;
  uint32_t esp1;
  uint32_t ss1;
  uint32_t esp2;
  uint32_t ss2;
  uint32_t cr3;
  uint32_t eip;
  uint32_t eflags;
  uint32_t eax;
  uint32_t ecx;
  uint32_t edx;
  uint32_t ebx;
  uint32_t esp;
  uint32_t ebp;
  uint32_t esi;
  uint32_t edi;
  uint32_t es;
  uint32_t cs;
  uint32_t ss;
  uint32_t ds;
  uint32_t fs;
  uint32_t gs;
  uint32_t ldt;
  uint16_t trap;
  uint16_t iomap;
} __attribute__((packed));

#include <cpu/gdt.h>
void tss_init(GDT_entry_t *gdt_entries, uint8_t index, uint32_t ss0);
void tss_load(uint16_t segsel);
void tss_set_esp0(uint32_t new_esp0);

#endif
