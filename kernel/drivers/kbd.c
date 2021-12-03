/*
 *  File: kbd.c
 *  Description: TODO
 * */

#include <drivers/kbd.h>
#include <string.h>
#include <term/term.h>
#include <io/io.h>
#include <cpu/registers.h>
#include <cpu/irq.h>
#include <cpu/pic.h>
#include <drivers/ps2.h>

/* TODO: Make a complete keyboard layout */
/* Note : There are some mistakes in my layouts */

/* TODO : Implement a proper keyboard driver with a ring buffer */

char kbd_layout_us[128] = {
    0,    27,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-',  '=',
    '\b', '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[',  ']',
    '\n', 0,    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0,    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,    '*',
    0,    ' ',  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,    0,
    0,    0,    0,   0,   '-', 0,   0,   0,   '+'};

char kbd_layout_fr[128] = {
    0,    27,   '&', 'e', '"', '\'', '(', '-', 'e', '_', 'c', 'a', ')', '=',
    '\b', '\t', 'a', 'z', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '^', '$',
    '\n', 0,    'q', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'm', ' ', '*',
    0,    '\\', 'w', 'x', 'c', 'v', 'b', 'n', ',', ';', ':', '!', 0,   '*',
    0,    ' ',  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,    0,    0,   0,   '-', 0,   0,   0,   '+'};

char kbd_layout_us_cap[128] = {
    0,    27,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-',  '=',
    '\b', '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[',  ']',
    '\n', 0,    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`',
    0,    '\\', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/', 0,    '*',
    0,    ' ',  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,    0,
    0,    0,    0,   0,   '-', 0,   0,   0,   '+'};

char kbd_layout_fr_cap[128] = {
    0,    27,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', ')', '+',
    '\b', '\t', 'A', 'Z', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '^', '$',
    '\n', 0,    'Q', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 'M', ' ', '*',
    0,    '\\', 'W', 'X', 'C', 'V', 'B', 'N', '?', '.', '/', ' ', 0,   '*',
    0,    ' ',  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,    0,    0,   0,   '-', 0,   0,   0,   '+'};

char *kbd_cur_layout = kbd_layout_us;
char *kbd_cur_layout_cap = kbd_layout_us_cap;

char kbd_key_states[128] = {0};
uint8_t kbd_last_key = 0;

#include <util/logger.h>
void kbd_init(uint8_t ch){
  
  ps2_ctrlr_cmd(PS2_CMD_READ_CFG);

  uint8_t cfg = ps2_inb(PS2_DATA);

  cfg |= ch == 2 ? PS2_CFG_PORT2_INT : PS2_CFG_PORT1_INT;
  cfg |= PS2_CFG_PORT1_TRANSLATION;

  cfg &= ~(ch == 2 ? PS2_CFG_PORT2_CLK : PS2_CFG_PORT1_CLK);

  ps2_ctrlr_cmd(PS2_CMD_WRITE_CFG);
  ps2_outb(PS2_DATA, cfg);

  ps2_dev_cmd(ch, PS2_DEV_CMD_SCAN_ENABLE);
  ps2_expect(PS2_DEV_ACK);
  
  irq_register(1, &kbd_event);
  pic_unmask(1);
  
}

uint8_t kbd_switch_layout(char *layout_name) {
  if (!strcmp(layout_name, "en"))
    return kbd_set_layout(kbd_layout_us, kbd_layout_us_cap);
  else if (!strcmp(layout_name, "fr"))
    return kbd_set_layout(kbd_layout_fr, kbd_layout_fr_cap);
  return 1;
}

uint8_t kbd_set_layout(char *layout, char *layout_cap) {
  if (!layout)
    return 0;
  kbd_cur_layout = layout;
  kbd_cur_layout_cap = layout_cap;

  return 1;
}

uint8_t kbd_keydown() {
  uint16_t key = 0xFF;
  for (uint16_t i = 0; i < 128; i++) {
    if (kbd_key_states[i]) {
      key = i;
      if (key != 0xA)
        return key; /* Dirty fix for "shift blocking other keys" problem */
    }
  }
  return key;
}

uint8_t kbd_get() {
  char c;
  if (kbd_key_states[0xA])
    c = kbd_cur_layout_cap[kbd_last_key];
  else
    c = kbd_cur_layout[kbd_last_key];

  kbd_key_states[kbd_last_key & 0xF] = 0;
  return c;
}

#include <multitasking/scheduler.h>
#include <fs/file_ops.h>
#include <fs/file_descriptor.h>
#include <fs/vfs.h>

void kbd_event(registers_t *r) {
  (void) r;
  
  uint8_t scancode = io_inb(0x60);
  
  kbd_key_states[scancode & 0xF] = !(scancode & 0x80);
  
  /* Released */
  if (scancode & 0x80) {
    
  }
  /* Pressed */
  else {
    kbd_last_key = scancode;
    
    task_t *t = mt_get_fg_task();
    if(!t) return;
    
    file_descriptor_t *f = list_get(t->file_descriptors, 0);
    vfs_file_t *file = fd_to_node(f)->file;
    file->write(file, 0, 1,  &kbd_cur_layout[kbd_last_key]);
  }
}
