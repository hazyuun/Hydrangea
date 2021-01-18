/*
 *	File  vesa.h
 *	Description  TODO
 * */

#ifndef _VESA_H_
#define _VESA_H_

#include <boot/multiboot.h>

#include <stddef.h>
#include <stdint.h>

/* Some nice colors */


#define NICE_FG 0x1d2342
#define NICE_BG 0x0A050F
#define NICE_CUR 0xffffff

#define NICE_BLACK_0 0x686868
#define NICE_BLACK 0x101116

#define NICE_RED_0 0xff6e67
#define NICE_RED 0xff5680

#define NICE_GREEN_0 0x5ffa68
#define NICE_GREEN 0x00ff9c

#define NICE_YELLOW_0 0xfffc67
#define NICE_YELLOW 0xfffc58

#define NICE_BLUE_0 0x6871ff
#define NICE_BLUE 0x00b0ff

#define NICE_MAGENTA_0 0xd682ec
#define NICE_MAGENTA 0xd57bff

#define NICE_CYAN_0 0x60fdff
#define NICE_CYAN 0x76c1ff

#define NICE_WHITE_0 0xffffff
#define NICE_WHITE 0xc7c7c7

void vesa_init(multiboot_info_t *mbi);
void vesa_put_pixel(uint32_t x, uint32_t y, uint32_t color);
void vesa_put_char(uint32_t x, uint32_t y, uint32_t color, char c);
void vesa_write(uint32_t offset, uint32_t *buffer, size_t size);
void vesa_clear();
uint32_t *vesa_get_framebuffer();

#endif