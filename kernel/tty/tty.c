/*
 *	File : tty.c
 *	Description : TODO 
 * */

#include "tty.h"
#include <stddef.h>
#include <stdint.h>
struct tty_state c_tty_state;  		/* Current tty state */
uint16_t*	 tty_frame_buffer; 	/* Pointer to video memory */


void tty_init(){
	c_tty_state.row 	= 0;
	c_tty_state.column 	= 0; 
	tty_use_color(VGA_WHITE, VGA_BLACK);
	tty_frame_buffer	= (uint16_t*) 0xB8000;
	for(size_t k = 0; k < (size_t)(VGA_WIDTH * VGA_HEIGHT); k++)
		*(tty_frame_buffer + k) = vga_char(' ', c_tty_state.color);  
}

void tty_use_color(vga_color fg, vga_color bg){
	c_tty_state.color = vga_color_byte(fg, bg);
}

void tty_putat(unsigned char c, size_t x, size_t y){
	tty_frame_buffer[x+y*(size_t)(VGA_WIDTH)] = vga_char(c, c_tty_state.color);
}

void tty_putchar(unsigned char c){
	if(c == '\n') c_tty_state.row++; /* TODO: implement scrolling*/
	else{
		tty_putat(c, c_tty_state.column++, c_tty_state.row);
		if(c_tty_state.column == VGA_WIDTH){
			c_tty_state.column = 0;
			if(++c_tty_state.row == VGA_HEIGHT)
				c_tty_state.row = 0;	
		}
	}
}	


void tty_print(const char* string){
	size_t 	index = 0;
	char	c;
	while((c = *(string+index++)))
		tty_putchar(c);
}

