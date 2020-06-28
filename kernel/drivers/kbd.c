/*
 *  File: kbd.c
 *  Description: TODO
 * */

#include "kbd.h"
#include "../tty/tty.h"


/* TODO: Make a complete keyboard layout*/
char kbd_layout_us [128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',   
  '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',     
    0,   'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',  0, '\\', 
          'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',   0,
  '*', 0,  ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
    0, 0, 0, 0, 0,'-',0,0,0,'+' 
};

char kbd_key_states[128] = {0};


void kbd_event(uint8_t scancode){
    kbd_key_states[scancode] = !(scancode & 0x80);
    /* Released */
    if(scancode & 0x80){
        
    }
    /* Pressed */
    else{
        //tty_putchar(kbd_layout_us[scancode]);
    }
}


