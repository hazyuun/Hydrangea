/*
 *	File : io.h
 *	Description : TODO
 * */

#ifndef _IO_H_
#define _IO_H_

#include <stdint.h>

void io_outb(uint16_t port, uint8_t data);
uint8_t io_inb(uint16_t port);

void io_outl(uint16_t port, uint32_t data);
uint32_t io_inl(uint16_t port);

#endif
