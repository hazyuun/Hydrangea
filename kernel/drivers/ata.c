#include <cpu/pit.h>
#include <drivers/ata.h>
#include <io/io.h>
#include <mem/heap.h>
#include <stdio.h>
#include <string.h>

struct {
  uint16_t base;
  uint16_t ctrl;
  uint16_t bus_master_ide;
  uint8_t IRQ_disable;
} ch_regs[2];

ATA_drive_t ATA_drives[4];

static char *ATA_ERR_in_english_please(int err) {
  switch (err) {
  case ATA_ERR_BAD_BLOCK:
    return "Bad block";
  case ATA_ERR_UNCORRECTABLE:
    return "Uncorrectable data";
  case ATA_ERR_MEDIA_CHANGED:
    return "Media changed";
  case ATA_ERR_ID_MARK_NOT_FOUND:
    return "ID mark not found";
  case ATA_ERR_MEDIA_CHANGE_REQUEST:
    return "Media change request";
  case ATA_ERR_CMD_ABORTED:
    return "Command aborted";
  case ATA_ERR_TRACK0_NOT_FOUND:
    return "Track 0 not found";
  case ATA_ERR_ADDRESS_MARK_NOT_FOUND:
    return "Address mark not found";
  default:
    return "<Unknown error>";
  }
}

static char *ATA_TYPE_in_english_please(int type) {
  switch (type) {
  case ATA_TYPE_PATA:
    return "PATA";
  case ATA_TYPE_PATAPI:
    return "PATAPI";
  case ATA_TYPE_SATA:
    return "SATA";
  case ATA_TYPE_SATAPI:
    return "SATAPI";
  default:
    return "<Unknown type>";
  }
}

void ATA_print_infos() {
  for (uint8_t ps = 0; ps < 2; ps++) {
    for (uint8_t ms = 0; ms < 2; ms++) {
      uint8_t drive_num = 2 * ps + ms;

      uint8_t p = ATA_drives[drive_num].present;
      printk("\n [ATA] %s %s : ", ps ? "SECONDARY" : "PRIMARY",
             ms ? "MASTER" : "SLAVE");
      if (p)
        printk("%s", ATA_drives[drive_num].name);
      else {
        printk("NOT PRESENT\n");
        continue;
      }

      printk("\n       type : %s",
             ATA_TYPE_in_english_please(ATA_drives[drive_num].type));
      printk("\n       sectors : %d", ATA_drives[drive_num].sectors);
      printk("\n");
    }
  }
}

static void ATA_400_nano_sec(uint16_t base) {
  for (int i = 0; i < 4; i++)
    io_inb(base + ATA_REG_ALTSTATUS);
}

static void ATA_print_error(uint16_t base) {
  ATA_400_nano_sec(base);
  uint8_t err = io_inb(base + ATA_REG_ERROR);
  printk("\n [ATA] Error %d : %s", err, ATA_ERR_in_english_please(err));
}

static uint8_t ATA_check_for_errors(uint16_t base) {
  uint8_t status = io_inb(base + ATA_REG_STATUS);
  uint8_t err;

  if (status & ATA_STATUS_ERROR) {
    err = io_inb(base + ATA_REG_ERROR);
    ATA_print_error(err);
    return err;
  }

  if (!(status & ATA_STATUS_DATA_REQUEST_READY))
    return -1;
  return 0;
}

ATA_drive_t *ATA_get_drive(uint8_t ps, uint8_t ms) {
  ATA_drive_t *drv = &ATA_drives[2 * ps + ms];
  if (drv->present)
    return &ATA_drives[2 * ps + ms];
  return NULL;
}

static void ATA_PIO_prepare(ATA_drive_t *drv, uint64_t block, uint64_t size) {
  uint16_t base = ch_regs[drv->ps].base;
  uint8_t status;

  io_outb(base + ATA_REG_HDDEVSEL,
          (drv->ms ? 0xE0 : 0xF0) | ((block >> 24) & 0x0F));

  ATA_400_nano_sec(base);

  do{
    status = io_inb(base + ATA_REG_STATUS);
  } while ((status & ATA_STATUS_BUSY) || (status & ATA_STATUS_DATA_REQUEST_READY));

  io_outb(base + ATA_REG_ERROR, 0x00);
  io_outb(base + ATA_REG_SECCOUNT0, size);
  io_outb(base + ATA_REG_SECCOUNT1, 0);

  io_outb(base + ATA_REG_LBA0, (unsigned char)block);
  io_outb(base + ATA_REG_LBA1, (unsigned char)(block >> 8));
  io_outb(base + ATA_REG_LBA2, (unsigned char)(block >> 16));
  io_outb(base + ATA_REG_LBA3, (unsigned char)0);
  io_outb(base + ATA_REG_LBA4, (unsigned char)0);
  io_outb(base + ATA_REG_LBA5, (unsigned char)0);
}

inline size_t ATA_read(ATA_drive_t *drv, uint64_t block, uint64_t size, unsigned char *buf){
  return ATA_PIO_read(drv, block, size, buf);
}
inline size_t ATA_write(ATA_drive_t *drv, uint64_t block, uint64_t size, unsigned char *buf){
  return ATA_PIO_write(drv, block, size, buf);
}

inline size_t ATA_read_b(ATA_drive_t *drv, uint64_t offset, uint64_t size, unsigned char *buf){
  // printk("\nATA 0");
  // printk("\n offset          :"); vesa_term_print_hex((uint32_t)&offset);
  // printk("\n size            :"); vesa_term_print_hex((uint32_t)&size);
  // printk("\n buf             :"); vesa_term_print_hex((uint32_t)&buf);
  // printk("\n drv             :"); vesa_term_print_hex((uint32_t)&drv);
  uint64_t start = offset / 512;
  uint64_t end = (offset + size) / 512;

  uint64_t N = end - start + 1;

  char whole_sector[N * 512];
  // printk("\n ");
  // printk("\n offset          :"); vesa_term_print_hex((uint32_t)offset);
  // printk("\n size            :"); vesa_term_print_hex((uint32_t)size);
  // printk("\n buf             :"); vesa_term_print_hex((uint32_t)buf);
  // printk("\n drv             :"); vesa_term_print_hex((uint32_t)drv);
  // printk("\n start           :"); vesa_term_print_hex((uint32_t)start);
  // printk("\n N               :"); vesa_term_print_hex((uint32_t)N);
  // printk("\n whole_sector    :"); vesa_term_print_hex((uint32_t)whole_sector);
  ATA_read(drv, start, N, (uint8_t *)whole_sector);
  //printk("\nATA 1");
  memcpy(buf, (char *) (whole_sector + offset % 512), size);
  //printk("\nATA 2");
  return size;
}

size_t ATA_PIO_read(ATA_drive_t *drv, uint64_t block, uint64_t size, unsigned char *buf) {
  uint16_t base = ch_regs[drv->ps].base;

  ATA_PIO_prepare(drv, block, size);

  io_outb(base + ATA_REG_COMMAND, ATA_CMD_READ_PIO);

  uint8_t err = ATA_check_for_errors(base);
  if(err == ATA_ERR_CMD_ABORTED){
    return 0;
  }

  uint16_t word;
  uint8_t status;

  for (uint64_t i = 0; i < 256 * size; i++) {
    io_inb(base + ATA_REG_ALTSTATUS); /* ignored */

    do{
      status = io_inb(base + ATA_REG_STATUS);
    } while ((status & ATA_STATUS_BUSY) || (!(status & ATA_STATUS_DATA_REQUEST_READY)));

    word = io_inw(base);
    buf[i * 2] = (unsigned char)word;
    buf[i * 2 + 1] = (unsigned char)(word >> 8);
  }

  io_inb(base + ATA_REG_ALTSTATUS); /* ignored */
  io_inb(base + ATA_REG_STATUS);

  return size;
}

size_t ATA_PIO_write(ATA_drive_t *drv, uint64_t block, uint64_t size, unsigned char *buf) {
  uint16_t base = ch_regs[drv->ps].base;

  ATA_PIO_prepare(drv, block, size);

  uint8_t status;
  do{
    status = io_inb(base + ATA_REG_STATUS);
  } while ((status & ATA_STATUS_BUSY) || (status & ATA_STATUS_DATA_REQUEST_READY));

  io_outb(base + ATA_REG_COMMAND, ATA_CMD_WRITE_PIO);

  uint8_t err = ATA_check_for_errors(base);
  if(err == ATA_ERR_CMD_ABORTED){
    return 0;
  }

  uint16_t word;
  
  for (uint64_t i = 0; i < 256 * size; i++) {
    io_inb(base + ATA_REG_ALTSTATUS); /* ignored */
    do{
      status = io_inb(base + ATA_REG_STATUS);
    } while ((status & ATA_STATUS_BUSY) || (!(status & ATA_STATUS_DATA_REQUEST_READY)));

    word = (buf[i * 2 + 1] << 8) | buf[i * 2];
    io_outw(base, word);
  }
  
  io_inb(base + ATA_REG_ALTSTATUS); /* ignored */
  io_inb(base + ATA_REG_STATUS);
  return size;
}

#include <stdio.h>
uint8_t ATA_init(PCI_device_t *dev) {

  PCI_header0_t *header = (PCI_header0_t *)dev->header;
  /* Get the base and ctrl registers */
  ch_regs[ATA_PRIMARY].base =
      (header->BAR[0] & 0xFFFFFFFC) + 0x1F0 * (!header->BAR[0]);
  ch_regs[ATA_PRIMARY].ctrl =
      (header->BAR[1] & 0xFFFFFFFC) + 0x3F6 * (!header->BAR[1]);
  ch_regs[ATA_SECONDARY].base =
      (header->BAR[2] & 0xFFFFFFFC) + 0x170 * (!header->BAR[2]);
  ch_regs[ATA_SECONDARY].ctrl =
      (header->BAR[3] & 0xFFFFFFFC) + 0x376 * (!header->BAR[3]);

  ch_regs[ATA_PRIMARY].bus_master_ide = (header->BAR[4] & 0xFFFFFFFC) + 0;
  ch_regs[ATA_SECONDARY].bus_master_ide = (header->BAR[4] & 0xFFFFFFFC) + 8;

  /* Disable IRQs of both PRIMARY and SECONDARY */
  io_outb(ch_regs[ATA_PRIMARY].ctrl + ATA_REG_CONTROL, 0x2);
  io_outb(ch_regs[ATA_SECONDARY].ctrl + ATA_REG_CONTROL, 0x2);

  /* Detect drives */
  for (uint8_t ps = 0; ps < 2; ps++) {
    for (uint8_t ms = 0; ms < 2; ms++) {

      /* Default values */
      uint8_t drive_num = 2 * ps + ms;
      ATA_drives[drive_num].present = 0;
      ATA_drives[drive_num].ps = ps;
      ATA_drives[drive_num].ms = ms;
      ATA_drives[drive_num].type = IDE_ATA;

      /* Get io addresses */
      uint16_t base = ch_regs[ps].base;
      uint16_t ctrl = ch_regs[ps].ctrl;

      /* Soft reset */
      io_outb(ctrl, 0x04);
      ATA_400_nano_sec(base);
      io_outb(ctrl, 0);

      /* Select the current drive */
      while (io_inb(base + ATA_REG_STATUS) & ATA_STATUS_BUSY)
        ;
      io_outb(base + ATA_REG_HDDEVSEL, ms ? 0xA0 : 0xB0);
      ATA_400_nano_sec(base);

      /* Send IDENTIFY command */
      io_outb(base + ATA_REG_SECCOUNT0, 0);
      io_outb(base + ATA_REG_LBA0, 0);
      io_outb(base + ATA_REG_LBA1, 0);
      io_outb(base + ATA_REG_LBA2, 0);

      while (io_inb(base + ATA_REG_STATUS) & ATA_STATUS_BUSY)
        ;
      io_outb(base + ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
      ATA_400_nano_sec(base);

      uint8_t status = io_inb(base + ATA_REG_STATUS);
      if (!status) {
        /* No drive is here, cricket noises .. */
        continue;
      }

      uint8_t check_more = 0;
      uint8_t timeout = 20;
      uint8_t skip = 0;
      for (;;) {
        --timeout;
        uint8_t status = io_inb(base + ATA_REG_STATUS);

        if ((status & ATA_STATUS_ERROR)) {
          check_more = 1; /* ATAPI maybe ? */
          break;
        }

        if (!(status & ATA_STATUS_BUSY) &&
            (status & ATA_STATUS_DATA_REQUEST_READY))
          break;

        pit_sleep(100);

        if (timeout == 10) { /* BSY never clears maybe ? */
          printk("\n [ATA] %s %s :", ps ? "PRIMARY" : "SEONDARY",
                 ms ? "SLAVE" : "MASTER");
          printk("\n       First timeout");
          printk("\n       Trying soft reset ..");

          timeout = 10;
          io_outb(ctrl, 0x04);
          ATA_400_nano_sec(base);
          io_outb(ctrl, 0);
        }

        if (!timeout) {
          printk("\n [ATA] Second timeout");
          printk("\n       Skipping ..");
          skip = 1;
          break;
        }
      }

      if (skip)
        continue;

      if (check_more) {
        uint8_t lo = io_inb(base + ATA_REG_LBA1);
        uint8_t hi = io_inb(base + ATA_REG_LBA2);
        uint16_t type = (hi << 8) | lo;

        switch (type) {
        case ATA_TYPE_PATAPI:
        case ATA_TYPE_SATA:
        case ATA_TYPE_SATAPI:
          printk("\n [ATA] %s device detected at %s %s",
                 ATA_TYPE_in_english_please(type), ps ? "PRIMARY" : "SEONDARY",
                 ms ? "SLAVE" : "MASTER");
          printk("\n       %s devices are not supported (yet ?)",
                 ATA_TYPE_in_english_please(type));
          break;
        default:
          printk("\n [ATA] Unkonwn device detected");
        }

        /* TODO : Wait bsy */
        io_outb(base + ATA_REG_COMMAND, ATA_CMD_IDENTIFY_PACKET);
        ATA_400_nano_sec(base);
        continue;
      }

      /* Buffer that will be holding identification information */
      uint8_t *buf = (uint8_t *)kmalloc(512);
      memset(buf, 0, 512);

      for (size_t i = 0; i < 512; i += 2) {
        uint16_t data = io_inw(base + ATA_REG_DATA);
        buf[i] = data & 0xFF;
        buf[i + 1] = (data >> 8) & 0xFF;
      }

      ATA_drives[drive_num].present = 1;
      ATA_drives[drive_num].sig = *(uint16_t *)(buf + ATA_IDENTIFY_DEVICE_TYPE);
      ATA_drives[drive_num].cap =
          *(uint16_t *)(buf + ATA_IDENTIFY_CAPABILITIES);
      ATA_drives[drive_num].com =
          *(uint16_t *)(buf + ATA_IDENTIFY_COMMAND_SETS);

      if (ATA_drives[drive_num].com & (1 << 26))

        ATA_drives[drive_num].sectors =
            *(uint32_t *)(buf + ATA_IDENTIFY_MAX_LBA_EXT);

      else
        ATA_drives[drive_num].sectors =
            *(uint32_t *)(buf + ATA_IDENTIFY_MAX_LBA);

      for (size_t i = 0; i < 40; i += 2) {
        /* Be careful, endianness */
        ATA_drives[drive_num].name[i] = buf[ATA_IDENTIFY_MODEL + i + 1];
        ATA_drives[drive_num].name[i + 1] = buf[ATA_IDENTIFY_MODEL + i];
      }
      ATA_drives[drive_num].name[40] = '\0';

      kfree(buf);
    }
  }
  return 0;
}
