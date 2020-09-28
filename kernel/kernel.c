/*
 *	File : kernel.c
 *	Description : Contains the entry point for the kernel
 *
 * */


#if !defined(__i386__) || defined(__linux__)
#error "ERR : Please use a ix86-elf cross-compiler"
#endif

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include <boot/multiboot.h>
#include <kernel.h>
#include <tty/tty.h>
#include <drivers/serial.h>
#include <cpu/gdt.h>
#include <cpu/idt.h>

#include <mem/paging.h>
#include <mem/pmm.h>
//#include "../libc/string.h"

multiboot_info_t* 	global_mb_header;
uint32_t 			mb_begin_addr;
uint32_t 			mb_end_addr;
uint32_t			memory_size;

#define OK() \
	tty_use_color(VGA_GREEN, VGA_BLACK); \
	tty_print(" -- OK -- \n"); \
	tty_use_color(VGA_WHITE, VGA_BLACK);

void panic(char* err_msg){
	tty_use_color(VGA_RED, VGA_BLACK);
	tty_print(" KERNEL PANIC : ");
	tty_print(err_msg);
	while(1);
}

void kmain(uint32_t mb_magic, multiboot_info_t* mb_header){
	tty_init();
	tty_print("[*] Kernel loaded !\n");
	tty_print("\n");
	


	if(mb_magic != MULTIBOOT_BOOTLOADER_MAGIC)
		panic("Invalid multiboot magic number !");
	
	if(!(mb_header->flags & (1 << 6)))
		panic("No multiboot memory map !");
	
	global_mb_header 	= mb_header;
	mb_begin_addr 		= (uint32_t)mb_header;
	mb_end_addr 		= (uint32_t)(mb_header + sizeof(multiboot_info_t));
	memory_size		= global_mb_header->mem_upper - global_mb_header->mem_lower;

	tty_print("[*] Available memory : ");
	tty_print_hex(memory_size);
	tty_print("\n");
	
	tty_print("[@] Setting up GDT ");
	gdt_init();	
	OK();

    tty_print("[@] Setting up IDT ");
    idt_init();
    OK();
    
    //tty_print("[@] Trying int $0x3 \n");
    //asm volatile ("int $0x3");	
    //OK();

    tty_print("[@] Initializing serial port : COM1 ");
	serial_init(SERIAL_COM1);
	OK();

	tty_print("[@] Sending some data to COM1");
	serial_write(SERIAL_COM1, '*');
	OK();

	tty_print("[@] Setting up paging ..");
	pg_init();
	OK();
	
#if 0 // I was testing page faults
	uint32_t* ptr = (uint32_t*) 0x00000001;//pmalloc(sizeof(uint32_t));
	*ptr = 0x56111;
	uint32_t* ooo = (uint32_t*) 0x007FFFFF;//pmalloc(sizeof(uint32_t));
	//uint32_t* ooo = (uint32_t*) 0x00400001;//pmalloc(sizeof(uint32_t));
	tty_print_hex(*ooo);
	//uint32_t pf = *ptr;
#endif
	tty_print("Welcome to ");
	tty_use_color(VGA_MAGENTA, VGA_BLACK);
	tty_print("YuunOS !\n");
	tty_use_color(VGA_WHITE, VGA_BLACK);
	tty_print(">\n");
	
    while(1)	__asm__("hlt\n\t");    
}


