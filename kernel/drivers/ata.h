#ifndef _ATA_H_
#define _ATA_H_

#include <drivers/pci.h>
#include <stddef.h>

#define IDE_ATA 0x00
#define IDE_ATAPI 0x01

#define ATA_MS 0x00
#define ATA_SL 0x01

#define ATA_PRIMARY 0x00
#define ATA_SECONDARY 0x01

#define ATA_READ 0x00
#define ATA_WRITE 0x01

#define ATA_TYPE_PATA 0x0000
#define ATA_TYPE_SATA 0xC33C
#define ATA_TYPE_PATAPI 0xEB14
#define ATA_TYPE_SATAPI 0x9669

#define ATA_CMD_READ_PIO 0x20
#define ATA_CMD_READ_PIO_EXT 0x24
#define ATA_CMD_READ_DMA 0xC8
#define ATA_CMD_READ_DMA_EXT 0x25
#define ATA_CMD_WRITE_PIO 0x30
#define ATA_CMD_WRITE_PIO_EXT 0x34
#define ATA_CMD_WRITE_DMA 0xCA
#define ATA_CMD_WRITE_DMA_EXT 0x35
#define ATA_CMD_CACHE_FLUSH 0xE7
#define ATA_CMD_CACHE_FLUSH_EXT 0xEA
#define ATA_CMD_PACKET 0xA0
#define ATA_CMD_IDENTIFY_PACKET 0xA1
#define ATA_CMD_IDENTIFY 0xEC

#define ATA_IDENTIFY_DEVICE_TYPE 0
#define ATA_IDENTIFY_CYLINDERS 2
#define ATA_IDENTIFY_HEADS 6
#define ATA_IDENTIFY_SECTORS 12
#define ATA_IDENTIFY_SERIAL 20
#define ATA_IDENTIFY_MODEL 54
#define ATA_IDENTIFY_CAPABILITIES 98
#define ATA_IDENTIFY_FIELD_VALID 106
#define ATA_IDENTIFY_MAX_LBA 120
#define ATA_IDENTIFY_COMMAND_SETS 164
#define ATA_IDENTIFY_MAX_LBA_EXT 200

#define ATA_STATUS_BUSY 0x80
#define ATA_STATUS_DRIVE_READY 0x40
#define ATA_STATUS_DEVICE_FAULT 0x20
#define ATA_STATUS_SEEK_COMPLETE 0x10
#define ATA_STATUS_DATA_REQUEST_READY 0x08
#define ATA_STATUS_CORRECTED_DATA 0x04
#define ATA_STATUS_INDEX 0x02
#define ATA_STATUS_ERROR 0x01

#define ATA_ERR_BAD_BLOCK 0x80
#define ATA_ERR_UNCORRECTABLE 0x40
#define ATA_ERR_MEDIA_CHANGED 0x20
#define ATA_ERR_ID_MARK_NOT_FOUND 0x10
#define ATA_ERR_MEDIA_CHANGE_REQUEST 0x08
#define ATA_ERR_CMD_ABORTED 0x04
#define ATA_ERR_TRACK0_NOT_FOUND 0x02
#define ATA_ERR_ADDRESS_MARK_NOT_FOUND 0x01

#define ATA_REG_DATA 0x00
#define ATA_REG_ERROR 0x01
#define ATA_REG_FEATURES 0x01
#define ATA_REG_SECCOUNT0 0x02
#define ATA_REG_LBA0 0x03
#define ATA_REG_LBA1 0x04
#define ATA_REG_LBA2 0x05
#define ATA_REG_HDDEVSEL 0x06
#define ATA_REG_COMMAND 0x07
#define ATA_REG_STATUS 0x07
#define ATA_REG_SECCOUNT1 0x08
#define ATA_REG_LBA3 0x09
#define ATA_REG_LBA4 0x0A
#define ATA_REG_LBA5 0x0B
#define ATA_REG_CONTROL 0x0C
#define ATA_REG_ALTSTATUS 0x0C
#define ATA_REG_DEVADDRESS 0x0D

#define ATA_SECT_SIZE 512

typedef struct {
  void *mtpts[4];
  uint8_t present;
  uint8_t ps;
  uint8_t ms;
  uint8_t type;
  uint16_t sig;
  uint16_t cap;
  uint32_t com;
  uint32_t sectors;
  uint8_t name[64];
} ATA_drive_t;

uint8_t ATA_init(PCI_device_t *dev);
void ATA_print_infos();
ATA_drive_t *ATA_get_drive(uint8_t ps, uint8_t ms);

/* Wrappers, so I can easily switch to DMA later */
size_t ATA_read(ATA_drive_t *drv, uint64_t block, uint64_t size,
                unsigned char *buf);
size_t ATA_write(ATA_drive_t *drv, uint64_t block, uint64_t size,
                 unsigned char *buf);

size_t ATA_PIO_read(ATA_drive_t *drv, uint64_t block, uint64_t size,
                    unsigned char *buf);
size_t ATA_PIO_write(ATA_drive_t *drv, uint64_t block, uint64_t size,
                     unsigned char *buf);

/* Uses bytes instead of sectors */
size_t ATA_read_b(ATA_drive_t *drv, uint64_t offset, uint64_t size,
                  unsigned char *buf);

size_t ATA_DMA_read_sector(uint8_t *buffer, ATA_drive_t *drv, uint32_t LBA);

#endif