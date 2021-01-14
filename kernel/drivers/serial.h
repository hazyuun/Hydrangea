/*
 *	File : serial.h
 *	Description : TODO
 *
 * */

#ifndef _SERIAL_H_
#define _SERIAL_H_

#include <stddef.h>
#include <stdint.h>

#include <io/io.h>

/* Serial ports */
#define SERIAL_COM1 0x3F8
#define SERIAL_COM2 0x2F8
#define SERIAL_COM3 0x3E8
#define SERIAL_COM4 0x2E8

#define SERIAL_FIFO_CMD(s_port) (s_port + 2)

/* I am feeling lonely */

#define SERIAL_LINE_CMD(s_port) (s_port + 3)

#define SERIAL_MODEM_CMD(s_port) (s_port + 4)

#define SERIAL_LINE_STATUS(s_port) (s_port + 5)

void serial_init(uint16_t port);

int serial_available(uint16_t port);
int serial_tx_empty(uint16_t port);
/* TODO: Implement other status getters*/

uint8_t serial_read(uint16_t port);
void serial_write(uint16_t port, uint8_t data);

#endif
