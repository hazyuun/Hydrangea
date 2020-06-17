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
#include "io/io.h"

//#include "../libc/string.h"

void kmain(void){
	tty_init();
	tty_print("Welcome to ");
	tty_use_color(VGA_GREEN, VGA_BLACK);
	tty_print("YuunOS !");
	//tty_scroll();
	for(char i = 'a'; i<'a'+24; i++){
		tty_putchar(i);
		tty_print("\n");
	}
	tty_use_color(VGA_MAGENTA, VGA_BLACK);
	tty_print("testing\nscroll\n");
	tty_print("Yatta~ ^^");
}

