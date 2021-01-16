#include <kernel.h>
#include <mem/heap.h>
#include <mem/paging.h>
#include <mem/pmm.h>
#include <stdio.h>
#include <string.h>
#include <tty/tty.h>
#include <boot/multiboot.h>

extern multiboot_info_t *global_mb_header;
extern uint32_t memory_size;
extern size_t placement_addr;

heap_t *kheap;

#define DIRECTORY_INDEX(x) ((x) >> 22)
#define TABLE_INDEX(x) (((x) >> 12) & 0x3FF)

uint32_t *cur_page_dir;
uint32_t ker_page_dir[1024] __attribute__((aligned(4096)));
uint32_t first_page_table[1024] __attribute__((aligned(4096)));
uint32_t sec_page_table[1024] __attribute__((aligned(4096)));

uint32_t *pg_get_ker_dir() { return ker_page_dir; }

uint32_t *pg_get_current_dir() { return cur_page_dir; }

static uint32_t *pg_mk_pgtbl(uint32_t *dir, uint32_t dir_index) {
  uint32_t *new_table = (uint32_t *)frame_alloc();
  for (int i = 0; i < 1024; i++) {
    new_table[i] = 0;
  }
  dir[dir_index] = (uint32_t)new_table | PG_PRESENT | PG_RW;
  return new_table;
}

static uint8_t pg_tbl_exists(uint32_t *dir, uint32_t *virt) {
  if (!IS_ALIGNED((uint32_t)virt))
    panic("pg_tbl_exists: Unaligned page address");
  uint32_t dir_index = (uint32_t)virt >> 22;
  return ((dir[dir_index] & PG_PRESENT));
}

uint8_t pg_is_mapped(uint32_t *dir, uint32_t *virt) {
  if (!IS_ALIGNED((uint32_t)virt))
    panic("pg_is_mapped: Unaligned page address");

  uint32_t dir_index = (uint32_t)virt >> 22;
  uint32_t table_index = ((uint32_t)virt >> 12) & 0x3FF;
  uint32_t *tbl = (uint32_t *)(dir[dir_index] & PG_FRAME);
  return (pg_tbl_exists(dir, virt) && (tbl[table_index] & PG_PRESENT));
}

void pg_map_pages(uint32_t *dir, uint32_t *virt, uint32_t *phys, uint32_t num,
                  uint32_t flags) {
  if (!IS_ALIGNED((uint32_t)virt))
    panic("pg_map_pages: Unaligned page address (virt)");
  if (!IS_ALIGNED((uint32_t)phys))
    panic("pg_map_pages: Unaligned page address (phys)");

  uint32_t dir_index = (uint32_t)virt >> 22;

  if (!pg_tbl_exists(dir, virt)) {
    pg_mk_pgtbl(dir, dir_index);
  }

  uint32_t *tbl = (uint32_t *)(dir[dir_index] & PG_FRAME);

  for (uint32_t i = 0; i < num; i++) {
    if (pg_is_mapped(dir, virt + i * PG_SIZE))
      panic("pg_map_pages: Tried to map an already mapped page");
    tbl[i] = ((uint32_t)phys + i * PG_SIZE) | PG_PRESENT | flags;
  }
}

void pg_unmap_pages(uint32_t *dir, uint32_t *virt, uint32_t num) {
  uint32_t dir_index = (uint32_t)virt >> 22;

  if (!pg_tbl_exists(dir, virt)) {
    return;
  }

  uint32_t *tbl = (uint32_t *)(dir[dir_index] & PG_FRAME);

  for (uint32_t i = 0; i < num; i++) {
    if (tbl[i] | PG_PRESENT) {
      frame_free(tbl[i] | PG_FRAME);
      tbl[i] = 0;
    }
  }
}

void pg_init() {
  for (int i = 0; i < 1024; i++) {
    ker_page_dir[i] = PG_RW;
  }

  pg_map_pages(ker_page_dir, (uint32_t *)0x00000000, 0x00000000, 1024, PG_RW);
  pg_map_pages(ker_page_dir, (uint32_t *)  0x400000, (uint32_t *)0x400000, 1024, PG_RW);
  
  /* Map the page of the VESA framebuffer */
  uint32_t page = global_mb_header->framebuffer_addr & 0xFFF00000;
  pg_map_pages(pg_get_ker_dir(), (uint32_t *)  page, (uint32_t *)page, 1024, PG_RW);
  
  pg_map_pages(ker_page_dir, (uint32_t *)HEAP_START, (uint32_t *)HEAP_START, 1024, PG_RW);

  kheap = heap_create(HEAP_START, HEAP_INITIAL_SIZE, HEAP_MAX_SIZE, 0);
  pg_switch_page_dir(ker_page_dir);

  uint32_t cr0;
  __asm__ __volatile__("mov %%cr0, %0" : "=r"(cr0));
  cr0 |= 0x80000000;
  __asm__ __volatile__("mov %0, %%cr0" ::"r"(cr0));
}

void pg_switch_page_dir(uint32_t *dir) {
  cur_page_dir = dir;
  asm volatile("mov %0, %%cr3\n" ::"r"(dir));
}

void pg_invalidate_cache() {
  asm("mov %cr3, %eax\n"
      "mov %eax, %cr3\n");
}

void pg_invalidate_page(uint32_t *virt) {
  asm volatile("invlpg (%0)" ::"b"(virt) : "memory");
}

void pg_page_fault(uint8_t code) {
  tty_use_color(VGA_LIGHT_RED, VGA_BLACK);
  printk("\n PAGE FAULT !\n");
  uint32_t addr;
  asm volatile("mov %%cr2, %0" : "=r"(addr));
  printk(" at ");
  tty_print_hex(addr);
  printk("\n\n");

  int present = !(code & 0x1);
  int write = code & 0x2;
  int user = code & 0x4;
  int reserved = code & 0x8;
  int instr = code & 0x10;
  printk("\t Present               : %s\n", (present ? "true" : "false"));
  printk("\t Write?                : %s\n", (write ? "true" : "false"));
  printk("\t User mode             : %s\n", (user ? "true" : "false"));
  printk("\t Reserved              : %s\n", (reserved ? "true" : "false"));
  printk("\t Instruction fetch ?   : %s\n", (instr ? "true" : "false"));
}
