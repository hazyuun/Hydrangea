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

#include "tty/tty.h"
#include "drivers/serial.h"
#include "cpu/gdt.h"
#include "cpu/idt.h"
//#include "../libc/string.h"

#define OK() \
	tty_use_color(VGA_GREEN, VGA_BLACK); \
	tty_print(" -- OK -- \n"); \
	tty_use_color(VGA_WHITE, VGA_BLACK);

void kmain(void){
	tty_init();
	tty_print("[*] Kernel loaded !\n");
	
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
    while(1);    
}


