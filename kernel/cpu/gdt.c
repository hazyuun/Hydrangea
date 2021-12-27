#include <cpu/gdt.h>
#include <cpu/tss.h>
#include <term/term.h>

static GDT_ptr_t gdt;
static GDT_entry_t gdt_entries[6];

void gdt_init() {
  gdt.base = (uint32_t)&gdt_entries;
  gdt.size = sizeof(gdt_entries) - 1;

  /* Null segment */
  gdt_set_entry(gdt_entries, 0, 0x0, 0x0, 0, 0);

  /* Kernel code */
  gdt_set_entry(gdt_entries, 1, 0x0, 0xFFFFFFFF,
                (GDT_RW | GDT_X | GDT_S | GDT_PR), 0xC0);

  /* Kernel data */
  gdt_set_entry(gdt_entries, 2, 0x0, 0xFFFFFFFF, (GDT_RW | GDT_S | GDT_PR),
                0xC0);

  /* User code (Ring 3) */
  gdt_set_entry(gdt_entries, 3, 0x0, 0xFFFFFFFF,
                (GDT_RW | GDT_X | GDT_S | GDT_RG(3) | GDT_PR), 0xC0);

  /* User data */
  gdt_set_entry(gdt_entries, 4, 0x0, 0xFFFFFFFF,
                (GDT_RW | GDT_S | GDT_RG(3) | GDT_PR), 0xC0);

  tss_init(gdt_entries, 5, 0x10);

  gdt_load(&gdt);
  tss_load(0x28);
}

void gdt_set_entry(GDT_entry_t *entries, int index, uint32_t base,
                   uint32_t limit, uint8_t rights, uint8_t flags) {
  entries[index].base_lo = base & 0x0000FFFF;
  entries[index].base_mid = (base & 0x00FF0000) >> 16;
  entries[index].base_hi = (base & 0xFF000000) >> 24;

  entries[index].limit_lo = limit & 0x0000FFFF;
  entries[index].lim_hi_and_flags =
      (((limit & 0xFFFF0000) >> 16) & 0x0F) | (flags & 0xF0);
  entries[index].rights = rights;
}
