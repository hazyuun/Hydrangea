#include <kernel.h>
#include <mem/heap.h>
#include <mem/paging.h>
#include <mem/pmm.h>
#include <util/logger.h>

#include <stdio.h>
#include <string.h>

extern uint32_t end_of_kernel;
volatile size_t placement_addr = -0xC0000000 + (size_t)&end_of_kernel;

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
 * memory
 *
 * EDIT : Hey me ! Where tf are you gonna use this function ?
 * TODO : Find useless stuff and delete it
 */
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
  memory_size = 0;

  multiboot_memory_map_t *mmap;
  mmap = (multiboot_memory_map_t *)(mbi->mmap_addr);

  /* Initialize the bitmap */
  frames_bmp = (uint32_t *)pmalloc(((0xffffffff / 4096) / 32));
  memset(frames_bmp, 0, ((0xffffffff / 4096) / 32));

  for (uint32_t i = 0; i < mbi->mmap_length;
       i += sizeof(multiboot_memory_map_t)) {
    mmap = (multiboot_memory_map_t *)(mbi->mmap_addr + i);

    /* No 64-bit addresses */
    if (mmap->addr_hi)
      continue;

    if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE) {
      memory_size += mmap->len_lo;
    } else {
      uint32_t frames = mmap->len_lo / 4096;

      for (uint32_t frame = 0; frame < frames; frame++)
        frame_bmp_set(mmap->addr_lo + frame * 4096);
    }
  }

  uint32_t kernel_size = (uint32_t)&end_of_kernel;
  kernel_size -= 1024 * 1024; /* because the kernel starts at 1 MiB */
  kernel_size -= 0xC0000000;
  
  uint32_t kernel_frames = kernel_size / 4096;

  /* TODO : Maybe extract this loop in its own function */
  for (uint32_t frame = 0; frame < kernel_frames; frame++)
    frame_bmp_set(mmap->addr_lo + frame * 4096);

  frame_alloc();

  // log_f(INFO, "mem", "Kernel size : %d", kernel_size);
  // log_f(INFO, "mem", "Frames %d", kernel_frames);
}

uint32_t pmm_available_memory() { return memory_size / 1024; }

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
    if (frames_bmp[i] == 0xFFFFFFFF)
      continue;

    for (uint32_t j = 0; j < 32; j++) {
      if (frames_bmp[i] & (1 << j))
        continue;
      return 32 * i + j;
    }
  }
  log_f(ERROR, "MEM", "Physical memory manager run out of free frames");
  panic("No free frames !");
  return -1;
}

uint32_t frame_alloc() {
  uint32_t ff = frame_bmp_get_first();
  frame_bmp_set(ff * 0x1000);
  return ff * 0x1000;
}

void frame_free(uint32_t addr) { frame_bmp_reset(addr); }
