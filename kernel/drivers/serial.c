/*
 *	File : serial.c
 *	Description : TODO
 *
 * */

#include "serial.h"

void 	serial_init(uint16_t port){
	/* No interrupts, and enable DLAB*/
	io_outb(port + 1, 0x00);
	io_outb(SERIAL_LINE_CMD(port), 0x80);
	
	/* use transmission speed 38400 baud (divisor = 3)*/
	io_outb(port, 0x03);
	io_outb(port + 1, 0x00);
	/* 
	 * length : 1 byte
	 * parity : none
	 * 1 stop bit
	 * All together : 0b00000011
	 * In hex : 0x03
	 * */
	io_outb(SERIAL_LINE_CMD(port), 0x03);    // 8 bits, no parity, one stop bit
	
	/* FIFO : on
	 * Clear Tx and Rx
	 * FIFO length : 14 bytes
	 * 
	 * All together : 0b11000111
	 * In hex : 0xC7
	 *
	 * */
	io_outb(SERIAL_FIFO_CMD(port), 0xC7);

	/*
	 * Enable Interrupt Request
	 * and set RTS/DSR
	 * */
	io_outb(SERIAL_MODEM_CMD(port), 0x0B);
}

int 	serial_available(uint16_t port){
	return 0x01 & io_inb(SERIAL_LINE_STATUS(port));
}

int 	serial_tx_empty(uint16_t port){
	return 0x20 & io_inb(SERIAL_LINE_STATUS(port));
}

uint8_t serial_read(uint16_t port){
	while(!serial_available(port)); /* Wait for the data */
	return io_inb(port);
}

void 	serial_write(uint16_t port, uint8_t data){
	while(!serial_tx_empty(port));
	io_outb(port, data);
}


