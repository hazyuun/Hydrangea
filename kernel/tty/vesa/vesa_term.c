#include <tty/vesa/default_font.h>
#include <tty/vesa/vesa_term.h>
#include <vesa/vesa.h>

vesa_term_t c_term;

void vesa_term_init(uint32_t *vesa_fb) {
  c_term.row = 0;
  c_term.col = 0;
  c_term.vesa_fb = vesa_fb;
  vesa_term_use_color(NICE_WHITE);
  vesa_term_clear();
  vesa_term_cur_mov(0, 0);
}

void vesa_term_use_color(uint32_t color) { c_term.color = color; }

void vesa_term_cur_mov(size_t x, size_t y) {
  c_term.col = x;
  c_term.row = y;
}

void vesa_term_clear() {
  vesa_clear();
  vesa_term_cur_mov(0, 0);
}

void vesa_term_putat(unsigned char c, size_t x, size_t y) {
  uint32_t *p = font_getchar(c);
  for (int i = 0; i < FONT_HEIGHT; i++) {
    for (int j = 0; j < FONT_WIDTH; j++) {
      vesa_put_pixel(x * FONT_WIDTH + j, y * FONT_HEIGHT + i,
                     (p[i]) & (0x80 >> j) ? c_term.color : NICE_BG);
    }
  }
}

void vesa_term_scroll() {
  for (size_t i = 0; i < FB_HEIGHT; i++) {
    for (size_t j = 0; j < FB_WIDTH; j++) {
      c_term.vesa_fb[i * FB_WIDTH + j] =
          (i >= FB_HEIGHT - FONT_HEIGHT)
              ? NICE_BG
              : c_term.vesa_fb[(i + 16) * FB_WIDTH + j];
    }
  }
}

void vesa_term_cur_stp() { vesa_term_cur_mov(c_term.col, c_term.row); }

void vesa_term_putchar(unsigned char c) {
  if (c == '\n') {
    vesa_term_putat(' ', c_term.col, c_term.row);
    c_term.col = 0;
    c_term.row++;
    if (c_term.row == FB_ROWS) {
      c_term.row -= 1;
      vesa_term_scroll();
    }
    vesa_term_cur_stp();
  } else if (c == '\b') {
    vesa_term_putat(' ', c_term.col, c_term.row);
    c_term.col--;
    vesa_term_cur_stp();

  } else if (c == '\t') {
    size_t i = 0;
    while (c_term.col + 1 > i)
      i += 4;
    c_term.col = i;
    vesa_term_cur_stp();

  } else {
    if (c_term.col + 1 == FB_COLS) {
      c_term.col = 0;
      c_term.row++;
      if (c_term.row == FB_ROWS) {
        c_term.row -= 1;
        vesa_term_scroll();
      }
    }
    vesa_term_putat(c, c_term.col++, c_term.row);
  }

  uint32_t clr = c_term.color;
  vesa_term_use_color(NICE_CUR);
  vesa_term_putat('_', c_term.col, c_term.row);
  vesa_term_use_color(clr);
}

void vesa_term_print(const char *string) {
  size_t index = 0;
  char c;
  while ((c = *(string + index++)))
    vesa_term_putchar(c);
}

void vesa_term_print_hex(uint32_t x) {
  /* TODO : clean this */
  char n[16] = {'0', '1', '2', '3', '4', '5', '6', '7',
                '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
  vesa_term_print("0x");
  vesa_term_putchar(n[(x & 0xF0000000) >> 28]);
  vesa_term_putchar(n[(x & 0x0F000000) >> 24]);
  vesa_term_putchar(n[(x & 0x00F00000) >> 20]);
  vesa_term_putchar(n[(x & 0x000F0000) >> 16]);
  vesa_term_putchar(n[(x & 0x0000F000) >> 12]);
  vesa_term_putchar(n[(x & 0x00000F00) >> 8]);
  vesa_term_putchar(n[(x & 0x000000F0) >> 4]);
  vesa_term_putchar(n[(x & 0x0000000F) >> 0]);
}
