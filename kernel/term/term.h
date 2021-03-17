#ifndef _TERM_H_
#define _TERM_H_

#define VGA_TERM  0
#define VESA_TERM 1

#include <stdint.h>
#include <term/vesa/vesa_term.h>
#include <vesa/nice_colors.h>

typedef void (*term_use_color_fptr_t)(uint32_t);
typedef void (*term_cur_mov_fptr_t)(size_t, size_t);
typedef void (*term_cur_step_fptr_t)();
typedef void (*term_scroll_fptr_t)();
typedef void (*term_clear_fptr_t)();
typedef void (*term_putchar_fptr_t)(unsigned char);
typedef void (*term_putat_fptr_t)(unsigned char, size_t, size_t);
typedef void (*term_print_fptr_t)(const char *);
typedef void (*term_print_hex_fptr_t)(uint32_t);

struct term {
  uint8_t                 type;
  term_use_color_fptr_t   use_color;
  term_cur_mov_fptr_t     cur_mov;
  term_cur_step_fptr_t    cur_step;
  term_scroll_fptr_t      scroll;
  term_clear_fptr_t       clear;
  term_putchar_fptr_t     putchar;
  term_putat_fptr_t       putat;
  term_print_fptr_t       print;
  term_print_hex_fptr_t   print_hex;
};

#include <boot/multiboot.h>
uint8_t term_init(uint8_t type, multiboot_info_t *mbi);


#include <stddef.h>
void term_use_color(uint32_t color);

void term_cur_mov(size_t x, size_t y);
void term_cur_stp();

void term_scroll();
void term_clear();

void term_putchar(unsigned char c);
void term_putat(unsigned char c, size_t x, size_t y);
void term_print(const char *string);
void term_print_hex(uint32_t x);
uint8_t term_get_type();

#endif