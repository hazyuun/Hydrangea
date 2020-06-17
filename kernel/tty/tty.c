/*
 *	File : tty.c
 *	Description : TODO 
 * */

#include "tty.h"
#include "../io/io.h"

#include <stddef.h>
#include <stdint.h>
struct tty_state c_tty_state;  		/* Current tty state */
uint16_t*	 tty_frame_buffer; 	/* Pointer to video memory */


void tty_init(){
	c_tty_state.row 	= 0;
	c_tty_state.column 	= 0; 
	tty_use_color(VGA_WHITE, VGA_BLACK);
	tty_frame_buffer	= (uint16_t*) 0xB8000;
	tty_cur_mov(0,0);
	for(size_t k = 0; k < (size_t)(VGA_WIDTH * VGA_HEIGHT); k++)
		*(tty_frame_buffer + k) = vga_char(' ', c_tty_state.color);  
}

void tty_use_color(vga_color fg, vga_color bg){
	c_tty_state.color = vga_color_byte(fg, bg);
}

void tty_cur_mov(size_t x, size_t y){
	uint16_t index = x + (uint16_t)VGA_WIDTH * y;
	io_outb(0x3D4, 0x0F);
	io_outb(0x3D5, (uint8_t) (index & 0xFF));
	io_outb(0x3D4, 0x0E);
	io_outb(0x3D5, (uint8_t) ((index >> 8) & 0xFF));
}

uint16_t tty_cur_pos(){
	uint16_t cur_pos = 0x0;
	io_outb(0x3D4, 0x0F);
	cur_pos |= io_inb(0x3D5);
	io_outb(0x3D4, 0x0E);
	cur_pos |= ((uint16_t)io_inb(0x3D5)) << 8;
	return cur_pos;
}
void tty_cur_stp(){
	tty_cur_mov(
		c_tty_state.column,
		c_tty_state.row
		);
}

void tty_scroll(){
	for(size_t i = 0; i < VGA_HEIGHT ;i++){
		for(size_t j = 0; j < VGA_WIDTH; j++){
			tty_frame_buffer[i*VGA_WIDTH+j] = tty_frame_buffer[(i+1)*VGA_WIDTH+j];
		}
	}
}

void tty_putat(unsigned char c, size_t x, size_t y){
	tty_frame_buffer[x+y*(size_t)(VGA_WIDTH)] = vga_char(c, c_tty_state.color);
	tty_cur_stp();
}

void tty_putchar(unsigned char c){
	if(c == '\n'){
		c_tty_state.column = 0;
		c_tty_state.row++;
		if(c_tty_state.row == VGA_HEIGHT){
		 	c_tty_state.row -= 1;
			tty_scroll();	
		}
		tty_cur_stp();
	}else{
		if(c_tty_state.column+1 == VGA_WIDTH){
			c_tty_state.column = 0;
			c_tty_state.row++;
			if(c_tty_state.row == VGA_HEIGHT){
				c_tty_state.row -= 1;
				tty_scroll();	
			}
		}
		tty_putat(c, c_tty_state.column++, c_tty_state.row);
	}
}	


void tty_print(const char* string){
	size_t 	index = 0;
	char	c;
	while((c = *(string+index++)))
		tty_putchar(c);
}

