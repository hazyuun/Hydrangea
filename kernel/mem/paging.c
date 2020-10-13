#include <mem/paging.h>
#include <mem/pmm.h>
#include <mem/heap.h>
#include <tty/tty.h>
#include <string.h>
#include <kernel.h>
#include <stdio.h>

extern uint32_t* frames_bmp;
extern uint32_t memory_size;
extern size_t placement_addr;

uint32_t* ker_page_dir;

uint32_t* ker_page_table;

uint32_t* ker_hp_page_table;
heap_t* kheap;

void pg_init(){
     ker_page_dir = (uint32_t*) pmalloc_a(1024 *sizeof(uint32_t),1);
     ker_page_table = (uint32_t*) pmalloc_a(1024 *sizeof(uint32_t),1);
     ker_hp_page_table = (uint32_t*) pmalloc_a(1024 *sizeof(uint32_t),1);
     
     for(int i = 0; i < 1024; i++){
          ker_page_dir[i] = PG_READ_WRITE;
          ker_page_table[i] = (i*0x1000) | PG_PRESENT | PG_READ_WRITE;
     }
     /*
     for (i = HEAP_START; i < HEAP_START+HEAP_INITIAL_SIZE; i += PG_SIZE)
          pg_get_page(i, 1);

     for (i = HEAP_START; i < HEAP_START+HEAP_INITIAL_SIZE; i += PG_SIZE)
          frame_alloc(pg_get_page(i, 1), 0, 0);
     */
     ker_page_dir[0] = ((uint32_t) ker_page_table) | PG_PRESENT | PG_READ_WRITE;

     /*for(int i = 0; i < 1024; i++){
          ker_hp_page_table[i] = (0xC0000000 + i*0x1000) | PG_PRESENT | PG_READ_WRITE;
     }
     ker_page_dir[0x300] = ((uint32_t) ker_hp_page_table) | PG_PRESENT | PG_READ_WRITE;
     */
     pg_switch_page_dir(ker_page_dir);
     //kheap = heap_create((void*)HEAP_START, HEAP_INITIAL_SIZE, HEAP_MAX_SIZE, 0);
     
     uint32_t cr0;
     __asm__  __volatile__("mov %%cr0, %0": "=r"(cr0));
     cr0 |= 0x80000000;
     __asm__  __volatile__("mov %0, %%cr0":: "r"(cr0));
}

void pg_imap_page(void* addr, uint32_t flags){
     pg_map_page(addr, addr, flags);
}
void pg_map_page(void* addr, void* phy, uint32_t flags){
     uint32_t* pgtbl = (uint32_t*) pmalloc_a(1024 * sizeof(uint32_t), 1);
     for(int i = 0; i < 1024; i++){
          pgtbl[i] = ((uint32_t)phy + i*0x1000) | PG_PRESENT | flags;
     }
     ker_page_dir[(uint32_t)addr/(1024 * 1024 * 4)] = ((uint32_t) pgtbl) | PG_PRESENT | flags;

     pg_invalidate((uint32_t)addr);
}

void pg_unmap_page(void* virt){
     uint32_t* pgtbl = (uint32_t*)VIRT_TO_PG_TBL(((uint32_t)virt));
     for(int i = 0; i < 1024; i++){
          pgtbl[i] = ((uint32_t)virt + i*0x1000) & ~(PG_PRESENT|PG_READ_WRITE);
     }
     ker_page_dir[(uint32_t)virt/(1024 * 1024 * 4)] = ((uint32_t) pgtbl) & ~(PG_PRESENT|PG_READ_WRITE);

     frame_free((void*)((uint32_t)virt & PG_FRAME));    
}

void pg_switch_page_dir(uint32_t* page_dir){
     __asm__  __volatile__("mov %0, %%cr3":: "r"(page_dir));
}

void* pg_get_page(uint32_t addr, uint8_t make, uint32_t* dir){
     if(addr % 0x1000) panic("Attempt to get a page with non aligned address");
     
     uint32_t di = PG_DIR_INDEX(addr);
     uint32_t ti = PG_TBL_INDEX(addr);
     
     if(dir[di] & PG_PRESENT) 
          return (void*)(   (uint32_t*)(*((uint32_t*)dir + di)) + ti   );
     if(make){
          uint32_t* new_table = (uint32_t*) frame_alloc();
		dir[di] = (uint32_t)new_table | PG_PRESENT | PG_READ_WRITE; 
		memset( new_table, 0, 4096);
          return (void*)(   (uint32_t*)(*((uint32_t*)dir + di)) + ti   );

     }

	return NULL;
}

void pg_invalidate(uintptr_t virt) {
	asm volatile ("invlpg (%0)" :: "b"(virt) : "memory");
}

void pg_page_fault(uint8_t code){
     tty_use_color(VGA_LIGHT_RED, VGA_BLACK);
     printk("\n PAGE FAULT !\n");
     uint32_t addr;
     asm volatile("mov %%cr2, %0": "=r"(addr));
     printk(" at ");
     tty_print_hex(addr);
     printk("\n\n");

     int present    = !(code & 0x1);
     int write      = code & 0x2;
     int user       = code & 0x4;
     int reserved   = code & 0x8;
     int instr      = code & 0x10;
     printk("\t Present               : %s\n", (present?"true":"false"));
     printk("\t Write?                : %s\n", (write?"true":"false"));
     printk("\t User mode             : %s\n", (user?"true":"false"));
     printk("\t Reserved              : %s\n", (reserved?"true":"false"));
     printk("\t Instruction fetch ?   : %s\n", (instr?"true":"false"));
}
