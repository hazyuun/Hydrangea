/*
 *	File : serial.c
 *	Description : TODO
 *
 * */

#include <drivers/serial.h>
#include <mem/heap.h>

char *serial_device_names[] = {"com1", "com2", "com3", "com4"};
uint16_t serial_device_ports[] = {SERIAL_COM1_IO, SERIAL_COM2_IO,
                                  SERIAL_COM3_IO, SERIAL_COM4_IO};

#include <util/logger.h>
void serial_init(uint16_t p) {
  uint16_t port = serial_device_ports[p];

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
  io_outb(SERIAL_LINE_CMD(port), 0x03); // 8 bits, no parity, one stop bit

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

  /* TODO : set to loopback and do a self-test */

  devfs_node_t *serial_dev;
  serial_dev = (devfs_node_t *)kmalloc(sizeof(devfs_node_t));
  serial_dev->name = serial_device_names[p];
  serial_dev->type = VFS_CDEV;
  serial_dev->read = serial_read;
  serial_dev->write = serial_write;
  serial_dev->device_specific = p;

  devfs_register_dev(serial_dev);
}

int serial_available(uint16_t p) {
  uint16_t port = serial_device_ports[p];

  return 0x01 & io_inb(SERIAL_LINE_STATUS(port));
}

int serial_tx_empty(uint16_t p) {
  uint16_t port = serial_device_ports[p];

  return 0x20 & io_inb(SERIAL_LINE_STATUS(port));
}

uint8_t serial_read_byte(uint16_t p) {
  uint16_t port = serial_device_ports[p];

  while (!serial_available(port))
    asm volatile("pause");
  ; /* Wait for the data */
  return io_inb(port);
}

void serial_write_byte(uint16_t p, uint8_t data) {
  uint16_t port = serial_device_ports[p];

  while (!serial_tx_empty(port))
    asm volatile("pause");
  ;
  io_outb(port, data);
}

/*
    TODO : Something is wrong with reading from serial
    TODO : Probably rewrite this driver, it lacks so many things,
    it's the first driver I ever wrote so..
*/
uint32_t serial_read(vfs_file_t *node, uint32_t offset, uint32_t size,
                     char *buffer) {
  for (size_t i = 0; i < size; i++) {
    buffer[offset + i] = serial_read_byte(node->inode);
  }
  /* TODO: return the amount of read bytes */
  return 0;
}

uint32_t serial_write(vfs_file_t *node, uint32_t offset, uint32_t size,
                      char *buffer) {

  for (size_t i = 0; i < size; i++) {
    serial_write_byte(node->inode, buffer[offset + i]);
  }

  /* TODO: return the amount of written bytes */
  return 0;
}
