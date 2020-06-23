/*
 *	File : tty.h
 *	Description : TODO
 * */


#ifndef _TTY_H_
#define _TTY_H_

#include "../vga/vga.h"
#include <stdint.h>
#include <stddef.h>

struct tty_state{
	uint16_t	color;
	size_t		row;
	size_t		column;
};

void 	 tty_init();
void 	 tty_use_color(vga_color fg, vga_color bg);

void 	 tty_cur_mov(size_t x, size_t y);
uint16_t tty_cur_pos();
void 	 tty_cur_stp();

void	 tty_scroll();

void 	 tty_putchar(unsigned char c);
void 	 tty_putat(unsigned char c, size_t x, size_t y);
void 	 tty_print(const char* string);
void     tty_print_hex(uint32_t x);
#endif
