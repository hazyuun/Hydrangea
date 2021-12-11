/*
 *	File : vesa.c
 *	Description : TODO
 * */

#include <boot/multiboot.h>
#include <mem/paging.h>
#include <stddef.h>
#include <string.h>
#include <vesa/vesa.h>
#include <mem/heap.h>

uint8_t *vesa_fb;
uint8_t *vesa_back_buffer;

uint32_t vesa_width, vesa_height, vesa_pitch, vesa_bpp;
#include <drivers/serial.h>
void vesa_init(multiboot_info_t *mbi) {

  vesa_fb = (uint8_t *) ((uint32_t) (mbi->framebuffer_addr));
  
  vesa_width = mbi->framebuffer_width;
  vesa_height = mbi->framebuffer_height;
  vesa_pitch = mbi->framebuffer_pitch;
  
  vesa_bpp = mbi->framebuffer_bpp;
  vesa_back_buffer = (uint8_t*) kmalloc(vesa_pitch * vesa_bpp);
}

void vesa_swap_buffers(){
  uint32_t *a = (uint32_t *) vesa_fb;

  /* TODO : optimize memcpy */
  for(uint32_t i = 0; i < vesa_height * vesa_width; i++) {
    if(a[i] != ((uint32_t *)vesa_back_buffer)[i])
      a[i] = ((uint32_t *)vesa_back_buffer)[i]; 
  }
  
}

void vesa_write(uint32_t offset, uint32_t *buffer, size_t size) {
  memcpy(vesa_fb + offset, buffer, size);
}

void vesa_clear() {
  for(uint32_t i = 0; i < vesa_height * vesa_width; i++) {
    ((uint32_t *)vesa_fb)[i] = 0x0; 
  }
  //vesa_swap_buffers();
}

void vesa_put_pixel(uint32_t x, uint32_t y, uint32_t color) {
  uint8_t *p = vesa_fb + y * vesa_pitch + x * vesa_bpp/8;
  *((uint32_t *)(p)) = color;
}


void vesa_back_buffer_clear() {
  for(uint32_t i = 0; i < vesa_height * vesa_width; i++) {
    ((uint32_t *)vesa_back_buffer)[i] = 0x0; 
  }
}

void vesa_back_buffer_put_pixel(uint32_t x, uint32_t y, uint32_t color) {
  uint8_t *p = vesa_back_buffer + y * vesa_pitch + x * vesa_bpp/8;
  *((uint32_t *)(p)) = color;
}

uint32_t *vesa_get_framebuffer() { return (uint32_t *) vesa_fb; }
uint32_t *vesa_get_framebuffer_width() { return vesa_width; }
uint32_t *vesa_get_framebuffer_height() { return vesa_height; }
uint32_t *vesa_get_framebuffer_pitch() { return vesa_pitch; }
uint32_t *vesa_get_framebuffer_bpp() { return vesa_bpp; }

uint32_t *vesa_get_back_buffer() { return (uint32_t *) vesa_back_buffer; }
