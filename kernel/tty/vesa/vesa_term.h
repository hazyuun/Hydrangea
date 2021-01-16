#ifndef _VESA_TERM_H_
#define _VESA_TERM_H_

#include <stddef.h>
#include <vesa/vesa.h>

/* TODO : Avoid hardcoding these */
#define FB_HEIGHT 768
#define FB_WIDTH 1024

#define FONT_HEIGHT 16
#define FONT_WIDTH 8

#define FB_ROWS (FB_HEIGHT / FONT_HEIGHT)
#define FB_COLS (FB_WIDTH / FONT_WIDTH)

typedef struct {
  size_t row;
  size_t col;
  size_t cur_x;
  size_t cur_y;
  uint32_t color;
  uint32_t *vesa_fb;
} vesa_term_t;

void vesa_term_init(uint32_t *vesa_fb);
void vesa_term_use_color(uint32_t color);

void vesa_term_cur_mov(size_t x, size_t y);
void vesa_term_cur_stp();

void vesa_term_scroll();
void vesa_term_clear();

void vesa_term_putchar(unsigned char c);
void vesa_term_putat(unsigned char c, size_t x, size_t y);
void vesa_term_print(const char *string);
void vesa_term_print_hex(uint32_t x);

#endif