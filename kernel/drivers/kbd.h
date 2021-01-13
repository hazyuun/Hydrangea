/*
 *  File : kbd.h
 *  Description: TODO
 * */

#ifndef _KBD_
#define _KBD_

#include <stddef.h>
#include <stdint.h>

uint8_t kbd_switch_layout(char *layout_name);
uint8_t kbd_set_layout(char *layout);
uint8_t kbd_keydown();
uint8_t kbd_get();
void kbd_event(uint8_t scancode);

#endif
