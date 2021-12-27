#include <term/term.h>
#include <term/vesa/vesa_term.h>
#include <term/vga/vga_term.h>

struct term current_term = {0};

uint8_t term_init(uint8_t type, multiboot_info_t *mbi) {
  current_term.type = type;
  switch (type) {
  case VGA_TERM: {
    vga_term_init();
    /* TODO : fix this */
    current_term.use_color = &vga_term_use_vesa_color;
    current_term.cur_mov = &vga_term_cur_mov;
    current_term.cur_step = &vga_term_cur_step;
    current_term.scroll = &vga_term_scroll;
    current_term.clear = &vga_term_clear;
    current_term.swap_buffers = 0;
    current_term.putchar = &vga_term_putchar;
    current_term.putat = &vga_term_putat;
    current_term.print = &vga_term_print;
    current_term.print_hex = &vga_term_print_hex;
    return 0;
  }
  case VESA_TERM: {
    vesa_init(mbi);
    vesa_term_init(vesa_get_framebuffer());
    current_term.use_color = &vesa_term_use_color;
    current_term.cur_mov = &vesa_term_cur_mov;
    current_term.cur_step = &vesa_term_cur_step;
    current_term.scroll = &vesa_term_scroll;
    current_term.clear = &vesa_term_clear;
    current_term.swap_buffers = &vesa_swap_buffers;
    current_term.putchar = &vesa_term_putchar;
    current_term.putat = &vesa_term_putat;
    current_term.print = &vesa_term_print;
    current_term.print_hex = &vesa_term_print_hex;
    return 0;
  }
  }
  return 1;
}

void term_use_color(uint32_t color) {
  if (current_term.use_color)
    current_term.use_color(color);
}

void term_cur_mov(size_t x, size_t y) {
  if (current_term.cur_mov)
    current_term.cur_mov(x, y);
}

void term_cur_step() {
  if (current_term.cur_step)
    current_term.cur_step();
}

void term_scroll() {
  if (current_term.scroll)
    current_term.scroll();
}

void term_clear() {
  if (current_term.clear)
    current_term.clear();
}

void term_swap_buffers() {
  if (current_term.swap_buffers)
    current_term.swap_buffers();
}

void term_putchar(unsigned char c) {
  if (current_term.putchar)
    current_term.putchar(c);
}

void term_putat(unsigned char c, size_t x, size_t y) {
  if (current_term.putat)
    current_term.putat(c, x, y);
}

void term_print(const char *string) {
  if (current_term.print)
    current_term.print(string);
}

void term_print_hex(uint32_t x) {
  if (current_term.print_hex)
    current_term.print_hex(x);
}
uint8_t term_get_type() { return current_term.type; }