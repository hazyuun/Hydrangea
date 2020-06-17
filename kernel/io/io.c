/*
 *	File : io.c
 *	Description : TODO
 * */

#include "io.h"


void io_outb(uint16_t port, uint8_t data){
	asm volatile ("outb %0, %1": :"a"(data), "Nd"(port));
}

uint8_t io_inb(uint16_t port){
    uint8_t result;
    asm volatile ("inb %1, %0":"=a"(result):"Nd"(port));
    return result;
}
