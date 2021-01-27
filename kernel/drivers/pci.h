/*
 *      File: pci.h
 *      Description : TODO
 *
 * */

#ifndef _PCI_H_
#define _PCI_H_

#include <stdint.h>

#define PCI_CONFIG_ADDR 0xCF8
#define PCI_CONFIG_DATA 0xCF8

#define PCI_VENDOR_OFFSET 0x0
#define PCI_DEV_ID_OFFSET 0x2
#define PCI_CMD_OFFSET 0x4
#define PCI_INT_LINE_OFFSET 0x3C
#define PCI_INT_PIN_OFFSET 0x3E

typedef struct {
  uint32_t BAR[6];
  uint8_t int_line;
  uint8_t int_pin;
  uint8_t min_grant;
  uint8_t max_latency;
} PCI_header0_t;


typedef struct PCI_device {
  uint16_t bus;
  uint16_t device;
  uint16_t function;
  uint16_t device_id;
  uint16_t vendor_id;
  uint16_t command;
  uint8_t class_code;
  uint8_t subclass;
  uint8_t prog_if;
  uint8_t rev_id;
  uint8_t header_type;
  void *header;
  const char *name;
  struct PCI_device *next;
} PCI_device_t;

uint16_t PCI_read_word(int8_t bus, uint8_t slot, uint8_t function,
                       uint8_t offset);

typedef uint8_t (*driver_init_t)(PCI_device_t *);

void PCI_detect();
void PCI_list();

#endif
