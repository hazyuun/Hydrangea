#include <misc/mbr.h>

size_t mbr_parse(ATA_drive_t *drv, mbr_t *mbr){
  return ATA_read(drv, 0, 1, (unsigned char*) mbr);
}

#include <stdio.h>
static char *partition_type_str(uint8_t type){
  switch(type){
  case MBR_PART_TYPE_LINUX: return "Linux";
  case MBR_PART_TYPE_LINUX_SWAP: return "Linux swap";
  case MBR_PART_TYPE_EXTENDED: return "Extended";
  default: return "UNKNOWN";
  }
}

void mbr_print(mbr_t *mbr){
  for(uint8_t i = 0; i < 4; i++){
    printk("\n + Partition %d :", i);
    printk("\n +--> Type           : %s", partition_type_str(mbr->partitions[i].type));
    printk("\n +--> Starting block : %d", mbr->partitions[i].LBA_start);
    printk("\n +--> Sector count   : %d", mbr->partitions[i].sector_count);
    printk("\n");
  }
}
