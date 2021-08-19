#ifndef _PIC_H_
#define _PIC_H_

#define PIC_1_CMD    0x20
#define PIC_1_STATUS 0x20
#define PIC_1_DATA   0x21
#define PIC_1_MASK   0x21

#define PIC_2_CMD    0xA0
#define PIC_2_STATUS 0xA0
#define PIC_2_DATA   0xA1
#define PIC_2_MASK   0xA1

/* PIC 1 */
#define IRQ_TIMER       0x0
#define IRQ_KEYBOARD    0x1
#define IRQ_SERIAL2     0x3
#define IRQ_SERIAL1     0x4
#define IRQ_PARALLEL2   0x5
#define IRQ_FLOPPY      0x6
#define IRQ_PARALLEL1   0x7

/* PIC 2 */
#define IRQ_CMOS_RTC    0x0
#define IRQ_9           0x1
#define IRQ_10          0x2
#define IRQ_11          0x3
#define IRQ_MOUSE       0x4
#define IRQ_FPU         0x5
#define IRQ_ATA1        0x6
#define IRQ_ATA2        0x7

#include <stdint.h>

void pic_init();
uint8_t pic_get_mask(uint8_t pic);
void pic_set_mask(uint8_t pic, uint8_t mask);
void pic_mask(uint8_t irq);
void pic_unmask(uint8_t irq);
void pic_eoi(uint8_t irq);
uint8_t pic_is_irq7_sus();
uint8_t pic_is_irq15_sus();

#endif
