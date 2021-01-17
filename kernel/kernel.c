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

#include <tty/vesa/vesa_term.h>
#include <vesa/vesa.h>

#include <stdio.h>
#include <string.h>

multiboot_info_t *global_mb_header;
uint32_t mb_begin_addr;
uint32_t mb_end_addr;
uint64_t memory_size;
extern uint32_t *ker_page_dir;

#define OK()                                                                   \
  vesa_term_use_color(NICE_GREEN);                                             \
  vesa_term_print(" < OK > ");                                                 \
  vesa_term_use_color(NICE_WHITE);

void panic(char *err_msg) {
  vesa_term_use_color(NICE_RED);
  vesa_term_print(" KERNEL PANIC : ");
  vesa_term_print(err_msg);
  while (1)
    ;
}

void kmain(uint32_t mb_magic, multiboot_info_t *mb_header) {

  vesa_init(mb_header);
  vesa_term_init(vesa_get_framebuffer());

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
  vesa_term_use_color(NICE_MAGENTA);
  printk("YuunOS !\n");
  vesa_term_use_color(NICE_WHITE);

  kbd_switch_layout("en");
  /* This is a quick and dirty and temporary cli */
  /* just for the sake of testing ! */
  /* Edit : it is getting messy lol */

  vfs_node_t *cwd = vfs_get_root();
  char cmd[100];
  while (1) {
    vesa_term_use_color(NICE_CYAN_0);
    printk("\nKernel ");
    vesa_term_use_color(NICE_CYAN);
    printk("%s", vfs_abs_path_to(cwd));
    vesa_term_use_color(NICE_WHITE);
    printk("> ");

    scank("%s", cmd);
    if (!strcmp("info", cmd)) {
      vesa_term_print("YuunOS v0.0.1\n");
    } else if (!strcmp("clear", cmd)) {
      vesa_term_clear();
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
        if (vfs_is_dir(node))
          vesa_term_use_color(NICE_YELLOW);
        else
          vesa_term_use_color(NICE_WHITE);
        printk("%s \t", node->name);
        node = node->next;
      }
      printk("\n");
      vesa_term_use_color(NICE_WHITE);
    } else if (!strcmp(cmd, "ki")) {
      vfs_show_tree(cwd, 0);
    } else if (strcmp(cmd, "")) {
      char *token = strtok(cmd, " ");
      if (!strcmp(token, "cd")) {
        token = strtok(NULL, " ");
        vfs_node_t *n;

        if ((n = vfs_abspath_to_node(cwd, token))) {
          if (vfs_is_dir(n))
            cwd = n;
          else
            printk("Not a directory");
        } else {
          printk("Not found");
        }

      } else if (!strcmp("kbd", cmd)) {
        token = strtok(NULL, " ");
        if (!strcmp(token, "fr") || !strcmp(token, "en")) {
          kbd_switch_layout(token);

          printk("Keyboard layout changed to : %s%s", token,
                 !strcmp(token, "fr") ? "\nEnjoy your baguette !\n" : "\n");
        } else
          printk("Invalid keyboard layout\n");

      } else if (!strcmp("cat", cmd)) {
        token = strtok(NULL, " ");

        vfs_node_t *node = vfs_get_child_by_name(cwd, token);
        if (!node)
          printk("Not found");
        else {
          char *contents = kmalloc(node->file->size);
          if (vfs_read(node->file, 0, node->file->size, contents))
            printk("%s", contents);
          kfree(contents);
        }

      } else
        printk("Unknown command\n");
    }
  }
  while (1)
    __asm__("hlt\n\t");
}
