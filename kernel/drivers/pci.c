/*
 *      File: pci.c
 *      Description : TODO
 *
 * */

#include <drivers/pci.h>
#include <io/io.h>
#include <stddef.h>
#include <stdio.h>
#include <tty/tty.h>
#include <mem/pmm.h>

const char* class_names[19] = {
     "Unclassified",
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
     "Non-Essential Instrumentation"
};

uint16_t PCI_read_word(int8_t bus, uint8_t slot, uint8_t function, uint8_t offset){
     uint32_t address;
     uint32_t bus_32  = (uint32_t)bus;
     uint32_t slot_32 = (uint32_t)slot;
     uint32_t func_32 = (uint32_t)function;
     
     address = (uint32_t)(
               (bus_32 << 16) |
               (slot_32 << 11)|
               (func_32 << 8) |
               (offset & 0xfc)|
               ((uint32_t)0x80000000));

     io_outl(PCI_CONFIG_ADDR, address);
     return (uint16_t)((io_inl(0xCFC) >> ((offset & 2) * 8)) & 0xffff);
}

void PCI_detect(){
     /* I won't indent here for clean code */
     for (size_t bus = 0; bus < 256; bus++){
	for (size_t slot = 0; slot < 32; slot++){
     for (size_t function = 0; function < 8; function++){
     
          uint16_t vendor = PCI_read_word(bus, slot, function, PCI_VENDOR_OFFSET);
          if (vendor == 0xffff) continue;
          
          PCI_device_t *dev = (PCI_device_t *) pmalloc(sizeof(PCI_device_t));
          
          dev->bus            = bus;
          dev->device         = slot;
          dev->function       = function;
          dev->vendor_id      = vendor;
          dev->device_id      =   PCI_read_word(bus, slot, function, PCI_DEV_ID_OFFSET);
          dev->command        =   PCI_read_word(bus, slot, function, PCI_CMD_OFFSET);
          dev->class_code     = ((PCI_read_word(bus, slot, function, 0xA)& ~0xFF) >> 8);
          dev->subclass       = ( PCI_read_word(bus, slot, function, 0xA)& ~0xFF00);
          dev->prog_if        = ((PCI_read_word(bus, slot, function, 0x8)& ~0xFF) >> 8);
          dev->header_type    = ( PCI_read_word(bus, slot, function, 0xE)& ~0xFF00);
          dev->rev_id         =   PCI_read_word(bus, slot, function, 0x8) & ~0xFF00;
          dev->name           = "PCI device";
          
          printk("[PCI] 0x%x:0x%x:0x%x %s \n",dev->bus, dev->device, dev->function, class_names[dev->class_code]);
          

	} /* function loop */
	} /* slot loop */
	} /* bus loop */
}
