/*
 *	File : idt.h
 *	Description : TODO
 * */


#ifndef _IDT_H_
#define _IDT_H_

#include <stddef.h>
#include <stdint.h>

/* Interrupt Descriptor Table */
typedef struct{
	uint16_t size;
	uint32_t base;
} __attribute__((packed)) IDT_ptr;

typedef struct{
	uint16_t base_lo;
	uint16_t segsel;
	uint8_t	zero; /* pimples ? zero ! this byte ? ZERO ! */
	uint8_t flags;
	uint16_t base_hi;
} __attribute__((packed)) IDT_entry;

/* ISR Handlers */
extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
/* Sigh.. A lot of handlers ! */
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
/* This is boring ... */
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
/* Anyway */
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();
/* Finally !*/

void idt_init();
void idt_set_entry(uint8_t, uint32_t, uint16_t, uint8_t);

#endif

