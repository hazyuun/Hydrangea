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

#include "../boot/multiboot.h"

#include "tty/tty.h"
#include "drivers/serial.h"
#include "cpu/gdt.h"
#include "cpu/idt.h"
//#include "../libc/string.h"

multiboot_info_t* 	global_mb_header;
uint32_t 			mb_begin_addr;
uint32_t 			mb_end_addr;

#define OK() \
	tty_use_color(VGA_GREEN, VGA_BLACK); \
	tty_print(" -- OK -- \n"); \
	tty_use_color(VGA_WHITE, VGA_BLACK);

void panic(char* err_msg){
	tty_fill(VGA_LIGHT_BROWN);
	tty_use_color(VGA_RED, VGA_LIGHT_BROWN);
	tty_cur_mov(2, 1);
	tty_print(" KERNEL PANIC : ");
	tty_print(err_msg);
	while(1);
}

void kmain(uint32_t mb_magic, multiboot_info_t* mb_header){
	tty_init();
	tty_print("[*] Kernel loaded !\n");

	if(mb_magic != MULTIBOOT_BOOTLOADER_MAGIC)
		panic("Invalid multiboot magic number !");
	
	if(!(mb_header->flags & (1 << 6)))
		panic("No multiboot memory map !");
	
	global_mb_header 	= mb_header;
	mb_begin_addr 		= (uint32_t)mb_header;
	mb_end_addr 		= (uint32_t)(mb_header + sizeof(multiboot_info_t));


	tty_print("[*] Available memory : ");
	tty_print_hex(global_mb_header->mem_upper - global_mb_header->mem_lower);
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

	tty_print("Welcome to ");
	tty_use_color(VGA_MAGENTA, VGA_BLACK);
	tty_print("YuunOS !\n");
	tty_use_color(VGA_WHITE, VGA_BLACK);
    tty_print(">");
    while(1) __asm__("hlt\n\t");    
}


