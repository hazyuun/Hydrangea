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
#include <drivers/ata.h>

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

#define OK()                                                                   \
  vesa_term_use_color(NICE_GREEN);                                             \
  vesa_term_print("\n < OK > ");                                               \
  vesa_term_use_color(NICE_WHITE);

void panic(char *err_msg) {
  vesa_term_use_color(NICE_RED);
  vesa_term_print(" KERNEL PANIC : ");
  vesa_term_print(err_msg);
  while (1)
    ;
}

void kmain(uint32_t mb_magic, multiboot_info_t *mbi) {
  vesa_init(mbi);
  vesa_term_init(vesa_get_framebuffer());

  printk("[*] Kernel loaded !\n");

  if (mb_magic != MULTIBOOT_BOOTLOADER_MAGIC)
    panic("Invalid multiboot magic number !");

  if (!(mbi->flags & (1 << 6)))
    panic("No multiboot memory map !");

  pmm_init(mbi);
  printk("[*] Available memory : %d KiB\n", pmm_available_memory());

  gdt_init();
  OK();
  printk("GDT set up");

  idt_init();
  OK();
  printk("IDT set up");

  serial_init(SERIAL_COM1);
  OK();
  printk("Serial port COM1 initialized");

  pg_init(mbi);
  OK();
  printk("Paging enabled");

  pit_init(100);
  OK();
  printk("PIT set up");

  vfs_dummy();

  if (mbi->flags & (1 << 3)) {
    uint8_t mods_loaded = initrd_init(mbi);
    OK();
    printk("Loaded %d modules", mods_loaded);
  }

  PCI_detect();
  OK();
  printk("Detecting and initializing PCI devices");

  printk("\n\nWelcome to ");
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
      PCI_list();
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
    } else if (!strcmp(cmd, "lsdrv")) {
      ATA_print_infos();
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
