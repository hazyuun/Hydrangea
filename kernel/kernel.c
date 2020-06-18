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

//#include "../libc/string.h"

void kmain(void){
	tty_init();
	tty_print("[*] Kernel loaded\n");
	tty_print("[@] Initializing serial port : COM1\n");
	serial_init(SERIAL_COM1);
	tty_print("[*] Initialized !\n");
	tty_print("[@] Sending some data to COM1\n");
	serial_write(SERIAL_COM1, '*');
	
	tty_print("Welcome to ");
	tty_use_color(VGA_GREEN, VGA_BLACK);
	tty_print("YuunOS !\n");
	tty_use_color(VGA_WHITE, VGA_BLACK);
	tty_print(">");

}


