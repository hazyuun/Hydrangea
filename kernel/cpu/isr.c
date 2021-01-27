/*
 *      File: isr.c
 *      Description : TODO
 *
 * */

#include <cpu/pit.h>
#include <drivers/kbd.h>
#include <drivers/serial.h>
#include <kernel.h>
#include <mem/paging.h>
#include <stddef.h>
#include <stdint.h>
#include <tty/tty.h>
#include <stdio.h>


typedef struct {
  uint32_t gs, fs, es, ds;
  uint32_t edi, esi, ebp, useless, ebx, edx, ecx, eax;
  uint32_t int_num, err_code;
  uint32_t eip, cs, eflags, esp, ss;
} registers;

void isr_common_handler(registers *r) {
  tty_print("Interrupt :  ");
  tty_print_hex(r->int_num);
  tty_print(" Err code : ");
  tty_print_hex(r->err_code);
  tty_print("\n");

  switch (r->int_num) {
  case 0x0:
    panic("Division by zero exception");
    break;
  case 0x1:
    panic("Debug exception");
    break;
  case 0x2:
    panic("Non maskable interrupt");
    break;
  case 0x3:
    panic("Breakpoint exception");
    break;
  case 0x4:
    panic("Overflow");
    break;
  case 0x5:
    panic("Out of bounds exception");
    break;
  case 0x6:
    panic("Invalid opcode exception");
    break;
  case 0x7:
    panic("No coprocessor exception");
    break;
  case 0x8:
    panic("Double fault");
    break;
  case 0x9:
    panic("Coprocessor segment overrun");
    break;
  case 0xA:
    panic("Bad TSS");
    break;
  case 0xB:
    panic("Segment not present");
    break;
  case 0xC:
    panic("Stack fault");
    break;
  case 0xD:
    panic("General protection fault");
    break;
  case 0xE:
    pg_page_fault(r->err_code);
    panic("Page fault");
    break;
  case 0xF:
    panic("Unknown interrupt exception");
    break;
  case 0x10:
    panic("Coprocessor fault");
    break;
  case 0x11:
    panic("Alignment check exception");
    break;
  case 0x12:
    panic("Machine check exception");

    break;
  }
}

void irq_common_handler(registers *r) {
  if (r->int_num >= 40) {
    io_outb(0xA0, 0x20);
  }
  io_outb(0x20, 0x20);

  switch (r->int_num) {
  case 32:
    pit_event();
    break;
  case 0x21: {
    uint8_t scancode = io_inb(0x60);
    kbd_event(scancode);
    break;
  }
  case 32+14:
  case 32+15:
    break;
  }
}
