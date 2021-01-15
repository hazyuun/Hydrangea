/*
 *	File : kernel.c
 *	Description : Contains the entry point for the kernel
 *
 * */

#if !defined(__i386__) || defined(__linux__)
#error "ERR : Please use a ix86-elf cross-compiler"
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <kernel.h>

#include <boot/multiboot.h>
#include <tty/tty.h>

#include <drivers/kbd.h>
#include <drivers/pci.h>
#include <drivers/rtc.h>
#include <drivers/serial.h>

#include <cpu/gdt.h>
#include <cpu/idt.h>
#include <cpu/pit.h>

#include <mem/heap.h>
#include <mem/paging.h>
#include <mem/pmm.h>

#include <fs/initrd/initrd.h>

#include <stdio.h>
#include <string.h>

multiboot_info_t *global_mb_header;
uint32_t mb_begin_addr;
uint32_t mb_end_addr;
uint64_t memory_size;
extern uint32_t *ker_page_dir;

#define OK()                                                                   \
  tty_use_color(VGA_GREEN, VGA_BLACK);                                         \
  tty_print(" < OK > ");                                                       \
  tty_use_color(VGA_WHITE, VGA_BLACK);

void panic(char *err_msg) {
  tty_use_color(VGA_RED, VGA_BLACK);
  tty_print(" KERNEL PANIC : ");
  tty_print(err_msg);
  while (1)
    ;
}

void kmain(uint32_t mb_magic, multiboot_info_t *mb_header) {
  tty_init();

  printk("[*] Kernel loaded !\n");

  if (mb_magic != MULTIBOOT_BOOTLOADER_MAGIC)
    panic("Invalid multiboot magic number !");

  if (!(mb_header->flags & (1 << 6)))
    panic("No multiboot memory map !");

  global_mb_header = mb_header;
  mb_begin_addr = (uint32_t)mb_header;
  mb_end_addr = (uint32_t)(mb_header + sizeof(multiboot_info_t));
  memory_size = 0;

  multiboot_memory_map_t *mmap;

  for (mmap = (multiboot_memory_map_t *)global_mb_header->mmap_addr;
       (unsigned long)mmap <
       global_mb_header->mmap_addr + global_mb_header->mmap_length;
       mmap = (multiboot_memory_map_t *)((unsigned long)mmap + mmap->size +
                                         sizeof(mmap->size))) {
    if (mmap->type & MULTIBOOT_MEMORY_AVAILABLE)
      memory_size += (((uint64_t)(mmap->len_hi) << 8) | (mmap->len_lo));
  }

  printk("[*] Available memory : %d KiB\n", (memory_size / 1024));

  gdt_init();
  OK();
  printk("GDT set up \n");

  idt_init();
  OK();
  printk("IDT set up \n");

  serial_init(SERIAL_COM1);
  OK();
  printk("Serial port COM1 initialized \n");

  pmm_init();
  pg_init();
  OK();
  printk("Paging enabled \n");

  pit_init(100);
  OK();
  printk("PIT set up \n");

  vfs_dummy();
  initrd_init((multiboot_module_t *)mb_header->mods_addr);

  printk("Welcome to ");
  tty_use_color(VGA_MAGENTA, VGA_BLACK);
  printk("YuunOS !\n");
  tty_use_color(VGA_WHITE, VGA_BLACK);

  /* This is a quick and dirty and temporary cli */
  /* just for the sake of testing ! */
  /* Edit : it is getting messy lol */

  vfs_node_t *cwd = vfs_get_root();
  char cmd[100];
  while (1) {
    tty_use_color(VGA_LIGHT_BLUE, VGA_BLACK);
    printk("\nKernel ");
    tty_use_color(VGA_CYAN, VGA_BLACK);
    printk("%s", vfs_abs_path_to(cwd));
    tty_use_color(VGA_WHITE, VGA_BLACK);
    printk("> ");

    scank("%s", cmd);
    if (!strcmp("info", cmd)) {
      tty_print("YuunOS v0.0.1\n");
    } else if (!strcmp("clear", cmd)) {
      tty_clear();
    } else if (!strcmp("reboot", cmd)) {
      uint8_t TW = 0x02;
      while (TW & 0x02)
        TW = io_inb(0x64);
      io_outb(0x64, 0xFE);
      while (1)
        ;
    } else if (!strcmp("sleep", cmd)) { /* Will sleep 10 secs */
      pit_sleep(10 * 100);
    } else if (!strcmp("datetime", cmd)) {
      rtc_print_now();
    } else if (!strcmp("lspci", cmd)) {
      PCI_detect();
    } else if (!strcmp(cmd, "ls")) {
      vfs_node_t *node = cwd->childs;
      while (node) {
        printk("%s\n", node->name);
        node = node->next;
      }
    } else if (!strcmp(cmd, "ki")) {
      vfs_show_tree(cwd, 0);
    } else if (strcmp(cmd, "")) {
      char *token = strtok(cmd, " ");
      if (!strcmp(token, "cd")) {
        token = strtok(NULL, " ");
        vfs_node_t *n;
        if (!strcmp(token, ".."))
          cwd = cwd->parent;
        else {
          if ((n = vfs_abspath_to_node(cwd, token))) {
            cwd = n;
          } else {
            printk("Not found");
          }
        }
      } else if (!strcmp("kbd", cmd)) {
        token = strtok(NULL, " ");
        if (!strcmp(token, "fr") || !strcmp(token, "en")) {
          kbd_switch_layout(token);

          printk("Keyboard layout changed to : %s%s", token,
                 !strcmp(token, "fr") ? "\nEnjoy your baguette !\n" : "\n");
        } else
          printk("Invalid keyboard layout\n");

      } else
        printk("Unknown command\n");
    }
  }
  while (1)
    __asm__("hlt\n\t");
}
