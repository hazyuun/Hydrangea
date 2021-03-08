/*
 *	File : vesa.c
 *	Description : TODO
 * */

#include <boot/multiboot.h>
#include <mem/paging.h>
#include <stddef.h>
#include <string.h>
#include <vesa/vesa.h>

uint32_t *vesa_fb;
uint32_t vesa_width, vesa_height, vesa_pitch, vesa_bpp;

void vesa_init(multiboot_info_t *mbi) {
  vesa_fb = (uint32_t *) ((uint32_t) (mbi->framebuffer_addr));
  vesa_width = mbi->framebuffer_width;
  vesa_height = mbi->framebuffer_height;
  vesa_pitch = mbi->framebuffer_pitch;
  vesa_bpp = mbi->framebuffer_bpp;
}

void vesa_write(uint32_t offset, uint32_t *buffer, size_t size) {
  memcpy(vesa_fb + offset, buffer, size);
}

void vesa_clear() {
  for (int i = 0; i < 1024; i++) {
    for (int j = 0; j < 768; j++) {
      vesa_put_pixel(i, j, NICE_BG);
    }
  }
}

void vesa_put_pixel(uint32_t x, uint32_t y, uint32_t color) {
  uint32_t offset = y * vesa_width + x;
  vesa_fb[offset] = color;
}

uint32_t *vesa_get_framebuffer() { return vesa_fb; }
