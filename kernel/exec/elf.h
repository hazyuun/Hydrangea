#ifndef _ELF_H_
#define _ELF_H_

#include <stdint.h>

#define ELF_MAGIC0  0x7F
#define ELF_MAGIC1  'E'
#define ELF_MAGIC2  'L'
#define ELF_MAGIC3  'F'

#define ELF_CLASS_INVALID 0
#define ELF_CLASS_32      1
#define ELF_CLASS_64      2

#define ELF_ENDIAN_INVALID  0
#define ELF_ENDIAN_LITTLE   1
#define ELF_ENDIAN_BIG      2

#define ELF_VERSION_INVALID 0
#define ELF_VERSION_CURRENT 1

#define ELF_TYPE_NONE     0
#define ELF_TYPE_REL      1
#define ELF_TYPE_EXEC     2
#define ELF_TYPE_DYN      3
#define ELF_TYPE_CORE     4
#define ELF_TYPE_LOPROC   0xff00
#define ELF_TYPE_HIPROC   0xffff

#define ELF_MACHINE_NONE  0
#define ELF_MACHINE_M32   1 /* AT&T WE 32100 */
#define ELF_MACHINE_SPARC 2 /* SPARC */
#define ELF_MACHINE_386   3 /* Intel 80386 */
#define ELF_MACHINE_68K   4 /* Motorola 68000 */
#define ELF_MACHINE_88K   5 /* Motorola 88000 */
#define ELF_MACHINE_860   7 /* Intel 80860 */
#define ELF_MACHINE_MIPS  8 /* MIPS RS3000 */

#define ELF_SH_IDX_UNDEFINED  0
#define ELF_SH_IDX_LORESERVE  0xff00
#define ELF_SH_IDX_LOPROC     0xff00
#define ELF_SH_IDX_HIPROC     0xff1f
#define ELF_SH_IDX_ABS        0xfff1
#define ELF_SH_IDX_COMMON     0xfff2
#define ELF_SH_IDX_HIRESERVE  0xffff

typedef struct {
  uint8_t magic0;
  uint8_t magic1;
  uint8_t magic2;
  uint8_t magic3;
  uint8_t cl;
  uint8_t endianness;
  uint8_t version;
  uint8_t OS_ABI;
  uint8_t OS_ABI_version;
  uint8_t pad[7];

  uint16_t type;
  uint16_t machine;
  uint32_t elf_version;
  uint32_t entry;
  uint32_t ph_offset;
  uint32_t sh_offset;
  uint32_t flags;
  uint16_t elf_header_size;
  uint16_t ph_table_entry_size;
  uint16_t ph_table_entry_count;
  uint16_t sh_table_entry_size;
  uint16_t sh_table_entry_count;

  uint16_t section_name_table_index;
} elf_header_t;

#define ELF_SH_TYPE_NULL      0
#define ELF_SH_TYPE_PROGBITS  1
#define ELF_SH_TYPE_SYMTAB    2
#define ELF_SH_TYPE_STRTAB    3
#define ELF_SH_TYPE_RELA      4
#define ELF_SH_TYPE_HASH      5
#define ELF_SH_TYPE_DYNAMIC   6
#define ELF_SH_TYPE_NOTE      7
#define ELF_SH_TYPE_NOBITS    8
#define ELF_SH_TYPE_REL       9
#define ELF_SH_TYPE_SHLIB     10
#define ELF_SH_TYPE_DYNSYM    11
#define ELF_SH_TYPE_LOPROC    0x70000000
#define ELF_SH_TYPE_HIPROC    0x7fffffff
#define ELF_SH_TYPE_LOUSER    0x80000000
#define ELF_SH_TYPE_HIUSER    0xffffffff

#define ELF_SH_FLAGS_WRITE      (1 << 0)
#define ELF_SH_FLAGS_ALLOC      (1 << 1)
#define ELF_SH_FLAGS_EXEC       (1 << 2)
#define ELF_SH_FLAGS_PROC_MASK  0xF0000000

typedef struct {
  uint32_t name;
  uint32_t type;
  uint32_t flags;
  uint32_t addr;
  uint32_t offset;
  uint32_t size;
  uint32_t link;
  uint32_t info;
  uint32_t align;
  uint32_t entry_size;
} elf_sh_header_t;

#define ELF_PH_SEGTYPE_NULL     0
#define ELF_PH_SEGTYPE_LOAD     1
#define ELF_PH_SEGTYPE_DYNAMIC  2
#define ELF_PH_SEGTYPE_INTERP   3
#define ELF_PH_SEGTYPE_NOTE     4
#define ELF_PH_SEGTYPE_SHLIB    5
#define ELF_PH_SEGTYPE_PHDR     6
#define ELF_PH_SEGTYPE_LOPROC   0x70000000
#define ELF_PH_SEGTYPE_HIPROC   0x7fffffff

typedef struct {
  uint32_t type;
  uint32_t offset;
  uint32_t vaddr;
  uint32_t paddr;
  uint32_t file_size;
  uint32_t mem_size;
  uint32_t flags;
  uint32_t align;
} elf_ph_header_t;

#include <fs/vfs.h>

/* Loads an ELF executable to memory */
uint32_t elf_load(vfs_node_t *node);

#endif
