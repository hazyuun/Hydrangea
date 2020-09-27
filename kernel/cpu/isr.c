/*
 *      File: isr.c
 *      Description : TODO
 *
 * */

#include <stddef.h>
#include <stdint.h>
#include <tty/tty.h>
#include <drivers/serial.h>
#include <drivers/kbd.h>

typedef struct {
	uint32_t gs, fs, es, ds;
	uint32_t edi, esi, ebp, useless, ebx, edx, ecx, eax;
	uint32_t int_num, err_code;
	uint32_t eip, cs, eflags, esp, ss;
} registers;

void isr_common_handler(registers* r){
    tty_print("Interrupt :  ");
    tty_print_hex(r->int_num);
    tty_print(" Err code : ");
    tty_print_hex(r->err_code);
    tty_print("\n");
} 

//extern kbd_keys

void irq_common_handler(registers* r){
    if (r->int_num >= 40){
        io_outb(0xA0, 0x20);
    }
    io_outb(0x20, 0x20);
    
    if(r->int_num == 0x21){ /* Keyboard */
        uint8_t scancode = io_inb(0x60); 
        kbd_event(scancode);
    }
    //uint8_t code = io_inb(0x60);
    //tty_print("IRQ ");
    //tty_print_hex(r->int_num);
    //tty_print(" \n int :  ");
    //tty_print_hex(r->int_num);
    //tty_putchar(kbd_US[code]);
}
 

 
