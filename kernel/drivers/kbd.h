/*
 *  File : kbd.h
 *  Description: TODO
 * */

#ifndef _KBD_H_
#define _KBD_H_

#include <cpu/registers.h>
#include <stddef.h>
#include <stdint.h>

void kbd_init(uint8_t ch);
uint8_t kbd_switch_layout(char *layout_name);
uint8_t kbd_set_layout(char *layout, char *layout_cap);
uint8_t kbd_keydown();
uint8_t kbd_get();
void kbd_event(registers_t *r);

#endif
