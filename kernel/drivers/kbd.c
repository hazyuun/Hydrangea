/*
 *  File: kbd.c
 *  Description: TODO
 * */

#include <drivers/kbd.h>
#include <tty/tty.h>
#include <string.h>

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

char kbd_layout_fr [128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',   
  '\t', 'a', 'z', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '^', '$', '\n',     
    0,   'q', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'm', ' ', '*',  0, '\\', 
          'w', 'x', 'c', 'v', 'b', 'n', ',', ';', ':', '!',   0,
  '*', 0,  ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
    0, 0, 0, 0, 0,'-',0,0,0,'+' 
};

char* kbd_cur_layout = kbd_layout_us;

char kbd_key_states[128] = {0};
uint8_t kbd_last_key = 0;

uint8_t kbd_switch_layout(char* layout_name){
  if(!strcmp(layout_name, "en")) return kbd_set_layout(kbd_layout_us);
  else if(!strcmp(layout_name, "fr")) return kbd_set_layout(kbd_layout_fr);
  return 1;
}

uint8_t kbd_set_layout(char* layout){
  if(!layout) return 0;
  kbd_cur_layout = layout;
  return 1;
}

uint8_t kbd_keydown(){
  for(uint16_t i = 0; i<128; i++)
    if(kbd_key_states[i]) return 1;
  return 0;
}

uint8_t kbd_get(){
  kbd_key_states[kbd_last_key&0xF] = 0;
  return kbd_cur_layout[kbd_last_key];
}

void kbd_event(uint8_t scancode){
    kbd_key_states[scancode&0xF] = !(scancode & 0x80);
    /* Released */
    if(scancode & 0x80){
        
    }
    /* Pressed */
    else{
      kbd_last_key = scancode;
    }
}


