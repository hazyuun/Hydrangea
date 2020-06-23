/*
 *      File: isr.c
 *      Description : TODO
 *
 * */

#include <stddef.h>
#include <stdint.h>
#include "../tty/tty.h"
#include "../drivers/serial.h"

typedef struct {
	uint32_t gs, fs, es, ds;
	uint32_t edi, esi, ebp, useless, ebx, edx, ecx, eax;
	uint32_t int_num, err_code;
	uint32_t eip, cs, eflags, esp, ss;
} registers;

void isr_common_handler(registers* r){
    tty_print("Interrupt ! ");
    tty_print_hex(r->int_num);
    tty_print("\n");
} 



