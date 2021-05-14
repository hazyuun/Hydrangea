#include <cpu/tss.h>

struct tss_ent tss = {0};
extern uint32_t stb;

void tss_init(GDT_entry_t *gdt_entries, uint8_t index, uint32_t ss0) {
  tss.ss0 = ss0;
  tss.iomap = (uint16_t) sizeof(tss);
  uint32_t base = (uint32_t)&tss;
  uint32_t limit = base + sizeof(tss);
  uint32_t rights = 1 | GDT_X | GDT_PR;

  uint32_t flags = 0;

  gdt_set_entry(gdt_entries, index, base, limit, rights, flags);
}

void tss_set_esp0(uint32_t esp0) { tss.esp0 = esp0; }
