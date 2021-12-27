/*
 *      File: pci.c
 *      Description : TODO
 *
 * */

#include <drivers/ata.h>
#include <drivers/pci.h>
#include <io/io.h>
#include <mem/heap.h>
#include <mem/pmm.h>
#include <stddef.h>
#include <stdio.h>
#include <term/term.h>
#include <util/logger.h>

const char *class_names[19] = {"Unclassified",
                               "Mass Storage Controller",
                               "Network Controller",
                               "Display Controller",
                               "Multimedia Controller",
                               "Bridge Device",
                               "Simple Communication Controller",
                               "Base System Peripheral",
                               "Input Device Controller",
                               "Docking Station",
                               "Processor",
                               "Serial Bus Controller",
                               "Wireless Controller",
                               "Intelligent Controller",
                               "Satellite Communication Controller",
                               "Encryption Controller",
                               "Signal Processing Controller",
                               "Processing Accelerator",
                               "Non-Essential Instrumentation"};

PCI_device_t *PCI_devices;

uint16_t PCI_read_word(int8_t bus, uint8_t slot, uint8_t function,
                       uint8_t offset) {
  uint32_t address;
  uint32_t bus_32 = (uint32_t)bus;
  uint32_t slot_32 = (uint32_t)slot;
  uint32_t func_32 = (uint32_t)function;

  address = (uint32_t)((bus_32 << 16) | (slot_32 << 11) | (func_32 << 8) |
                       (offset & 0xfc) | ((uint32_t)0x80000000));

  io_outl(PCI_CONFIG_ADDR, address);
  return (uint16_t)((io_inl(0xCFC) >> ((offset & 2) * 8)) & 0xffff);
}

static uint16_t PCI_get_base_address(uint16_t bus, uint16_t device,
                                     uint16_t function, int address) {
  uint32_t ba =
      ((uint32_t)PCI_read_word(bus, device, function, 0xf + address * 4)) << 16;
  ba |= (uint32_t)((uint16_t)PCI_read_word(bus, device, function,
                                           0xf + address * 4 + 2));
  return ba;
}

static void PCI_parse_header0(PCI_device_t *dev, uint16_t bus, uint16_t slot,
                              uint16_t function) {

  PCI_header0_t *header = (PCI_header0_t *)kmalloc(sizeof(PCI_header0_t));
  for (size_t i = 0; i < 6; i++)
    header->BAR[i] = PCI_get_base_address(bus, slot, function, i);
  header->int_pin = PCI_read_word(bus, slot, function, PCI_INT_PIN_OFFSET);
  header->int_line = PCI_read_word(bus, slot, function, PCI_INT_LINE_OFFSET);
  dev->header = (void *)header;
}

driver_init_t PCI_is_known(PCI_device_t *dev) {
  /* TODO: more devices */
  if (dev->class_code == 0x1 && dev->subclass == 0x1) {
    return &ATA_init;
  }
  return NULL;
}

void PCI_list() {
  PCI_device_t *dev = PCI_devices;
  while (dev) {
    log_info(NICE_BLUE, "PCI", "(%d:%d:%d) %s", dev->bus, dev->device,
             dev->function, class_names[dev->class_code]);
    dev = dev->next;
  }
  printk("\n");
}

#include <string.h>
uint8_t PCI_detect() {
  PCI_devices = (PCI_device_t *)kmalloc(sizeof(PCI_device_t));
  PCI_device_t *dev = PCI_devices;

  for (size_t bus = 0; bus < 256; bus++) {
    for (size_t slot = 0; slot < 32; slot++) {
      for (size_t function = 0; function < 8; function++) {

        uint16_t vendor = PCI_read_word(bus, slot, function, PCI_VENDOR_OFFSET);
        if (vendor == 0xffff)
          continue;

        dev->next = (PCI_device_t *)kmalloc(sizeof(PCI_device_t));
        dev = dev->next;
        memset(dev, 0, sizeof(PCI_device_t));

        dev->bus = bus;
        dev->device = slot;
        dev->function = function;
        dev->vendor_id = vendor;
        dev->device_id = PCI_read_word(bus, slot, function, PCI_DEV_ID_OFFSET);
        dev->command = PCI_read_word(bus, slot, function, PCI_CMD_OFFSET);
        dev->class_code =
            ((PCI_read_word(bus, slot, function, 0xA) & ~0xFF) >> 8);
        dev->subclass = (PCI_read_word(bus, slot, function, 0xA) & ~0xFF00);
        dev->prog_if = ((PCI_read_word(bus, slot, function, 0x8) & ~0xFF) >> 8);
        dev->header_type = (PCI_read_word(bus, slot, function, 0xE) & ~0xFF00);
        if (dev->header_type == 0x0)
          PCI_parse_header0(dev, bus, slot, function);
        dev->rev_id = PCI_read_word(bus, slot, function, 0x8) & ~0xFF00;
        dev->name = "PCI device";

        driver_init_t init = PCI_is_known(dev);
        if (init) {
          log_info(NICE_BLUE, "PCI", "Initializing %s (0x%x:0x%x:0x%x)",
                   class_names[dev->class_code], dev->bus, dev->device,
                   dev->function);
          init(dev);
        }

      } /* function loop */
    }   /* slot loop */
  }     /* bus loop */
  return 0;
}
