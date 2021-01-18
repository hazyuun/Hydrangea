/*
 *	File : pmm.c
 *	Description : TODO
 * */

#include <kernel.h>
#include <mem/heap.h>
#include <mem/paging.h>
#include <mem/pmm.h>
#include <stdio.h>
#include <string.h>
#include <tty/tty.h>

extern uint32_t end_of_bin_addr;

volatile size_t placement_addr = (size_t)&end_of_bin_addr;

extern heap_t *kheap;

uint32_t *frames_bmp;

uint64_t memory_size;

/* Placement malloc */
void *pmalloc(size_t size) {
  placement_addr += size;
  return (void *)(placement_addr - size);
}

/* Placement malloc with alignment */
void *pmalloc_a(size_t size, size_t align) {
  if (align && (placement_addr & 0xFFF)) {
    placement_addr &= 0xFFFFF000;
    placement_addr += 0x1000;
  }
  return pmalloc(size);
}

/* Placement malloc with alignment that also gives physical address of allocated
 * memory */
void *pmalloc_ap(size_t size, size_t align, size_t *physical_addr) {
  if (align && (placement_addr & 0xFFF)) {
    placement_addr &= 0xFFFFF000;
    placement_addr += 0x1000;
  }
  if (physical_addr)
    *physical_addr = placement_addr;
  return pmalloc(size);
}

void pmm_init(multiboot_info_t *mbi) {

  // uint32_t mb_begin_addr;
  // uint32_t mb_end_addr;
  
  // mb_begin_addr = (uint32_t)mbi;
  // mb_end_addr = (uint32_t)(mbi + sizeof(multiboot_info_t));
  memory_size = 0;

  multiboot_memory_map_t *mmap;

  for (mmap = (multiboot_memory_map_t *)mbi->mmap_addr;
       (unsigned long)mmap < mbi->mmap_addr + mbi->mmap_length;
       mmap = (multiboot_memory_map_t *)((unsigned long)mmap + mmap->size +
                                         sizeof(mmap->size))) {
    if (mmap->type & MULTIBOOT_MEMORY_AVAILABLE)
      memory_size += (uint64_t)(mmap->len);
   
  }

  frames_bmp = (uint32_t *)pmalloc(((memory_size / 4096) / 32));
  memset(frames_bmp, 0, ((memory_size / 4096) / 32));
  frame_alloc();
  for (mmap = (multiboot_memory_map_t *)mbi->mmap_addr;
       (unsigned long)mmap < mbi->mmap_addr + mbi->mmap_length;
       mmap = (multiboot_memory_map_t *)((unsigned long)mmap + mmap->size +
                                         sizeof(mmap->size))) {
    if (!(mmap->type & MULTIBOOT_MEMORY_AVAILABLE))
      frame_bmp_set(mmap->addr);
   
  }
}

uint32_t pmm_available_memory() {
  return memory_size / 1024;
}

void frame_bmp_set(uint32_t addr) {
  uint32_t frame_nbr = addr / 4096;
  frames_bmp[frame_nbr / 32] |= (1 << (frame_nbr % 32));
}
void frame_bmp_reset(uint32_t addr) {
  uint32_t frame_nbr = addr / 4096;
  frames_bmp[frame_nbr / 32] &= ~(1 << (frame_nbr % 32));
}
uint32_t frame_bmp_test(uint32_t addr) {
  uint32_t frame_nbr = addr / 4096;
  return (frames_bmp[frame_nbr / 32] & (1 << (frame_nbr % 32)));
}
uint32_t frame_bmp_get_first() {
  for (uint32_t i = 0; i < (memory_size / 4096) / 32; i++) {
    if (frames_bmp[i] != 0xFFFFFFFF) {
      for (uint32_t j = 0; j < 32; j++) {
        if (!(frames_bmp[i] & (1 << j))) {
          return 32 * i + j;
        }
      }
    }
  }
  panic("No free frames !");
  return -1;
}

uint32_t frame_alloc() {
  uint32_t ff = frame_bmp_get_first();
  frame_bmp_set(ff * 0x1000);
  return ff * 0x1000;
}

void frame_free(uint32_t addr) { frame_bmp_reset(addr); }
