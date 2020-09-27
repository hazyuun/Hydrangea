/*
 *	File : vga.c
 *	Description : TODO
 * */

#include <vga/vga.h>

uint8_t vga_color_byte(vga_color fg, vga_color bg){
	return (bg << 4) | fg;
}

uint16_t vga_char(unsigned char c, uint8_t color){
	return (uint16_t)(color << 8) | (uint16_t) c;
}


