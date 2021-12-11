#include <kernel.h>

#include <mem/heap.h>
#include <mem/paging.h>
#include <mem/pmm.h>

#include <term/term.h>
#include <util/logger.h>

#include <stdio.h>
#include <string.h>

extern heap_t *kheap;

uint32_t cur_page_dir;
uint32_t ker_page_dir[1024] __attribute__((aligned(4096)));

uint32_t pg_get_ker_dir() { return (uint32_t) ker_page_dir; }

uint32_t pg_get_current_dir() { return cur_page_dir; }

static uint32_t pg_mk_pgtbl(uint32_t dir, uint32_t dir_index, uint32_t flags) {
  uint32_t *new_table = (uint32_t *)frame_alloc();
  for (int i = 0; i < 1024; i++) {
    new_table[i] = 0;
  }
  ((uint32_t*) dir)[dir_index] = (uint32_t)new_table | PG_PRESENT | flags;
  return (uint32_t) new_table;
}

static uint8_t pg_tbl_exists(uint32_t dir, uint32_t virt) {
  if (!IS_ALIGNED((uint32_t)virt))
    panic("pg_tbl_exists: Unaligned page address");
  uint32_t dir_index = (uint32_t)virt >> 22;
  return ((((uint32_t*) dir)[dir_index] & PG_PRESENT));
}

uint8_t pg_is_mapped(uint32_t dir, uint32_t virt) {
  if (!IS_ALIGNED((uint32_t)virt))
    panic("pg_is_mapped: Unaligned page address");

  uint32_t dir_index = (uint32_t)virt >> 22;
  uint32_t table_index = ((uint32_t)virt >> 12) & 0x3FF;
  uint32_t *tbl = (uint32_t *)(((uint32_t*) dir)[dir_index] & PG_FRAME);
  return (pg_tbl_exists(dir, virt) && (tbl[table_index] & PG_PRESENT));
}

uint32_t pg_virt_to_phys(uint32_t dir, uint32_t virt){
  if (!IS_ALIGNED((uint32_t)virt))
    panic("pg_is_mapped: Unaligned page address");

  uint32_t dir_index = (uint32_t)virt >> 22;
  uint32_t table_index = ((uint32_t)virt >> 12) & 0x3FF;
  uint32_t *tbl = (uint32_t *)(((uint32_t*) dir)[dir_index] & PG_FRAME);
  return (uint32_t)(tbl[table_index] & PG_FRAME);
}


void pg_map_pages(uint32_t dir, uint32_t virt, uint32_t phys, uint32_t num,
                  uint32_t flags) {
  
  if (!IS_ALIGNED((uint32_t)virt))
    panic("pg_map_pages: Unaligned page address (virt)");
  if (!IS_ALIGNED((uint32_t)phys))
    panic("pg_map_pages: Unaligned page address (phys)");
  //log_info(NICE_RED, "MEM", "dir:%d virt:%d phys:%d num:%d flahs:%d", dir, virt, phys, num, flags);
  uint32_t dir_index = (uint32_t)virt >> 22;

  if (!pg_tbl_exists(dir, virt)) {
    pg_mk_pgtbl(dir, dir_index, flags);
  }

  uint32_t *tbl = (uint32_t *)(((uint32_t*) dir)[dir_index] & PG_FRAME);

  for (uint32_t i = 0; i < num; i++) {
    uint32_t addr = virt + i * PG_SIZE;
    if (pg_is_mapped(dir, addr)){
      /* if it is already mapped then just change the flags          */
      /* This is just a workaround for now, I will change this later */

      // log_f(ERROR, "pg_map_pages", 
      // "Virtual address %d is already mapped to physical address %d ",
      // addr, pg_virt_to_phys(dir, addr));
      tbl[i] |= flags;
      ((uint32_t*) dir)[dir_index] |= flags;
      //panic("pg_map_pages: Tried to map an already mapped page");
    }
    tbl[i] = ((uint32_t)phys + i * PG_SIZE) | PG_PRESENT | flags;
  }
}

void pg_unmap_pages(uint32_t dir, uint32_t virt, uint32_t num) {
  uint32_t dir_index = (uint32_t)virt >> 22;

  if (!pg_tbl_exists(dir, virt)) {
    return;
  }

  uint32_t *tbl = (uint32_t *)(((uint32_t*) dir)[dir_index] & PG_FRAME);

  for (uint32_t i = 0; i < num; i++) {
    if (tbl[i] | PG_PRESENT) {
      frame_free(tbl[i] | PG_FRAME);
      tbl[i] = 0;
    }
  }
}

uint32_t pg_alloc(uint32_t virt, uint32_t flags){
  uint32_t phys = frame_alloc();
  pg_map_pages(pg_get_current_dir(), virt, phys, 1, flags);
  return phys;
}

void pg_free(uint32_t virt){
  pg_unmap_pages(pg_get_current_dir(), virt, 1);
}


void pg_init(multiboot_info_t *mbi) {
  for (int i = 0; i < 1024; i++) {
    ker_page_dir[i] = PG_RW;
  }

  pg_map_pages(pg_get_ker_dir(), 0x00000000, 0x00000000, 1024, PG_RW);
  // pg_map_pages(pg_get_ker_dir(),   0x400000,   0x400000, 1024, PG_RW);
  
  /* Map the page of the VESA framebuffer */
  if(term_get_type() != VESA_TERM){
    uint32_t page = mbi->framebuffer_addr & 0xFFF00000;
    pg_map_pages(pg_get_ker_dir(), page, page, 2*1024, PG_RW);
  }
  
  pg_map_pages(pg_get_ker_dir(), (uint32_t) HEAP_START, (uint32_t) HEAP_START, 1024, PG_RW);

  kheap = heap_create(HEAP_START, HEAP_INITIAL_SIZE, HEAP_MAX_SIZE, 0);
  pg_switch_page_dir(pg_get_ker_dir());

  uint32_t cr0;
  __asm__ __volatile__("mov %%cr0, %0" : "=r"(cr0));
  cr0 |= 0x80000000;
  cr0 &= ~((1<<29) | (1<<30));
  
  __asm__ __volatile__("mov %0, %%cr0" ::"r"(cr0));
}

static uint32_t *pg_clone_page_table(uint32_t* tbl){
  uint32_t *new_tbl = (uint32_t*) frame_alloc();
  memcpy(new_tbl, (uint32_t *)((uint32_t)tbl & PG_FRAME), 1024*sizeof(uint32_t));
  return (uint32_t *)((uint32_t) new_tbl | ((uint32_t)tbl & PG_FLAGS));
}

uint32_t pg_clone_page_dir(uint32_t* dir){
  uint32_t *new_dir = (uint32_t*) frame_alloc();
  memset(new_dir, 0, 1024*sizeof(uint32_t));

  for(int i = 0; i < 1024; i++){
    if(dir[i] & PG_PRESENT){
      new_dir[i] = (uint32_t) pg_clone_page_table((uint32_t*)dir[i]);
      new_dir[i] |= (dir[i]&PG_FLAGS);
    }  
  }
  return (uint32_t) new_dir;
}

inline uint32_t pg_make_user_page_dir(){
  uint32_t r = pg_clone_page_dir(ker_page_dir);
  return r;
}

void pg_switch_page_dir(uint32_t dir) {
  cur_page_dir = dir;
  asm volatile("mov %0, %%cr3\n" ::"r"(dir));
}

void pg_invalidate_cache() {
  asm("mov %cr3, %eax\n"
      "mov %eax, %cr3\n");
}

void pg_invalidate_page(uint32_t virt) {
  asm volatile("invlpg (%0)" ::"b"(virt) : "memory");
}

void pg_page_fault(uint32_t code) {
  term_use_color(NICE_WHITE_0);
  printk("\n PAGE FAULT !\n");
  uint32_t addr;
  asm volatile("mov %%cr2, %0" : "=r"(addr));
  printk(" at ");
  term_print_hex(addr);
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
