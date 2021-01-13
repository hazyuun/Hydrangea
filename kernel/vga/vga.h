/*
 * 	File : vga.h
 *
 *	Description : TODO
 *
 * */

#ifndef _VGA_H_
#define _VGA_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

typedef enum VGA_COLOR {
  VGA_BLACK = 0x0,
  VGA_BLUE = 0x1,
  VGA_GREEN = 0x2,
  VGA_CYAN = 0x3,
  VGA_RED = 0x4,
  VGA_MAGENTA = 0x5,
  VGA_BROWN = 0x6,
  VGA_LIGHT_GREY = 0x7,
  VGA_DARK_GREY = 0x8,
  VGA_LIGHT_BLUE = 0x9,
  VGA_LIGHT_GREEN = 0xA,
  VGA_LIGHT_CYAN = 0xB,
  VGA_LIGHT_RED = 0xC,
  VGA_LIGHT_MAGENTA = 0xD,
  VGA_LIGHT_BROWN = 0xE,
  VGA_WHITE = 0xF
} vga_color;

uint8_t vga_color_byte(vga_color fg, vga_color bg);
uint16_t vga_char(unsigned char c, uint8_t color);

#endif
