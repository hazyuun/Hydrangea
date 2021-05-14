#ifndef _IDT_H_
#define _IDT_H_

#include <stddef.h>
#include <stdint.h>

/* Interrupt Descriptor Table */
typedef struct {
  uint16_t size;
  uint32_t base;
} __attribute__((packed)) IDT_ptr;

typedef struct {
  uint16_t base_lo;
  uint16_t segsel;
  uint8_t zero;
  uint8_t flags;
  uint16_t base_hi;
} __attribute__((packed)) IDT_entry;


#define I_GATETYPE_TASK32 0x5
#define I_GATETYPE_INT16  0x6
#define I_GATETYPE_TRAP16 0x7
#define I_GATETYPE_INT32  0xE
#define I_GATETYPE_TRAP32 0xF

#define I_STORAGE_SEG     (1 << 4)
#define I_DPL(r)          (r << 5)
#define I_PRESENT         (1 << 7)



/* ISR Handlers */
extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
/* Sigh.. A lot of handlers ! 
 *
 * EDIT : haha, yes but copy paste exists
 */
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
/* This is boring ... 
 *
 * EDIT: Yeaah it is
 * */
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

/* isr32 : Hello there fellow ISRs ! I am the syscall handler 
 *      Nice to meet you !
 */
extern void isr32();

/*
 * Me           : Sigh.. Finally !
 * IRQ handlers : Allow us to introduce ourselves
 * Me           : *Surprised Pikachu*
 * 
 * EDIT: Yo ! wtf ? Why do I write memes in my code
 */


extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
/* Sigh.. I feel sleepy.. 
 *
 * EDIT : Hey, go take a rest, I am from the future
 *     and I confirm that your current bugs will be solved,
 *     I am trying to make user tasks now, so far so good !
 * */
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();
/* .. and lonely.. 
 *
 * EDIT : Hey, I am you from the future !
 *    No you are not lonely, I am with you
 * */


#define PIC_1 0x20
#define PIC_2 0xA0
#define PIC_1_DATA 0x21
#define PIC_2_DATA 0xA1

void idt_init();
void idt_set_entry(uint8_t, uint32_t, uint16_t, uint8_t);

#endif
