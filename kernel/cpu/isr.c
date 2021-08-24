#include <cpu/registers.h>

#include <drivers/pit.h>
#include <drivers/kbd.h>
#include <drivers/ata.h>
#include <drivers/serial.h>

#include <mem/paging.h>
#include <term/term.h>

#include <syscalls/syscall.h>

#include <kernel.h>

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <util/logger.h>
#include <multitasking/scheduler.h>

void isr_common_handler(registers_t *r) {
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
    printk("\n eip : "); term_print_hex(r->eip);
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
  case 0x20:
    syscall_handler(r);
    break;
  }
}
