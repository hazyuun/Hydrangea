#include <mem/paging.h>
#include <mem/pmm.h>
#include <tty/tty.h>
#include <string.h>
#include <kernel.h>

extern uint32_t* frames_bmp;
extern uint32_t memory_size;
extern size_t placement_addr;

uint32_t* ker_page_dir;

uint32_t* ker_page_table;


void pg_init(){
     ker_page_dir = (uint32_t*) pmalloc_a(1024 *sizeof(uint32_t),1);
     ker_page_table = (uint32_t*) pmalloc_a(1024 *sizeof(uint32_t),1);
     
     for(int i = 0; i < 1024; i++){
          ker_page_dir[i] = PG_READ_WRITE;
          ker_page_table[i] = (i*0x1000) | PG_PRESENT | PG_READ_WRITE;
     }
     ker_page_dir[0] = ((uint32_t) ker_page_table) | PG_PRESENT | PG_READ_WRITE;
     ker_page_dir[1] = ((uint32_t) ker_page_table) | PG_PRESENT | PG_READ_WRITE;
     
     pg_switch_page_dir(ker_page_dir);
     
     uint32_t cr0;
     __asm__  __volatile__("mov %%cr0, %0": "=r"(cr0));
     cr0 |= 0x80000000;
     __asm__  __volatile__("mov %0, %%cr0":: "r"(cr0));
}
void pg_switch_page_dir(uint32_t* page_dir){
     __asm__  __volatile__("mov %0, %%cr3":: "r"(page_dir));
}

void pg_page_fault(uint8_t code){
     tty_use_color(VGA_LIGHT_RED, VGA_BLACK);
     tty_print("\n PAGE FAULT !\n");
     uint32_t addr;
     asm volatile("mov %%cr2, %0": "=r"(addr));
     tty_print(" at "); tty_print_hex(addr);
     tty_print("\n\n"); 
     int present    = !(code & 0x1); // Page not present
     int write      = code & 0x2;           // Write operation?
     int user         = code & 0x4;           // Processor was in user-mode?
     int reserved   = code & 0x8;     // Overwritten CPU-reserved bits of page entry?
     int instr         = code & 0x10;
     tty_print("    Present               : "); tty_print(present?"true\n":"false\n");
     tty_print("    Write?                : "); tty_print(write?"true\n":"false\n");
     tty_print("    User mode             : "); tty_print(user?"true\n":"false\n");
     tty_print("    Reserved              : "); tty_print(reserved?"true\n":"false\n");
     tty_print("    Instruction fetch ?   : "); tty_print(instr?"true\n":"false\n");
     /* Seriously, I need a printf */
}
