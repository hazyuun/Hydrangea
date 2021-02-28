#ifndef _MBR_H_
#define _MBR_H_

#include <drivers/ata.h>

#define MBR_PART_TYPE_LINUX 0x83
#define MBR_PART_TYPE_LINUX_SWAP 0x82
#define MBR_PART_TYPE_EXTENDED 0x5

typedef struct {
  uint8_t attr;
  uint8_t CHS_first_sector[3];
  uint8_t type;
  uint8_t CHS_last_sector[3];
  uint32_t LBA_start;
  uint32_t sector_count;
} __attribute__((packed)) partition_t;


/* Fields prefixed with OPT_ are optionnal */
typedef struct {
  uint8_t bootstrap[440];
  uint8_t OPT_disk_id[4];
  uint8_t OPT_reserved[2]; /* Usually 0x0000 */
  
  partition_t partitions[4];
  uint8_t sig[2];
} __attribute__((packed)) mbr_t;

size_t mbr_parse(ATA_drive_t *drv, mbr_t *mbr);
void mbr_print(mbr_t *mbr);

#endif
