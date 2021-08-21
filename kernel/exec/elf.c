#include <exec/elf.h>
#include <fs/vfs.h>
#include <util/logger.h>
#include <stdio.h>
#include <mem/heap.h>
#include <stdbool.h>

static bool elf_verify_magic(elf_header_t *eh){
  bool valid = true;
  valid &= eh->magic0 == ELF_MAGIC0;
  valid &= eh->magic1 == ELF_MAGIC1;
  valid &= eh->magic2 == ELF_MAGIC2;
  valid &= eh->magic3 == ELF_MAGIC3;
  return valid;
}

static bool elf_is_runnable(elf_header_t *eh){
  
  if(eh->cl != ELF_CLASS_32){
    log_info(NICE_GREEN_0, "ELF", "Err: Not a 32bit executable");
    return false;
  }
  if(eh->endianness != ELF_ENDIAN_LITTLE){
    log_info(NICE_GREEN_0, "ELF", "Err: Not little endian");
    return false;
  }
  if(eh->machine != ELF_MACHINE_386){
    log_info(NICE_GREEN_0, "ELF", "Err: Not for i386");
    return false;
  }
  if(eh->elf_version != ELF_VERSION_CURRENT){
    log_info(NICE_GREEN_0, "ELF", "Err: Invalid ELF version");
    return false;
  }
  if(eh->type != ELF_TYPE_EXEC){
    log_info(NICE_GREEN_0, "ELF", "Err: Not an executable");
    return false;
  }
  return true;
}
#include <string.h>
#include <mem/paging.h>
static uint32_t elf_load_segments(uint8_t *buffer){
  elf_header_t *eh = (elf_header_t *) buffer;
  /*
  log_info(NICE_GREEN_0, "ELF", "Program header table offset : %d Bytes", eh->ph_offset);
  log_info(NICE_GREEN_0, "ELF", "Program header size         : %d Bytes", eh->ph_table_entry_size);
  log_info(NICE_GREEN_0, "ELF", "Program header count        : %d Bytes", eh->ph_table_entry_count);
  */
  elf_ph_header_t *ph_tbl = (elf_ph_header_t *)(buffer + eh->ph_offset);
  
  for(uint16_t n = 0; n < eh->ph_table_entry_count; n++){
    if(ph_tbl[n].type == ELF_PH_SEGTYPE_NULL) continue;
    if(ph_tbl[n].type == ELF_PH_SEGTYPE_LOAD){
      log_info(NICE_GREEN_0, "ELF", "Loading segment into vaddr=%d", ph_tbl[n].vaddr);
      
     
      pg_alloc(ph_tbl[n].vaddr, PG_RW | PG_USER);
      pg_invalidate_page(ph_tbl[n].vaddr);
      pg_invalidate_cache();
      *((uint8_t*)ph_tbl[n].vaddr) = 0x5;

      
      memcpy(ph_tbl[n].vaddr, buffer + ph_tbl[n].offset, ph_tbl[n].mem_size);
      if(ph_tbl[n].file_size < ph_tbl[n].mem_size)
        memset(ph_tbl[n].vaddr + ph_tbl[n].file_size, 0, ph_tbl[n].mem_size - ph_tbl[n].file_size);
    }
  }

  return ph_tbl->vaddr + eh->entry;
}

uint32_t elf_load(vfs_node_t *node){
  
  if (!node){
    log_info(NICE_GREEN_0, "ELF", "File not found");
    return 0;
  }
  uint32_t entry = 0x0;

  uint8_t *buffer = (uint8_t *) kmalloc(node->file->size);

  elf_header_t *eh = (elf_header_t *) buffer;

  if (!vfs_read(node->file, 0, node->file->size, buffer)){
    log_info(NICE_GREEN_0, "ELF", "Couldn't read from file : %s", node->name);
    kfree(buffer);
    return 0;
  }

  if(!elf_verify_magic(eh)){
    log_info(NICE_GREEN_0, "ELF", "Invalid ELF file : %s", node->name);
    kfree(buffer);
    return 0;
  }

  if(!elf_is_runnable(eh)){
    kfree(buffer);
    return 0;
  }

  uint32_t entry = elf_load_segments(buffer);
  kfree(buffer);
  return entry;
}