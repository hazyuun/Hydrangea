#include <cpu/tss.h>

struct tss_ent tss = {0};
extern uint32_t stb;

void tss_init(GDT_entry_t *gdt_entries, uint8_t index) {
  __asm__ volatile ("mov %%esp, %0":"=r"(tss.esp0));
  tss.ss0 = 0x10;

  uint32_t base = (uint32_t) &tss;
  uint32_t limit = sizeof(tss);
  uint32_t rights = 1 | GDT_X | GDT_PR | GDT_RG(3);	
  uint32_t flags = 0;

  gdt_set_entry(gdt_entries, index, base, limit, rights, flags);
}
