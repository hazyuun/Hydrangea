/*
 *	File : pmm.c
 *	Description : TODO
 * */

#include <mem/pmm.h>
#include <mem/paging.h>
#include <mem/heap.h>
#include <kernel.h>
#include <tty/tty.h>

extern uint32_t end_of_bin_addr;
extern uint32_t memory_size;
volatile size_t placement_addr = (size_t) &end_of_bin_addr;
extern heap_t* kheap;
uint32_t* frames_bmp;

/* Placement malloc */
void* pmalloc(size_t size){
     placement_addr += size;
     return (void*) (placement_addr - size);
}


/* Placement malloc with alignment */
void* pmalloc_a(size_t size, size_t align){
     if (align && (placement_addr & 0xFFF)){
          placement_addr &= 0xFFFFF000;
          placement_addr += 0x1000;
     }
     return pmalloc(size);
}

/* Placement malloc with alignment that also gives physical address of allocated memory */
void* pmalloc_ap(size_t size, size_t align, size_t* physical_addr){
     if (align && (placement_addr & 0xFFF)){
          placement_addr &= 0xFFFFF000;
          placement_addr += 0x1000;
     }
     if(physical_addr) *physical_addr = placement_addr;
     return pmalloc(size);
}

void frame_bmp_set(uint32_t addr){
     uint32_t frame_nbr = addr / 4096;
     frames_bmp[frame_nbr / 32] |= (1 << (frame_nbr % 32));
}
void frame_bmp_reset(uint32_t addr){
     uint32_t frame_nbr = addr / 4096;
     frames_bmp[frame_nbr / 32] &= ~(1 << (frame_nbr % 32));
}
uint32_t frame_bmp_test(uint32_t addr){
     uint32_t frame_nbr = addr / 4096;
     return (frames_bmp[frame_nbr / 32] & (1 << (frame_nbr % 32)));
}
uint32_t frame_bmp_get_first(){
     for(uint32_t i = 0; i < (memory_size/4096)/32; i++){
          if(frames_bmp[i] != 0xFFFFFFFF){
               for(uint32_t j = 0; j < 32; j++){
                    if (!(frames_bmp[i] & (1<<j)) ){
                         return 32*i + j;
                    }
               }
          }
     }
     panic("No free frames !");
     return -1;
}

void* frame_alloc(){
     uint32_t ff = frame_bmp_get_first();
     frame_bmp_set(ff);
     return (void*)ff;
}

void frame_free(void* addr){
     frame_bmp_reset((uint32_t)addr);
}
