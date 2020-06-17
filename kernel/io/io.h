/*
 *	File : io.h
 *	Description : TODO
 * */


#ifndef _IO_H_
#define _IO_H_

#include <stdint.h>

void 	io_outb(uint16_t port, uint8_t data);
uint8_t io_inb(uint16_t port);

#endif
