#ifndef _GDT_H_
#define _GDT_H_

#include <stddef.h>
#include <stdint.h>

/*
  Segment selector :

  **** **** **** *0##
                  ^ Always 0 for GDT, and always 1 for LDT


  where * means the segment index in the gdt
  and # means RPL

*/
#define GDT_SEGSEL(index, RPL) ((index << 3) | RPL)

/* Code : Readable | Data : Writable */
#define GDT_RW (1 << 1)

/* Segment growing direction | 0:UP | 1:Down */
#define GDT_DC (1 << 2)

/* Executable? (ie. 0:data | 1:code) */
#define GDT_X (1 << 3)

/* Ring (Privilliege level) */
#define GDT_RG(k) (k << 5)

/* Present bit (must be 1) */
#define GDT_PR (1 << 7)

/* Global Descriptor Table */
typedef struct {
  uint16_t size;
  uint32_t base;
} __attribute__((packed)) GDT_ptr_t;

typedef struct {
  uint16_t limit_lo; /* Lowest 16 bits of the limit */
  uint16_t base_lo;  /* First 16 bits of the base  */
  uint8_t base_mid;  /* Next 8 bits */
  uint8_t rights;    /* Access rights */
  uint8_t lim_hi_and_flags;
  uint8_t base_hi; /* Highest 8 bits of the base */
} __attribute__((packed)) GDT_entry_t;

void gdt_init();
void gdt_set_entry(GDT_entry_t *entries, int index, uint32_t base, uint32_t limit,
                   uint8_t rights, uint8_t flags);
extern void gdt_load(GDT_ptr_t *gdt_ptr);

#endif
