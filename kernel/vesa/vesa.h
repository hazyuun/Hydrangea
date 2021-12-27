/*
 *	File  vesa.h
 *	Description  TODO
 * */

#ifndef _VESA_H_
#define _VESA_H_

#include <boot/multiboot.h>

#include <stddef.h>
#include <stdint.h>
#include <vesa/nice_colors.h>

void vesa_init(multiboot_info_t *mbi);
void vesa_put_pixel(uint32_t x, uint32_t y, uint32_t color);
void vesa_put_char(uint32_t x, uint32_t y, uint32_t color, char c);
void vesa_write(uint32_t offset, uint32_t *buffer, size_t size);
void vesa_clear();
void vesa_swap_buffers();
uint32_t *vesa_get_framebuffer();
uint32_t vesa_get_framebuffer_width();
uint32_t vesa_get_framebuffer_height();
uint32_t vesa_get_framebuffer_pitch();
uint32_t vesa_get_framebuffer_bpp();


#endif