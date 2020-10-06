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

#include <kernel.h>

#include <boot/multiboot.h>
#include <tty/tty.h>

#include <drivers/serial.h>
#include <drivers/kbd.h>
#include <drivers/rtc.h>

#include <cpu/gdt.h>
#include <cpu/idt.h>
#include <cpu/pit.h>

#include <mem/paging.h>
#include <mem/pmm.h>
#include <mem/heap.h>

#include <string.h>
#include <stdio.h>

multiboot_info_t* 	global_mb_header;
uint32_t 			mb_begin_addr;
uint32_t 			mb_end_addr;
uint64_t			memory_size;

#define OK() \
	tty_use_color(VGA_GREEN, VGA_BLACK); \
	tty_print(" < OK > "); \
	tty_use_color(VGA_WHITE, VGA_BLACK);

void panic(char* err_msg){
	tty_use_color(VGA_RED, VGA_BLACK);
	tty_print(" KERNEL PANIC : ");
	tty_print(err_msg);
	while(1);
}

void kmain(uint32_t mb_magic, multiboot_info_t* mb_header){
	tty_init();

	printk("[*] Kernel loaded !\n");


	if(mb_magic != MULTIBOOT_BOOTLOADER_MAGIC)
		panic("Invalid multiboot magic number !");
	
	if(!(mb_header->flags & (1 << 6)))
		panic("No multiboot memory map !");
	
	global_mb_header 	= mb_header;
	mb_begin_addr 		= (uint32_t)mb_header;
	mb_end_addr 		= (uint32_t)(mb_header + sizeof(multiboot_info_t));
	memory_size		= 0;

	multiboot_memory_map_t *mmap;
	
	for (mmap = (multiboot_memory_map_t *) global_mb_header->mmap_addr;
		(unsigned long) mmap < global_mb_header->mmap_addr + global_mb_header->mmap_length;
		mmap = (multiboot_memory_map_t *) ((unsigned long) mmap + mmap->size + sizeof (mmap->size)))
	{
		if(mmap->type & MULTIBOOT_MEMORY_AVAILABLE) memory_size += (((uint64_t)(mmap->len_hi) << 8)|(mmap->len_lo));
	}
	
	printk("[*] Available memory : %d KiB\n", (memory_size/1024));

	gdt_init();	
	OK(); printk("GDT set up \n");
	
	idt_init();
	OK(); printk("IDT set up \n");

	
	serial_init(SERIAL_COM1);
	OK(); printk("Serial port COM1 initialized \n");

	serial_init(SERIAL_COM1);
	OK(); printk("Initializing serial port : COM1 \n");
	
	pg_init();
	OK(); printk("Paging set up \n");

	pit_init(100);
	OK(); printk("PIT set up \n");

	printk("Welcome to ");
	tty_use_color(VGA_MAGENTA, VGA_BLACK);
	printk("YuunOS !\n");
	tty_use_color(VGA_WHITE, VGA_BLACK);

	
	/* This is a quick and dirty and temporary cli */
	/* just for the sake of testing ! */
	char cmd[100];
	while(1){
		tty_use_color(VGA_LIGHT_BLUE, VGA_BLACK);
		printk("\nKernel>");
		tty_use_color(VGA_WHITE, VGA_BLACK);
		scank("%s", cmd);
		if(!strcmp("info", cmd)){
			tty_print("YuunOS v0.0.1\n");
		}
		else if(!strcmp("clear", cmd)){
			tty_clear();
		}
		else if(!strcmp("reboot", cmd)){
			uint8_t TW = 0x02;
			while(TW & 0x02)
				TW = io_inb(0x64);
			io_outb(0x64, 0xFE);
			while(1);
		}
		else if(!strcmp("kbd fr", cmd)){
			kbd_switch_layout("fr");
			printk("Keyboard layout changed to : FR\nEnjoy your baguette !\n");
		}
		else if(!strcmp("kbd en", cmd)){
			kbd_switch_layout("en");
			printk("Keyboard layout changed to : EN\n");
		}
		else if(!strcmp("sleep", cmd)){ /* Will sleep 10 secs */ 
			pit_sleep(10* 100);
		}
		else if(!strcmp("datetime", cmd)){ 
			rtc_print_now();
		}
		else if(!strcmp("", cmd)){}
		else printk("Unknown command\n");
	}
	while(1)	__asm__("hlt\n\t");    
}


