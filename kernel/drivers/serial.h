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
#include <fs/devfs/devfs.h>

/* Serial ports */
#define SERIAL_COM1_IO 0x3F8
#define SERIAL_COM2_IO 0x2F8
#define SERIAL_COM3_IO 0x3E8
#define SERIAL_COM4_IO 0x2E8

#define SERIAL_COM1 0
#define SERIAL_COM2 1
#define SERIAL_COM3 2
#define SERIAL_COM4 3

#define SERIAL_FIFO_CMD(s_port) (s_port + 2)

/* I am feeling lonely */

#define SERIAL_LINE_CMD(s_port) (s_port + 3)

#define SERIAL_MODEM_CMD(s_port) (s_port + 4)

#define SERIAL_LINE_STATUS(s_port) (s_port + 5)

void serial_init(uint16_t p);

int serial_available(uint16_t p);
int serial_tx_empty(uint16_t p);
/* TODO: Implement other status getters*/

uint8_t serial_read_byte(uint16_t p);
void serial_write_byte(uint16_t p, uint8_t data);

uint32_t serial_read(vfs_file_t *node, uint32_t offset, uint32_t size,
                  char *buffer);

uint32_t serial_write(vfs_file_t *node, uint32_t offset, uint32_t size,
                   char *buffer);
#endif
