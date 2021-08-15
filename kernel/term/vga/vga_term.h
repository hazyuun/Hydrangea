#ifndef _VGA_TERM_H_
#define _VGA_TERM_H_

#include <stddef.h>
#include <stdint.h>
#include <vga/vga.h>

struct vga_term_state {
  uint16_t color;
  size_t row;
  size_t column;
};

void vga_term_init();
void vga_term_use_vesa_color(uint32_t color);
void vga_term_use_color(vga_color fg, vga_color bg);

void vga_term_cur_mov(size_t x, size_t y);
uint16_t vga_term_cur_pos();
void vga_term_cur_step();

void vga_term_scroll();
void vga_term_clear();
void vga_term_fill(vga_color color);

void vga_term_putchar(unsigned char c);
void vga_term_putat(unsigned char c, size_t x, size_t y);
void vga_term_print(const char *string);
void vga_term_print_hex(uint32_t x);
#endif
