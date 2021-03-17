#include <io/io.h>
#include <term/vga/vga_term.h>

#include <stddef.h>
#include <stdint.h>
struct vga_term_state c_vga_term_state; /* Current tty state */
uint16_t *vga_term_frame_buffer;   /* Pointer to video memory */

void vga_term_init() {
  c_vga_term_state.row = 0;
  c_vga_term_state.column = 0;
  vga_term_use_color(VGA_WHITE, VGA_BLACK);
  vga_term_frame_buffer = (uint16_t *)0xB8000;
  vga_term_cur_mov(0, 0);
  for (size_t k = 0; k < (size_t)(VGA_WIDTH * VGA_HEIGHT); k++)
    *(vga_term_frame_buffer + k) = vga_char(' ', c_vga_term_state.color);
}

void vga_term_use_color(vga_color fg, vga_color bg) {
  c_vga_term_state.color = vga_color_byte(fg, bg);
}

void vga_term_cur_mov(size_t x, size_t y) {
  uint16_t index = x + (uint16_t)VGA_WIDTH * y;
  io_outb(0x3D4, 0x0F);
  io_outb(0x3D5, (uint8_t)(index & 0xFF));
  io_outb(0x3D4, 0x0E);
  io_outb(0x3D5, (uint8_t)((index >> 8) & 0xFF));
  c_vga_term_state.row = y;
  c_vga_term_state.column = x;
}

uint16_t vga_term_cur_pos() {
  uint16_t cur_pos = 0x0;
  io_outb(0x3D4, 0x0F);
  cur_pos |= io_inb(0x3D5);
  io_outb(0x3D4, 0x0E);
  cur_pos |= ((uint16_t)io_inb(0x3D5)) << 8;
  return cur_pos;
}
void vga_term_cur_step() { vga_term_cur_mov(c_vga_term_state.column, c_vga_term_state.row); }

void vga_term_scroll() {
  for (size_t i = 0; i < VGA_HEIGHT; i++) {
    for (size_t j = 0; j < VGA_WIDTH; j++) {
      vga_term_frame_buffer[i * VGA_WIDTH + j] =
          vga_term_frame_buffer[(i + 1) * VGA_WIDTH + j];
    }
  }
}

void vga_term_clear() {
  for (size_t i = 0; i < VGA_HEIGHT; i++)
    vga_term_scroll();
  vga_term_cur_mov(0, 0);
}

void vga_term_fill(vga_color color) {
  vga_term_use_color(color, color);
  for (size_t i = 0; i < VGA_HEIGHT; i++) {
    for (size_t j = 0; j < VGA_WIDTH; j++) {
      vga_term_putat(' ', j, i);
    }
  }
  vga_term_use_color(VGA_WHITE, VGA_BLACK);
}

void vga_term_putat(unsigned char c, size_t x, size_t y) {
  vga_term_frame_buffer[x + y * (size_t)(VGA_WIDTH)] =
      vga_char(c, c_vga_term_state.color);
  vga_term_cur_step();
}

void vga_term_putchar(unsigned char c) {
  if (c == '\n') {
    c_vga_term_state.column = 0;
    c_vga_term_state.row++;
    if (c_vga_term_state.row == VGA_HEIGHT) {
      c_vga_term_state.row -= 1;
      vga_term_scroll();
    }
    vga_term_cur_step();
  } else if (c == '\b') {
    c_vga_term_state.column--;
    vga_term_cur_step();
    vga_term_putat(' ', c_vga_term_state.column, c_vga_term_state.row);

  } else if (c == '\t') {
    size_t i = 0;
    while (c_vga_term_state.column + 1 > i)
      i += 4;
    c_vga_term_state.column = i;
    vga_term_cur_step();

  } else {
    if (c_vga_term_state.column + 1 == VGA_WIDTH) {
      c_vga_term_state.column = 0;
      c_vga_term_state.row++;
      if (c_vga_term_state.row == VGA_HEIGHT) {
        c_vga_term_state.row -= 1;
        vga_term_scroll();
      }
    }
    vga_term_putat(c, c_vga_term_state.column++, c_vga_term_state.row);
  }
}

void vga_term_print(const char *string) {
  size_t index = 0;
  char c;
  while ((c = *(string + index++)))
    vga_term_putchar(c);
}

void vga_term_print_hex(uint32_t x) {
  char n[16] = {'0', '1', '2', '3', '4', '5', '6', '7',
                '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
  vga_term_print("0x");
  vga_term_putchar(n[(x & 0xF0000000) >> 28]);
  vga_term_putchar(n[(x & 0x0F000000) >> 24]);
  vga_term_putchar(n[(x & 0x00F00000) >> 20]);
  vga_term_putchar(n[(x & 0x000F0000) >> 16]);
  vga_term_putchar(n[(x & 0x0000F000) >> 12]);
  vga_term_putchar(n[(x & 0x00000F00) >> 8]);
  vga_term_putchar(n[(x & 0x000000F0) >> 4]);
  vga_term_putchar(n[(x & 0x0000000F) >> 0]);

  /*TODO : Avoid repetition */
}
